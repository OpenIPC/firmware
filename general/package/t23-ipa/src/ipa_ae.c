/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Auto exposure on the hardware AE zones. The zone luma is the 8-bit,
 * pre-gamma mean of each zone, weighted by the tuning's 15x15 zone weights.
 * The target follows the tuning's target-by-exposure curve. The frame
 * histogram limits it so the brightest pixels stay below clipping, by at
 * most AE_HI_MAX_STOPS. With a mains frequency, exposures of at least one
 * light flicker period are whole periods, so lamps cannot band the frame.
 */
#include <errno.h>
#include <math.h>
#include <string.h>
#include "t23_ipa.h"

/* Frames between an exposure change and the statistics that reflect it */
#define AE_SETTLE_FRAMES	3
#define AE_DEADBAND		0.06	/* log2 units */
#define AE_HI_QUANTILE		0.98
#define AE_HI_LEVEL		170.0	/* pre-gamma; output reaches ~225 of 235 */
#define AE_HI_MAX_STOPS		1.0
#define AE_FRAME_GAIN		2.0	/* gain reached before frames lengthen */
#define AE_STEP_MAX		3.0	/* stops per step far from the target */
#define AE_FAR_DAMP		0.85
#define AE_CLIP_LEVEL		254.0	/* histogram bin of clipped pixels */
#define AE_CLIP_STEP		1.5	/* least step down while clipped */

int ipa_ae_init(struct ipa_ae *ae, const struct ipa_tuning *t,
		unsigned int target, unsigned int mains_hz)
{
	const int64_t *weight = ipa_tuning_get(t, "ae", "ae_aezone_weight",
					       T23_ISP_ZONES);
	const int64_t *ev = ipa_tuning_get(t, "ae", "ae_ev_list", AE_CURVE_POINTS);
	const int64_t *curve = ipa_tuning_get(t, "ae", "ae_target_list_p0960_0x220",
					      AE_CURVE_POINTS);
	unsigned int i;

	if (!weight || !ev || !curve)
		return -EINVAL;
	memset(ae, 0, sizeof(*ae));
	ae->fixed_target = target;
	/* Lamps flicker at twice the mains frequency. */
	ae->flicker_us = mains_hz ? 1e6 / (2.0 * mains_hz) : 0;
	for (i = 0; i < T23_ISP_ZONES; i++)
		ae->zone_weight[i] = weight[i];
	for (i = 0; i < AE_CURVE_POINTS; i++) {
		ae->curve_ev[i] = ev[i];
		ae->curve_target[i] = curve[i];
	}
	return 0;
}

/* Target luma for an exposure in microseconds x gain */
static double ae_curve(const struct ipa_ae *ae, double ev_us)
{
	unsigned int i;

	if (ev_us <= ae->curve_ev[0])
		return ae->curve_target[0];
	for (i = 1; i < AE_CURVE_POINTS; i++)
		if (ev_us <= ae->curve_ev[i])
			return ae->curve_target[i - 1] +
			       (ae->curve_target[i] - ae->curve_target[i - 1]) *
			       (ev_us - ae->curve_ev[i - 1]) /
			       (ae->curve_ev[i] - ae->curve_ev[i - 1]);
	return ae->curve_target[AE_CURVE_POINTS - 1];
}

/* Pre-gamma level below which a fraction q of the frame lies */
static double hist_quantile(const uint32_t *hist, double q)
{
	double total = 0, sum = 0;
	unsigned int i;

	for (i = 0; i < T23_ISP_HIST_BINS; i++)
		total += hist[i];
	if (!total)
		return -1;
	for (i = 0; i < T23_ISP_HIST_BINS; i++) {
		sum += hist[i];
		if (sum >= q * total)
			return i + 0.5;
	}
	return T23_ISP_HIST_BINS - 0.5;
}

static double ae_measure(const struct ipa_ae *ae,
			 const struct t23_isp_stat_buffer *stats)
{
	double area = (double)stats->zone_width * stats->zone_height;
	double sum = 0, weights = 0;
	unsigned int i;

	if (!area)
		return -1;
	for (i = 0; i < T23_ISP_ZONES; i++) {
		const struct t23_isp_ae_zone *z = &stats->ae[i];
		double luma = ((double)z->sum_low + z->sum_mid + z->sum_high) / area;

		sum += ae->zone_weight[i] * luma;
		weights += ae->zone_weight[i];
	}
	return weights ? sum / weights : -1;
}

/*
 * Shortest frame for the exposure. With a flicker period it is whole
 * periods, so flicker left in short exposures stands still between frames.
 */
static int ae_vblank(const struct ipa_sensor *s, double period, int exposure)
{
	double frame = fmax(exposure + s->exposure_margin,
			    s->height + s->vblank_min);

	if (period)
		frame = round(ceil(frame / period - 1e-6) * period);
	return (int)fmin(frame - s->height, s->vblank_max);
}

