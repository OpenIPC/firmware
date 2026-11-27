/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Open-source SmartSens SC430AI MIPI CMOS sensor driver for Novatek NT9856x
 * (2022 HDAL ABI, kflow_videocapture).
 *
 * Supported modes:
 *   0: 2688x1520 @ 30fps, 10-bit linear
 *   1: 2688x1520 @ 60fps, 10-bit linear
 *   2: 2688x1520 @ 30fps, 10-bit 2-frame Stagger HDR
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/gpio/consumer.h>

#include "sc430ai_regs.h"

#define SEN_MAX_ID		8
#define SEN_MAX_MODE		3
#define SEN_IF_TYPE_MIPI	2
#define SEN_DATA_FMT_RGB	0
#define SEN_CMDIF_TYPE_I2C	3
#define SEN_MODE_LINEAR		1
#define SEN_MODE_STAGGER_HDR	2
#define SEN_STPIX_R		0
#define SEN_PROPERTY_ALL	0x7 /* MIRROR | FLIP | CHGFPS */

#define SEN_E_NOSPT		(-17)

/* CTL_SEN_CFGID_* (2022 HDAL ABI) */
#define CFGID_SET_EXPT		0x00000001U
#define CFGID_SET_GAIN		0x00000002U
#define CFGID_GET_EXPT		0x01000001U
#define CFGID_GET_GAIN		0x01000002U
#define CFGID_GET_ATTR_BASIC	0x01000003U
#define CFGID_GET_ATTR_SIGNAL	0x01000004U
#define CFGID_GET_ATTR_CMDIF	0x01000005U
#define CFGID_GET_ATTR_IF	0x01000006U
#define CFGID_GET_FPS		0x01000008U
#define CFGID_GET_MODE_SPEED	0x01000009U
#define CFGID_GET_MODESEL	0x0100000aU
#define CFGID_GET_MODE_BASIC	0x0100000bU
#define CFGID_GET_MODE_MIPI	0x0100000dU
#define CFGID_GET_MIN_EXPT	0x01000018U
#define CFGID_FLIP_TYPE		0x02000001U
#define CFGID_USER_DEFINE1	0x03000001U
#define CFGID_USER_DEFINE2	0x03000002U

struct ctl_sen_cmd {
	u32 addr;
	u32 data_len;
	u32 data[2];
};

struct ctl_sen_chgmode_obj {
	u32 mode;
	u32 frame_rate;
};

struct ctl_sen_drv_tab {
	int (*open)(u32 id);
	int (*close)(u32 id);
	int (*sleep)(u32 id);
	int (*wakeup)(u32 id);
	int (*write_reg)(u32 id, struct ctl_sen_cmd *cmd);
	int (*read_reg)(u32 id, struct ctl_sen_cmd *cmd);
	int (*chg_mode)(u32 id, struct ctl_sen_chgmode_obj obj);
	int (*chg_fps)(u32 id, u32 fps);
	int (*set_cfg)(u32 id, u32 cfg_id, void *data);
	int (*get_cfg)(u32 id, u32 cfg_id, void *data);
};

typedef void (*ctl_sen_clk_cb)(u32 id, bool en);

struct ctl_sen_reg_sendrv {
	void (*pwr_ctrl)(u32 id, u32 flag, ctl_sen_clk_cb clk_cb);
	int (*det_plug_in)(u32 id);
	struct ctl_sen_drv_tab *drv_tab;
};

/* 2022 HDAL ABI: 0x94 (148) bytes */
struct ctl_sen_mode_basic_param {
	u32 mode;
	u32 if_type;
	u32 data_fmt;
	u32 mode_type;
	u32 dft_fps;
	u32 frame_num;
	u32 stpix;
	u32 pixel_depth;
	u32 fmt;
	u32 valid_w;
	u32 valid_h;
	u32 act_rect[4][4]; /* {x, y, w, h} x 4 */
	u32 crp_w;
	u32 crp_h;
	u32 hd_sync;
	u32 hd_period;
	u32 vd_sync;
	u32 vd_period;
	u32 ratio_h_v;
	u32 gain_min;
	u32 gain_max;
	u32 gain_prec;
};

struct ctl_sen_basic_param {
	char name[32];
	u32 vendor;
	u32 max_sen_mode;
	u32 property;
	u32 sync_timing;
};

struct ctl_sen_speed_param {
	u32 mode;
	u32 mclksrc;
	u32 mclk;
	u32 pclk;
	u32 data_rate;
};

struct ctl_sen_mipi_param {
	u32 mode;
	u32 clk_lane;
	u32 data_lane;
	u32 pad[12];
};

struct isp_sensor_ctrl {
	u32 exp_time[2];
	u32 gain_ratio[2];
};

