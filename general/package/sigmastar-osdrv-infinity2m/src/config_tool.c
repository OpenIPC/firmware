/*
 * config_tool -- the memory map, display timings and PQ defaults of the
 * SSR621Q (infinity2m) NVR, for SigmaStar's mi_sys.
 *
 * OpenIPC.org | 2026 | MIT License
 *
 * While it loads, mi_sys runs <default_config_path>/config_tool twice and
 * waits for it (call_usermodehelper, UMH_WAIT_PROC):
 *
 *   argv = { "<dir>/load_mmap", "<dir>/mmap.ini" }   memory map
 *   argv = { "<dir>/load_config" }                   panel timings, PQ
 *
 * Without it mi_sys never finishes loading. The SDK's own config_tool parses
 * the SDK board files (mmap.ini, model/Customer_1.ini, board.ini,
 * panel/<timing>.ini, DLC/DLC.ini, ColorMatrix/ColorMatrix.ini,
 * PQ_general/PQConfig.ini). This one carries what it makes of them for this
 * board as tables and writes the same data to the same /proc/mi_modules/common
 * files, in the same order:
 *
 *   memory_info    text "total miu0 miu1 boundary items 4k_align miu1_enable"
 *   mmap_info      text "name gid addr size layer align type miu cma", one
 *                  write per entry, the file offset being the entry's index
 *   config_info    the 441896-byte MI_SYSCFG_Config_t, in one write
 *   pq_info        the first 32 bytes of MI_SYSCFG_PQConfig_t (DBC values,
 *                  the dynamic NR tables' rows and columns)
 *   noise_table, motion_table, motion_hdmi_dtv_table, motion_comp_pc_table,
 *   misc_table, misc_luma_table, misc_param_table: the NR tables, rows*cols
 *
 * What the kernel uses: mi_sys hands the memory map entries to fbdev, mi_gfx,
 * mi_ao, mi_ai, mi_vdec, mi_venc and mi_disp by name (MI_SYSCFG_GetMmapInfo),
 * and mi_disp looks the output timing up in config_info's panel table
 * (MI_SYSCFG_GetPanelInfo, by EN_DISPLAYTIMING_RES_TYPE). The rest of
 * config_info and the PQ data are kept by mi_sys and read by nothing but
 * their /proc files; they are written all the same.
 *
 * Layouts are the SDK's (the DWARF of its config_tool and mi_sys.ko);
 * values are what its config_tool writes for this board. Where config_info
 * differs from it:
 *   - m_pCurrentPanelInfo, a pointer into the SDK tool's own memory, is 0;
 *   - m_stMatrx is all 0: the SDK tool fails to parse ColorMatrix.ini and
 *     leaves leftovers of its stack there (they change from run to run);
 *   - m_PQPathName is empty: the SDK tool writes "/config/pq/" there, a
 *     directory of PQ binaries nothing reads.
 * The kernel reads none of these three.
 *
 * config_tool load_mmap | load_config   runs one step by hand
 */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROC "/proc/mi_modules/common/"

/* ------------------------------------------------------------------------
 * Memory map: 256 MiB of DDR on MIU0. Linux gets the first 244 MiB
 * (LX_MEM= in the bootargs), the media blocks the rest.
 */
#define MEM_TOTAL	0x10000000u
#define MEM_MIU0	0x10000000u
#define MEM_MIU1	0u
#define MIU_BOUNDARY	0x40000000u	/* MIU_INTERVAL */
#define MEM_4K_ALIGN	1

/* memory type: MIU0 | UNCACHE */
#define UNCACHE		(1u << 2)

static const struct mmap_entry {
	const char *name;
	uint32_t addr, size;
	uint8_t layer;
	uint32_t align, type, miu, cma_hid;
} mmap_table[] = {
	{ "E_LX_MEM",            0x00000000, 0x0f400000, 0, 0x1000, UNCACHE, 0, 0 },
	{ "E_MMAP_ID_DUMMY2",    0x0f400000, 0x00400000, 0, 0,      UNCACHE, 0, 0 },
	{ "E_MMAP_ID_CMDQ",      0x0f400000, 0x00100000, 1, 0x1000, UNCACHE, 0, 0 },
	{ "E_MMAP_ID_GE_VQ",     0x0f500000, 0x00020000, 1, 0x1000, UNCACHE, 0, 0 },
	{ "E_MMAP_ID_AO",        0x0f520000, 0x0004b000, 1, 0x1000, UNCACHE, 0, 0 },
	{ "E_MMAP_ID_AI",        0x0f56b000, 0x0004b000, 1, 0x1000, UNCACHE, 0, 0 },
	{ "E_MMAP_ID_HW_CURSOR", 0x0f5b6000, 0x00020000, 1, 0x1000, UNCACHE, 0, 0 },
	{ "E_MMAP_ID_FB",        0x0f800000, 0x00800000, 0, 0x1000, UNCACHE, 0, 0 },
};
#define N_MMAP (sizeof(mmap_table) / sizeof(mmap_table[0]))

