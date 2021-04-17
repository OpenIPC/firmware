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


#define HD1296_25_LINES		(1350)

extern XM_U8 gu8Fps;
extern ISP_CMOS_SNS_ATTR_S gstSnsAttr;
extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID XM_MPI_ISP_Memset(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);
extern XM_U32 gau32AllGain;
extern XM_U32 gu32AGainNow;
extern XM_U32 gu32DGainNow;

static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable = 
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800 */
	/* sharpen_D	H	*/
	{0x34,0x34,0x2C,0x2C,0x2A,0x22,0x1C,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* sharpen_Ud M	*/
	{0x34,0x34,0x30,0x2C,0x2A,0x24,0x18,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* sharpen_Kd  */
	{0x3C,0x3C,0x2E,0x2C,0x28,0x24,0x1C,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* snr_thresh 2DNr		*/
	{0x01,0x02,0x03,0x06,0x09,0x0b,0x0F,0x1C,0x30,0x40,0x40,0x40,0x40,0x40,0x40,0x40},
	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x05,0x08,0x0A,0x11,0x18,0x1A,0x20,0x20,0x19,0x18,0x18,0x18,0x18,0x18,0x18}, 
	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x01,0x04,0x0C,0x10,0x14,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x10,0x10},  
	/* DyDpc_thresh 		*/
	{0x00,0x00,0x00,0x00,0x00,0xC0,0xD0,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8},   
	/* saturation_strength */
	{0xAF,0xAF,0xAF,0xAA,0xA8,0x96,0x6E,0x64,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	/* Blc	*/
	{0x104,0x104,0x104,0x104,0x0F0,0x0F0,0x0E6,0x0DC,0x0DC,0x0DC,0x104,0x104,0x104,0x104,0x104,0x104},
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
	int i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
 
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	u32InTimes = u32InTimes/32;
	u32InTimes = u32InTimes < 32? 32: u32InTimes;

	for(i = 0; i < 4; i++)
	{
		if(u32InTimes < 64)
		{
			break;
		}
		u32InTimes >>= 1;
		pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<1)|1;
	}
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8)+u32InTimes; 
	u32InTimes = u32InTimes<<i;
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes*32;
	gu32AGainNow = pstAeSnsGainInfo->u32SnsTimes;
	return;
}

static XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("cmos_dgain_calc_table failed!\n");
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;
	for(i = 0; i < 5; i++)
	{
		if(u32InTimes < 256)
		{
			break;
		}
		u32InTimes >>= 1;
		pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<1)|1;
	}
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes<<i;
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8)+u32InTimes;
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
	pstAeSnsDft->u32FullLinesStd = HD1296_25_LINES;
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


	gu32AGainNow = pstAeSnsDft->u32MinAgain;
	gu32DGainNow = pstAeSnsDft->u32MinDgain;
	return 0;
}

static XM_S32 cmos_get_sensor_max_resolution(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution)
{
	if (XM_NULL == pstSensorMaxResolution)
	{
	    ERR("null pointer when get sensor max resolution \n");
	    return -1;
	}

	pstSensorMaxResolution->u32MaxWidth  = 2304;
	pstSensorMaxResolution->u32MaxHeight = 1296;
	return 0;
}

static void shutLogic_Auge(XM_U32 u32IntTime)
{
#if 0
	static XM_U8 su8Val = 0xFF;
	XM_U8 u8Val;
	u8Val = su8Val;
	if(u32IntTime < 0x250)	u8Val = 0x14;
	else if(u32IntTime > 0x450)	u8Val = 0x04;
	if(su8Val!=u8Val)
	{
		su8Val = u8Val;
		sensor_write_register(0x3314, (XM_U32)u8Val);
	}
#endif
}
#if 0
const static XM_U8 gau8Logic_Auge[3][5] = {
	//  0x3633,0x3622,0x3301,0x3630,0x3632
		{0x42, 0xf6, 0x80, 0xc3, 0x18},
		{0x53, 0x76, 0x90, 0x98, 0x58},
		{0x54, 0x16, 0xa0, 0x6c, 0xd8}
};
const static XM_U16 gau16LogicAddr_Mipi[5] = {
	0x3633,0x3622,0x3301,0x3630,0x3632
};
#else
const static XM_U8 gau8Logic_Auge[3][3] = {
	//  0x3632,0x3306,0x330b
		{0x18, 0x50, 0xd0},
		{0x58, 0x58, 0xd8},
		{0xd8, 0x5c, 0xdb}
};
const static XM_U16 gau16LogicAddr_Mipi[3] = {
	0x3632,0x3306,0x330b
};

