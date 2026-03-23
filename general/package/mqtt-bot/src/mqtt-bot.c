/*
 *
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 *
 * mqtt-bot.c — MQTT command executor for embedded Linux
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <syslog.h>
#include <mosquitto.h>

#define DEFAULT_PORT      1883
#define DEFAULT_PORT_TLS  8883
#define DEFAULT_TOPIC     "cmds"
#define DEFAULT_KEEPALIVE 60
#define MAX_STR           256
#define MAX_WHITELIST     64
#define MAX_ARGS          8             /* max argv entries per command (incl. argv[0]) */
#define RECONNECT_DELAY   5
#define SNAP_URL_PORT     80
#define SNAP_URL_PATH     "/image.jpg"
#define SNAP_MAX_BYTES    (512 * 1024)  /* 512 KB max JPEG size                        */
#define CMD_OUTPUT_MAX    8192          /* max captured command output in bytes         */
#define WATCHER_TIMEOUT   3             /* seconds before a watcher child self-kills    */

/* Global run flag — cleared by signal handler */
static volatile sig_atomic_t g_running = 1;

/* ------------------------------------------------------------------ */
/* Logging: mirrors output to stdout/stderr and syslog simultaneously  */
/* ------------------------------------------------------------------ */

static void log_info(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt); vprintf(fmt, ap);                     va_end(ap);
    va_start(ap, fmt); vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
    syslog(LOG_INFO, "%s", buf);
}

static void log_err(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt); vfprintf(stderr, fmt, ap);            va_end(ap);
    va_start(ap, fmt); vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
    syslog(LOG_ERR, "%s", buf);
}

/* ------------------------------------------------------------------ */
/* Whitelist: maps received command name -> executable + arguments     */
/* ------------------------------------------------------------------ */

typedef struct {
    char cmd[MAX_STR];                  /* command name received via MQTT  */
    char exec[MAX_STR];                 /* absolute path to executable     */
    char argv_store[MAX_ARGS][MAX_STR]; /* argv[0]=exec, [1..]=extra args  */
    int  argc;                          /* entries used in argv_store       */
} WhitelistEntry;

typedef struct {
    WhitelistEntry entries[MAX_WHITELIST];
    int            count;
} Whitelist;

/*
 * Parse "cmd:exec[:arg1[:arg2...]]" and append to whitelist.
 * Returns 0 on success.
 */
static int whitelist_add(Whitelist *wl, const char *spec)
{
    if (wl->count >= MAX_WHITELIST) {
        fprintf(stderr, "Whitelist full (max %d entries)\n", MAX_WHITELIST);
        return -1;
    }

    WhitelistEntry *e = &wl->entries[wl->count];
    const char *p = spec;
    const char *sep;

    /* First field: cmd */
    sep = strchr(p, ':');
    if (!sep || sep == p) {
        fprintf(stderr, "Invalid whitelist entry '%s'. Expected cmd:exec[:arg...]\n", spec);
        return -1;
    }
    snprintf(e->cmd, sizeof(e->cmd), "%.*s", (int)(sep - p), p);
    p = sep + 1;

    /* Second field: exec — parse into a local buffer first to avoid
     * aliasing with argv_store[0] in the subsequent memcpy calls      */
    char exec_buf[MAX_STR];
    sep = strchr(p, ':');
    if (sep)
        snprintf(exec_buf, sizeof(exec_buf), "%.*s", (int)(sep - p), p);
    else
        snprintf(exec_buf, sizeof(exec_buf), "%s", p);

    if (exec_buf[0] == '\0') {
        fprintf(stderr, "Invalid whitelist entry '%s': exec path is empty\n", spec);
        return -1;
    }

    memcpy(e->exec,          exec_buf, sizeof(e->exec));
    memcpy(e->argv_store[0], exec_buf, sizeof(e->argv_store[0]));
    e->argc = 1;

    /* Remaining fields: optional extra arguments */
    if (sep) {
        p = sep + 1;
        while (*p && e->argc < MAX_ARGS) {
            sep = strchr(p, ':');
            if (sep) {
                snprintf(e->argv_store[e->argc], sizeof(e->argv_store[e->argc]),
                         "%.*s", (int)(sep - p), p);
                p = sep + 1;
            } else {
                snprintf(e->argv_store[e->argc], sizeof(e->argv_store[e->argc]),
                         "%s", p);
                p += strlen(p);
            }
            e->argc++;
        }
        if (*p)
            fprintf(stderr, "Warning: too many args in '%s', truncated to %d\n",
                    spec, MAX_ARGS);
    }

    wl->count++;
    return 0;
}

