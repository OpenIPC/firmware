#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

#define DEV_NAME "/dev/gpiochip0"
#define STEP_TIME 2 * 1000

static int motor_gpio[][4] = {
	{01, 02, 12, 13},
	{62, 63, 64, 65},
};

static int sequence[][4] = {
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1},
	{0, 0, 0, 1},
	{0, 0, 1, 0},
	{0, 1, 0, 0},
	{1, 0, 0, 0},
};

static int write_gpio(int pin, int val) {
	struct gpiohandle_request rq;
	struct gpiohandle_data data;

	int fd = open(DEV_NAME, O_RDONLY);
	if (fd < 0) {
		printf("Unable to open chip: %s\n", strerror(errno));
		return 1;
	}

	rq.lineoffsets[0] = pin;
	rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
	rq.lines = 1;

	if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq) < 0) {
		printf("Unable to request gpio %d: %s\n", pin, strerror(errno));
		return 1;
	}

	close(fd);
	data.values[0] = val;

	if (ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
		printf("Unable to set value %d: %s\n", val, strerror(errno));
		return 1;
	}

	close(rq.fd);

	return 0;
}

static int motor_control(int dir, int step) {
	usleep(STEP_TIME);
	for (int i = 0; i < 4; i++) {
		if (write_gpio(motor_gpio[dir][i], sequence[step][i])) {
			return 1;
		}
	}

	return 0;
}

static int motor_check(int dir, int check) {
	for (int i = 0; i < 4; i++) {
		if (write_gpio(motor_gpio[dir][i], 0) && check) {
			return 1;
		}
	}

	return 0;
}

static void usage(const char *name) {
	printf("Usage: %s [options]\n"
		"Options:\n"
		"\t-x [count]\tHorizontal direction step (-10 | +10)\n"
		"\t-y [count]\tVertical direction step (-10 | +10)\n", name);
}

int main(int argc, char **argv) {
	int cnt = 0;
	int dir = 0;
	int opt = 0;

	while ((opt = getopt(argc, argv, "hx:y:")) != -1) {
		switch (opt) {
			case 'h':
				usage(argv[0]);
				return -1;

			case 'x':
				cnt = atoi(optarg);
				break;

			case 'y':
				dir = 1;
				cnt = atoi(optarg);
				break;

			default:
				return -1;
		}
	}

	if (argc == 1 || argc != optind) {
		usage(argv[0]);
		return -1;
	}

	if (cnt > 10) {
		cnt = 10;
	}

	if (cnt < -10) {
		cnt = -10;
	}

	if (motor_check(dir, 1)) {
		return -1;
	}

	int c1 = (cnt < 0) ? cnt * 10 : 0;
	int c2 = (cnt < 0) ? 0 : cnt * 10;
	int d1 = (cnt < 0) ? 4 : 0;
	int d2 = (cnt < 0) ? 8 : 4;

	for (int i = c1; i < c2; i++) {
		for (int j = d1; j < d2; j++) {
			if (motor_control(dir, j)) {
				goto reset;
			}
		}
	}

reset:
	motor_check(dir, 0);

	return 0;
}
