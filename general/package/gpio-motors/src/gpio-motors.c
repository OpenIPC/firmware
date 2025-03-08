#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char *name;
    int pan[4];
    int tilt[4];
} config;

static config list[] = {
    { "gk7205v200",    { 52, 53, 56, 57 }, { 69, 70, 59, 58 } },
    { "gm8136-faleemi",{ 51, 52, 53, 54 }, { 55, 28, 29, 30 } },
    { "hi3516cv200-fdt",{ 60, 61, 37, 38 }, { 54, 55, 56, 57 } },
    { "ssc337de-foscam",{ 1, 2, 12, 13  }, { 62, 63, 64, 65 } },
};

int PAN_PINS[4];
int TILT_PINS[4];

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
    } else {
        printf("Unable to set value of GPIO %d to 0: [%d] %s\n", pin, errno, strerror(errno));	
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

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <config> <pan steps> <tilt steps> <delay>\n", argv[0]);
        return 1;
    }

	int pan_steps = atoi(argv[2]);
    int tilt_steps = atoi(argv[3]);
	int delay = atoi(argv[4]);

    config *selected_config = NULL;
    int config_count = sizeof(list) / sizeof(list[0]);
    for (int i = 0; i < config_count; i++) {
        if (strcmp(list[i].name, argv[1]) == 0) {
            selected_config = &list[i];
            break;
        }
    }
    if (selected_config == NULL) {
        fprintf(stderr, "Error: config '%s' not found.\n", argv[1]);
		return 1;
    }

    for (int i = 0; i < 4; i++) {
        PAN_PINS[i] = selected_config->pan[i];
        TILT_PINS[i] = selected_config->tilt[i];
    }

    for (int i = 0; i < 4; i++) {
        export_gpio(PAN_PINS[i]);
        export_gpio(TILT_PINS[i]);
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
}
