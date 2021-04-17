#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"
#include "mpi_phyvi.h"
#include "xm_print.h"
#include "XAx_cmos.h"

#define STATIC	static
extern GAIN_MODE_DT gstGainModeDt;


STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U32 u32Tmp = sensor_read_register(0x3007);
	u32Tmp &= 0xF0;
	if(u8Mirror)	u32Tmp |= 0x02;
	if(u8Flip)		u32Tmp |= 0x01;
	sensor_write_register(0x3007, u32Tmp);

	return XM_SUCCESS;
}




 static XM_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
 {
	 if (XM_NULL == pstAeSnsDft)
	 {
		 ERR("null pointer when get ae default value!\n");
		 return -1;
	 }
	 pstAeSnsDft->u8UpdateMode = 1;
	 pstAeSnsDft->u32FullLinesStd = HD1080P_LINES;
	 pstAeSnsDft->u8AeCompensation = 0x39;	 
	 pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	 pstAeSnsDft->u32FlickerFreq = 0;
 
	 pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-2;
	 pstAeSnsDft->u32MinIntTime = 2;	
 
	 pstAeSnsDft->u32MaxAgain = 62564;//31744 15872
	 pstAeSnsDft->u32MinAgain = 1024;
 
	 pstAeSnsDft->u32MaxDgain = 1024;  
	 pstAeSnsDft->u32MinDgain = 1024;
 
	 pstAeSnsDft->u32ISPDgainShift = 8;
	 pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
	 pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;
 
	 pstAeSnsDft->bDelayCfg = XM_FALSE;
	 return 0;
 }

static const XM_U16 gau16ShutCal[35][4] = {
 //Exp Shut Gain GainIdx
 {1623,2,1024,0},
 {1663,2,1059,1},
 {1706,2,1097,2},
 {1773,2,1135,3},
 {1822,2,1175,4},
 {1861,2,1217,5},
 {1925,2,1259,6},
 {1985,2,1304,7},
 {2042,2,1349,8},
 {2089,2,1397,9},
 {2169,2,1446,10},
 {2246,2,1497,11},
 {2302,2,1549,12},
 {2351,2,1604,13},
 {2407,2,1660,14},
 {2483,2,1719,15},
 {2578,2,1779,16},
 {2671,2,1842,17},
 {2717,2,1906,18},
 {2784,2,1973,19},
 {2929,2,2043,20},
 {2945,3,1024,0},
 {3044,3,1059,1},
 {3100,3,1097,2},
 {3130,3,1135,3},
 {3208,3,1175,4},
 {3398,3,1217,5},
 {3501,3,1259,6},
 {3498,3,1304,7},
 {3655,3,1349,8},
 {3796,3,1397,9},
 {3908,3,1446,10},
 {3976,3,1497,11},
 {4056,3,1549,12},
 {4096,4,1024,0},
};
static XM_U8 su8Idx = 0xFF;
STATIC XM_VOID cmos_shut_calc_table(XM_S32 s32IntTime,AE_SENSOR_SHUTINFO_S *pstAeSnsShutInfo)
{
	int i,s32Idx = 0;
	if(s32IntTime < 4)
	{
		if(pstAeSnsShutInfo->u64Exp <= gau16ShutCal[0][0])
		{
			s32Idx = 0;
		}
		else if(pstAeSnsShutInfo->u64Exp >= gau16ShutCal[34][0])		
		{			 
			s32Idx = 34;
		}
		else
		{
			for(i = 1; i <= 34; i++)		 
			{			
				if(pstAeSnsShutInfo->u64Exp < gau16ShutCal[i][0]) 		   
				{  
					s32Idx = i-1;
					break;			  
				}			 
			}	 
		}
		
		pstAeSnsShutInfo->u32ShutDb = gau16ShutCal[s32Idx][1];
		su8Idx = s32Idx;
	}
	else			// Normal
	{
		su8Idx = 0xFF;
	}
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

STATIC XM_VOID cmos_gain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;

	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
 
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	if(su8Idx == 0xFF)
	{
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
	}
	else
	{
		pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
		pstAeSnsGainInfo->u32GainDb = gau16ShutCal[su8Idx][3];
	}
	return;
}

