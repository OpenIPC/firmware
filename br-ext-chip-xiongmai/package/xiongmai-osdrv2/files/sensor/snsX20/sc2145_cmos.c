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

#ifdef SOC_NONE
#include "xm540_tmp.h"
#else
#include "xm510_tmp.h"
#endif
#include "mpi_phyvi.h"


#define HD1080_25_LINES		(1125)

extern XM_U8 gu8Fps;
extern ISP_CMOS_SNS_ATTR_S gstSnsAttr;
extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID XM_MPI_ISP_Memset(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);

static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable = 
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800 */
	/* sharpen_D	H	*/
	{0x3C,0x3C,0x39,0x37,0x32,0x2C,0x1E,0x10,0x06,0x06,0x06,0x06,0x06,0x04,0x04,0x04},
	/* sharpen_Ud M	*/
	{0x32,0x32,0x30,0x2E,0x2A,0x26,0x1C,0x10,0x02,0x02,0x02,0x02,0x02,0x04,0x04,0x04},
	/* sharpen_Kd  */
	{0x44,0x44,0x40,0x3C,0x32,0x26,0x1A,0x0A,0x02,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* snr_thresh 2DNr		*/
	{0x01,0x04,0x08,0x0E,0x14,0x30,0x48,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60},
	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x08,0x0A,0x0C,0x0E,0x10,0x11,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12}, 
	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x00,0x04,0x0C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},  
	/* DyDpc_thresh 		*/
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},   
	/* saturation_strength */
	{0xB8,0xB8,0xB4,0xB0,0xA8,0x96,0x80,0x64,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	/* Blc	*/
	{0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32}
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
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	u32InTimes = u32InTimes/64;
		u32InTimes = u32InTimes < 16? 16: u32InTimes;

	pstAeSnsGainInfo->u32GainDb = u32InTimes;
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes*64;
	return;
}




static XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("cmos_dgain_calc_table failed!\n");
		return;
	}
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
	pstAeSnsGainInfo->u32GainDb = 0;
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

	pstAeSnsDft->u32MaxAgain = 63488;
	pstAeSnsDft->u32MinAgain = 1024;

	pstAeSnsDft->u32MaxDgain = 32;  
	pstAeSnsDft->u32MinDgain = 32;

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


static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	gu32ShutNow = u32IntTime;
	sensor_write_register(0x3e00, ((u32IntTime&0xFF000)>>12));
	sensor_write_register(0x3e01, ((u32IntTime&0x0ff0)>>4));
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;

}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	unsigned int aGain;
	
	aGain = (u32Again>0x03e0)? 0x03e0:u32Again;   
	if(su32AGain == aGain)
		return ;
	su32AGain = aGain;
	//1??Logic
	if(aGain < (16*4))//gain<4
	{
		sensor_write_register(0x3632,0x40);
	}
	else//gain>=4
	{
		sensor_write_register(0x3632,0x70);
	}
	// 2. Update
	sensor_write_register(0x3e08, (aGain >> 8)&0x00ff);
	sensor_write_register(0x3e09, aGain & 0x00ff);
	return;
}

static XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	static XM_U16 preU16FullLine = 0xffff;
	if(preU16FullLine == u16FullLines)
		return;

	preU16FullLine = u16FullLines;
	u16FullLines = (u16FullLines>=4096)?4000:u16FullLines;
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


/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	static XM_U8 su8Mode = 0xFF;
	XM_U32 u32Pixs=2640;
	XM_U32 u32TotalSizeV;
	XM_U32 u32ExpNow;
	XM_U8 u8Mode;

	u32ExpNow = sensor_read_register(0x3e01);
	u32Pixs = sensor_read_register(0x3e02);
	u32ExpNow = ((u32ExpNow<<4)|((u32Pixs>>4)&0x0F))>>1;
	switch(u8Fps)
	{
		case 30:// Change the frame rate via changing the vertical blanking
			u32Pixs = 2200;
			u32TotalSizeV = HD1080_25_LINES;
			break;
		case 25:// Change the frame rate via changing the vertical blanking
			u32Pixs = 2640;
			u32TotalSizeV = HD1080_25_LINES;
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
		pstAeSnsDft->u32MaxIntTime = HD1080_25_LINES-4;
		pstAeSnsDft->u32LinesPer500ms = HD1080_25_LINES * u8Fps / 2;
	}
	if(u32ExpNow > (u32TotalSizeV-4))
	{
		u32ExpNow = u32TotalSizeV-4;
		cmos_inttime_update(u32ExpNow);
	}
	sensor_write_register(0x320c, (u32Pixs>>8)&0xFF);
	sensor_write_register(0x320d, u32Pixs&0xFF);
	sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);
	gu8Fps = u8Fps;
	return;
}



/****************************************************************************
* AWB
****************************************************************************/
const static ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
#if(defined CSTM_AJ)
	5000,//0.3
	{
		0x0000, 256,  0,  0,
		0x0000, 0,  256, 0,
		0x0000, 0,   0,  256
	},
	4000,//0.15
	{
		0x0000, 249, -106,  -113,
		0x0000, 12, 118, 126,
		0x0000, 26, -206,  436
	},
	2800,//0.15
	{
		0x0000,  252, -138,  142,
		0x0000, 3,  90,  163,
		0x0000, -5,  -345,  606
	}
#else
	5000,//0.3
	{
		0x0000, 318,  -133,  71,
		0x0000, 33,  177, 46,
		0x0000, 59,   -169,  366
	},
	4000,//0.15
	{
		0x0000, 249, -106,  -113,
		0x0000, 12, 118, 126,
		0x0000, 26, -206,  436
	},
	2800,//0.15
	{
		0x0000,  252, -138,  142,
		0x0000, 3,  90,  163,
		0x0000, -5,  -345,  606
	}
