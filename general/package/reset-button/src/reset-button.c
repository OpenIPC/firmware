// SPDX-License-Identifier: MIT
/*
 * reset-button - reboot or factory-reset an OpenIPC camera from its button.
 *
 * Hold the button and let go:
 *   under 5 s     nothing
 *   5 to 15 s     reboot            (one beep at 5 s)
 *   15 to 30 s    factory reset     (two beeps at 15 s): firstboot, which
 *                 wipes the overlay and reboots; the U-Boot environment
 *                 (WiFi credentials, MAC) is kept
 *   over 30 s     nothing           a button stuck down (an object resting on
 *                                   it, a mount, water) must not wipe the
 *                                   camera whenever it comes free
 *
 * The pin comes from /usr/share/openipc/gpio.conf (button=N), or -p N. The
 * button pulls it low unless -a 1 (or button_active=1 in gpio.conf) says
 * active-high. Many SoC GPIOs cannot raise an edge interrupt (an SSC325's
 * GPIO 66 refuses "both" in sysfs), so the pin is polled every 100 ms: one
 * pread() on a file kept open, nothing else while the button is up. A button
 * that is already down when the watch starts (held through boot, or over a
 * restart) starts nothing until it has been let go. A pin that is already
 * exported as an output is refused, never turned round: a wrong button= must
 * not switch off whatever that pin drives.
 *
 * The beeps go through majestic's /play_audio when majestic is running; with
 * no speaker or no majestic the button works the same, silently. They are
 * sent from a child, so the pin is read on time while curl waits.
 */
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#define GPIO_CONF "/usr/share/openipc/gpio.conf"
#define POLL_MS 100
#define REBOOT_MS 5000
#define RESET_MS 15000
#define STUCK_MS 30000

static long now_ms(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

static int write_str(const char *path, const char *s)
{
	int fd = open(path, O_WRONLY);
	ssize_t n;

	if (fd < 0)
		return -1;
	n = write(fd, s, strlen(s));
	close(fd);
	return n < 0 ? -1 : 0;
}

/* button=N and button_active=N from gpio.conf; -1 where absent. */
static void read_conf(int *pin, int *active)
{
	char line[128];
	FILE *f = fopen(GPIO_CONF, "r");

	if (!f)
		return;
	while (fgets(line, sizeof(line), f)) {
		if (*pin < 0 && !strncmp(line, "button=", 7))
			*pin = atoi(line + 7);
		else if (*active < 0 && !strncmp(line, "button_active=", 14))
			*active = atoi(line + 14);
	}
	fclose(f);
}

/* The pin's value file, opened for reading; -1 with errno set if it cannot be
 * had. The pins in gpio.conf are not all measured, and a wrong button= can name
 * one a script already drives as an output (WiFi power, IR-cut, an amplifier).
 * So a pin exported before us is only read, and refused if it is an output;
 * only a pin this daemon exports itself is set to "in". */
static int open_pin(int pin)
{
	char path[64], num[16], dir[8] = "";
	struct stat st;
	ssize_t n;
	int fd;

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);
	if (stat(path, &st) == 0) {
		snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
		fd = open(path, O_RDONLY);
		if (fd >= 0) {
			n = read(fd, dir, sizeof(dir) - 1);
			dir[n > 0 ? n : 0] = '\0';
			close(fd);
		}
		if (!strncmp(dir, "out", 3)) {
			fprintf(stderr, "reset-button: gpio %d is already exported as an output, "
				"not taking it as the button\n", pin);
			syslog(LOG_ERR, "gpio %d is already exported as an output, "
			       "not taking it as the button", pin);
			errno = EBUSY;
			return -1;
		}
	} else {
		snprintf(num, sizeof(num), "%d", pin);
		if (write_str("/sys/class/gpio/export", num) != 0)
			return -1;
		snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
		write_str(path, "in");
	}
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
	return open(path, O_RDONLY);
}

static int read_pin(int fd)
{
	char c;

	if (pread(fd, &c, 1, 0) != 1)
		return -1;
	return c == '1';
}

/* A 0.15 s, 1 kHz tone as 8 kHz s16le mono, the rate /play_audio is told. */
static short beep_pcm[1200];

static void make_beep(void)
{
	int i;

	for (i = 0; i < 1200; i++)
		beep_pcm[i] = (short)(9000 * sin(2 * M_PI * 1000 * i / 8000.0));
}

/* Run argv with stdin from in (or /dev/null when in < 0) and stdout and stderr
 * on /dev/null, wait for it, and return its wait status (-1 if it could not be
 * started). The output is dropped rather than piped back: a firstboot whose
 * output fed this daemon would die of SIGPIPE in the middle of its erase if
 * the daemon went first. */
static int run(char *const argv[], int in)
{
	int status;
	pid_t pid = fork();

	if (pid < 0)
		return -1;
	if (pid == 0) {
		int null = open("/dev/null", O_RDWR);

		if (null >= 0) {
			dup2(in >= 0 ? in : null, 0);
			dup2(null, 1);
			dup2(null, 2);
		}
		execvp(argv[0], argv);
		_exit(127);
	}
	if (waitpid(pid, &status, 0) < 0)
		return -1;
	return status;
}