STATIC XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;

	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
 
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	if(su8Idx == 0xFF)
	{
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
	}
	else
	{
		pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
		pstAeSnsGainInfo->u32GainDb = 0;
	}
	return;
}

STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32Time = 0xFFFF;
	gu32ShutNow = u32IntTime;
	u32IntTime = gu16FullLines - u32IntTime;
	if(su32Time == u32IntTime)		return ;
	su32Time = u32IntTime;
#if 0
	sensor_write_register(0x3020,u32IntTime & 0xFF);
	sensor_write_register(0x3021,(u32IntTime & 0xFF00) >> 8);
	sensor_write_register(0x3022,(u32IntTime & 0x30000) >> 16);
#else
	gstIspI2CData[0].u32Data = u32IntTime & 0xFF;
	gstIspI2CData[1].u32Data = (u32IntTime & 0xFF00) >> 8;
#endif
	return;
}



STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	XM_U32 u32HCG = 0x02;	 
	XM_U32 u32Tmp;		  
	if(u32Again >= 21)	 //2048 
	{		 
		u32HCG = u32HCG | 0x10;  // bit[4] HCG	.Reg0x209[7:0]		  
		u32Again = u32Again - 21;	
	}	
	u32Tmp = u32Again + u32Dgain;		 
#if 0
	sensor_write_register(0x3014,u32Tmp & 0xFF);	
	sensor_write_register(0x3009,u32HCG & 0xFF);
#else
	gstIspI2CData[2].u32Data = u32Tmp & 0xFF;
	gstIspI2CData[3].u32Data = u32HCG & 0xFF;
#endif
	return;
}

/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    XM_U32 u32TotalSizeV;
	u32TotalSizeV = HD1080P_LINES;
	switch(u8Fps)
	{
		case 30:
			// Change the frame rate via changing the vertical blanking
			//u32LineLength = 1100;
			sensor_write_register(0x301c, 0x30);
	        sensor_write_register(0x301d, 0x11);// H_Total * 2 = 2200
			DEBUG("cmos_fps_set IMX307 2M 30fps\n");
			break;
		case 25:
		default:
			// Change the frame rate via changing the vertical blanking
			//u32LineLength = 1320;
			sensor_write_register(0x301c, 0xA0);
            sensor_write_register(0x301d, 0x14);// H_Total * 2 = 2640
			DEBUG("cmos_fps_set IMX307 2M 25fps\n");
			break;
	}
	if(pstAeSnsDft != NULL) 
	{
	    pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
	    pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd - gu8MaxShutterOfst;
	    pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
        gu16FullLines = u32TotalSizeV;
    }
	return;
}

STATIC XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	static XM_U16 preU16FullLine = 0xffff;
	if(preU16FullLine == u16FullLines)
		return;
	gu16FullLines = u16FullLines;
	
	preU16FullLine = u16FullLines;
	u16FullLines = (u16FullLines>=4096)?4000:u16FullLines;
	pstAeSnsDft->u32MaxIntTime = u16FullLines - 4;
	SysDelay_ms(100);
	if(gu32ShutNow>pstAeSnsDft->u32MaxIntTime)
	{
		cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
	}
	sensor_write_register(0x3018, u16FullLines&0xff);
	sensor_write_register(0x3019, (u16FullLines&0xff00)>>8);		
	sensor_write_register(0x301A, (u16FullLines&0x30000)>>16);		
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}