#endif
static void gainLogic_Auge(XM_U32 u32AllGain)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Vstd   [bit4~bit7]:Agc
	XM_U8 u8Idx2,u8i;
	u32AllGain = u32AllGain/64;	// *1024->*16
	if(u32AllGain<32)
		u8Idx2 = 0;
	else if(u32AllGain<128)
		u8Idx2 = 1;
	else
		u8Idx2 = 2;
	if(	((su8Idx>>4)&0x0F) != u8Idx2)
	{
		su8Idx = ((u8Idx2&0x0F)<<4);
		sensor_write_register(0x3812, 0x00);
		for(u8i=0; u8i<3; u8i ++)
		{
			sensor_write_register((XM_U32)gau16LogicAddr_Mipi[u8i], (XM_U32)gau8Logic_Auge[u8Idx2][u8i]);
		}
		sensor_write_register(0x3812, 0x30);
	}
}

static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	if(gu32ShutNow == u32IntTime)	return;
	gu32ShutNow = u32IntTime;
	u32IntTime = u32IntTime*2;
	shutLogic_Auge(u32IntTime);
	sensor_write_register(0x3e00, ((u32IntTime&0xFF000)>>12));
	sensor_write_register(0x3e01, ((u32IntTime&0x0ff0)>>4));
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;

}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	unsigned int u32Tmp,tmp[4];
	gau32AllGain = (XM_U64)gu32AGainNow*gu32DGainNow/128;
	gainLogic_Auge(gau32AllGain);
	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;
		// 2.GainUpdate
		tmp[0] = u32Again&0xFF;	
		u32Tmp = (u32Again>>8) & 0xFF;
		tmp[1] = 0x03 | (u32Tmp<<2);

		tmp[2] = u32Dgain&0xFF;	
		tmp[3] = (u32Dgain>>8) & 0xFF;
		sensor_write_register(0x3e08, tmp[1]);
		sensor_write_register(0x3e09, tmp[0]);
		sensor_write_register(0x3e06, tmp[3]);
		sensor_write_register(0x3e07, tmp[2]);
	}
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
	XM_U32 u32Pixs=2880;
	XM_U32 u32TotalSizeV;
	XM_U32 u32ExpNow;
	XM_U8 u8Mode;
	u32ExpNow = sensor_read_register(0x3e00);
	u32ExpNow = (u32ExpNow<<8) | sensor_read_register(0x3e01);
	u32Pixs = sensor_read_register(0x3e02);
	u32ExpNow = (u32ExpNow<<4)|((u32Pixs>>4)&0x0F);
	switch(u8Fps)
	{
		case 30:
			u32Pixs = 2400;
			u32TotalSizeV = HD1296_25_LINES;
			break;
		case 25:
			u32Pixs = 2880;
			u32TotalSizeV = HD1296_25_LINES;
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
		pstAeSnsDft->u32MaxIntTime = HD1296_25_LINES-4;
		pstAeSnsDft->u32LinesPer500ms = HD1296_25_LINES * u8Fps / 2;
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
#if 0
	5000,
	{
		0x0000, 353,  -166,  69,
		0x0000, -126,  363,  19,
		0x0000, -4,   -196,  456
	},
	4000,
	{
		0x0000, 329, -177,  104,
		0x0000, -148, 368, 36,
		0x0000, -60, -325,  641
	},
	2800,
	{
		0x0000, 264,  -96,  88,
		0x0000, -165, 344,  77,
		0x0000, -162, -468,  886
	}
#else
#if(defined CSTM_AJ)
	5000,
	{
		0x0000, 256,  0,  0,
		0x0000, 0,  256,  0,
		0x0000, 32,  0,  224
	},
	4000,
	{
		0x0000, 256,  0,  0,
		0x0000, 0,  256,  0,
		0x0000, 30,  0,  226
	},
	2800,
	{
		0x0000, 256,  0,  0,
		0x0000, 0,  256,  0,
		0x0000, 30,  0,  226
	}
#else
	5000,
	{
		0x0000, 385,  -187,  58,
		0x0000, -107,  407,  -44,
		0x0000, -15,  -221,  492
	},
	4000,
	{
		0x0000, 321, -173,	108,
		0x0000, -144, 357, 43,
		0x0000, -58, -317,	631
	},
	2800,
	{
		0x0000, 293,  -99,	62,
		0x0000, -180, 389,	47,
		0x0000, -177, -508,  941
	}

#endif
#endif
};

