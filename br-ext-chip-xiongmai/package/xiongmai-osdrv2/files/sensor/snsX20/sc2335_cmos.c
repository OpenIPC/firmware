#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"
#include "XAx_cmos.h"
#include "xm_print.h"


#ifndef SOC_NONE
#include "xm510_tmp.h"
#endif
#include "mpi_phyvi.h"

#if USE_VBIN
#define HD1080_25_LINES		(560)
#else
#define HD1080_25_LINES		(1125)
#endif
extern XM_U8 gu8Fps;
extern ISP_CMOS_SNS_ATTR_S gstSnsAttr;
extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID XM_MPI_ISP_Memset(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);
extern XM_U32 gau32AllGain;
extern XM_U32 gu32DGainNow;
extern XM_U32 gu32AGainNow;

static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable = 
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800 */
	/* sharpen_D	H	*/
	{0x18,0x18,0x18,0x18,0x18,0x14,0x06,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* sharpen_Ud M	*/
	{0x1C,0x1C,0x1C,0x1C,0x1C,0x18,0x10,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* sharpen_Kd  */
	{0x2C,0x2C,0x2C,0x2C,0x28,0x18,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* snr_thresh 2DNr		*/
	{0x01,0x02,0x03,0x06,0x09,0x0D,0x0F,0x1C,0x30,0x40,0x40,0x40,0x40,0x40,0x40,0x40},
	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x05,0x08,0x0A,0x11,0x14,0x18,0x20,0x20,0x19,0x18,0x18,0x18,0x18,0x18,0x18}, 
	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x01,0x04,0x0C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},  
	/* DyDpc_thresh 		*/
	{0x00,0x00,0x00,0x00,0x00,0xC0,0xD0,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8},   
	/* saturation_strength */
	{0xE8,0xE6,0xE4,0xD8,0xC0,0x90,0x60,0x5C,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
	/* Blc	*/
	{0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104,0x104},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x38,0x38,0x38,0x38,0x28,0x28,0x28,0x28,0x28,0x28}
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

static XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	return XM_SUCCESS;
}





static XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	XM_U8 u8i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;

	for(u8i = 0; u8i < 10; u8i++)
	{
		if(u32InTimes < 2048)	// 2x
		{
			break;
		}
		u32InTimes >>= 1;
		pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<1)|1;
	}
	
	u32InTimes = (u32InTimes>>4);
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8) | u32InTimes;
	pstAeSnsGainInfo->u32SnsTimes = (u32InTimes<<4)<<u8i;
	gu32AGainNow = pstAeSnsGainInfo->u32SnsTimes;
	return;
}





static XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	XM_U8 u8i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;

	for(u8i = 0; u8i < 10; u8i++)
	{
		if(u32InTimes < 2048)	// 2x
		{
			break;
		}
		u32InTimes >>= 1;
		pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<1)|1;
	}
	
	u32InTimes = (u32InTimes>>3);
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8) | u32InTimes;
	pstAeSnsGainInfo->u32SnsTimes = (u32InTimes<<3)<<u8i;
	gu32DGainNow = pstAeSnsGainInfo->u32SnsTimes;
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

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-5;
	pstAeSnsDft->u32MinIntTime = 1;    

	pstAeSnsDft->u32MaxAgain = 16256;
	pstAeSnsDft->u32MinAgain = 1024;

	pstAeSnsDft->u32MaxDgain = 32512;
	pstAeSnsDft->u32MinDgain = 1024;

	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
	pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;

	pstAeSnsDft->bDelayCfg = XM_TRUE;

	gu32DGainNow = pstAeSnsDft->u32MinDgain;
	gu16FullLines = HD1080_25_LINES;
	gu8Fps = 25;
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


#if USE_VBIN
const static XM_U8 gau8Logic_sc2335[4][1] = {
	//0x363c
	{0x0e},
	{0x07},
	{0x07},
	{0x07},
};
#else
const static XM_U8 gau8Logic_sc2335[3][1] = {
	//0x363c
	{0x0e},
	{0x07},
	{0x0f},
};
#endif
const static XM_U16 gau16LogicAddr_Mipi[1] = {
	0x363c
};

const static XM_U8 TempLogic_sc2335[5][3] = {

	{0x00,0x10,0x10},	//0x5787
	{0x00,0x06,0x06},	//0x5788
	{0x00,0x10,0x10},	//0x5790
	{0x00,0x10,0x10},	//0x5791
	{0x07,0x07,0x00}	//0x5799
};

const static XM_U16 TempLogicAddr_Mipi[5] = {
	0x5787,
	0x5788,
	0x5790,
	0x5791,
	0x5799
};

