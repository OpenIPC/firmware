/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Noise reduction, sharpening and demosaic register words for the linear
 * (non-WDR) profile of the vendor ISP, as a function of total gain.
 *
 * Every word is the register value the vendor driver writes from the
 * gain-interpolated tuning parameters: the register expressions come from
 * the tuning parameter functions for noise reduction and demosaic.
 * I(x) is the tuning array "<module>_x" interpolated at the gain index
 * (interpolated over log2 total gain in Q16); S(x, i, n) is element i of
 * the static tuning field "<module>_x" holding n values. Both are unsigned
 * 32-bit, as in the vendor code, so shifts wrap the same way.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "t23_ipa.h"

struct dns_ctx {
	const struct ipa_tuning *t;
	uint32_t gain;		/* log2 of total gain, Q16 */
	const char *mod;	/* tuning module and field name prefix */
	int err;
};

static uint32_t dns_intp(struct dns_ctx *x, const char *name)
{
	char key[80];
	const int64_t *v;

	if (x->err)
		return 0;

	snprintf(key, sizeof(key), "%s_%s", x->mod, name);
	v = ipa_tuning_get(x->t, x->mod, key, 9);
	if (!v) {
		x->err = -ENOENT;
		return 0;
	}
	return (uint32_t)ipa_intp9(v, x->gain);
}

static uint32_t dns_static(struct dns_ctx *x, const char *name,
			   unsigned int index, unsigned int count)
{
	char key[80];
	const int64_t *v;

	if (x->err)
		return 0;

	snprintf(key, sizeof(key), "%s_%s", x->mod, name);
	v = ipa_tuning_get(x->t, x->mod, key, count);
	if (!v || index >= count) {
		x->err = -ENOENT;
		return 0;
	}
	return (uint32_t)v[index];
}

#define I(n)		dns_intp(x, #n)
#define S(n, i, cnt)	dns_static(x, #n, i, cnt)

/*
 * MDNS: Y/C 3D and 2D parameter configuration.
 * The block grid registers (except the statistics words) depend on the
 * frame size and are derived by the kernel.
 */