struct sen_preset_cfg {
	u32 expt_time;
	u32 gain_ratio;
	u32 frame_rate;
	u32 pad[2];
};

struct sen_power_cfg {
	u32 mclk;
	u32 pwdn_pin;
	u32 rst_pin;
	u32 rst_time;
	u32 stable_time;
};

struct sen_i2c_cfg {
	u32 id;
	u32 addr;
};

extern int ctl_sen_reg_sendrv(char *name, struct ctl_sen_reg_sendrv *reg);
extern int ctl_sen_unreg_sendrv(char *name);

static char *sen_cfg_path = "null";
module_param(sen_cfg_path, charp, 0644);
MODULE_PARM_DESC(sen_cfg_path, "Path of cfg file");

static int sen_debug_level;
module_param(sen_debug_level, int, 0644);
MODULE_PARM_DESC(sen_debug_level, "Debug message level");

static const struct ctl_sen_basic_param basic_param = {
	.name = "sen_sc430ai",
	.vendor = 4,
	.max_sen_mode = SEN_MAX_MODE,
	.property = SEN_PROPERTY_ALL,
	.sync_timing = 0,
};

static const struct ctl_sen_mode_basic_param mode_basic_param[SEN_MAX_MODE] = {
	{
		.mode = 0,
		.if_type = SEN_IF_TYPE_MIPI,
		.data_fmt = SEN_DATA_FMT_RGB,
		.mode_type = SEN_MODE_LINEAR,
		.dft_fps = 3000,
		.frame_num = 1,
		.stpix = SEN_STPIX_R,
		.pixel_depth = 10,
		.fmt = 0,
		.valid_w = 2688,
		.valid_h = 1522,
		.act_rect = { { 0, 2, 2688, 1520 } },
		.crp_w = 2688,
		.crp_h = 1520,
		.hd_sync = 0,
		.hd_period = 1600,
		.vd_sync = 0,
		.vd_period = 1650,
		.ratio_h_v = 0x00100009,
		.gain_min = 1000,
		.gain_max = 301178,
		.gain_prec = 100,
	},
	{
		.mode = 1,
		.if_type = SEN_IF_TYPE_MIPI,
		.data_fmt = SEN_DATA_FMT_RGB,
		.mode_type = SEN_MODE_LINEAR,
		.dft_fps = 6000,
		.frame_num = 1,
		.stpix = SEN_STPIX_R,
		.pixel_depth = 10,
		.fmt = 0,
		.valid_w = 2688,
		.valid_h = 1522,
		.act_rect = { { 0, 2, 2688, 1520 } },
		.crp_w = 2688,
		.crp_h = 1520,
		.hd_sync = 0,
		.hd_period = 1600,
		.vd_sync = 0,
		.vd_period = 1650,
		.ratio_h_v = 0x00100009,
		.gain_min = 1000,
		.gain_max = 301178,
		.gain_prec = 100,
	},
	{
		.mode = 2,
		.if_type = SEN_IF_TYPE_MIPI,
		.data_fmt = SEN_DATA_FMT_RGB,
		.mode_type = SEN_MODE_STAGGER_HDR,
		.dft_fps = 3000,
		.frame_num = 2,
		.stpix = SEN_STPIX_R,
		.pixel_depth = 10,
		.fmt = 0,
		.valid_w = 2688,
		.valid_h = 1522,
		.act_rect = { { 0, 2, 2688, 1520 }, { 0, 2, 2688, 1520 } },
		.crp_w = 2688,
		.crp_h = 1520,
		.hd_sync = 0,
		.hd_period = 1600,
		.vd_sync = 0,
		.vd_period = 3300,
		.ratio_h_v = 0x00100009,
		.gain_min = 1000,
		.gain_max = 301178,
		.gain_prec = 100,
	},
};

static const struct ctl_sen_speed_param speed_param[SEN_MAX_MODE] = {
	{ .mode = 0, .mclksrc = 0, .mclk = 27000000, .pclk = 158400000, .data_rate = 79200000 },
	{ .mode = 1, .mclksrc = 0, .mclk = 27000000, .pclk = 316800000, .data_rate = 158400000 },
	{ .mode = 2, .mclksrc = 0, .mclk = 27000000, .pclk = 316800000, .data_rate = 158400000 },
};

static const struct ctl_sen_mipi_param mipi_param[SEN_MAX_MODE] = {
	{ .mode = 0, .clk_lane = 1, .data_lane = 4 },
	{ .mode = 1, .clk_lane = 1, .data_lane = 4 },
	{ .mode = 2, .clk_lane = 1, .data_lane = 4, .pad = { [7] = 1, [11] = 3 } },
};

