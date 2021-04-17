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

#ifdef SOC_NONE
#include "xm540_tmp.h"
#else
#include "xm510_tmp.h"
#endif
#include "mpi_phyvi.h"


#define HD8M_25P_LINES		(2250)


extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID snsMemSet(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);
//extern XM_U32 gau32AllGain;
static XM_U32 gu32AGain = 64;	// *64
static XM_U32 gu32DGain = 128;	// *128
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
	{0x01,0x03,0x06,0x0C,0x1C,0x28,0x40,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48},
	/* snr_thresh 3DNr	 Tf	*/
	{0x11,0x12,0x13,0x15,0x18,0x1A,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}, 
	/* snr_thresh 3DNr	 Sf	*/
	{0x10,0x10,0x18,0x20,0x28,0x2A,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, 
	/* DyDpc_thresh 		*/
	{0xB8,0xC0,0xD0,0xD4,0xD8,0xDC,0xE0,0xE0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0},   
	/* saturation_strength */
	{0x98,0x98,0x94,0x80,0x58,0x50,0x38,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
	/* Blc	*/
	//{0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111,0x111},
	{0x108,0x108,0x108,0x100,0x0F0,0x0E8,0x0E8,0x0E8,0x0E8,0x0E8,0x0E8,0x0E8,0x0E8,0x0E8,0x0E8,0x0E8},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x30,0x2e,0x2a,0x2a,0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26}
};
static const XM_U8 gau8AgcTabXvi_apollo[5][8] =
{
	{0x28,0x28,0x28,0x20,0x18,0x14,0x0E,0x10},	/* sharpen_D H	*/
	{0x1C,0x1C,0x1C,0x1C,0x1C,0x18,0x10,0x10},	/* sharpen_Ud M */
	{0x40,0x3e,0x3C,0x38,0x34,0x20,0x08,0x08},	/* sharpen_Kd  */
	{0x01,0x03,0x03,0x05,0x08,0x0E,0x14,0x20},	/* snr_thresh 2DNr		*/
	{0x98,0x98,0x94,0x80,0x58,0x50,0x38,0x30}/* saturation_strength */
};
static const XM_U8 gau8AgcTabCvbs_apollo[5][8] =
{
	{0x14,0x13,0x12,0x12,0x12,0x12,0x12,0x12},	/* sharpen_D H	*/
	{0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12},	/* sharpen_Ud M */
	{0x30,0x2a,0x26,0x24,0x24,0x14,0x04,0x04},	/* sharpen_Kd  */
	{0x01,0x01,0x03,0x05,0x08,0x0E,0x10,0x20},	/* snr_thresh 2DNr		*/
    {0xc0,0xbc,0xb8,0xb0,0xa0,0x88,0x78,0x78}/* saturation_strength */
};
//YCNr
static const XM_U8 gau8YcNrXvi_apollo[9][8] = {
	{0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02},
	{0x1C,0x1C,0x04,0x00,0x00,0x00,0x00,0x00},	// NR_Km1
	{0x00,0x02,0x02,0x00,0x00,0x00,0x00,0x00},	// NR_Kh1
	{0x01,0x03,0x02,0x00,0x00,0x00,0x00,0x00},	// NR_Km2
	{0x01,0x03,0x02,0x00,0x00,0x00,0x00,0x00},	// NR_Kh2
	{0x28,0x1C,0x10,0x10,0x10,0x10,0x10,0x10},	// SP_Km1
	{0x18,0x14,0x10,0x10,0x10,0x10,0x10,0x10},	// SP_Kh1
	{0x18,0x14,0x10,0x10,0x10,0x10,0x10,0x10},	// SP_Km2
	{0x18,0x14,0x10,0x10,0x10,0x10,0x10,0x10},	// SP_Kh2
};

//YCNr
static const XM_U8 gau8YcNr_apollo[9][8] = {
	{0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02},
	{0x1C,0x1C,0x1C,0x1C,0x1C,0x20,0x30,0x90},	// NR_Km1
	{0x00,0x02,0x04,0x08,0x10,0x20,0x30,0x90},	// NR_Kh1
	{0x01,0x03,0x06,0x10,0x18,0x18,0x18,0x18},	// NR_Km2
	{0x01,0x03,0x06,0x10,0x20,0x28,0x28,0x28},	// NR_Kh2
	{0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28},	// SP_Km1
	{0x18,0x18,0x18,0x18,0x18,0x14,0x10,0x10},	// SP_Kh1
	{0x18,0x18,0x18,0x18,0x18,0x18,0x10,0x10},	// SP_Km2
	{0x18,0x18,0x18,0x18,0x18,0x18,0x10,0x10},	// SP_Kh2
};


