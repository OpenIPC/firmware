/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only */
/* Copyright (C) 2026 Difan Zhang */
#ifndef _AE_CONTROLLER_H
#define _AE_CONTROLLER_H

#include "fh8626-3ad.h"

/* Anti-flicker mode */
typedef enum {
	AE_ANTIFLICKER_OFF  = 0,
	AE_ANTIFLICKER_50HZ = 50,
	AE_ANTIFLICKER_60HZ = 60,
} ae_antiflicker_mode_t;

/* Exposure Priority Profile (motion sharpness vs low noise) */
typedef enum {
	AE_PROFILE_NORMAL = 0,	/* Prioritize low noise: maximize shutter before gain */
	AE_PROFILE_MOTION = 1,	/* Prioritize sharpness: cap shutter at 1/100s, raise gain */
} ae_profile_t;

struct ae_control_range {
	uint32_t min, max, step;
};

struct ae_config {
	uint32_t target_luma;		/* perceived 12-bit target level (default 1856) */
	uint32_t low_light_target;	/* target at full integration and gain knee */
	uint32_t gain_knee;		/* Q6 analogue gain used before ISP gain */
	uint32_t digital_knee;		/* Q6 ISP gain used before the rest of analogue */
	ae_antiflicker_mode_t flicker_mode;
	ae_profile_t profile;
	uint32_t line_time_ns;		/* Queried active timing; zero means unknown. */
	uint32_t frame_lines;		/* current frame length */
	/*
	 * Slow shutter. Integration normally stays inside the nominal frame,
	 * the length a client chose. With max_frame_lines above it, AE may
	 * lengthen the frame up to max_frame_lines once the analogue gain
	 * reaches slow_gain_enter (Q6), and returns to the nominal frame once
	 * the gain at the nominal frame would be slow_gain_leave or less.
	 * Integration stays exposure_margin lines below the frame length.
	 */
	uint32_t nominal_frame_lines, max_frame_lines, exposure_margin;
	uint32_t slow_gain_enter, slow_gain_leave;
	/*
	 * Statistics frames between a write issued after statistics frame N
	 * and the first statistics frame N + delay that shows it: sensor
	 * integration, sensor analogue gain, ISP digital gain.
	 */
	uint8_t exposure_delay, gain_delay, digital_gain_delay;
	uint8_t clip_budget;		/* cells allowed to clip, of 256 */
};

/* Exposure state commanded after a statistics frame. */
struct ae_setting {
	uint32_t sequence;
	uint32_t exposure, again, dgain;
};

#define AE_HISTORY 16

struct ae_controller {
	struct ae_config config;

	/* Supported sensor drivers expose integration lines and Q6 gain (64 = 1x). */
	struct ae_control_range exposure, gain, digital_gain;
	uint32_t current_exposure_lines, current_analog_gain, current_digital_gain;
	uint32_t target_exposure_lines, target_analog_gain, target_digital_gain;

	/* Commanded settings, newest last, to find the one each frame shows. */
	struct ae_setting history[AE_HISTORY];
	unsigned int history_len;
	bool history_valid;

	/* Meter results of the last frame (linear 12-bit Q4 unless noted). */
	uint32_t mean, target, shadow, highlight;
	unsigned int clipped;
	uint32_t perceived;		/* square-root level of mean, 12-bit */
	int32_t error;			/* log2 Q8 against the commanded exposure */

	/* Hysteresis */
	bool stable, saturated, slow;
	unsigned int stable_count, unstable_count;
};

void ae_controller_set_defaults(struct ae_config *cfg, uint32_t target_luma);
void ae_controller_init(struct ae_controller *ae, const struct ae_config *cfg);

/*
 * Reset the exposure history to the settings read back from the devices,
 * for example at start-up or after a manual change.
 */
void ae_controller_sync(struct ae_controller *ae, uint32_t sequence);

/*
 * Run the meter and the controller on one statistics frame. wb holds the
 * current Q9 red, green and blue white-balance gains. Returns true when new
 * targets should be written; the caller writes them and then calls
 * ae_controller_commit() with the same sequence.
 */
bool ae_controller_update(struct ae_controller *ae, const struct fh8626_grid *grid,
			  const uint32_t wb[3]);
void ae_controller_commit(struct ae_controller *ae, uint32_t sequence);

/*
 * True if a commanded change can first show in statistics frame sequence,
 * so that frame differs from the previous one by the exposure change.
 */
bool ae_controller_in_transition(const struct ae_controller *ae, uint32_t sequence);

/*
 * Total gain, analogue times ISP, Q6: commanded, and shown in a frame
 * together with its analogue part.
 */
uint32_t ae_controller_total_gain(const struct ae_controller *ae);
uint32_t ae_controller_effective_gain(const struct ae_controller *ae, uint32_t sequence,
				      uint32_t *again);

/*
 * Frame length in lines that the integration commanded and still in effect
 * at statistics frame sequence needs: at least the nominal frame. Zero
 * without slow shutter.
 */
uint32_t ae_controller_frame_lines(const struct ae_controller *ae, uint32_t sequence);
/* Frame length the newest target integration needs, or zero. */
uint32_t ae_controller_target_frame_lines(const struct ae_controller *ae);
bool ae_controller_slow_shutter(const struct ae_controller *ae);

void ae_controller_set_digital_gain_range(struct ae_controller *ae,
					 uint32_t min, uint32_t max, uint32_t step);
void ae_controller_set_current_digital_gain(struct ae_controller *ae, uint32_t gain);

#endif /* _AE_CONTROLLER_H */