/* Find entry for a command. Returns NULL if not whitelisted. */
static const WhitelistEntry *whitelist_lookup(const Whitelist *wl, const char *cmd)
{
    int i;
    for (i = 0; i < wl->count; i++)
        if (strcmp(wl->entries[i].cmd, cmd) == 0)
            return &wl->entries[i];
    return NULL;
}

/* ------------------------------------------------------------------ */
/* Config                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    char      host[MAX_STR];
    int       port;
    int       port_explicit;            /* 1 if user set -p explicitly     */
    char      username[MAX_STR];
    char      password[MAX_STR];
    char      topic[MAX_STR];           /* subscribe topic                 */
    char      response_topic[MAX_STR];  /* topic for output / snap images  */
    int       qos;                      /* subscription QoS: 0, 1, or 2   */
    int       tls_enabled;
    char      tls_cafile[MAX_STR];      /* optional CA file                */
    Whitelist whitelist;
} Config;

static void config_defaults(Config *cfg)
{
    memset(cfg, 0, sizeof(*cfg));
    snprintf(cfg->host,  sizeof(cfg->host),  "%s", "localhost");
    snprintf(cfg->topic, sizeof(cfg->topic), "%s", DEFAULT_TOPIC);
    cfg->port             = DEFAULT_PORT;
    cfg->qos              = 0;
    cfg->response_topic[0] = '\0';      /* derived from topic after arg parsing */
}

/* ------------------------------------------------------------------ */
/* HTTP: minimal synchronous GET for binary content from localhost     */
/* ------------------------------------------------------------------ */

/*
 * Perform an HTTP/1.0 GET on localhost:<port><path>.
 * Allocates *body on success (caller must free).
 * Returns body byte count, or -1 on error.
 */
static int http_get_localhost(int port, const char *path,
                               unsigned char **body, int max_bytes)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    /* Apply send/receive timeout so a stalled server cannot block forever */
    struct timeval tv;
    tv.tv_sec  = WATCHER_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons((unsigned short)port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(sock);
        return -1;
    }

    char req[256];
    snprintf(req, sizeof(req),
             "GET %s HTTP/1.0\r\nHost: localhost\r\n\r\n", path);
    /* Send request, handling partial writes */
    {
        const char *p = req;
        size_t rem = strlen(req);
        while (rem > 0) {
            ssize_t wn = write(sock, p, rem);
            if (wn <= 0) { close(sock); return -1; }
            p   += wn;
            rem -= (size_t)wn;
        }
    }

    unsigned char *buf = malloc((size_t)max_bytes);
    if (!buf) { close(sock); return -1; }

    int total = 0;
    ssize_t n;
    while (total < max_bytes &&
           (n = read(sock, buf + total, (size_t)(max_bytes - total))) > 0)
        total += (int)n;
    close(sock);

    /* Require at least a minimal HTTP response with status 200 */
    if (total < 12 || buf[9] != '2' || buf[10] != '0' || buf[11] != '0') {
        free(buf);
        return -1;
    }

    /* Locate end of HTTP headers (\r\n\r\n) */
    int hdr_end = -1;
    int i;
    for (i = 0; i <= total - 4; i++) {
        if (buf[i]=='\r' && buf[i+1]=='\n' &&
            buf[i+2]=='\r' && buf[i+3]=='\n') {
            hdr_end = i + 4;
            break;
        }
    }

    if (hdr_end < 0) { free(buf); return -1; }

    int body_len = total - hdr_end;
    memmove(buf, buf + hdr_end, (size_t)body_len);
    *body = buf;
    return body_len;
}

/* ------------------------------------------------------------------ */
/* Response publisher — runs inside watcher (child) processes only     */
/* ------------------------------------------------------------------ */

static volatile int g_watcher_publish_done;

static void on_publish_watcher(struct mosquitto *mosq, void *ud, int mid)
{
    (void)mosq; (void)ud; (void)mid;
    g_watcher_publish_done = 1;
}

/*
 * Open a temporary MQTT connection, publish payload to response_topic,
 * wait for delivery confirmation, then disconnect.
 *
 * Called ONLY from child/watcher processes — never from the main process.
 * The parent's mosquitto instance is NOT touched here.
 * mosquitto is compiled against mbedTLS (fork-safe; no OpenSSL state issues).
 */
