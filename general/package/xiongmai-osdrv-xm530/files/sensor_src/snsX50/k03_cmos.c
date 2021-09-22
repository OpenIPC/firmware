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


#define HD5MP_25P_LINES		(1980)


extern const ISP_CMOS_GAMMA_S gstIspGamma; 

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
	{0x42,0x42,0x42,0x40,0x3e,0x32,0x30,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x30,0x2e,0x2a,0x2a,0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26,0x26}
};
static const XM_U8 gau8AgcTabXvi_K03[5][8] =
{
	{0x18,0x18,0x18,0x18,0x18,0x14,0x0E,0x10},	/* sharpen_D H	*/
	{0x1C,0x1C,0x1C,0x1C,0x1C,0x18,0x10,0x10},	/* sharpen_Ud M */
	{0x40,0x3e,0x3C,0x38,0x34,0x20,0x08,0x08},	/* sharpen_Kd  */
	{0x00,0x01,0x03,0x05,0x08,0x0E,0x14,0x20},	/* snr_thresh 2DNr		*/
	{0xD8,0xD4,0xD0,0xC8,0xC0,0xB0,0x80,0x78}/* saturation_strength */
};
static const XM_U8 gau8AgcTabCvbs_K03[5][8] =
{
	{0x14,0x13,0x12,0x12,0x12,0x12,0x12,0x12},	/* sharpen_D H	*/
	{0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12},	/* sharpen_Ud M */
	{0x30,0x2a,0x26,0x24,0x24,0x14,0x04,0x04},	/* sharpen_Kd  */
	{0x00,0x01,0x03,0x05,0x08,0x0E,0x10,0x20},	/* snr_thresh 2DNr		*/
    {0xc0,0xbc,0xb8,0xb0,0xa0,0x88,0x78,0x78}/* saturation_strength */
};
//YCNr
static const XM_U8 gau8YcNr_K03[9][8] = {
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
/* 4M */
static const XM_U8 gsau8Saturation[8] = 
{0xC4,0xC0,0xB6,0xAC,0x94,0x74,0x48,0x10};

//u8Mode:  0:Normal  0x10:XVI
//u8RlstMode: 0:1M 1:2M 3:3M 4:4M 5:5M
XM_S32 cmos_set_isp_para_K03(XM_U8 u8RlstMode,XM_U8 u8Mode)
{
	XM_S32 s32Ret;
	ISP_SHARPEN_ATTR_S stSharpenAttr;
	ISP_2DNR_ATTR_S stNRAttr;
	ISP_SATURATION_ATTR_S stSatAttr;
	XM_U8 u8i;
	//const XM_U8 (*au8YcNr)[8];
	const XM_U8 (*au8AgcPar)[8];
	s32Ret = XM_MPI_ISP_GetSaturationAttr(&stSatAttr);
    if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_GetSaturationAttr failed!\n");
		return XM_FAILURE;
	}
    #if 0
    ISP_CSC_ATTR_S stCscAttr;
	s32Ret = XM_MPI_ISP_GetCSCAttr(&stCscAttr);
	if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_SetCSCAttr failed!\n");
		return XM_FAILURE;
	}
    s32Ret = XM_MPI_ISP_GetYcNRAttr(&stYcNrAttr);
    if(s32Ret != XM_SUCCESS)
    {
        ERR("XM_MPI_ISP_GetYcNRAttr failed!\n");
        return XM_FAILURE;
    }
	#endif
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
    switch(u8Mode)
    {
        case 0x10:	// XVI
            stSharpenAttr.bEnable = XM_TRUE;
            //stYcNrAttr.bEnable = XM_TRUE;
    	    //au8YcNr = gau8YcNr_K03;
    		au8AgcPar = gau8AgcTabXvi_K03;
    		break;
        case 0x03:///CVBS
            stSharpenAttr.bEnable = XM_TRUE;
            //stYcNrAttr.bEnable = XM_TRUE;
            //au8YcNr = gau8YcNr_K03;
            au8AgcPar = gau8AgcTabCvbs_K03;
            break;
        default:// AHD/CVI/TVI	
    		stSharpenAttr.bEnable = XM_TRUE;
            //stYcNrAttr.bEnable = XM_TRUE;
            //au8YcNr = gau8YcNr_K03;
            au8AgcPar = gau8AgcTabXvi_K03;
            for(u8i=0;u8i<8;u8i ++)
            {
                stSharpenAttr.stAuto.au8SharpenD[u8i] = g_stIspAgcTable.au8SharpenAltD[u8i];
                stSharpenAttr.stAuto.au8SharpenUd[u8i] = g_stIspAgcTable.au8SharpenAltUd[u8i];
                stSharpenAttr.stAuto.au8SharpenKd[u8i] = g_stIspAgcTable.au8SharpenKd[u8i];
                stNRAttr.stAuto.au8Thresh[u8i] = g_stIspAgcTable.au8SnrThresh[u8i];
                stSatAttr.stAuto.au8Sat[u8i] = g_stIspAgcTable.au8Saturation[u8i];
            }
            break;
    }
	for(u8i=0;u8i<8;u8i ++)
    {
        if((u8Mode == 0x10)||(u8Mode == 0x03))
        {
            stSharpenAttr.stAuto.au8SharpenD[u8i] = au8AgcPar[0][u8i];
            stSharpenAttr.stAuto.au8SharpenUd[u8i] = au8AgcPar[1][u8i];
            stSharpenAttr.stAuto.au8SharpenKd[u8i] = au8AgcPar[2][u8i];
            stNRAttr.stAuto.au8Thresh[u8i] = au8AgcPar[3][u8i];
            stSatAttr.stAuto.au8Sat[u8i] = au8AgcPar[4][u8i];
        }
        if(u8RlstMode == 4)//4M输出的饱和度不一样
	    {
		    stSatAttr.stAuto.au8Sat[u8i] = gsau8Saturation[u8i];
	    }
		#if 0
        // YCNr
        stYcNrAttr.stAuto.au8SmpMode[u8i] = au8YcNr[0][u8i];
        stYcNrAttr.stAuto.au8NrKm1[u8i] = au8YcNr[1][u8i];
        stYcNrAttr.stAuto.au8NrKh1[u8i] = au8YcNr[2][u8i];
        stYcNrAttr.stAuto.au8NrKm2[u8i] = au8YcNr[3][u8i];
        stYcNrAttr.stAuto.au8NrKh2[u8i] = au8YcNr[4][u8i];
        stYcNrAttr.stAuto.au8SharpKm1[u8i] = au8YcNr[5][u8i];
        stYcNrAttr.stAuto.au8SharpKh1[u8i] = au8YcNr[6][u8i];
        stYcNrAttr.stAuto.au8SharpKm2[u8i] = au8YcNr[7][u8i];
        stYcNrAttr.stAuto.au8SharpKh2[u8i] = au8YcNr[8][u8i]; 
		#endif
    }
	for(; u8i<ISP_AUTO_STENGTH_NUM; u8i++)
	{
		stSharpenAttr.stAuto.au8SharpenD[u8i] = stSharpenAttr.stAuto.au8SharpenD[7];
		stSharpenAttr.stAuto.au8SharpenUd[u8i] = stSharpenAttr.stAuto.au8SharpenUd[7];
		stSharpenAttr.stAuto.au8SharpenKd[u8i] = stSharpenAttr.stAuto.au8SharpenKd[7];
		stNRAttr.stAuto.au8Thresh[u8i] = stNRAttr.stAuto.au8Thresh[7];
        stSatAttr.stAuto.au8Sat[u8i] = stSatAttr.stAuto.au8Sat[7];
		#if 0
		//YCNr
		stYcNrAttr.stAuto.au8SmpMode[u8i] = stYcNrAttr.stAuto.au8SmpMode[7];
		stYcNrAttr.stAuto.au8NrKm1[u8i] = stYcNrAttr.stAuto.au8NrKm1[7];
		stYcNrAttr.stAuto.au8NrKh1[u8i] = stYcNrAttr.stAuto.au8NrKh1[7];
		stYcNrAttr.stAuto.au8NrKm2[u8i] = stYcNrAttr.stAuto.au8NrKm2[7];
		stYcNrAttr.stAuto.au8NrKh2[u8i] = stYcNrAttr.stAuto.au8NrKh2[7];
		stYcNrAttr.stAuto.au8SharpKm1[u8i] = stYcNrAttr.stAuto.au8SharpKm1[7];
		stYcNrAttr.stAuto.au8SharpKh1[u8i] = stYcNrAttr.stAuto.au8SharpKh1[7];
		stYcNrAttr.stAuto.au8SharpKm2[u8i] = stYcNrAttr.stAuto.au8SharpKm2[7];
		stYcNrAttr.stAuto.au8SharpKh2[u8i] = stYcNrAttr.stAuto.au8SharpKh2[7];
		#endif
	}
	s32Ret = XM_MPI_ISP_SetSaturationAttr(&stSatAttr);
    if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_SetSaturationAttr failed!\n");
        return XM_FAILURE;
	}
    #if 0
	s32Ret = XM_MPI_ISP_SetCSCAttr(&stCscAttr);
	if(s32Ret != XM_SUCCESS)
	{
		ERR("XM_MPI_ISP_SetCSCAttr failed!\n");
		return XM_FAILURE;
	}
    s32Ret = XM_MPI_ISP_SetYcNRAttr(&stYcNrAttr);
    if(s32Ret != XM_SUCCESS)
    {
        ERR("XM_MPI_ISP_SetYcNRAttr failed!\n");
        return XM_FAILURE;
    }
	#endif
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

	u32InTimes = u32InTimes/64;
	u32InTimes = u32InTimes < 16? 16: u32InTimes;

	for(i = 0; i < 5; i++)
	{
		if(u32InTimes < 32)
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

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-2;
	pstAeSnsDft->u32MinIntTime = 2;    

	pstAeSnsDft->u32MaxAgain = 15872;//31744 15872
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

	sensor_write_register(0x23, (u16FullLines&0xff00)>>8);	
	sensor_write_register(0x22, u16FullLines&0xff);
	pstAeSnsDft->u32MaxIntTime = u16FullLines - 4;
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}


