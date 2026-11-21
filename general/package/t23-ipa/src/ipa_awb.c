/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Locus-constrained auto white balance.
 *
 * The tuning describes white points on a 15x15 grid over R/G (columns) and
 * B/G (rows), scaled as 256 * ratio * channel multiplier: a white-likelihood
 * table, a mired table and a per-zone spatial weight. Zones are weighted as
 * in the vendor algorithm (likelihood x count x spatial, one outlier pass).
 * The weighted white point is then projected onto the Planckian locus,
 * taken as the ridge of the likelihood table, allowing a bounded tint
 * offset. The projection keeps single-colour scenes from pulling the
 * estimate off the locus.
 */
#include <errno.h>
#include <math.h>
#include <string.h>
#include "t23_ipa.h"

#define AWB_GRID		15
#define AWB_DIM_EV		200	/* exposure lines x gain */
#define AWB_SAMPLES		96
#define AWB_SMOOTHING		0.3

static int load_grid(const struct ipa_tuning *t, const char *name,
		     double grid[AWB_GRID][AWB_GRID], double scale)
{
	const int64_t *v = ipa_tuning_get(t, "awb", name, AWB_GRID * AWB_GRID);
	unsigned int i;

	if (!v)
		return -EINVAL;
	for (i = 0; i < AWB_GRID * AWB_GRID; i++)
		grid[i / AWB_GRID][i % AWB_GRID] = v[i] * scale;
	return 0;
}

/* Index of the node interval holding @x on an ascending axis, and its fraction */
static unsigned int axis_find(const double *axis, double x, double *frac)
{
	unsigned int i;

	if (x <= axis[0]) {
		*frac = 0;
		return 0;
	}
	for (i = 1; i < AWB_GRID; i++)
		if (x < axis[i])
			break;
	if (i == AWB_GRID) {
		*frac = 1;
		return AWB_GRID - 2;
	}
	*frac = (x - axis[i - 1]) / (axis[i] - axis[i - 1]);
	return i - 1;
}

/* Bilinear lookup; points outside the grid take the edge value. */
static double grid_lookup(const struct ipa_awb *awb,
			  const double grid[AWB_GRID][AWB_GRID],
			  double rg, double bg)
{
	double fx, fy, top, bottom;
	unsigned int c = axis_find(awb->axis_rg, rg, &fx);
	unsigned int r = axis_find(awb->axis_bg, bg, &fy);

	top = grid[r][c] + (grid[r][c + 1] - grid[r][c]) * fx;
	bottom = grid[r + 1][c] + (grid[r + 1][c + 1] - grid[r + 1][c]) * fx;
	return top + (bottom - top) * fy;
}

/*
 * The locus is the ridge of the likelihood table: for each mired band, the
 * centroid of the samples within 90% of that band's peak weight.
 */
static void build_locus(struct ipa_awb *awb)
{
	const unsigned int bands = sizeof(awb->locus) / sizeof(awb->locus[0]);
	double peak[64] = { 0 }, sum_rg[64] = { 0 }, sum_bg[64] = { 0 };
	double sum_m[64] = { 0 }, n[64] = { 0 };
	double lo = 1e9, hi = 0, rg, bg, w, m;
	unsigned int i, j, pass, b;

	for (pass = 0; pass < 3; pass++) {
		for (i = 0; i < AWB_SAMPLES; i++) {
			bg = awb->axis_bg[0] + (awb->axis_bg[AWB_GRID - 1] -
				awb->axis_bg[0]) * i / (AWB_SAMPLES - 1);
			for (j = 0; j < AWB_SAMPLES; j++) {
				rg = awb->axis_rg[0] + (awb->axis_rg[AWB_GRID - 1] -
					awb->axis_rg[0]) * j / (AWB_SAMPLES - 1);
				w = grid_lookup(awb, awb->weight, rg, bg);
				if (w < 0.5)
					continue;
				m = grid_lookup(awb, awb->mired, rg, bg);
				if (pass == 0) {
					lo = fmin(lo, m);
					hi = fmax(hi, m);
					continue;
				}
				b = (unsigned int)((m - lo) / (hi - lo + 1e-9) * bands);
				if (b >= bands)
					b = bands - 1;
				if (pass == 1) {
					peak[b] = fmax(peak[b], w);
				} else if (w >= 0.9 * peak[b]) {
					sum_rg[b] += rg;
					sum_bg[b] += bg;
					sum_m[b] += m;
					n[b]++;
				}
			}
		}
	}
	awb->locus_points = 0;
	for (b = 0; b < bands; b++) {
		struct ipa_awb_locus_point *p;

		if (n[b] < 3)
			continue;
		p = &awb->locus[awb->locus_points++];
		p->rg = sum_rg[b] / n[b];
		p->bg = sum_bg[b] / n[b];
		p->mired = sum_m[b] / n[b];
	}
	/* Light three-tap smoothing; the end points stay fixed. */
	for (i = 1; i + 1 < awb->locus_points; i++) {
		awb->locus[i].rg = (awb->locus[i - 1].rg + 2 * awb->locus[i].rg +
				    awb->locus[i + 1].rg) / 4;
		awb->locus[i].bg = (awb->locus[i - 1].bg + 2 * awb->locus[i].bg +
				    awb->locus[i + 1].bg) / 4;
	}
}