/* ------------------------------------------------------------------------
 * config_info: MI_SYSCFG_Config_t, 441896 bytes, little endian. Only the
 * members written here are named; everything else is 0.
 */
#define CFG_SIZE		441896
#define CFG_HDMITX_ANALOG	900	/* HDMITx_Analog_Param_t[30] */
#define CFG_DLC_TABLE_COUNT	13168	/* MI_U8 */
#define CFG_PANEL_NAME		13169	/* char[256], the default panel */
#define CFG_PANEL_COUNT		13428	/* int */
#define CFG_PANELS		13436	/* PanelInfo_t[100] */
#define CFG_BACKLIGHT_PWM	43936	/* PanelBacklightPWMInfo */
#define CFG_SAW_TYPE		43976	/* SawArchitecture */
#define CFG_MIRROR_MODE		44493	/* MI_U8 */
#define CFG_FBL_THRESHOLD	437840	/* MI_U32 */
#define CFG_DLC			438364	/* MAPI_XC_DLC_init[10] */

/* PanelInfo_t: MAPI_PanelType, then link type, timing and HDMI flag */
struct panel {
	char name[128];
	uint8_t dither, pad0[3];
	uint32_t link_type;		/* 7 LINK_DAC_I, 8 LINK_DAC_P */
	uint8_t dual_port, swap_port, swap_odd_ml, swap_even_ml;
	uint8_t swap_odd_rb, swap_even_rb, swap_lvds_pol, swap_lvds_ch;
	uint8_t pdp_10bit, lvds_ti_mode, dclk_delay, inv_dclk;
	uint8_t inv_de, inv_hsync, inv_vsync, dclk_current;
	uint8_t de_current, odd_data_current, even_data_current, pad1;
	uint16_t on_timing1, on_timing2, off_timing1, off_timing2;
	uint8_t hsync_width, pad2;
	uint16_t hsync_back_porch;
	uint8_t vsync_width, vsync_back_porch;
	uint16_t hstart, vstart, width, height;
	uint16_t htotal_max, htotal, htotal_min;
	uint16_t vtotal_max, vtotal, vtotal_min;
	uint8_t dclk_max_u8, dclk_u8, dclk_min_u8, pad3;
	uint16_t ss_step, ss_span;
	uint8_t dimming_ctl, max_pwm, min_pwm, deinter_mode, pad4[2];
	uint32_t aspect_ratio;		/* 1: 16:9 */
	uint16_t lvds_tx_swap, pad5;
	uint32_t ti_bit_mode;		/* 2: TI_8BIT_MODE */
	uint32_t output_bit_mode;	/* 0: OUTPUT_10BIT_MODE */
	uint8_t swap_odd_rg, swap_even_rg, swap_odd_gb, swap_even_gb;
	uint8_t double_clk, pad6[3];
	uint32_t max_set, min_set;	/* LPLL limits */
	uint32_t out_timing_mode;	/* 0 E_PNL_CHG_DCLK, 2 E_PNL_CHG_VTOTAL */
	uint8_t noise_dither, freerun_3d, freerun_2d, reverse;
	uint8_t sg_panel, xc_120hz_sg_panel;
	uint16_t dclk_max, dclk, dclk_min;	/* MHz */
	struct {			/* ST_MAPI_PANEL_CFD_ATTRIBUTE */
		uint16_t rx, ry, gx, gy, bx, by, wx, wy;
		uint16_t max_lum, med_lum, min_lum;
		uint8_t color_format, color_data_format;
		uint8_t full_range, linear_rgb, customer_primaries, pad;
		uint16_t source_wx, source_wy;
	} cfd;
	uint16_t mirror, mirror_h, mirror_v, pad7;
	uint16_t link_ext_type, pad8;
	uint32_t timing;		/* EN_DISPLAYTIMING_RES_TYPE */
	uint8_t hdmi_tx, pad9[3];
};
_Static_assert(sizeof(struct panel) == 304, "PanelInfo_t");
_Static_assert(offsetof(struct panel, htotal) == 180, "wPanelHTotal");
_Static_assert(offsetof(struct panel, dclk) == 248, "u16PanelDCLK");
_Static_assert(offsetof(struct panel, timing) == 296, "eTiming");

