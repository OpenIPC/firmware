/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Daylight tuning for the GC1054 module.
 *
 * Gain-scheduled tables hold one entry per octave of total gain, starting
 * at 1x, and are interpolated linearly inside an octave; beyond the last
 * entry the last value holds. Noise in a Bayer sensor grows with gain, so
 * noise reduction strength and the noise thresholds rise with it. The
 * sensor black pedestal drifts with analogue gain; the per-channel black
 * levels follow a table indexed by the analogue gain alone.
 *
 * The same tables run on into low light. Luma noise reduction is
 * light at 1x and grows from there; chroma noise reduction stays off up to
 * 4x; edge enhancement is lowered in steps and cut back hard from 256x, so
 * that amplified noise is not sharpened; saturation falls with gain and the
 * picture is nearly monochrome from 128x, where colour is mostly noise.
 *
 * The colour correction matrix is chosen by illuminant. Four matrices are
 * calibrated at four reference white points, expressed as the B/G and R/G
 * ratios of a neutral surface (Q12). The current white point follows from
 * the white-balance gains; the matrix is blended along the segment of the
 * reference locus closest to it.
 */
#include "isp-tuning.h"
#include "awb-controller.h"
#include "linux/fh8626-isp.h"

#define OCTAVES 8

/*
 * Black pedestal (12-bit) per Bayer site, R, Gr, Gb, B, one entry per
 * octave of analogue gain from 1x to 32x, at 20, 40 and 80 ms of
 * integration, from GC1054 dark frames: the pedestal falls with analogue
 * gain and with integration time, the Gb and B sites faster. ISP
 * digital gain does not move it. Between the times the pedestal is
 * interpolated linearly; outside them the nearest time holds.
 */
#define BLACK_STEPS 6
#define BLACK_TIMES 3
static const uint32_t black_time_us[BLACK_TIMES] = { 20000, 40000, 80000 };
static const int32_t black_level[BLACK_TIMES][4][BLACK_STEPS] = {
	{
		{ 129, 129, 129, 128, 127, 123 },
		{ 129, 129, 129, 128, 127, 122 },
		{ 129, 129, 129, 127, 126, 120 },
		{ 129, 129, 129, 126, 124, 118 },
	}, {
		{ 129, 129, 130, 127, 123, 116 },
		{ 129, 129, 130, 127, 123, 115 },
		{ 129, 129, 130, 125, 121, 111 },
		{ 129, 129, 129, 123, 118, 106 },
	}, {
		{ 129, 128, 126, 119, 111, 92 },
		{ 129, 128, 127, 121, 112, 94 },
		{ 128, 126, 124, 118, 107, 87 },
		{ 128, 126, 123, 115, 101, 76 },
	},
};

/*
 * Temporal NR: strength scale, temporal blend and motion weight, for the
 * day and the low-light profile. The blend is the share of the current
 * frame: a higher value filters less over time. On a static flat field the
 * residual temporal noise is flat for blends up to 32 and rises above 64;
 * the motion weight is best near 32. In a static scene the blend stays at
 * its table value; with scene motion it rises towards NR3D_BLEND_MOTION so
 * that moving objects leave no trails, and the spatial filters take over
 * part of the noise budget.
 */
static const int32_t nr3d_scale[2][OCTAVES] = {
	{ 2, 3, 4, 6, 8, 10, 16, 20 },
	{ 2, 3, 4, 5, 12, 16, 16, 16 },
};
static const int32_t nr3d_blend[2][OCTAVES] = {
	{ 16, 16, 16, 16, 16, 20, 26, 32 },
	{ 28, 28, 28, 28, 28, 28, 32, 32 },
};
static const int32_t nr3d_motion[OCTAVES] = { 32, 32, 32, 32, 32, 32, 32, 32 };
static const int32_t nr3d_noise_slope[2] = { 8, 6 };
static const int32_t nr3d_noise_base[2] = { 8, 6 };
#define NR3D_STRENGTH_SLOPE	6
#define NR3D_STRENGTH_BASE	6
#define NR3D_NOISE_SHIFT	6
#define NR3D_PRESET		4
#define NR3D_BLEND_MOTION	96

