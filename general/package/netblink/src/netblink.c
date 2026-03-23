/*
 *
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 *
 * netblink.c — unified network LED indicator: ping quality or traffic activity
 *
 * Modes:
 *   ping     — periodically ping a host; blink a GPIO LED based on RTT
 *   traffic  — monitor network interface; blink a GPIO LED based on tx+rx rate
 *
 * Usage:
 *   netblink <gpio> ping <host> [interval_sec [timeout_sec]]
 *   netblink <gpio> traffic [interface]
 *
 */

#define _XOPEN_SOURCE 600

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

/* ── GPIO sysfs ───────────────────────────────────────────────── */

#define GPIO_SYSFS_PATH "/sys/class/gpio"
#define MAX_GPIO_PIN    150

/* ── Ping mode constants ──────────────────────────────────────── */

#define DEFAULT_PING_INTERVAL_SEC 60
#define DEFAULT_PING_TIMEOUT_SEC  2
#define MAX_PING_INTERVAL_SEC     86400
#define MAX_PING_TIMEOUT_SEC      30
#define MAX_HOST_LEN              200

#define FAST_THRESHOLD_MS   50.0f
#define MEDIUM_THRESHOLD_MS 150.0f

#define FAST_BLINK_MS   100
#define MEDIUM_BLINK_MS 300
#define SLOW_BLINK_MS   500
#define ERROR_BLINK_MS  200  /* shorter than SLOW to make double-flash distinct */
#define ERROR_GAP_MS    200

/* ── Traffic mode constants ───────────────────────────────────── */

#define TRAFFIC_MIN_DELAY_US 10000   /* 10 ms — heavy traffic */
#define TRAFFIC_MAX_DELAY_US 200000  /* 200 ms — idle */
#define TRAFFIC_SMOOTHING    0.3     /* EMA smoothing factor (0..1) */
#define TRAFFIC_THRESHOLD    100     /* min smoothed bytes/poll to start blinking */
#define TRAFFIC_IDLE_US      50000   /* 50 ms sleep when no traffic */

/* ── Shared state ─────────────────────────────────────────────── */

static volatile sig_atomic_t g_stop          = 0;
static int                   g_gpio_exported = 0;
static int                   g_gpio_value_fd = -1;

/* ── Signal handling ──────────────────────────────────────────── */

static void handle_signal(int sig) {
    (void)sig;
    g_stop = 1;
}

static void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

/* ── sysfs I/O ────────────────────────────────────────────────── */

static int sysfs_write(const char *path, const char *val) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    ssize_t n = write(fd, val, strlen(val));
    close(fd);
    return (n != (ssize_t)strlen(val)) ? -1 : 0;
}

/* ── GPIO lifecycle ───────────────────────────────────────────── */

static int gpio_init(int pin) {
    char path[64], pin_str[12];
    snprintf(pin_str, sizeof(pin_str), "%d", pin);

    if (sysfs_write(GPIO_SYSFS_PATH "/export", pin_str) == 0) {
        g_gpio_exported = 1;
        usleep(100000); /* wait for sysfs to create the gpio directory */
    }
    /* if export failed, gpio may already be exported — proceed anyway */

    snprintf(path, sizeof(path), GPIO_SYSFS_PATH "/gpio%d/direction", pin);
    if (sysfs_write(path, "out") != 0) return -1;

    /* open value file once; reused for the lifetime of the program */
    snprintf(path, sizeof(path), GPIO_SYSFS_PATH "/gpio%d/value", pin);
    g_gpio_value_fd = open(path, O_WRONLY);
    return (g_gpio_value_fd < 0) ? -1 : 0;
}

static void gpio_deinit(int pin) {
    char pin_str[12];
    if (g_gpio_value_fd >= 0) {
        close(g_gpio_value_fd);
        g_gpio_value_fd = -1;
    }
    if (!g_gpio_exported) return; /* only unexport what we exported */
    snprintf(pin_str, sizeof(pin_str), "%d", pin);
    sysfs_write(GPIO_SYSFS_PATH "/unexport", pin_str);
}

