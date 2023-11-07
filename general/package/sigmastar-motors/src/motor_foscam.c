#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

#define DEV_NAME "/dev/gpiochip0"
#define STEP_TIME 250
#define STEP_COUNT 4
#define MAX_COUNT 10
#define SEQ_COUNT 8

static int motor_gpio[][4] = {
	{01, 02, 12, 13},
	{62, 63, 64, 65},
};

static int sequence[][4] = {
	{1, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 0, 0, 1},
	{1, 0, 0, 1},

	{1, 0, 0, 1},
	{0, 0, 0, 1},
	{0, 0, 1, 1},
	{0, 0, 1, 0},
	{0, 1, 1, 0},
	{0, 1, 0, 0},
	{1, 1, 0, 0},
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

static int motor_control(int dir, int seq) {
	for (int i = seq; i < seq + SEQ_COUNT; i++) {
		for (int j = 0; j < 4; j++) {
			if (write_gpio(motor_gpio[dir][j], sequence[i][j])) {
				return 1;
			}
			usleep(STEP_TIME);
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

static int limit_value(int x, int in_min, int in_max) {
	if (x < in_min) {
		x = in_min;
	}

	if (x > in_max) {
		x = in_max;
	}

	return x;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: %s [x_step] [y_step]\n", argv[0]);
		return -1;
	}

	int x = limit_value(atoi(argv[1]), -MAX_COUNT, MAX_COUNT);
	int y = limit_value(atoi(argv[2]), -MAX_COUNT, MAX_COUNT);

	if (motor_check(0, 1) || motor_check(1, 1)) {
		return -1;
	}

	int x_max = (x < 0) ? -x : x;
	int y_max = (y < 0) ? -y : y;

	for (int i = 0; i < x_max * STEP_COUNT * 2; i++) {
		if (motor_control(0, (x < 0) ? SEQ_COUNT : 0)) {
			goto reset;
		}
	}

	for (int i = 0; i < y_max * STEP_COUNT; i++) {
		if (motor_control(1, (y < 0) ? 0 : SEQ_COUNT)) {
			goto reset;
		}
	}

reset:
	motor_check(0, 0);
	motor_check(1, 0);

	return 0;
}