static const u32 i2c_table_default[16] = {
	0, 0x60, 1, 0xff, 2, 0xff, 3, 0xff,
	4, 0xff, 5, 0xff, 0, 0, 0, 0,
};

struct sc430ai_state {
	struct i2c_adapter *adap;
	u32 cur_mode;
	u32 cur_fps;
	u32 chg_fps;
	u32 direction;
	u32 compensation_gain[2];
	struct isp_sensor_ctrl ctrl;
	struct sen_preset_cfg preset;
	struct sen_power_cfg power;
	struct sen_i2c_cfg i2c;
};

static struct sc430ai_state g_sen[SEN_MAX_ID];

static int sc430ai_i2c_write(u32 id, u16 reg, u8 val)
{
	struct i2c_adapter *adap = g_sen[id].adap;
	u8 buf[3] = { reg >> 8, reg & 0xff, val };
	struct i2c_msg msg = {
		.addr = g_sen[id].i2c.addr,
		.flags = 0,
		.len = 3,
		.buf = buf,
	};
	int ret;

	if (!adap)
		return -ENODEV;
	ret = i2c_transfer(adap, &msg, 1);
	return (ret == 1) ? 0 : (ret < 0 ? ret : -EIO);
}

static int sc430ai_i2c_read(u32 id, u16 reg, u8 *val)
{
	struct i2c_adapter *adap = g_sen[id].adap;
	u8 rbuf[2] = { reg >> 8, reg & 0xff };
	struct i2c_msg msgs[2] = {
		{ .addr = g_sen[id].i2c.addr, .flags = 0, .len = 2, .buf = rbuf },
		{ .addr = g_sen[id].i2c.addr, .flags = I2C_M_RD, .len = 1, .buf = val },
	};
	int ret;

	if (!adap)
		return -ENODEV;
	ret = i2c_transfer(adap, msgs, 2);
	return (ret == 2) ? 0 : (ret < 0 ? ret : -EIO);
}

/* Half-line period in 0.1 us units (SC430AI exposure registers use half-lines, 2*VTS/frame). */
static u32 sc430ai_calc_rowtime_10x(u32 id, u32 mode)
{
	u32 pclk_100k;

	if (mode >= SEN_MAX_MODE)
		mode = g_sen[id].cur_mode;
	pclk_100k = speed_param[mode].pclk / 100000U;
	if (!pclk_100k)
		return 1;
	return (100U * mode_basic_param[mode].hd_period) / pclk_100k;
}

static u32 sc430ai_calc_vd(u32 id, u32 fps)
{
	u32 mode = g_sen[id].cur_mode;
	u32 dft_fps = mode_basic_param[mode].dft_fps;
	u32 dft_vd = mode_basic_param[mode].vd_period;
	u32 vd;

	if (!fps || fps > dft_fps)
		fps = dft_fps;
	vd = (dft_fps * dft_vd) / fps;
	if (vd < dft_vd)
		vd = dft_vd;
	if (vd > 0x7fff)
		vd = 0x7fff;
	g_sen[id].cur_fps = fps;
	return vd;
}

static int sc430ai_set_flip(u32 id, u32 dir)
{
	u8 val = 0;
	int ret;

	ret = sc430ai_i2c_read(id, 0x3221, &val);
	if (ret)
		return ret;
	if (dir & 1)
		val |= 0x06;
	else
		val &= ~0x06;
	if (dir & 2)
		val |= 0x60;
	else
		val &= ~0x60;
	g_sen[id].direction = dir & 3;
	return sc430ai_i2c_write(id, 0x3221, val);
}

static u32 sc430ai_align_hdr_line(u32 line)
{
	if (line & 3) {
		line &= ~3U;
		if (line <= 4)
			line = 5;
	}
	return line;
}

