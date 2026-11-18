/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only */
/* Copyright (C) 2026 Difan Zhang */
#ifndef _AWB_CONTROLLER_H
#define _AWB_CONTROLLER_H
#include "fh8626-3ad.h"

#define AWB_UNIT_GAIN_Q9 512
/* The ISP white-balance gain fields are 13 bits wide. */
#define AWB_MIN_GAIN_Q9 1
#define AWB_MAX_GAIN_Q9 0x1fff

typedef enum { AWB_MODE_GRAY_WORLD, AWB_MODE_WHITE_POINT } awb_mode_t;

/*
 * Reference white points of the sensor, warm to cool, as the B/G and R/G
 * ratios (Q12) of a neutral surface before white balance. They define the
 * illuminant locus in the white-point mode; the colour correction of
 * isp-tuning.c is calibrated at the same points.
 */
#define AWB_LOCUS_POINTS 4
extern const int32_t awb_locus_q12[AWB_LOCUS_POINTS][2];

struct awb_config {
	awb_mode_t mode;
	uint16_t min_gain, max_gain;
};

struct awb_controller {
	struct awb_config config;
	uint16_t current_gain_r, current_gain_g, current_gain_b;
	uint16_t target_gain_r, target_gain_g, target_gain_b;

	/* Locus in log2 chroma (Q8): x = log2(B/G), y = log2(R/G). */
	int32_t locus[AWB_LOCUS_POINTS + 2][2];

	/* Smoothed estimate (Q12 log2) and the applied white point (Q8). */
	int32_t est_x, est_y;
	int32_t wp_x, wp_y;
	bool have_estimate, tracking, apply_pending;
	unsigned int move_count;

	/* Results of the last frame. */
	unsigned int grey_cells, edge_pairs;
	int32_t meas_x, meas_y;		/* Q8, before smoothing */
	uint32_t confidence;		/* 0..256 */
};

void awb_controller_set_defaults(struct awb_config *cfg);
void awb_controller_init(struct awb_controller *awb, const struct awb_config *cfg);

/*
 * Restart the estimate from the current gains, for example at start-up or
 * after a manual change.
 */
void awb_controller_sync(struct awb_controller *awb);

/*
 * Estimate the illuminant from one statistics frame and step the gains.
 * Returns true when current_gain_* changed and should be written.
 */
bool awb_controller_update(struct awb_controller *awb, const struct fh8626_grid *grid);
#endif
