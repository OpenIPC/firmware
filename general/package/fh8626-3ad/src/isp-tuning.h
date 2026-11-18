/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only */
/* Copyright (C) 2026 Difan Zhang */
#ifndef _ISP_TUNING_H
#define _ISP_TUNING_H

#include "fh8626-3ad.h"

/*
 * Gain- and illuminant-scheduled ISP parameters: black level, temporal and
 * Bayer noise reduction, luma and chroma noise reduction, edge
 * enhancement, saturation, colour correction, the highlight chroma
 * threshold and the luma tone curve. Each block is optional; a block whose
 * controls the ISP does not expose is left alone.
 */
struct isp_tuning {
	int fd;
	bool have_black, have_nr3d, have_bnr, have_ccm, have_hlr;
	bool have_ynr, have_cnr, have_apc, have_sat;
	bool written;
	bool low_light;		/* low-light profile selected */
	unsigned int profile_count;
	uint32_t motion;	/* last motion level, 0..256 */
	int32_t hlr;		/* highlight chroma threshold, 10-bit luma */
	uint16_t black[4];
	int32_t nr3d[6];	/* strength, noise, shift, motion, blend, preset */
	int32_t bnr[3];		/* strength, noise, shift */
	uint16_t ynr_th[3];	/* luma NR band thresholds */
	uint16_t ynr_str[3];	/* luma NR band strengths, Q6 */
	uint16_t ynr_noise[2];	/* luma NR noise at mid-dark and mid-grey */
	int32_t cnr_str;	/* chroma NR strength */
	uint16_t cnr_th[2];	/* chroma NR thresholds, lower then upper */
	uint16_t apc_str[2];	/* edge enhancement limit, detail and edge */
	uint16_t apc_gain[2];	/* edge enhancement gain, detail and edge */
	int32_t sat;		/* saturation policy scale, Q6 */
	int32_t ccm[9];
	uint32_t ccm_weight;	/* 0..256 between the two nearest matrices */
	unsigned int ccm_a, ccm_b;
};

int isp_tuning_init(struct isp_tuning *t, int isp_fd);
/*
 * total_gain is sensor analogue gain times ISP digital gain, Q6 (64 = 1x),
 * and again the analogue gain alone. exposure_us is the sensor integration
 * time. gain_r/g/b are the current Q9 white-balance gains. motion is the
 * smoothed share of the scene in motion, 0..256.
 */
void isp_tuning_update(struct isp_tuning *t, uint32_t total_gain, uint32_t again,
		       uint32_t exposure_us, uint32_t gain_r, uint32_t gain_g,
		       uint32_t gain_b, uint32_t motion);

#endif /* _ISP_TUNING_H */
