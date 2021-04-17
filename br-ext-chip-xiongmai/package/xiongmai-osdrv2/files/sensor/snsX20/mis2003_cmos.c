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
    XM_U32 u32Val;
	XM_U16 u16OfstV;	
	XM_U16 u16OfstH;	

	u32Val = sensor_read_register(0x3007);
    
    if(u8Mirror)
	{
		u32Val |= 0x01;
		u16OfstH = 1;
	}
	else
	{
		u32Val &= ~0x01;
		u16OfstH = 0;
	}
	if(u8Flip)
	{
		u32Val |= 0x02;
        u16OfstV = 1;
	}
	else
	{
		u32Val &= ~0x02;
		u16OfstV = 0;
	}
	sensor_write_register(0x3007, u32Val);
	VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfstV);

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
    pstAeSnsDft->u32MinIntTime = 1;	

    pstAeSnsDft->u32MaxAgain = 512;//15.75X
    pstAeSnsDft->u32MinAgain = 64;

    pstAeSnsDft->u32MaxDgain = 128;  
    pstAeSnsDft->u32MinDgain = 32;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;

    pstAeSnsDft->bDelayCfg = XM_FALSE;
    return 0;
}

const STATIC XM_U16 gau16GainTbl[4] =
{
//	/* 1, 2, 3.7, 6.8*/  64,128,237,435  /* U.6 */ 
	/* 1, 2, 3.7, 6.8*/  64,128,256,512  /* U.6 */ 
};

static XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info  value!\n");
		return;
	}
	XM_U8 u8i;
	if(u32InTimes >= gau16GainTbl[3])
	{
		pstAeSnsGainInfo->u32GainDb = 3;
	}
	else
	{
		pstAeSnsGainInfo->u32GainDb = 0;
		for(u8i = 0x1; u8i < 4; u8i ++)        
		{           
			if(u32InTimes <gau16GainTbl[u8i])            
			{                
				pstAeSnsGainInfo->u32GainDb = u8i-1;
				break;            
			}            
		}
	}
	pstAeSnsGainInfo->u32SnsTimes = gau16GainTbl[pstAeSnsGainInfo->u32GainDb];
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

	if(u32InTimes > 496)// 248
	{
		u32InTimes = 496;
	}

	for(i = 0; i < 5; i++)
	{
		if(u32InTimes < 64)
		{
			break;
		}
		u32InTimes >>= 1;
	}
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes<<i;
	pstAeSnsGainInfo->u32GainDb = ((i<<5)|(u32InTimes-32)) | (pstAeSnsGainInfo->u32SnsTimes<<16);
	return;
}


STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
    static XM_U32 su32IntTime = 0xFFFFFFFF;
	if(su32IntTime == u32IntTime)
		return ;
    gu32ShutNow = u32IntTime;
    su32IntTime = u32IntTime;
    sensor_write_register(0x3101, (u32IntTime&0xff));  
    sensor_write_register(0x3100, ((u32IntTime&0xff00)>>8));  
	return;
}

STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	//XM_U32 u32AllGain;
	//u32AllGain = (XM_U32)gau16GainTbl[u32Again]*(u32Dgain>>16) >> 5;//64*32 -> 64
	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;
		// 1.AGainUpdate
		sensor_write_register(0x3102, u32Again);
		// 2.DGainUpdate
		sensor_write_register(0x3103, u32Dgain&0xFF);
		// 3.Logic
        //temper_logic(u32AllGain);
	}
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
            sensor_write_register(0x3203,0x98); 
            sensor_write_register(0x3202,0x08); 
			DEBUG("cmos_fps_set MIS2003 2M 30fps\n");
			break;
		case 25:
            sensor_write_register(0x3203,0x50); 
            sensor_write_register(0x3202,0x0a); 
			DEBUG("cmos_fps_set MIS2003 2M 30fps\n");
			break;
		default:
            DEBUG("cmos_fps_set MIS2003 2M Error\n");
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
	preU16FullLine = u16FullLines;
	u16FullLines = (u16FullLines>=4096)?4000:u16FullLines;
	pstAeSnsDft->u32MaxIntTime = u16FullLines - 2;
	SysDelay_ms(100);
	if(gu32ShutNow>pstAeSnsDft->u32MaxIntTime)
	{
		cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
	}
	sensor_write_register(0x3200,((u16FullLines>>8)&0xFF));
	sensor_write_register(0x3201,(u16FullLines&0xFF));
	
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}


