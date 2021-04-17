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
extern void (*pfn_gainLogic)(XM_U32 u32AllGain);
extern void (*pfn_shutLogic)(XM_U32 u32IntTime);

extern XM_U32 gau32AllGain;
extern XM_SENSOR_CHIP gSensorChip;
extern XM_U32 gu32AGainNow;
extern XM_U32 gu32AGainMin;
extern XM_U32 gu32DGainNow;
extern XM_U32 gu32DGainMin;
extern XM_U8 gu8BlcDpcFlg;
extern XM_U32 gu32ShutNow;

STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
    XM_U32 u32Val;
    XM_U16 u16OfstH = 0;
    XM_U16 u16OfstV = 0;
	u32Val = sensor_read_register(0x3221);
	u16OfstH = 0;
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
		u16OfstV = 1;
	}
	else
	{
		u32Val &= ~0x60;
		u16OfstV = 0;
	}
	sensor_write_register(0x3221, u32Val);
	VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfstV);
    XM_MPI_MIPI_RefreshFV(100, 0);
    return XM_SUCCESS;
}




STATIC XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
 
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	XM_U8 u8i;
	if(u32InTimes >= gau16GainTbl_SmatSns[63])
	{
		pstAeSnsGainInfo->u32GainDb = 63;
	}
	else
	{
		pstAeSnsGainInfo->u32GainDb = 0;
		for(u8i = 0x1; u8i < 64; u8i ++)		
		{			
			if(u32InTimes <gau16GainTbl_SmatSns[u8i])			 
			{				 
				pstAeSnsGainInfo->u32GainDb = u8i-1;
				break;			  
			}			 
		}
	}
	pstAeSnsGainInfo->u32SnsTimes = gau16GainTbl_SmatSns[pstAeSnsGainInfo->u32GainDb];
	return;
}

STATIC XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	XM_U8 u8i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
	
	for(u8i = 0; u8i < 10; u8i++)
	{
		if(u32InTimes < 256)
		{
			break;
		}
		u32InTimes >>= 1;
	}
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes<<u8i;
	pstAeSnsGainInfo->u32GainDb = 0;
	for(;u8i>0;u8i --)
	{
		pstAeSnsGainInfo->u32GainDb=(pstAeSnsGainInfo->u32GainDb<<1)|0x01;
	}
	pstAeSnsGainInfo->u32GainDb = (pstAeSnsGainInfo->u32GainDb<<8) | u32InTimes;
	return;
}

void shutLogic_sc4236(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;
	if(u32IntTime<80)
	{
		sensor_write_register(0x330e, 0x40);
		sensor_write_register(0x3314, 0x14);
	}
	else if(u32IntTime>160)
	{
		sensor_write_register(0x3314, 0x04);
		sensor_write_register(0x330e, 0x50);
	}
}

STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
    static XM_U32 su32IntTime = 0xFFFFFFFF;
	XM_U32 u32Tmp;
    if(su32IntTime == u32IntTime)
        return ;
    su32IntTime = u32IntTime;
    gu32ShutNow = u32IntTime;
    u32IntTime = u32IntTime*2;
	if(pfn_shutLogic)
	{
		(*pfn_shutLogic)(u32IntTime);
	}	
	u32Tmp = ((u32IntTime&0x0ff0)>>4);
	sensor_write_register(0x3e00, ((u32IntTime&0xFF000)>>12));
	sensor_write_register(0x3e01, u32Tmp);
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;
}