static int sc430ai_set_expt(u32 id, const struct isp_sensor_ctrl *ctrl)
{
	u32 mode = g_sen[id].cur_mode;
	const struct ctl_sen_mode_basic_param *mb = &mode_basic_param[mode];
	u32 rowtime = sc430ai_calc_rowtime_10x(id, mode);
	u32 line[2] = { 0, 0 }, req_line[2] = { 0, 0 };
	u32 total_lines, fps, vd, max_long;
	u32 i, n = mb->frame_num ? mb->frame_num : 1;
	int ret;

	g_sen[id].ctrl.exp_time[0] = ctrl->exp_time[0];
	g_sen[id].ctrl.exp_time[1] = ctrl->exp_time[1];

	for (i = 0; i < n && i < 2; i++) {
		line[i] = (ctrl->exp_time[i] * 10U) / rowtime;
		if (mb->mode_type == SEN_MODE_LINEAR) {
			if (line[i] < 3)
				line[i] = 3;
		} else {
			if (line[i] < 5)
				line[i] = 5;
		}
		req_line[i] = line[i];
	}

	if (mb->mode_type == SEN_MODE_STAGGER_HDR && line[1] > 0x180)
		line[1] = 0x180;

	total_lines = (mb->mode_type == SEN_MODE_STAGGER_HDR) ?
		      (line[0] + line[1]) : line[0];
	fps = (total_lines * rowtime) / 10U;
	fps = fps ? (100000000U / fps) : mb->dft_fps;
	if (fps > g_sen[id].chg_fps)
		fps = g_sen[id].chg_fps;
	if (!fps)
		fps = mb->dft_fps;
	vd = (mb->dft_fps * mb->vd_period) / fps;
	if (vd < mb->vd_period)
		vd = mb->vd_period;
	if (vd > 0x7fff)
		vd = 0x7fff;
	g_sen[id].cur_fps = fps;

	ret = sc430ai_i2c_write(id, 0x320e, (vd >> 8) & 0x7f);
	ret |= sc430ai_i2c_write(id, 0x320f, vd & 0xff);

	if (mb->mode_type == SEN_MODE_LINEAR) {
		max_long = (vd - 5) * 2;
		if (line[0] > 0xffff)
			line[0] = 0xffff;
		if (line[0] > max_long)
			line[0] = max_long;
		g_sen[id].compensation_gain[0] = (100U * req_line[0]) / line[0];

		ret |= sc430ai_i2c_write(id, 0x3e00, (line[0] >> 12) & 0x0f);
		ret |= sc430ai_i2c_write(id, 0x3e01, (line[0] >> 4) & 0xff);
		ret |= sc430ai_i2c_write(id, 0x3e02, (line[0] << 4) & 0xf0);
	} else {
		if (line[0] > 0xffff)
			line[0] = 0xffff;
		if (line[1] > 0x180)
			line[1] = 0x180;
		max_long = (vd > 0xd0) ? ((vd - 0xd0) * 2) : 5;
		if (line[0] > max_long)
			line[0] = max_long;
		line[0] = sc430ai_align_hdr_line(line[0]);
		line[1] = sc430ai_align_hdr_line(line[1]);
		g_sen[id].compensation_gain[0] = (100U * req_line[0]) / line[0];
		g_sen[id].compensation_gain[1] = (100U * req_line[1]) / line[1];

		ret |= sc430ai_i2c_write(id, 0x3e00, (line[0] >> 12) & 0x0f);
		ret |= sc430ai_i2c_write(id, 0x3e01, (line[0] >> 4) & 0xff);
		ret |= sc430ai_i2c_write(id, 0x3e02, (line[0] << 4) & 0xf0);
		ret |= sc430ai_i2c_write(id, 0x3e22, (line[1] >> 12) & 0x0f);
		ret |= sc430ai_i2c_write(id, 0x3e04, (line[1] >> 4) & 0xff);
		ret |= sc430ai_i2c_write(id, 0x3e05, (line[1] << 4) & 0xf0);
	}
	return ret;
}

static void sc430ai_calc_gain(u32 gain, u32 comp, u32 min_g, u32 max_g,
			      u8 *again_reg, u8 *dgain_coarse, u8 *dgain_fine)
{
	u32 dgain_1000, fine;

	if (comp >= 100)
		gain = (gain * comp) / 100U;
	if (gain < min_g)
		gain = min_g;
	if (gain > max_g)
		gain = max_g;

	*dgain_coarse = 0;

	/* SmartSens SC430AI DCG analog gain steps (2.39x DCG jump at 0x40) */
	if (gain < 2000) {
		*again_reg = 0x00;
		dgain_1000 = gain;
	} else if (gain <= 2389) {
		*again_reg = 0x01;
		dgain_1000 = gain / 2U;
	} else if (gain <= 4779) {
		*again_reg = 0x40;
		dgain_1000 = (gain * 1000U) / 2390U;
	} else if (gain <= 9559) {
		*again_reg = 0x48;
		dgain_1000 = (gain * 1000U) / 4780U;
	} else if (gain <= 19119) {
		*again_reg = 0x49;
		dgain_1000 = (gain * 1000U) / 9560U;
	} else if (gain <= 38239) {
		*again_reg = 0x4b;
		dgain_1000 = (gain * 1000U) / 19120U;
	} else if (gain <= 76479) {
		*again_reg = 0x4f;
		dgain_1000 = (gain * 1000U) / 38240U;
	} else {
		*again_reg = 0x5f;
		dgain_1000 = (gain * 1000U) / 76480U;
		if (gain > 152959) {
			*dgain_coarse = 0x01;
			dgain_1000 /= 2U;
		}
	}

	fine = (dgain_1000 * 128U) / 1000U;
	if (fine & 3)
		fine = ((fine - 0x80) & ~3U) + 0x80;
	if (fine < 0x80)
		fine = 0x80;
	if (fine > 0xfc)
		fine = 0xfc;
	*dgain_fine = (u8)fine;
}

