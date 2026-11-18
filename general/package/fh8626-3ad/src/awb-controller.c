/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Automatic white balance on the 16x16 zone statistics.
 *
 * Chroma is measured in log2 space before white balance: x = log2(B/G),
 * y = log2(R/G), in Q8. Black-body and daylight illuminants of a given
 * sensor lie close to a curve in this plane; the four reference white
 * points of the sensor, extended by 30% beyond each end, approximate it as
 * a polyline.
 *
 * White-point mode:
 *  1. Cells take part when their green mean is at least 32 (12-bit), well
 *     above the noise left after black-level subtraction, and their
 *     brightest sample is below 3600, so no channel is clipped.
 *  2. Grey zone: a cell within 48 (about 14% in each ratio) of the locus is
 *     a candidate grey surface. Its weight falls with the squared distance
 *     and rises with its brightness up to 1024. The weighted mean chroma of
 *     these cells is the grey-world estimate within the zone. Strongly
 *     coloured cells are outliers and carry no weight.
 *  3. Grey edge: the summed channel differences between neighbouring cells
 *     with at least 12% green contrast, both near the locus, estimate the
 *     illuminant from edges, which coloured flat areas do not bias. It
 *     counts for half when there are enough such pairs and it lies near
 *     the locus itself.
 *  4. The combined estimate is pulled to within 12 (about 3.5%) of the
 *     locus, which removes most of the remaining scene bias towards green
 *     or magenta.
 *  5. With fewer than 8 cells in the zone, the mean of all usable cells,
 *     projected onto the locus, serves at a quarter of the confidence. With
 *     fewer than 8 usable cells the white point holds.
 *
 * Start-up: a white point taken over from gains that are not near the
 * locus (the unity reset gains, for instance) is replaced by the third
 * reference point, which is applied at once.
 *
 * Temporal behaviour: an exponential filter, a quarter per frame scaled by
 * confidence (the number of grey cells, full at 64), a half for changes
 * larger than a quarter stop. The applied white point holds until the
 * filtered estimate is more than 8 (about 2%) away for three frames, or 32
 * at once, then tracks it until it stops moving.
 *
 * Gains: the white point gives the neutral B/G and R/G ratios; each gain is
 * the inverse ratio, normalised so that the smallest gain is unity. A
 * clipped white then reaches full scale in all three channels.
 *
 * The grey-world mode takes the mean of all usable cells, without the zone
 * or the locus.
 */
#include "awb-controller.h"

#define MIN_GREEN_Q4	(32 << 4)
#define CLIP_SAMPLE	3600
#define ZONE		48
#define EDGE_ZONE	(2 * ZONE)
#define LOCUS_PULL	12
#define MIN_GREY_CELLS	8
#define FULL_CONFIDENCE	64
#define MIN_EDGE_PAIRS	16
#define HOLD_BAND	8
#define JUMP_BAND	32
#define DEFAULT_POINT	2	/* index into awb_locus_q12 */

const int32_t awb_locus_q12[AWB_LOCUS_POINTS][2] = {
	{ 1252, 3768 }, { 1635, 3026 }, { 2217, 2456 }, { 2923, 2081 },
};

void awb_controller_set_defaults(struct awb_config *cfg)
{
	memset(cfg, 0, sizeof(*cfg));
	cfg->mode = AWB_MODE_WHITE_POINT;
	cfg->min_gain = AWB_MIN_GAIN_Q9;
	cfg->max_gain = AWB_MAX_GAIN_Q9;
}

static int32_t ratio_log(uint64_t num, uint64_t den)
{
	return fx_log2_q8(num) - fx_log2_q8(den);
}