static void publish_response(const Config *cfg,
                              const void *payload, int payloadlen)
{
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) return;

    mosquitto_username_pw_set(mosq, cfg->username, cfg->password);
    mosquitto_publish_callback_set(mosq, on_publish_watcher);

    if (cfg->tls_enabled) {
        const char *cafile = cfg->tls_cafile[0] ? cfg->tls_cafile : NULL;
        mosquitto_tls_set(mosq, cafile, NULL, NULL, NULL, NULL);
    }

    if (mosquitto_connect(mosq, cfg->host, cfg->port, 30) != MOSQ_ERR_SUCCESS)
        goto cleanup;

    g_watcher_publish_done = 0;
    if (mosquitto_publish(mosq, NULL, cfg->response_topic,
                          payloadlen, payload, cfg->qos, false) != MOSQ_ERR_SUCCESS)
        goto cleanup;

    /* Pump event loop until publish acknowledged or alarm fires */
    {
        int retries = 40; /* secondary safeguard; alarm(WATCHER_TIMEOUT) is the real limit */
        while (!g_watcher_publish_done && retries-- > 0)
            if (mosquitto_loop(mosq, 500, 1) != MOSQ_ERR_SUCCESS) break;
    }

cleanup:
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
}

/* ------------------------------------------------------------------ */
/* Watcher functions — run in child processes, never block main loop   */
/* ------------------------------------------------------------------ */

/*
 * Reset signal handlers to SIG_DFL in watcher child processes.
 * Main process installs custom handlers; fork() inherits them.
 * - SIGCHLD must be SIG_DFL so waitpid() can collect grandchildren
 *   (SIG_IGN + SA_NOCLDWAIT causes waitpid to return ECHILD immediately).
 * - SIGINT/SIGTERM must be SIG_DFL so ctrl-C / kill terminates watchers
 *   rather than just setting g_running which belongs to the parent.
 */