const static XM_U16 gau16LogicAddr[4] = {
	0x3301,0x3633,0x3630,0x3622
};
const static XM_U8 gau8Logic_sc4236[5][4] = {
	//  0x3301,0x3633,0x3630,0x3622
		{0x1e, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0xb8, 0x43, 0x82, 0x16}
};
void gainLogic_sc4236(XM_U32 u32AllGain)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Vstd   [bit4~bit7]:Agc
	XM_U8 u8Idx2,u8i;

	if(u32AllGain<32)
		u8Idx2 = 0;
	else if(u32AllGain<64)
		u8Idx2 = 1;
	else if(u32AllGain<128)
		u8Idx2 = 2;
	else if(u32AllGain<248)
		u8Idx2 = 3;
	else
		u8Idx2 = 4;
	if(((su8Idx>>4)&0x0F) != u8Idx2)
	{
		if(su8Idx != 0xFF)
		{
			sensor_write_register(0x3812, 0x00);
		}
		for(u8i=0; u8i<4; u8i ++)
		{
			sensor_write_register((XM_U32)gau16LogicAddr[u8i], (XM_U32)gau8Logic_sc4236[u8Idx2][u8i]);
		}
		if(su8Idx != 0xFF)
		{
			sensor_write_register(0x3812, 0x30);
		}
		su8Idx = ((u8Idx2&0x0F)<<4);
	}
}

STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	static XM_U8 su8Reg0x5785 = 0xFF;
	XM_U8 u8i,u8DpcFlg;
	XM_U32 u32AllGain,u32Tmp;
	unsigned int tmp[2];
	u8DpcFlg = 1;

	u32Again = (u32Again>63)?63:u32Again;
	// DGain
	for(u8i=0,u32Tmp = u32Dgain>>8;u32Tmp>0;u32Tmp=u32Tmp>>1,u8i++);
	u32AllGain = (u32Dgain&0xFF)<<u8i;
	if((gu32DGainNow !=0 ) && (gu32DGainMin != 0))
	{
		u32AllGain = gu32DGainNow;
	}
	else
	{
		gu32DGainMin = 128;
	}
	// Add AGain
	if((gu32AGainNow !=0 ) && (gu32AGainMin != 0))
	{
		u32AllGain = u32AllGain * gu32AGainNow *16/ gu32AGainMin;
	}
	else
	{
		u32AllGain = (XM_U64)gau16GainTbl_SmatSns[u32Again]*u32AllGain*16 / 1024; 
	}
	u32AllGain = u32AllGain/gu32DGainMin;
	gau32AllGain = u32AllGain;

	if(pfn_gainLogic)
	{
		(*pfn_gainLogic)(gau32AllGain);
	}
	if(gu8BlcDpcFlg)
	{
		u8DpcFlg = (gu8BlcDpcFlg>>5) &0x01;
#ifdef DEBUG_ON
		DBG("DpcLogic:	");
		PrintInt(8, (XM_U32)u8DpcFlg); ENTER();
#endif

	}

	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;

		// 3.GainUpdate
		tmp[0] = (u32Again&0x0F) | 0x10;	
		u32Tmp = 0x03;
		u32Again = u32Again/16;
		for(;u32Again>0;u32Again--)
		{
			u32Tmp = (u32Tmp<<1)|0x01;
		}
		tmp[1] = u32Tmp;
		sensor_write_register(0x3e08, tmp[1]);
		sensor_write_register(0x3e09, tmp[0]);
		
		sensor_write_register(0x3e06, (u32Dgain>>8)&0x0F);
		sensor_write_register(0x3e07, u32Dgain&0xFF);
		// 4.DPC
		if(u8DpcFlg)
		{
			if(u32AllGain<160)
			{
				tmp[0] = 0x04;
				tmp[1] = 0x18;
			}
			else 
			{
				tmp[0] = 0x02;
				tmp[1] = 0x08;
			}
			if(su8Reg0x5785 != tmp[1])
			{	
				su8Reg0x5785 = (XM_U8)tmp[1];
				sensor_write_register(0x5781, tmp[0]);
				sensor_write_register(0x5785, tmp[1]);
			}
		}
	}
	return;
}