static XM_S32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
	XM_S32 i;
	if(g_stSnsRegsInfo.u32RegNum != 4)	// First time
	{
		g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
		g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;		
		g_stSnsRegsInfo.u32RegNum = 4;
		g_stSnsRegsInfo.pstI2CData = gstIspI2CData;
		for (i = 0; i < g_stSnsRegsInfo.u32RegNum; i++)
		{	
			gstIspI2CData[i].bUpdate = XM_TRUE;
		}
		
		gstIspI2CData[0].u8DelayFrmNum = 0;		// Shut_L
		gstIspI2CData[0].u32RegAddr = 0x3020;
		gstIspI2CData[1].u8DelayFrmNum = 0;
		gstIspI2CData[1].u32RegAddr = 0x3021;	// Shut_H

		gstIspI2CData[2].u8DelayFrmNum = 0;	// Gain
		gstIspI2CData[2].u32RegAddr = 0x3014;
		
		gstIspI2CData[3].u8DelayFrmNum = 1;
		gstIspI2CData[3].u32RegAddr = 0x3009;	// HCG
	}
	else
	{
		for (i = 0; i < g_stSnsRegsInfo.u32RegNum; i++)        
		{
			if(gstIspI2CData[i].u32Data == gau32PreI2CData[i])            
			{                
				gstIspI2CData[i].bUpdate = XM_FALSE;
			}            
			else            
			{                
				gstIspI2CData[i].bUpdate = XM_TRUE;
				gau32PreI2CData[i] = gstIspI2CData[i].u32Data;
			}
		}
	}

	if (XM_NULL == pstSnsRegsInfo)
	{
		ERR("null pointer when get sns reg info!\n");
		return -1;
	}
	XM_MPI_ISP_Memncpy((XM_U8*)pstSnsRegsInfo, (XM_U8*)&g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S));
	return 0;
}