/*
 * Low-light profile: entered after 25 frames at a total gain of 16x or
 * more, left after 25 frames at 6x or less. Besides its temporal NR tables
 * it lowers saturation by 15%, as chroma noise grows faster than the
 * colour it carries.
 */
#define LOW_LIGHT_ENTER		(16 * 64)
#define LOW_LIGHT_LEAVE		(6 * 64)
#define LOW_LIGHT_FRAMES	25
#define LOW_LIGHT_SAT_Q6	54

/* Bayer NR: strength scale. */
static const int32_t bnr_scale[OCTAVES] = { 2, 4, 5, 8, 12, 16, 20, 24 };
#define BNR_STRENGTH_SLOPE	16
#define BNR_STRENGTH_BASE	16
#define BNR_NOISE_SLOPE		64
#define BNR_NOISE_BASE		64
#define BNR_NOISE_SHIFT		9

/* Low-light tables, one entry per octave from 1x to 256x. */
#define LL_STEPS 9

/*
 * Luma NR: band strengths (Q6) and the noise levels at mid-dark and
 * mid-grey. The thresholds stay fixed; the strengths and levels carry the
 * gain dependence in both profiles, so the filter keeps growing with gain:
 * the two coarser bands follow the finest one from 2x and the noise levels
 * track the amplified sensor noise, which the edge enhancement would
 * otherwise sharpen into speckle.
 */
static const uint16_t ynr_threshold[3] = { 16, 16, 20 };
static const int32_t ynr_str0[LL_STEPS] = { 6, 10, 16, 24, 36, 48, 56, 64, 128 };
static const int32_t ynr_str12[LL_STEPS] = { 0, 4, 8, 16, 32, 48, 56, 64, 64 };
static const int32_t ynr_noise_dark[LL_STEPS] = { 5, 10, 12, 16, 24, 32, 36, 40, 47 };
static const int32_t ynr_noise_grey[LL_STEPS] = { 3, 8, 14, 20, 30, 40, 44, 48, 52 };

/* Chroma NR: strength and the lower/upper difference thresholds. */
static const int32_t cnr_strength[LL_STEPS] = { 0, 0, 0, 5, 16, 48, 64, 64, 64 };
static const int32_t cnr_th_lo[LL_STEPS] = { 0, 0, 0, 0, 0, 4, 6, 10, 16 };
static const int32_t cnr_th_hi[LL_STEPS] = { 0, 0, 0, 0, 0, 8, 10, 16, 24 };

/* Edge enhancement limits and gains, detail path then edge path. */
static const int32_t apc_str_detail[LL_STEPS] = { 112, 112, 96, 96, 80, 80, 80, 80, 12 };
static const int32_t apc_str_edge[LL_STEPS] = { 170, 170, 160, 160, 150, 150, 150, 150, 52 };
static const int32_t apc_gain_detail[LL_STEPS] = { 7, 7, 6, 6, 5, 4, 4, 3, 3 };
static const int32_t apc_gain_edge[LL_STEPS] = { 8, 8, 7, 7, 6, 5, 5, 4, 4 };

/* Saturation policy scale, Q6. */
static const int32_t sat_scale[LL_STEPS] = { 72, 68, 65, 62, 58, 55, 36, 8, 8 };

/* One matrix per reference white point of awb_locus_q12, warm to cool. */
static const int32_t ccm_mat[AWB_LOCUS_POINTS][9] = {
	{ 435, 26, 51, -127, 640, 0, -41, -65, 618 },
	{ 512, 0, 0, -122, 666, -41, 41, -36, 507 },
	{ 509, -58, 61, -92, 655, -61, 47, 0, 465 },
	{ 445, -9, 76, -91, 645, -51, 11, 13, 487 },
};

