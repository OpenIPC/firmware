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

#define HD5MP_25P_LINES		(2475)
static XM_U16 gu16FullLines = HD5MP_25P_LINES;

extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID XM_MPI_ISP_Memset(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);
extern XM_U32 gu32ShutNow;
extern PROFILE gstRlstType;
static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable = {
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	/* sharpen_D	H	*/
	{0x3C,0x3C,0x30,0x2C,0x28,0x20,0x16,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* sharpen_Ud M	*/
	{0x30,0x30,0x2E,0x28,0x24,0x20,0x18,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* sharpen_Kd  */
	{0x3E,0x3A,0x34,0x32,0x30,0x28,0x20,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* snr_thresh 2DNr		*/
	{0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x30,0x38,0x40,0x40,0x40,0x40,0x40,0x40,0x40},
	/* snr_thresh 3DNr	 Tf	*/
	{0x02,0x04,0x08,0x0a,0x0C,0x11,0x14,0x18,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b}, 
	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x01,0x04,0x0A,0x10,0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, 
	/* DyDpc_thresh 		*/
	{0x97,0xA8,0xD0,0xD4,0xD8,0xDC,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0},
	/* saturation_strength */
	{0xA8,0xA6,0xA4,0x9c,0x90,0x88,0x74,0x5C,0x50,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
	/* Blc	*/
	{0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x35,0x35,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x2E,0x2E,0x2E}
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
    //XM_U32 u32TmpH = sensor_read_register(0x304E);
   // XM_U32 u32TmpV = sensor_read_register(0x304F);
    if(u8Mirror)
    {
        sensor_write_register(0x304E, 0x01);
    }
    else
    {
        sensor_write_register(0x304E, 0x00);
    }
    if(u8Flip)
    {
        sensor_write_register(0x304F, 0x01);
        sensor_write_register(0x3081, 0xFE);
        sensor_write_register(0x3083, 0xFE);
        sensor_write_register(0x30B6, 0xFA);
        sensor_write_register(0x30B7, 0x01);
        sensor_write_register(0x3116, 0x02);
    }
    else
    {
        sensor_write_register(0x304F, 0x00);
        sensor_write_register(0x3081, 0x02);
        sensor_write_register(0x3083, 0x02);
        sensor_write_register(0x30B6, 0x00);
        sensor_write_register(0x30B7, 0x00);
        sensor_write_register(0x3116, 0x08);
    }
    //sensor_write_register(0x304E, u32TmpH);
    //sensor_write_register(0x304F, u32TmpV);

    VI_WinSet(1, u8Mirror, u8Flip, 0, 0);
    XM_MPI_MIPI_RefreshFV(100, 0);
    return XM_SUCCESS;
 }


 static const XM_U32 gau32GainTbl[141] =
 {1024,  1059,	1097,  1135, 1175, 1217, 1259, 1304, 1349, 1397, 1446, 1497, 1549, 1604,
  1660,  1719,	1779,  1842, 1906, 1973, 2043, 2048, 2119, 2194, 2271, 2351, 2434, 2519, 
  2608,  2699,	2794,  2892, 2994, 3099, 3208, 3321, 3438, 3559, 3684, 3813, 3947, 4086, 
  4229,  4378,	4532,  4691, 4856, 5027, 5203, 5386, 5576, 5772, 5974, 6184, 6402, 6627, 
  6860,  7101,	7350,  7609, 7876, 8153, 8439, 8736, 9043, 9361, 9690, 10030,10383,10748,
  11125, 11516, 11921, 12340,12774,13222,13687,14168,14666,15182,15715,16267,16839,17431,
  18043, 18677, 19334, 20013,20717,21445,22198,22978,23786,24622,25487,26383,27310,28270,
  29263, 30292, 31356, 32458,33599,34780,36002,37267,38577,39932,41336,42788,44292,45849,
  47460, 49128, 50854, 52641,54491,56406,58388,60440,62564,64763,67039,69395,71833,74358,
  76971, 79676, 82476, 85374,88375,91480,94695,98023,101468,105034,108725,112545,116501,
  120595,124833};

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

	if (u32InTimes >= gau32GainTbl[140])       
	{            
		pstAeSnsGainInfo->u32GainDb = 140;
	}
	else
	{
		for(i = 1; i <= 140; i++)        
		{           
			if(u32InTimes < gau32GainTbl[i])            
			{  
				pstAeSnsGainInfo->u32GainDb = i-1;
				break;            
			}            
		}    
	}
	pstAeSnsGainInfo->u32SnsTimes = gau32GainTbl[pstAeSnsGainInfo->u32GainDb];
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

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-9;
	pstAeSnsDft->u32MinIntTime = 1;    

	pstAeSnsDft->u32MaxAgain = 62564;// 15.5
	pstAeSnsDft->u32MinAgain = 1024;

	pstAeSnsDft->u32MaxDgain = 1024;  
	pstAeSnsDft->u32MinDgain = 1024;

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

static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32Time = 0xFFFF;
    XM_U32 u32Tmp;	
	gu32ShutNow = u32IntTime;
	u32IntTime = gu16FullLines - u32IntTime;
	if(su32Time == u32IntTime)		return ;
	su32Time = u32IntTime;
	u32Tmp = u32IntTime*4;
	sensor_write_register(0x3058, u32Tmp&0xFF);
	sensor_write_register(0x3059, (u32Tmp>>8)&0xFF);
	sensor_write_register(0x305a, (u32Tmp>>16)&0x0F);
	return;
}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
    static XM_U32 Preu32Again = 0xffff;
    XM_U32 u32Tmp;	
    
    if(Preu32Again == u32Again)
        return;
    Preu32Again = u32Again;
    u32Tmp = u32Again + u32Dgain;   

	sensor_write_register(0x30E8, u32Tmp & 0xFF);
	sensor_write_register(0x30E9, (u32Tmp>>8)&0x07);
	return;
}

/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32TotalSizeV;
    XM_U32 u32Temp;
	u32TotalSizeV = HD5MP_25P_LINES;
	switch(gstRlstType)
	{
		case P5M_:
		default:	u8Fps = 15;
					u32TotalSizeV = HD5MP_25P_LINES;
					DEBUG("cmos_fps_set 5M 15fps\n");
					break;
		case P1080_:
		case P4M_:	u8Fps = 20;
					u32TotalSizeV = HD5MP_25P_LINES*15/u8Fps;
					DEBUG("cmos_fps_set 5M 20fps\n");
					break;
	}
	// Change the frame rate via changing the vertical blanking
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = u32TotalSizeV-9;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
        gu16FullLines = u32TotalSizeV;
	}
	u32Temp = u32TotalSizeV*4;
	sensor_write_register(0x3030, u32Temp&0xFF);
	sensor_write_register(0x3031, (u32Temp>>8)&0xFF);
	sensor_write_register(0x3032, (u32Temp>>16)&0x0F);
	XM_MPI_MIPI_RefreshFV(0, u32TotalSizeV*2);
	return;
}

static XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    static XM_U16 preU16FullLine = 0xffff;
    XM_U32 u32Temp;
    if(preU16FullLine == u16FullLines)
        return;

    preU16FullLine = u16FullLines;
    u16FullLines = (u16FullLines>=4096)?4000:u16FullLines;
    gu16FullLines = u16FullLines;
    pstAeSnsDft->u32MaxIntTime = u16FullLines - 9;
    SysDelay_ms(30);
    if(gu32ShutNow>pstAeSnsDft->u32MaxIntTime)
    {
        cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
    }
    u32Temp = u16FullLines*4;
    sensor_write_register(0x3030, u32Temp&0xFF);
	sensor_write_register(0x3031, (u32Temp>>8)&0xFF);
	sensor_write_register(0x3032, (u32Temp>>16)&0x0F);
       
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)(u16FullLines*2));
}


/****************************************************************************
* AWB
****************************************************************************/
static const ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,//0.25
	{
		0x000,329, -78, 5,
		0x000,-43, 356, -57,
		0x000,39, -77, 294
	},
	4000,//0.23
	{
		0x000,333, -90,13,
		0x000,-58, 350,-36,
		0x000, 43,-89,302
	},
	2800,//0.2
	{
		0x000, 269, -2,-11,
		0x000,-73, 363,-34,
		0x000, 31,-142,367
	}
};

static const ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
	{0, 0, 4096, 3396, 4096, 2899, 1545, 4096},
	{4096, 4096, 0, 0, 2811, 3034, 4096, 1013},
	{-585, -3603, -1457, -4096, -3052, -4096, -2164, -2237},
	213,
	0, 1254, 0, 2367,
	{0, 416, 647, 983, 1085, 1199, 1462, 1491, 2367, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1824, 1024, 2017, 0}
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


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
XM_S32 cmos_init_sensor_exp_function_IMX335(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	//    XM_MPI_ISP_Memset((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;//cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;//cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_IMX335(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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

XM_S32 cmos_init_awb_exp_function_IMX335(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}