/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32LineLength = 3120;
	gu16FullLines = 1500;
	
	switch(u8Fps)
	{
		case 30:// 4M_30Fps 2600*1500
			// Change the frame rate via changing the vertical blanking
			u32LineLength = 2600;
			break;
		case 25://4M_25Fps 3120*1500
		default:
			// Change the frame rate via changing the vertical blanking
			u32LineLength = 3120;
			break;
	}		
	pstAeSnsDft->u32MaxIntTime = gu16FullLines-gu8MaxShutterOfst;
	pstAeSnsDft->u32FullLinesStd = gu16FullLines;
	pstAeSnsDft->u32LinesPer500ms = gu16FullLines * u8Fps / 2;
	sensor_write_register(0x320c, (u32LineLength>>8)&0xFF);
	sensor_write_register(0x320d, u32LineLength&0xFF);
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

XM_S32 cmos_init_sensor_exp_function_sc4236(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
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

XM_S32 cmos_init_ae_exp_function_sc4236(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
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
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc4236 =
{
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
	{0xb2,0xb0,0xa8,0x98,0x88,0x78,0x50,0x50,0x50,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	/* Blc	*/
	{0x53,0x53,0x53,0x53,0x53,0x53,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x2d,0x2d,0x2d,0x2d,0x2d,0x2d,0x2d,0x2d,0x2d,0x2d}
};

const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sc4236 = {
	2, 1500-4,	// minInt   	maxInt
	1024,15872,				// minAgain	maxAgain
	128,512,				// minDgain     maxDgain
	0xFFF,				// maxIspGain
	0x01010002,			// Update
	{700,700,700,700},
	0x00
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc4236 =
{
	5000,
	{
		0x000,356,  -135, 35,
		0x000,-49,  327, -22,
        0x000, -5, -145, 406//0x000, -35, -130, 421
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
const ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc4236 = {
	{0, 0, 4096, 2529, 3487, 1967, 1033, 4096},
	{4096, 3885, 0, 0, 3458, 2671, 4096, 1354},
	{-1495, -4096, -1933, -4096, -4096, -4096, -2783, -2907},
	213,
	0, 1925, 0, 3252,
	{0, 890, 1211, 1606, 1727, 1854, 2145, 2156, 3252, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1350, 1024, 1486, 0}
};

// ExtClk: 18M -> 117M
static const XM_U16 gau16SnsInit_sc4236[][2] = {
    // ExtClk: 18M -> 117M
    // @20180202
	{0x0100,0x00},
	{0x3001,0xfe},
	{0x3018,0x33},
	{0x301c,0x78},
	{0x3031,0x0a},
	{0x3037,0x20},
	{0x3038,0xcc},
	{0x303f,0x01},
	{0x3202,0x00},
	{0x3203,0x22},
	{0x3206,0x05},
	{0x3207,0xd7},
	{0x320a,0x05},
	{0x320b,0xb0},
	{0x320c,0x0a},
	{0x320d,0x28},
	{0x320e,0x05},
	{0x320f,0xdc},
	{0x3222,0x29},
	{0x3235,0x0b},
	{0x3236,0xb0},
	{0x3301,0x1e},
	{0x3303,0x30},
	{0x3306,0x60},
	{0x3307,0x18},
	{0x3308,0x18},
	{0x3309,0x80},
	{0x330a,0x01},
	{0x330b,0x10},
	{0x330e,0x50},
	{0x330f,0x04},
	{0x3310,0x20},
	{0x3314,0x04},
	{0x331e,0x21},
	{0x331f,0x71},
	{0x3320,0x06},
	{0x3326,0x00},
	{0x3333,0x20},
	{0x3366,0x78},
	{0x3367,0x08},
	{0x3368,0x04},
	{0x3369,0x00},
	{0x336a,0x00},
	{0x336b,0x00},
	{0x337f,0x03},
	{0x33aa,0x00},
	{0x3620,0x28},
	{0x3622,0xf6},
	{0x3625,0x03},
	{0x3630,0x80},
	{0x3631,0x88},
	{0x3632,0x18},
	{0x3633,0x23},
	{0x3635,0xe2},
	{0x3636,0x24},
	{0x3637,0x63},
	{0x3638,0x18},
	{0x3639,0x09},
	{0x363a,0x1f},
	{0x363b,0x0c},
	{0x363c,0x05},
	{0x363d,0x05},
	{0x3640,0x02},
	{0x3641,0x02},
	{0x3650,0x42},
	{0x366e,0x08},
	{0x366f,0x2f},
	{0x36e9,0x24},
	{0x36ea,0x33},
	{0x36eb,0x0e},
	{0x36ec,0x1e},
	{0x36ed,0x23},
	{0x36f9,0x04},
	{0x36fa,0xca},
	{0x3802,0x01},
	{0x3901,0x02},
	{0x3908,0x15},
	{0x3933,0x0a},
	{0x3934,0x08},
	{0x3940,0x19},
	{0x3941,0x14},
	{0x3942,0x02},
	{0x3943,0x0d},
	{0x3946,0x20},
	{0x3947,0x18},
	{0x3948,0x06},
	{0x3949,0x06},
	{0x394a,0x18},
	{0x394b,0x2c},
	{0x394c,0x08},
	{0x394d,0x14},
	{0x394e,0x24},
	{0x394f,0x34},
	{0x3950,0x14},
	{0x3951,0x08},
	{0x3952,0x78},
	{0x3953,0x48},
	{0x3954,0x18},
	{0x3955,0x18},
	{0x3956,0x48},
	{0x3957,0x80},
	{0x3958,0x10},
	{0x3959,0x20},
	{0x395a,0x38},
	{0x395b,0x38},
	{0x395c,0x20},
	{0x395d,0x10},
	{0x395e,0x24},
	{0x395f,0x00},
	{0x3960,0xc4},
	{0x3961,0xb1},
	{0x3962,0x0d},
	{0x3963,0x80},
	{0x3e01,0xbb},
	{0x3e02,0x60},
	{0x3e06,0x00},
	{0x3e08,0x03},
	{0x3e09,0x10},
	{0x3e1e,0x34},
	{0x3f00,0x0f},
	{0x3f04,0x05},
	{0x3f05,0x00},
	{0x4603,0x00},
	{0x4827,0x46},
	{0x4837,0x20},
	{0x5784,0x10},
	{0x5788,0x10},
#if 0
	{0x5780,0xFF},	// DPC Manual
#else // AutoDpc
	{0x5780,0x7f},   //auto dpc
	{0x5781,0x04},  //white 1x
	{0x5782,0x03},  // 1x~4x
	{0x5783,0x02},  // 4x~8x
	{0x5784,0x01},  // 8x~128x
	{0x5785,0x18},  //black 1x
	{0x5786,0x10},  // 1x~4x
	{0x5787,0x08},  // 4x~8x
	{0x5788,0x02},  // 8x~128x
	{0x5789,0x20},  //gain0=4x 低4位表示 a_fine_gain，高4位表示2^n(不考虑d_fine_gain)
	{0x578a,0x30},  //gain1=8x 2^3=8x,a_fine_gain=0x10,即={0x0f10}=8x
#endif
	{0x3946,0x00},
	{0x3947,0x00},
	{0x3948,0x00},
	{0x3949,0x00},
	{0x394A,0x00},
	{0x394B,0x00},

	{0x3952,0x10},
	{0x3953,0x10},
	{0x3954,0x08},
	{0x3955,0x08},
	{0x3956,0x10},
	{0x3957,0x14},
	{0x3958,0x0a},
	{0x3959,0x10},
	{0x395a,0x18},
	{0x395b,0x18},
	{0x395c,0x10},
	{0x395d,0x0a},
	{0x3962,0x0d},
	{0x0100,0x01}
};

XM_U32 sensor_getlist_sc4236(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau16SnsInit_sc4236)/sizeof(gau16SnsInit_sc4236[0]);
	return (XM_U32)gau16SnsInit_sc4236;
}

#endif

