#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdint.h>

struct stepper_motor {
	struct gpiod_chip *chip;
	struct gpiod_line *input_line[4];
	struct gpiod_line *en_line;
	uint8_t polarity;
};

static int step_sequence[8][4] = { { 1, 0, 0, 0 }, { 1, 1, 0, 0 },
				   { 0, 1, 0, 0 }, { 0, 1, 1, 0 },
				   { 0, 0, 1, 0 }, { 0, 0, 1, 1 },
				   { 0, 0, 0, 1 }, { 1, 0, 0, 1 } };

static int parse_int(const char *str, int *out)
{
	char *end;
	long v = strtol(str, &end, 10);
	if (end == str || *end || v > INT_MAX || v < INT_MIN)
		return -1;
	*out = (int)v;
	return 0;
}

static void print_help(const char *prog_name)
{
	printf("\nUsage: %s [OPTIONS]\n"
	       "Options:\n"
	       "  --pan=NUM    Horizontal motor run steps\n"
	       "  --tilt=NUM   Vertical motor run steps\n"
	       "  -h, --help   Display this help message\n",
	       prog_name);
}

static void release_motor_config(struct stepper_motor *all_motors)
{
	for (int i = 0; i < 4; i++) {
		if (all_motors[0].input_line[i])
			gpiod_line_release(all_motors[0].input_line[i]);
		if (all_motors[1].input_line[i] &&
		    all_motors[1].input_line[i] != all_motors[0].input_line[i])
			gpiod_line_release(all_motors[1].input_line[i]);
	}

	if (all_motors[0].en_line)
		gpiod_line_release(all_motors[0].en_line);
	if (all_motors[1].en_line &&
	    all_motors[1].en_line != all_motors[0].en_line)
		gpiod_line_release(all_motors[1].en_line);

	if (all_motors[0].chip)
		gpiod_chip_close(all_motors[0].chip);
	if (all_motors[1].chip && all_motors[1].chip != all_motors[0].chip)
		gpiod_chip_close(all_motors[1].chip);
}

static int get_motor_config(struct stepper_motor *all_motors)
{
	struct gpiod_chip *chip = NULL;
	struct gpiod_line *line = NULL;
	const char *pos = NULL, *line_name = NULL;

	int val = 0;
	int num_lines = 0, num_in_lines = 0, num_en_lines = 0;

	memset(all_motors, 0, sizeof(struct stepper_motor) * 2);

	const char *chip_name = "/dev/gpiochip0";
	chip = gpiod_chip_open(chip_name);
	if (!chip) {
		fprintf(stderr, "Failed to open GPIO chip %s.\n", chip_name);
		return -1;
	}

	num_lines = gpiod_chip_num_lines(chip);
	for (int i = 0; i < num_lines; i++) {
		line = gpiod_chip_get_line(chip, i);
		if (!line) {
			fprintf(stderr, "Failed to get line %d from chip %s.\n",
				i, chip_name);
			release_motor_config(all_motors);
			return -1;
		}

		line_name = gpiod_line_name(line);
		if (!line_name)
			continue;

		pos = strstr(line_name, "motor_in");
		if (pos && strlen(pos) > strlen("motor_in")) {
			val = *(pos + strlen("motor_in")) - '0' - 1;
			if (val > 8 || val < 0) {
				fprintf(stderr,
					"Invalid motor input line name: %s.\n",
					line_name);
				release_motor_config(all_motors);
				return -1;
			}
			all_motors[val / 4].input_line[val % 4] = line;
			num_in_lines++;
			printf("Matched input line %s, offset %d.\n", line_name,
			       i);
			continue;
		}

		pos = strstr(line_name, "motor_en");
		if (pos && pos != line_name) {
			char c = *(pos - 1);
			if (c == 'h' || c == 'v') {
				all_motors[c == 'v'].en_line = line;
				num_en_lines++;
				printf("Matched enable line %s, offset %d.\n",
				       line_name, i);
				continue;
			}
		}

		gpiod_line_release(line);
	}

	if (num_in_lines == 4) {
		for (int i = 0; i < 4; i++)
			all_motors[1].input_line[i] =
				all_motors[0].input_line[i];
	} else if (num_in_lines != 8) {
		fprintf(stderr,
			"Expected 8 or 4 motor input lines, but found %d.\n",
			num_in_lines);
		release_motor_config(all_motors);
		gpiod_chip_close(chip);
		return -1;
	}

	if (num_en_lines != 2 && num_en_lines != 0) {
		fprintf(stderr,
			"Expected 2 or 0 motor enable lines, but found %d.\n",
			num_en_lines);
		release_motor_config(all_motors);
		gpiod_chip_close(chip);
		return -1;
	}

	all_motors[0].polarity = all_motors[1].polarity = 1;
	all_motors[0].chip = all_motors[1].chip = chip;
	return 0;
}

