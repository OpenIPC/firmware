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
extern GAIN_MODE_DT gstGainModeDt;

extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID XM_MPI_ISP_Memset(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);
#define HD3MP_25P_LINES		(1584)
static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable = {
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	/* sharpen_D	H	*/
	{0x30,0x2E,0x28,0x20,0x1c,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* sharpen_Ud M	*/
	{0x2E,0x2A,0x28,0x24,0x22,0x14,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* sharpen_Kd  */
	{0x3E,0x3C,0x38,0x30,0x28,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* snr_thresh 2DNr		*/
	{0x00,0x01,0x06,0x08,0x18,0x30,0x40,0x40,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48},
	/* snr_thresh 3DNr	 Tf	*/
	{0x02,0x04,0x06,0x08,0x10,0x14,0x16,0x18,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}, 
	/* snr_thresh 3DNr	 Sf	*/
	{0x01,0x02,0x08,0x0c,0x12,0x28,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},  
	/* DyDpc_thresh 		*/
	{0xB8,0xC0,0xD0,0xD4,0xD8,0xDC,0xE0,0xE0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0},   
	/* saturation_strength */
	{0xA0,0xA0,0x9A,0x90,0x88,0x78,0x50,0x50,0x50,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	/* Blc	*/
	{0x46,0x46,0x46,0x46,0x46,0x44,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},
	/*Y_Tran gamma*/ 
	{0x34,0x34,0x34,0x35,0x35,0x35,0x30,0x30,0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26},
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

STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U32 u32Tmp1,u32Tmp2;
	XM_U16 u16OfstH,u16OfsV;
	u32Tmp1 = sensor_read_register(0x3221);
	u32Tmp2 = sensor_read_register(0x3220);
	u16OfstH = 0;
	u16OfsV = 0;
	if(u8Mirror)
	{
		u32Tmp1 |= 0x06;
	}
	else
	{
		u32Tmp1 &= ~0x06;
	}
		
	if(u8Flip)
	{
		u32Tmp2 |= 0x06;
	}
	else
	{
		u32Tmp2 &= ~0x06;
	}
	sensor_write_register(0x3221, u32Tmp1);
	sensor_write_register(0x3220, u32Tmp2);
	VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfsV);
	return XM_SUCCESS;
}



STATIC XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
	pstAeSnsGainInfo->u32GainDb = u32InTimes;
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
	return;
}


STATIC XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info  value!\n");
		return;
	}
	pstAeSnsGainInfo->u32GainDb = u32InTimes;
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
	return;
}


static XM_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	if (XM_NULL == pstAeSnsDft)
	{
		ERR("null pointer when get ae default value!\n");
		return -1;
	}
	pstAeSnsDft->u32FullLinesStd = HD3MP_25P_LINES;
	pstAeSnsDft->u8AeCompensation = 0x39;	
	pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	pstAeSnsDft->u32FlickerFreq = 0;

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
	pstAeSnsDft->u32MinIntTime = 2;    

	pstAeSnsDft->u32MaxAgain = 1984;// 124
	pstAeSnsDft->u32MinAgain = 16;

	pstAeSnsDft->u32MaxDgain = 32;  
	pstAeSnsDft->u32MinDgain = 32;

	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
	pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;

	pstAeSnsDft->bDelayCfg = XM_FALSE;
	return 0;
}

static XM_S32 cmos_get_sensor_max_resolution(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution)
{
	if (XM_NULL == pstSensorMaxResolution)
	{
	    ERR("null pointer when get sensor max resolution \n");
	    return -1;
	}
	pstSensorMaxResolution->u32MaxWidth  = 2048;
	pstSensorMaxResolution->u32MaxHeight = 1536;
	return 0;
}



STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	XM_U32 u32Tmp;
	if(gu32ShutNow == u32IntTime)	return;
	gu32ShutNow = u32IntTime;
	u32Tmp = ((u32IntTime&0x0ff0)>>4);
	
	sensor_write_register(0x3e01, u32Tmp);
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;
}


const static XM_U8 gau8Logic_sc3035[3][4] = {
	// 0x3630,0x3635,0x3620,0x3627
		{0xa9, 0x66, 0xd2, 0x02},
		{0x67, 0x64, 0xd2, 0x02},
		{0x67, 0x62, 0x63, 0x06}
};
const static XM_U16 gau16LogicAddr[4] = {
	0x3630,0x3635,0x3620,0x3627
};