static void watcher_reset_signals(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

/*
 * Watcher for whitelisted commands.
 * Forks the command with stdout/stderr captured via a pipe, accumulates
 * the output, appends an exit-code footer, then publishes to the response
 * topic.  Runs entirely in a child process; parent returns immediately.
 */
static void watcher_run_command(const Config *cfg, const WhitelistEntry *e)
{
    /* Restore inherited signal handlers and enable alarm hard-limit */
    watcher_reset_signals();
    alarm((unsigned int)WATCHER_TIMEOUT);

    int pipefd[2];
    if (pipe(pipefd) != 0) _exit(1);

    pid_t pid = fork();
    if (pid < 0) _exit(1);

    if (pid == 0) {
        /* Grandchild: redirect stdout+stderr to pipe, then exec */
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        char *argv[MAX_ARGS + 1];
        int i;
        for (i = 0; i < e->argc; i++) argv[i] = (char *)e->argv_store[i];
        argv[e->argc] = NULL;
        execv(e->exec, argv);
        _exit(127);
    }

    /* Watcher: read command output until EOF */
    close(pipefd[1]);

    char *output = malloc(CMD_OUTPUT_MAX);
    if (!output) { waitpid(pid, NULL, 0); _exit(1); }

    int total = 0;
    ssize_t n;
    while (total < CMD_OUTPUT_MAX - 1 &&
           (n = read(pipefd[0], output + total,
                     (size_t)(CMD_OUTPUT_MAX - 1 - total))) > 0)
        total += (int)n;

    /* Warn if output was truncated */
    if (total >= CMD_OUTPUT_MAX - 1)
        log_err("[cmd] Output of '%s' truncated at %d bytes\n", e->exec, total);

    close(pipefd[0]);

    int status = 0;
    waitpid(pid, &status, 0);
    int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    /* Append exit-code footer to the captured output */
    total += snprintf(output + total, (size_t)(CMD_OUTPUT_MAX - total),
                      "\n[exit: %d]\n", exit_code);

    log_info("[cmd] %s finished (exit %d), publishing %d bytes\n",
             e->exec, exit_code, total);

    publish_response(cfg, output, total);
    free(output);
    _exit(0);
}

/*
 * Watcher for the built-in "snap" command.
 * Fetches /image.jpg from the local HTTP server on port 80 and publishes
 * the raw JPEG bytes to the response topic.
 */
static void watcher_snap(const Config *cfg)
{
    /* Restore inherited signal handlers and enable alarm hard-limit */
    watcher_reset_signals();
    alarm((unsigned int)WATCHER_TIMEOUT);

    unsigned char *body = NULL;
    int len = http_get_localhost(SNAP_URL_PORT, SNAP_URL_PATH,
                                 &body, SNAP_MAX_BYTES);
    if (len > 0) {
        log_info("[snap] Fetched %d bytes, publishing to %s\n",
                 len, cfg->response_topic);
        publish_response(cfg, body, len);
        free(body);
        _exit(0);
    }
    log_err("[snap] Failed to fetch http://localhost%s\n", SNAP_URL_PATH);
    _exit(1);
}

/* ------------------------------------------------------------------ */
/* MQTT callbacks                                                       */
/* ------------------------------------------------------------------ */

static void on_connect(struct mosquitto *mosq, void *userdata, int rc)
{
    const Config *cfg = (const Config *)userdata;

    if (rc != 0) {
        log_err("[mqtt] Connection refused (code %d)\n", rc);
        return;
    }

    log_info("[mqtt] Connected. Subscribing to '%s' (QoS %d)\n",
             cfg->topic, cfg->qos);

    if (mosquitto_subscribe(mosq, NULL, cfg->topic, cfg->qos) != MOSQ_ERR_SUCCESS)
        log_err("[mqtt] Subscribe failed\n");
}

static void on_disconnect(struct mosquitto *mosq, void *userdata, int rc)
{
    (void)mosq; (void)userdata;
    if (rc != 0)
        log_err("[mqtt] Unexpected disconnect (code %d). Will reconnect...\n", rc);
    else
        log_info("[mqtt] Disconnected.\n");
}

static void on_subscribe(struct mosquitto *mosq, void *userdata,
                         int mid, int qos_count, const int *granted_qos)
{
    (void)mosq; (void)userdata; (void)mid; (void)qos_count; (void)granted_qos;
    log_info("[mqtt] Subscription confirmed. Waiting for commands...\n");
}

static void on_message(struct mosquitto *mosq, void *userdata,
                       const struct mosquitto_message *msg)
{
    (void)mosq;
    const Config *cfg = (const Config *)userdata;

    if (!msg->payload || msg->payloadlen == 0) return;

    /* Safe null-terminated copy of the payload */
    char cmd[MAX_STR];
    size_t len = (size_t)msg->payloadlen < sizeof(cmd) - 1
                 ? (size_t)msg->payloadlen : sizeof(cmd) - 1;
    if ((size_t)msg->payloadlen >= sizeof(cmd))
        log_err("[cmd] Payload truncated from %d to %zu bytes\n",
                msg->payloadlen, sizeof(cmd) - 1);
    memcpy(cmd, msg->payload, len);
    cmd[len] = '\0';

    log_info("[cmd] Received: '%s'\n", cmd);

    /* ---- Built-in: snap ---- */
    if (strcmp(cmd, "snap") == 0) {
        log_info("[cmd] Built-in snap: fetching http://localhost%s\n",
                 SNAP_URL_PATH);
        pid_t pid = fork();
        if (pid < 0) {
            log_err("[cmd] fork failed: %s\n", strerror(errno));
            return;
        }
        if (pid == 0) watcher_snap(cfg);
        log_info("[cmd] Snap watcher PID %d\n", (int)pid);
        return;
    }

    /* ---- Whitelist lookup ---- */
    const WhitelistEntry *e = whitelist_lookup(&cfg->whitelist, cmd);
    if (!e) {
        log_err("[cmd] Rejected: '%s' is not in the whitelist\n", cmd);
        return;
    }

    log_info("[cmd] Executing: %s\n", e->exec);

    pid_t pid = fork();
    if (pid < 0) {
        log_err("[cmd] fork failed: %s\n", strerror(errno));
        return;
    }
    if (pid == 0) watcher_run_command(cfg, e);
    log_info("[cmd] Command watcher PID %d\n", (int)pid);
}

/* ------------------------------------------------------------------ */
/* Signal handling                                                      */
/* ------------------------------------------------------------------ */

static void signal_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

/* ------------------------------------------------------------------ */
/* Usage                                                                */
/* ------------------------------------------------------------------ */

static void usage(const char *prog)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr,
            "  %s -h <host> [-p <port>] -u <user> -P <pass> [-t <topic>]\n",
            prog);
    fprintf(stderr, "         [-r <resp_topic>] [-q <qos>] [-s [-C <cafile>]]\n");
    fprintf(stderr, "         -w <cmd>:<exec>[:<arg1>[:<arg2>...]] [-w ...]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h <host>               MQTT broker hostname or IP\n");
    fprintf(stderr, "  -p <port>               Broker port"
                    " (default: %d, with -s: %d)\n", DEFAULT_PORT, DEFAULT_PORT_TLS);
    fprintf(stderr, "  -u <user>               Username\n");
    fprintf(stderr, "  -P <pass>               Password\n");
    fprintf(stderr, "  -t <topic>              Subscribe topic (default: %s)\n",
            DEFAULT_TOPIC);
    fprintf(stderr, "  -r <resp_topic>         Topic for command output and snap images\n");
    fprintf(stderr, "                          (default: same as -t <topic>)\n");
    fprintf(stderr, "  -q <qos>                Subscription QoS: 0, 1 or 2 (default: 0)\n");
    fprintf(stderr, "  -s                      Enable TLS (uses mbedTLS via mosquitto)\n");
    fprintf(stderr, "  -C <cafile>             CA certificate file (optional)\n");
    fprintf(stderr, "  -w <cmd>:<exec>[:<arg>...]"
                    "  Whitelist entry. May be repeated (max %d)\n\n", MAX_WHITELIST);
    fprintf(stderr, "Built-in commands:\n");
    fprintf(stderr, "  snap    Fetch http://localhost/image.jpg and publish\n");
    fprintf(stderr, "          raw JPEG bytes to <resp_topic>\n\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "  %s -h mqtt.local -u admin -P secret"
                    " -t cam/cmds -r cam/events -q 1 \\\n", prog);
    fprintf(stderr, "         -w reboot:/sbin/reboot \\\n");
    fprintf(stderr, "         -w status:/usr/local/bin/report_status\n");
}

