#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int PAN_PINS[4];
int TILT_PINS[4];
int SELECT_PIN = -1;

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
		gpio_release(PAN_PINS[i]);
		gpio_release(TILT_PINS[i]);
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

void gpio_set(int pin, int value) {
	char path[64];
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
	FILE *file = fopen(path, "w");
	if (file) {
		fprintf(file, "%d", value);
		fclose(file);
	} else {
		printf("Unable to set value of GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		gpio_clean(1);
	}
}

void gpio_config() {
	FILE *fp = popen("fw_printenv -n gpio_motors", "r");
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
		printf("Error: Unable to read gpio_motors from fw_printenv\n");
		exit(EXIT_FAILURE);
	}

	pclose(fp);
}

void axis_run(const int pins[4], int level, int steps, int delay) {
	int remaining = abs(steps);
	if (remaining == 0) {
		return;
	}

	const int (*seq)[4] = (steps < 0) ? REVERSE_STEP_SEQUENCE : STEP_SEQUENCE;
	if (SELECT_PIN != -1) {
		gpio_set(SELECT_PIN, level);
		usleep(100);
	}

	int micro = 0;
	while (remaining > 0) {
		for (int i = 0; i < 4; i++) {
			gpio_set(pins[i], seq[micro][i]);
		}

		usleep(delay);
		if (++micro >= 8) {
			micro = 0;
			--remaining;
		}
	}

	for (int i = 0; i < 4; i++) {
		gpio_set(pins[i], 0);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <pan steps> <tilt steps> <delay (ms)>\n", argv[0]);
		return 1;
	}

	int pan_steps = atoi(argv[1]);
	int tilt_steps = atoi(argv[2]);
	int delay = atoi(argv[3]) * 1000;

	gpio_config();
	for (int i = 0; i < 4; i++) {
		gpio_export(PAN_PINS[i]);
		gpio_export(TILT_PINS[i]);
	}

	if (SELECT_PIN != -1) {
		gpio_export(SELECT_PIN);
	}

	axis_run(PAN_PINS, 0, pan_steps, delay);
	axis_run(TILT_PINS, 1, tilt_steps, delay);
	gpio_clean(0);

	return 0;
}