static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	sensor_write_register(0x01, u32IntTime&0xFF);
	sensor_write_register(0x02, (u32IntTime>>8)&0xFF);
	return;
}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
    static XM_U32 Preu32Again = 0xffff;
    if(Preu32Again == u32Again)
        return;
    Preu32Again = u32Again;
	sensor_write_register(0x00, u32Again);
	if(u32Again >= 0x20)
        sensor_write_register(0x8C, 0x80);
    else
        sensor_write_register(0x8C, 0x00);
        
	return;
}



static const XM_U8 gau8FpsSet[][6]= {
	//Reg  HD   SD_P  SD_N  4M_HD  4M_15Fps
	{0x12,0x40,0x42, 0x42,  0x40,    0x40},
	{0x0D,0x53,0x53, 0x53,  0x53,    0xA3},
	{0x0E,0x11,0x12, 0x12,  0x10,    0x11},
	{0x10,0x42,0x50, 0x50,  0x1E,    0x1E},
	{0x60,0x12,0x10, 0x10,  0x12,    0x12},
	{0x24,0x80,0x80, 0x80,  0x82,    0x82},
	{0x25,0x98,0xB4, 0xCA,  0xA8,    0xA8},
	{0x26,0x72,0x22, 0x22,  0x52,    0x52},
	{0x27,0x73,0x80, 0x01,  0x51,    0xC7},
	{0x28,0x1D,0x13, 0x13,  0x29,    0x23},
	{0x29,0x02,0x02, 0x02,  0x02,    0x01},
	{0x2A,0x5E,0x5E, 0xDE,  0x31,    0xB2},
	{0x2B,0x22,0x22, 0x21,  0x22,    0x21},
	{0x2C,0x0E,0x01, 0x01,  0x04,    0x11},
	{0x2D,0x01,0x46, 0x3F,  0x3B,    0x38},
	{0x2E,0xEE,0xAF, 0xBF,  0xB0,    0xAE},
	{0x30,0xA4,0xA4, 0xBB,  0xA4,    0x87},
	{0x32,0x0C,0x0C, 0x0C,  0x0C,    0x06},
	{0x33,0x0C,0x0C, 0x0C,  0x0C,    0x09},
	{0x34,0x13,0x13, 0x13,  0x13,    0x1C},
	{0x3D,0x00,0x00, 0x00,  0x00,    0x08},
	{0x3E,0xB8,0xB8, 0xB8,  0xB8,    0xA0},
	{0x5D,0x2D,0x2D, 0x2D,  0x2D,    0x2F},
	{0x70,0x69,0x69, 0x69,  0x69,    0x25},
	{0x71,0x4E,0x4E, 0x4E,  0x4E,    0x26},
	{0x73,0xA4,0xA4, 0xA4,  0xA4,    0x91},
	{0x74,0x86,0x46, 0x46,  0x86,    0x86},
	{0x75,0x02,0x02, 0x02,  0x02,    0x04},
	{0x77,0x0A,0x05, 0x05,  0x0A,    0x0A},
	{0x78,0x94,0x94, 0x94,  0x94,    0x88}, 
	{0x7A,0x32,0x32, 0x32,  0x32,    0x43},
	{0x7D,0x81,0x81, 0x81,  0x81,    0x42},
	{0x7E,0x0C,0x0C, 0x0C,  0x0C,    0x08},
	{0x87,0xB7,0xB7, 0xA6,  0xB7,    0xB7},
	{0x88,0x0F,0x0F, 0x0E,  0x0F,    0x0F},
	{0x92,0x64,0x64, 0x64,  0x64,    0x7F}	
};

