#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"

#include "xm_print.h"
#include "XAx_cmos.h"

#define STATIC	static
#define HD1080_25_LINES		(1125)


extern XM_U32 gau32AllGain;
extern XM_SENSOR_CHIP gSensorChip;
extern XM_U32 gu32AGainNow;
extern XM_U32 gu32AGainMin;
extern XM_U32 gu32DGainNow;
extern XM_U32 gu32DGainMin;
extern XM_U8 gu8BlcDpcFlg;
extern XM_U32 gu32ShutNow;
extern const ISP_CMOS_GAMMA_S gstIspGamma; 

static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	//[0~7]:Normal						
	/* sharpen_D	H	*/
	{0x18,0x18,0x18,0x18,0x18,0x14,0x10,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	
	/* sharpen_Ud M */
	{0x1C,0x1C,0x1C,0x1C,0x1C,0x18,0x10,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},

	/* sharpen_Kd  */
	{0x2C,0x2C,0x2C,0x2C,0x28,0x18,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	
	/* snr_thresh 2DNr		*/
	{0x00,0x01,0x04,0x10,0x20,0x28,0x30,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},

	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x06,0x0A,0x0E,0x12,0x15,0x17,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18},

	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x00,0x00,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},  

	/* DyDpc_thresh 		*/
	{0x00,0x00,0x00,0x00,0x00,0xC0,0xD0,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8},
	
	/* saturation_strength */
	{0x99,0x99,0x96,0x90,0x88,0x78,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
	
	/* Blc	*/
	{0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109,0x109},

	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x31,0x2F,0x2D,0x2A,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28},
};

static XM_S32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
	if (XM_NULL == pstDef)
	{
		ERR("null pointer when get isp default value!\n");
		return -1;
	}
	pstDef->stGamma.bValid = XM_TRUE;
	XM_MPI_ISP_Memncpy((XM_U8*)&pstDef->stGamma, (XM_U8*)&gstIspGamma, sizeof(ISP_CMOS_GAMMA_S));
	XM_MPI_ISP_Memncpy((XM_U8 *)&pstDef->stAgcTbl, (XM_U8 *)&g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
	return 0;
}

static XM_VOID cmos_set_pixel_detect(XM_BOOL bEnable)
{
    return;
}

static XM_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	if (XM_NULL == pstAeSnsDft)
	{
		ERR("null pointer when get ae default value!\n");
		return -1;
	}
	pstAeSnsDft->u32FullLinesStd = HD1080_25_LINES;
	pstAeSnsDft->u8AeCompensation = 0x39;	
	pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	pstAeSnsDft->u32FlickerFreq = 0;

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
	pstAeSnsDft->u32MinIntTime = 2;    

	pstAeSnsDft->u32MaxAgain = 16128;
	pstAeSnsDft->u32MinAgain = 1024;

	pstAeSnsDft->u32MaxDgain = 4032;  
	pstAeSnsDft->u32MinDgain = 128;

	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
	pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;

	pstAeSnsDft->bDelayCfg = XM_TRUE;
	return 0;
}

static XM_S32 cmos_get_sensor_max_resolution(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution)
{
	if (XM_NULL == pstSensorMaxResolution)
	{
	    ERR("null pointer when get sensor max resolution \n");
	    return -1;
	}

	pstSensorMaxResolution->u32MaxWidth  = 1920;
	pstSensorMaxResolution->u32MaxHeight = 1080;
	return 0;
}

STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U32 u32Val;
	XM_U16 u16OfstV;

	u32Val = sensor_read_register(0x3221);
	
	if(u8Mirror)
	{
		u32Val |= 0x06;
	}
	else
	{
		u32Val &= ~0x06;
	}
		
	if(u8Flip)
	{
		u32Val |= 0x60;
		u16OfstV = 0;
	}
	else
	{
		u32Val &= ~0x60;
		u16OfstV = 0;
	}
	sensor_write_register(0x3221, u32Val);
	VI_WinSet(1, u8Mirror, u8Flip, 0, u16OfstV);
	XM_MPI_MIPI_RefreshFV(100, 0);
	return XM_SUCCESS;
}

static XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 64;
	u32InTimes= u32InTimes/32;
	u32InTimes = u32InTimes < 32? 32: u32InTimes;
	for(i = 0; i < 5; i++)
	{
		if(u32InTimes < 64)
		{
			break;
		}
		u32InTimes >>= 1;
		pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<1)|1;
	}
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8)|u32InTimes; 
	pstAeSnsGainInfo->u32SnsTimes = (u32InTimes<<i)*32;
	gu32AGainNow = pstAeSnsGainInfo->u32SnsTimes;
	return;
}