XM_S32 cmos_init_sensor_exp_function_imx307(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
#if 0
	pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
#endif
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_imx307(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_get = NULL;
	pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_gain_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
	pstExpFuncs->pfn_cmos_shut_calc_table = cmos_shut_calc_table;
	pstExpFuncs->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;
	pstExpFuncs->pfn_cmos_write_register = sensor_write_register;
	pstExpFuncs->pfn_cmos_read_register = sensor_read_register;
	return 0;
}

#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_imx307 =
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	//[0~7]:Normal						
	/* sharpen_D	H	*/
	{0x3C,0x3C,0x38,0x36,0x30,0x28,0x20,0x18,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* (2) sharpen_Ud M */
	{0x30,0x30,0x30,0x30,0x2A,0x24,0x1A,0x12,0x08,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	/* (3) sharpen_Kd  */
	{0x3C,0x3C,0x3A,0x38,0x34,0x30,0x28,0x18,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	/* (4) snr_thresh 2DNr		*/
	{0x00,0x01,0x02,0x03,0x04,0x06,0x0E,0x12,0x1C,0x40,0x40,0x40,0x40,0x40,0x40,0x40},
	/* (5) snr_thresh 3DNr	 Tf	*/
	{0x02,0x04,0x06,0x08,0x0C,0x10,0x14,0x18,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b},
	/* (6) snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x01,0x02,0x03,0x04,0x08,0x0A,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},   
	/* DyDpc_thresh 		*/
	{0x97,0xA8,0xD0,0xD4,0xD8,0xDC,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0},
	/* saturation_strength */
	{0xC8,0xC8,0xC8,0xBC,0xB4,0xa2,0x96,0x78,0x50,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
	/* Blc	*/
	{0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xB0,0xB0},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x30,0x30,0x30,0x28,0x28,0x28,0x28,0x28}
};


const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_imx307 =
{
	5082,//0.16
	{
		0x0000, 294, -56, 18,
		//0x0000, -9, 284, -19,
		0x0000, -15, 284, -9,
		//0x0000, 55, -43, 244,
		0x0000, 33, -43, 266,
	},
	4400,//0.16
	{
		0x0000, 298, -72, 30,
		0x0000, -10, 287, -21,
		0x0000, 61, -64, 259,
	},
	2800,//0.13
	{
		0x0000, 252, -8, 12,
		0x0000, -50, 308, -2,
		0x0000, 43, -96, 309,
	}
};


const ISP_AWB_CALIBRATION_V2_S gstAwbCal_imx307 = {
	{0, 0, 4096, 3117, 4096, 2739, 1659, 4096},
	{4096, 4096, 0, 0, 2729, 2697, 4096, 916},
	{-16, -3961, -976, -4096, -2344, -4096, -1605, -1643},
	213,
	0, 1173, 0, 2282,
	{0, 320, 589, 871, 994, 1181, 1348, 1379, 2282, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1985, 1024, 1956, 0}
};


// ExtClk: 37.125M
// 1080P30fps,12Bit,SPI,LVDS 4CH,ADC12Bit
// Nomal Mode register setting
static const XM_U16 gau16SnsInit_imx307[][2] = 
{
    // Enter Standby
    {0x3000, 0x01},     // Standby mode
    {0x3002, 0x01},  // Master mode stop
    // Initial register setting (need rewrite after reset)
    // Chip id:02
    {0x3011, 0x0a},
    {0x309e, 0x4a},
    {0x309f, 0x4a},
    //chip id:03    
    {0x3128, 0x04},
    {0x313b, 0x41},
    // Mode register setting
    {0x3005, 0x01},
    {0x3007, 0x00},
    {0x3009, 0x02},  // 30fps;0x00->120fps
    {0x3012, 0x64},
    {0x3018, 0x65},  // VMAX
    {0x3019, 0x04},
    {0x301c, 0x30},  // HMAX;1130H->30fps;14A0H->25fps;
    {0x301d, 0x11},  // HMAX;
    
    {0x3046,    0x01},  // LVDS 4CH;12Bit
    //0x3048    0x02    // XVS/XHS Output low width
    //0x3049    0x03
    {0x305c, 0x18},  //37.125MHz INCK Setting
    {0x305d, 0x03},
    {0x305e, 0x20},
    {0x305f, 0x01},
    {0x315e, 0x1a},  //37.125MHz INCK5 Setting
    {0x3164, 0x1a},
    {0x3480, 0x49},  //37.125MHz INCK7 Setting
    
    {0x3129, 0x00},  // ADBIT1,12Bit;
    {0x317c, 0x00},  // ADBIT2,12Bit;
    {0x31ec, 0x0e},  // ADBIT3,12Bit;
    //CHIP ID 06 For MIPI I/F
    {0x3405, 0x20},
    {0x3407, 0x03},
    {0x3414, 0x0a},
    {0x3418, 0x49},
    {0x3419, 0x04},  
    {0x3441, 0x0c},
    {0x3442, 0x0c},
    {0x3443, 0x03},    
    {0x3444, 0x20},
    {0x3445, 0x25},
    
    {0x3446, 0x47},
    {0x3447, 0x00},
    {0x3448, 0x1f},
    {0x3449, 0x00},
    {0x344A, 0x17},
    {0x344B, 0x00},
    {0x344C, 0x0F},
    {0x344D, 0x00},
    {0x344E, 0x17},
    {0x344F, 0x00},
    {0x3450, 0x47},
    {0x3451, 0x00},
    {0x3452, 0x0F},
    {0x3453, 0x00},
    {0x3454, 0x0f},
    {0x3455, 0x00},
    {0x3472, 0x9C},
    {0x3473, 0x07},
    // Standby Cancel
    {0x3000, 0x00},  // standby
    //wait(20)       // DELAY20mS
    {0x3002, 0x00},  // master mode start
    {0x304B, 0x0a}  // XVSOUTSEL XHSOUTSEL enable output
    //wait(20)

};



XM_U32 sensor_getlist_imx307(XM_U16 *pu16Num)
{
    DEBUG("------------- IMX307 2M 30fps  init ok! (@201812111)----------------\n");
	*pu16Num = sizeof(gau16SnsInit_imx307)/sizeof(gau16SnsInit_imx307[0]);
	return (XM_U32)gau16SnsInit_imx307;
}

#endif

