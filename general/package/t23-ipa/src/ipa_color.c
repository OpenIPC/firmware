/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Colour correction, gamma and lens shading from the tuning file.
 *
 * CCM: four matrices at the CT anchors, held within 200 K of an anchor and
 * blended linearly in kelvin between the held bands, then multiplied by a
 * BT.601 saturation matrix whose strength follows exposure.
 * LSC: A, T and D meshes blended by CT, then pulled toward unity by a
 * gain-dependent strength (4096 = full correction).
 */
#include <errno.h>
#include <string.h>
#include "t23_ipa.h"

#define CT_BAND		200.0

static double reg14(int64_t v)
{
	v &= 0x3fff;
	return v > 0x1fff ? v - 0x4000 : v;
}

int ipa_color_init(struct ipa_color *c, const struct ipa_tuning *t)
{
	static const char *const ccm[4] = {
		"ccm_a_linear", "ccm_t_linear", "ccm_d_linear", "ccm_d2_linear",
	};
	static const char *const lsc[3] = { "lsc_a_lut", "lsc_t_lut", "lsc_d_lut" };
	const int64_t *ct = ipa_tuning_get(t, "ccm", "ccm_ct_list", 4);
	const int64_t *ev = ipa_tuning_get(t, "ccm", "ccm_cm_ev_list", 9);
	const int64_t *sat = ipa_tuning_get(t, "ccm", "ccm_cm_sat_list", 9);
	const int64_t *gamma = ipa_tuning_get(t, "gamma", "gamma_lut",
					      T23_ISP_GAMMA_POINTS);
	const int64_t *lsc_ct = ipa_tuning_get(t, "lsc", "lsc_ct_points", 4);
	const int64_t *words = ipa_tuning_get(t, "lsc", "lsc_lut_num", 1);
	const int64_t *size = ipa_tuning_get(t, "lsc", "lsc_mesh_size", 2);
	const int64_t *stride = ipa_tuning_get(t, "lsc", "lsc_lut_stride", 1);
	const int64_t *scale = ipa_tuning_get(t, "lsc", "lsc_mesh_scale", 1);
	const int64_t *mean = ipa_tuning_get(t, "lsc", "lsc_mean_en", 1);
	unsigned int i, j;

	memset(c, 0, sizeof(*c));
	if (!ct || !ev || !sat || !gamma || !lsc_ct || !words || !size ||
	    !stride || !scale || !mean)
		return -EINVAL;
	for (i = 0; i < 4; i++) {
		const int64_t *m = ipa_tuning_get(t, "ccm", ccm[i], 9);

		if (!m)
			return -EINVAL;
		for (j = 0; j < 9; j++)
			c->ccm[i][j] = reg14(m[j]);
		c->ct_list[i] = ct[i];
		c->lsc_ct[i] = lsc_ct[i];
	}
	for (i = 0; i < 9; i++) {
		c->sat_ev[i] = ev[i];
		c->sat[i] = sat[i] / Q8_ONE;
	}
	for (i = 0; i < T23_ISP_GAMMA_POINTS; i++)
		c->gamma[i] = gamma[i];

	if (words[0] > T23_ISP_LSC_MAX_WORDS || words[0] % 3 || scale[0] > 3)
		return -EINVAL;
	c->lsc_words = words[0];
	for (i = 0; i < 3; i++) {
		c->lsc_lut[i] = ipa_tuning_get(t, "lsc", lsc[i], 0);
		if (!c->lsc_lut[i])
			return -EINVAL;
	}
	c->lsc_strength = ipa_tuning_get(t, "lsc", "lsc_mesh_str", 9);
	if (!c->lsc_strength)
		return -EINVAL;
	c->lsc_base.mesh_width = size[0];
	c->lsc_base.mesh_height = size[1];
	c->lsc_base.stride = stride[0];
	c->lsc_base.scale = scale[0];
	c->lsc_base.mean_en = mean[0];
	c->lsc_base.words = c->lsc_words;
	return 0;
}

