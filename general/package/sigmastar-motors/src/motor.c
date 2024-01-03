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
#define MAX_COUNT 8
#define SEQ_COUNT 8

int device_x5[] = {01, 02, 12, 13, 62, 63, 64, 65};

int gpio_x[4];
int gpio_y[4];

int sequence[][4] = {
	{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
	{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1},
	{1, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, 0},
	{0, 1, 1, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0},
};

int write_gpio(int pin, int val) {
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

int motor_control(int *gpio, int count) {
	for (int i = count; i < count + SEQ_COUNT; i++) {
		for (int j = 0; j < 4; j++) {
			if (write_gpio(gpio[j], sequence[i][j])) {
				return 1;
			}
			usleep(STEP_TIME);
		}
	}

	return 0;
}

int gpio_export(int *gpio) {
	for (int i = 0; i < 4; i++) {
		if (write_gpio(gpio[i], 0)) {
			return 1;
		}
	}

	return 0;
}

int limit_value(int x) {
	if (x < -MAX_COUNT) {
		x = -MAX_COUNT;
	}

	if (x > MAX_COUNT) {
		x = MAX_COUNT;
	}

	return x;
}

int main(int argc, char **argv) {
	if (argc < 2 || argc > 3) {
		printf("Usage: %s [x_step] [y_step]\n", argv[0]);
		return -1;
	}

	int x = limit_value(argv[1] ? atoi(argv[1]) : 0);
	int y = limit_value(argv[2] ? atoi(argv[2]) : 0);

	memcpy(gpio_x, device_x5 + 0, sizeof(gpio_x));
	memcpy(gpio_y, device_x5 + 4, sizeof(gpio_y));

	if (gpio_export(gpio_x) || gpio_export(gpio_y)) {
		return -1;
	}

	for (int i = 0; i < abs(x) * STEP_COUNT * 2; i++) {
		if (motor_control(gpio_x, (x < 0) ? SEQ_COUNT : 0)) {
			goto reset;
		}
	}

	for (int i = 0; i < abs(y) * STEP_COUNT; i++) {
		if (motor_control(gpio_y, (y < 0) ? 0 : SEQ_COUNT)) {
			goto reset;
		}
	}

reset:
	gpio_export(gpio_x);
	gpio_export(gpio_y);

	return 0;
}
