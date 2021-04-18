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
extern XM_U32 gu32ShutNow;
extern PROFILE gstRlstType;
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
	{0x44,0x44,0x44,0x44,0x44,0x44,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},
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

static XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
    XM_U32 u32Val;
    XM_U16 u16OfstV = 0;
    XM_U16 u16OfstH = 0;
    u32Val = sensor_read_register(0x3221);
    if(u8Mirror)
    {
        u32Val |= 0x06;
		//u16OfstH = 1;
    }
    else
    {
        u32Val &= ~0x06;
    }
        
    if(u8Flip)
    {
        u32Val |= 0x60;
        u16OfstV = 1;
    }
    else
    {
        u32Val &= ~0x60;
    }
    sensor_write_register(0x3221, u32Val);
    VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfstV);
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

	u32InTimes = u32InTimes/64;
	u32InTimes = u32InTimes < 16? 16: u32InTimes;//15.5
	for(i = 0; i < 4; i++)//0 1 2 3
	{
		if(u32InTimes < 32)// 1 2 4 8 16
		{
			break;
		}
		u32InTimes >>= 1;
	}
	pstAeSnsGainInfo->u32GainDb = (i<<4)+u32InTimes-16; 
	u32InTimes = u32InTimes<<i;
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
	pstAeSnsDft->u32FullLinesStd = HD5MP_25P_LINES;
	pstAeSnsDft->u8AeCompensation = 0x39;	
	pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	pstAeSnsDft->u32FlickerFreq = 0;

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
	pstAeSnsDft->u32MinIntTime = 2;    

	pstAeSnsDft->u32MaxAgain = 15872;// 15.5
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

static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;
	gu32ShutNow = u32IntTime;
	u32IntTime = u32IntTime*2;
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	sensor_write_register(0x3e01, ((u32IntTime&0x0ff0)>>4));
	sensor_write_register(0x3e00, ((u32IntTime&0xf000)>>12));
	return;
}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
    static XM_U32 Preu32Again = 0xffff;
	XM_U32 u32Tmp1 = 0;
	XM_U32 u32Tmp2 = 0;
    if(Preu32Again == u32Again)
        return;
    Preu32Again = u32Again;

    u32Tmp1 = (u32Again&0xf0)>>4;
	for(;u32Tmp1 > 0;u32Tmp1--)
    {
        u32Tmp2 = (u32Tmp2<<1)|0x01;
    }
    if(u32Again < 0x10 )//[gain<2]
    {
        sensor_write_register(0x3620,0x42);
        sensor_write_register(0x3036,0x20);
    }
    else//[gain>=2]
    {
        sensor_write_register(0x3620,0x43);
        sensor_write_register(0x3036,0x40);
    }
	sensor_write_register(0x3e08, ((u32Tmp2<<2)|0x03));
	sensor_write_register(0x3e09, ((u32Again&0x0f)+0x10));
	return;
}


/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32TotalSizeV;
	u32TotalSizeV = HD5MP_25P_LINES;
	switch(gstRlstType)
	{
		case P5M_:
		default:	u8Fps = 15;
					u32TotalSizeV = HD5MP_25P_LINES*20/u8Fps;
					DEBUG("cmos_fps_set 5M 15fps\n");
					break;
		case P1080_:
		case P4M_:	u8Fps = 20;
					u32TotalSizeV = HD5MP_25P_LINES;
					DEBUG("cmos_fps_set 5M 20fps\n");
					break;
	}
	// Change the frame rate via changing the vertical blanking
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u32TotalSizeV);
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
    SysDelay_ms(300);
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
	5000,//0.28
	{
		0x000,359, -110, 7,
		0x000,-51, 405, -98,
		0x000,28, -118, 346
	},
	4000,//0.25
	{
		0x000,356, -122,22,
		0x000,-61, 383,-66,
		0x000, 32,-136,360
	},
	2800,//0.2
	{
		0x000, 288, -29,-3,
		0x000,-75, 387,-56,
		0x000, 3,-228,481
	}
};
static const ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
	{0, 0, 4096, 2773, 4096, 2185, 1126, 4096},
	{4096, 4037, 0, 0, 3488, 2829, 4096, 1190},
	{-452, -4096, -1119, -4096, -3029, -4096, -1724, -2034},
	213,
	0, 1536, 0, 2601,
	{0, 626, 958, 1272, 1388, 1508, 1721, 1755, 2601, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1729, 1024, 1812, 0}
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
XM_S32 cmos_init_sensor_exp_function_sc335e(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	//    XM_MPI_ISP_Memset((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;//cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;//cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_sc335e(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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

XM_S32 cmos_init_awb_exp_function_sc335e(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}