static int init_input_lines(struct stepper_motor *m)
{
	for (int i = 0; i < 4; i++) {
		if (gpiod_line_request_output(m->input_line[i],
					      "my-gpio-motors", 0) < 0) {
			fprintf(stderr,
				"Failed to request line %s as output: %s\n",
				gpiod_line_name(m->input_line[i]),
				strerror(errno));
			return -1;
		}
	}
	return 0;
}

static int init_en_lines(struct gpiod_line *en_line)
{
	if (gpiod_line_request_output(en_line, "my-gpio-motors", 0) < 0) {
		fprintf(stderr,
			"Failed to request enable line %s as output: %s\n",
			gpiod_line_name(en_line), strerror(errno));
		return -1;
	}
	gpiod_line_set_value(en_line, 0);
	return 0;
}

static int init_motor_gpio_lines(struct stepper_motor *all_motors,
				 int pan_steps, int tilt_steps)
{
	if (init_input_lines(&all_motors[0]) < 0)
		return -1;

	if (tilt_steps != 0 &&
	    all_motors[1].input_line[0] != all_motors[0].input_line[0])
		if (init_input_lines(&all_motors[1]) < 0)
			return -1;

	if (all_motors[0].en_line && init_en_lines(all_motors[0].en_line) < 0)
		return -1;

	if (all_motors[1].en_line &&
	    all_motors[1].en_line != all_motors[0].en_line)
		if (init_en_lines(all_motors[1].en_line) < 0)
			return -1;

	return 0;
}

static int run_motor(struct stepper_motor *motor, int run_step, int direction)
{
	int step_start = direction == 1 ? 0 : run_step;
	int step_end = direction == 1 ? run_step : 0;
	int step_delta = direction == 1 ? 1 : -1;

	if (motor->en_line)
		gpiod_line_set_value(motor->en_line, motor->polarity);

	for (int step = step_start; step != step_end; step += step_delta) {
		for (int i = 0; i < 4; i++)
			gpiod_line_set_value(motor->input_line[i],
					     step_sequence[step % 8][i]);
		usleep(1000);
	}

	if (motor->en_line)
		gpiod_line_set_value(motor->en_line, !motor->polarity);
	for (int i = 0; i < 4; i++)
		gpiod_line_set_value(motor->input_line[i], 0);

	return 0;
}

static int run_motor_by_steps(struct stepper_motor *all_motors, int steps,
			      int motor_idx, const char *name)
{
	if (steps == 0)
		return 0;

	int ret = run_motor(&all_motors[motor_idx], abs(steps),
			    steps < 0 ? -1 : 1);
	if (ret < 0) {
		release_motor_config(all_motors);
		fprintf(stderr, "Failed to run %s motor\n", name);
	}
	return ret;
}

int main(int argc, char **argv)
{
	struct stepper_motor all_motors[2];
	int pan_steps = 0, tilt_steps = 0, opt = 0;

	static struct option long_options[] = {
		{ "pan", required_argument, 0, 1 },
		{ "tilt", required_argument, 0, 2 },
		{ "help", no_argument, 0, 'h' },
		{ 0, 0, 0, 0 }
	};

	while ((opt = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
		switch (opt) {
		case 1:
			if (parse_int(optarg, &pan_steps) < 0) {
				fprintf(stderr, "Invalid pan steps value: %s\n",
					optarg);
				print_help(argv[0]);
				return -1;
			}
			break;
		case 2:
			if (parse_int(optarg, &tilt_steps) < 0) {
				fprintf(stderr,
					"Invalid tilt steps value: %s\n",
					optarg);
				print_help(argv[0]);
				return -1;
			}
			break;
		case 'h':
			print_help(argv[0]);
			return 0;
		default:
			print_help(argv[0]);
			return -1;
		}
	}

	printf("pan_steps: %d, tilt_steps: %d\n", pan_steps, tilt_steps);

	if (get_motor_config(all_motors) < 0) {
		fprintf(stderr,
			"Failed to get GPIO configuration for motors.\n");
		return -1;
	}

	if (init_motor_gpio_lines(all_motors, pan_steps, tilt_steps) < 0) {
		release_motor_config(all_motors);
		fprintf(stderr,
			"Failed to set GPIO lines as output for motors.\n");
		return -1;
	}

	if (run_motor_by_steps(all_motors, pan_steps, 0, "pan") < 0)
		return -1;

	if (run_motor_by_steps(all_motors, tilt_steps, 1, "tilt") < 0)
		return -1;

	release_motor_config(all_motors);
	return 0;
}
