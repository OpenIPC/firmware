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
    XM_U32 u32Tmp;
    XM_U16 u16OfstH,u16OfsV;
    u32Tmp = sensor_read_register(0x12);
    u32Tmp = u32Tmp&0xCF;
    if(u8Mirror)
    {
        u32Tmp &= 0xDF;
        u16OfstH = 1;
    }
    else
    {
        u32Tmp |= 0x20;
        u16OfstH = 0;
    }
        
    if(u8Flip)
    {
        u32Tmp |= 0x10;
        u16OfsV = 1;
    }
    else
    {
        u32Tmp &= 0xEF;
        u16OfsV = 0;
    }
    sensor_write_register(0x12, u32Tmp);
    VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfsV);
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

    pstAeSnsDft->u32MaxAgain = 15872;//15.5X
    pstAeSnsDft->u32MinAgain = 1024;

    pstAeSnsDft->u32MaxDgain = 1024;  
    pstAeSnsDft->u32MinDgain = 1024;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;

    pstAeSnsDft->bDelayCfg = XM_FALSE;
    return 0;
}

static XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	XM_U8 i;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	u32InTimes = u32InTimes/64;
      	u32InTimes = u32InTimes < 16? 16: u32InTimes;

	if(u32InTimes > 248)// 248
	{
		u32InTimes = 248;
	}
	
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
    pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;
	return;
}

STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
    static XM_U32 su32IntTime = 0xFFFFFFFF;
    if(su32IntTime == u32IntTime)
        return ;
	gu32ShutNow = u32IntTime;
    su32IntTime = u32IntTime;
    sensor_write_register(0x01, u32IntTime&0xFF);
	sensor_write_register(0x02, (u32IntTime>>8)&0xFF);
    return;
}

STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
    static XM_U32 su32AGain = 0xFFFFFFF;
	if(su32AGain == u32Again)
	{
        return;
	}
	su32AGain = u32Again;
	sensor_write_register(0x00, u32Again);
	if(u32Again > 0x10)//2X
	{
        sensor_write_register(0x2f, 0x44);
        sensor_write_register(0x0C, 0x00);
        sensor_write_register(0x82, 0x21);
	}
	else
	{
        sensor_write_register(0x2f, 0x64);
        sensor_write_register(0x0C, 0x40);
        sensor_write_register(0x82, 0x23);
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
            sensor_write_register(0x20,0x4C);
            sensor_write_register(0x21,0x04);
            sensor_write_register(0x27,0x18);
            //sensor_write_register(0x29,0x07);
            sensor_write_register(0x2a,0x12);
            //sensor_write_register(0x2b,0x17);
			DEBUG("cmos_fps_set F37 2M 30fps\n");
			break;
		case 25:
		default:
            sensor_write_register(0x20,0x28);
            sensor_write_register(0x21,0x05);
            sensor_write_register(0x27,0xF4);
            //sensor_write_register(0x29,0x08);
            sensor_write_register(0x2a,0xEE);
            //sensor_write_register(0x2b,0x18);
            DEBUG("cmos_fps_set F37 2M 25fps\n");
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
    sensor_write_register(0x22, u16FullLines&0xFF);
	sensor_write_register(0x23, (u16FullLines>>8)&0xFF);

	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}