/* Program exposure lines x gain @total, preferring exposure time. */
static void ae_apply(struct ipa_ae *ae, struct ipa_sensor *s, double total)
{
	double period = ae->flicker_us && s->line_us ?
			ae->flicker_us / s->line_us : 0;
	double gain, exposure, longest;
	uint32_t log2_q16;
	int index, vblank;

	/*
	 * Prefer exposure time within the shortest frame, then analogue gain
	 * up to AE_FRAME_GAIN, then longer frames, then the rest of the gain.
	 */
	longest = ipa_sensor_exposure_max(s, s->vblank_max);
	exposure = fmin(total, ipa_sensor_exposure_max(s, s->vblank_min));
	if (total > exposure * AE_FRAME_GAIN)
		exposure = fmin(total / AE_FRAME_GAIN, longest);
	exposure = fmax(exposure, s->exposure_min);
	if (period && exposure >= period)
		exposure = floor(exposure / period) * period;
	log2_q16 = total > exposure ?
		   (uint32_t)(log2(total / exposure) * Q16_ONE) : 0;
	index = ipa_sensor_gain_index(s, log2_q16);
	gain = exp2(ipa_sensor_gain_log2(s, index) / Q16_ONE);
	/* Gain steps are fine; a whole-period exposure stays as it is. */
	if (!period || exposure < period)
		exposure = fmax(s->exposure_min, fmin(longest, total / gain));
	vblank = ae_vblank(s, period, (int)lround(exposure));
	if (ipa_sensor_apply(s, (int)lround(exposure), index, vblank) == 0)
		ae->settle = AE_SETTLE_FRAMES;
	ae->gain_log2_q16 = ipa_sensor_gain_log2(s, s->gain_index);
	ae->ev = s->exposure * exp2(ae->gain_log2_q16 / Q16_ONE);
}

void ipa_ae_run(struct ipa_ae *ae, struct ipa_sensor *s,
		const struct t23_isp_stat_buffer *stats)
{
	double gain, step, luma_step, period;
	int vblank;

	ae->gain_log2_q16 = ipa_sensor_gain_log2(s, s->gain_index);
	gain = exp2(ae->gain_log2_q16 / Q16_ONE);
	ae->ev = s->exposure * gain;
	if (!(stats->flags & T23_ISP_STATS_AE))
		return;
	ae->luma = ae_measure(ae, stats);
	ae->highlight = stats->flags & T23_ISP_STATS_HIST ?
			hist_quantile(stats->hist[0], AE_HI_QUANTILE) : -1;
	ae->target = ae->fixed_target ? ae->fixed_target :
		     ae_curve(ae, ae->ev * s->line_us);
	if (ae->luma < 0 || ae->settle) {
		if (ae->settle)
			ae->settle--;
		return;
	}

	luma_step = log2(ae->target / fmax(ae->luma, 0.25));
	step = luma_step;
	if (ae->highlight > 0)
		step = fmin(step, fmax(log2(AE_HI_LEVEL / ae->highlight),
				       luma_step - AE_HI_MAX_STOPS));
	/*
	 * A clipped mean hides how far over the frame is. Step down harder,
	 * but never past the highlight limit, so a lamp cannot pull it lower.
	 */
	if (ae->highlight >= AE_CLIP_LEVEL && step < 0)
		step = fmin(step, fmax(-AE_CLIP_STEP / AE_FAR_DAMP,
				       luma_step - AE_HI_MAX_STOPS));
	ae->converged = fabs(step) < AE_DEADBAND;
	if (ae->converged) {
		period = ae->flicker_us && s->line_us ?
			 ae->flicker_us / s->line_us : 0;
		vblank = ae_vblank(s, period, s->exposure);
		if (period && vblank != s->vblank &&
		    !ipa_sensor_apply(s, s->exposure, s->gain_index, vblank))
			ae->settle = AE_SETTLE_FRAMES;
		return;
	}
	/*
	 * Approach the target in damped steps. Far from it, as after a
	 * day/night switch, take up to AE_STEP_MAX stops at a time.
	 */
	if (fabs(step) <= 1.0)
		step *= 0.7;
	else
		step = copysign(fmin(fabs(step) * AE_FAR_DAMP, AE_STEP_MAX), step);
	ae_apply(ae, s, ae->ev * exp2(step));
}

/* Scale the exposure ahead of a known change in scene light. */
void ipa_ae_scale(struct ipa_ae *ae, struct ipa_sensor *s, double factor)
{
	double ev = s->exposure *
		    exp2(ipa_sensor_gain_log2(s, s->gain_index) / Q16_ONE);

	if (factor > 0 && fabs(log2(factor)) >= AE_DEADBAND) {
		ae_apply(ae, s, ev * factor);
		ae->converged = false;
	}
}