static void gainLogic_sc2335(XM_U32 u32AllGain)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Temp   [bit4~bit7]:Agc
	XM_U32 u32Temp = 0xffff;
	XM_U8 u8Idx2,u8i,u8flag;
	u32AllGain = u32AllGain/64;	// *1024->*16
	u32Temp = sensor_read_register(0x3974)&0xff;
	u32Temp = (u32Temp<<4)|(sensor_read_register(0x3975)&0xff);
	//gain
	if(0x40 == sensor_read_register(0x3034))
	{
		if(u32AllGain < 2)
			u8Idx2 = 0;
		else
			u8Idx2 = 1;
	}
	else if(0x41 == sensor_read_register(0x3034))
	{
		if(u32AllGain < 2)
			u8Idx2 = 2;
		else
			u8Idx2 = 1;
	}
	else
	{
		u8Idx2 = 1;
	}
	//Temp
	if ((u32Temp > 0x1040) || (u32AllGain>=32))
	{
		if (u32Temp>0x1040)
		{
			u8flag = 0;
		}
		else
		{
			u8flag = 1;
		}
	}
	else if ((u32Temp < 0x1030)&& (u32AllGain<=24)) 
	{
		u8flag = 2;
	}
	else
	{
		u8flag = su8Idx&0x0F;
	}

	if((((su8Idx>>4)&0x0F) != u8Idx2)||((su8Idx&0x0F) != u8flag))
	{
		su8Idx = ((u8Idx2&0x0F)<<4)|(u8flag&0x0F);
		sensor_write_register(0x3812, 0x00);
		for(u8i=0; u8i<1; u8i ++)
		{
			sensor_write_register((XM_U32)gau16LogicAddr_Mipi[u8i], (XM_U32)gau8Logic_sc2335[u8Idx2][u8i]);
		}
		for(u8i=0; u8i<5; u8i ++)
		{
			sensor_write_register((XM_U32)TempLogicAddr_Mipi[u8i], (XM_U32)TempLogic_sc2335[u8i][u8flag]);
		}
		sensor_write_register(0x3812, 0x30);
	}
}

static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	if(gu32ShutNow == u32IntTime)	return;
	gu32ShutNow = u32IntTime;
	u32IntTime = u32IntTime*2;
	sensor_write_register(0x3e00, ((u32IntTime&0xFF000)>>12));
	sensor_write_register(0x3e01, ((u32IntTime&0x0ff0)>>4));
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;

}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	unsigned int tmp[4];

	gau32AllGain = (XM_U64)gu32AGainNow*gu32DGainNow>>10;
	gainLogic_sc2335(gau32AllGain);
	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;
		// 2.GainUpdate
		// a.Again
		tmp[0] = u32Again&0xFF;	
		tmp[1] = (u32Again>>6)|0x03;
		// b.DGain
		tmp[2] = (u32Dgain>>8)&0xFF;
		tmp[3] = u32Dgain&0xFF;
		
		sensor_write_register(0x3e08, tmp[1]);
		sensor_write_register(0x3e09, tmp[0]);
		sensor_write_register(0x3e06, tmp[2]);
		sensor_write_register(0x3e07, tmp[3]);
	}
	return;
}

static XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	return;
}


/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	static XM_U8 su8Mode = 0xFF;
	XM_U32 u32TotalSizeV,u32Pixs;
	XM_U8 u8Mode;

	u32Pixs = 2640;
	switch(u8Fps)
	{
		case 30:// Change the frame rate via changing the vertical blanking
			u32Pixs = u32Pixs*25/20;
			u32TotalSizeV = HD1080P_LINES;
			break;
		case 25:// Change the frame rate via changing the vertical blanking
			u32Pixs = u32Pixs;
			u32TotalSizeV = HD1080P_LINES;
			break;
		default:
			return;
			break;
	}
			
	u8Mode = u8Fps;
	if(su8Mode == u8Mode)
	{
		return;
	}
	su8Mode = u8Mode;
	
	if(pstAeSnsDft != NULL) 
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-5;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	if(gstSnsAttr.u8InputMode == SENSCONT_DVP)
	{
		;
	}
	else	// mipi
	{
		;
	}
	u32Pixs = u32Pixs>>1;
	sensor_write_register(0x320c, (u32Pixs>>8)&0xFF);
	sensor_write_register(0x320d, u32Pixs&0xFF);
	sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);
	gu8Fps = u8Fps;

	SysDelay_ms(20);
	return;
}




/****************************************************************************
* AWB
****************************************************************************/
const static ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,
	{
	    0x000,180, 30, 46,
	    0x000,-22, 230, 48,
	    0x000,-2, -78, 336
	},
	4000,
	{
	    0x000,215, -61, 102,
	    0x000,-7, 142, 121,
	    0x000,4, -136, 388
	},
	2800,
	{
	    0x000,189, -85, 152,
	    0x000,-40, 105, 192,
	    0x000,-58, -284, 598
	}
};