#endif
};

const static ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
    {0, 0, 4096, 2597, 3493, 1845, 2190, 4096},
    {4096, 3575, 0, 0, 3341, 2505, 4096, 1314},
    {-1292, -4096, -1924, -4096, -4096, -4096, -3600, -3206},
    213,
    0, 1884, 0, 3274,
    {0, 1085, 1312, 1645, 1745, 1921, 2138, 2174, 3274, 0, 0, 0, 0, 0, 0, 0},
    {2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
    {1432, 1024, 1362, 0}
};

static XM_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	if (XM_NULL == pstAwbSnsDft)
	{
		ERR("null pointer when get awb default value!\n");
		return XM_FAILURE;
	}
//	XM_MPI_ISP_Memset((XM_U8*)pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
	pstAwbSnsDft->pstRbTable = (const  AWB_COEF_TABLE_S*)(&gstAwbCal);
	// CCM
	XM_MPI_ISP_Memncpy((XM_U8*)&(pstAwbSnsDft->stCcm), (XM_U8*)&g_stAwbCcm, sizeof(ISP_COLORMATRIX_AUTO_S));
	return XM_SUCCESS;
}


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
XM_S32 cmos_init_sensor_exp_function_sc2145(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
//    XM_MPI_ISP_Memset((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
	pstSensorExpFunc->pfn_cmos_sensor_init = NULL;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_sc2145(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
//    XM_MPI_ISP_Memset((char *)pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
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

XM_S32 cmos_init_awb_exp_function_sc2145(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
//	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}



static const XM_U16 gau16SnsInit_sc2145[][2] = {
	// ExtClk: 37.125M PCLK 74.25M
	{0x0100,0x00},
	
	{0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 50},			// 50ms

	{0x3001,0xfe},
	{0x3018,0x33},
	{0x3031,0x0a},
	{0x3034,0x31},
	{0x3035,0xaa},
	{0x3039,0x00},
	{0x303a,0x36},
	{0x303b,0x06},
	{0x303c,0x08},
	{0x303f,0x01},
	{0x3200,0x00},
	{0x3201,0x00},
	{0x3204,0x07},
	{0x3205,0x9f},
	{0x3208,0x07},
	{0x3209,0x88},
	{0x320a,0x04},
	{0x320b,0x40},
	{0x320c,0x0a},
	{0x320d,0x50},
	{0x3211,0x10},
	{0x3213,0x10},
	{0x3223,0x50},
	{0x322c,0x07},
	{0x322d,0xa8},
	{0x322e,0x08},
	{0x322f,0x37},
	{0x3300,0x20},
	{0x3302,0x28},
	{0x3306,0xd8},
	{0x3308,0x20},
	{0x3309,0x20},
	{0x330a,0x01},
	{0x330b,0xd0},
	{0x330e,0x20},
	{0x331f,0x17},
	{0x3321,0x1a},
	{0x3333,0x80},
	{0x3334,0xa0},
	{0x3364,0x05},
	{0x3367,0x08},
	{0x3368,0x04},
	{0x3369,0x00},
	{0x336a,0x00},
	{0x336b,0x00},
	{0x337f,0x03},
	{0x3400,0x53},
	{0x3401,0x1e},
	{0x3402,0x00},
	{0x3403,0x78},
	{0x3416,0x11},
	{0x3620,0xc4},
	{0x3621,0x18},
	{0x3622,0x0e},
	{0x3623,0x02},
	{0x3630,0x00},
	{0x3631,0x80},
	{0x3632,0x70},
	{0x3633,0x74},
	{0x3635,0x03},
	{0x3637,0x83},
	{0x3638,0x84},
	{0x3670,0x0b},
	{0x3671,0xc2},
	{0x3672,0xc2},
	{0x3673,0x63},
	{0x3674,0xd0},
	{0x3675,0x90},
	{0x3676,0x0f},
	{0x3677,0x0b},
	{0x3678,0x08},
	{0x3679,0x06},
	{0x367a,0x07},
	{0x367b,0x1f},
	{0x367c,0x07},
	{0x367d,0x0f},
	{0x367e,0x07},
	{0x367f,0x1f},
	{0x3907,0x01},
	{0x3908,0xc0},
	{0x3c00,0x41},
	{0x3d08,0x00},
	{0x3e00,0x00},
	{0x3e01,0x8c},
	{0x3e02,0x60},
	{0x3e03,0x03},
	{0x3e07,0x00},
	{0x3e08,0x03},	// 0x0b
	{0x3e09,0x10},
	{0x5781,0x08},
	{0x5782,0x08},
	{0x5785,0x20},
	{0x57a2,0x01},
	{0x57a3,0xf1},
	{0x0100,0x01},
};


XM_U32 sensor_getlist_sc2145(XM_U16 *pu16Num)
{
	if(gstSnsAttr.u8InputMode == SENSCONT_MIPI)
	{
		DEBUG("------------- SC2145 (@20181220_mipi)----------------\n");
		*pu16Num = sizeof(gau16SnsInit_sc2145)/sizeof(gau16SnsInit_sc2145[0]);
		return (XM_U32)gau16SnsInit_sc2145;
	}
	else
	{
		ERR("------------- SC2145 (DVP not Support)----------------\n");
		*pu16Num = 0;
		return (XM_U32)gau16SnsInit_sc2145;
	}
}