static int gpio_set(int pin, int value) {
    const char *val = value ? "1" : "0";
    if (g_gpio_value_fd >= 0) {
        lseek(g_gpio_value_fd, 0, SEEK_SET);
        return (write(g_gpio_value_fd, val, 1) == 1) ? 0 : -1;
    }
    /* fallback: open on each call if fd is not cached */
    char path[64];
    snprintf(path, sizeof(path), GPIO_SYSFS_PATH "/gpio%d/value", pin);
    return sysfs_write(path, val);
}

/* Sleep up to ms milliseconds in 10 ms steps, aborting early on g_stop */
static void interruptible_sleep_ms(int ms) {
    int remaining = ms;
    while (remaining > 0 && !g_stop) {
        int step = (remaining < 10) ? remaining : 10;
        usleep((useconds_t)step * 1000u);
        remaining -= step;
    }
}

/* Turn LED on for duration_ms, then off; aborts early on g_stop */
static void gpio_blink(int pin, int duration_ms) {
    if (duration_ms <= 0) return;
    if (gpio_set(pin, 1) != 0) {
        syslog(LOG_ERR, "gpio_set(%d, 1) failed", pin);
        return;
    }
    interruptible_sleep_ms(duration_ms);
    if (gpio_set(pin, 0) != 0)
        syslog(LOG_ERR, "gpio_set(%d, 0) failed", pin);
}

/* ── Argument parsing helpers ─────────────────────────────────── */

static int parse_long(const char *str, long *out) {
    char *end;
    errno = 0;
    long val = strtol(str, &end, 10);
    if (errno != 0 || end == str || *end != '\0') return -1;
    *out = val;
    return 0;
}

/* Allow only chars safe for shell (letters, digits, '.', '-', ':') */
static int is_safe_host(const char *host) {
    int len = 0;
    const char *p;
    for (p = host; *p; p++, len++) {
        char c = *p;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '.' || c == '-' || c == ':'))
            return 0;
    }
    return len > 0 && len <= MAX_HOST_LEN;
}

/* ── Ping mode ────────────────────────────────────────────────── */

/*
 * Run one ping, return RTT in milliseconds, or -1.0 on failure.
 * LC_ALL=C ensures consistent output regardless of system locale.
 */
static float ping_once(const char *host, int timeout_s) {
    char cmd[384], line[256];
    float rtt_ms = -1.0f;

    snprintf(cmd, sizeof(cmd),
             "LC_ALL=C ping -c 1 -W %d %s 2>/dev/null", timeout_s, host);

    FILE *fp = popen(cmd, "r");
    if (!fp) return -1.0f;

    while (fgets(line, sizeof(line), fp)) {
        char *p = strstr(line, "time=");
        if (p && sscanf(p + 5, "%f", &rtt_ms) == 1) break;
    }
    pclose(fp);
    return rtt_ms;
}

static int run_ping_mode(int gpio_pin, const char *host,
                         int interval_sec, int timeout_sec) {
    if (gpio_init(gpio_pin) != 0) {
        syslog(LOG_ERR, "gpio_init(%d) failed", gpio_pin);
        fprintf(stderr, "gpio_init(%d) failed\n", gpio_pin);
        return 1;
    }

    syslog(LOG_INFO, "ping mode: host=%s gpio=%d interval=%ds timeout=%ds",
           host, gpio_pin, interval_sec, timeout_sec);
    printf("Pinging %s every %d s via GPIO %d, timeout %d s\n",
           host, interval_sec, gpio_pin, timeout_sec);

    while (!g_stop) {
        time_t now = time(NULL);
        printf("[%ld] ping %s... ", (long)now, host);
        fflush(stdout);

        float t = ping_once(host, timeout_sec);
        if (t < 0.0f) {
            printf("fail\n");
            syslog(LOG_WARNING, "ping %s: no reply", host);
            /* double-flash to indicate failure */
            gpio_blink(gpio_pin, ERROR_BLINK_MS);
            interruptible_sleep_ms(ERROR_GAP_MS);
            gpio_blink(gpio_pin, ERROR_BLINK_MS);
        } else {
            printf("%.2f ms\n", t);
            syslog(LOG_INFO, "ping %s: %.2f ms", host, t);
            if (t < FAST_THRESHOLD_MS)
                gpio_blink(gpio_pin, FAST_BLINK_MS);
            else if (t < MEDIUM_THRESHOLD_MS)
                gpio_blink(gpio_pin, MEDIUM_BLINK_MS);
            else
                gpio_blink(gpio_pin, SLOW_BLINK_MS);
        }

        interruptible_sleep_ms(interval_sec * 1000);
    }

    gpio_deinit(gpio_pin);
    syslog(LOG_INFO, "ping mode stopped, GPIO %d unexported", gpio_pin);
    return 0;
}