const static ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
	{0, 0, 4096, 2499, 3127, 2234, 3619, 4096},
	{4096, 3779, 0, 0, 4096, 2469, 4096, 1874},
	{-782, -4096, -1385, -4096, -3530, -4096, -3819, -2926},
	213,
	0, 1533, 0, 2786,
	{0, 738, 1082, 1317, 1388, 1445, 1704, 1757, 2786, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1396, 1024, 1582, 0} 
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


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
XM_S32 cmos_init_sensor_exp_function_sc2335(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_sensor_init = NULL;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_sc2335(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_get = NULL;
	pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;    
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}

XM_S32 cmos_init_awb_exp_function_sc2335(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}


// SC2335
static const XM_U16 gau16SnsInit_sc2335_mipi[][2] = {
	{0x0103,0x01},
	{0x0100,0x00},
	{0x36e9,0x80},
	{0x36f9,0x80},
	{0x301f,0x16},
	{0x3200,0x00},
	{0x3201,0x00},
	{0x3202,0x00},
	{0x3203,0x00},
	{0x3204,0x07},
	{0x3205,0x8b},
	{0x3206,0x04},
	{0x3207,0x43},
	{0x3208,0x07},
	{0x3209,0x88},
	{0x320a,0x04},
	{0x320b,0x40},
	{0x3210,0x00},
	{0x3211,0x02},
	{0x3212,0x00},
	{0x3213,0x02},
	{0x3249,0x0f},
	{0x3253,0x08},
	{0x3271,0x00},
	{0x3273,0x03},
	{0x3301,0x06},
	{0x3302,0x09},
	{0x3304,0x28},
	{0x3306,0x30},
	{0x330b,0x94},
	{0x330c,0x08},
	{0x330d,0x18},
	{0x330e,0x14},
	{0x330f,0x05},
	{0x3310,0x06},
	{0x3314,0x96},
	{0x3316,0x00},
	{0x331e,0x21},
	{0x332b,0x08},
	{0x3333,0x10},
	{0x3338,0x80},
	{0x333a,0x04},
	{0x334c,0x04},
	{0x335f,0x04},
	{0x3364,0x17},
	{0x3366,0x62},
	{0x337c,0x05},
	{0x337d,0x09},
	{0x337e,0x00},
	{0x3390,0x08},
	{0x3391,0x18},
	{0x3392,0x38},
	{0x3393,0x09},
	{0x3394,0x20},
	{0x3395,0x20},
	{0x33a2,0x07},
	{0x33ac,0x04},
	{0x33ae,0x14},
	{0x3614,0x00},
	{0x3622,0x16},
	{0x3630,0x68},
	{0x3631,0x84},
	{0x3637,0x20},
	{0x363a,0x1f},
	{0x363c,0x0e},
	{0x3670,0x0e},
	{0x3674,0xa1},
	{0x3675,0x9c},
	{0x3676,0x9e},
	{0x3677,0x84},
	{0x3678,0x85},
	{0x3679,0x87},
	{0x367c,0x18},
	{0x367d,0x38},
	{0x367e,0x08},
	{0x367f,0x18},
	{0x3690,0x32},
	{0x3691,0x32},
	{0x3692,0x44},
	{0x369c,0x08},
	{0x369d,0x38},
	{0x3908,0x82},
	{0x391f,0x18},
	{0x3e01,0x8c},
	{0x3e02,0x00},
	{0x3f00,0x0d},
	{0x3f04,0x02},
	{0x3f05,0x0e},
	{0x3f09,0x48},
	{0x4505,0x0a},
	{0x4509,0x20},
	{0x481d,0x0a},
	{0x4827,0x03},
	{0x5787,0x10},
	{0x5788,0x06},
	{0x578a,0x10},
	{0x578b,0x06},
	{0x5790,0x10},
	{0x5791,0x10},
	{0x5792,0x00},
	{0x5793,0x10},
	{0x5794,0x10},
	{0x5795,0x00},
	{0x5799,0x00},
	{0x57c7,0x10},
	{0x57c8,0x06},
	{0x57ca,0x10},
	{0x57cb,0x06},
	{0x57d1,0x10},
	{0x57d4,0x10},
	{0x57d9,0x00},
	{0x36e9,0x20},
	{0x36f9,0x27},
	{0x0100,0x01},

	{0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 50},		// 50ms
};


XM_U32 sensor_getlist_sc2335(XM_U16 *pu16Num)
{
	DEBUG("------------- SC2335 (@20200104_mipi) ----------------\n");
	*pu16Num = sizeof(gau16SnsInit_sc2335_mipi)/sizeof(gau16SnsInit_sc2335_mipi[0]);
	return (XM_U32)gau16SnsInit_sc2335_mipi;
}