/*
 * Highlight chroma threshold. A raw channel clips at a white-balanced level
 * proportional to its gain, so once a highlight passes the level where the
 * lowest-gain channel clips, its hue drifts towards the gain ratios. The
 * threshold weights each channel's share of luma (BT.601, Q10) by
 * min(gain) / gain[c]: full scale for a neutral balance, lower as the gains
 * spread apart.
 */
static const int32_t luma_q10[3] = { 306, 601, 116 };

static int32_t hlr_threshold(uint32_t gr, uint32_t gg, uint32_t gb)
{
	uint32_t g[3] = { gr, gg, gb };
	uint32_t gmin = gr;
	int64_t sum = 0;
	unsigned int i;

	if (gg < gmin)
		gmin = gg;
	if (gb < gmin)
		gmin = gb;
	for (i = 0; i < 3; i++)
		sum += (int64_t)luma_q10[i] * gmin / g[i];
	return clamp_i32((int32_t)sum, 0, FH8626_ISP_HLR_THRESHOLD_MAX);
}

/*
 * Luma tone curve, 10-bit output at the hardware's 160 curve points.
 * Not const: VIDIOC_S_EXT_CTRLS copies the applied value back.
 */
static uint16_t gamma_y[FH8626_ISP_GAMMA_POINTS] = {
	0, 3, 7, 10, 13, 16, 19, 22, 25, 28, 31, 33, 36, 39, 41, 44,
	46, 49, 51, 53, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78,
	80, 81, 83, 85, 86, 88, 90, 92, 94, 95, 97, 98, 100, 102, 103, 105,
	106, 108, 109, 111, 113, 114, 115, 117, 119, 120, 121, 123, 124, 125, 127, 128,
	130, 135, 141, 146, 151, 156, 161, 166, 171, 176, 180, 185, 190, 195, 199, 203,
	208, 213, 217, 222, 226, 230, 234, 238, 242, 247, 251, 256, 259, 264, 268, 272,
	276, 280, 283, 288, 292, 296, 300, 304, 308, 312, 316, 320, 324, 327, 331, 335,
	339, 354, 370, 385, 400, 415, 430, 444, 459, 474, 488, 503, 517, 532, 546, 560,
	575, 589, 603, 618, 632, 646, 660, 674, 688, 702, 716, 731, 745, 759, 773, 787,
	801, 815, 829, 843, 857, 871, 885, 898, 913, 927, 941, 954, 968, 982, 996, 1010,
};

static int32_t interp_n(const int32_t *tab, unsigned int n, uint32_t gain)
{
	unsigned int oct = 0;
	uint32_t lo, frac;

	if (gain < 64)
		gain = 64;
	while (oct < n - 1 && gain >= (128U << oct))
		oct++;
	if (oct == n - 1)
		return tab[oct];
	lo = 64U << oct;
	frac = (uint32_t)((uint64_t)(gain - lo) * 256 / lo);
	return (tab[oct] * (int32_t)(256 - frac) + tab[oct + 1] * (int32_t)frac + 128) >> 8;
}

static int32_t black_for(unsigned int site, uint32_t again, uint32_t exposure_us)
{
	unsigned int k = 0;
	int32_t lo, hi;
	uint32_t span;

	if (exposure_us <= black_time_us[0])
		return interp_n(black_level[0][site], BLACK_STEPS, again);
	while (k < BLACK_TIMES - 2 && exposure_us > black_time_us[k + 1])
		k++;
	if (exposure_us >= black_time_us[k + 1])
		return interp_n(black_level[k + 1][site], BLACK_STEPS, again);
	lo = interp_n(black_level[k][site], BLACK_STEPS, again);
	hi = interp_n(black_level[k + 1][site], BLACK_STEPS, again);
	span = black_time_us[k + 1] - black_time_us[k];
	return lo + (int32_t)((int64_t)(hi - lo) * (exposure_us - black_time_us[k]) / span);
}

static int32_t interp(const int32_t tab[OCTAVES], uint32_t gain)
{
	return interp_n(tab, OCTAVES, gain);
}

static int32_t interp_ll(const int32_t tab[LL_STEPS], uint32_t gain)
{
	return interp_n(tab, LL_STEPS, gain);
}