const static ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
	{0, 0, 4096, 3102, 4096, 2342, 1299, 4096},
	{4096, 4096, 0, 0, 3885, 2729, 4096, 1166},
	{-756, -4004, -1201, -4096, -3483, -4096, -2012, -2081},
	213,
	0, 1467, 0, 2589,
	{0, 754, 976, 1210, 1294, 1382, 1595, 1618, 2589, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1610, 1024, 1853, 0}
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
XM_S32 cmos_init_sensor_exp_function_auge(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_sensor_init = NULL;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_auge(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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

XM_S32 cmos_init_awb_exp_function_auge(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}



static const XM_U16 gau16SnsInit_auge[][2] = {
#if 0
	{0x0100,0x00},
	{0x36e9,0x87},
	{0x36f9,0xb3},
	{0x36eb,0x05},
	{0x36ec,0x15},
	{0x36ed,0x04},
	{0x36fb,0x23},
	{0x36fc,0x01},
	{0x36fd,0x14},
	{0x3e09,0x20},
	{0x3637,0x2c},
	{0x3635,0x10},
	{0x363b,0x10},
	{0x363c,0x07},
	{0x3306,0x58},
	{0x330a,0x00},
	{0x330b,0xd8},
	{0x3638,0x28},
	{0x331c,0x01},
	{0x3304,0x30},
	{0x331e,0x29},
	{0x3320,0x03},
	{0x3356,0x01},
	{0x57a4,0xa0},
	{0x5781,0x04},
	{0x5782,0x04},
	{0x5783,0x02},
	{0x5784,0x02},
	{0x5785,0x40},
	{0x5786,0x20},
	{0x5787,0x18},
	{0x5788,0x10},
	{0x5789,0x10},
	{0x578a,0x30},
	{0x3908,0x41},
	{0x3622,0xf6},
	{0x3e25,0x03},
	{0x3e26,0x20},
	{0x3902,0xc5},
	{0x3905,0x99},
	{0x3314,0x94},
	{0x3347,0x05},
	{0x3301,0x80},
	{0x3630,0xc3},
	{0x3633,0x42},
	{0x363a,0xa8},
	{0x3614,0x80},
	{0x3632,0x18},
	{0x3631,0x8a},
	{0x3e01,0x62},
	{0x3208,0x09},
	{0x3209,0x08},
	{0x320a,0x05},
	{0x320b,0x18},
	{0x320C,2880>>8},
	{0x320D,2880&0xFF},
	{0x3211,0x04},
	{0x3213,0x04},
	{0x36e9,0x07},
	{0x36f9,0x33},
	{0x0100,0x01},


	{0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 50},		// 50ms
#else
    {0x0103,0x01},
    {0x0100,0x00},
              
    {0x3641,0x0c},
    {0x36e9,0x07}, // PLL1 972M
    {0x36eb,0x05},
    {0x36ec,0x15},
    {0x36ed,0x04}, // MIPI 2xlane 486M
    {0x36f9,0x33}, // PLL_DAC 777.6M
    {0x36fb,0x23},
    {0x36fc,0x01},
    {0x36fd,0x14},
    {0x3641,0x00},
               
    {0x301f,0x01},
    {0x3038,0x66},
               
    {0x3208,0x09},
    {0x3209,0x08},
    {0x320a,0x05},
    {0x320b,0x18},
    {0x320C,2880>>8},
	{0x320D,2880&0xFF},
    {0x3211,0x04},
    {0x3213,0x04},
               
    {0x3253,0x08},
    {0x3301,0x11},
    {0x3304,0x30},
    {0x3306,0x50},
    {0x330a,0x00},
    {0x330b,0xd0},
    {0x330e,0x30},
    {0x3314,0x94},
    {0x331c,0x01},
    {0x331e,0x29},
    {0x3320,0x03},
    {0x3347,0x05},
    {0x334c,0x10},
    {0x3356,0x01},
    {0x3364,0x17},
    {0x3367,0x10},
    {0x3368,0x04},
    {0x3369,0x00},
    {0x336a,0x00},
    {0x336b,0x00},
    {0x3390,0x08},
    {0x3391,0x38},
    {0x3392,0x38},
    {0x3393,0x1a},
    {0x3394,0x88},
    {0x3395,0x88},
    {0x360f,0x05},
    {0x3614,0x80},
    {0x3622,0xf6},
    {0x3630,0xc3},
    {0x3631,0x8a},
    {0x3632,0x18},
    {0x3633,0x44},
    {0x3635,0x20},
    {0x3637,0x2c},
    {0x3638,0x28},
    {0x363a,0xa8},
    {0x363b,0x20},
    {0x363c,0x06},
    {0x3670,0x0a},
    {0x3671,0xf6},
    {0x3672,0x76},
    {0x3673,0x16},
    {0x3674,0xa0},
    {0x3675,0x98},
    {0x3676,0x6a},
    {0x367a,0x08},
    {0x367b,0x38},
    {0x367c,0x08},
    {0x367d,0x38},
    {0x3690,0x64},
    {0x3691,0x63},
    {0x3692,0x64},
    {0x369c,0x08},
    {0x369d,0x38},
    {0x3900,0x29},
    {0x3902,0xc5},
    {0x3905,0xd1},
    {0x3906,0x62},
    {0x3908,0x41},
    {0x3909,0x00},
    {0x390a,0x19},
    {0x390b,0x00},
    {0x390c,0x4c},
    {0x390d,0x00},
    {0x390e,0x19},
    {0x390f,0x00},
    {0x3910,0x4c},
    {0x391d,0x04},
    {0x391e,0x00},
    {0x3920,0x00},
    {0x3921,0x4c},
    {0x3922,0x00},
    {0x3923,0x19},
    {0x3924,0x00},
    {0x3925,0x4c},
    {0x3926,0x00},
    {0x3927,0x19},
    {0x3933,0x0a},
    {0x3934,0x28},
    {0x3935,0x18},
    {0x3936,0x08},
    {0x3937,0x13},
    {0x3940,0x68},
    {0x3942,0x02},
    {0x3943,0x33},
    {0x3e01,0xa8},
    {0x3e02,0x40},
    {0x3e09,0x20},
    {0x3e1b,0x35},
    {0x3e25,0x03},
    {0x3e26,0x20},
    {0x5781,0x04},
    {0x5782,0x04},
    {0x5783,0x02},
    {0x5784,0x02},
    {0x5785,0x40},
    {0x5786,0x20},
    {0x5787,0x18},
    {0x5788,0x10},
    {0x5789,0x10},
    {0x578a,0x30},
    {0x57a4,0xa0},
    {0x0100,0x01},
    {0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 50},		// 50ms
#endif
};


XM_U32 sensor_getlist_auge(XM_U16 *pu16Num)
{
	DEBUG("------------- auge (@20190126) ----------------\n");
	*pu16Num = sizeof(gau16SnsInit_auge)/sizeof(gau16SnsInit_auge[0]);
	return (XM_U32)gau16SnsInit_auge;
}