static int sc430ai_set_gain(u32 id, const struct isp_sensor_ctrl *ctrl)
{
	u32 mode = g_sen[id].cur_mode;
	const struct ctl_sen_mode_basic_param *mb = &mode_basic_param[mode];
	u8 ag[2] = { 0, 0 }, dc[2] = { 0, 0 }, df[2] = { 0x80, 0x80 };
	u32 i, n = mb->frame_num ? mb->frame_num : 1;
	int ret;

	g_sen[id].ctrl.gain_ratio[0] = ctrl->gain_ratio[0];
	g_sen[id].ctrl.gain_ratio[1] = ctrl->gain_ratio[1];

	for (i = 0; i < n && i < 2; i++)
		sc430ai_calc_gain(ctrl->gain_ratio[i], g_sen[id].compensation_gain[i],
				  mb->gain_min, mb->gain_max, &ag[i], &dc[i], &df[i]);

	ret = sc430ai_i2c_write(id, 0x3e09, ag[0]);
	ret |= sc430ai_i2c_write(id, 0x3e06, dc[0]);
	ret |= sc430ai_i2c_write(id, 0x3e07, df[0]);
	if (mb->mode_type == SEN_MODE_STAGGER_HDR) {
		ret |= sc430ai_i2c_write(id, 0x3e13, ag[1]);
		ret |= sc430ai_i2c_write(id, 0x3e10, dc[1]);
		ret |= sc430ai_i2c_write(id, 0x3e11, df[1]);
	}
	return ret;
}

static void sc430ai_pwr_ctrl(u32 id, u32 flag, ctl_sen_clk_cb clk_cb)
{
	struct sen_power_cfg *p;
	struct gpio_desc *gd;

	if (id >= SEN_MAX_ID)
		return;
	p = &g_sen[id].power;

	if (flag == 0) { /* TURN_ON */
		if (p->pwdn_pin != 0xffffffffU) {
			gd = gpio_to_desc(p->pwdn_pin);
			if (gd) {
				gpiod_direction_output_raw(gd, 0);
				gpiod_set_raw_value(gd, 0);
				gpiod_set_raw_value(gd, 1);
			}
		}
		if (clk_cb)
			clk_cb(id, true);
		if (p->rst_pin != 0xffffffffU) {
			msleep(p->stable_time);
			gd = gpio_to_desc(p->rst_pin);
			if (gd) {
				gpiod_direction_output_raw(gd, 0);
				gpiod_set_raw_value(gd, 0);
				msleep(p->rst_time);
				gpiod_set_raw_value(gd, 1);
				msleep(p->stable_time);
			}
		}
	} else { /* TURN_OFF */
		if (p->rst_pin != 0xffffffffU) {
			gd = gpio_to_desc(p->rst_pin);
			if (gd)
				gpiod_set_raw_value(gd, 0);
		}
		if (clk_cb)
			clk_cb(id, false);
		if (p->pwdn_pin != 0xffffffffU) {
			gd = gpio_to_desc(p->pwdn_pin);
			if (gd)
				gpiod_set_raw_value(gd, 0);
		}
	}
}

static int sc430ai_open(u32 id)
{
	if (id >= SEN_MAX_ID)
		return -EINVAL;
	if (g_sen[id].adap)
		return 0;
	g_sen[id].adap = i2c_get_adapter(g_sen[id].i2c.id);
	if (!g_sen[id].adap) {
		pr_err("nvt_sen_sc430ai: i2c adapter %u not found\n", g_sen[id].i2c.id);
		return -ENODEV;
	}
	return 0;
}

static int sc430ai_close(u32 id)
{
	if (id >= SEN_MAX_ID)
		return -EINVAL;
	if (g_sen[id].adap) {
		i2c_put_adapter(g_sen[id].adap);
		g_sen[id].adap = NULL;
	}
	return 0;
}

static int sc430ai_sleep(u32 id)
{
	return 0;
}

static int sc430ai_wakeup(u32 id)
{
	return 0;
}

static int sc430ai_write_reg(u32 id, struct ctl_sen_cmd *cmd)
{
	if (id >= SEN_MAX_ID || !cmd)
		return -EINVAL;
	return sc430ai_i2c_write(id, (u16)cmd->addr, (u8)cmd->data[0]);
}

static int sc430ai_read_reg(u32 id, struct ctl_sen_cmd *cmd)
{
	u8 val = 0;
	int ret;

	if (id >= SEN_MAX_ID || !cmd)
		return -EINVAL;
	ret = sc430ai_i2c_read(id, (u16)cmd->addr, &val);
	if (!ret)
		cmd->data[0] = val;
	return ret;
}