/* EN_DISPLAYTIMING_RES_TYPE, the values used here */
enum {
	T_DEFAULT = 0,
	T_480I = 2, T_480P, T_576I, T_576P, T_720P_50, T_720P_60,
	T_1080P_24, T_1080P_25, T_1080P_30, T_1080I_50, T_1080P_50,
	T_1080I_60, T_1080P_60,
	T_2K2KP_24 = 20,
	T_4K2KP_24 = 28, T_4K2KP_25, T_4K2KP_30, T_4K2KP_50, T_4K2KP_60,
	T_VGA_1024x768P_60 = 40, T_VGA_1366x768P_60, T_VGA_1280x800P_60,
	T_VGA_1280x1024P_60, T_VGA_1440x900P_60, T_VGA_1600x1200P_60,
	T_VGA_1680x1050P_60,
};

/* panel power sequence (ms): on 1 is always 30, then on 2, off 1, off 2 */
#define PWR_DAC		400, 80, 30
#define PWR_4K		200, 220, 10
/* LPLL max/min set */
#define SET_DAC		0x1c848e, 0x18eb59
#define SET_480I	0x2cd3fe, 0x1ed4c4
#define SET_576I	0x2ce213, 0x1edb6d
#define SET_4K		0x1aab29, 0x1a94bf

/*
 * The output timings, in the SDK tool's order: the default panel
 * (Customer_1.ini m_pPanelName, 1080p50) first as timing 0, then the ones
 * Customer_1.ini names; 720p60 is named twice. mi_disp takes the first
 * entry of a timing. The 2205-line 3D timing sits in the 2K2K_24 slot, and
 * the 1680x1050 entry is called DACOUT_1600X1050P_60, as in the SDK files.
 * i: interlaced (LINK_DAC_I); dual: dual port and double clock.
 */