/* ------------------------------------------------------------------ */
/* Main                                                                 */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    /* Static to avoid a large stack allocation (~165 KB with MAX_ARGS=8) */
    static Config cfg;
    config_defaults(&cfg);

    int opt;
    int got_host = 0, got_user = 0, got_pass = 0;

    while ((opt = getopt(argc, argv, "h:p:u:P:t:r:q:w:sC:")) != -1) {
        switch (opt) {
        case 'h':
            snprintf(cfg.host,     sizeof(cfg.host),     "%s", optarg);
            got_host = 1;
            break;
        case 'p': {
            char *end;
            long val = strtol(optarg, &end, 10);
            if (*end != '\0' || val < 1 || val > 65535) {
                fprintf(stderr,
                        "Error: invalid port '%s' (must be 1-65535)\n\n", optarg);
                usage(argv[0]);
                return 1;
            }
            cfg.port = (int)val;
            cfg.port_explicit = 1;
            break;
        }
        case 'u':
            snprintf(cfg.username, sizeof(cfg.username), "%s", optarg);
            got_user = 1;
            break;
        case 'P':
            snprintf(cfg.password, sizeof(cfg.password), "%s", optarg);
            got_pass = 1;
            break;
        case 't':
            snprintf(cfg.topic,          sizeof(cfg.topic),          "%s", optarg);
            break;
        case 'r':
            snprintf(cfg.response_topic, sizeof(cfg.response_topic), "%s", optarg);
            break;
        case 'q': {
            char *end;
            long val = strtol(optarg, &end, 10);
            if (*end != '\0' || val < 0 || val > 2) {
                fprintf(stderr,
                        "Error: invalid QoS '%s' (must be 0, 1, or 2)\n\n", optarg);
                usage(argv[0]);
                return 1;
            }
            cfg.qos = (int)val;
            break;
        }
        case 's':
            cfg.tls_enabled = 1;
            break;
        case 'C':
            snprintf(cfg.tls_cafile, sizeof(cfg.tls_cafile), "%s", optarg);
            break;
        case 'w':
            if (whitelist_add(&cfg.whitelist, optarg) != 0)
                return 1;
            break;
        default:
            usage(argv[0]);
            return 1;
        }
    }

    if (!got_host || !got_user || !got_pass || cfg.whitelist.count == 0) {
        if (got_host && got_user && got_pass)
            fprintf(stderr,
                    "Error: at least one -w <cmd>:<exec> entry is required\n\n");
        usage(argv[0]);
        return 1;
    }

    /* Apply TLS default port if -s given but -p was not */
    if (cfg.tls_enabled && !cfg.port_explicit)
        cfg.port = DEFAULT_PORT_TLS;

    /* If no -r given, responses go to the same topic as commands */
    if (cfg.response_topic[0] == '\0')
        snprintf(cfg.response_topic, sizeof(cfg.response_topic),
                 "%s", cfg.topic);

    openlog("mqtt-bot", LOG_PID | LOG_CONS, LOG_DAEMON);

    /* Print active whitelist */
    log_info("[cfg] Whitelist (%d entries):\n", cfg.whitelist.count);
    {
        int i;
        for (i = 0; i < cfg.whitelist.count; i++) {
            const WhitelistEntry *e = &cfg.whitelist.entries[i];
            char disp[512];
            int pos = snprintf(disp, sizeof(disp), "%s", e->exec);
            int j;
            for (j = 1; j < e->argc && pos < (int)sizeof(disp) - 1; j++)
                pos += snprintf(disp + pos, sizeof(disp) - (size_t)pos,
                                " %s", e->argv_store[j]);
            log_info("[cfg]   '%s' -> %s\n", e->cmd, disp);
        }
    }
    log_info("[cfg] Response topic : %s\n", cfg.response_topic);

    /* SIGCHLD: auto-reap watcher children without blocking */
    {
        struct sigaction sa_chld;
        memset(&sa_chld, 0, sizeof(sa_chld));
        sa_chld.sa_handler = SIG_IGN;
        sa_chld.sa_flags   = SA_NOCLDWAIT;
        sigaction(SIGCHLD, &sa_chld, NULL);
    }

    /* SIGINT / SIGTERM: no SA_RESTART so mosquitto_loop_forever() is interruptible */
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGINT,  &sa, NULL) != 0 ||
            sigaction(SIGTERM, &sa, NULL) != 0) {
            perror("sigaction");
            closelog();
            return 1;
        }
    }

    mosquitto_lib_init();

    struct mosquitto *mosq = mosquitto_new(NULL, true, &cfg);
    if (!mosq) {
        log_err("Failed to create mosquitto instance\n");
        mosquitto_lib_cleanup();
        closelog();
        return 1;
    }

    mosquitto_username_pw_set(mosq, cfg.username, cfg.password);
    mosquitto_connect_callback_set(mosq,    on_connect);
    mosquitto_disconnect_callback_set(mosq, on_disconnect);
    mosquitto_subscribe_callback_set(mosq,  on_subscribe);
    mosquitto_message_callback_set(mosq,    on_message);

    if (cfg.tls_enabled) {
        const char *cafile = cfg.tls_cafile[0] ? cfg.tls_cafile : NULL;
        int rc_tls = mosquitto_tls_set(mosq, cafile, NULL, NULL, NULL, NULL);
        if (rc_tls != MOSQ_ERR_SUCCESS) {
            log_err("[tls] Failed to configure TLS: %s\n",
                    mosquitto_strerror(rc_tls));
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();
            closelog();
            return 1;
        }
        log_info("[tls] TLS enabled%s\n", cafile ? "" : " (system CA store)");
    }

    mosquitto_reconnect_delay_set(mosq, RECONNECT_DELAY, RECONNECT_DELAY * 2, false);

    log_info("[mqtt] Connecting to %s:%d as '%s'...\n",
             cfg.host, cfg.port, cfg.username);

    int rc = mosquitto_connect(mosq, cfg.host, cfg.port, DEFAULT_KEEPALIVE);
    if (rc != MOSQ_ERR_SUCCESS) {
        log_err("[mqtt] Connect error: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        closelog();
        return 1;
    }

    /*
     * Main event loop. mosquitto_loop_forever() handles reconnection internally.
     * When SIGINT/SIGTERM fires, the blocking select() inside returns EINTR
     * (SA_RESTART is not set), causing loop_forever to return MOSQ_ERR_ERRNO.
     * A signal-driven exit is distinguished from a fatal error via g_running.
     */
    rc = mosquitto_loop_forever(mosq, 1000, 1);
    if (rc != MOSQ_ERR_SUCCESS && g_running)
        log_err("[mqtt] Fatal loop error: %s\n", mosquitto_strerror(rc));

    log_info("[mqtt] Shutting down.\n");
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    closelog();

    return 0;
}