static int sc430ai_chg_fps(u32 id, u32 fps)
{
	u32 vd;
	int ret;

	if (id >= SEN_MAX_ID)
		return -EINVAL;
	g_sen[id].chg_fps = fps;
	vd = sc430ai_calc_vd(id, fps);
	ret = sc430ai_i2c_write(id, 0x320e, (vd >> 8) & 0x7f);
	ret |= sc430ai_i2c_write(id, 0x320f, vd & 0xff);
	return ret;
}

static int sc430ai_chg_mode(u32 id, struct ctl_sen_chgmode_obj obj)
{
	const struct sc430ai_reg *tbl;
	size_t i, n;
	struct isp_sensor_ctrl ctrl;
	int ret = 0;

	if (id >= SEN_MAX_ID)
		return -EINVAL;
	if (obj.mode >= SEN_MAX_MODE)
		obj.mode = 0;

	g_sen[id].cur_mode = obj.mode;
	g_sen[id].chg_fps = obj.frame_rate ? obj.frame_rate : mode_basic_param[obj.mode].dft_fps;
	g_sen[id].cur_fps = g_sen[id].chg_fps;

	if (obj.mode == 1) {
		tbl = sc430ai_mode_2;
		n = ARRAY_SIZE(sc430ai_mode_2);
	} else if (obj.mode == 2) {
		tbl = sc430ai_mode_3;
		n = ARRAY_SIZE(sc430ai_mode_3);
	} else {
		tbl = sc430ai_mode_1;
		n = ARRAY_SIZE(sc430ai_mode_1);
	}

	for (i = 0; i < n; i++) {
		if (tbl[i].addr == SEN_CMD_DELAY) {
			msleep(tbl[i].val);
		} else if (tbl[i].addr == SEN_CMD_SETVD) {
			ret |= sc430ai_chg_fps(id, g_sen[id].chg_fps);
		} else if (tbl[i].addr == SEN_CMD_PRESET) {
			memset(&ctrl, 0, sizeof(ctrl));
			ctrl.exp_time[0] = g_sen[id].preset.expt_time;
			ctrl.exp_time[1] = g_sen[id].preset.expt_time;
			ctrl.gain_ratio[0] = g_sen[id].preset.gain_ratio;
			ctrl.gain_ratio[1] = g_sen[id].preset.gain_ratio;
			ret |= sc430ai_set_expt(id, &ctrl);
			ret |= sc430ai_set_gain(id, &ctrl);
		} else if (tbl[i].addr == SEN_CMD_DIRECTION) {
			ret |= sc430ai_set_flip(id, g_sen[id].direction);
		} else {
			ret |= sc430ai_i2c_write(id, (u16)tbl[i].addr, tbl[i].val);
		}
	}

	return ret;
}

static int sc430ai_set_info(u32 id, u32 cfg_id, void *data)
{
	if (id >= SEN_MAX_ID || !data)
		return -EINVAL;

	switch (cfg_id) {
	case CFGID_SET_EXPT:
		return sc430ai_set_expt(id, data);
	case CFGID_SET_GAIN:
		return sc430ai_set_gain(id, data);
	case CFGID_FLIP_TYPE:
		return sc430ai_set_flip(id, *(u32 *)data);
	case CFGID_USER_DEFINE1:
		memcpy(&g_sen[id].preset, data, sizeof(g_sen[id].preset));
		return 0;
	default:
		return SEN_E_NOSPT;
	}
}