STATIC XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;
	u32InTimes = (u32InTimes>>2) << 2;
	u32InTimes = u32InTimes < 128? 128: u32InTimes;
	for(i = 0; i < 5; i++)
	{
		if(u32InTimes < 256)
		{
			break;
		}
		u32InTimes >>= 1;
		pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<1)|1;
	}
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8)|u32InTimes; 
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes<<i;
	gu32DGainNow = pstAeSnsGainInfo->u32SnsTimes;
	return;
}

STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;
	gu32ShutNow = u32IntTime;
	
	u32IntTime = u32IntTime*2;
	sensor_write_register(0x3e00, ((u32IntTime&0xF000)>>12));
	sensor_write_register(0x3e01, ((u32IntTime&0x0FF0)>>4));
	sensor_write_register(0x3e02, ((u32IntTime&0x000F)<<4));
	return;
}

XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	XM_U32 u32AllGain;
	unsigned int tmp[2];
    
	u32AllGain = ((XM_U64)gu32AGainNow*128>>10)*gu32DGainNow >> 10;	// * 16
	gau32AllGain = u32AllGain;
	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;
		
		// 1.AGainUpdate
		tmp[1] = (u32Again>>8)&0x0F;
		tmp[1] = (tmp[1]<<2)|0x03;
		tmp[0] = u32Again&0xFF;
		sensor_write_register(0x3e08, tmp[1]);
		sensor_write_register(0x3e09, tmp[0]);

		// 2.DGainUpdate
		tmp[1] = (u32Dgain>>8)&0x0F;
		tmp[0] = u32Dgain&0xFF;
		sensor_write_register(0x3e06, tmp[1]);
		sensor_write_register(0x3e07, tmp[0]);
	}
	return;
}

static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32Tmp,u32ExpNow;
	XM_U32 u32TotalSizeV,u32Pixs=2640;
	u32ExpNow = sensor_read_register(0x3e00);
	u32ExpNow = (u32ExpNow<<12) | (sensor_read_register(0x3e01) << 4);
	u32Tmp = sensor_read_register(0x3e02);
	u32ExpNow |= ((u32Tmp>>4)&0x0F);
	u32ExpNow = u32ExpNow>>1;
	u32TotalSizeV = HD1080P_LINES;
	switch(u8Fps)
	{
		case 30:
			u32Pixs = 2200;
			break;
		case 25:
			u32Pixs = 2640;
			break;
		default:
			//ERR("cmos_fps_set: fps err!\n");
			return;
			break;
	}
	if(pstAeSnsDft != NULL) 
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-gu8MaxShutterOfst;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	if(u32ExpNow>u32TotalSizeV-gu8MaxShutterOfst)
	{
		cmos_inttime_update(u32TotalSizeV-gu8MaxShutterOfst);
	}
	sensor_write_register(0x320c, (u32Pixs>>8)&0xFF);
	sensor_write_register(0x320d, u32Pixs&0xFF);
	sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);
	gu8Fps = u8Fps;
	return;
}



static XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	static XM_U16 preU16FullLine = 0xffff;
	if(preU16FullLine == u16FullLines)
		return;

	preU16FullLine = u16FullLines;
	u16FullLines = (u16FullLines>=4500)?4500:u16FullLines;
	pstAeSnsDft->u32MaxIntTime = u16FullLines - 4;
	SysDelay_ms(100);
	if(gu32ShutNow>pstAeSnsDft->u32MaxIntTime)
	{
		cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
	}
	sensor_write_register(0x320e, (u16FullLines&0xff00)>>8);	
	sensor_write_register(0x320f, u16FullLines&0xff);

	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}

static const ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,//0.16
	{
		0x000,356, -148, 48,
		0x000,-75, 373,  -42,
		0x000,8, -170, 418
	},
	4000,//0.16
	{
		0x000,358, -167,  65,
		0x000,-104, 376, -16,
		0x000, -17,-222, 495
	},
	2800,//0.16
	{
		0x000, 290, -70, 36,
		0x000,-174, 440, -10,
		0x000,-170,-422, 848
	}
};
	
static const ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
    {0, 0, 4096, 3320, 4090, 2425, 1032, 4096},
	{4096, 4096, 0, 0, 3914, 3059, 4096, 1025},
	{-1252, -3552, -1899, -4096, -4096, -4096, -2289, -2676},
	213,
	0, 1570, 0, 2655,
	{0, 869, 1077, 1329, 1391, 1475, 1658, 1683, 2655, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1577, 1024, 1840, 0}
};