static int pack_mdns(struct dns_ctx *x, struct t23_isp_mdns_config *m)
{
	uint32_t v1, v2, v8;
	uint32_t bs;

	x->mod = "mdns";
	/* Top function configuration, filters enabled */
	m->top[0] = (S(y_sf_cur_en, 0, 1) << 4 | S(y_sf_ref_en, 0, 1) << 8 |
		     0x10001U | S(y_filter_en, 0, 1) << 12 |
		     S(uv_sf_cur_en, 0, 1) << 20 | S(uv_sf_ref_en, 0, 1) << 24 |
		     S(uv_filter_en, 0, 1) << 28 |
		     S(out_adj_mode_y, 0, 1) << 30 |
		     S(out_adj_mode_c, 0, 1) << 31) & 0xd1111111U;
	m->top[1] = (S(bgm_enable, 0, 1) << 4 | S(ref_wei_byps, 0, 1) << 8 |
		     S(sta_group_num, 0, 1) << 12 | S(psn_enable, 0, 1) << 16 |
		     S(psn_max_num, 0, 1) << 20 | I(y_con_thres)) & 0x00f1310fU;
	m->top[2] = 0;

	/* Y 3D parameters: block size rounded up to 4, at least 16 */
	bs = I(y_sta_blk_size);
	if (bs & 3)
		bs = ((bs >> 2) + 1) * 4;
	if ((int32_t)bs < 16)
		bs = 16;
	if (bs > 255)
		return -ERANGE;
	m->block_size = bs;

	v8 = S(sta_max_num, 0, 1) << 4 | S(sta_inter_en, 0, 1) |
		I(y_sta_motion_thres) << 8;
	m->sta_cfg = v8;
	v8 = I(y_sta_ave_thres) << 4 | I(y_sta_ass_thres) << 0x10 |
		I(y_sta_win_opt) | I(y_sta_dtb_thres) << 0x18;
	m->sta_thres = v8;
	m->val[30] = I(y_sad_ave_thres) << 3 | I(y_sad_ave_slope) << 0xc |
		I(y_sad_win_opt) | I(y_sad_ass_thres) << 0x10 |
		I(y_sad_dtb_thres) << 0x18;
	v8 = S(y_ref_wei_sta, 1, 16) << 8 | S(y_ref_wei_sta, 2, 16) << 0x10 |
		S(y_ref_wei_sta, 0, 16) | S(y_ref_wei_sta, 3, 16) << 0x18;
	m->val[31] = v8;
	m->val[35] = v8;
	m->val[39] = v8;
	m->val[43] = v8;
	v8 = S(y_ref_wei_sta, 5, 16) << 8 | S(y_ref_wei_sta, 6, 16) << 0x10 |
		S(y_ref_wei_sta, 4, 16) | S(y_ref_wei_sta, 7, 16) << 0x18;
	m->val[32] = v8;
	m->val[36] = v8;
	m->val[40] = v8;
	m->val[44] = v8;
	v8 = S(y_ref_wei_sta, 9, 16) << 8 | S(y_ref_wei_sta, 10, 16) << 0x10 |
		S(y_ref_wei_sta, 8, 16) | S(y_ref_wei_sta, 11, 16) << 0x18;
	m->val[33] = v8;
	m->val[37] = v8;
	m->val[41] = v8;
	m->val[45] = v8;
	v8 = S(y_ref_wei_sta, 13, 16) << 8 | S(y_ref_wei_sta, 14, 16) << 0x10 |
		S(y_ref_wei_sta, 12, 16) | S(y_ref_wei_sta, 15, 16) << 0x18;
	m->val[34] = v8;
	m->val[38] = v8;
	m->val[42] = v8;
	m->val[46] = v8;
	m->val[47] = S(y_ref_wei_psn, 1, 16) << 8 |
		S(y_ref_wei_psn, 2, 16) << 0x10 | S(y_ref_wei_psn, 0, 16) |
		S(y_ref_wei_psn, 3, 16) << 0x18;
	m->val[48] = S(y_ref_wei_psn, 5, 16) << 8 |
		S(y_ref_wei_psn, 6, 16) << 0x10 | S(y_ref_wei_psn, 4, 16) |
		S(y_ref_wei_psn, 7, 16) << 0x18;
	m->val[49] = S(y_ref_wei_psn, 9, 16) << 8 |
		S(y_ref_wei_psn, 10, 16) << 0x10 | S(y_ref_wei_psn, 8, 16) |
		S(y_ref_wei_psn, 11, 16) << 0x18;
	m->val[50] = S(y_ref_wei_psn, 13, 16) << 8 |
		S(y_ref_wei_psn, 14, 16) << 0x10 | S(y_ref_wei_psn, 12, 16) |
		S(y_ref_wei_psn, 15, 16) << 0x18;
	m->val[51] = I(y_ref_wei_sta_fs_opt) << 4 | I(y_ref_wei_fake) << 8 |
		I(y_ref_wei_psn_fs_opt) | I(y_ref_wei_mv) << 0x10;
	m->val[52] = I(y_ref_wei_f_min) << 8 | I(y_ref_wei_b_max) << 0x10 |
		I(y_ref_wei_f_max) | I(y_ref_wei_b_min) << 0x18;
	m->val[53] = I(y_ref_wei_r_min) << 8 | I(y_ref_wei_increase) << 0x10 |
		I(y_ref_wei_r_max);
	v1 = I(y_hist_sta_en);
	m->val[0] = v1 << 1 | v1 << 2 | I(y_hist_sad_en) | v1 << 3 | v1 << 4;
	m->val[3] = I(y_hist_thres1) << 0x10 | I(y_hist_thres0);
	m->val[4] = I(y_hist_thres3) << 0x10 | I(y_hist_thres2);
	m->val[1] = I(y_hist_cmp_thres1) << 0x10 | I(y_hist_cmp_thres0);
	m->val[2] = I(y_hist_cmp_thres3) << 0x10 | I(y_hist_cmp_thres2);
	m->val[5] = I(y_corner_length_b) << 8 | I(y_corner_length_l) << 0x10 |
		I(y_corner_length_t) | I(y_corner_length_r) << 0x18;
	m->val[6] = I(y_corner_thr_adj_value) << 8 | I(y_corner_wei_adj_value);
	m->val[7] = I(y_edge_div_opt) << 2 | I(y_edge_type_opt) << 4 |
		I(y_edge_win_opt) | I(y_edge_wei_adj_seg) << 8 |
		I(y_edge_thr_adj_seg) << 10;
	m->val[8] = I(y_edge_wei_adj_value1) << 8 |
		I(y_edge_wei_adj_value2) << 0x10 | I(y_edge_wei_adj_value0) |
		I(y_edge_wei_adj_value3) << 0x18;
	v1 = I(y_edge_wei_adj_value5);
	m->val[9] = v1 << 8 | v1 << 0x10 | I(y_edge_wei_adj_value4) |
		v1 << 0x18;
	m->val[10] = I(y_edge_thr_adj_value1) << 8 |
		I(y_edge_thr_adj_value2) << 0x10 | I(y_edge_thr_adj_value0) |
		I(y_edge_thr_adj_value3) << 0x18;
	v1 = I(y_edge_thr_adj_value5);
	m->val[11] = v1 << 8 | v1 << 0x10 | I(y_edge_thr_adj_value4) |
		v1 << 0x18;
	m->val[12] = I(y_luma_wei_adj_seg) << 8 | I(y_luma_thr_adj_seg) << 10 |
		I(y_luma_win_opt);
	m->val[13] = I(y_luma_wei_adj_value1) << 8 |
		I(y_luma_wei_adj_value2) << 0x10 | I(y_luma_wei_adj_value0) |
		I(y_luma_wei_adj_value3) << 0x18;
	v1 = I(y_luma_wei_adj_value5);
	m->val[14] = v1 << 8 | v1 << 0x10 | I(y_luma_wei_adj_value4) |
		v1 << 0x18;
	m->val[15] = I(y_luma_thr_adj_value1) << 8 |
		I(y_luma_thr_adj_value2) << 0x10 | I(y_luma_thr_adj_value0) |
		I(y_luma_thr_adj_value3) << 0x18;
	v1 = I(y_luma_thr_adj_value5);
	m->val[16] = v1 << 8 | v1 << 0x10 | I(y_luma_thr_adj_value4) |
		v1 << 0x18;
	m->val[17] = I(y_dtb_div_opt) << 2 | I(y_dtb_squ_en) << 4 |
		I(y_dtb_squ_div_opt) | I(y_dtb_wei_adj_seg) << 8 |
		I(y_dtb_thr_adj_seg) << 10;
	m->val[18] = I(y_dtb_wei_adj_value1) << 8 |
		I(y_dtb_wei_adj_value2) << 0x10 | I(y_dtb_wei_adj_value0) |
		I(y_dtb_wei_adj_value3) << 0x18;
	v1 = I(y_dtb_wei_adj_value5);
	m->val[19] = v1 << 8 | v1 << 0x10 | I(y_dtb_wei_adj_value4) |
		v1 << 0x18;
	m->val[20] = I(y_dtb_thr_adj_value1) << 8 |
		I(y_dtb_thr_adj_value2) << 0x10 | I(y_dtb_thr_adj_value0) |
		I(y_dtb_thr_adj_value3) << 0x18;
	v1 = I(y_dtb_thr_adj_value5);
	m->val[21] = v1 << 8 | v1 << 0x10 | I(y_dtb_thr_adj_value4) |
		v1 << 0x18;
	m->val[22] = I(y_ass_win_opt) << 4 | I(y_ass_wei_adj_seg) << 8 |
		I(y_ass_div_opt) | I(y_ass_thr_adj_seg) << 10;
	m->val[23] = I(y_ass_wei_adj_value1) << 8 |
		I(y_ass_wei_adj_value2) << 0x10 | I(y_ass_wei_adj_value0) |
		I(y_ass_wei_adj_value3) << 0x18;
	v1 = I(y_ass_wei_adj_value5);
	m->val[24] = v1 << 8 | v1 << 0x10 | I(y_ass_wei_adj_value4) |
		v1 << 0x18;
	m->val[25] = I(y_ass_thr_adj_value1) << 8 |
		I(y_ass_thr_adj_value2) << 0x10 | I(y_ass_thr_adj_value0) |
		I(y_ass_thr_adj_value3) << 0x18;
	v1 = I(y_ass_thr_adj_value5);
	m->val[26] = v1 << 8 | v1 << 0x10 | I(y_ass_thr_adj_value4) |
		v1 << 0x18;
	m->val[27] = I(y_sad_wei_adj_seg);
	m->val[28] = I(y_sad_wei_adj_value1) << 8 |
		I(y_sad_wei_adj_value2) << 0x10 | I(y_sad_wei_adj_value0) |
		I(y_sad_wei_adj_value3) << 0x18;
	v1 = I(y_sad_wei_adj_value5);
	m->val[29] = v1 << 8 | v1 << 0x10 | I(y_sad_wei_adj_value4) |
		v1 << 0x18;
	m->val[54] = I(y_pspa_cur_bi_wei_seg) << 8 |
		I(y_pspa_cur_lmt_op_en) << 10 | I(y_pspa_cur_bi_thres);
	v1 = I(y_pspa_cur_bi_wei0);
	m->val[55] = v1 << 4 | v1 << 8 | v1 | v1 << 0xc | v1 << 0x10 |
		v1 << 0x14;
	m->val[56] = I(y_pspa_ref_bi_wei_seg) << 8 |
		I(y_pspa_ref_lmt_op_en) << 10 | I(y_pspa_ref_bi_thres);
	v1 = I(y_pspa_ref_bi_wei0);
	m->val[57] = v1 << 4 | v1 << 8 | v1 | v1 << 0xc | v1 << 0x10 |
		v1 << 0x14;
	m->val[58] = I(y_pspa_ref_median_win_opt) << 1 |
		I(y_fspa_cur_fus_seg) << 4 | I(y_pspa_cur_median_win_opt) |
		I(y_fspa_ref_fus_seg) << 8 | I(y_pspa_fnl_fus_thres) << 0x10 |
		I(y_pspa_fnl_fus_dwei) << 0x18 |
		I(y_pspa_fnl_fus_swei) << 0x1c;
	v1 = I(y_fspa_cur_fus_wei_0);
	m->val[59] = v1 << 8 | v1 << 0x10 | v1 | v1 << 0x18;
	v1 = I(y_fspa_cur_fus_wei_0);
	m->val[60] = v1 << 8 | v1 << 0x10 | v1 | v1 << 0x18;
	v1 = I(y_fspa_cur_fus_wei_0);
	m->val[61] = v1 << 8 | v1 << 0x10 | v1 |
		I(y_fspa_cur_fus_wei_144) << 0x18;
	m->val[62] = I(y_fspa_cur_fus_wei_176) << 8 |
		I(y_fspa_cur_fus_wei_192) << 0x10 | I(y_fspa_cur_fus_wei_160) |
		I(y_fspa_cur_fus_wei_208) << 0x18;
	v1 = I(y_fspa_ref_fus_wei_0);
	m->val[63] = v1 << 8 | v1 << 0x10 | v1 | v1 << 0x18;
	v1 = I(y_fspa_ref_fus_wei_0);
	m->val[64] = v1 << 8 | v1 << 0x10 | v1 | v1 << 0x18;
	v1 = I(y_fspa_ref_fus_wei_0);
	m->val[65] = v1 << 8 | v1 << 0x10 | v1 |
		I(y_fspa_ref_fus_wei_144) << 0x18;
	m->val[66] = I(y_fspa_ref_fus_wei_176) << 8 |
		I(y_fspa_ref_fus_wei_192) << 0x10 | I(y_fspa_ref_fus_wei_160) |
		I(y_fspa_ref_fus_wei_208) << 0x18;
	m->val[67] = I(y_piir_edge_thres1) << 8 |
		I(y_piir_edge_thres2) << 0x10 | I(y_piir_edge_thres0);
	m->val[68] = I(y_piir_edge_wei1) << 8 | I(y_piir_edge_wei2) << 0x10 |
		I(y_piir_edge_wei0) | I(y_piir_edge_wei3) << 0x18;
	m->val[69] = I(y_piir_ref_fs_wei) << 8 | I(y_piir_cur_fs_wei);
	m->val[70] = I(y_piir_edge_thres1) << 8 |
		I(y_piir_edge_thres2) << 0x10 | I(y_piir_edge_thres0);
	m->val[71] = I(y_piir_edge_wei1) << 8 | I(y_piir_edge_wei2) << 0x10 |
		I(y_piir_edge_wei0) | I(y_piir_edge_wei3) << 0x18;
	m->val[72] = I(y_fiir_fus_seg);
	v1 = I(y_fiir_fus_wei0);
	v1 = v1 << 8 | v1 << 0x10 | v1 | v1 << 0x18;
	m->val[73] = v1;
	m->val[74] = v1;
	m->val[75] = I(y_fiir_fus_wei2) << 8 | I(y_fiir_fus_wei3) << 0x10 |
		I(y_fiir_fus_wei1) | I(y_fiir_fus_wei4) << 0x18;
	m->val[76] = I(y_fiir_fus_wei6) << 8 | I(y_fiir_fus_wei7) << 0x10 |
		I(y_fiir_fus_wei5) | I(y_fiir_fus_wei8) << 0x18;
	m->val[77] = I(y_con_stren) << 8;
	m->val[102] = I(c_sad_ave_thres) << 3 | I(c_sad_ave_slope) << 0xc |
		I(c_sad_win_opt) | I(c_sad_ass_thres) << 0x10 |
		I(c_sad_dtb_thres) << 0x18;
	m->val[103] = I(c_ref_wei_fake) << 8 | I(c_ref_wei_mv) << 0x10;
	m->val[104] = I(c_ref_wei_f_min) << 8 | I(c_ref_wei_b_max) << 0x10 |
		I(c_ref_wei_f_max) | I(c_ref_wei_b_min) << 0x18;
	m->val[105] = I(c_ref_wei_r_min) << 8 | I(c_ref_wei_increase) << 0x10 |
		I(c_ref_wei_r_max);
	m->val[78] = I(c_corner_thr_adj_value) << 8 |
		I(c_corner_wei_adj_value);
	m->val[79] = I(c_edge_wei_adj_seg) << 8 | I(c_edge_thr_adj_seg) << 10;
	m->val[80] = I(c_edge_wei_adj_value1) << 8 |
		I(c_edge_wei_adj_value2) << 0x10 | I(c_edge_wei_adj_value0) |
		I(c_edge_wei_adj_value3) << 0x18;
	v1 = I(c_edge_wei_adj_value5);
	m->val[81] = v1 << 8 | v1 << 0x10 | I(c_edge_wei_adj_value4) |
		v1 << 0x18;
	m->val[82] = I(c_edge_thr_adj_value1) << 8 |
		I(c_edge_thr_adj_value2) << 0x10 | I(c_edge_thr_adj_value0) |
		I(c_edge_thr_adj_value3) << 0x18;
	v1 = I(c_edge_thr_adj_value5);
	m->val[83] = v1 << 8 | v1 << 0x10 | I(c_edge_thr_adj_value4) |
		v1 << 0x18;
	m->val[84] = I(c_luma_wei_adj_seg) << 8 | I(c_luma_thr_adj_seg) << 10;
	m->val[85] = I(c_luma_wei_adj_value1) << 8 |
		I(c_luma_wei_adj_value2) << 0x10 | I(c_luma_wei_adj_value0) |
		I(c_luma_wei_adj_value3) << 0x18;
	v1 = I(c_luma_wei_adj_value5);
	m->val[86] = v1 << 8 | v1 << 0x10 | I(c_luma_wei_adj_value4) |
		v1 << 0x18;
	m->val[87] = I(c_luma_thr_adj_value1) << 8 |
		I(c_luma_thr_adj_value2) << 0x10 | I(c_luma_thr_adj_value0) |
		I(c_luma_thr_adj_value3) << 0x18;
	v1 = I(c_luma_thr_adj_value5);
	m->val[88] = v1 << 8 | v1 << 0x10 | I(c_luma_thr_adj_value4) |
		v1 << 0x18;
	m->val[89] = I(c_dtb_wei_adj_seg) << 8 | I(c_dtb_thr_adj_seg) << 10;
	m->val[90] = I(c_dtb_wei_adj_value1) << 8 |
		I(c_dtb_wei_adj_value2) << 0x10 | I(c_dtb_wei_adj_value0) |
		I(c_dtb_wei_adj_value3) << 0x18;
	v1 = I(c_dtb_wei_adj_value5);
	m->val[91] = v1 << 8 | v1 << 0x10 | I(c_dtb_wei_adj_value4) |
		v1 << 0x18;
	m->val[92] = I(c_dtb_thr_adj_value1) << 8 |
		I(c_dtb_thr_adj_value2) << 0x10 | I(c_dtb_thr_adj_value0) |
		I(c_dtb_thr_adj_value3) << 0x18;
	v1 = I(c_dtb_thr_adj_value5);
	m->val[93] = v1 << 8 | v1 << 0x10 | I(c_dtb_thr_adj_value4) |
		v1 << 0x18;
	m->val[94] = I(c_ass_wei_adj_seg) << 8 | I(c_ass_thr_adj_seg) << 10;
	m->val[95] = I(c_ass_wei_adj_value1) << 8 |
		I(c_ass_wei_adj_value2) << 0x10 | I(c_ass_wei_adj_value0) |
		I(c_ass_wei_adj_value3) << 0x18;
	v1 = I(c_ass_wei_adj_value5);
	m->val[96] = v1 << 8 | v1 << 0x10 | I(c_ass_wei_adj_value4) |
		v1 << 0x18;
	m->val[97] = I(c_ass_thr_adj_value1) << 8 |
		I(c_ass_thr_adj_value2) << 0x10 | I(c_ass_thr_adj_value0) |
		I(c_ass_thr_adj_value3) << 0x18;
	v1 = I(c_ass_thr_adj_value5);
	m->val[98] = v1 << 8 | v1 << 0x10 | I(c_ass_thr_adj_value4) |
		v1 << 0x18;
	m->val[99] = I(c_sad_wei_adj_seg);
	m->val[100] = I(c_sad_wei_adj_value1) << 8 |
		I(c_sad_wei_adj_value2) << 0x10 | I(c_sad_wei_adj_value0) |
		I(c_sad_wei_adj_value3) << 0x18;
	v1 = I(c_sad_wei_adj_value5);
	m->val[101] = v1 << 8 | v1 << 0x10 | I(c_sad_wei_adj_value4) |
		v1 << 0x18;
	m->val[106] = I(c_median_edg_thres) << 8 |
		I(c_median_cur_lmt_op_en) << 0x10 | I(c_median_smj_thres) |
		I(c_median_cur_lmt_wei) << 0x11 |
		I(c_median_ref_lmt_op_en) << 0x18 |
		I(c_median_ref_lmt_wei) << 0x19;
	m->val[107] = I(c_median_cur_se_wei) << 4 |
		I(c_median_cur_ms_wei) << 8 | I(c_median_cur_ss_wei) |
		I(c_median_cur_me_wei) << 0xc;
	m->val[108] = I(c_median_ref_se_wei) << 4 |
		I(c_median_ref_ms_wei) << 8 | I(c_median_ref_ss_wei) |
		I(c_median_ref_me_wei) << 0xc;
	m->val[109] = I(c_bgm_cur_src) << 4 | I(c_bgm_ref_src) << 6 |
		S(bgm_inter_en, 0, 1) | I(c_bgm_false_thres) << 8 |
		I(c_bgm_false_step) << 0x10 | I(c_bgm_win_opt) << 0x14;
	m->val[110] = I(c_fspa_cur_fus_seg) << 4 | I(c_fspa_ref_fus_seg) << 8;
	v2 = I(c_fspa_cur_fus_wei_0);
	m->val[111] = v2 << 8 | v2 << 0x10 | v2 | v2 << 0x18;
	v2 = I(c_fspa_cur_fus_wei_0);
	m->val[112] = v2 << 8 | v2 << 0x10 | v2 | v2 << 0x18;
	v2 = I(c_fspa_cur_fus_wei_0);
	m->val[113] = v2 << 8 | v2 << 0x10 | v2 |
		I(c_fspa_cur_fus_wei_144) << 0x18;
	m->val[114] = I(c_fspa_cur_fus_wei_176) << 8 |
		I(c_fspa_cur_fus_wei_192) << 0x10 | I(c_fspa_cur_fus_wei_160) |
		I(c_fspa_cur_fus_wei_208) << 0x18;
	v2 = I(c_fspa_ref_fus_wei_0);
	m->val[115] = v2 << 8 | v2 << 0x10 | v2 | v2 << 0x18;
	v2 = I(c_fspa_ref_fus_wei_0);
	m->val[116] = v2 << 8 | v2 << 0x10 | v2 | v2 << 0x18;
	v2 = I(c_fspa_ref_fus_wei_0);
	m->val[117] = v2 << 8 | v2 << 0x10 | v2 |
		I(c_fspa_ref_fus_wei_144) << 0x18;
	m->val[118] = I(c_fspa_ref_fus_wei_176) << 8 |
		I(c_fspa_ref_fus_wei_192) << 0x10 | I(c_fspa_ref_fus_wei_160) |
		I(c_fspa_ref_fus_wei_208) << 0x18;
	m->val[119] = I(c_piir_edge_thres1) << 8 |
		I(c_piir_edge_thres2) << 0x10 | I(c_piir_edge_thres0);
	m->val[120] = I(c_piir_edge_wei1) << 8 | I(c_piir_edge_wei2) << 0x10 |
		I(c_piir_edge_wei0) | I(c_piir_edge_wei3) << 0x18;
	m->val[121] = I(c_piir_ref_fs_wei) << 8 | I(c_piir_cur_fs_wei);
	m->val[122] = I(c_piir_edge_thres1) << 8 |
		I(c_piir_edge_thres2) << 0x10 | I(c_piir_edge_thres0);
	m->val[123] = I(c_piir_edge_wei1) << 8 | I(c_piir_edge_wei2) << 0x10 |
		I(c_piir_edge_wei0) | I(c_piir_edge_wei3) << 0x18;
	m->val[124] = I(c_fiir_fus_seg);
	v2 = I(c_fiir_fus_wei0);
	v2 = v2 << 8 | v2 << 0x10 | v2 | v2 << 0x18;
	m->val[125] = v2;
	m->val[126] = v2;
	m->val[127] = I(c_fiir_fus_wei2) << 8 | I(c_fiir_fus_wei3) << 0x10 |
		I(c_fiir_fus_wei1) | I(c_fiir_fus_wei4) << 0x18;
	m->val[128] = I(c_fiir_fus_wei6) << 8 | I(c_fiir_fus_wei7) << 0x10 |
		I(c_fiir_fus_wei5) | I(c_fiir_fus_wei8) << 0x18;
	m->val[129] = I(c_false_edg_thres0) << 8 |
		I(c_false_edg_thres1) << 0x10 | I(c_false_smj_thres) |
		I(c_false_edg_thres2) << 0x18;
	v2 = I(c_false_thres_s0);
	v1 = I(c_false_step_s0);
	m->val[130] = v1 << 8 | v2 << 0x10 | v2 | v1 << 0x18;
	v2 = I(c_false_thres_s0);
	v1 = I(c_false_step_s0);
	m->val[131] = v1 << 8 | v2 << 0x10 | v2 | v1 << 0x18;
	v2 = I(c_false_thres_m0);
	v1 = I(c_false_step_m0);
	m->val[132] = v1 << 8 | v2 << 0x10 | v2 | v1 << 0x18;
	v2 = I(c_false_thres_m0);
	v1 = I(c_false_step_m0);
	m->val[133] = v1 << 8 | v2 << 0x10 | v2 | v1 << 0x18;
	m->val[134] = I(c_sat_lmt_thres) << 8 | I(c_sat_lmt_stren) << 0x10 |
		I(c_sat_nml_stren);
	return x->err;
}