/* ── Traffic mode ─────────────────────────────────────────────── */

static int run_traffic_mode(int gpio_pin, const char *ifc) {
    char rx_path[128], tx_path[128];
    snprintf(rx_path, sizeof(rx_path),
             "/sys/class/net/%s/statistics/rx_bytes", ifc);
    snprintf(tx_path, sizeof(tx_path),
             "/sys/class/net/%s/statistics/tx_bytes", ifc);

    if (gpio_init(gpio_pin) != 0) {
        syslog(LOG_ERR, "gpio_init(%d) failed", gpio_pin);
        fprintf(stderr, "gpio_init(%d) failed\n", gpio_pin);
        return 1;
    }

    FILE *f_rx = fopen(rx_path, "r");
    FILE *f_tx = fopen(tx_path, "r");
    if (!f_rx || !f_tx) {
        syslog(LOG_ERR, "cannot open stats for interface %s", ifc);
        fprintf(stderr, "Cannot open stats for interface %s"
                " (check interface name or permissions)\n", ifc);
        if (f_rx) fclose(f_rx);
        if (f_tx) fclose(f_tx);
        gpio_deinit(gpio_pin);
        return 1;
    }

    syslog(LOG_INFO, "traffic mode: iface=%s gpio=%d", ifc, gpio_pin);
    printf("Monitoring %s traffic via GPIO %d... Press Ctrl+C to stop.\n",
           ifc, gpio_pin);

    unsigned long last_total = 0;
    double        smoothed   = 0.0;

    while (!g_stop) {
        unsigned long rx = 0, tx = 0;

        /*
         * Rewind instead of reopening: avoids repeated open/close overhead
         * and is safe for sysfs pseudo-files.
         */
        rewind(f_rx);
        rewind(f_tx);
        if (fscanf(f_rx, "%lu", &rx) != 1) rx = 0;
        if (fscanf(f_tx, "%lu", &tx) != 1) tx = 0;

        unsigned long current = rx + tx;

        if (last_total > 0) {
            unsigned long delta = (current >= last_total)
                                  ? (current - last_total) : 0;

            /*
             * Exponential Moving Average smoothing prevents LED jitter
             * during short traffic bursts.
             */
            smoothed = (TRAFFIC_SMOOTHING * (double)delta)
                     + ((1.0 - TRAFFIC_SMOOTHING) * smoothed);

            if (smoothed > TRAFFIC_THRESHOLD) {
                /* Adaptive delay: more traffic → shorter blink interval */
                long delay_us = TRAFFIC_MAX_DELAY_US
                                / ((long)(smoothed / 1024) + 1);
                if (delay_us < TRAFFIC_MIN_DELAY_US)
                    delay_us = TRAFFIC_MIN_DELAY_US;

                gpio_set(gpio_pin, 1);
                usleep((useconds_t)(delay_us / 2));
                gpio_set(gpio_pin, 0);
                usleep((useconds_t)(delay_us / 2));
            } else {
                gpio_set(gpio_pin, 0);
                usleep(TRAFFIC_IDLE_US);
            }
        } else {
            usleep(TRAFFIC_IDLE_US);
        }

        last_total = current;
    }

    fclose(f_rx);
    fclose(f_tx);
    gpio_deinit(gpio_pin);
    syslog(LOG_INFO, "traffic mode stopped, GPIO %d unexported", gpio_pin);
    return 0;
}

