#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"
#include "Camera.h"

#include "xm_print.h"
#include "X50_cmos.h"

#ifdef SOC_NONE
#include "xm540_tmp.h"
#else
#include "xm510_tmp.h"
#endif
#include "mpi_phyvi.h"


#define HD5MP_25P_LINES		(1980)


extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID XM_MPI_ISP_Memset(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);

static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable = {
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	/* sharpen_D	H	*/
	{0x14,0x14,0x14,0x14,0x12,0x12,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E},
	/* sharpen_Ud M	*/
	{0x14,0x14,0x14,0x14,0x14,0x14,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	/* sharpen_Kd  */
	{0x40,0x3E,0x3E,0x3C,0x3C,0x28,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* snr_thresh 2DNr		*/
	{0x00,0x02,0x06,0x0C,0x1C,0x28,0x40,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48},
	/* snr_thresh 3DNr	 Tf	*/
	{0x0F,0x11,0x13,0x15,0x18,0x1A,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}, 
	/* snr_thresh 3DNr	 Sf	*/
	{0x10,0x10,0x18,0x20,0x28,0x2A,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},  
	/* DyDpc_thresh 		*/
	{0xB8,0xC0,0xD0,0xD4,0xD8,0xDC,0xE0,0xE0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0},   
	/* saturation_strength */
	{0xA0,0x9A,0x94,0x8C,0x7C,0x64,0x50,0x50,0x50,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	/* Blc	*/
	{0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x30,0x2e,0x2a,0x2a,0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26}
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
	XM_U16 u16OfstV;
	XM_U32 u32Tmp;

	u32Tmp = sensor_read_register(0x12);
	u32Tmp = u32Tmp&0xCF;
	if(u8Mirror)
	{
		u32Tmp |= 0x20;
	}
	else
	{
		u32Tmp &= 0xDF;
	}
		
	if(u8Flip)
	{
		u32Tmp |= 0x10;
		u16OfstV = 0;
	}
	else
	{
		u32Tmp &= 0xEF;
		u16OfstV = 0;
	}
	sensor_write_register(0x12, u32Tmp);
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
	u32InTimes = u32InTimes < 32? 32: u32InTimes;//15.75

	for(i = 0; i < 4; i++)//0 1 2 3
	{
		if(u32InTimes < 64)// 1 2 4 8 16
		{
			break;
		}
		u32InTimes >>= 1;
	}
	pstAeSnsGainInfo->u32GainDb = (i<<4)+u32InTimes-32; 
	u32InTimes = u32InTimes<<i;
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes*32;
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
	pstAeSnsDft->u32FullLinesStd = HD5MP_25P_LINES;
	pstAeSnsDft->u8AeCompensation = 0x39;	
	pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	pstAeSnsDft->u32FlickerFreq = 0;

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
	pstAeSnsDft->u32MinIntTime = 2;    

	pstAeSnsDft->u32MaxAgain = 16182;//31744 15872
	pstAeSnsDft->u32MinAgain = 1024;

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

	pstSensorMaxResolution->u32MaxWidth  = 2592;
	pstSensorMaxResolution->u32MaxHeight = 1944;
	return 0;
}

static XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	static XM_U16 preU16FullLine = 0xffff;

	if(preU16FullLine == u16FullLines)
		return;
	preU16FullLine = u16FullLines;
	
	u16FullLines = (u16FullLines>=4096)?4000:u16FullLines;

	sensor_write_register(0x320e, (u16FullLines&0xff00)>>8);	
	sensor_write_register(0x320f, u16FullLines&0xff);
	pstAeSnsDft->u32MaxIntTime = u16FullLines - 4;
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}


static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	XM_U32 u32Tmp;
	gu32ShutNow = u32IntTime;
	u32IntTime = u32IntTime*2;

	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;
	u32Tmp = ((u32IntTime&0x0ff0)>>4);
	sensor_write_register(0x3e00, (u32IntTime>>12)&0x0F);
	sensor_write_register(0x3e01, u32Tmp);
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;
}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
    static XM_U32 Preu32Again = 0xffff;
	XM_U32 u32Tmp;
    if(Preu32Again == u32Again)
        return;
    Preu32Again = u32Again;
	// 2.GainUpdate
	u32Tmp = ((u32Again&0xf0) >> 2)|0x03;
	sensor_write_register(0x3e08, u32Tmp);
	sensor_write_register(0x3e09, ((u32Again&0x0f)+0x20));
	#if 0
	// 2.LogicUpdate
    if(u32Tmp < 0x07)//[gain<2]
    {
        sensor_write_register(0x3301,0x1c);//20181101  [12,24]
        sensor_write_register(0x3630,0x30);//20180713
        sensor_write_register(0x3633,0x34);//20180713
        sensor_write_register(0x3622,0xf6);
        sensor_write_register(0x363a,0x83);
    }
    else if(u32Tmp < 0x0f)//[4>gain>=2]
    {
        sensor_write_register(0x3301,0x28);//20181101  [18,66]
        sensor_write_register(0x3630,0x34);//20180129
        sensor_write_register(0x3633,0x35);//20180713
        sensor_write_register(0x3622,0xf6);
        sensor_write_register(0x363a,0x87);
    }
    else if(u32Tmp < 0x1f)//[8>gain>=4]
    {
        sensor_write_register(0x3301,0x28);//20181101  [1a,66]
        sensor_write_register(0x3630,0x24);//1114
        sensor_write_register(0x3633,0x35);//20180713
        sensor_write_register(0x3622,0xf6);
        sensor_write_register(0x363a,0x9f);
    }
    else//[15.5>gain>=8]
    {
        sensor_write_register(0x3301,0x48);//20181101  [1e,66]
        sensor_write_register(0x3630,0x16); 
        sensor_write_register(0x3633,0x45);//20180713
        sensor_write_register(0x3622,0xf6);
        sensor_write_register(0x363a,0x9f);
    }
    /*
    [gain>=15.75]
    sensor_write_register(0x3301,0x48);//20181101  [21,66]
    sensor_write_register(0x3630,0x09);//0f  1226
    sensor_write_register(0x3633,0x46);//20180713
    sensor_write_register(0x3622,0x16);//20180713
    sensor_write_register(0x363a,0x9f);
    */
    #endif
	return;
}