XM_S32 cmos_init_sensor_exp_function_F37(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_F37(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_F37 =
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
	{0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x30,0x30,0x30,0x28,0x28,0x28,0x28,0x28}
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_F37 =
{
    5000,//0.14
	{
		0x0000, 277, -70, 49,
		0x0000, 10, 258, -12,
		0x0000, 44, -68, 280,
	},
	4000,//0.14
	{
		0x0000, 280, -89, 65,
		0x0000, -6, 251, 11,
		0x0000, 31, -99, 324,
	},
	2800,//0.12
	{
		0x0000, 239, -48, 65,
		0x0000, -42, 274, 24,
		0x0000, -41, -211, 508,
	}
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_F37 = {
    {0, 0, 4096, 2953, 3828, 2046, 1051, 4096},
    {4096, 4096, 0, 0, 4096, 2975, 4096, 1412},
    {-744, -4007, -1325, -4096, -3425, -4096, -1873, -2384},
    213,
    0, 1693, 0, 2716,
    {0, 908, 1109, 1430, 1524, 1631, 1848, 1864, 2716, 0, 0, 0, 0, 0, 0, 0},
    {2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
    {1643, 1024, 1821, 0}
};


// ExtClk:27M
// 1080P30fps,12Bit,SPI,LVDS 2CH,ADC12Bit
// Nomal Mode register setting
static const XM_U8 gau8SnsInit_F37[][2] = {
#if 1
    {0x12,0x60}, 
    {0x48,0x86}, 
    {0x48,0x06}, 
    {0x0E,0x11}, 
    {0x0F,0x14}, 
    {0x10,0x37}, 
    {0x11,0x80}, 
    {0x0D,0xF0}, 
    {0x5F,0x42}, 
    {0x60,0x2B}, 
    {0x58,0x12}, 
    {0x57,0x60}, 
    {0x9D,0x00}, 
    {0x20,0x4C}, 
    {0x21,0x04}, 
    {0x22,0x65}, 
    {0x23,0x04}, 
    {0x24,0xE2}, 
    {0x25,0x40}, 
    {0x26,0x41}, 
    {0x27,0x18}, 
    {0x28,0x11}, 
    {0x29,0x04}, 
    {0x2A,0x12}, 
    {0x2B,0x14}, 
    {0x2C,0x00}, 
    {0x2D,0x01}, 
    {0x2E,0x15}, 
    {0x2F,0x44}, 
    {0x41,0xC7}, 
    {0x42,0x3B}, 
    {0x47,0x42}, 
    {0x76,0x6A}, 
    {0x77,0x09}, 
    {0x1D,0x00}, 
    {0x1E,0x04}, 
    {0x6C,0x40}, 
    {0x6E,0x2C}, 
    {0x70,0xDC}, 
    {0x71,0xD3}, 
    {0x72,0xD4}, 
    {0x73,0x58}, 
    {0x74,0x02}, 
    {0x78,0x8B}, 
    {0x89,0x01}, 
    {0x6B,0x20}, 
    {0x86,0x40}, 
    {0x31,0x06}, 
    {0x32,0x1C}, 
    {0x33,0x5C}, 
    {0x34,0x44}, 
    {0x35,0x44}, 
    {0x3A,0xAF}, 
    {0x3B,0x00}, 
    {0x3C,0x3C}, 
    {0x3D,0x4A}, 
    {0x3E,0xFF}, 
    {0x3F,0x68}, 
    {0x40,0xFF}, 
    {0x56,0xB2}, 
    {0x59,0x80}, 
    {0x5A,0x04}, 
    {0x85,0x40}, 
    {0x8A,0x04}, 
    {0x91,0x13}, 
    {0x9B,0x03}, 
    {0x9C,0xE1}, 
    {0xA9,0x78}, 
    {0x5B,0xB0}, 
    {0x5C,0x71}, 
    {0x5D,0xF6}, 
    {0x5E,0x14}, 
    {0x62,0x01}, 
    {0x63,0x0F}, 
    {0x64,0xC0}, 
    {0x65,0x02}, 
    {0x67,0x65}, 
    {0x66,0x04}, 
    {0x68,0x00}, 
    {0x69,0x7C}, 
    {0x6A,0x12}, 
    {0x7A,0x80}, 
    {0x82,0x21}, 
    {0x8F,0x91}, 
    {0xAE,0x30}, 
    {0x13,0x81}, 
    {0x96,0x04}, 
    {0x4A,0x05}, 
    {0x7E,0xCD}, 
    {0x50,0x02}, 
    {0x49,0x10}, 
    {0xAF,0x12}, 
    {0x80,0x43}, 
    {0x7B,0x4A}, 
    {0x7C,0x08}, 
    {0x7F,0x57}, 
    {0x90,0x00}, 
    {0x8C,0xFF}, 
    {0x8D,0xC7}, 
    {0x8E,0x00}, 
    {0x8B,0x01}, 
    {0x0C,0x00}, 
    {0x81,0x74}, 
    {0x19,0x20}, 
    {0x46,0x00}, 
    {0x12,0x20}, 
    {0x48,0x86}, 
    {0x48,0x06}
#else
    {0x12,0x60},    
    {0x48,0x8A},    
    {0x48,0x0A},    
    {0x0E,0x11},    
    {0x0F,0x14},    
    {0x10,0x36},    
    {0x11,0x80},    
    {0x0D,0xF0},    
    {0x5F,0x41},    
    {0x60,0x20},    
    {0x58,0x12},    
    {0x57,0x60},    
    {0x9D,0x00},    
    {0x20,0x00},    
    {0x21,0x09},    
    {0x22,0x65},    
    {0x23,0x04},    
    {0x24,0xC4},    
    {0x25,0x40},    
    {0x26,0x43},    
    {0x27,0x98},    
    {0x28,0x11},    
    {0x29,0x08},    
    {0x2A,0x8C},    
    {0x2B,0x18},    
    {0x2C,0x00},    
    {0x2D,0x00},    
    {0x2E,0x14},    
    {0x2F,0x44},    
    {0x41,0xC7},    
    {0x42,0x3B},    
    {0x47,0x42},    
    {0x76,0x6A},    
    {0x77,0x09},    
    {0x1D,0x00},    
    {0x1E,0x04},    
    {0x6C,0x40},    
    {0x6E,0x2C},    
    {0x70,0xDC},    
    {0x71,0xD3},    
    {0x72,0xD4},    
    {0x73,0x58},    
    {0x74,0x02},    
    {0x78,0x96},    
    {0x89,0x01},    
    {0x6B,0x20},    
    {0x86,0x40},    
    {0x31,0x0C},    
    {0x32,0x38},    
    {0x33,0x6C},    
    {0x34,0x88},    
    {0x35,0x88},    
    {0x3A,0xAF},    
    {0x3B,0x00},    
    {0x3C,0x57},    
    {0x3D,0x78},    
    {0x3E,0xFF},    
    {0x3F,0xF8},    
    {0x40,0xFF},    
    {0x56,0xB2},    
    {0x59,0xE8},    
    {0x5A,0x04},    
    {0x85,0x70},    
    {0x8A,0x04},    
    {0x91,0x13},    
    {0x9B,0x03},    
    {0x9C,0xE1},    
    {0xA9,0x78},    
    {0x5B,0xB0},    
    {0x5C,0x71},    
    {0x5D,0xF6},    
    {0x5E,0x14},    
    {0x62,0x01},    
    {0x63,0x0F},    
    {0x64,0xC0},    
    {0x65,0x02},    
    {0x67,0x65},    
    {0x66,0x04},    
    {0x68,0x00},    
    {0x69,0x7C},    
    {0x6A,0x12},    
    {0x7A,0x80},    
    {0x82,0x21},    
    {0x8F,0x91},    
    {0xAE,0x30},    
    {0x13,0x81},    
    {0x96,0x04},    
    {0x4A,0x05},    
    {0x7E,0xCD},    
    {0x50,0x02},    
    {0x49,0x10},    
    {0xAF,0x12},    
    {0x80,0x41},    
    {0x7B,0x4A},    
    {0x7C,0x08},    
    {0x7F,0x57},    
    {0x90,0x00},    
    {0x8C,0xFF},    
    {0x8D,0xC7},    
    {0x8E,0x00},    
    {0x8B,0x01},    
    {0x0C,0x00},    
    {0x81,0x74},    
    {0x19,0x20},    
    {0x46,0x00},    
    {0x12,0x20},    
    {0x48,0x8A},    
    {0x48,0x0A}  
#endif
};

XM_U32 sensor_getlist_F37(XM_U16 *pu16Num)
{
    DEBUG("------------- SOIF37 2M 25fps  init ok! (@20190909)----------------\n");
	*pu16Num = sizeof(gau8SnsInit_F37)/sizeof(gau8SnsInit_F37[0]);
	return (XM_U32)gau8SnsInit_F37;
}
#endif



