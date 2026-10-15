#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int PAN_PINS[4];
int TILT_PINS[4];
int SELECT_PIN = -1;

int PAN_FDS[4] = {-1, -1, -1, -1};
int TILT_FDS[4] = {-1, -1, -1, -1};
int SELECT_FD = -1;

/* End switches, from fw env `ptz_limits`: "pan+ pan- tilt+ tilt- [active]"
 * (pan+ = the switch hit by a positive pan move). -1 = none. Inputs are
 * active-low (0 = hit) unless a 5th value of 1 says active-high. */
int LIM_PINS[4] = {-1, -1, -1, -1};
int LIM_FDS[4] = {-1, -1, -1, -1};
int LIM_ACTIVE = 0;

/* CLOCK_MONOTONIC resolution, read once in main(): 1ns on kernels with
 * high-resolution timers, one jiffy (10ms at HZ=100) without them */
long CLOCK_RES_NS = 0;

int STEP_SEQUENCE[8][4] = {
	{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
	{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1}
};

int REVERSE_STEP_SEQUENCE[8][4] = {
	{1, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, 0},
	{0, 1, 1, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}
};

void gpio_release(int pin) {
	char path[64];
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
	FILE *file = fopen(path, "w");
	if (file) {
		fprintf(file, "%d", 0);
		fclose(file);
	}

	file = fopen("/sys/class/gpio/unexport", "w");
	if (file) {
		fprintf(file, "%d", pin);
		fclose(file);
	} else {
		printf("Unable to unexport GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
	}
}

void gpio_clean(int error) {
	for (int i = 0; i < 4; i++) {
		if (PAN_FDS[i] != -1) {
			close(PAN_FDS[i]);
		}
		if (TILT_FDS[i] != -1) {
			close(TILT_FDS[i]);
		}
		gpio_release(PAN_PINS[i]);
		gpio_release(TILT_PINS[i]);
	}

	if (SELECT_FD != -1) {
		close(SELECT_FD);
	}
	if (SELECT_PIN != -1) {
		gpio_release(SELECT_PIN);
	}

	for (int i = 0; i < 4; i++) {
		if (LIM_FDS[i] != -1) {
			close(LIM_FDS[i]);
		}
		/* limit pins are left exported as inputs: harmless, and avoids
		 * re-export churn on every move */
	}

	if (error) {
		exit(EXIT_FAILURE);
	}
}

void gpio_export(int pin) {
	char path[64];
	FILE *file;

	file = fopen("/sys/class/gpio/export", "w");
	if (file) {
		fprintf(file, "%d", pin);
		fclose(file);
	} else {
		printf("Unable export GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		gpio_clean(1);
	}

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
	file = fopen(path, "w");
	if (file) {
		fprintf(file, "out");
		fclose(file);
	} else {
		printf("Unable to set direction of GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		gpio_clean(1);
	}
}

/* the value fds stay open for the whole run: a path lookup plus open/close per
 * write costs ~0.5ms on these SoCs, which at 4 writes per micro-step dwarfs
 * the step delay itself */
int gpio_open(int pin) {
	char path[64];
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
	int fd = open(path, O_WRONLY);
	if (fd == -1) {
		printf("Unable to open value of GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		gpio_clean(1);
	}
	return fd;
}

void gpio_set(int fd, int pin, int value) {
	if (lseek(fd, 0, SEEK_SET) == -1 || write(fd, value ? "1" : "0", 1) != 1) {
		printf("Unable to set value of GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		gpio_clean(1);
	}
}

/* 1 when the pin's sysfs direction reads "in", 0 otherwise (also when the
 * pin is not exported) */
int gpio_is_input(int pin, int *exported) {
	char path[64], dir[8] = "";
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
	FILE *file = fopen(path, "r");
	*exported = file != NULL;
	if (file == NULL) {
		return 0;
	}
	if (fgets(dir, sizeof(dir), file) == NULL) {
		dir[0] = '\0';
	}
	fclose(file);
	return strncmp(dir, "in", 2) == 0;
}

/* Open a limit switch for reading. A pin nobody has exported is exported
 * and made an input. An already-exported pin keeps its direction: if it is
 * an output, something is driving it (a coil of a move in progress, or the
 * IR-cut), and turning it into an input would cut that off, so it is
 * refused instead. Either way the pin is used only once its direction reads
 * back as "in". */
int limit_open(int pin) {
	char path[64];
	int exported;
	if (!gpio_is_input(pin, &exported)) {
		if (exported) {
			printf("ptz_limits: gpio%d is exported as an output - ignoring it\n", pin);
			return -1;
		}
		FILE *file = fopen("/sys/class/gpio/export", "w");
		if (file) {
			fprintf(file, "%d", pin);
			fclose(file);
		}
		snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
		file = fopen(path, "w");
		if (file) {
			fprintf(file, "in");
			fclose(file);
		}
		if (!gpio_is_input(pin, &exported)) {
			printf("ptz_limits: unable to make gpio%d an input - ignoring it\n", pin);
			return -1;
		}
	}
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		printf("Unable to open limit switch GPIO %d: [%d] %s (ignoring it)\n", pin, errno, strerror(errno));
	}
	return fd;
}

/* 1 pressed, 0 free, -1 unreadable; idx = axis*2 + (steps<0) */
int limit_read(int idx) {
	int fd = LIM_FDS[idx];
	if (fd == -1) {
		return -1;
	}
	char c;
	if (lseek(fd, 0, SEEK_SET) == -1 || read(fd, &c, 1) != 1) {
		return -1;
	}
	return (c - '0') == LIM_ACTIVE;
}

/* 1 when the switch for this direction is pressed; a switch that cannot be
 * read counts as not pressed, so a bad input never blocks a move */
int limit_hit(int idx) {
	return limit_read(idx) == 1;
}

int motor_pin(int pin) {
	for (int i = 0; i < 4; i++) {
		if (PAN_PINS[i] == pin || TILT_PINS[i] == pin) {
			return 1;
		}
	}
	return pin == SELECT_PIN;
}

/* check_motor_pins is 0 only for `status`, which runs without ptz_gpio;
 * limit_open() still refuses any pin that is currently an output */
void limits_config(int check_motor_pins) {
	FILE *fp = popen("fw_printenv -n ptz_limits 2>/dev/null", "r");
	if (fp == NULL) {
		return;
	}
	char line[128];
	if (fgets(line, sizeof(line), fp) != NULL) {
		/* Strict: a typo must not quietly pick another GPIO (atoi("x") is
		 * gpio0), so every token is a whole number, pins are -1 or above,
		 * the polarity is 0 or 1, and anything else disables the limits.
		 * Tabs are accepted: the env is typed by hand. */
		int value[5], count = 0, ok = strchr(line, '\n') != NULL || feof(fp);
		for (char *token = strtok(line, " \t\n"); ok && token != NULL; token = strtok(NULL, " \t\n")) {
			char *end;
			errno = 0;
			long v = strtol(token, &end, 10);
			ok = count < 5 && *end == '\0' && errno == 0 && v >= -1 && v <= INT_MAX &&
				(count < 4 || v == 0 || v == 1);
			if (ok) {
				value[count++] = (int)v;
			}
		}
		if (ok && count >= 4) {
			for (int i = 0; i < 4; i++) {
				LIM_PINS[i] = value[i];
			}
			if (count == 5) {
				LIM_ACTIVE = value[4];
			}
		} else {
			printf("ptz_limits: expected \"<pan+> <pan-> <tilt+> <tilt-> [0|1]\" (gpio numbers or -1) - limits disabled\n");
		}
	}
	pclose(fp);

	/* A switch pin that is also a coil would be read back as the coil's own
	 * output and stop or reverse the head at random. One pin shared by both
	 * ends of an axis cannot tell which end is pressed, so the back-off could
	 * drive into the stop it should leave. Refuse both rather than guess. */
	for (int i = 0; i < 4; i++) {
		if (LIM_PINS[i] < 0) {
			continue;
		}
		int bad = check_motor_pins && motor_pin(LIM_PINS[i]);
		for (int j = 0; j < i; j++) {
			bad |= LIM_PINS[j] == LIM_PINS[i];
		}
		if (bad) {
			printf("ptz_limits: gpio%d is a motor pin or listed twice - limits disabled\n", LIM_PINS[i]);
			for (int k = 0; k < 4; k++) {
				LIM_PINS[k] = -1;
			}
			return;
		}
	}
	for (int i = 0; i < 4; i++) {
		if (LIM_PINS[i] >= 0) {
			LIM_FDS[i] = limit_open(LIM_PINS[i]);
		}
	}
}

void gpio_config() {
	/* ptz_gpio is the documented name (majestic-webui#227); gpio_motors is
	 * every camera configured before the rename, so it stays as the
	 * fallback. `grep .` turns an empty first answer into a failure so the
	 * || actually falls through. */
	FILE *fp = popen(
		"fw_printenv -n ptz_gpio 2>/dev/null | grep . || fw_printenv -n gpio_motors", "r");
	if (fp == NULL) {
		printf("Unable to run fw_printenv\n");
		exit(EXIT_FAILURE);
	}

	char line[64];
	if (fgets(line, sizeof(line), fp) != NULL) {
		char *token = strtok(line, " ");
		int value[8];
		int count = 0;

		while (token != NULL && count < 8) {
			value[count++] = atoi(token);
			token = strtok(NULL, " ");
		}

		if (count == 8 || count == 5) {
			for (int i = 0; i < 4; i++) {
				PAN_PINS[i] = value[i];
				TILT_PINS[i] = value[i + (count == 8 ? 4 : 0)];
			}

			if (count == 5) {
				SELECT_PIN = value[4];
			}
		} else {
			printf("Error: Expected 8 or 5 GPIO values, but got %d\n", count);
			exit(EXIT_FAILURE);
		}
	} else {
		printf("Error: Unable to read ptz_gpio or gpio_motors from fw_printenv\n");
		exit(EXIT_FAILURE);
	}

	pclose(fp);
}

/*
 * On kernels built without high-resolution timers every sleep rounds up to a
 * whole tick, so at HZ=100 usleep(1500) waits ~10ms and 8 micro-steps x 10ms
 * put a hard ~80ms floor under every step no matter how small the requested
 * delay is (measured on Hi3518EV200: 200 steps took 33s at delay 15 and still
 * 18s at delay 4). Kernels with hrtimers deliver usleep(1500) in ~1.5ms, and
 * sleeping is strictly better there. clock_getres() tells the two apart.
 *
 * Spinning is only worth it while the delay is a small fraction of a tick,
 * because its cost runs opposite to its benefit: the longer the delay, the
 * longer the core stays pinned and the smaller the rounding error a sleep
 * would have added. A 9ms delay would hold the core ~14s over a 200-step
 * move to shave an 11% error, with every clock_gettime a real syscall on
 * cores that cannot read the arch timer from userspace. So spin on
 * CLOCK_MONOTONIC only below a quarter tick, where sleeping would at least
 * quadruple the step period, and sleep everywhere else.
 */
void delay_us(long us) {
	if (us <= 0) {
		return;
	}

	if (CLOCK_RES_NS <= 1000000 || us >= CLOCK_RES_NS / 4000) {
		usleep(us);
		return;
	}

	struct timespec start, now;
	if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
		usleep(us);
		return;
	}

	/* 64-bit on purpose: a 32-bit long overflows after ~2.1s of elapsed
	 * time, which a preemption in the middle of the spin can reach */
	long long target = (long long)us * 1000;
	for (;;) {
		if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
			usleep(us);
			return;
		}
		long long elapsed = (long long)(now.tv_sec - start.tv_sec) * 1000000000LL + (now.tv_nsec - start.tv_nsec);
		if (elapsed >= target) {
			return;
		}
	}
}

/* Reverse out of a pressed end switch until it releases, then a few steps
 * more for margin, so the head never rests on the switch. Bounded, so a
 * switch that never releases (broken wire) cannot run the head away, and
 * stopped by the switch at the other end, which the back-off travels toward.
 *
 * phase is the STEP_SEQUENCE index last energised by the move, so the
 * reversal continues from the neighbouring half-step instead of jumping
 * coils; -1 when nothing was energised yet (the head started on the switch),
 * which starts from the same pattern a fresh move in that direction would. */
#define BACKOFF_MAX_STEPS 40
#define BACKOFF_MARGIN_STEPS 3

void axis_backoff(const int pins[4], const int fds[4], int level, int steps, int delay, int lim, int phase) {
	const char *axis = level ? "tilt" : "pan";
	int dir = (steps < 0) ? 1 : -1;	/* opposite of the move */
	if (phase < 0) {
		phase = (steps < 0) ? 7 : 0;
	}
	int done = 0, released_at = -1, micro = 0, blocked = 0;

	while (done < BACKOFF_MAX_STEPS) {
		if (released_at < 0 && !limit_hit(lim)) {
			released_at = done;
		}
		if (released_at >= 0 && done - released_at >= BACKOFF_MARGIN_STEPS) {
			break;
		}
		if (limit_hit(lim ^ 1)) {	/* the switch at the end we are backing toward */
			blocked = 1;
			break;
		}
		phase = (phase + dir + 8) % 8;
		for (int i = 0; i < 4; i++) {
			gpio_set(fds[i], pins[i], STEP_SEQUENCE[phase][i]);
		}
		delay_us(delay);
		if (++micro >= 8) {
			micro = 0;
			++done;
		}
	}
	for (int i = 0; i < 4; i++) {
		gpio_set(fds[i], pins[i], 0);
	}
	if (blocked) {
		printf("%s: back-off stopped by the opposite end switch after %d steps - check the switches\n", axis, done);
	} else if (released_at < 0) {
		printf("%s: end switch still pressed after backing off %d steps - check the switch\n", axis, done);
	} else {
		printf("%s: backed off %d steps clear of the switch\n", axis, done);
	}
}

void axis_run(const int pins[4], const int fds[4], int level, int steps, int delay) {
	int remaining = abs(steps);
	if (remaining == 0) {
		return;
	}
	/* select first: the back-off below drives the coils too, and on a
	 * shared-coil head they still belong to the other axis until here */
	if (SELECT_PIN != -1) {
		gpio_set(SELECT_FD, SELECT_PIN, level);
		delay_us(100);
	}

	int lim = level * 2 + (steps < 0 ? 1 : 0);	/* level doubles as the axis index: 0 pan, 1 tilt */
	if (limit_hit(lim)) {
		printf("%s: already at %s end stop\n", level ? "tilt" : "pan", steps < 0 ? "negative" : "positive");
		axis_backoff(pins, fds, level, steps, delay, lim, -1);
		return;
	}

	const int (*seq)[4] = (steps < 0) ? REVERSE_STEP_SEQUENCE : STEP_SEQUENCE;

	int micro = 0, phase = -1;
	while (remaining > 0) {
		if (limit_hit(lim)) {
			printf("%s: end stop reached, %d steps not done\n", level ? "tilt" : "pan", remaining);
			axis_backoff(pins, fds, level, steps, delay, lim, phase);
			break;
		}
		for (int i = 0; i < 4; i++) {
			gpio_set(fds[i], pins[i], seq[micro][i]);
		}
		/* REVERSE_STEP_SEQUENCE[m] is STEP_SEQUENCE[7 - m] */
		phase = (steps < 0) ? 7 - micro : micro;

		delay_us(delay);
		if (++micro >= 8) {
			micro = 0;
			--remaining;
		}
	}

	for (int i = 0; i < 4; i++) {
		gpio_set(fds[i], pins[i], 0);
	}
}

int main(int argc, char *argv[]) {
	if (argc == 2 && strcmp(argv[1], "status") == 0) {
		limits_config(0);
		const char *name[4] = {"pan+", "pan-", "tilt+", "tilt-"};
		for (int i = 0; i < 4; i++) {
			if (LIM_PINS[i] < 0) {
				printf("%-5s none\n", name[i]);
			} else {
				int state = limit_read(i);
				printf("%-5s gpio%-3d %s\n", name[i], LIM_PINS[i],
					state < 0 ? "unreadable (counts as not pressed)" : state ? "PRESSED" : "free");
			}
		}
		return 0;
	}
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <pan steps> <tilt steps> <delay (ms)>\n       %s status\n", argv[0], argv[0]);
		return 1;
	}

	int pan_steps = atoi(argv[1]);
	int tilt_steps = atoi(argv[2]);
	int delay_ms = atoi(argv[3]);
	if (delay_ms < 0 || delay_ms > INT_MAX / 1000) {
		fprintf(stderr, "delay must be between 0 and %d ms\n", INT_MAX / 1000);
		return 1;
	}
	int delay = delay_ms * 1000;

	struct timespec res;
	if (clock_getres(CLOCK_MONOTONIC, &res) == 0) {
		CLOCK_RES_NS = res.tv_sec ? 1000000000L : res.tv_nsec;
	}

	gpio_config();
	limits_config(1);
	for (int i = 0; i < 4; i++) {
		gpio_export(PAN_PINS[i]);
		gpio_export(TILT_PINS[i]);
	}

	if (SELECT_PIN != -1) {
		gpio_export(SELECT_PIN);
	}

	for (int i = 0; i < 4; i++) {
		PAN_FDS[i] = gpio_open(PAN_PINS[i]);
		TILT_FDS[i] = gpio_open(TILT_PINS[i]);
	}

	if (SELECT_PIN != -1) {
		SELECT_FD = gpio_open(SELECT_PIN);
	}

	axis_run(PAN_PINS, PAN_FDS, 0, pan_steps, delay);
	axis_run(TILT_PINS, TILT_FDS, 1, tilt_steps, delay);
	gpio_clean(0);

	return 0;
}