static int sc430ai_get_info(u32 id, u32 cfg_id, void *data)
{
	u32 mode;

	if (id >= SEN_MAX_ID || !data)
		return -EINVAL;

	switch (cfg_id) {
	case CFGID_GET_EXPT: {
		struct isp_sensor_ctrl *c = data;

		c->exp_time[0] = g_sen[id].ctrl.exp_time[0];
		c->exp_time[1] = g_sen[id].ctrl.exp_time[1];
		return 0;
	}
	case CFGID_GET_GAIN: {
		struct isp_sensor_ctrl *c = data;

		c->gain_ratio[0] = g_sen[id].ctrl.gain_ratio[0];
		c->gain_ratio[1] = g_sen[id].ctrl.gain_ratio[1];
		return 0;
	}
	case CFGID_GET_ATTR_BASIC:
		memcpy(data, &basic_param, sizeof(basic_param));
		return 0;
	case CFGID_GET_ATTR_SIGNAL:
		memset(data, 0, 24);
		return 0;
	case CFGID_GET_ATTR_CMDIF: {
		u8 *out = data;

		*(u32 *)out = SEN_CMDIF_TYPE_I2C;
		memcpy(out + 4, i2c_table_default, sizeof(i2c_table_default));
		out[0x08] = (u8)g_sen[id].i2c.addr;
		*(u32 *)(out + 0x34) = i2c_table_default[0];
		out[0x38] = (u8)g_sen[id].i2c.addr;
		*(u32 *)(out + 0x3c) = g_sen[id].i2c.id;
		return 0;
	}
	case CFGID_GET_ATTR_IF:
		return (*(u32 *)data == SEN_IF_TYPE_MIPI) ? 0 : SEN_E_NOSPT;
	case CFGID_GET_FPS: {
		u32 *out = data;

		out[0] = g_sen[id].cur_fps;
		out[1] = g_sen[id].chg_fps;
		return 0;
	}
	case CFGID_GET_MODE_SPEED: {
		struct ctl_sen_speed_param *out = data;

		mode = out->mode < SEN_MAX_MODE ? out->mode : 0;
		*out = speed_param[mode];
		if (g_sen[id].power.mclk <= 2)
			out->mclksrc = g_sen[id].power.mclk + 1;
		else
			out->mclksrc = g_sen[id].power.mclk;
		return 0;
	}
	case CFGID_GET_MODESEL: {
		u32 *p = data;
		u32 fps = p[0], w = p[1], h = p[2], if_type = p[3], fmt = p[4], fnum = p[5];

		if (if_type != SEN_IF_TYPE_MIPI || fmt != SEN_DATA_FMT_RGB)
			return 0;
		if (fnum == 2 && w <= 2688 && h <= 1520 && fps <= 3000)
			p[13] = 2; /* 2-frame Stagger HDR */
		else if (w <= 2688 && h <= 1520 && fps <= 3000)
			p[13] = 0; /* 30fps linear */
		else if (w <= 2688 && h <= 1520 && fps <= 6000)
			p[13] = 1; /* 60fps linear */
		else
			p[13] = 0;
		return 0;
	}
	case CFGID_GET_MODE_BASIC:
		mode = *(u32 *)data;
		if (mode >= SEN_MAX_MODE)
			mode = 0;
		memcpy(data, &mode_basic_param[mode], sizeof(mode_basic_param[0]));
		return 0;
	case CFGID_GET_MODE_MIPI:
		mode = *(u32 *)data;
		if (mode >= SEN_MAX_MODE)
			mode = 0;
		memcpy(data, &mipi_param[mode], sizeof(mipi_param[0]));
		return 0;
	case CFGID_GET_MIN_EXPT: {
		/* HDAL 2022 ABI: out[1] is min exposure in 0.1 us (min_line * rowtime_10x), matching nvt_sen_sc401ai. */
		u32 *out = data;
		u32 rt, min_line;

		mode = out[0] < SEN_MAX_MODE ? out[0] : g_sen[id].cur_mode;
		rt = sc430ai_calc_rowtime_10x(id, mode);
		min_line = (mode_basic_param[mode].mode_type == SEN_MODE_LINEAR) ? 2 : 4;
		out[2] = min_line;
		out[1] = min_line * rt;
		return 0;
	}
	case CFGID_FLIP_TYPE: {
		u8 val = 0;
		u32 dir = 0;

		if (sc430ai_i2c_read(id, 0x3221, &val) == 0) {
			if (val & 0x06)
				dir |= 1;
			if (val & 0x60)
				dir |= 2;
		} else {
			dir = g_sen[id].direction;
		}
		*(u32 *)data = dir;
		return 0;
	}
	case CFGID_USER_DEFINE2:
		*(u32 *)data = sc430ai_calc_rowtime_10x(id, g_sen[id].cur_mode) / 10U + 1;
		return 0;
	default:
		return SEN_E_NOSPT;
	}
}

static struct ctl_sen_drv_tab sc430ai_drv_tab = {
	.open = sc430ai_open,
	.close = sc430ai_close,
	.sleep = sc430ai_sleep,
	.wakeup = sc430ai_wakeup,
	.write_reg = sc430ai_write_reg,
	.read_reg = sc430ai_read_reg,
	.chg_mode = sc430ai_chg_mode,
	.chg_fps = sc430ai_chg_fps,
	.set_cfg = sc430ai_set_info,
	.get_cfg = sc430ai_get_info,
};