static const struct timing {
	const char *name;
	uint8_t timing, i, dual;
	uint8_t hsw;
	uint16_t hbp;
	uint8_t vsw, vbp;
	uint16_t hstart, vstart, width, height;
	uint16_t htotal_max, htotal, htotal_min;
	uint16_t vtotal_max, vtotal, vtotal_min;
	uint16_t dclk_max, dclk, dclk_min;
	uint8_t out_timing_mode;
	uint16_t on2, off1, off2;
	uint32_t max_set, min_set;
} timings[] = {
/*	  name                    timing             i dual hsw hbp vsw vbp hst vst  width height   htotal max typ min   vtotal max typ min   dclk max typ min mode */
	{ "DACOUT_1080P_50",      T_DEFAULT,         0, 1,  44, 148, 5, 36, 192, 41, 1920, 1080, 2740, 2640, 2540, 1225, 1125, 1025, 167, 148, 130, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_480I_60",       T_480I,            1, 0,  62,  60, 6, 30, 122,  0,  720,  480,  958,  858,  758,  625,  525,  425,  37,  27,  17, 2, PWR_DAC, SET_480I },
	{ "DACOUT_480P_60",       T_480P,            0, 1,  62,  60, 6, 30, 122, 36,  720,  480,  958,  858,  758,  625,  525,  425,  36,  27,  19, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_576I_50",       T_576I,            1, 0,  64,  68, 5, 39, 132,  0,  720,  576,  964,  864,  764,  725,  625,  525,  37,  27,  17, 2, PWR_DAC, SET_576I },
	{ "DACOUT_576P_50",       T_576P,            0, 1,  64,  68, 5, 39, 132, 44,  720,  576,  964,  864,  764,  725,  625,  525,  35,  27,  20, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_720P_50",       T_720P_50,         0, 1,  40, 220, 5, 20, 260, 25, 1280,  720, 2080, 1980, 1880,  850,  750,  650,  88,  74,  61, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_720P_60",       T_720P_60,         0, 1,  40, 220, 5, 20, 260, 25, 1280,  720, 1750, 1650, 1550,  850,  750,  650,  89,  74,  60, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_720P_60",       T_720P_60,         0, 1,  40, 220, 5, 20, 260, 25, 1280,  720, 1750, 1650, 1550,  850,  750,  650,  89,  74,  60, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1080P_24",      T_1080P_24,        0, 1,  44, 148, 5, 36, 192, 41, 1920, 1080, 2850, 2750, 2650, 1225, 1125, 1025,  85,  74,  65, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1080P_25",      T_1080P_25,        0, 1,  44, 148, 5, 36, 192, 41, 1920, 1080, 2740, 2640, 2540, 1225, 1125, 1025,  85,  74,  65, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1080P_30",      T_1080P_30,        0, 1,  44, 148, 5, 36, 192, 41, 1920, 1080, 2300, 2200, 2100, 1225, 1125, 1025,  85,  74,  65, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1080I_50",      T_1080I_50,        1, 1,  44, 148, 5, 15, 192,  0, 1920, 1080, 2740, 2640, 2540, 1225, 1125, 1025, 158, 148, 138, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1080P_50",      T_1080P_50,        0, 1,  44, 148, 5, 36, 192, 41, 1920, 1080, 2740, 2640, 2540, 1225, 1125, 1025, 167, 148, 130, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1080I_60",      T_1080I_60,        1, 1,  44, 148, 5, 15, 192,  0, 1920, 1080, 2300, 2200, 2100, 1225, 1125, 1025, 158, 148, 138, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1080P_60",      T_1080P_60,        0, 1,  44, 148, 5, 36, 192, 41, 1920, 1080, 2300, 2200, 2100, 1225, 1125, 1025, 169, 148, 130, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_2205P_24",      T_2K2KP_24,        0, 1,  44, 148, 5, 36, 192,  0, 1920, 2205, 2850, 2750, 2650, 2350, 2250, 2150, 159, 149, 139, 0, PWR_DAC, SET_DAC },
	{ "DACOUT_4K2KP_24",      T_4K2KP_24,        0, 1,  88, 296, 11, 72, 384, 0, 3840, 2160, 5600, 5500, 5400, 2350, 2250, 2150, 307, 297, 287, 0, PWR_4K, SET_4K },
	{ "DACOUT_4K2KP_25",      T_4K2KP_25,        0, 1,  88, 296, 11, 72, 384, 0, 3840, 2160, 5380, 5280, 5180, 2350, 2250, 2150, 307, 297, 287, 0, PWR_4K, SET_4K },
	{ "DACOUT_4K2KP_30",      T_4K2KP_30,        0, 1,  88, 296, 10, 72, 384, 0, 3840, 2160, 4500, 4400, 4300, 2350, 2250, 2150, 307, 297, 287, 0, PWR_4K, SET_4K },
	{ "DACOUT_4K2KP_50",      T_4K2KP_50,        0, 1,  88, 296, 11, 72, 384, 0, 3840, 2160, 5380, 5280, 5180, 2350, 2250, 2150, 607, 594, 584, 0, PWR_4K, SET_4K },
	{ "DACOUT_4K2KP_60",      T_4K2KP_60,        0, 1,  88, 296, 11, 72, 384, 0, 3840, 2160, 4500, 4400, 4300, 2350, 2250, 2150, 604, 594, 584, 0, PWR_4K, SET_4K },
	{ "DACOUT_1024X768P_60",  T_VGA_1024x768P_60,  0, 1, 136, 160, 6, 29, 296, 35, 1024,  768, 1444, 1344, 1244,  906,  806,  706,  78,  65,  52, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1366X768P_60",  T_VGA_1366x768P_60,  0, 1, 143, 215, 3, 24, 358, 27, 1366,  768, 1892, 1792, 1692,  898,  798,  698, 101,  86,  70, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1280X800P_60",  T_VGA_1280x800P_60,  0, 1, 128, 200, 6, 22, 328, 28, 1280,  800, 1780, 1680, 1580,  931,  831,  731,  94,  84,  74, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1280X1024P_60", T_VGA_1280x1024P_60, 0, 1, 112, 248, 3, 38, 360, 41, 1280, 1024, 1788, 1688, 1588, 1166, 1066,  906, 125, 108,  86, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1440X900P_60",  T_VGA_1440x900P_60,  0, 1, 152, 232, 6, 25, 384, 31, 1440,  900, 2004, 1904, 1804, 1034,  934,  834, 124, 106,  90, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1600X1050P_60", T_VGA_1680x1050P_60, 0, 1, 176, 280, 6, 30, 456, 36, 1680, 1050, 2340, 2240, 2140, 1139, 1089, 1039, 159, 146, 133, 2, PWR_DAC, SET_DAC },
	{ "DACOUT_1600X1200P_60", T_VGA_1600x1200P_60, 0, 1, 192, 304, 3, 46, 496, 49, 1600, 1200, 2260, 2160, 2060, 1350, 1250, 1150, 183, 162, 142, 2, PWR_DAC, SET_DAC },
};
#define N_TIMINGS (sizeof(timings) / sizeof(timings[0]))
#define DEFAULT_PANEL "DACOUT_1080P_50"

static void fill_panel(struct panel *p, const struct timing *t)
{
	memset(p, 0, sizeof(*p));
	snprintf(p->name, sizeof(p->name), "%s", t->name);
	p->link_type = t->i ? 7 : 8;
	p->dual_port = p->double_clk = t->dual;
	/* board.ini: m_bPANEL_PDP_10BIT = 1 */
	p->pdp_10bit = 1;
	p->lvds_ti_mode = 1;
	p->dclk_current = p->de_current = 1;
	p->odd_data_current = p->even_data_current = 1;
	p->on_timing1 = 30;
	p->on_timing2 = t->on2;
	p->off_timing1 = t->off1;
	p->off_timing2 = t->off2;
	p->hsync_width = t->hsw;
	p->hsync_back_porch = t->hbp;
	p->vsync_width = t->vsw;
	p->vsync_back_porch = t->vbp;
	p->hstart = t->hstart;
	p->vstart = t->vstart;
	p->width = t->width;
	p->height = t->height;
	p->htotal_max = t->htotal_max;
	p->htotal = t->htotal;
	p->htotal_min = t->htotal_min;
	p->vtotal_max = t->vtotal_max;
	p->vtotal = t->vtotal;
	p->vtotal_min = t->vtotal_min;
	/* the clock twice: MI_U8 (cut to 8 bits: 297 MHz reads 41) and MI_U16 */
	p->dclk_max_u8 = (uint8_t)t->dclk_max;
	p->dclk_u8 = (uint8_t)t->dclk;
	p->dclk_min_u8 = (uint8_t)t->dclk_min;
	p->dclk_max = t->dclk_max;
	p->dclk = t->dclk;
	p->dclk_min = t->dclk_min;
	p->dimming_ctl = 160;
	p->max_pwm = 255;
	p->min_pwm = 80;
	p->aspect_ratio = 1;
	p->ti_bit_mode = 2;
	p->output_bit_mode = 0;
	p->max_set = t->max_set;
	p->min_set = t->min_set;
	p->out_timing_mode = t->out_timing_mode;
	/* BT.709 primaries and white point (x50000), 100/50/500 nits, full range */
	p->cfd.rx = 32000; p->cfd.ry = 16500;
	p->cfd.gx = 15000; p->cfd.gy = 30000;
	p->cfd.bx = 7500;  p->cfd.by = 3000;
	p->cfd.wx = 15635; p->cfd.wy = 16450;
	p->cfd.max_lum = 100; p->cfd.med_lum = 50; p->cfd.min_lum = 500;
	p->cfd.color_format = 3;
	p->cfd.full_range = 1;
	p->link_ext_type = 10;
	p->timing = t->timing;
	p->hdmi_tx = 0;
}

/*
 * DLC (dynamic luma curve), MAPI_XC_DLC_init: DLC.ini's two sections, the
 * same. The SDK tool reads the scalar parameters only; the curves (luma,
 * histogram limit), written "=\{ ... }" in DLC.ini, stay 0, and so does the
 * older MAPI_XC_DLC_MFinit at the start of each entry.
 */
#define DLC_SIZE	272
#define DLC_TABLES	2
#define DLC_EX		92	/* MAPI_XC_DLC_MFinit_Ex */
static const struct { uint16_t off; uint8_t size; uint16_t val; } dlc_params[] = {
	/* offsets into MAPI_XC_DLC_MFinit_Ex */
	{ 68, 1, 8 },		/* u8_L_L_U */
	{ 69, 1, 8 },		/* u8_L_L_D */
	{ 70, 1, 2 },		/* u8_L_H_U */
	{ 71, 1, 3 },		/* u8_L_H_D */
	{ 72, 1, 104 },		/* u8_S_L_U */
	{ 73, 1, 23 },		/* u8_S_L_D */
	{ 74, 1, 0 },		/* u8_S_H_U */
	{ 75, 1, 38 },		/* u8_S_H_D */
	{ 79, 1, 1 },		/* ucCGCYCslope */
	{ 80, 1, 8 },		/* ucCGCYth */
	{ 81, 1, 1 },		/* ucDlcPureImageMode */
	{ 82, 1, 2 },		/* ucDlcLevelLimit */
	{ 84, 1, 3 },		/* ucDlcAvgDeltaStill */
	{ 85, 1, 31 },		/* ucDlcFastAlphaBlending */
	{ 89, 1, 200 },		/* ucDlcYAvgThresholdH */
	{ 90, 1, 32 },		/* ucDlcYAvgThresholdL */
	{ 91, 1, 48 },		/* ucDlcBLEPoint */
	{ 92, 1, 48 },		/* ucDlcWLEPoint */
	{ 94, 1, 70 },		/* ucDlcYAvgThresholdM */
	{ 95, 1, 2 },		/* ucDlcCurveMode */
	{ 96, 1, 72 },		/* ucDlcCurveModeMixAlpha */
	{ 97, 1, 2 },		/* ucDlcAlgorithmMode */
	{ 115, 1, 188 },	/* ucDlcSepPointH */
	{ 116, 1, 80 },		/* ucDlcSepPointL */
	{ 118, 2, 640 },	/* uwDlcBleStartPointTH */
	{ 120, 2, 320 },	/* uwDlcBleEndPointTH */
	{ 122, 1, 56 },		/* ucDlcCurveDiff_L_TH */
	{ 123, 1, 148 },	/* ucDlcCurveDiff_H_TH */
	{ 124, 2, 1032 },	/* uwDlcBLESlopPoint_1 */
	{ 126, 2, 1172 },	/* uwDlcBLESlopPoint_2 */
	{ 128, 2, 1242 },	/* uwDlcBLESlopPoint_3 */
	{ 130, 2, 1324 },	/* uwDlcBLESlopPoint_4 */
	{ 132, 2, 1396 },	/* uwDlcBLESlopPoint_5 */
	{ 134, 2, 1200 },	/* uwDlcDark_BLE_Slop_Min */
	{ 136, 1, 2 },		/* ucDlcCurveDiffCoringTH */
	{ 137, 1, 1 },		/* ucDlcAlphaBlendingMin */
	{ 138, 1, 128 },	/* ucDlcAlphaBlendingMax */
	{ 139, 1, 96 },		/* ucDlcFlicker_alpha */
	{ 140, 1, 56 },		/* ucDlcYAVG_L_TH */
	{ 141, 1, 136 },	/* ucDlcYAVG_H_TH */
	{ 142, 1, 4 },		/* ucDlcDiffBase_L */
	{ 143, 1, 14 },		/* ucDlcDiffBase_M */
	{ 144, 1, 20 },		/* ucDlcDiffBase_H */
};

static void put16(uint8_t *b, size_t off, uint16_t v)
{
	b[off] = v;
	b[off + 1] = v >> 8;
}

static void put32(uint8_t *b, size_t off, uint32_t v)
{
	put16(b, off, v);
	put16(b, off + 2, v >> 16);
}

static uint8_t cfg[CFG_SIZE];

static void build_config(void)
{
	size_t i, j;

	/* HDMITx_Analog_Param_t, the SDK board.h's astHdmiAnalogCfg[2]:
	 * { tm_txcurrent 1 }, { tm_ten 7 } */
	cfg[CFG_HDMITX_ANALOG + 0] = 1;
	cfg[CFG_HDMITX_ANALOG + 6 + 5] = 7;

	cfg[CFG_DLC_TABLE_COUNT] = DLC_TABLES;
	memcpy(cfg + CFG_PANEL_NAME, DEFAULT_PANEL, sizeof(DEFAULT_PANEL));

	put32(cfg, CFG_PANEL_COUNT, N_TIMINGS);
	for (i = 0; i < N_TIMINGS; i++) {
		struct panel p;

		fill_panel(&p, &timings[i]);
		memcpy(cfg + CFG_PANELS + i * sizeof(p), &p, sizeof(p));
	}

	/* PanelBacklightPWMInfo: u8PWMPort 2 (board.ini), bPolPWM 1 */
	cfg[CFG_BACKLIGHT_PWM + 18] = 2;
	cfg[CFG_BACKLIGHT_PWM + 19] = 1;

	put32(cfg, CFG_SAW_TYPE, 3);		/* NO_SAW */
	cfg[CFG_MIRROR_MODE] = 3;		/* MIRROR_VIDEO_TYPE default */
	put32(cfg, CFG_FBL_THRESHOLD, 148500000);

	for (i = 0; i < DLC_TABLES; i++) {
		uint8_t *d = cfg + CFG_DLC + i * DLC_SIZE + DLC_EX;

		for (j = 0; j < sizeof(dlc_params) / sizeof(dlc_params[0]); j++) {
			if (dlc_params[j].size == 2)
				put16(d, dlc_params[j].off, dlc_params[j].val);
			else
				d[dlc_params[j].off] = dlc_params[j].val;
		}
	}
}

/* ------------------------------------------------------------------------
 * PQ: PQConfig.ini's DBC values and dynamic noise reduction tables.
 */
static const uint8_t dbc[17] = {
	55, 30, 10,		/* MAX, MID, MIN_VIDEO_DBC */
	254, 66, 0,		/* MAX_PWM, BACKLIGHT_THRES, MIN_PWM */
	0x48, 0x50, 0x48, 0x50,	/* Y_GAIN_M, Y_GAIN_L, C_GAIN_M, C_GAIN_L */
	60, 8, 32,		/* ALPHA_BLENDING_CURRENT, AVG_DELTA, FAST_ALPHABLENDING */
	4, 6, 2, 2,		/* LOOP_DLY_H, _MH, _ML, _L */
};

static const uint8_t motion[6][8] = {
	{ 0xE8, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x00 },
	{ 0xD8, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12, 0x00 },
	{ 0xC8, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01, 0x00 },
	{ 0xB8, 0x9A, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00 },
	{ 0xA8, 0x89, 0x67, 0x45, 0x23, 0x01, 0x00, 0x00 },
	{ 0x98, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00 },
};

static const uint8_t motion_hdmi_dtv[4][8] = {
	{ 0x68, 0x45, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00 },
	{ 0xA8, 0x89, 0x67, 0x45, 0x23, 0x01, 0x00, 0x00 },
	{ 0xC8, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01, 0x00 },
	{ 0xE8, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x00 },
};

static const uint8_t motion_comp_pc[4][8] = {
	{ 0x68, 0x45, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00 },
	{ 0x98, 0x78, 0x56, 0x34, 0x12, 0x01, 0x00, 0x00 },
	{ 0xA8, 0x89, 0x67, 0x45, 0x23, 0x12, 0x01, 0x00 },
	{ 0xB8, 0x9A, 0x78, 0x56, 0x34, 0x12, 0x01, 0x00 },
};

static const uint8_t misc_param[4][4] = {
	{ 0x02, 0xFF, 0x00, 0x00 },
	{ 0x02, 0xFF, 0x01, 0x66 },
	{ 0x03, 0x88, 0x01, 0x66 },
	{ 0x03, 0xAA, 0x01, 0x66 },
};

static const uint8_t misc_luma[8][2] = {
	{ 0x33, 0x08 }, { 0x33, 0x07 }, { 0x22, 0x06 }, { 0x22, 0x05 },
	{ 0x11, 0x04 }, { 0x11, 0x03 }, { 0x00, 0x02 }, { 0x00, 0x01 },
};

/* PQConfig.ini declares six rows here and fills five; the sixth is 0 */
static const uint8_t noise[6][8] = {
	{ 0x67, 0x45, 0x34, 0x23, 0x12, 0x01, 0x00, 0x00 },
	{ 0xAB, 0x89, 0x67, 0x45, 0x23, 0x12, 0x01, 0x00 },
	{ 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x23, 0x01, 0x00 },
	{ 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12, 0x00 },
	{ 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x00 },
};

/* the same: six rows declared, five filled */
static const uint8_t misc_noise[6][15] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00, 0xC0, 0x9F, 0x00, 0x01, 0x66 },
	{ 0x10, 0x01, 0xC1, 0xC1, 0xC1, 0x04, 0x09, 0x1F, 0x03, 0x88, 0xC0, 0x9F, 0x00, 0x01, 0x66 },
	{ 0x21, 0x02, 0xC2, 0xC2, 0xC2, 0x08, 0x09, 0x1F, 0x03, 0x99, 0x00, 0x00, 0x00, 0x01, 0x88 },
	{ 0x31, 0x04, 0xC2, 0xC2, 0xC2, 0x0A, 0x09, 0x1F, 0x03, 0xAA, 0x00, 0x00, 0x00, 0x01, 0xAA },
	{ 0x41, 0x06, 0xC3, 0xC3, 0xC3, 0x10, 0x19, 0x1F, 0x03, 0xBB, 0x00, 0x00, 0x14, 0x01, 0xBB },
};

/* ------------------------------------------------------------------------ */

/*
 * One write to /proc/mi_modules/common/<file>: at file offset <off> for
 * mmap_info, which takes the offset as the entry's index; with write()
 * otherwise, as memory_info cannot seek (pwrite() fails with ESPIPE).
 * Only an error counts: memory_info answers with the number of values it
 * took (7), not of bytes.
 */
static int put(const char *file, const void *buf, size_t len, off_t off)
{
	char path[64];
	ssize_t n;
	int fd;

	snprintf(path, sizeof(path), PROC "%s", file);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "config_tool: %s: %s\n", path, strerror(errno));
		return -1;
	}
	n = off < 0 ? write(fd, buf, len) : pwrite(fd, buf, len, off);
	close(fd);
	if (n < 0) {
		fprintf(stderr, "config_tool: %s: %s\n", path, strerror(errno));
		return -1;
	}
	return 0;
}

