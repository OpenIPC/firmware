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
	if(XM_NULL == pstAeSnsGainInfo)
	{
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
	pstAeSnsGainInfo->u32SnsTimes = (u32InTimes<<i)*64;
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
	XM_U32 u32Tmp = 1920;
	switch(u8Fps)
	{
		case 30:
			// Change the frame rate via changing the vertical blanking
			u32Tmp = 1600;
			break;
		case 25:
		default:
			u8Fps = 25;
			u32Tmp = 1920;
			break;
	}		
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = gu16FullLines;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-gu8MaxShutterOfst;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	sensor_write_register(0x20,u32Tmp&0xFF);   		//FrameW  
	sensor_write_register(0x21,(u32Tmp>>8)&0xFF);   //FrameW  
	return;
}





XM_S32 cmos_init_sensor_exp_function_h62(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
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

XM_S32 cmos_init_ae_exp_function_h62(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
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
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_h62 =
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

const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_h62 = {
	1, HD720P_LINES-4,	// minInt   	maxInt
	1024,31744, 		// minAgain maxAgain
	32,32,				// minDgain 	maxDgain
	0xFFF,				// maxIspGain
	0x01000002,			// Update
	{700,700,700,700},
	0x00
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_h62 = {
	5000,
	{
		0x0000,241, 28,-13,
		0x0000,-38,349,-55,
		0x0000,  2, -2,256
	},
	4000,
	{
		0x0000,242,  7,  7,
		0x0000,-95,354,-3,
		0x0000,-21,-44,321
	},
	2800,
	{
		0x0000,197, 98,-39,
		0x0000,-90,360,-14,
		0x0000,-62,-78,396
	}
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_h62 = {
    {0, 0, 4096, 3068, 2226, 2101, 1351, 4096},
    {4096, 4096, 0, 0, 4096, 4030, 4096, 1024},
    {-996, -3084, -1672, -4096, -3218, -4096, -2454, -2352},
    213,
    0, 1564, 0, 2572,
    {0, 686, 983, 1280, 1394, 1562, 1742, 1757, 2572, 0, 0, 0, 0, 0, 0, 0},
    {2000, 2500, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
    {1731, 1024, 1698, 0}
};

static const XM_U8 gau8SnsInit_h62[][2] = {
	//INI Start
	{0x12,0x40}, // System status set up
	// PllClk =  ExtClk*p / (1+(m&0x03)) /(1+(n&0x38))
	// 24M=>36M
#if 1
	// 24M=>36M
	{0x0E,0x11}, //m  1
#else
	// 36M=>36M
	{0x0E,0x12}, //m  2
#endif

	{0x0F,0x00}, //n  0
	{0x10,0x18}, //p  24
	{0x11,0x80}, // use pllClk
	{0x19,0x48}, // 0x68
	//Frame/Window
	{0x20,0x80}, // FrameW [7:0]        
	{0x20, 0x80}, 
	{0x21, 0x07},            
	{0x22,0xEE}, // FrameH [7:0]                   
	{0x23,0x02}, // FrameH [15:8]   
	{0x24,0x08}, // HWin LSBs [7:0]                
	{0x25,0xD8}, // VWin LSBs [7:0]                
	{0x26,0x25}, // HVWin MSBs {V[11:8],H[11:8]}   
	{0x27,0x10}, // HWinSt LSBs [7:0]              
	{0x28,0x13}, // VWinSt LSBs [7:0]              
	{0x29,0x02}, // HVWinSt MSBs {V[11:8],H[11:8]} 
	{0x2A,0x01}, // CShift1
	{0x2B,0x21}, // CShift2
	{0x2C,0x04}, // SenHASt
	{0x2D,0x00}, // SenVSt 
	{0x2E,0xBB}, // SenVEnd
	{0x2F,0xC0}, // SenVadd
	{0x41,0x8C}, 
	{0x42,0x12}, 
	{0x39,0x90}, 
	//Interface
	{0x1D,0xFF}, // DVP control 3
	{0x1E,0xBF}, // DVP control 4 ( Pclk Polarity )下降沿输出，1F改为BF
	{0x7A,0x80}, 
	{0x1F,0x20}, 
	//Sensor Timing
	{0x30,0x90}, 
	{0x31,0x0C}, 
	{0x32,0xFF}, 
	{0x33,0x0C}, 
	{0x34,0x4B}, 
	{0x35,0xA3}, 
	{0x36,0x06}, 
	{0x38,0x40}, 
	{0x3A,0x08}, 
	{0x56,0x02}, 
	{0x60,0x01}, 
	//DVP Setting  
	{0x0D,0x54}, 	// 0x58
	{0x57,0x80}, 
	{0x58,0x33}, 
	{0x5A,0x04}, 
	{0x5B,0xB6}, 
	{0x5C,0x08}, 
	{0x5D,0x67}, 
	{0x5E,0x04}, 
	{0x5F,0x08}, 
	{0x66,0x28}, 
	{0x67,0xF8}, 
	{0x68,0x04}, 
	{0x69,0x74}, 
	{0x6A,0x1F}, 

	{0x63,0x80}, //降低功耗20170405

	{0x6C,0xC0}, 
	{0x6E,0x5C}, 
	{0x82,0x01}, 
	{0x0C,0x00}, 
	{0x46,0xC2}, 
	{0x48,0x7E}, 
	{0x62,0x40}, 
	{0x7D,0x57}, 
	{0x7E,0x28}, 
	{0x80,0x00}, 
	{0x4A,0x05}, 
	{0x49,0x10}, 
	{0x13,0x81}, 
	{0x59,0x97}, 
	//INI End
	{0x12,0x00}, // System status set up
	{0x47,0x47}, 

	{0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 0x64},		// 100ms

	{0x47,0x44}, // BLCCtrl2
	{0x1F,0x21} // Group latch control
};

XM_U32 sensor_getlist_h62(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau8SnsInit_h62)/sizeof(gau8SnsInit_h62[0]);
	return (XM_U32)gau8SnsInit_h62;
}

#endif

