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


STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U32 u32Tmp;
	XM_U16 u16OfstH,u16OfsV;
	u32Tmp = sensor_read_register(0x12);
	u32Tmp = u32Tmp&0xCF;
	if(u8Mirror)
	{
		u32Tmp |= 0x20;
		u16OfstH = 1;
	}
	else
	{
		u32Tmp &= 0xDF;
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

STATIC XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;
	XM_U32 u32MinGain;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info  value!\n");
		return;
	}
	u32MinGain = 1<<gstGainModeDt.stLinear.u8AgainMin;
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	u32InTimes = u32InTimes<<((gstGainModeDt.stLinear.u8AgainSft>>4)&0x0F);
	u32InTimes = u32InTimes>>(gstGainModeDt.stLinear.u8AgainSft&0x0F);
	u32InTimes = u32InTimes < u32MinGain? u32MinGain: u32InTimes;
	
	for(i = 0; i < 10; i++)
	{
		if(u32InTimes < (u32MinGain<<1))
		{
			break;
		}
		u32InTimes >>= 1;
	}
	
	pstAeSnsGainInfo->u32GainDb = (i<<gstGainModeDt.stLinear.u8AgainMin)+u32InTimes-u32MinGain; 
	u32InTimes = u32InTimes<<i;

	u32InTimes = u32InTimes<<(gstGainModeDt.stLinear.u8AgainSft&0x0F);
	u32InTimes = u32InTimes>>((gstGainModeDt.stLinear.u8AgainSft>>4)&0x0F);
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
	return;
}






STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
#if 0
	sensor_write_register(0x01, u32IntTime&0xFF);
	sensor_write_register(0x02, (u32IntTime>>8)&0xFF);
#else
	XM_U32 u32Tmp;
	u32Tmp = sensor_read_register(0x1F);
	sensor_write_register(0xC0, 0x01);
	sensor_write_register(0xC1, u32IntTime&0xFF);
	sensor_write_register(0xC2, 0x02);
	sensor_write_register(0xC3, (u32IntTime>>8)&0xFF);
	sensor_write_register(0x1F, u32Tmp|0x80);
#endif
	return;
}

STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	sensor_write_register(0x00, u32Again);
	return;
}



/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32Tmp = 0xC0;
	switch(u8Fps)
	{
		case 30:
			// Change the frame rate via changing the vertical blanking
			if(gu8Resolution == (1<<RSLTION_BIT_960P))
			{
				u32Tmp = 0x38;
				pstAeSnsDft->u32MaxIntTime = gu16FullLines-4;
				u8Fps = 20;
			}
			else
			{
				u32Tmp = 0x20;
				pstAeSnsDft->u32MaxIntTime = gu16FullLines-4;
			}
			if(pstAeSnsDft != NULL)
			{
				pstAeSnsDft->u32FullLinesStd = gu16FullLines;
				pstAeSnsDft->u32LinesPer500ms = gu16FullLines * u8Fps / 2;
			}
			sensor_write_register(0x20, u32Tmp); 
			break;

		case 25:
			// Change the frame rate via changing the vertical blanking
			if(gu8Resolution == (1<<RSLTION_BIT_960P))
			{
				u32Tmp = 0x38;
				pstAeSnsDft->u32MaxIntTime = gu16FullLines-4;
				u8Fps = 20;
			}
			else
			{
				u32Tmp = 0xC0;
				pstAeSnsDft->u32MaxIntTime = gu16FullLines-4;
			}
			
			if(pstAeSnsDft != NULL)
			{
				pstAeSnsDft->u32FullLinesStd = gu16FullLines;
				pstAeSnsDft->u32LinesPer500ms = gu16FullLines * u8Fps / 2;
			}
			sensor_write_register(0x20, u32Tmp); 
			break;

		default:
			break;
	}		
	//preFps = u8Fps;
	return;
}





XM_S32 cmos_init_sensor_exp_function_soi(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
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

XM_S32 cmos_init_ae_exp_function_soi(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	//pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
	if((gstGainModeDt.stLinear.u8AgainSft==0xFF) && (gstGainModeDt.stLinear.u8AgainMin==0xFF))
	{
		gstGainModeDt.stLinear.u8AgainSft = 6;
		gstGainModeDt.stLinear.u8AgainMin = 4;
	}
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= NULL;    
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_gain_calc_table_com;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}



#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_h65 =
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	//[0~7]:Normal						
	/* sharpen_D	H	*/
	{0x18,0x18,0x18,0x18,0x18,0x14,0x10,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	
	/* sharpen_Ud M */
	{0x20,0x20,0x20,0x1C,0x1C,0x1A,0x18,0x18,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},

	/* sharpen_Kd  */
	{0x31,0x30,0x30,0x2C,0x28,0x1C,0x18,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	
	/* snr_thresh 2DNr		*/
	{0x00,0x01,0x04,0x10,0x20,0x28,0x30,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},

	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x06,0x0A,0x0E,0x12,0x15,0x17,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18},

	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x00,0x00,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},  

	/* DyDpc_thresh 		*/
	{0xB4,0xB8,0xD4,0xD8,0xE0,0xE0,0xE8,0xE8,0xE8,0xE8,0xE8,0xE8,0xE8,0xE8,0xE8,0xE8},
	
	/* saturation_strength */
	{0xA4,0x9F,0x9A,0x90,0x88,0x78,0x55,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	
	/* Blc	*/
	{0x40,0x40,0x40,0x40,0x38,0x30,0x30,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18},

	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32},
};