static void gainLogic_sc3035(XM_U32 u32AllGain)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Vstd   [bit4~bit7]:Agc
	XM_U8 u8Idx2,u8i;
	u32AllGain = u32AllGain/64;	// *1024->*16
	if(u32AllGain<32)
		u8Idx2 = 0;
	else if(u32AllGain<256)
		u8Idx2 = 1;
	else
		u8Idx2 = 2;
	if(	((su8Idx>>4)&0x0F) != u8Idx2)
	{
		su8Idx = ((u8Idx2&0x0F)<<4);
		for(u8i=0; u8i<4; u8i ++)
		{
			sensor_write_register((XM_U32)gau16LogicAddr[u8i], (XM_U32)gau8Logic_sc3035[u8Idx2][u8i]);
		}
	}
}


STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	XM_U32 u32AllGain;
	u32AllGain = u32Again; 
	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;
		// 1. Logic
		gainLogic_sc3035(u32AllGain);

		// 2.GainUpdate
		sensor_write_register(0x3e08, (u32Again>>8)&0xFF);
		sensor_write_register(0x3e09, u32Again&0xFF);
	}
	return;
}




/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32Tmp;
	switch(u8Fps)
	{
		case 30:
			// Change the frame rate via changing the vertical blanking
			u32Tmp = 2500;
			break;

		case 25:
		default:
			// Change the frame rate via changing the vertical blanking
			u32Tmp = 3000;
			break;
	}	
	
	if(pstAeSnsDft != NULL) 
	{
		pstAeSnsDft->u32FullLinesStd = HD3MP_25P_LINES;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-gu8MaxShutterOfst;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	u32Tmp = u32Tmp>>1;
	sensor_write_register(0x320c, (u32Tmp>>8)&0xFF);
	sensor_write_register(0x320d, u32Tmp&0xFF);
	return;
}

static XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	return ;
    static XM_U16 preU16FullLine = 0xffff;
    if(preU16FullLine == u16FullLines)
        return;

    preU16FullLine = u16FullLines;
    u16FullLines = (u16FullLines>=4096)?4000:u16FullLines;
    pstAeSnsDft->u32MaxIntTime = u16FullLines - gu8MaxShutterOfst;
    SysDelay_ms(100);
    if(gu32ShutNow>pstAeSnsDft->u32MaxIntTime)
    {
        cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
    }
	sensor_write_register(0x320e, (u16FullLines&0xff00)>>8);	
	sensor_write_register(0x320f, u16FullLines&0xff);      
       
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}



/****************************************************************************
* AWB
****************************************************************************/
static const ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,
	{
		0x000,356,  -135, 35,
		0x000,-59,  327, -12,
        0x000, 5,  -145, 396//0x000, -35, -130, 421
	},
	4000,
	{
		0x000,327, -130,59,
		0x000,-70,281,45,
		0x000,-39,-233,528
	},
	2800,
	{
		0x000, 287,-86,55,
		0x000,-87,269,74,
		0x000, -115,-369,740
	}
};
static const ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
	{0, 0, 4096, 2602, 3773, 1836, 1107, 4096},
	{4096, 3420, 0, 0, 3625, 2345, 4096, 1355},
	{-1150, -4096, -1687, -4096, -4096, -4096, -2524, -2972},
	213,
	0, 1908, 0, 3281,
	{0, 1073, 1314, 1642, 1752, 1900, 2145, 2166, 3281, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1386, 1024, 1400, 0}
};

static XM_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	if(XM_NULL == pstAwbSnsDft)
	{
		ERR("null pointer when get awb default value!\n");
		return XM_FAILURE;
	}
	XM_MPI_ISP_Memset((XM_U8*)pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
	pstAwbSnsDft->pstRbTable = (const  AWB_COEF_TABLE_S*)(&gstAwbCal);
	// CCM
	XM_MPI_ISP_Memncpy((XM_U8*)&(pstAwbSnsDft->stCcm), (XM_U8*)&g_stAwbCcm, sizeof(ISP_COLORMATRIX_AUTO_S));
	return XM_SUCCESS;
}




XM_S32 cmos_init_sensor_exp_function_sc3035(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	//	  XM_MPI_ISP_Memset((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
	pstSensorExpFunc->pfn_cmos_sensor_init = NULL;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;//cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;//cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}


XM_S32 cmos_init_ae_exp_function_sc3035(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
//	  XM_MPI_ISP_Memset((char *)pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
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


XM_S32 cmos_init_awb_exp_function_sc3035(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}