/*
 * SDNS: sensor de-noise configuration. The fixed filter kernels are not
 * part of the block.
 */
static int pack_sdns(struct dns_ctx *x, uint32_t *s)
{
	x->mod = "sdns";

	s[0] = S(aa_mv_det_opt, 1, 7) << 4 | S(aa_mv_det_opt, 0, 7);
	s[1] = I(grad_zy_thres) << 0x10 | I(grad_zx_thres);
	s[2] = I(std_thr1) << 4 | I(std_thr2) << 0x10 | S(aa_mv_det_opt, 2, 7);
	s[3] = S(h_mv_wei, 1, 4) << 4 | S(h_mv_wei, 2, 4) << 8 |
		S(h_mv_wei, 0, 4) | S(h_mv_wei, 3, 4) << 0xc;
	s[4] = S(aa_mv_det_opt, 4, 7) << 8 | S(aa_mv_det_opt, 3, 7);
	s[5] = I(mv_num_thr_7x7) << 8 | I(mv_num_thr_9x9) << 0x10 |
		I(mv_num_thr_5x5) | I(mv_num_thr_11x11) << 0x18;
	s[6] = S(aa_mv_det_opt, 6, 7) << 0x18 | S(aa_mv_det_opt, 5, 7);
	s[7] = S(r_s, 1, 15) << 0x10 | S(r_s, 0, 15);
	s[8] = S(r_s, 3, 15) << 0x10 | S(r_s, 2, 15);
	s[9] = S(r_s, 5, 15) << 0x10 | S(r_s, 4, 15);
	s[10] = S(r_s, 7, 15) << 0x10 | S(r_s, 6, 15);
	s[11] = S(r_s, 9, 15) << 0x10 | S(r_s, 8, 15);
	s[12] = S(r_s, 11, 15) << 0x10 | S(r_s, 10, 15);
	s[13] = S(r_s, 13, 15) << 0x10 | S(r_s, 12, 15);
	s[14] = S(r_s, 14, 15);
	s[15] = S(r_mv, 1, 15) << 0x10 | S(r_mv, 0, 15);
	s[16] = S(r_mv, 3, 15) << 0x10 | S(r_mv, 2, 15);
	s[17] = S(r_mv, 5, 15) << 0x10 | S(r_mv, 4, 15);
	s[18] = S(r_mv, 7, 15) << 0x10 | S(r_mv, 6, 15);
	s[19] = S(r_mv, 9, 15) << 0x10 | S(r_mv, 8, 15);
	s[20] = S(r_mv, 11, 15) << 0x10 | S(r_mv, 10, 15);
	s[21] = S(r_mv, 13, 15) << 0x10 | S(r_mv, 12, 15);
	s[22] = S(r_mv, 14, 15);
	s[23] = I(h_s_2) << 8 | I(h_s_3) << 0x10 | I(h_s_1) | I(h_s_4) << 0x18;
	s[24] = I(h_s_6) << 8 | I(h_s_7) << 0x10 | I(h_s_5) | I(h_s_8) << 0x18;
	s[25] = I(h_s_10) << 8 | I(h_s_11) << 0x10 | I(h_s_9) |
		I(h_s_12) << 0x18;
	s[26] = I(h_s_14) << 8 | I(h_s_15) << 0x10 | I(h_s_13) |
		I(h_s_16) << 0x18;
	s[27] = I(h_mv_2) << 8 | I(h_mv_3) << 0x10 | I(h_mv_1) |
		I(h_mv_4) << 0x18;
	s[28] = I(h_mv_6) << 8 | I(h_mv_7) << 0x10 | I(h_mv_5) |
		I(h_mv_8) << 0x18;
	s[29] = I(h_mv_10) << 8 | I(h_mv_11) << 0x10 | I(h_mv_9) |
		I(h_mv_12) << 0x18;
	s[30] = I(h_mv_14) << 8 | I(h_mv_15) << 0x10 | I(h_mv_13) |
		I(h_mv_16) << 0x18;
	s[31] = I(light_thres) << 8 | S(h_val_max, 0, 1) << 0x10 |
		I(dark_thres) | I(sharpen_tt_opt) << 0x18;
	s[32] = S(d_s1_thr, 1, 15) << 0x10 | S(d_s1_thr, 0, 15);
	s[33] = S(d_s1_thr, 3, 15) << 0x10 | S(d_s1_thr, 2, 15);
	s[34] = S(d_s1_thr, 5, 15) << 0x10 | S(d_s1_thr, 4, 15);
	s[35] = S(d_s1_thr, 7, 15) << 0x10 | S(d_s1_thr, 6, 15);
	s[36] = S(d_s1_thr, 9, 15) << 0x10 | S(d_s1_thr, 8, 15);
	s[37] = S(d_s1_thr, 11, 15) << 0x10 | S(d_s1_thr, 10, 15);
	s[38] = S(d_s1_thr, 13, 15) << 0x10 | S(d_s1_thr, 12, 15);
	s[39] = S(d_s1_thr, 14, 15);
	s[40] = S(w_thr, 1, 16) << 8 | S(w_thr, 0, 16);
	s[41] = S(w_thr, 3, 16) << 8 | S(w_thr, 2, 16);
	s[42] = S(w_thr, 5, 16) << 8 | S(w_thr, 4, 16);
	s[43] = S(w_thr, 7, 16) << 8 | S(w_thr, 6, 16);
	s[44] = S(w_thr, 9, 16) << 8 | S(w_thr, 8, 16);
	s[45] = S(w_thr, 11, 16) << 8 | S(w_thr, 10, 16);
	s[46] = S(w_thr, 13, 16) << 8 | S(w_thr, 12, 16);
	s[47] = S(w_thr, 15, 16) << 8 | S(w_thr, 14, 16);
	s[48] = I(ave_thres) << 4 | S(ave_fliter, 0, 3) |
		S(ave_fliter, 1, 3) << 1 | S(ave_fliter, 2, 3) << 0x10;
	s[49] = S(sharpen_g_std, 1, 2) << 4 | S(sharpen_g_std, 0, 2);
	s[50] = I(sp_uu_thres) << 8 | S(sp_uu_par, 0, 3);
	s[51] = S(sp_uu_par, 1, 3) << 0x10 | S(sp_uu_par, 2, 3) << 0x18 |
		I(sp_uu_stren);
	s[52] = I(sp_mv_uu_stren) << 8 | I(sp_mv_uu_thres);
	s[53] = S(sp_mv_wei_uu_value, 1, 4) << 4 |
		S(sp_mv_wei_uu_value, 2, 4) << 8 |
		S(sp_mv_wei_uu_value, 0, 4) |
		S(sp_mv_wei_uu_value, 3, 4) << 0xc;
	s[54] = S(sp_d_v2_sigma_win5_slope, 1, 2) << 0x10 |
		S(sp_d_wbhl_flat, 0, 9) << 0x18 |
		S(sp_d_v2_sigma_win5_slope, 0, 2) | I(sp_d_v2_win5_thres) << 8;
	s[55] = I(sp_d_w_sp_stren_1) << 0x10 | I(sp_d_w_sp_stren_0);
	s[56] = I(sp_d_w_sp_stren_3) << 0x10 | I(sp_d_w_sp_stren_2);
	s[57] = S(sp_d_wbhl_flat, 2, 9) << 0x10 | S(sp_d_wbhl_flat, 1, 9);
	s[58] = I(sp_d_b_sp_stren_1) << 0x10 | I(sp_d_b_sp_stren_0);
	s[59] = I(sp_d_b_sp_stren_3) << 0x10 | I(sp_d_b_sp_stren_2);
	s[60] = S(sp_d_wbhl_flat, 4, 9) << 0x10 | S(sp_d_wbhl_flat, 3, 9);
	s[61] = S(sp_d_wbhl_flat, 6, 9) << 0x10 | S(sp_d_wbhl_flat, 5, 9);
	s[62] = I(sp_d_flat_thres) << 0x10 | S(sp_d_wbhl_flat, 7, 9);
	s[63] = I(sp_d_flat_stren) << 0x10 | S(sp_d_wbhl_flat, 8, 9);
	s[64] = S(sp_ud_v2_1_coef, 1, 8) << 4 | S(sp_ud_v2_1_coef, 2, 8) << 8 |
		S(sp_ud_v2_1_coef, 0, 8) | S(sp_ud_v2_1_coef, 3, 8) << 0xc |
		S(sp_ud_v2_1_coef, 4, 8) << 0x10;
	s[65] = S(sp_ud_v2_1_coef, 6, 8) << 8 |
		S(sp_ud_v2_1_coef, 7, 8) << 0xc | S(sp_ud_v2_1_coef, 5, 8) |
		S(sp_ud_wbhl_flat, 0, 11) << 0x10;
	s[66] = I(sp_ud_w_sp_stren_1) << 0x10 | I(sp_ud_w_sp_stren_0);
	s[67] = I(sp_ud_w_sp_stren_3) << 0x10 | I(sp_ud_w_sp_stren_2);
	s[68] = S(sp_ud_wbhl_flat, 2, 11) << 0x10 | S(sp_ud_wbhl_flat, 1, 11);
	s[69] = I(sp_ud_b_sp_stren_1) << 0x10 | I(sp_ud_b_sp_stren_0);
	s[70] = I(sp_ud_b_sp_stren_3) << 0x10 | I(sp_ud_b_sp_stren_2);
	s[71] = I(sp_ud_std_thres) << 0x10 | I(sp_ud_std_stren) << 0x18 |
		S(sp_ud_wbhl_flat, 3, 11);
	s[72] = S(sp_ud_wbhl_flat, 5, 11) << 8 |
		S(sp_ud_wbhl_flat, 6, 11) << 0x10 | S(sp_ud_wbhl_flat, 4, 11);
	s[73] = S(sp_ud_wbhl_flat, 8, 11) << 8 | S(sp_ud_wbhl_flat, 7, 11) |
		I(sp_ud_flat_thres) << 0x14;
	s[74] = I(sp_ud_flat_stren) << 0x10 | S(sp_ud_wbhl_flat, 9, 11);
	s[75] = S(sp_ud_wbhl_flat, 10, 11);
	s[76] = S(sp_uu_np, 1, 16) << 8 | S(sp_uu_np, 2, 16) << 0x10 |
		S(sp_uu_np, 0, 16) | S(sp_uu_np, 3, 16) << 0x18;
	s[77] = S(sp_uu_np, 5, 16) << 8 | S(sp_uu_np, 6, 16) << 0x10 |
		S(sp_uu_np, 4, 16) | S(sp_uu_np, 7, 16) << 0x18;
	s[78] = S(sp_uu_np, 9, 16) << 8 | S(sp_uu_np, 10, 16) << 0x10 |
		S(sp_uu_np, 8, 16) | S(sp_uu_np, 11, 16) << 0x18;
	s[79] = S(sp_uu_np, 13, 16) << 8 | S(sp_uu_np, 14, 16) << 0x10 |
		S(sp_uu_np, 12, 16) | S(sp_uu_np, 15, 16) << 0x18;
	s[80] = S(sp_d_w_wei_np, 1, 22) << 8 |
		S(sp_d_w_wei_np, 2, 22) << 0x10 | S(sp_d_w_wei_np, 0, 22) |
		S(sp_d_w_wei_np, 3, 22) << 0x18;
	s[81] = S(sp_d_w_wei_np, 5, 22) << 8 |
		S(sp_d_w_wei_np, 6, 22) << 0x10 | S(sp_d_w_wei_np, 4, 22) |
		S(sp_d_w_wei_np, 7, 22) << 0x18;
	s[82] = S(sp_d_w_wei_np, 9, 22) << 8 |
		S(sp_d_w_wei_np, 10, 22) << 0x10 | S(sp_d_w_wei_np, 8, 22) |
		S(sp_d_w_wei_np, 11, 22) << 0x18;
	s[83] = S(sp_d_w_wei_np, 13, 22) << 8 |
		S(sp_d_w_wei_np, 14, 22) << 0x10 | S(sp_d_w_wei_np, 12, 22) |
		S(sp_d_w_wei_np, 15, 22) << 0x18;
	s[84] = S(sp_d_w_wei_np, 17, 22) << 8 |
		S(sp_d_w_wei_np, 18, 22) << 0x10 | S(sp_d_w_wei_np, 16, 22) |
		S(sp_d_w_wei_np, 19, 22) << 0x18;
	s[85] = S(sp_d_w_wei_np, 21, 22) << 8 | S(sp_d_w_wei_np, 20, 22);
	s[86] = S(sp_d_b_wei_np, 1, 22) << 8 |
		S(sp_d_b_wei_np, 2, 22) << 0x10 | S(sp_d_b_wei_np, 0, 22) |
		S(sp_d_b_wei_np, 3, 22) << 0x18;
	s[87] = S(sp_d_b_wei_np, 5, 22) << 8 |
		S(sp_d_b_wei_np, 6, 22) << 0x10 | S(sp_d_b_wei_np, 4, 22) |
		S(sp_d_b_wei_np, 7, 22) << 0x18;
	s[88] = S(sp_d_b_wei_np, 9, 22) << 8 |
		S(sp_d_b_wei_np, 10, 22) << 0x10 | S(sp_d_b_wei_np, 8, 22) |
		S(sp_d_b_wei_np, 11, 22) << 0x18;
	s[89] = S(sp_d_b_wei_np, 13, 22) << 8 |
		S(sp_d_b_wei_np, 14, 22) << 0x10 | S(sp_d_b_wei_np, 12, 22) |
		S(sp_d_b_wei_np, 15, 22) << 0x18;
	s[90] = S(sp_d_b_wei_np, 17, 22) << 8 |
		S(sp_d_b_wei_np, 18, 22) << 0x10 | S(sp_d_b_wei_np, 16, 22) |
		S(sp_d_b_wei_np, 19, 22) << 0x18;
	s[91] = S(sp_d_b_wei_np, 21, 22) << 8 | S(sp_d_b_wei_np, 20, 22);
	s[92] = S(sp_ud_w_wei_np, 1, 22) << 8 |
		S(sp_ud_w_wei_np, 2, 22) << 0x10 | S(sp_ud_w_wei_np, 0, 22) |
		S(sp_ud_w_wei_np, 3, 22) << 0x18;
	s[93] = S(sp_ud_w_wei_np, 5, 22) << 8 |
		S(sp_ud_w_wei_np, 6, 22) << 0x10 | S(sp_ud_w_wei_np, 4, 22) |
		S(sp_ud_w_wei_np, 7, 22) << 0x18;
	s[94] = S(sp_ud_w_wei_np, 9, 22) << 8 |
		S(sp_ud_w_wei_np, 10, 22) << 0x10 | S(sp_ud_w_wei_np, 8, 22) |
		S(sp_ud_w_wei_np, 11, 22) << 0x18;
	s[95] = S(sp_ud_w_wei_np, 13, 22) << 8 |
		S(sp_ud_w_wei_np, 14, 22) << 0x10 | S(sp_ud_w_wei_np, 12, 22) |
		S(sp_ud_w_wei_np, 15, 22) << 0x18;
	s[96] = S(sp_ud_w_wei_np, 17, 22) << 8 |
		S(sp_ud_w_wei_np, 18, 22) << 0x10 | S(sp_ud_w_wei_np, 16, 22) |
		S(sp_ud_w_wei_np, 19, 22) << 0x18;
	s[97] = S(sp_ud_w_wei_np, 21, 22) << 8 | S(sp_ud_w_wei_np, 20, 22);
	s[98] = S(sp_ud_b_wei_np, 1, 22) << 8 |
		S(sp_ud_b_wei_np, 2, 22) << 0x10 | S(sp_ud_b_wei_np, 0, 22) |
		S(sp_ud_b_wei_np, 3, 22) << 0x18;
	s[99] = S(sp_ud_b_wei_np, 5, 22) << 8 |
		S(sp_ud_b_wei_np, 6, 22) << 0x10 | S(sp_ud_b_wei_np, 4, 22) |
		S(sp_ud_b_wei_np, 7, 22) << 0x18;
	s[100] = S(sp_ud_b_wei_np, 9, 22) << 8 |
		S(sp_ud_b_wei_np, 10, 22) << 0x10 | S(sp_ud_b_wei_np, 8, 22) |
		S(sp_ud_b_wei_np, 11, 22) << 0x18;
	s[101] = S(sp_ud_b_wei_np, 13, 22) << 8 |
		S(sp_ud_b_wei_np, 14, 22) << 0x10 | S(sp_ud_b_wei_np, 12, 22) |
		S(sp_ud_b_wei_np, 15, 22) << 0x18;
	s[102] = S(sp_ud_b_wei_np, 17, 22) << 8 |
		S(sp_ud_b_wei_np, 18, 22) << 0x10 | S(sp_ud_b_wei_np, 16, 22) |
		S(sp_ud_b_wei_np, 19, 22) << 0x18;
	s[103] = S(sp_ud_b_wei_np, 21, 22) << 8 | S(sp_ud_b_wei_np, 20, 22);
	return x->err;
}

