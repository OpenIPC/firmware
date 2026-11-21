/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
#ifndef T23_IPA_H
#define T23_IPA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/media/ingenic/t23-isp-config.h>

#define IPA_PARAM_WB		(1U << 0)
#define IPA_PARAM_CCM		(1U << 1)
#define IPA_PARAM_GAMMA		(1U << 2)
#define IPA_PARAM_LSC		(1U << 3)
#define IPA_PARAM_MDNS		(1U << 4)
#define IPA_PARAM_SDNS		(1U << 5)
#define IPA_PARAM_YDNS		(1U << 6)
#define IPA_PARAM_YSP		(1U << 7)
#define IPA_PARAM_DMSC		(1U << 8)
#define IPA_PARAM_ALL		((1U << 9) - 1U)

struct ipa_params_staging {
	uint32_t update;
	struct t23_isp_wb_config wb;
	struct t23_isp_ccm_config ccm;
	struct t23_isp_gamma_config gamma;
	struct t23_isp_lsc_config lsc;
	struct t23_isp_mdns_config mdns;
	uint32_t sdns[T23_ISP_SDNS_WORDS];
	uint32_t ydns[T23_ISP_YDNS_WORDS];
	uint32_t ysp[T23_ISP_YSP_WORDS];
	uint32_t dmsc[T23_ISP_DMSC_WORDS];
};

#define ipa_err(fmt, ...)   fprintf(stderr, "ipa: " fmt, ##__VA_ARGS__)
#define ipa_warn(fmt, ...)  fprintf(stderr, "ipa: " fmt, ##__VA_ARGS__)
#define ipa_info(fmt, ...)  fprintf(stdout, "ipa: " fmt, ##__VA_ARGS__)

#define Q16_ONE             65536.0
#define Q12_ONE             4096.0
#define Q10_ONE             1024.0
#define Q8_ONE              256.0

/* Tuning file decoded by tuning/bin2yaml.py */
struct ipa_field;

struct ipa_tuning_set {
	struct ipa_field *fields;
	unsigned int count;
	unsigned int capacity;
};

struct ipa_tuning {
	struct ipa_tuning_set set[2];	/* day, night */
	unsigned int night;
};

int ipa_tuning_load(struct ipa_tuning *t, const char *path);
void ipa_tuning_free(struct ipa_tuning *t);
/* @count 0 accepts any length; a mismatch or a missing field returns NULL. */
const int64_t *ipa_tuning_get(const struct ipa_tuning *t, const char *module,
			      const char *name, unsigned int count);
/* Vendor 9-point interpolation over log2(total gain) in Q16 */
int64_t ipa_intp9(const int64_t *v, uint32_t gain_log2_q16);

/* Sensor exposure in lines and analogue gain as log2 in Q16 */
struct ipa_sensor {
	int fd;			/* node carrying the sensor controls */
	int exposure_min;
	int exposure_max;
	int gain_index_max;
	const uint32_t *gain_log2;
	int exposure;
	int gain_index;
	double line_us;		/* exposure line period, 0 when unknown */
	int height;
	int exposure_margin;	/* frame length minus the longest exposure */
	int vblank;
	int vblank_min;		/* shortest frame AE may use */
	int vblank_max;		/* longest frame AE may use */
};

int ipa_sensor_gain_init(struct ipa_sensor *s, const char *name, int maximum);
int ipa_sensor_init(struct ipa_sensor *s, int fd, const char *name);
void ipa_sensor_frame_limits(struct ipa_sensor *s, double min_fps);
int ipa_sensor_exposure_max(const struct ipa_sensor *s, int vblank);
uint32_t ipa_sensor_gain_log2(const struct ipa_sensor *s, int index);
int ipa_sensor_gain_index(const struct ipa_sensor *s, uint32_t gain_log2_q16);
int ipa_sensor_apply(struct ipa_sensor *s, int exposure, int gain_index,
		     int vblank);
int ipa_sensor_follow(struct ipa_sensor *s, int master_fd);

#define AE_CURVE_POINTS	10

struct ipa_ae {
	unsigned int fixed_target;	/* 0 follows the tuning curve */
	double target;			/* mean zone luma, 8-bit scale */
	double curve_ev[AE_CURVE_POINTS];	/* exposure us x gain */
	double curve_target[AE_CURVE_POINTS];
	double highlight;		/* pre-gamma 98th percentile, -1 unknown */
	unsigned int zone_weight[T23_ISP_ZONES];
	double luma;			/* last measured weighted mean */
	double ev;			/* exposure lines x analogue gain */
	uint32_t gain_log2_q16;		/* applied analogue gain */
	double flicker_us;		/* light flicker period, 0 ignores it */
	unsigned int settle;		/* frames until stats follow a change */
	bool converged;
};