static void sc430ai_parse_cfg(const char *path)
{
	struct file *f;
	char *buf, *p, *line;
	loff_t pos = 0;
	ssize_t len;

	if (!path || !path[0] || !strcmp(path, "null") || !strcmp(path, "NULL"))
		return;
	f = filp_open(path, O_RDONLY, 0);
	if (IS_ERR(f))
		return;
	buf = kzalloc(4096, GFP_KERNEL);
	if (!buf) {
		filp_close(f, NULL);
		return;
	}
	len = kernel_read(f, buf, 4095, &pos);
	filp_close(f, NULL);
	if (len <= 0) {
		kfree(buf);
		return;
	}

	p = buf;
	while ((line = strsep(&p, "\r\n")) != NULL) {
		char *eq, *k, *v, *hash;
		unsigned long val;
		u32 id = 0;

		hash = strchr(line, '#');
		if (hash)
			*hash = '\0';
		eq = strchr(line, '=');
		if (!eq)
			continue;
		*eq = '\0';
		k = strim(line);
		v = strim(eq + 1);
		if (strncmp(k, "id_", 3) != 0 || k[3] < '0' || k[3] >= '0' + SEN_MAX_ID || k[4] != '_')
			continue;
		id = k[3] - '0';
		k += 5;
		if (kstrtoul(v, 0, &val))
			continue;

		if (!strcmp(k, "i2c_id"))
			g_sen[id].i2c.id = val;
		else if (!strcmp(k, "i2c_addr"))
			g_sen[id].i2c.addr = val;
		else if (!strcmp(k, "mclk"))
			g_sen[id].power.mclk = val;
		else if (!strcmp(k, "pwdn_pin"))
			g_sen[id].power.pwdn_pin = val;
		else if (!strcmp(k, "rst_pin"))
			g_sen[id].power.rst_pin = val;
		else if (!strcmp(k, "rst_time"))
			g_sen[id].power.rst_time = val;
		else if (!strcmp(k, "stable_time"))
			g_sen[id].power.stable_time = val;
		else if (!strcmp(k, "mirror"))
			g_sen[id].direction = (g_sen[id].direction & ~1U) | (val ? 1U : 0U);
		else if (!strcmp(k, "flip"))
			g_sen[id].direction = (g_sen[id].direction & ~2U) | (val ? 2U : 0U);
		else if (!strcmp(k, "expt_time"))
			g_sen[id].preset.expt_time = val;
		else if (!strcmp(k, "gain_ratio"))
			g_sen[id].preset.gain_ratio = val;
	}
	kfree(buf);
}

static int __init sc430ai_init(void)
{
	struct ctl_sen_reg_sendrv reg = {
		.pwr_ctrl = sc430ai_pwr_ctrl,
		.det_plug_in = NULL,
		.drv_tab = &sc430ai_drv_tab,
	};
	int i, ret;

	BUILD_BUG_ON(sizeof(struct ctl_sen_mode_basic_param) != 0x94);
	BUILD_BUG_ON(sizeof(struct ctl_sen_basic_param) != 0x30);
	BUILD_BUG_ON(sizeof(struct ctl_sen_speed_param) != 0x14);
	BUILD_BUG_ON(sizeof(struct ctl_sen_mipi_param) != 0x3c);

	for (i = 0; i < SEN_MAX_ID; i++) {
		g_sen[i].cur_fps = 3000;
		g_sen[i].chg_fps = 3000;
		g_sen[i].compensation_gain[0] = 100;
		g_sen[i].compensation_gain[1] = 100;
		g_sen[i].preset.expt_time = 10000;
		g_sen[i].preset.gain_ratio = 1000;
		g_sen[i].preset.frame_rate = 3000;
		g_sen[i].power.mclk = 0;
		g_sen[i].power.pwdn_pin = 0xffffffffU;
		g_sen[i].power.rst_pin = 0x41; /* S_GPIO_1 */
		g_sen[i].power.rst_time = 1;
		g_sen[i].power.stable_time = 1;
		g_sen[i].i2c.id = 0;
		g_sen[i].i2c.addr = 0x30;
	}

	sc430ai_parse_cfg(sen_cfg_path);

	ret = ctl_sen_reg_sendrv("nvt_sen_sc430ai", &reg);
	if (ret)
		pr_err("nvt_sen_sc430ai: ctl_sen_reg_sendrv failed (%d)\n", ret);
	else
		pr_info("nvt_sen_sc430ai: registered (i2c=%u@0x%02x rst=0x%x, 3 modes)\n",
			g_sen[0].i2c.id, g_sen[0].i2c.addr, g_sen[0].power.rst_pin);
	return ret;
}

static void __exit sc430ai_exit(void)
{
	int i;

	ctl_sen_unreg_sendrv("nvt_sen_sc430ai");
	for (i = 0; i < SEN_MAX_ID; i++)
		sc430ai_close(i);
}

module_init(sc430ai_init);
module_exit(sc430ai_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenIPC");
MODULE_DESCRIPTION("SmartSens SC430AI sensor driver (2022 HDAL ABI)");
MODULE_VERSION("1.0.0");