//u8Mode:  0:Normal  0x10:XVI
//u8RlstMode: 0:1M 1:2M 3:3M 4:4M 5:5M
XM_S32 cmos_set_isp_para_apollo(XM_U8 u8RlstMode,XM_U8 u8Mode)
{
	XM_S32 s32Ret;
	ISP_SHARPEN_ATTR_S stSharpenAttr;
	ISP_2DNR_ATTR_S stNRAttr;
	ISP_SATURATION_ATTR_S stSatAttr;
	XM_U8 u8i;
	const XM_U8 (*au8AgcPar)[8];
	s32Ret = XM_MPI_ISP_GetSaturationAttr(&stSatAttr);
    if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_GetSaturationAttr failed!\n");
		return XM_FAILURE;
	}
	s32Ret = XM_MPI_ISP_GetSharpenAttr(&stSharpenAttr);
	if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_GetSharpenAttr failed!\n");
		return XM_FAILURE;
	}
	s32Ret = XM_MPI_ISP_GetNRAttr(&stNRAttr);
	if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_GetNRAttr failed!\n");
		return XM_FAILURE;
	}
	au8AgcPar = NULL;
    switch(u8Mode)
    {
        case 0x10:	// XVI
            stSharpenAttr.bEnable = XM_TRUE;
    		au8AgcPar = gau8AgcTabXvi_apollo;
    		break;
        case 0x03:///CVBS
            stSharpenAttr.bEnable = XM_TRUE;
            au8AgcPar = gau8AgcTabCvbs_apollo;
            break;
        default:// AHD/CVI/TVI	
    		stSharpenAttr.bEnable = XM_TRUE;
            for(u8i=0;u8i<8;u8i ++)
            {
                stSharpenAttr.stAuto.au8SharpenD[u8i] = g_stIspAgcTable.au8SharpenAltD[u8i];
                stSharpenAttr.stAuto.au8SharpenUd[u8i] = g_stIspAgcTable.au8SharpenAltUd[u8i];
                stSharpenAttr.stAuto.au8SharpenKd[u8i] = g_stIspAgcTable.au8SharpenKd[u8i];
            }
            break;
    }
	if(au8AgcPar != NULL)
	{
		for(u8i=0;u8i<8;u8i ++)
		{
			stSharpenAttr.stAuto.au8SharpenD[u8i] = au8AgcPar[0][u8i];
			stSharpenAttr.stAuto.au8SharpenUd[u8i] = au8AgcPar[1][u8i];
			stSharpenAttr.stAuto.au8SharpenKd[u8i] = au8AgcPar[2][u8i];
			stNRAttr.stAuto.au8Thresh[u8i] = au8AgcPar[3][u8i];
			stSatAttr.stAuto.au8Sat[u8i] = au8AgcPar[4][u8i];
		}
	}

	for(; u8i<ISP_AUTO_STENGTH_NUM; u8i++)
	{
		stSharpenAttr.stAuto.au8SharpenD[u8i] = stSharpenAttr.stAuto.au8SharpenD[7];
		stSharpenAttr.stAuto.au8SharpenUd[u8i] = stSharpenAttr.stAuto.au8SharpenUd[7];
		stSharpenAttr.stAuto.au8SharpenKd[u8i] = stSharpenAttr.stAuto.au8SharpenKd[7];
		stNRAttr.stAuto.au8Thresh[u8i] = stNRAttr.stAuto.au8Thresh[7];
        stSatAttr.stAuto.au8Sat[u8i] = stSatAttr.stAuto.au8Sat[7];
	}
	s32Ret = XM_MPI_ISP_SetSaturationAttr(&stSatAttr);
    if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_SetSaturationAttr failed!\n");
        return XM_FAILURE;
	}
	s32Ret = XM_MPI_ISP_SetSharpenAttr(&stSharpenAttr);
	if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_SetSharpenAttr failed!\n");
		return XM_FAILURE;
	}
	s32Ret = XM_MPI_ISP_SetNRAttr(&stNRAttr);
	if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_SetNRAttr failed!\n");
		return XM_FAILURE;
	}
	
	return XM_SUCCESS;
}


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
		u32Val |= 0x06;;
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
		u16OfstV = 0;
	}
	sensor_write_register(0x3221, u32Val);
	VI_WinSet(1, u8Mirror, u8Flip, 0, u16OfstV);

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

	u32InTimes = u32InTimes < 64? 64: u32InTimes;
	for(i = 0; i < 5; i++)
	{
		if(u32InTimes < 128)
		{
			break;
		}
		u32InTimes >>= 1;
		pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<1)|1;
	}
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8)|u32InTimes; 
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes<<i;
	gu32AGain = pstAeSnsGainInfo->u32SnsTimes;
	return;
}



static XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 128;

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
	gu32DGain = pstAeSnsGainInfo->u32SnsTimes;
	return;
}



static XM_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	if (XM_NULL == pstAeSnsDft)
	{
		ERR("null pointer when get ae default value!\n");
		return -1;
	}
	pstAeSnsDft->u32FullLinesStd = HD8M_25P_LINES;
	pstAeSnsDft->u8AeCompensation = 0x39;	
	pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	pstAeSnsDft->u32FlickerFreq = 0;

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-2;
	pstAeSnsDft->u32MinIntTime = 2;    

	pstAeSnsDft->u32MaxAgain = 1016;//15.875
	pstAeSnsDft->u32MinAgain = 64;

	pstAeSnsDft->u32MaxDgain = 4032;  
	pstAeSnsDft->u32MinDgain = 128;

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
	return;
}


static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;

	u32IntTime = u32IntTime*2;
	sensor_write_register(0x3e00, (u32IntTime>>12)&0xFF);
	sensor_write_register(0x3e01, (u32IntTime>>4)&0xFF);
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;
}


static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	unsigned int tmp[2];
	
	//gau32AllGain = ((XM_U64)gu32AGain*(XM_U64)gu32DGain)>>8; 	// *32
	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;
		// 1. Logic
		//gainLogic_sc2235P(u32AllGain, 0);
		
		// 2.AGainUpdate
		tmp[1] = (u32Again>>8)&0x0F;
		tmp[1] = (tmp[1]<<2)|0x03;
		tmp[0] = u32Again&0xFF;
		sensor_write_register(0x3e08, tmp[1]);
		sensor_write_register(0x3e09, tmp[0]);

		// 3.DGainUpdate
		tmp[1] = (u32Dgain>>8)&0x0F;
		tmp[0] = u32Dgain&0xFF;
		sensor_write_register(0x3e06, tmp[1]);
		sensor_write_register(0x3e07, tmp[0]);
	}
	return;
}


/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32TotalSizeV,u32ExpNow,u32Tmp;
	u32TotalSizeV = HD8M_25P_LINES;
	
	u32Tmp = sensor_read_register(0x3e00)&0xFF;
	u32ExpNow = (u32Tmp<<12)|((sensor_read_register(0x3e01)&0xFF)<<4);
	u32ExpNow |= (sensor_read_register(0x3e02)>>4)&0x0F;
	u32ExpNow = u32ExpNow>>1;
	switch(u8Fps)
	{
		// 8M_15Fps
		case 15:
		default:
			u32TotalSizeV = HD8M_25P_LINES;
			break;
	}		
	// Change the frame rate via changing the vertical blanking
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}

	if(u32ExpNow>pstAeSnsDft->u32MaxIntTime)
	{
		cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
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
	5000,
	{
		0x0000, 429,  -186,  13,
		0x0000, -50,  444, -138,
		0x0000, -40,   -183,  479	// -8,   -183,  447
	},
	4000,
	{
		0x0000, 466, -249,  39,
		0x0000, -105, 407, -46,
		0x0000, -10, -188,  454
	},
	2800,
	{
		0x0000,  332, -65,   -11,
		0x0000, -116,  427,  -55,
		0x0000, -56,  -394,  706
	}
};

const static ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
	{0, 0, 4096, 2794, 3734, 2110, 1480, 4096},
	{4096, 3964, 0, 0, 4096, 2754, 4096, 1529},
	{-742, -4096, -1357, -4096, -3446, -4096, -2357, -2184},
	213,
	0, 1644, 0, 2772,
	{0, 778, 1079, 1403, 1505, 1653, 1857, 1863, 2772, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1662, 1024, 1639, 0}
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
XM_S32 cmos_init_sensor_exp_function_apollo(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	//    snsMemSet((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
	pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_apollo(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_get = NULL;
	pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;    
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
	return 0;
}

XM_S32 cmos_init_awb_exp_function_apollo(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}