/* Sound the tone `times` times. The tone goes to curl through a pipe, so no
 * file is written. All of it runs in a child the poll loop reaps later: curl
 * may take up to its 2 s timeout, and a release and a new press within that
 * time must not be missed, or two short holds would add up to one long. */
static void beep(int times)
{
	char *const curl[] = {
		"curl", "-s", "-m", "2", "-X", "POST",
		"-H", "Content-Type: application/octet-stream;rate=8000",
		"--data-binary", "@-", "http://127.0.0.1/play_audio", NULL
	};
	int i, p[2];

	if (fork() != 0)
		return;
	for (i = 0; i < times; i++) {
		if (pipe(p) != 0)
			break;
		/* 2400 bytes fit in the pipe: write them, then start curl */
		if (write(p[1], beep_pcm, sizeof(beep_pcm)) == sizeof(beep_pcm)) {
			close(p[1]);
			run(curl, p[0]);
		} else {
			close(p[1]);
		}
		close(p[0]);
		usleep(350000);
	}
	_exit(0);
}

/* Run a command whose job is to end this system, and log it if it did not. */
static void run_final(char *const argv[])
{
	int st = run(argv, -1);

	if (st == -1)
		syslog(LOG_ERR, "%s could not be started", argv[0]);
	else if (WIFEXITED(st))
		syslog(st ? LOG_ERR : LOG_INFO, "%s exited with status %d",
		       argv[0], WEXITSTATUS(st));
	else if (WIFSIGNALED(st))
		syslog(LOG_ERR, "%s was killed by signal %d", argv[0], WTERMSIG(st));
}

static void usage(const char *me)
{
	fprintf(stderr, "Usage: %s [-p pin] [-a 0|1] [-f]\n"
		"  -p  GPIO of the button (default: button= in " GPIO_CONF ")\n"
		"  -a  level of a pressed button, 0 (default) or 1\n"
		"  -f  stay in the foreground\n", me);
}

int main(int argc, char **argv)
{
	int pin = -1, active = -1, foreground = 0, fd, opt;
	int stable = -1, last = -1;
	long down_at = 0;
	int beeped = 0;

	while ((opt = getopt(argc, argv, "p:a:fh")) != -1) {
		switch (opt) {
		case 'p': pin = atoi(optarg); break;
		case 'a': active = atoi(optarg); break;
		case 'f': foreground = 1; break;
		default: usage(argv[0]); return opt == 'h' ? 0 : 1;
		}
	}
	read_conf(&pin, &active);
	if (pin < 0) {
		fprintf(stderr, "reset-button: no button pin (-p, or button= in %s)\n", GPIO_CONF);
		return 1;
	}
	if (active < 0)
		active = 0;

	fd = open_pin(pin);
	if (fd < 0) {
		fprintf(stderr, "reset-button: cannot open gpio %d: %s\n", pin, strerror(errno));
		return 1;
	}
	if (!foreground && daemon(0, 0) != 0)
		return 1;
	signal(SIGCHLD, SIG_DFL);
	openlog("reset-button", LOG_PID, LOG_DAEMON);
	syslog(LOG_INFO, "watching gpio %d (pressed = %d)", pin, active);
	make_beep();

	for (;;) {
		int v = read_pin(fd), pressed;
		long held;

		usleep(POLL_MS * 1000);
		while (waitpid(-1, NULL, WNOHANG) > 0)
			;	/* beeps that are done */
		if (v < 0)
			continue;
		pressed = (v == active);
		/* two equal samples in a row before a change counts: 100 ms debounce */
		if (pressed != last) {
			last = pressed;
			continue;
		}
		if (pressed == stable) {
			if (pressed && down_at) {
				held = now_ms() - down_at;
				if (beeped == 0 && held >= REBOOT_MS) {
					beep(1);
					beeped = 1;
				} else if (beeped == 1 && held >= RESET_MS) {
					beep(2);
					beeped = 2;
				} else if (beeped == 2 && held >= STUCK_MS) {
					syslog(LOG_WARNING, "held over %d s: taken as stuck, "
					       "letting go will do nothing", STUCK_MS / 1000);
					beeped = 3;
				}
			}
			continue;
		}
		if (stable < 0) {
			/* The first settled state. Down already means it went down
			 * before the watch began, at an unknown time: it starts
			 * nothing, and down_at stays 0 until a press of its own. */
			stable = pressed;
			if (pressed)
				syslog(LOG_INFO, "button down at start: ignored until released");
			continue;
		}
		stable = pressed;
		if (pressed) {
			down_at = now_ms();
			beeped = 0;
			continue;
		}
		if (down_at == 0)	/* let go after being down at start */
			continue;
		held = now_ms() - down_at;
		down_at = 0;
		if (held >= STUCK_MS) {
			syslog(LOG_WARNING, "held %ld ms: stuck, no action", held);
		} else if (held >= RESET_MS) {
			char *const reset[] = { "firstboot", NULL };

			syslog(LOG_WARNING, "held %ld ms: factory reset", held);
			run_final(reset);
		} else if (held >= REBOOT_MS) {
			char *const boot[] = { "reboot", NULL };

			syslog(LOG_WARNING, "held %ld ms: reboot", held);
			run_final(boot);
		} else {
			syslog(LOG_INFO, "held %ld ms: no action", held);
		}
	}
}