/*
 * YDNS: luma de-noise parameters.
 */
static int pack_ydns(struct dns_ctx *x, uint32_t *y)
{
	x->mod = "ydns";

	y[0] = I(mv_thres1) << 8 | I(mv_thres2) << 0x10 | I(mv_thres0);
	y[1] = I(fus_max_thres) << 8 | I(fus_level) << 0x10 | I(fus_min_thres);
	y[2] = I(fus_sewei) << 4 | I(fus_mswei) << 8 | I(fus_sswei) |
		I(fus_mewei) << 0xc | I(fus_uvwei) << 0x10;
	y[3] = S(edge_out, 0, 1) << 0x10 | I(edge_wei) | I(edge_div) << 4 |
		I(edge_thres) << 8;
	return x->err;
}

/*
 * YSP: luma sharpening parameters.
 */
static int pack_ysp(struct dns_ctx *x, uint32_t *p)
{
	x->mod = "sharpen";

	p[0] = I(y_sp_sl_exp_num) << 0x10 | I(y_sp_sl_exp_thres);
	p[1] = S(y_sp_std_cfg, 0, 2) << 0x10 | S(y_sp_std_cfg, 1, 2);
	p[2] = S(y_sp_uu_par_cfg, 1, 4) << 8 | S(y_sp_uu_par_cfg, 0, 4) |
		I(y_sp_uu_min_stren) << 0x10;
	p[3] = S(y_sp_uu_par_cfg, 2, 4) << 0x10 | I(y_sp_uu_min_thres) |
		I(y_sp_mv_uu_thres) << 0x18 | I(y_sp_uu_thres) << 8;
	p[4] = S(y_sp_out_opt, 0, 1) << 0x1c |
		S(y_sp_uu_par_cfg, 3, 4) << 0x18 | I(y_sp_uu_stren) << 8 |
		I(y_sp_mv_uu_stren);
	p[5] = I(y_sp_mv_fl_std_thres) << 8 | I(y_sp_fl_std_thres);
	p[6] = I(y_sp_fl_thres) << 0x10 | I(y_sp_fl_min_thres) << 0x18 |
		S(y_sp_fl_par_cfg, 0, 2);
	p[7] = I(y_sp_mv_fl_thres) << 0x10 | I(y_sp_mv_fl_min_thres) << 0x18 |
		S(y_sp_fl_par_cfg, 1, 2);
	p[8] = S(y_sp_v1_v2_coef_par_cfg, 1, 12) << 0x10 |
		S(y_sp_v1_v2_coef_par_cfg, 0, 12) | I(y_sp_v2_win5_thres) << 8;
	p[9] = S(y_sp_v1_v2_coef_par_cfg, 3, 12) << 4 |
		S(y_sp_v1_v2_coef_par_cfg, 4, 12) << 8 |
		S(y_sp_v1_v2_coef_par_cfg, 2, 12) |
		S(y_sp_v1_v2_coef_par_cfg, 5, 12) << 0xc |
		S(y_sp_v1_v2_coef_par_cfg, 6, 12) << 0x10 |
		S(y_sp_v1_v2_coef_par_cfg, 7, 12) << 0x14;
	p[10] = S(y_sp_v1_v2_coef_par_cfg, 9, 12) << 8 |
		S(y_sp_v1_v2_coef_par_cfg, 10, 12) << 0x10 |
		S(y_sp_v1_v2_coef_par_cfg, 8, 12) |
		S(y_sp_v1_v2_coef_par_cfg, 11, 12) << 0x18;
	p[11] = S(y_sp_w_b_ll_par_cfg, 1, 9) << 8 |
		S(y_sp_w_b_ll_par_cfg, 2, 9) << 0x10 |
		S(y_sp_w_b_ll_par_cfg, 0, 9) |
		S(y_sp_w_b_ll_par_cfg, 3, 9) << 0x18;
	p[12] = S(y_sp_w_b_ll_par_cfg, 5, 9) << 8 |
		S(y_sp_w_b_ll_par_cfg, 6, 9) << 0x10 |
		S(y_sp_w_b_ll_par_cfg, 4, 9);
	p[13] = S(y_sp_w_b_ll_par_cfg, 8, 9) << 0x10 |
		S(y_sp_w_b_ll_par_cfg, 7, 9);
	p[14] = S(y_sp_uu_np, 1, 16) << 8 | S(y_sp_uu_np, 2, 16) << 0x10 |
		S(y_sp_uu_np, 0, 16) | S(y_sp_uu_np, 3, 16) << 0x18;
	p[15] = S(y_sp_uu_np, 5, 16) << 8 | S(y_sp_uu_np, 6, 16) << 0x10 |
		S(y_sp_uu_np, 4, 16) | S(y_sp_uu_np, 7, 16) << 0x18;
	p[16] = S(y_sp_uu_np, 9, 16) << 8 | S(y_sp_uu_np, 10, 16) << 0x10 |
		S(y_sp_uu_np, 8, 16) | S(y_sp_uu_np, 11, 16) << 0x18;
	p[17] = S(y_sp_uu_np, 13, 16) << 8 | S(y_sp_uu_np, 14, 16) << 0x10 |
		S(y_sp_uu_np, 12, 16) | S(y_sp_uu_np, 15, 16) << 0x18;
	p[18] = S(y_sp_w_wei_np, 1, 16) << 8 |
		S(y_sp_w_wei_np, 2, 16) << 0x10 | S(y_sp_w_wei_np, 0, 16) |
		S(y_sp_w_wei_np, 3, 16) << 0x18;
	p[19] = S(y_sp_w_wei_np, 5, 16) << 8 |
		S(y_sp_w_wei_np, 6, 16) << 0x10 | S(y_sp_w_wei_np, 4, 16) |
		S(y_sp_w_wei_np, 7, 16) << 0x18;
	p[20] = S(y_sp_w_wei_np, 9, 16) << 8 |
		S(y_sp_w_wei_np, 10, 16) << 0x10 | S(y_sp_w_wei_np, 8, 16) |
		S(y_sp_w_wei_np, 11, 16) << 0x18;
	p[21] = S(y_sp_w_wei_np, 13, 16) << 8 |
		S(y_sp_w_wei_np, 14, 16) << 0x10 | S(y_sp_w_wei_np, 12, 16) |
		S(y_sp_w_wei_np, 15, 16) << 0x18;
	p[22] = S(y_sp_b_wei_np, 1, 16) << 8 |
		S(y_sp_b_wei_np, 2, 16) << 0x10 | S(y_sp_b_wei_np, 0, 16) |
		S(y_sp_b_wei_np, 3, 16) << 0x18;
	p[23] = S(y_sp_b_wei_np, 5, 16) << 8 |
		S(y_sp_b_wei_np, 6, 16) << 0x10 | S(y_sp_b_wei_np, 4, 16) |
		S(y_sp_b_wei_np, 7, 16) << 0x18;
	p[24] = S(y_sp_b_wei_np, 9, 16) << 8 |
		S(y_sp_b_wei_np, 10, 16) << 0x10 | S(y_sp_b_wei_np, 8, 16) |
		S(y_sp_b_wei_np, 11, 16) << 0x18;
	p[25] = S(y_sp_b_wei_np, 13, 16) << 8 |
		S(y_sp_b_wei_np, 14, 16) << 0x10 | S(y_sp_b_wei_np, 12, 16) |
		S(y_sp_b_wei_np, 15, 16) << 0x18;
	p[26] = I(y_sp_w_sl_stren_1) << 0x10 | I(y_sp_w_sl_stren_0);
	p[27] = I(y_sp_w_sl_stren_3) << 0x10 | I(y_sp_w_sl_stren_2);
	p[28] = I(y_sp_b_sl_stren_1) << 0x10 | I(y_sp_b_sl_stren_0);
	p[29] = I(y_sp_b_sl_stren_3) << 0x10 | I(y_sp_b_sl_stren_2);
	p[30] = I(y_sp_uu_sl_1) << 8 | I(y_sp_uu_sl_2) << 0x10 |
		I(y_sp_uu_sl_0) | I(y_sp_uu_sl_3) << 0x18;
	p[31] = I(y_sp_fl_sl_1) << 8 | I(y_sp_fl_sl_2) << 0x10 |
		I(y_sp_fl_sl_0) | I(y_sp_fl_sl_3) << 0x18;
	return x->err;
}

