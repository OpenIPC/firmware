#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include <errno.h>

#define CHIP_NAME "/dev/gpiochip0"
#define LINE_IN "stepper-in"
#define LINE_EN "stepper-en"
#define STEP_DELAY_US 2000

enum motor_index {
    MOTOR_PAN = 0,
    MOTOR_TILT,
    MOTOR_ALL
};

struct stepper_motor {
    struct gpiod_line *in[4];
    struct gpiod_line *enable;
    int epolar;
};

static const int step_seq[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
};

static struct stepper_motor motors[MOTOR_ALL];

static void stepper_output(enum motor_index index, int seq)
{
    int i;
		struct stepper_motor *motor = &motors[index];

    for (i = 0; i < 4; i++) {
        gpiod_line_set_value(motor->in[i], step_seq[seq][i]);
    }
}

static void stepper_rotate(enum motor_index index, int steps)
{
    int dir;
    int seq = 0;
    int i;
		struct stepper_motor *stepper;

    if (steps == 0)
        return;

		stepper = &motors[index];

    dir = (steps > 0) ? 1 : -1;
    steps = abs(steps);

    gpiod_line_set_value(stepper->enable, 1);

    for (i = 0; i < steps; i++) {
        seq += dir;

        if (seq >= 8)
            seq = 0;
        else if (seq < 0)
            seq = 7;

        stepper_output(index, seq);

        usleep(STEP_DELAY_US);
    }

   stepper_output(index, seq);;

    gpiod_line_set_value(stepper->enable, 0);
}

static void usage(const char *prog)
{
    printf("Usage:\n"
            "  %s [options]\n\n"
            "Options:\n"
            "  -p, --pan <steps>     pan stepper steps\n"
            "  -t, --tilt <steps>    tilt stepper steps\n"
            "  -h, --help            show this help\n\n"
            "Example:\n"
            "  %s -p 100 -t -50\n",
            prog, prog);
}

int main(int argc, char *argv[])
{
    struct gpiod_chip *chip;
    int pan_steps = 0;
    int tilt_steps = 0;
    int ret = 0;
    int opt;
    int i, j;
    static const struct option long_options[] = {
        {"pan",  required_argument, 0, 'p'},
        {"tilt", required_argument, 0, 't'},
        {"help", no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

		const char *app = basename(argv[0]);

    while ((opt = getopt_long(argc, argv, "p:t:h", long_options, NULL)) != -1) {
        switch (opt) {
        case 'p':
            pan_steps = atoi(optarg);
            break;

        case 't':
            tilt_steps = atoi(optarg);
            break;

        case 'h':
        default:
            usage(app);
            return 0;
        }
    }

    chip = gpiod_chip_open(CHIP_NAME);
    if (!chip) {
        perror(CHIP_NAME);
        return -ENODEV;
    }

    for (i = 0; i < MOTOR_ALL; i++) {
        char name[32];

				// init IN lines
				for (j = 0; j < 4; j++) {
						struct gpiod_line *line;

						snprintf(name, sizeof(name), LINE_IN "%d", i * 4 + j + 1);
						line = gpiod_chip_find_line(chip, name);
						if (line != NULL) {
								motors[i].in[j] = line;

								// Request output for IN lines
								ret = gpiod_line_request_output(line, app, 0);
								if (ret < 0) {
										fprintf(stderr, "Fail to request output: %s\n", name);
										goto out1;
								}
						} else {
								if (i == 0 || j > 0 && motors[i].in[0] != NULL) {
										fprintf(stderr, "Fail to find gpio line: %s\n", name);
										ret = -ENODEV;
										goto out1;
								}
								motors[i].in[j] = motors[0].in[j];
						}
				}

        // init EN lines
        snprintf(name, sizeof(name), LINE_EN "%d", i + 1);
        motors[i].enable = gpiod_chip_find_line(chip, name);

        if (NULL != motors[i].enable) {
            if (i > 0 && motors[0].enable == NULL) {
								fprintf(stderr, "Invalid gpio line: %s\n", name);
								ret = -EINVAL;
								goto out1;
						}
            motors[i].epolar = 1;

						// Request output for EN line
						ret = gpiod_line_request_output(motors[i].enable, app, !motors[i].epolar);
						if (ret < 0) {
								fprintf(stderr, "Fail to request output: %s\n", name);
								goto out1;
						}
						gpiod_line_set_value(motors[i].enable, !motors[i].epolar);
        } else if (i > 0) {
            motors[i].enable = motors[0].enable;
            motors[i].epolar = !motors[0].epolar;
        }
    }

		// TODO: support simultaneous pan and tilt
    if (pan_steps)
        stepper_rotate(MOTOR_PAN, pan_steps);

    if (tilt_steps)
        stepper_rotate(MOTOR_TILT, tilt_steps);

   for (i = 0; i < MOTOR_ALL; i++) {
			 for (j = 0; j < 4; j++) {
						struct gpiod_line *line = motors[i].in[j];
            if (i > 0 && line == motors[0].in[j])
								break;

						gpiod_line_set_value(line, 0);
						gpiod_line_release(line);
				}

				if (motors[i].enable) {
						if (i > 0 && motors[i].enable == motors[0].enable)
								break;

						gpiod_line_set_value(motors[i].enable, !motors[i].epolar);
						gpiod_line_release(motors[i].enable);
				}
		}

// TODO: add out2 here

out1:
    gpiod_chip_close(chip); // FIXME

    return ret;
}
