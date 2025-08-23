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

void release_gpio(int pin) {
	char path[50];
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

void cleanup() {
	for (int i = 0; i < 4; i++) {
		release_gpio(PAN_PINS[i]);
		release_gpio(TILT_PINS[i]);
	}

	if (SELECT_PIN != -1) {
		release_gpio(SELECT_PIN);
	}

	exit(EXIT_FAILURE);
}

void export_gpio(int pin) {
	char path[50];
	FILE *file;

	file = fopen("/sys/class/gpio/export", "w");
	if (file) {
		fprintf(file, "%d", pin);
		fclose(file);
	} else {
		printf("Unable export GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		cleanup();
	}

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
	file = fopen(path, "w");
	if (file) {
		fprintf(file, "out");
		fclose(file);
	} else {
		printf("Unable to set direction of GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		cleanup();
	}
}

void unexport_gpio(int pin) {
	FILE *file = fopen("/sys/class/gpio/unexport", "w");
	if (file) {
		fprintf(file, "%d", pin);
		fclose(file);
	} else {
		printf("Unable to unexport GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		cleanup();
	}
}

void set_gpio(int pin, int value) {
	char path[50];
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
	FILE *file = fopen(path, "w");
	if (file) {
		fprintf(file, "%d", value);
		fclose(file);
	} else {
		printf("Unable to set value of GPIO %d: [%d] %s\n", pin, errno, strerror(errno));
		cleanup();
	}
}

void get_gpio_config() {
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

		if (count == 8) {
			for (int i = 0; i < 4; i++) {
				PAN_PINS[i] = value[i];
				TILT_PINS[i] = value[i + 4];
			}
		} else if (count == 5) {
			for (int i = 0; i < 4; i++) {
				PAN_PINS[i] = value[i];
				TILT_PINS[i] = value[i];
			}
			SELECT_PIN = value[4];
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

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <pan steps> <tilt steps> <delay (ms)>\n", argv[0]);
		return 1;
	}

	int pan_steps = atoi(argv[1]);
	int tilt_steps = atoi(argv[2]);
	int delay = atoi(argv[3]) * 1000;

	get_gpio_config();

	for (int i = 0; i < 4; i++) {
		export_gpio(PAN_PINS[i]);
		export_gpio(TILT_PINS[i]);
	}

	if (SELECT_PIN != -1) {
		export_gpio(SELECT_PIN);
	}

	int pan_remaining = abs(pan_steps);
	int tilt_remaining = abs(tilt_steps);
	int pan_reverse = (pan_steps < 0);
	int tilt_reverse = (tilt_steps < 0);
	int pan_micro = 0;
	int tilt_micro = 0;

	while (pan_remaining > 0 || tilt_remaining > 0) {
		int pan_has = pan_remaining > 0;
		int tilt_has = tilt_remaining > 0;

		int pan_eff = pan_has ? (tilt_has ? delay : delay / 2) : 0;
		int tilt_eff = tilt_has ? (pan_has ? delay : delay / 2) : 0;
		int eff_delay = (pan_eff > tilt_eff) ? pan_eff : tilt_eff;

		if (pan_has) {
			if (SELECT_PIN != -1) {
				set_gpio(SELECT_PIN, 0);
			}

			const int (*seq)[4] = pan_reverse ? REVERSE_STEP_SEQUENCE : STEP_SEQUENCE;
			for (int k = 0; k < 4; k++) {
				set_gpio(PAN_PINS[k], seq[pan_micro][k]);
			}
			if (++pan_micro >= 8) {
				pan_micro = 0;
				pan_remaining--;
			}
		}

		if (tilt_has) {
			if (SELECT_PIN != -1) {
				set_gpio(SELECT_PIN, 1);
			}

			const int (*seq)[4] = tilt_reverse ? REVERSE_STEP_SEQUENCE : STEP_SEQUENCE;
			for (int k = 0; k < 4; k++) {
				set_gpio(TILT_PINS[k], seq[tilt_micro][k]);
			}
			if (++tilt_micro >= 8) {
				tilt_micro = 0;
				tilt_remaining--;
			}
		}

		usleep(eff_delay);
	}

	for (int i = 0; i < 4; i++) {
		release_gpio(PAN_PINS[i]);
		release_gpio(TILT_PINS[i]);
	}

	if (SELECT_PIN != -1) {
		release_gpio(SELECT_PIN);
	}

	return 0;
}