/* ── Usage ────────────────────────────────────────────────────── */

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s <gpio> ping <host> [interval_sec [timeout_sec]]\n"
        "  %s <gpio> traffic [interface]\n"
        "\n"
        "Arguments:\n"
        "  gpio         GPIO pin number (1..%d, required)\n"
        "\n"
        "Modes:\n"
        "  ping         Periodically ping <host>; blink LED based on RTT.\n"
        "  traffic      Monitor network interface; blink LED based on tx+rx rate.\n"
        "\n"
        "Ping options:\n"
        "  host         IP or hostname (letters, digits, '.', '-', ':'; max %d chars)\n"
        "  interval_sec Seconds between pings          [default: %d, max: %d]\n"
        "  timeout_sec  Ping timeout in seconds         [default: %d, max: %d]\n"
        "\n"
        "Traffic options:\n"
        "  interface    Network interface name           [default: eth0]\n",
        prog, prog,
        MAX_GPIO_PIN, MAX_HOST_LEN,
        DEFAULT_PING_INTERVAL_SEC, MAX_PING_INTERVAL_SEC,
        DEFAULT_PING_TIMEOUT_SEC,  MAX_PING_TIMEOUT_SEC);
}

/* ── Entry point ──────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    /* Require at least: <prog> <gpio> <mode> */
    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    setup_signals();
    openlog("netblink", LOG_PID | LOG_CONS, LOG_DAEMON);

    /* Parse mandatory GPIO pin (argv[1]) */
    long val;
    if (parse_long(argv[1], &val) != 0 || val <= 0 || val > MAX_GPIO_PIN) {
        fprintf(stderr, "Invalid GPIO pin '%s': must be an integer 1..%d\n",
                argv[1], MAX_GPIO_PIN);
        closelog();
        return 1;
    }
    int gpio_pin = (int)val;

    const char *mode = argv[2];
    int ret;

    if (strcmp(mode, "ping") == 0) {
        /* netblink <gpio> ping <host> [interval_sec [timeout_sec]] */
        if (argc < 4) {
            fprintf(stderr, "ping mode requires <host>\n");
            usage(argv[0]);
            closelog();
            return 1;
        }

        const char *host = argv[3];
        if (!is_safe_host(host)) {
            fprintf(stderr,
                    "Invalid host '%s': only letters, digits, '.', '-', ':' allowed"
                    " (max %d chars)\n", host, MAX_HOST_LEN);
            closelog();
            return 1;
        }

        int interval_sec = DEFAULT_PING_INTERVAL_SEC;
        if (argc >= 5) {
            if (parse_long(argv[4], &val) != 0 || val <= 0
                    || val > MAX_PING_INTERVAL_SEC) {
                fprintf(stderr, "Invalid interval: must be 1..%d s\n",
                        MAX_PING_INTERVAL_SEC);
                closelog();
                return 1;
            }
            interval_sec = (int)val;
        }

        int timeout_sec = DEFAULT_PING_TIMEOUT_SEC;
        if (argc >= 6) {
            if (parse_long(argv[5], &val) != 0 || val <= 0
                    || val > MAX_PING_TIMEOUT_SEC) {
                fprintf(stderr, "Invalid timeout: must be 1..%d s\n",
                        MAX_PING_TIMEOUT_SEC);
                closelog();
                return 1;
            }
            timeout_sec = (int)val;
        }

        ret = run_ping_mode(gpio_pin, host, interval_sec, timeout_sec);

    } else if (strcmp(mode, "traffic") == 0) {
        /* netblink <gpio> traffic [interface] */
        const char *ifc = (argc >= 4) ? argv[3] : "eth0";
        ret = run_traffic_mode(gpio_pin, ifc);

    } else {
        fprintf(stderr, "Unknown mode: '%s'\n", mode);
        usage(argv[0]);
        closelog();
        return 1;
    }

    closelog();
    return ret;
}