int ipa_awb_init(struct ipa_awb *awb, const struct ipa_tuning *t,
		 double tolerance)
{
	const int64_t *a = ipa_tuning_get(t, "awb", "awb_locus_a", AWB_GRID);
	const int64_t *b = ipa_tuning_get(t, "awb", "awb_locus_b", AWB_GRID);
	const int64_t *mult = ipa_tuning_get(t, "awb", "awb_cfg_0x110c", 2);
	const int64_t *min_count = ipa_tuning_get(t, "awb", "awb_cfg_0x10c4", 1);
	const int64_t *reject = ipa_tuning_get(t, "awb", "awb_cfg_0x1220", 3);
	const int64_t *ct = ipa_tuning_get(t, "awb", "awb_init_ct", 1);
	unsigned int i;

	if (!a || !b || !mult || !min_count || !reject || !ct)
		return -EINVAL;
	memset(awb, 0, sizeof(*awb));
	for (i = 0; i < AWB_GRID; i++) {
		awb->axis_rg[i] = a[i];
		awb->axis_bg[i] = b[i];
	}
	if (load_grid(t, "awb_zone_tbl_0x1cb8", awb->weight, 1.0 / Q8_ONE) ||
	    load_grid(t, "awb_zone_tbl_0x122c", awb->weight_dim, 1.0 / Q8_ONE) ||
	    load_grid(t, "awb_zone_tbl_0x15b0", awb->mired, 1.0) ||
	    load_grid(t, "awb_zone_tbl_0x1934", awb->spatial, 1.0))
		return -EINVAL;
	awb->mult_rg = mult[0] / Q10_ONE;
	awb->mult_bg = mult[1] / Q10_ONE;
	awb->min_count = min_count[0];
	if (reject[0]) {
		awb->reject_hi = reject[1] / Q10_ONE;
		awb->reject_lo = reject[2] / Q10_ONE;
	}
	awb->locus_tolerance = tolerance;
	awb->ct = ct[0];
	awb->gain_r = awb->gain_b = (uint16_t)Q10_ONE;
	build_locus(awb);
	return awb->locus_points >= 2 ? 0 : -EINVAL;
}

/* Nearest locus point to (rg, bg); returns the squared distance. */
static double locus_project(const struct ipa_awb *awb, double rg, double bg,
			    double *prg, double *pbg, double *mired)
{
	double best = 1e18;
	unsigned int i;

	*prg = awb->locus[0].rg;
	*pbg = awb->locus[0].bg;
	*mired = awb->locus[0].mired;
	for (i = 0; i + 1 < awb->locus_points; i++) {
		const struct ipa_awb_locus_point *p = &awb->locus[i], *q = p + 1;
		double dx = q->rg - p->rg, dy = q->bg - p->bg;
		double len = dx * dx + dy * dy;
		double u = len ? ((rg - p->rg) * dx + (bg - p->bg) * dy) / len : 0;
		double x, y, d;

		u = fmax(0, fmin(1, u));
		x = p->rg + u * dx;
		y = p->bg + u * dy;
		d = (rg - x) * (rg - x) + (bg - y) * (bg - y);
		if (d < best) {
			best = d;
			*prg = x;
			*pbg = y;
			*mired = p->mired + u * (q->mired - p->mired);
		}
	}
	return best;
}

static bool weighted_mean(const double *rg, const double *bg, const double *w,
			  double *mrg, double *mbg)
{
	double sw = 0, srg = 0, sbg = 0;
	unsigned int i;

	for (i = 0; i < T23_ISP_ZONES; i++) {
		sw += w[i];
		srg += w[i] * rg[i];
		sbg += w[i] * bg[i];
	}
	if (sw < 1e-3)
		return false;
	*mrg = srg / sw;
	*mbg = sbg / sw;
	return true;
}

