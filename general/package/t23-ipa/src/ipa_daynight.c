/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Day/night switching for the IR-cut filter and the infrared illuminator.
 *
 * The filter uses the named ircut-in and ircut-out GPIO lines. The
 * controlling input publishes completed moves for the following input.
 * The illuminator is an LED class device.
 * Brightness is judged by exposure time x analogue gain in microseconds:
 * high values mean a dark scene. At night the illuminator brightens the
 * scene, so its share is estimated when night starts and subtracted before
 * the day threshold is tested. The last estimate also predicts the exposure
 * change at the next switch.
 */
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include "t23_ipa.h"

#define DN_HOLD_S	3.0	/* threshold must hold this long */
#define DN_HOLD_START_S	0.5	/* ... just after statistics resume */
#define DN_START_S	5.0	/* how long "just after" lasts */
#define DN_NIGHT_GUESS	0.125	/* exposure factor before any estimate */
#define DN_FACTOR_MIN	(1.0 / 256)
#define DN_SETTLE_S	8.0	/* longest wait for AE after night starts */
#define DN_DWELL_S	10.0	/* minimum time in a mode */
#define DN_DWELL_MAX_S	600.0
#define DN_FOLLOW_S	0.2	/* poll interval of a following input */
#define IRCUT_STATE	"/var/run/t23-ipa-ircut"
#define IRCUT_PULSE_NS	150000000L

static double now_s(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

static int find_ircut(void)
{
	glob_t chips = {0};
	size_t i;
	int result = -1, saved_errno = ENODEV;

	if (glob("/dev/gpiochip*", 0, NULL, &chips)) {
		globfree(&chips);
		errno = ENODEV;
		return -1;
	}
	for (i = 0; i < chips.gl_pathc; i++) {
		struct gpio_v2_line_request req = {
			.consumer = "t23-ipa-ircut",
			.config = {
				.flags = GPIO_V2_LINE_FLAG_OUTPUT,
				.num_attrs = 1,
				.attrs = {{
					.attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES,
					.attr.values = 0,
					.mask = 3,
				}},
			},
			.num_lines = 2,
		};
		struct gpiochip_info chip;
		unsigned int line, found = 0;
		int fd = open(chips.gl_pathv[i], O_RDONLY | O_CLOEXEC);

		if (fd < 0)
			continue;
		if (ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip) < 0) {
			close(fd);
			continue;
		}
		for (line = 0; line < chip.lines; line++) {
			struct gpio_v2_line_info info = { .offset = line };

			if (ioctl(fd, GPIO_V2_GET_LINEINFO_IOCTL, &info) < 0)
				continue;
			if (!strcmp(info.name, "ircut-in")) {
				req.offsets[0] = line;
				found |= 1;
			} else if (!strcmp(info.name, "ircut-out")) {
				req.offsets[1] = line;
				found |= 2;
			}
		}
		if (found == 3) {
			int ret = ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req);
			saved_errno = errno;

			close(fd);
			if (!ret)
				result = req.fd;
			break;
		}
		close(fd);
	}
	globfree(&chips);
	errno = saved_errno;
	return result;
}