void awb_controller_init(struct awb_controller *awb, const struct awb_config *cfg)
{
	unsigned int i, n = AWB_LOCUS_POINTS;

	memset(awb, 0, sizeof(*awb));
	if (cfg)
		awb->config = *cfg;
	else
		awb_controller_set_defaults(&awb->config);
	awb->current_gain_r = awb->current_gain_g = awb->current_gain_b = AWB_UNIT_GAIN_Q9;
	awb->target_gain_r = awb->target_gain_g = awb->target_gain_b = AWB_UNIT_GAIN_Q9;
	for (i = 0; i < n; i++) {
		awb->locus[i + 1][0] = ratio_log(awb_locus_q12[i][0], 4096);
		awb->locus[i + 1][1] = ratio_log(awb_locus_q12[i][1], 4096);
	}
	for (i = 0; i < 2; i++) {
		awb->locus[0][i] = awb->locus[1][i] +
				   (awb->locus[1][i] - awb->locus[2][i]) * 3 / 10;
		awb->locus[n + 1][i] = awb->locus[n][i] +
				       (awb->locus[n][i] - awb->locus[n - 1][i]) * 3 / 10;
	}
}

/* Squared distance (Q8^2) from (x, y) to the locus and the nearest point. */
static int64_t locus_distance(const struct awb_controller *awb, int32_t x, int32_t y,
			      int32_t *px, int32_t *py)
{
	int64_t best = INT64_MAX;
	unsigned int i;

	/* Chroma is within +-16 stops (Q8), so 32-bit products suffice. */
	x = clamp_i32(x, -4096, 4096);
	y = clamp_i32(y, -4096, 4096);
	for (i = 0; i + 1 < AWB_LOCUS_POINTS + 2; i++) {
		int32_t dx = awb->locus[i + 1][0] - awb->locus[i][0];
		int32_t dy = awb->locus[i + 1][1] - awb->locus[i][1];
		int32_t ox = x - awb->locus[i][0], oy = y - awb->locus[i][1];
		int32_t len = dx * dx + dy * dy;
		int32_t dot = ox * dx + oy * dy;
		int32_t t, qx, qy;
		int64_t d;

		/* Projection parameter t in Q8, 0..256 along the segment. */
		if (dot <= 0 || !len)
			t = 0;
		else if (dot >= len)
			t = 256;
		else if (len < 1 << 22)
			t = (dot << 8) / len;
		else
			t = (int32_t)(((int64_t)dot << 8) / len);
		qx = awb->locus[i][0] + dx * t / 256;
		qy = awb->locus[i][1] + dy * t / 256;
		d = (int64_t)(x - qx) * (x - qx) + (int64_t)(y - qy) * (y - qy);
		if (d < best) {
			best = d;
			if (px) {
				*px = (int32_t)qx;
				*py = (int32_t)qy;
			}
		}
	}
	return best;
}

void awb_controller_sync(struct awb_controller *awb)
{
	if (!awb->current_gain_r || !awb->current_gain_g || !awb->current_gain_b)
		return;
	/* A neutral surface reads B/G = gain_g / gain_b, R/G likewise. */
	awb->wp_x = ratio_log(awb->current_gain_g, awb->current_gain_b);
	awb->wp_y = ratio_log(awb->current_gain_g, awb->current_gain_r);
	awb->apply_pending = false;
	if (locus_distance(awb, awb->wp_x, awb->wp_y, NULL, NULL) >= ZONE * ZONE) {
		awb->wp_x = awb->locus[DEFAULT_POINT + 1][0];
		awb->wp_y = awb->locus[DEFAULT_POINT + 1][1];
		awb->apply_pending = true;
	}
	awb->est_x = awb->wp_x * 16;
	awb->est_y = awb->wp_y * 16;
	awb->have_estimate = true;
	awb->tracking = false;
	awb->move_count = 0;
}

