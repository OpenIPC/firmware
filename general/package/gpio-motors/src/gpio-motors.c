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
 * sleeping is strictly better there. clock_getres() tells the two apart, so
 * sleep whenever the kernel can honour the delay - or when the delay is at
 * least a tick, where rounding no longer dominates - and spin on
 * CLOCK_MONOTONIC only for sub-tick delays on a coarse-timer kernel. Moves
 * are short and bounded, so burning the CPU for their duration is a fair
 * trade in that remaining case.
 */
void delay_us(long us) {
	if (us <= 0) {
		return;
	}

	if (CLOCK_RES_NS <= 1000000 || us >= CLOCK_RES_NS / 1000) {
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
		clock_gettime(CLOCK_MONOTONIC, &now);
		long long elapsed = (long long)(now.tv_sec - start.tv_sec) * 1000000000LL + (now.tv_nsec - start.tv_nsec);
		if (elapsed >= target) {
			return;
		}
	}
}

void axis_run(const int pins[4], const int fds[4], int level, int steps, int delay) {
	int remaining = abs(steps);
	if (remaining == 0) {
		return;
	}

	const int (*seq)[4] = (steps < 0) ? REVERSE_STEP_SEQUENCE : STEP_SEQUENCE;
	if (SELECT_PIN != -1) {
		gpio_set(SELECT_FD, SELECT_PIN, level);
		delay_us(100);
	}

	int micro = 0;
	while (remaining > 0) {
		for (int i = 0; i < 4; i++) {
			gpio_set(fds[i], pins[i], seq[micro][i]);
		}

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
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <pan steps> <tilt steps> <delay (ms)>\n", argv[0]);
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