/*
 * DMSC: demosaic configuration.
 */
static int pack_dmsc(struct dns_ctx *x, uint32_t *d)
{
	x->mod = "dmsc";

	d[0] = S(out_opt, 0, 1);
	d[19] = S(sp_d_limit, 0, 2);
	d[20] = S(sp_d_limit, 1, 2);
	d[25] = S(sp_ud_limit, 0, 2);
	d[26] = S(sp_ud_limit, 1, 2);
	d[34] = S(fc_slope, 0, 5) << 6 | S(fc_slope, 1, 5) << 0x10 |
		S(fc_slope, 3, 5);
	d[38] = S(sp_ud_con_0, 11, 17) << 8 | S(sp_ud_con_0, 10, 17) << 0xf |
		S(sp_ud_con_0, 12, 17) | S(sp_ud_con_0, 9, 17) << 0x1b;
	d[39] = S(sp_ud_con_0, 4, 17) << 4 | S(sp_ud_con_0, 3, 17) << 8 |
		S(sp_ud_con_0, 5, 17) | S(sp_ud_con_0, 2, 17) << 0x10 |
		S(sp_ud_con_0, 1, 17) << 0x18 | S(sp_ud_con_0, 0, 17) << 0x1b;
	d[64] = S(deir_con_par, 0, 3) << 8 | S(deir_con_par, 1, 3);
	d[67] = S(deir_r, 1, 8) << 0x10 | S(deir_r, 0, 8);
	d[68] = S(deir_r, 3, 8) << 0x10 | S(deir_r, 2, 8);
	d[69] = S(deir_r, 5, 8) << 0x10 | S(deir_r, 4, 8);
	d[70] = S(deir_r, 7, 8) << 0x10 | S(deir_r, 6, 8);
	d[71] = S(deir_g, 1, 8) << 0x10 | S(deir_g, 0, 8);
	d[72] = S(deir_g, 3, 8) << 0x10 | S(deir_g, 2, 8);
	d[73] = S(deir_g, 5, 8) << 0x10 | S(deir_g, 4, 8);
	d[74] = S(deir_g, 7, 8) << 0x10 | S(deir_g, 6, 8);
	d[75] = S(deir_b, 1, 8) << 0x10 | S(deir_b, 0, 8);
	d[76] = S(deir_b, 3, 8) << 0x10 | S(deir_b, 2, 8);
	d[77] = S(deir_b, 5, 8) << 0x10 | S(deir_b, 4, 8);
	d[78] = S(deir_b, 7, 8) << 0x10 | S(deir_b, 6, 8);
	d[80] = S(awb_gain, 0, 3) << 0x10 | S(awb_gain, 2, 3) << 0x1c |
		S(awb_gain, 1, 3);
	d[82] = S(uu_full_en, 1, 2) << 0x18;
	d[89] = S(h_gain_con, 1, 7) << 0x10 | S(h_gain_con, 0, 7);
	d[92] = S(h_gain_con, 3, 7) << 0x10 | S(h_gain_con, 2, 7);
	d[95] = S(h_gain_con, 5, 7) << 0x10 | S(h_gain_con, 4, 7);
	d[96] = ((S(uu_full_en, 0, 2) >> 16) & 1) << 8 | S(h_gain_con, 6, 7);
	d[102] = S(fc_en, 0, 2) << 8 | S(fc_slope, 4, 5) << 0xc |
		S(fc_slope, 2, 5) | S(fc_en, 1, 2) << 0x14;
	d[1] = S(uu_con_par, 0, 2) << 8 | S(uu_full_en, 0, 2) << 0x1f |
		S(uu_con_par, 1, 2) | I(g_std_stren) << 10;
	d[2] = I(uu_thres) << 0x11 | I(uu_stren);
	d[3] = I(alias_dir_diff_thres_rb) << 0x10 | I(alias_dir_diff_thres_g);
	d[4] = I(win5_hv_edge_thres_2) << 0x11 | I(win5_hv_edge_thres_1) << 1;
	d[5] = I(hv_edge_thres_2) << 0x11 | I(hv_edge_thres_1) << 1;
	d[6] = S(dir_con_par, 1, 6) << 0x10 | S(dir_con_par, 0, 6) << 0x18 |
		I(hv_edge_stren) << 2;
	d[7] = I(aa_edge_thres_2) << 0x11 | I(aa_edge_thres_1) << 1;
	d[8] = S(dir_con_par, 3, 6) << 0x10 | S(dir_con_par, 2, 6) << 0x18 |
		I(aa_edge_stren) << 2;
	d[9] = I(hvaa_edge_thres_2) << 0x11 | I(hvaa_edge_thres_1) << 1;
	d[10] = S(dir_con_par, 5, 6) << 0x10 | S(dir_con_par, 4, 6) << 0x18 |
		I(hvaa_edge_stren) << 2;
	d[11] = S(alias_con_par, 2, 3) << 6 | S(alias_con_par, 0, 3) << 10 |
		S(alias_con_par, 1, 3) | I(alias_stren) << 0x14;
	d[12] = I(alias_thres_2) << 0x10 | I(alias_thres_1);
	d[13] = I(nor_alias_blur_stren) << 0x11 | I(nor_alias_blur_thres) << 1;
	d[14] = S(nor_con_par, 0, 2) << 0x10 | S(nor_con_par, 1, 2) |
		I(nor_uu_wei_stren) << 8;
	d[15] = I(sp_d_dir_sigma) << 7 | I(sp_d_v2_sigma_1) << 0x11 |
		S(sp_d_con, 0, 2) << 2;
	d[16] = I(sp_d_w_sp_stren) << 0x10 | I(sp_d_b_sp_stren);
	d[17] = I(sp_d_hl_stren) << 0x12 | I(sp_d_ll_stren) << 2;
	d[18] = (I(sp_d_hl_thres) * 0x10 + 0xf) * 0x10000 |
		I(sp_d_ll_thres) << 4;
	d[21] = S(sp_ud_con_0, 13, 17) << 2 | S(sp_ud_con_0, 6, 17) << 4 |
		S(sp_ud_con_0, 7, 17) << 8 | S(sp_ud_con_0, 8, 17) << 0xc |
		I(sp_ud_v2_sigma_1) << 0x10;
	d[22] = I(sp_ud_w_sp_stren) << 0x10 | I(sp_ud_b_sp_stren);
	d[23] = I(sp_ud_hl_stren) << 0x14 | I(sp_ud_ll_stren) << 4;
	d[24] = (I(sp_ud_hl_thres) * 0x10 + 0xf) * 0x10000 |
		I(sp_ud_ll_thres) << 4;
	d[27] = I(sp_nor_alias_thres) << 0x11 | S(sp_d_con_0, 1, 2);
	d[28] = I(alias_fusion_thres_2) << 0x11 | I(alias_fusion_thres_1);
	d[29] = I(alias_fusion_alpha) << 5 |
		I(alias_dir_intp_sp_stren) << 0x10 |
		S(alias_fusion_slope_2, 0, 1);
	d[30] = I(fc_t2_stren) << 7 | I(fc_t1_stren) << 0xe | I(fc_oe_stren) |
		I(fc_alias_stren) << 0x15;
	d[31] = I(fc_alias_thres) << 0x11 | I(fc_t1_g_std_low);
	d[32] = I(fc_t1_lum_low) << 4 | I(fc_t1_sat_high) << 0x10;
	d[33] = I(fc_oe_num_thres) << 0xc | I(fc_t2_g_std_low) << 0x13 |
		(I(fc_oe_value_thres) * 0x10 + 0xfU);
	d[35] = I(fc_t2_g_std_high) << 0x10 | I(fc_t1_g_std_high);
	d[36] = I(sp_d_v2_win5_thres) << 0x10 | S(sp_d_con_0, 0, 2);
	d[37] = I(sp_d_oe_stren) << 2 | S(sp_d_con, 1, 2) << 0x10;
	d[40] = S(sp_ud_con_0, 14, 17) << 8 | I(sp_ud_std_stren) |
		I(sp_ud_std_thres) << 0x10;
	d[41] = S(sp_ud_con_0, 15, 17) << 0x1b | S(sp_ud_con_0, 16, 17) |
		I(sp_ud_oe_stren) << 0xc;
	d[65] = (I(deir_rgb_oe_h) * 0x10 + 0xf) * 0x10000 |
		I(deir_rgb_oe_l) << 4;
	d[66] = (I(deir_ir_oe_h) * 0x10 + 0xf) * 0x10000 |
		I(deir_ir_oe_l) << 4;
	d[79] = I(fc_lum_thres) << 0x10 | I(fc_lum_stren);
	d[81] = I(deir_fusion_stren) << 8 | I(deir_fusion_thres) << 0x10 |
		S(deir_con_par, 2, 3);
	d[83] = I(sp_ud_flat_thres) << 0x10 | I(sp_d_flat_thres);
	d[84] = I(sp_ud_flat_slope) << 0x12 | I(sp_d_flat_slope) << 1;
	d[85] = I(sp_d_flat_stren) << 1 | I(sp_ud_flat_stren) << 0x10;
	d[86] = I(sp_d_dir_thres);
	d[87] = I(h_gain_p1_value);
	d[88] = I(h_gain_p1_stren) << 0x10 | I(h_gain_p1_range);
	d[90] = I(h_gain_p2_value);
	d[91] = I(h_gain_p2_stren) << 0x10 | I(h_gain_p2_range);
	d[93] = I(h_gain_p3_value);
	d[94] = I(h_gain_p3_stren) << 0x10 | I(h_gain_p3_range);
	d[97] = (I(fc_t1_lum_high) * 0x10 + 0xf) * 0x10000 | I(fc_t1_sat_low);
	d[98] = I(fc_t1_h_range) << 0x10 | I(fc_t1_h_value);
	d[99] = I(fc_t2_sat_low) << 0x10 | I(fc_t2_sat_high);
	d[100] = (I(fc_t2_lum_high) * 0x10 + 0xf) * 0x10000 |
		I(fc_t2_lum_low) << 4;
	d[101] = I(fc_t2_h_range) << 0x10 | I(fc_t2_h_value);
	d[103] = (I(fc_oe_sat_high) * 0x10 + 0xf) * 0x10000 |
		I(fc_oe_sat_low) << 4;
	d[104] = (I(fc_oe_h_low) * 2 + I(fc_oe_h_diff)) * 0x10000 |
		I(fc_oe_h_low) * 2;
	d[105] = I(sp_ud_g_lap_3_std_thres) << 0xc | S(sp_ud_g_lap_3, 1, 2);
	d[106] = I(sp_ud_g_lap_3_std_stren) << 8 | S(sp_ud_g_lap_3, 0, 2);
	d[42] = I(uu_np_1) << 0x14 | I(uu_np_0) << 4;
	d[43] = I(uu_np_3) << 0x14 | I(uu_np_2) << 4;
	d[44] = I(uu_np_5) << 0x14 | I(uu_np_4) << 4;
	d[45] = I(uu_np_7) << 0x14 | I(uu_np_6) << 4;
	d[46] = I(uu_np_9) << 0x14 | I(uu_np_8) << 4;
	d[47] = I(uu_np_11) << 0x14 | I(uu_np_10) << 4;
	d[48] = I(uu_np_13) << 0x14 | I(uu_np_12) << 4;
	d[49] = I(uu_np_15) << 0x14 | I(uu_np_14) << 4;
	d[50] = I(sp_d_wei_np_1) << 6 | I(sp_d_wei_np_2) << 0xc |
		I(sp_d_wei_np_0) | I(sp_d_wei_np_3) << 0x12 |
		I(sp_d_wei_np_4) << 0x18;
	d[51] = I(sp_d_wei_np_6) << 6 | I(sp_d_wei_np_7) << 0xc |
		I(sp_d_wei_np_5) | I(sp_d_wei_np_8) << 0x12 |
		I(sp_d_wei_np_9) << 0x18;
	d[52] = I(sp_d_wei_np_11) << 6 | I(sp_d_wei_np_12) << 0xc |
		I(sp_d_wei_np_10) | I(sp_d_wei_np_13) << 0x12 |
		I(sp_d_wei_np_14) << 0x18;
	d[53] = I(sp_d_wei_np_16) << 6 | I(sp_d_wei_np_17) << 0xc |
		I(sp_d_wei_np_15) | I(sp_d_wei_np_18) << 0x12 |
		I(sp_d_wei_np_19) << 0x18;
	d[54] = I(sp_d_wei_np_21) << 6 | I(sp_d_wei_np_22) << 0xc |
		I(sp_d_wei_np_20) | I(sp_d_wei_np_23) << 0x12 |
		I(sp_d_wei_np_24) << 0x18;
	d[55] = I(sp_d_wei_np_26) << 6 | I(sp_d_wei_np_27) << 0xc |
		I(sp_d_wei_np_25) | I(sp_d_wei_np_28) << 0x12 |
		I(sp_d_wei_np_29) << 0x18;
	d[56] = I(sp_d_wei_np_31) << 6 | I(sp_d_wei_np_30);
	d[57] = I(sp_ud_wei_np_1) << 6 | I(sp_ud_wei_np_2) << 0xc |
		I(sp_ud_wei_np_0) | I(sp_ud_wei_np_3) << 0x12 |
		I(sp_ud_wei_np_4) << 0x18;
	d[58] = I(sp_ud_wei_np_6) << 6 | I(sp_ud_wei_np_7) << 0xc |
		I(sp_ud_wei_np_5) | I(sp_ud_wei_np_8) << 0x12 |
		I(sp_ud_wei_np_9) << 0x18;
	d[59] = I(sp_ud_wei_np_11) << 6 | I(sp_ud_wei_np_12) << 0xc |
		I(sp_ud_wei_np_10) | I(sp_ud_wei_np_13) << 0x12 |
		I(sp_ud_wei_np_14) << 0x18;
	d[60] = I(sp_ud_wei_np_16) << 6 | I(sp_ud_wei_np_17) << 0xc |
		I(sp_ud_wei_np_15) | I(sp_ud_wei_np_18) << 0x12 |
		I(sp_ud_wei_np_19) << 0x18;
	d[61] = I(sp_ud_wei_np_21) << 6 | I(sp_ud_wei_np_22) << 0xc |
		I(sp_ud_wei_np_20) | I(sp_ud_wei_np_23) << 0x12 |
		I(sp_ud_wei_np_24) << 0x18;
	d[62] = I(sp_ud_wei_np_26) << 6 | I(sp_ud_wei_np_27) << 0xc |
		I(sp_ud_wei_np_25) | I(sp_ud_wei_np_28) << 0x12 |
		I(sp_ud_wei_np_29) << 0x18;
	d[63] = I(sp_ud_wei_np_31) << 6 | I(sp_ud_wei_np_30);
	d[107] = I(r_gain_1) << 0x10 | I(r_gain_0);
	d[108] = I(r_gain_3) << 0x10 | I(r_gain_2);
	d[109] = I(r_gain_5) << 0x10 | I(r_gain_4);
	d[110] = I(r_gain_7) << 0x10 | I(r_gain_6);
	d[111] = I(r_gain_9) << 0x10 | I(r_gain_8);
	d[112] = I(r_gain_11) << 0x10 | I(r_gain_10);
	d[113] = I(r_gain_13) << 0x10 | I(r_gain_12);
	d[114] = I(g_gain_1) << 0x10 | I(g_gain_0);
	d[115] = I(g_gain_3) << 0x10 | I(g_gain_2);
	d[116] = I(g_gain_5) << 0x10 | I(g_gain_4);
	d[117] = I(g_gain_7) << 0x10 | I(g_gain_6);
	d[118] = I(g_gain_9) << 0x10 | I(g_gain_8);
	d[119] = I(g_gain_11) << 0x10 | I(g_gain_10);
	d[120] = I(g_gain_13) << 0x10 | I(g_gain_12);
	d[121] = I(b_gain_1) << 0x10 | I(b_gain_0);
	d[122] = I(b_gain_3) << 0x10 | I(b_gain_2);
	d[123] = I(b_gain_5) << 0x10 | I(b_gain_4);
	d[124] = I(b_gain_7) << 0x10 | I(b_gain_6);
	d[125] = I(b_gain_9) << 0x10 | I(b_gain_8);
	d[126] = I(b_gain_11) << 0x10 | I(b_gain_10);
	d[127] = I(b_gain_13) << 0x10 | I(b_gain_12);
	return x->err;
}