/* Queue a U16 array control when it differs from the cached value. */
static void queue_u16(struct v4l2_ext_control *c, unsigned int *n, bool force,
		      uint32_t id, uint16_t *cache, const uint16_t *val,
		      unsigned int count)
{
	if (!force && !memcmp(cache, val, count * sizeof(*val)))
		return;
	memcpy(cache, val, count * sizeof(*val));
	c[(*n)++] = (struct v4l2_ext_control){
		.id = id, .size = count * sizeof(*val), .p_u16 = cache,
	};
}

static bool has_ctrl(int fd, uint32_t id)
{
	struct v4l2_queryctrl q = { .id = id };

	return ioctl(fd, VIDIOC_QUERYCTRL, &q) == 0 &&
	       !(q.flags & (V4L2_CTRL_FLAG_DISABLED | V4L2_CTRL_FLAG_READ_ONLY));
}

int isp_tuning_init(struct isp_tuning *t, int isp_fd)
{
	struct v4l2_ext_control c = {
		.id = V4L2_CID_FH8626_GAMMA_Y, .size = sizeof(gamma_y),
		.ptr = gamma_y,
	};
	struct v4l2_ext_controls ctrls = {
		.ctrl_class = V4L2_CTRL_CLASS_USER, .count = 1, .controls = &c,
	};

	memset(t, 0, sizeof(*t));
	t->fd = isp_fd;
	t->have_black = has_ctrl(isp_fd, V4L2_CID_FH8626_BLACK_LEVEL);
	t->have_nr3d = has_ctrl(isp_fd, V4L2_CID_FH8626_NR3D_NOISE) &&
		       has_ctrl(isp_fd, V4L2_CID_FH8626_NR3D_PRESET);
	t->have_bnr = has_ctrl(isp_fd, V4L2_CID_FH8626_BNR_STRENGTH);
	t->have_ccm = has_ctrl(isp_fd, V4L2_CID_FH8626_CCM);
	t->have_hlr = has_ctrl(isp_fd, V4L2_CID_FH8626_HLR_THRESHOLD);
	t->have_ynr = has_ctrl(isp_fd, V4L2_CID_FH8626_YNR_THRESHOLD) &&
		      has_ctrl(isp_fd, V4L2_CID_FH8626_YNR_STRENGTH) &&
		      has_ctrl(isp_fd, V4L2_CID_FH8626_YNR_NOISE);
	t->have_cnr = has_ctrl(isp_fd, V4L2_CID_FH8626_CNR_STRENGTH) &&
		      has_ctrl(isp_fd, V4L2_CID_FH8626_CNR_THRESHOLD);
	t->have_apc = has_ctrl(isp_fd, V4L2_CID_FH8626_APC_STRENGTH) &&
		      has_ctrl(isp_fd, V4L2_CID_FH8626_APC_GAIN);
	t->have_sat = has_ctrl(isp_fd, V4L2_CID_FH8626_SATURATION_SCALE);
	if (has_ctrl(isp_fd, V4L2_CID_FH8626_GAMMA_Y) &&
	    ioctl(isp_fd, VIDIOC_S_EXT_CTRLS, &ctrls) < 0) {
		LOG_WARN("Luma tone curve: %s", strerror(errno));
		return -1;
	}
	LOG_INFO("ISP tuning: black=%d nr3d=%d bnr=%d ynr=%d cnr=%d apc=%d sat=%d ccm=%d hlr=%d",
		 t->have_black, t->have_nr3d, t->have_bnr, t->have_ynr,
		 t->have_cnr, t->have_apc, t->have_sat, t->have_ccm,
		 t->have_hlr);
	return 0;
}