/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U8 u8i,u8Num,u8Tmp,u8Idx = 1;
	XM_U8 u8Reg0x20 = 0xE8;
	XM_U32 u32TotalSizeV,u32ExpNow,u32Tmp;
	XM_U32 u32Reg2D_2E =  0;

	u32TotalSizeV = HD5MP_25P_LINES;
	u32Tmp = sensor_read_register(0x02);	
	u32ExpNow = (u32Tmp<<8) | sensor_read_register(0x01);
	switch(u8Fps)
	{
		// CVBS_PAL
		case 50:
			u8Idx = 2;
			u8Reg0x20 = 0xC0;
			u32TotalSizeV = 750;
			break;
		// CVBS_NTSC
		case 60:
			// 1560*770*59.94
			u8Idx = 3;
			u8Reg0x20 = 0x0C;
			u32TotalSizeV = 770;
			break;
		// 4M_25Fps
		case 25:
			u8Idx = 4;
			u8Reg0x20 = 0xC0;
			u32TotalSizeV = 1500;
			DEBUG("4M 25fps\n");
			break;
		// 4M_30Fps
		case 30:
			u8Idx = 4;
			u8Reg0x20 = 0x20;
			u32TotalSizeV = 1500;
			u32Reg2D_2E = 0x3CB1;
			DEBUG("4M 30fps\n");
			break;
	#if 0
		// 4M_15Fps
		case 15:
			u8Idx = 5;
			u8Reg0x20 = 0x20;
			u32TotalSizeV = 1500;
			break;
	#else
		// 5M_15Fps
		case 15:
			// 15fps
			u8Fps = 15;
			u8Idx = 1;
			u8Reg0x20 = 0xE8;
			u32TotalSizeV = HD5MP_25P_LINES*20/u8Fps;
			DEBUG("5M 15fps\n");
			break;
	#endif
		// 5M_10Fps
		case 10:
			// 10fps
			u8Fps = 10;
			u8Idx = 1;
			u8Reg0x20 = 0xE8;
			u32TotalSizeV = HD5MP_25P_LINES*20/u8Fps;
			DEBUG("5M 10fps\n");
			break;
		// 5M_20Fps
		case 20:
		default:
			// 20fps
			u8Fps = 20;
			u8Idx = 1;
			u8Reg0x20 = 0xE8;
			break;
	}		
	// Change the frame rate via changing the vertical blanking
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-2;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}

	if(u32ExpNow>pstAeSnsDft->u32MaxIntTime)
	{
		cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
	}
	u8Tmp = sensor_read_register(0x12);
	sensor_write_register(0x12, u8Tmp|0x40);
	u8Num = sizeof(gau8FpsSet)/sizeof(gau8FpsSet[0]);
	for(u8i=0; u8i < u8Num; u8i ++)
	{
		sensor_write_register((XM_U32)gau8FpsSet[u8i][0], (XM_U32)gau8FpsSet[u8i][u8Idx]);
	}
	sensor_write_register(0x20, (XM_U32)u8Reg0x20);
	sensor_write_register(0x22, u32TotalSizeV&0xFF);
	sensor_write_register(0x23, (u32TotalSizeV>>8)&0xFF);
	if(u32Reg2D_2E)
	{
		sensor_write_register(0x2D, (u32Reg2D_2E>>8)&0xFF);
		sensor_write_register(0x2E, u32Reg2D_2E&0xFF);
	}
	u8Tmp = (u8Tmp&0x30) + sensor_read_register(0x12);//防止Mirror和Flip标志位被清楚
	sensor_write_register(0x12, u8Tmp&0xbf);
	return;
}


