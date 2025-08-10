#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/gpio.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#define STEP_TIME 1000
#define STEP_COUNT 1
#define SEQ_COUNT 8

typedef struct {
	const char *name;
	int gpio_x[4];
	int gpio_y[4];
} config;

static config list[] = {
	{ "gk7205v200", { 52, 53, 56, 57 }, { 69, 70, 59, 58 }, },
};

const char* dev_name[] = { "/dev/gpiochip0", "/dev/gpiochip1", "/dev/gpiochip2", "/dev/gpiochip3",
			   "/dev/gpiochip4", "/dev/gpiochip5", "/dev/gpiochip6", "/dev/gpiochip7",
			 };

static int sequence[][4] = {
	{ 1, 0, 0, 0 }, { 1, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 1, 0 },
	{ 0, 0, 1, 0 }, { 0, 0, 1, 1 }, { 0, 0, 0, 1 }, { 1, 0, 0, 1 },
	{ 1, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 1, 1 }, { 0, 0, 1, 0 },
	{ 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 1, 1, 0, 0 }, { 1, 0, 0, 0 },
};

static int write_gpio(int pin, int val) {
	struct gpiohandle_request rq;
	struct gpiohandle_data data;
	unsigned char gpiochip = ((pin >> 3) == 0) ? 0 :  (pin >> 3) - 1;
	pin = (int) (pin & 7);
	int fd = open(dev_name[gpiochip], O_RDONLY);
	if (fd < 0) {
		printf("Unable to open chip: %s\n", strerror(errno));
		return 1;
	}

	rq.lineoffsets[0] = pin;
	rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
	rq.lines = 1;

	if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq) < 0) {
		printf("Unable to request gpio %d: [%d] %s\n", pin, errno, strerror(errno));
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

static int motor_control(int *gpio, int count) {
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

static int gpio_export(int *gpio) {
	for (int i = 0; i < 4; i++) {
		if (write_gpio(gpio[i], 0)) {
			return 1;
		}
	}

	return 0;
}

int main(int argc, char **argv) {
	if (argc < 3 || argc > 4) {
		printf("Usage: %s [device] [x_step] [y_step]\n", argv[0]);
		return -1;
	}

	int dev = -1;
	for (int i = 0; i < sizeof(list) / sizeof(config); i++) {
		if (strstr(argv[1], list[i].name)) {
			dev = i;
			break;
		}
	}

	if (dev < 0) {
		printf("Device not supported\n");
		return -1;
	}

	int pid = open("/var/run/motor.pid", O_RDWR | O_CREAT, 0644);
	if (flock(pid, LOCK_EX | LOCK_NB)) {
		printf("Control in progress\n");
		close(pid);
		return -1;
	}

	int x = argv[2] ? atoi(argv[2]) : 0;
	int y = argv[3] ? atoi(argv[3]) : 0;

	if (gpio_export(list[dev].gpio_x) || gpio_export(list[dev].gpio_y)) {
		close(pid);
		return -1;
	}

	int count_x = abs(x) ? STEP_COUNT : 0;
	int count_y = abs(y) ? STEP_COUNT : 0;

	while (count_x || count_y) {
		if (count_x) {
			if (motor_control(list[dev].gpio_x, (x < 0) ? SEQ_COUNT : 0)) {
				goto reset;
			}

			count_x--;
		}

		if (count_y) {
			if (motor_control(list[dev].gpio_y, (y < 0) ? 0 : SEQ_COUNT)) {
				goto reset;
			}

			count_y--;
		}
	}

reset:
	gpio_export(list[dev].gpio_x);
	gpio_export(list[dev].gpio_y);
	close(pid);

	return 0;
}