#undef I
#undef S

int ipa_dns_pack(const struct ipa_tuning *t, uint32_t gain_log2_q16,
		 struct ipa_params_staging *cfg)
{
	struct dns_ctx x = { .t = t, .gain = gain_log2_q16 };
	int ret;

	if (!t || !cfg)
		return -EINVAL;
	ret = pack_mdns(&x, &cfg->mdns);
	if (!ret)
		ret = pack_sdns(&x, cfg->sdns);
	if (!ret)
		ret = pack_ydns(&x, cfg->ydns);
	if (!ret)
		ret = pack_ysp(&x, cfg->ysp);
	if (!ret)
		ret = pack_dmsc(&x, cfg->dmsc);
	if (ret)
		return ret;
	cfg->update |= IPA_PARAM_MDNS | IPA_PARAM_SDNS |
		       IPA_PARAM_YDNS | IPA_PARAM_YSP |
		       IPA_PARAM_DMSC;
	return 0;
}

size_t ipa_pack_ext_params(void *ext_buf,
			   const struct ipa_params_staging *src)
{
	struct t23_isp_ext_params_cfg *ext = ext_buf;
	size_t off = 0;
	uint32_t u = src->update;

	memset(ext, 0, offsetof(struct t23_isp_ext_params_cfg, data));
	ext->version = T23_ISP_EXT_PARAMS_V1;

	if ((u & IPA_PARAM_WB) &&
	    off + sizeof(struct t23_isp_ext_params_wb_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_wb_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_WB;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		blk->config = src->wb;
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_CCM) &&
	    off + sizeof(struct t23_isp_ext_params_ccm_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_ccm_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_CCM;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		blk->config = src->ccm;
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_GAMMA) &&
	    off + sizeof(struct t23_isp_ext_params_gamma_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_gamma_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_GAMMA;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		blk->config = src->gamma;
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_LSC) &&
	    off + sizeof(struct t23_isp_ext_params_lsc_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_lsc_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_LSC;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		blk->config = src->lsc;
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_MDNS) &&
	    off + sizeof(struct t23_isp_ext_params_mdns_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_mdns_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_MDNS;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		blk->config = src->mdns;
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_SDNS) &&
	    off + sizeof(struct t23_isp_ext_params_sdns_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_sdns_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_SDNS;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		memcpy(blk->words, src->sdns, sizeof(blk->words));
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_YDNS) &&
	    off + sizeof(struct t23_isp_ext_params_ydns_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_ydns_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_YDNS;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		memcpy(blk->words, src->ydns, sizeof(blk->words));
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_YSP) &&
	    off + sizeof(struct t23_isp_ext_params_ysp_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_ysp_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_YSP;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		memcpy(blk->words, src->ysp, sizeof(blk->words));
		off += sizeof(*blk);
	}
	if ((u & IPA_PARAM_DMSC) &&
	    off + sizeof(struct t23_isp_ext_params_dmsc_config) <= sizeof(ext->data)) {
		struct t23_isp_ext_params_dmsc_config *blk = (void *)(ext->data + off);

		memset(blk, 0, sizeof(*blk));
		blk->header.type = T23_ISP_EXT_PARAMS_BLOCK_TYPE_DMSC;
		blk->header.flags = T23_ISP_EXT_PARAMS_FL_BLOCK_ENABLE;
		blk->header.size = sizeof(*blk);
		memcpy(blk->words, src->dmsc, sizeof(blk->words));
		off += sizeof(*blk);
	}

	ext->data_size = off;
	return offsetof(struct t23_isp_ext_params_cfg, data) + off;
}