static int set_filter(struct ipa_daynight *dn, bool in)
{
	struct gpio_v2_line_values values = { .mask = 3, .bits = in ? 1 : 2 };
	struct timespec pulse = { .tv_nsec = IRCUT_PULSE_NS };
	char state = in ? '1' : '0';
	int ret = 0;

	if (dn->ircut_fd < 0)
		return -ENODEV;
	if (ioctl(dn->ircut_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0)
		return -errno;
	while (nanosleep(&pulse, &pulse) < 0) {
		if (errno != EINTR) {
			ret = -errno;
			break;
		}
	}
	values.bits = 0;
	if (ioctl(dn->ircut_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0)
		return -errno;
	if (!ret) {
		ssize_t n = pwrite(dn->state_fd, &state, 1, 0);

		if (n != 1)
			ret = n < 0 ? -errno : -EIO;
	}
	return ret;
}

static int get_filter(bool *in)
{
	char state;
	int fd = open(IRCUT_STATE, O_RDONLY | O_CLOEXEC);
	ssize_t n;

	if (fd < 0)
		return -1;
	n = pread(fd, &state, 1, 0);
	close(fd);
	if (n != 1 || (state != '0' && state != '1'))
		return -1;
	*in = state == '1';
	return 0;
}

static void set_led(struct ipa_daynight *dn, bool on)
{
	if (dn->led_fd >= 0 &&
	    pwrite(dn->led_fd, on ? "255\n" : "0\n", on ? 4 : 2, 0) < 0)
		ipa_warn("illuminator: %s\n", strerror(errno));
}

static int enter(struct ipa_daynight *dn, bool night, double t)
{
	int ret;

	/* Remove the filter before lighting the scene, restore it after. */
	if (night) {
		ret = set_filter(dn, false);
		if (ret)
			goto err;
		set_led(dn, true);
	} else {
		set_led(dn, false);
		ret = set_filter(dn, true);
		if (ret)
			goto err;
	}
	dn->night = night;
	dn->changed = true;
	dn->since = t;
	dn->pending = 0;
	return 0;
err:
	ipa_warn("ir-cut: %s\n", strerror(-ret));
	return ret;
}

int ipa_daynight_init(struct ipa_daynight *dn, enum ipa_dn_mode mode,
		      const char *led, double to_night, double to_day)
{
	char path[96];
	bool in;

	memset(dn, 0, sizeof(*dn));
	dn->mode = mode;
	dn->to_night = to_night;
	dn->to_day = to_day;
	dn->dwell = DN_DWELL_S;
	dn->illuminator = -1;
	dn->led_fd = -1;
	dn->ircut_fd = -1;
	dn->state_fd = -1;
	if (mode != IPA_DN_FOLLOW) {
		dn->ircut_fd = find_ircut();
		if (dn->ircut_fd < 0) {
			ipa_warn("ir-cut GPIOs: %s; day/night disabled\n", strerror(errno));
			return 0;
		}
		dn->state_fd = open(IRCUT_STATE, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
		if (dn->state_fd < 0) {
			int ret = -errno;

			close(dn->ircut_fd);
			dn->ircut_fd = -1;
			return ret;
		}
	}
	if (mode != IPA_DN_FOLLOW && led && *led) {
		snprintf(path, sizeof(path), "/sys/class/leds/%s/brightness", led);
		dn->led_fd = open(path, O_WRONLY | O_CLOEXEC);
		if (dn->led_fd < 0)
			ipa_warn("%s: no illuminator\n", led);
	}

	switch (mode) {
	case IPA_DN_FOLLOW:
		if (!get_filter(&in))
			dn->night = !in;
		dn->changed = dn->night;
		return 0;
	case IPA_DN_NIGHT:
		return enter(dn, true, now_s());
	default:
		/* The filter position cannot be read back; set it. */
		return enter(dn, false, now_s());
	}
}

void ipa_daynight_close(struct ipa_daynight *dn)
{
	if (dn->mode != IPA_DN_FOLLOW && dn->night)
		enter(dn, false, now_s());
	if (dn->led_fd >= 0)
		close(dn->led_fd);
	if (dn->ircut_fd >= 0)
		close(dn->ircut_fd);
	if (dn->state_fd >= 0)
		close(dn->state_fd);
}

static bool held(struct ipa_daynight *dn, bool cond, double t)
{
	if (!cond) {
		dn->pending = 0;
		return false;
	}
	if (!dn->pending)
		dn->pending = t;
	return t - dn->pending >= (t < dn->quick_until ? DN_HOLD_START_S : DN_HOLD_S);
}

/*
 * Statistics resume after the stream was stopped. Nothing is known about
 * the light since, so the first decision need not wait out the dwell or the
 * full hold.
 */
void ipa_daynight_restart(struct ipa_daynight *dn)
{
	double t = now_s();

	dn->quick_until = t + DN_START_S;
	dn->pending = 0;
}

static bool dwelt(const struct ipa_daynight *dn, double t)
{
	return t - dn->since >= dn->dwell || t < dn->quick_until;
}

/* Exposure factor for the light change of the switch that just happened. */
double ipa_daynight_ev_factor(const struct ipa_daynight *dn, double ev_us)
{
	double share = dn->illuminator > 0 ? dn->illuminator * ev_us : -1;

	if (dn->night)
		return share > 0 ? 1 / (1 + share) : DN_NIGHT_GUESS;
	/* The illuminator went out and the filter took the infrared away. */
	if (share <= 0)
		return 1;
	return share < 1 - DN_FACTOR_MIN ? 1 / (1 - share) : 1 / DN_FACTOR_MIN;
}

/* Measure the illuminator's share once AE has adapted to it. */
static bool measure(struct ipa_daynight *dn, double ev_us, bool settled,
		    double t)
{
	if (!dn->measuring)
		return true;
	if (!settled && t - dn->since < DN_SETTLE_S)
		return false;
	dn->illuminator = fmax(0, 1 / ev_us - 1 / dn->ev_dark);
	dn->measuring = false;
	return false;
}

/*
 * @ev_us: exposure in microseconds times analogue gain
 * @settled: AE has reached its target
 * Returns true when the mode changed during this call.
 */
bool ipa_daynight_run(struct ipa_daynight *dn, double ev_us, bool settled)
{
	double t = now_s();
	bool in;

	dn->changed = false;
	if (dn->mode == IPA_DN_FOLLOW) {
		if (dn->night && ev_us > 0)
			measure(dn, ev_us, settled, t);
		if (t - dn->polled >= DN_FOLLOW_S) {
			dn->polled = t;
			if (!get_filter(&in) && in == dn->night) {
				dn->night = !in;
				dn->changed = true;
				dn->since = t;
				dn->ev_dark = ev_us;
				dn->measuring = dn->night && ev_us > 0;
			}
		}
		return dn->changed;
	}
	if (dn->mode != IPA_DN_AUTO || dn->ircut_fd < 0 || ev_us <= 0)
		return false;

	if (!dn->night) {
		if (dwelt(dn, t) && held(dn, ev_us >= dn->to_night, t)) {
			/* Back to night soon after a day switch: back off. */
			if (dn->last_day && t - dn->last_day < 2 * dn->dwell)
				dn->dwell = fmin(dn->dwell * 2, DN_DWELL_MAX_S);
			else
				dn->dwell = DN_DWELL_S;
			dn->ev_dark = ev_us;
			dn->measuring = true;
			enter(dn, true, t);
		}
		return dn->changed;
	}

	if (!measure(dn, ev_us, settled, t))
		return false;
	if (dwelt(dn, t) &&
	    held(dn, 1 / ev_us - dn->illuminator >= 1 / dn->to_day, t)) {
		dn->last_day = t;
		enter(dn, false, t);
	}
	return dn->changed;
}