const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_h65 = {
	1, HD720P_LINES-4,	// minInt   	maxInt
	1024,16384, 		// minAgain maxAgain
	32,32,				// minDgain 	maxDgain
	0xFFF,				// maxIspGain
	0x01000002,			// Update
	{700,700,700,700},
	0x00
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_h65 = {
	5000,
	{
		0x0000,262,	-36,	30,
		0x0000,	-3,	254,	5,
		0x0000,	29,	-93, 320,
	},
		
	4000,
	{
		0x0000, 219, -34, 71,
		0x0000, -12, 200, 68,
		0x0000, 27, -102, 331
	},

	2800,
	{
		0x0000, 182, 29, 45,
		0x0000, -43, 221, 78,
		0x0000, -40, -149, 445
	}
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_h65 = {
   {0, 0, 4096, 2537, 3013, 1771, 593, 4096},
   {4096, 4096, 0, 0, 4096, 4096, 4096, 1888},
   {-536, -2892, -1496, -4096, -3037, -3792, -1331, -2560},
   213,
   0, 1675, 0, 2485,
   {0, 540, 949, 1255, 1371, 1499, 1654, 1784, 2485, 0, 0, 0, 0, 0, 0, 0},
   {2000, 2500, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
   {1619, 1024, 2112, 0}
};

static const XM_U8 gau8SnsInit_h65[][2] = {
	{0x12, 0x40},
	{0x0E, 0x11},
	{0x0F, 0x04},
	{0x10, 0x1E},
	{0x11, 0x80},
	{0x5F, 0x01},
	{0x60, 0x10},
	{0x19, 0x44},
	{0x48, 0x25},
	{0x20, 0xC0},
	{0x21, 0x03},
	{0x22, 0xEE},
	{0x23, 0x02},
	{0x24, 0x86},
	{0x25, 0xD8},
	{0x26, 0x22},
	{0x27, 0x5C},
	{0x28, 0x12},
	{0x29, 0x01},
	{0x2A, 0x48},
	{0x2B, 0x25},
	{0x2C, 0x00},
	{0x2D, 0x1F},
	{0x2E, 0xDA},
	{0x2F, 0x40},
	{0x41, 0x90},
	{0x42, 0x12},
	{0x39, 0x90},
	{0x1D, 0xFF},
	{0x1E, 0x1F},
	{0x6C, 0x80},
	{0x1F, 0x10},
	{0x31, 0x0C},
	{0x32, 0x20},
	{0x33, 0x0C},
	{0x34, 0x4F},
	{0x36, 0x06},
	{0x38, 0x39},
	{0x3A, 0x08},
	{0x3B, 0x50},
	{0x3C, 0xA0},
	{0x3D, 0x00},
	{0x3E, 0x01},
	{0x3F, 0x00},
	{0x40, 0x00},
	{0x0D, 0x50},
	{0x5A, 0x43},
	{0x5B, 0xB3},
	{0x5C, 0x0C},
	{0x5D, 0x7E},
	{0x5E, 0x24},
	{0x62, 0x40},
	{0x67, 0x48},
	{0x6A, 0x11},
	{0x68, 0x04},
	{0x8F, 0x9F},
	{0x0C, 0x00},
	{0x59, 0x97},
	{0x4A, 0x05},
	{0x50, 0x03},
	{0x47, 0x62},
	{0x7E, 0xC0},
	{0x8D, 0x87},
	{0x49, 0x10},
	{0x7F, 0x52},
	{0x8E, 0x00},
	{0x8C, 0xFF},
	{0x8B, 0x01},
	{0x57, 0x02},
	{0x94, 0x00},
	{0x95, 0x00},
	{0x63, 0x80},
	{0x7B, 0x46},
	{0x7C, 0x2D},
	{0x90, 0x00},
	{0x79, 0x00},
	{0x13, 0x81},
	{0x12, 0x00},
	{0x45, 0x89},
	{0x93, 0x68},
	{0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 0x64},		// 100ms
	{0x45, 0x19},
	{0x1F, 0x11},
	{0x1E, 0x3F},
};

XM_U32 sensor_getlist_h65(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau8SnsInit_h65)/sizeof(gau8SnsInit_h65[0]);
	return (XM_U32)gau8SnsInit_h65;
}

#endif