int ipa_ae_init(struct ipa_ae *ae, const struct ipa_tuning *t,
		unsigned int target, unsigned int mains_hz);
void ipa_ae_run(struct ipa_ae *ae, struct ipa_sensor *s,
		const struct t23_isp_stat_buffer *stats);
void ipa_ae_scale(struct ipa_ae *ae, struct ipa_sensor *s, double factor);

/* AWB on the vendor white-point grid, constrained to the Planckian locus */
struct ipa_awb_locus_point {
	double rg, bg;			/* vendor grid coordinates */
	double mired;
};

struct ipa_awb {
	double axis_rg[15], axis_bg[15];
	double weight[15][15];		/* white likelihood, rows are bg */
	double weight_dim[15][15];	/* used below 200 EV units */
	double mired[15][15];
	double spatial[15][15];
	double mult_rg, mult_bg;	/* grid coordinate = 256 * ratio * mult */
	unsigned int min_count;
	double reject_hi, reject_lo;	/* outlier ratios to the mean distance */
	double locus_tolerance;		/* allowed distance off the locus */
	struct ipa_awb_locus_point locus[64];
	unsigned int locus_points;
	double rg, bg;			/* filtered white point */
	double ct;			/* kelvin */
	uint16_t gain_r, gain_b;	/* Q10 */
	double dbg_grey_rg, dbg_grey_bg;	/* count-weighted mean, all zones */
	double dbg_mean_rg, dbg_mean_bg;	/* white-weighted, before the locus */
	bool valid;
};

int ipa_awb_init(struct ipa_awb *awb, const struct ipa_tuning *t,
		 double tolerance);
void ipa_awb_run(struct ipa_awb *awb, const struct t23_isp_stat_buffer *stats,
		 double ev);
void ipa_awb_grey(struct ipa_awb *awb, const struct t23_isp_stat_buffer *stats);

/* CCM by colour temperature and saturation by EV, gamma, lens shading */
struct ipa_color {
	double ct_list[4];
	double ccm[4][9];
	double sat_ev[9], sat[9];
	uint16_t gamma[T23_ISP_GAMMA_POINTS];
	double lsc_ct[4];
	const int64_t *lsc_lut[3];	/* A, T, D */
	unsigned int lsc_words;
	const int64_t *lsc_strength;	/* 9 points over gain */
	struct t23_isp_lsc_config lsc_base;
	bool mono;			/* zero saturation */
};

int ipa_color_init(struct ipa_color *c, const struct ipa_tuning *t);
void ipa_color_ccm(const struct ipa_color *c, double ct, double ev,
		   struct t23_isp_ccm_config *out);
void ipa_color_gamma(const struct ipa_color *c,
		     struct t23_isp_gamma_config *out);
void ipa_color_lsc(const struct ipa_color *c, double ct,
		   uint32_t gain_log2_q16, struct t23_isp_lsc_config *out);

/* IR-cut filter and infrared illuminator */
enum ipa_dn_mode {
	IPA_DN_AUTO,		/* switch on exposure */
	IPA_DN_DAY,
	IPA_DN_NIGHT,
	IPA_DN_FOLLOW,		/* track the filter set by another input */
};

struct ipa_daynight {
	enum ipa_dn_mode mode;
	int ircut_fd;		/* GPIO line request, -1 when absent */
	int state_fd;		/* completed filter move, owner only */
	int led_fd;		/* LED brightness, -1 when absent */
	double to_night;	/* exposure us x gain that starts night */
	double to_day;		/* visible-light equivalent that ends night */
	double dwell;		/* minimum seconds in a mode */
	double since;		/* time of the last switch or poll */
	double pending;		/* time a threshold was first crossed */
	double last_day;	/* time of the last switch to day */
	double polled;		/* time a following input last read the filter */
	double quick_until;	/* short hold until then, after a restart */
	double ev_dark;		/* exposure just before night started */
	double illuminator;	/* last illuminator share in 1/ev, -1 unknown */
	bool measuring;		/* night started, share not yet measured */
	bool night;
	bool changed;		/* mode changed in the last call */
};

int ipa_daynight_init(struct ipa_daynight *dn, enum ipa_dn_mode mode,
		      const char *led, double to_night, double to_day);
bool ipa_daynight_run(struct ipa_daynight *dn, double ev_us, bool settled);
void ipa_daynight_restart(struct ipa_daynight *dn);
double ipa_daynight_ev_factor(const struct ipa_daynight *dn, double ev_us);
void ipa_daynight_close(struct ipa_daynight *dn);

/* Noise reduction, sharpening and demosaic register words by gain */
int ipa_dns_pack(const struct ipa_tuning *t, uint32_t gain_log2_q16,
		 struct ipa_params_staging *cfg);
size_t ipa_pack_ext_params(void *ext_buf,
			   const struct ipa_params_staging *src);

#endif /* T23_IPA_H */