static XM_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	if (XM_NULL == pstAwbSnsDft)
	{
		ERR("null pointer when get awb default value!\n");
		return XM_FAILURE;
	}
	pstAwbSnsDft->pstRbTable = (const  AWB_COEF_TABLE_S*)(&gstAwbCal);
	// CCM
	XM_MPI_ISP_Memncpy((XM_U8*)&(pstAwbSnsDft->stCcm), (XM_U8*)&g_stAwbCcm, sizeof(ISP_COLORMATRIX_AUTO_S));
	return XM_SUCCESS;
}


XM_S32 cmos_init_sensor_exp_function_sc2235E(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_sensor_init = NULL;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_sc2235E(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_get = NULL;
	pstExpFuncs->pfn_cmos_get_ae_default	= cmos_get_ae_default;
	pstExpFuncs->pfn_cmos_fps_set			= cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;    
	pstExpFuncs->pfn_cmos_inttime_update	= cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update		= cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table	= cmos_again_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table	= cmos_dgain_calc_table;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}

XM_S32 cmos_init_awb_exp_function_sc2235E(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}

// ExtClk: 27M
static const XM_U16 gau16SnsInit_sc2235E[][2] = {
	{0x0103,0x01},
	{0x0100,0x00},
	{0x3001,0x00},
	{0x3002,0x80},
	{0x3018,0x33},
	{0x301a,0xf0},
	{0x301c,0x78},
	{0x3037,0x20},
	{0x3038,0x6e},
	{0x3039,0x52},
	{0x303b,0x14},
	{0x303d,0x10},
	{0x303f,0x01},
	{0x3208,0x07},
	{0x3209,0x88},
	{0x320a,0x04},
	{0x320b,0x40},
	{0x3221,0x80},
	{0x3301,0x08},
	{0x3303,0x38},
	{0x3306,0x50},
	{0x3309,0x70},
	{0x330a,0x00},
	{0x3000,0x00},
	{0x330b,0xd0},
	{0x330d,0x36},
	{0x330e,0x18},
	{0x330f,0x01},
	{0x3310,0x23},
	{0x3314,0x14},
	{0x331e,0x31},
	{0x331f,0x69},
	{0x3338,0x37},
	{0x3339,0x37},
	{0x333a,0x33},
	{0x335d,0x20},
	{0x3364,0x1d},
	{0x3367,0x08},
	{0x33ae,0x32},
	{0x33b3,0x32},
	{0x33b4,0x32},
	{0x33b6,0x0f},
	{0x33b7,0x3e},
	{0x33b8,0x08},
	{0x33b9,0x80},
	{0x33ba,0xc0},
	{0x360f,0x05},
	{0x3614,0x80},
	{0x3622,0xf6},
	{0x3630,0x40},
	{0x3631,0x88},
	{0x3632,0x88},
	{0x3633,0x24},
	{0x3635,0x1c},
	{0x3637,0x2c},
	{0x3638,0x24},
	{0x363a,0x80},
	{0x363b,0x16},
	{0x3641,0x00},
	{0x366e,0x04},
	{0x3670,0x48},
	{0x3671,0xf6},
	{0x3672,0x16},
	{0x3673,0x16},
	{0x367a,0x38},
	{0x367b,0x38},
	{0x3690,0x24},
	{0x3691,0x44},
	{0x3692,0x44},
	{0x3699,0x80},
	{0x369a,0x80},
	{0x369b,0x9f},
	{0x369c,0x38},
	{0x369d,0x38},
	{0x36a2,0x00},
	{0x36a3,0x3f},
	{0x391e,0x00},
	{0x3933,0x0a},
	{0x3934,0x10},
	{0x3940,0x60},
	{0x3942,0x02},
	{0x3943,0x18},
	{0x3e00,0x00},
	{0x3e01,0x8c},
	{0x3e02,0x20},
	{0x3e03,0x0b},
	{0x3e06,0x00},
	{0x3e07,0x80},
	{0x3e08,0x03},
	{0x3e09,0x20},
	{0x3f00,0x0f},
	{0x3f04,0x04},
	{0x3f05,0x28},
	{0x4603,0x00},
	{0x5780,0x7f},
	{0x5781,0x04},
	{0x5782,0x04},
	{0x5783,0x02},
	{0x5784,0x01},
	{0x57a0,0x00},
	{0x57a1,0x71},
	{0x57a2,0x01},
	{0x57a3,0xf1},
	{0x57a4,0x60},
	{0x0100,0x01}
};

XM_U32 sensor_getlist_sc2235E(XM_U16 *pu16Num)
{
	DEBUG("------------- SC2235E (@20190110_mipi) ----------------\n");
	*pu16Num = sizeof(gau16SnsInit_sc2235E)/sizeof(gau16SnsInit_sc2235E[0]);
	return (XM_U32)gau16SnsInit_sc2235E;
}