/* Nearest reference segment a-b to (x, y) and the blend weight (0..256) along it. */
static void ccm_locate(int32_t x, int32_t y, unsigned int *a, unsigned int *b,
		       uint32_t *w)
{
	int64_t best = INT64_MAX;
	unsigned int i;

	for (i = 0; i + 1 < AWB_LOCUS_POINTS; i++) {
		int64_t dx = awb_locus_q12[i + 1][0] - awb_locus_q12[i][0];
		int64_t dy = awb_locus_q12[i + 1][1] - awb_locus_q12[i][1];
		int64_t px = x - awb_locus_q12[i][0], py = y - awb_locus_q12[i][1];
		int64_t len = dx * dx + dy * dy;
		int64_t dot = px * dx + py * dy;
		int64_t t = len ? dot * 256 / len : 0;
		int64_t ex, ey, dist;

		if (t < 0)
			t = 0;
		if (t > 256)
			t = 256;
		ex = px - dx * t / 256;
		ey = py - dy * t / 256;
		dist = ex * ex + ey * ey;
		if (dist < best) {
			best = dist;
			*a = i;
			*b = i + 1;
			*w = (uint32_t)t;
		}
	}
}

/* Low-light profile with hysteresis on the total gain. */
static void update_profile(struct isp_tuning *t, uint32_t gain)
{
	bool cross = t->low_light ? gain <= LOW_LIGHT_LEAVE : gain >= LOW_LIGHT_ENTER;

	if (!cross) {
		t->profile_count = 0;
		return;
	}
	if (++t->profile_count < LOW_LIGHT_FRAMES)
		return;
	t->low_light = !t->low_light;
	t->profile_count = 0;
	LOG_INFO("ISP tuning: %s profile at gain %u (Q6)",
		 t->low_light ? "low-light" : "day", gain);
}

/* Scale a strength by a Q8 factor, within the control range. */
static int32_t scale_q8(int32_t v, uint32_t f, int32_t max)
{
	return clamp_i32((int32_t)(((int64_t)v * f + 128) >> 8), 0, max);
}