/* Chroma estimate of one frame, false when the scene gives none. */
static bool estimate(struct awb_controller *awb, const struct fh8626_grid *grid,
		     int32_t *ex, int32_t *ey)
{
	static int32_t cx[GRID_CELLS], cy[GRID_CELLS];
	static uint8_t near[GRID_CELLS];
	int64_t sx = 0, sy = 0, sw = 0;
	uint64_t er = 0, eg = 0, eb = 0, tr = 0, tg = 0, tb = 0;
	unsigned int i, grey = 0, pairs = 0, usable = 0;
	int32_t gx, gy, x = 0, y = 0, px, py;
	uint32_t lambda = 0;
	int64_t d2;

	for (i = 0; i < GRID_CELLS; i++) {
		uint32_t r = grid->r[i], g = grid->g[i], b = grid->b[i];

		near[i] = 0;
		if (g < MIN_GREEN_Q4 || r < 16 || b < 16 || grid->max[i] >= CLIP_SAMPLE)
			continue;
		usable++;
		tr += r;
		tg += g;
		tb += b;
		cx[i] = ratio_log(b, g);
		cy[i] = ratio_log(r, g);
		d2 = locus_distance(awb, cx[i], cy[i], NULL, NULL);
		if (d2 < EDGE_ZONE * EDGE_ZONE)
			near[i] = 1;
		if (d2 < ZONE * ZONE) {
			uint32_t bright = g >> 4 < 1024 ? g >> 4 : 1024;
			int64_t w = (ZONE * ZONE - d2) * bright >> 4;

			near[i] = 2;
			sx += w * cx[i];
			sy += w * cy[i];
			sw += w;
			grey++;
		}
	}
	awb->grey_cells = grey;
	awb->edge_pairs = 0;

	if (awb->config.mode == AWB_MODE_GRAY_WORLD) {
		if (usable < MIN_GREY_CELLS)
			return false;
		*ex = ratio_log(tb, tg);
		*ey = ratio_log(tr, tg);
		awb->confidence = usable >= FULL_CONFIDENCE ? 256 :
				  usable * 256 / FULL_CONFIDENCE;
		return true;
	}
	if (grey < MIN_GREY_CELLS || !sw) {
		if (usable < MIN_GREY_CELLS)
			return false;
		/* Too few grey surfaces: the scene mean, on the locus. */
		locus_distance(awb, ratio_log(tb, tg), ratio_log(tr, tg), ex, ey);
		awb->confidence = usable >= FULL_CONFIDENCE ? 64 :
				  usable * 64 / FULL_CONFIDENCE;
		return true;
	}
	gx = (int32_t)(sx / sw);
	gy = (int32_t)(sy / sw);

	/* Grey edge over horizontal and vertical neighbours. */
	for (i = 0; i < GRID_CELLS; i++) {
		unsigned int k, j;

		if (!near[i])
			continue;
		for (k = 0; k < 2; k++) {
			uint32_t dg;

			if (k == 0 && (i % GRID_COLS) == GRID_COLS - 1)
				continue;
			j = k ? i + GRID_COLS : i + 1;
			if (j >= GRID_CELLS || !near[j])
				continue;
			dg = (uint32_t)abs_i32((int32_t)grid->g[i] - grid->g[j]);
			if (dg * 8 < (grid->g[i] > grid->g[j] ? grid->g[i] : grid->g[j]))
				continue;
			er += (uint32_t)abs_i32((int32_t)grid->r[i] - grid->r[j]);
			eg += dg;
			eb += (uint32_t)abs_i32((int32_t)grid->b[i] - grid->b[j]);
			pairs++;
		}
	}
	awb->edge_pairs = pairs;
	if (pairs && er && eb) {
		x = ratio_log(eb, eg);
		y = ratio_log(er, eg);
		if (locus_distance(awb, x, y, NULL, NULL) < EDGE_ZONE * EDGE_ZONE)
			lambda = pairs >= MIN_EDGE_PAIRS ? 128 : pairs * 128 / MIN_EDGE_PAIRS;
	}
	if (lambda) {
		gx = (gx * (int32_t)(256 - lambda) + x * (int32_t)lambda) / 256;
		gy = (gy * (int32_t)(256 - lambda) + y * (int32_t)lambda) / 256;
	}

	/* Pull towards the locus. */
	d2 = locus_distance(awb, gx, gy, &px, &py);
	if (d2 > LOCUS_PULL * LOCUS_PULL) {
		int32_t d = (int32_t)fx_isqrt((uint64_t)d2);

		gx = px + (gx - px) * LOCUS_PULL / d;
		gy = py + (gy - py) * LOCUS_PULL / d;
	}
	*ex = gx;
	*ey = gy;
	awb->confidence = grey >= FULL_CONFIDENCE ? 256 : grey * 256 / FULL_CONFIDENCE;
	return true;
}