XM_S32 cmos_init_sensor_exp_function_MIS2003(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_MIS2003(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
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


#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_MIS2003 =
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
	{0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x30,0x30,0x30,0x28,0x28,0x28,0x28,0x28}
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_MIS2003 =
{
#if 0
	5000,//0.15
	{
		0x000, 356, -161, 61,
		0x000,-62,  302,  16,
		0x000, 11, -137, 382
	},
	4000,//0.15
	{
		0x000,348, -164, 72,
		0x000,-81, 288, 49,
		0x000,-22,-167, 445
	},
	2800,//0.12
	{
		0x000, 259,  -95, 92,
		0x000, -130, 308, 78,
		0x000, -155, -295,706
	}
#else
	5000,//0.15
	{
		0x000, 348, -127, 35,
		0x000,-91,	372,  -25,
		0x000, 22, -148, 382
	},
	4000,//0.15
	{
		0x000,342, -143, 57,
		0x000,-95, 369, -18,
		0x000,8,-200, 448
	},
	2800,//0.12
	{
		0x000, 245,  -38, 49,
		0x000, -144, 397, 3,
		0x000, -110, -338,704
	}

#endif
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_MIS2003 ={
	{0, 0, 4096, 2613, 3999, 1931, 2078, 4096},
	{4096, 3526, 0, 0, 2891, 2395, 4096, 794},
	{-1357, -4096, -1975, -4096, -4096, -4096, -3521, -2770},
	213,
	0, 1783, 0, 3266,
	{0, 1011, 1320, 1591, 1698, 1815, 2041, 2069, 3266, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1415, 1024, 1357, 0} 
};

// ExtClk:27M
// 1080P30fps,12Bit,SPI,LVDS 2CH,ADC12Bit
// Nomal Mode register setting
static const XM_U16 gau16SnsInit_MIS2003[][2] = {
	{0x3006, 0x04},
	{0x3700, 0xD9},  // @2018.08.03, D9-->DB
	{0x370C, 0x01}, // BLC 稳定 TH = 0x03 modified by fanjia 20190419
	{0x370E, 0x01}, // 连续 (2^0x370E)帧变化 Target值才响应							
	{0x3800, 0x00}, // DRNC  ARNC 
	{0x3B00, 0x01}, // no overflow
	{0x3400, 0x0b}, //下降沿采样
	{0x3D00, 0x01},
	{0x410c, 0x40},	 // modified by fanjia 20190419
	{0x400E, 0x24},
	{0x4018, 0x18},
	{0x4020, 0x14},
	{0x4026, 0x1E},
	{0x402A, 0x26},
	{0x402C, 0x3C},
	{0x4030, 0x34},
	{0x4034, 0x34},
	{0x4036, 0xE0},  // sharding消除、第一个斜波拉长500ns、保持STA_DA与EN_RAMP间隔200ns
	{0x4111, 0x0f},  //  modified by fanjia 20190419
	{0x4110, 0x88}, 
	{0x410E, 0x02}, // 太阳黑子 LM模式
	{0x4104, 0x25}, //太阳黑子消除 modified by fanjia 20190419
	{0x4101, 0x01}, // CPN = OFF modified by songbo 20180118
	{0x3100, 0x04}, //曝光时间调整寄存器
	{0x3101, 0x64},
	//0x3102, 0x00,    //PGA调整寄存器 1x,2x,4x,8x
	//0x3103, 0x00,  //ADC 调整寄存器 1x～8(1+31/32)xx
	//*******CIS_IN=37.125Mhz,PCLK=74.25Mhz******//
	{0x3300, 0x30},
	{0x3301, 0x02},
	{0x3302, 0x02},
	{0x3303, 0x04},
	//1080P25帧帧长度配置(2640*1125);
	{0x3200, 0x04},
	{0x3201, 0x65},
	{0x3202, 0x0A},
	{0x3203, 0x50},
	//1080P25有效像素画幅(1928*1088);
	{0x3204, 0x00},
	{0x3205, 0x00},
	{0x3206, 0x04},
	{0x3207, 0x3F},
	{0x3208, 0x00},
	{0x3209, 0x00},
	{0x320A, 0x07},
	{0x320B, 0x87},
	//@20180524 条纹问题
	{0x4111, 0x02},	// 0x0F->0x02->0x00
	//@20180803 负截距问题
	{0x4108, 0x13},// 0x23->0x13 

	{0x3009, 0x02}	// 0x00 优化长线花屏
};

XM_U32 sensor_getlist_MIS2003(XM_U16 *pu16Num)
{
    DEBUG("------------- MIS2003 2M 25fps  init ok! (@20191010)----------------\n");
	*pu16Num = sizeof(gau16SnsInit_MIS2003)/sizeof(gau16SnsInit_MIS2003[0]);
	return (XM_U32)gau16SnsInit_MIS2003;
}
#endif