/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32TotalSizeV;

	u32TotalSizeV = HD5MP_25P_LINES;
	switch(u8Fps)
	{
		// CVBS_PAL   CVBS_NTSC
		case 50:
		case 60:// 1560*770*59.94
			DEBUG("cmos_fps_set 5M 50/60fps\n");
			break;
		case 25:// 4M_25Fps
		case 30:// 4M_30Fps
			DEBUG("cmos_fps_set 5M 25/30fps\n");
			break;
		case 15:// 5M_15Fps
			u8Fps = 15;
			u32TotalSizeV = HD5MP_25P_LINES*20/u8Fps;
			DEBUG("cmos_fps_set 5M 15fps\n");
			break;
		case 10:// 5M_10Fps
			u8Fps = 10;
			u32TotalSizeV = HD5MP_25P_LINES*20/u8Fps;
			DEBUG("cmos_fps_set 5M 10fps\n");
			break;
		case 20:// 5M_20Fps
		default:
			u8Fps = 20;
			u32TotalSizeV = HD5MP_25P_LINES;
			DEBUG("cmos_fps_set 5M 20fps\n");
			break;
	}		
	// Change the frame rate via changing the vertical blanking
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-2;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
    sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);
	return;
}


/****************************************************************************
* AWB
****************************************************************************/
static ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,//0.14392
	{
		0x0000, 294,  -16,  -22,
		0x0000, -89,  373, -28,
		0x0000, 8,   -59,  307
	},
	4000,//0.14392
	{
		0x0000, 289, -9,  -24,
		0x0000, -64, 370, -50,
		0x0000, -13, -49,  318
	},
	2800,//0.10486
	{
		0x0000,  260, 51,   -55,
		0x0000, -92,  421,  -73,
		0x0000, -50,  -30,  336
	}
};
const static ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
    {0, 0, 4096, 3549, 3580, 2294, 1470, 4096},
    {4096, 4053, 0, 0, 4096, 2736, 4096, 1441},
    {-1342, -4096, -1559, -4096, -3963, -4096, -2726, -2768},
    213,
    0, 1598, 0, 2783,
    {0, 1102, 1217, 1417, 1518, 1620, 1821, 1843, 2783, 0, 0, 0, 0, 0, 0, 0},
    {2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
    {1632, 1024, 1578, 0}
};

static XM_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	if (XM_NULL == pstAwbSnsDft)
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

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
XM_S32 cmos_init_sensor_exp_function_sc5235(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	//    XM_MPI_ISP_Memset((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
	pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_sc5235(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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

XM_S32 cmos_init_awb_exp_function_sc5235(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}