static uint16_t gain_from_ratio(int32_t log_ratio, const struct awb_config *cfg)
{
	/* gain = 512 / ratio = 512 * 2^-log_ratio */
	uint64_t g = (fx_exp2_q16(-log_ratio) * AWB_UNIT_GAIN_Q9 + 32768) >> 16;

	return (uint16_t)clamp_u32(g > UINT32_MAX ? UINT32_MAX : (uint32_t)g,
				   cfg->min_gain, cfg->max_gain);
}

/* Gains are the inverse neutral ratios; the smallest one is unity. */
static bool apply_white_point(struct awb_controller *awb)
{
	uint16_t old_r = awb->current_gain_r, old_g = awb->current_gain_g;
	uint16_t old_b = awb->current_gain_b;
	int32_t lo = 0;

	if (awb->wp_x > lo)
		lo = awb->wp_x;
	if (awb->wp_y > lo)
		lo = awb->wp_y;
	awb->target_gain_g = gain_from_ratio(-lo, &awb->config);
	awb->target_gain_b = gain_from_ratio(awb->wp_x - lo, &awb->config);
	awb->target_gain_r = gain_from_ratio(awb->wp_y - lo, &awb->config);
	awb->current_gain_r = awb->target_gain_r;
	awb->current_gain_g = awb->target_gain_g;
	awb->current_gain_b = awb->target_gain_b;
	awb->apply_pending = false;
	return awb->current_gain_r != old_r || awb->current_gain_g != old_g ||
	       awb->current_gain_b != old_b;
}

bool awb_controller_update(struct awb_controller *awb, const struct fh8626_grid *grid)
{
	int32_t mx, my, dx, dy, nx, ny, alpha, delta;

	if (!awb->config.min_gain || awb->config.min_gain > awb->config.max_gain)
		return false;
	if (!estimate(awb, grid, &mx, &my))
		return awb->apply_pending && apply_white_point(awb);
	awb->meas_x = mx;
	awb->meas_y = my;
	if (!awb->have_estimate) {
		awb->est_x = mx * 16;
		awb->est_y = my * 16;
		awb->have_estimate = true;
		awb->tracking = true;
	}

	dx = mx * 16 - awb->est_x;
	dy = my * 16 - awb->est_y;
	alpha = abs_i32(dx) > 64 * 16 || abs_i32(dy) > 64 * 16 ? 128 : 64;
	alpha = alpha * (int32_t)awb->confidence / 256;
	if (alpha < 8)
		alpha = 8;
	awb->est_x += dx * alpha / 256;
	awb->est_y += dy * alpha / 256;

	/* Hysteresis on the applied white point. */
	nx = (awb->est_x + 8) >> 4;
	ny = (awb->est_y + 8) >> 4;
	delta = abs_i32(nx - awb->wp_x);
	if (abs_i32(ny - awb->wp_y) > delta)
		delta = abs_i32(ny - awb->wp_y);
	if (!awb->tracking) {
		if (delta > JUMP_BAND || (delta > HOLD_BAND && ++awb->move_count >= 3))
			awb->tracking = true;
		else if (delta <= HOLD_BAND)
			awb->move_count = 0;
	}
	if (awb->tracking) {
		if (delta == 0) {
			awb->tracking = false;
			awb->move_count = 0;
		}
		awb->wp_x = nx;
		awb->wp_y = ny;
	}

	return apply_white_point(awb);
}