/****************************************************************************
* AWB
****************************************************************************/
#if 0
static ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,//0.14962
	{
		0x0000, 404,  -112,  -36,
		0x0000, -70,  444,	 -118,
		0x0000, 20,  -173,	 409
	},
    4000,//0.14962
    {
        0x0000, 393, -102,  -35,
        0x0000, -112, 439, -71,
        0x0000, -39, -160, 455
    },
    2800,//0.11837
    {
        0x0000,  307, 16, -67,
        0x0000, -112, 457,   -89,
        0x0000, -111, -347, 714
    }
};
#else
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
/*
static ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,//0.14392
	{
		0x0000, 258,  16,  -18,
		0x0000, -71,  349, -22,
		0x0000, 10,   -21,  267
	},
	4000,//0.14392
	{
		0x0000, 255, 22,  -21,
		0x0000, -48, 347, -43,
		0x0000, -4, -12,  272
	},
	2800,//0.10486
	{
		0x0000,  225, 77,   -46,
		0x0000, -77,  394,  -61,
		0x0000, -76,  -184, 516
	}
};
*/
#endif
#if 0
const static ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
	{0, 0, 4096, 3378, 3531, 2253, 1758, 4096},
	{4096, 3890, 0, 0, 4096, 2614, 4096, 1259},
	{-1133, -4096, -1344, -4096, -3641, -4096, -2692, -2387},
	213,
	0, 1554, 0, 2742,
	{0, 1045, 1178, 1428, 1508, 1600, 1774, 1796, 2742, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1681, 1024, 1563, 0}
};
#else
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
#endif
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
XM_S32 cmos_init_sensor_exp_function_k03(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	//    XM_MPI_ISP_Memset((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
	pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_k03(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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

XM_S32 cmos_init_awb_exp_function_k03(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}