static int load_mmap(void)
{
	char line[128];
	size_t i;
	int n, ret = 0;

	/* first: mi_sys refuses mmap_info entries at or past its item count */
	n = snprintf(line, sizeof(line), "%u %u %u %u %u %u %u\n",
		MEM_TOTAL, MEM_MIU0, MEM_MIU1, MIU_BOUNDARY, (unsigned)N_MMAP,
		MEM_4K_ALIGN, MEM_MIU1 != 0);
	if (put("memory_info", line, n, -1))
		return 1;

	for (i = 0; i < N_MMAP; i++) {
		const struct mmap_entry *e = &mmap_table[i];

		/* gid is the entry's index */
		n = snprintf(line, sizeof(line), "%s %u %u %u %u %u %u %u %u\n",
			e->name, (unsigned)i, e->addr, e->size, e->layer,
			e->align, e->type, e->miu, e->cma_hid);
		ret |= put("mmap_info", line, n, i);
	}
	return ret ? 1 : 0;
}

static int load_config(void)
{
	uint8_t pq[32] = { 0 };
	int ret = 0;

	build_config();
	if (put("config_info", cfg, sizeof(cfg), -1))
		ret = 1;

	/* MI_SYSCFG_PQConfig_t up to its table pointers: the DBC values,
	 * then rows and columns of each table, in this order */
	memcpy(pq, dbc, sizeof(dbc));
	pq[17] = sizeof(motion) / sizeof(motion[0]);
	pq[18] = sizeof(motion[0]);
	pq[19] = sizeof(motion_hdmi_dtv) / sizeof(motion_hdmi_dtv[0]);
	pq[20] = sizeof(motion_hdmi_dtv[0]);
	pq[21] = sizeof(motion_comp_pc) / sizeof(motion_comp_pc[0]);
	pq[22] = sizeof(motion_comp_pc[0]);
	pq[23] = sizeof(misc_param) / sizeof(misc_param[0]);
	pq[24] = sizeof(misc_param[0]);
	pq[25] = sizeof(misc_luma) / sizeof(misc_luma[0]);
	pq[26] = sizeof(misc_luma[0]);
	pq[27] = sizeof(noise) / sizeof(noise[0]);
	pq[28] = sizeof(noise[0]);
	pq[29] = sizeof(misc_noise) / sizeof(misc_noise[0]);
	pq[30] = sizeof(misc_noise[0]);
	/* mi_sys checks each table's size against these, so pq_info first */
	if (put("pq_info", pq, sizeof(pq), -1))
		return 1;

	ret |= put("noise_table", noise, sizeof(noise), -1);
	ret |= put("motion_table", motion, sizeof(motion), -1);
	ret |= put("motion_hdmi_dtv_table", motion_hdmi_dtv, sizeof(motion_hdmi_dtv), -1);
	ret |= put("motion_comp_pc_table", motion_comp_pc, sizeof(motion_comp_pc), -1);
	ret |= put("misc_table", misc_noise, sizeof(misc_noise), -1);
	ret |= put("misc_luma_table", misc_luma, sizeof(misc_luma), -1);
	ret |= put("misc_param_table", misc_param, sizeof(misc_param), -1);
	return ret ? 1 : 0;
}

int main(int argc, char **argv)
{
	const char *cmd = strrchr(argv[0], '/');

	cmd = cmd ? cmd + 1 : argv[0];
	if (strcmp(cmd, "load_mmap") && strcmp(cmd, "load_config") && argc > 1)
		cmd = argv[1];

	if (!strcmp(cmd, "load_mmap"))
		return load_mmap();
	if (!strcmp(cmd, "load_config"))
		return load_config();

	fprintf(stderr, "usage: config_tool load_mmap | load_config\n");
	return 2;
}