void isp_tuning_update(struct isp_tuning *t, uint32_t gain, uint32_t again,
		       uint32_t exposure_us, uint32_t gain_r, uint32_t gain_g,
		       uint32_t gain_b, uint32_t motion)
{
	struct v4l2_ext_control c[24];
	struct v4l2_ext_controls ctrls = {
		.ctrl_class = V4L2_CTRL_CLASS_USER, .controls = c,
	};
	uint16_t black[4], u16v[3];
	int32_t nr3d[6], bnr[3], ccm[9];
	uint64_t g2;
	unsigned int n = 0, i, a = 0, b = 1, p;
	uint32_t w = 0, bnr_f, ynr_f;
	int32_t s;

	if (gain < 64)
		gain = 64;
	g2 = (uint64_t)gain * gain;
	/* Quantise the motion level so that noise in it does not rewrite. */
	motion = motion > 256 ? 256 : motion & ~15U;
	t->motion = motion;
	update_profile(t, gain);
	p = t->low_light;
	/*
	 * Spatial budget: with motion the temporal filter does less, so the
	 * Bayer filter grows by up to 50% and the luma filter by up to 25%.
	 * In a static scene above 8x the temporal filter carries the noise and
	 * the luma filter is 15% lighter, which keeps detail.
	 */
	bnr_f = 256 + motion / 2;
	ynr_f = gain >= 512 ? 218 + motion * 102 / 256 : 256 + motion / 4;

	if (t->have_black) {
		for (i = 0; i < 4; i++)
			black[i] = (uint16_t)black_for(i, again, exposure_us);
		if (!t->written || memcmp(black, t->black, sizeof(black))) {
			memcpy(t->black, black, sizeof(black));
			c[n++] = (struct v4l2_ext_control){
				.id = V4L2_CID_FH8626_BLACK_LEVEL,
				.size = sizeof(t->black), .p_u16 = t->black,
			};
		}
	}

	if (t->have_nr3d) {
		static const uint32_t ids[6] = {
			V4L2_CID_FH8626_NR3D_STRENGTH, V4L2_CID_FH8626_NR3D_NOISE,
			V4L2_CID_FH8626_NR3D_NOISE_SHIFT, V4L2_CID_FH8626_NR3D_MOTION,
			V4L2_CID_FH8626_NR3D_BLEND, V4L2_CID_FH8626_NR3D_PRESET,
		};

		s = interp(nr3d_scale[p], gain);
		nr3d[0] = ((NR3D_STRENGTH_BASE +
			    (int32_t)((gain * NR3D_STRENGTH_SLOPE + 0x200) >> 10)) * s + 8) >> 4;
		nr3d[0] = clamp_i32(nr3d[0], 1, 255);
		nr3d[1] = (int32_t)(((nr3d_noise_slope[p] * g2 + 0x80000) >> 20) +
				    nr3d_noise_base[p] + 8) >> 4;
		nr3d[1] = clamp_i32(nr3d[1], 0, 0x7ffff);
		nr3d[2] = NR3D_NOISE_SHIFT;
		nr3d[3] = interp(nr3d_motion, gain);
		s = interp(nr3d_blend[p], gain);
		if (s < NR3D_BLEND_MOTION)
			s += (NR3D_BLEND_MOTION - s) * (int32_t)motion / 256;
		nr3d[4] = s;
		nr3d[5] = NR3D_PRESET;
		for (i = 0; i < 6; i++)
			if (!t->written || nr3d[i] != t->nr3d[i])
				c[n++] = (struct v4l2_ext_control){
					.id = ids[i], .value = nr3d[i],
				};
		memcpy(t->nr3d, nr3d, sizeof(nr3d));
	}

	if (t->have_bnr) {
		static const uint32_t ids[3] = {
			V4L2_CID_FH8626_BNR_STRENGTH, V4L2_CID_FH8626_BNR_NOISE,
			V4L2_CID_FH8626_BNR_NOISE_SHIFT,
		};

		s = interp(bnr_scale, gain);
		bnr[0] = scale_q8((s * (BNR_STRENGTH_BASE +
				   (int32_t)((gain * BNR_STRENGTH_SLOPE + 0x800) >> 12)) + 8) >> 4,
				  bnr_f, 255);
		bnr[1] = clamp_i32((s * ((int32_t)((BNR_NOISE_SLOPE * g2 + 0x200000) >> 22) +
				    BNR_NOISE_BASE) + 8) >> 4, 0, 0x7ffff);
		bnr[2] = BNR_NOISE_SHIFT;
		for (i = 0; i < 3; i++)
			if (!t->written || bnr[i] != t->bnr[i])
				c[n++] = (struct v4l2_ext_control){
					.id = ids[i], .value = bnr[i],
				};
		memcpy(t->bnr, bnr, sizeof(bnr));
	}

	if (t->have_ynr) {
		int32_t dark = interp_ll(ynr_noise_dark, gain);
		int32_t grey = interp_ll(ynr_noise_grey, gain);

		queue_u16(c, &n, !t->written, V4L2_CID_FH8626_YNR_THRESHOLD,
			  t->ynr_th, ynr_threshold, 3);
		u16v[0] = (uint16_t)scale_q8(interp_ll(ynr_str0, gain), ynr_f, 1023);
		u16v[1] = (uint16_t)scale_q8(interp_ll(ynr_str12, gain), ynr_f, 1023);
		u16v[2] = u16v[1];
		queue_u16(c, &n, !t->written, V4L2_CID_FH8626_YNR_STRENGTH,
			  t->ynr_str, u16v, 3);
		/* The level at mid-grey is never below the mid-dark level. */
		u16v[0] = (uint16_t)dark;
		u16v[1] = (uint16_t)(grey > dark ? grey : dark);
		queue_u16(c, &n, !t->written, V4L2_CID_FH8626_YNR_NOISE,
			  t->ynr_noise, u16v, 2);
	}

	if (t->have_cnr) {
		s = interp_ll(cnr_strength, gain);
		if (!t->written || s != t->cnr_str) {
			t->cnr_str = s;
			c[n++] = (struct v4l2_ext_control){
				.id = V4L2_CID_FH8626_CNR_STRENGTH, .value = s,
			};
		}
		u16v[0] = (uint16_t)interp_ll(cnr_th_lo, gain);
		u16v[1] = (uint16_t)interp_ll(cnr_th_hi, gain);
		queue_u16(c, &n, !t->written, V4L2_CID_FH8626_CNR_THRESHOLD,
			  t->cnr_th, u16v, 2);
	}

	if (t->have_apc) {
		u16v[0] = (uint16_t)interp_ll(apc_str_detail, gain);
		u16v[1] = (uint16_t)interp_ll(apc_str_edge, gain);
		queue_u16(c, &n, !t->written, V4L2_CID_FH8626_APC_STRENGTH,
			  t->apc_str, u16v, 2);
		u16v[0] = (uint16_t)interp_ll(apc_gain_detail, gain);
		u16v[1] = (uint16_t)interp_ll(apc_gain_edge, gain);
		queue_u16(c, &n, !t->written, V4L2_CID_FH8626_APC_GAIN,
			  t->apc_gain, u16v, 2);
	}

	if (t->have_sat) {
		s = interp_ll(sat_scale, gain);
		if (t->low_light)
			s = (s * LOW_LIGHT_SAT_Q6 + 32) >> 6;
		if (!t->written || s != t->sat) {
			t->sat = s;
			c[n++] = (struct v4l2_ext_control){
				.id = V4L2_CID_FH8626_SATURATION_SCALE, .value = s,
			};
		}
	}

	if (t->have_ccm && gain_r && gain_g && gain_b) {
		/* A neutral surface reads B/G = gain_g / gain_b, R/G likewise. */
		ccm_locate((int32_t)(4096 * gain_g / gain_b),
			   (int32_t)(4096 * gain_g / gain_r), &a, &b, &w);
		for (i = 0; i < 9; i++)
			ccm[i] = ((256 - (int32_t)w) * ccm_mat[a][i] +
				  (int32_t)w * ccm_mat[b][i] + 128) >> 8;
		if (!t->written || memcmp(ccm, t->ccm, sizeof(ccm))) {
			memcpy(t->ccm, ccm, sizeof(ccm));
			t->ccm_a = a;
			t->ccm_b = b;
			t->ccm_weight = w;
			c[n++] = (struct v4l2_ext_control){
				.id = V4L2_CID_FH8626_CCM,
				.size = sizeof(t->ccm), .ptr = t->ccm,
			};
		}
	}

	if (t->have_hlr && gain_r && gain_g && gain_b) {
		int32_t hlr = hlr_threshold(gain_r, gain_g, gain_b);

		if (!t->written || hlr != t->hlr) {
			t->hlr = hlr;
			c[n++] = (struct v4l2_ext_control){
				.id = V4L2_CID_FH8626_HLR_THRESHOLD, .value = hlr,
			};
		}
	}

	if (!n)
		return;
	ctrls.count = n;
	if (ioctl(t->fd, VIDIOC_S_EXT_CTRLS, &ctrls) < 0) {
		LOG_WARN("ISP tuning write: %s", strerror(errno));
		t->written = false;
		return;
	}
	if (!t->written)
		LOG_INFO("ISP tuning gain=%u black=%u/%u/%u nr3d=%d/%d bnr=%d/%d ynr=%u/%u cnr=%d apc=%u/%u sat=%d ccm=%u-%u@%u hlr=%d",
			 gain, t->black[0], t->black[1], t->black[3],
			 t->nr3d[0], t->nr3d[1], t->bnr[0], t->bnr[1],
			 t->ynr_str[0], t->ynr_noise[0], t->cnr_str,
			 t->apc_str[0], t->apc_str[1], t->sat,
			 t->ccm_a, t->ccm_b, t->ccm_weight, t->hlr);
	else
		LOG_DBG("ISP tuning gain=%u motion=%u nr3d=%d/%d bnr=%d ynr=%u/%u/%u cnr=%d apc=%u/%u sat=%d",
			gain, t->motion, t->nr3d[0], t->nr3d[4], t->bnr[0],
			t->ynr_str[0], t->ynr_str[1], t->ynr_noise[0],
			t->cnr_str, t->apc_str[0], t->apc_str[1], t->sat);
	t->written = true;
}