static double saturation(const struct ipa_color *c, double ev)
{
	unsigned int i;

	if (ev <= c->sat_ev[0])
		return c->sat[0];
	for (i = 1; i < 9; i++)
		if (ev <= c->sat_ev[i])
			return c->sat[i - 1] + (c->sat[i] - c->sat[i - 1]) *
			       (ev - c->sat_ev[i - 1]) /
			       (c->sat_ev[i] - c->sat_ev[i - 1]);
	return c->sat[8];
}

void ipa_color_ccm(const struct ipa_color *c, double ct, double ev,
		   struct t23_isp_ccm_config *out)
{
	static const double luma[3] = { 0.299, 0.587, 0.114 };
	const double *l = c->ct_list;
	double m[9], s[9], sat = c->mono ? 0 : saturation(c, ev), k;
	unsigned int i, j, n, lo = 0;

	/* Pure anchors within the band, linear blends between the bands. */
	if (ct <= l[0] + CT_BAND) {
		memcpy(m, c->ccm[0], sizeof(m));
	} else if (ct >= l[3] - CT_BAND) {
		memcpy(m, c->ccm[3], sizeof(m));
	} else {
		for (lo = 0; lo < 2; lo++)
			if (ct < l[lo + 1] + CT_BAND)
				break;
		if (ct >= l[lo + 1] - CT_BAND) {
			memcpy(m, c->ccm[lo + 1], sizeof(m));
		} else {
			k = (ct - l[lo] - CT_BAND) / (l[lo + 1] - l[lo] - 2 * CT_BAND);
			for (i = 0; i < 9; i++)
				m[i] = c->ccm[lo][i] +
				       (c->ccm[lo + 1][i] - c->ccm[lo][i]) * k;
		}
	}

	/* M x S, with S scaling chroma about BT.601 luma */
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			s[i * 3 + j] = luma[j] + sat * ((i == j) - luma[j]);
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			double v = 0;

			for (n = 0; n < 3; n++)
				v += m[i * 3 + n] * s[n * 3 + j];
			v = v < 0 ? v - 0.5 : v + 0.5;
			out->coeff[i * 3 + j] = (int16_t)(v < -8192 ? -8192 :
							  v > 8191 ? 8191 : v);
		}
	}
	out->reserved = 0;
}

void ipa_color_gamma(const struct ipa_color *c, struct t23_isp_gamma_config *out)
{
	memcpy(out->curve, c->gamma, sizeof(out->curve));
	out->reserved = 0;
}

static uint32_t lsc_blend(uint32_t a, uint32_t b, double k, double strength,
			  uint32_t unity)
{
	uint32_t out = 0;
	unsigned int half;

	for (half = 0; half < 2; half++) {
		double va = (a >> (12 * half)) & 0xfff;
		double vb = (b >> (12 * half)) & 0xfff;
		double v = va + (vb - va) * k;

		v = unity + (v - unity) * strength;
		v = v < 0 ? 0 : v > 0xfff ? 0xfff : v + 0.5;
		out |= (uint32_t)v << (12 * half);
	}
	return out;
}

void ipa_color_lsc(const struct ipa_color *c, double ct,
		   uint32_t gain_log2_q16, struct t23_isp_lsc_config *out)
{
	static const uint32_t unity[4] = { 0x800, 0x400, 0x200, 0x100 };
	const double *p = c->lsc_ct;
	const int64_t *a, *b;
	double k = 0, strength = ipa_intp9(c->lsc_strength, gain_log2_q16) / Q12_ONE;
	unsigned int i;

	/* A below P0, A->T to P1, T to P2, T->D to P3, D above */
	if (ct <= p[0]) {
		a = b = c->lsc_lut[0];
	} else if (ct <= p[1]) {
		a = c->lsc_lut[0];
		b = c->lsc_lut[1];
		k = (ct - p[0]) / (p[1] - p[0]);
	} else if (ct <= p[2]) {
		a = b = c->lsc_lut[1];
	} else if (ct <= p[3]) {
		a = c->lsc_lut[1];
		b = c->lsc_lut[2];
		k = (ct - p[2]) / (p[3] - p[2]);
	} else {
		a = b = c->lsc_lut[2];
	}
	*out = c->lsc_base;
	for (i = 0; i < c->lsc_words; i++)
		out->lut[i] = lsc_blend(a[i], b[i], k, strength,
					unity[c->lsc_base.scale]);
}