void ipa_awb_run(struct ipa_awb *awb, const struct t23_isp_stat_buffer *stats,
		 double ev)
{
	double (*likelihood)[AWB_GRID] = ev < AWB_DIM_EV ? awb->weight :
						      awb->weight_dim;
	double rg[T23_ISP_ZONES], bg[T23_ISP_ZONES], w[T23_ISP_ZONES];
	double mrg, mbg, lrg, lbg, mired, d2, d;
	uint32_t max_count = 0;
	unsigned int i;

	if (!(stats->flags & T23_ISP_STATS_AWB))
		return;
	for (i = 0; i < T23_ISP_ZONES; i++)
		if (stats->awb[i].count > max_count)
			max_count = stats->awb[i].count;
	if (!max_count)
		return;
	for (i = 0; i < T23_ISP_ZONES; i++) {
		const struct t23_isp_awb_zone *z = &stats->awb[i];

		w[i] = 0;
		rg[i] = bg[i] = 0;
		if (z->count <= awb->min_count || !z->sum_g)
			continue;
		rg[i] = 256.0 * awb->mult_rg * z->sum_r / z->sum_g;
		bg[i] = 256.0 * awb->mult_bg * z->sum_b / z->sum_g;
		w[i] = grid_lookup(awb, likelihood, rg[i], bg[i]) *
		       z->count / max_count *
		       awb->spatial[i / AWB_GRID][i % AWB_GRID];
	}
	if (!weighted_mean(rg, bg, w, &mrg, &mbg))
		return;
	{
		double gw[T23_ISP_ZONES];

		for (i = 0; i < T23_ISP_ZONES; i++)
			gw[i] = rg[i] > 0 ? stats->awb[i].count : 0;
		weighted_mean(rg, bg, gw, &awb->dbg_grey_rg, &awb->dbg_grey_bg);
	}

	if (awb->reject_hi > awb->reject_lo) {
		double dist[T23_ISP_ZONES], sw = 0, sd = 0;

		for (i = 0; i < T23_ISP_ZONES; i++) {
			dist[i] = (rg[i] - mrg) * (rg[i] - mrg) +
				  (bg[i] - mbg) * (bg[i] - mbg);
			sw += w[i];
			sd += w[i] * dist[i];
		}
		if (sd > 0) {
			for (i = 0; i < T23_ISP_ZONES; i++) {
				double q = dist[i] / (sd / sw);

				if (q > awb->reject_hi)
					w[i] = 0;
				else if (q > awb->reject_lo)
					w[i] *= 1 - (q - awb->reject_lo) /
						(awb->reject_hi - awb->reject_lo);
			}
			if (!weighted_mean(rg, bg, w, &mrg, &mbg))
				return;
		}
	}

	awb->dbg_mean_rg = mrg;
	awb->dbg_mean_bg = mbg;
	/* Constrain to the locus, keeping at most the tolerated tint. */
	d2 = locus_project(awb, mrg, mbg, &lrg, &lbg, &mired);
	d = sqrt(d2);
	if (d > awb->locus_tolerance) {
		mrg = lrg + (mrg - lrg) * awb->locus_tolerance / d;
		mbg = lbg + (mbg - lbg) * awb->locus_tolerance / d;
	}

	if (awb->valid) {
		awb->rg += (mrg - awb->rg) * AWB_SMOOTHING;
		awb->bg += (mbg - awb->bg) * AWB_SMOOTHING;
		awb->ct += (1e6 / mired - awb->ct) * AWB_SMOOTHING;
	} else {
		awb->rg = mrg;
		awb->bg = mbg;
		awb->ct = 1e6 / mired;
		awb->valid = true;
	}
	/* Gain = G/R in Q10; the grid multipliers cancel. */
	awb->gain_r = (uint16_t)fmin(0x3fff, Q10_ONE * Q8_ONE * awb->mult_rg / awb->rg + 0.5);
	awb->gain_b = (uint16_t)fmin(0x3fff, Q10_ONE * Q8_ONE * awb->mult_bg / awb->bg + 0.5);
}

/*
 * Grey world over every counted zone, for infrared scenes. The output is
 * monochrome at night; equal channels keep the demosaic free of pattern.
 */
void ipa_awb_grey(struct ipa_awb *awb, const struct t23_isp_stat_buffer *stats)
{
	double r = 0, g = 0, b = 0;
	unsigned int i;

	if (!(stats->flags & T23_ISP_STATS_AWB))
		return;
	for (i = 0; i < T23_ISP_ZONES; i++) {
		r += stats->awb[i].sum_r;
		g += stats->awb[i].sum_g;
		b += stats->awb[i].sum_b;
	}
	if (r <= 0 || g <= 0 || b <= 0)
		return;
	awb->gain_r = lround(awb->gain_r + AWB_SMOOTHING *
			     (fmin(0x3fff, Q10_ONE * g / r) - awb->gain_r));
	awb->gain_b = lround(awb->gain_b + AWB_SMOOTHING *
			     (fmin(0x3fff, Q10_ONE * g / b) - awb->gain_b));
}
