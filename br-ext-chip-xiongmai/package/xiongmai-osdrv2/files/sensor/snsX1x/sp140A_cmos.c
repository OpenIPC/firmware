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

volatile  XM_U8 u8BusyFlg = 0;	// 0:free 1:busy 2:stop
static XM_S32 busyFlg_set()
{
	XM_U8 u8i;
	for(u8i =0; u8i <50; u8i ++)
	{
		if(u8BusyFlg)
			SysDelay_ms(40);
		else
		{
			u8BusyFlg= 2;
			return XM_SUCCESS;
		}
	}
	return XM_FAILURE;
}

STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U32 u32Tmp;
	XM_U16 u16OfstH,u16OfstV;
	XM_U32 u32RegAddr = 0x15;
	XM_U8 u8MirrorBit,u8FlipBit;
	XM_U32 u32UpdateReg = 0xfe02;
	u16OfstV = 0;
	if(busyFlg_set() != XM_SUCCESS)
	{
		return XM_FAILURE;
	}
	u8MirrorBit = 0;
	u8FlipBit = 1;
	u32RegAddr = 0x3f;
	u32UpdateReg = 0x0101;
	u16OfstV = 0x8001;
	sensor_write_register(0xfd,0x01);//RegPage 1
	u32Tmp = sensor_read_register(u32RegAddr);
	u32Tmp = u32Tmp&0xFC;
	u16OfstH = 0;
	if(u8Mirror)
	{
		u32Tmp |= 1<<u8MirrorBit;
		u16OfstH = 1;
	}
	if(u8Flip)
	{
		u32Tmp |= 1<<u8FlipBit;
	}
	else
	{
		u16OfstV = 0;
	}
	sensor_write_register(u32RegAddr, u32Tmp);
	sensor_write_register((u32UpdateReg>>8)&0xFF,u32UpdateReg&0xFF);
	VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfstV);
	u8BusyFlg = 0;
	return XM_SUCCESS;
}



STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;

	if(u8BusyFlg)
		return;
	u8BusyFlg = 1;
	sensor_write_register(0xfd,0x01);//RegPage	1
	sensor_write_register(0x03,(u32IntTime>>8)&0xff);	
	sensor_write_register(0x04,u32IntTime&0xff);
	sensor_write_register(0x01,0x01);

	if(u8BusyFlg != 2)
		u8BusyFlg = 0;
	return;
}


STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32Again = 0;
	static XM_U32 su32Dgain = 0;
	if(u8BusyFlg)
		return;
	u8BusyFlg = 1;
	
	if(su32Again != u32Again)
	{
		su32Again = u32Again;
		sensor_write_register(0xfd, 0x01); // RegPage 1  
		sensor_write_register(0x24, u32Again&0xFF);   
		sensor_write_register(0x01, 1);  
	}
	if(su32Dgain != u32Dgain)
	{
		su32Dgain = u32Dgain;
		sensor_write_register(0xfd, 0x01);
		sensor_write_register(0x35, u32Dgain&0xFF);
	}

	if(u8BusyFlg != 2)
		u8BusyFlg = 0;
	return;
}

const static XM_U8 gau8FpsSet_sp140A[3][3] = {
	//Reg  25  30
	{0x09,0x03,0x01},  
	{0x0A,0x73,0xFD},  
	{0x01,0x01,0x01},
};

/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U8 u8i,u8Idx,u8Num;
	const XM_U8 *pu8RegVal = NULL;
	XM_PRODUCT_INFO  stProductInfo;
	SysGetProductInfo(&stProductInfo);
	if(busyFlg_set() != XM_SUCCESS)
	{
		return ;
	}
	pu8RegVal = (const XM_U8*)gau8FpsSet_sp140A;
	u8Num = 3;
	switch(u8Fps)
	{
		case 25:
			//1920x750
			u8Idx = 1;
			break;
		case 30:
			//1600x750
			u8Idx = 2;
			break;
		default:
			u8BusyFlg = 0;
			return;
			break;
	}
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = HD720P_LINES;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-gu8MaxShutterOfst;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	sensor_write_register(0xfd,0x01);	//RegPage 
	for(u8i=0;u8i<u8Num;u8i ++)
	{
		sensor_write_register((XM_U32)*(pu8RegVal+3*u8i+0), (XM_U32)*(pu8RegVal+3*u8i+u8Idx));
	}
	if((stProductInfo.u8StdType == 2)&&(stProductInfo.u8Encoder == 1)) 
	{
		sensor_write_register(0x09,0x02);	//RegPage 
		sensor_write_register(0x0a,0x02);	//RegPage 
		sensor_write_register(0x01,0x01);	//RegPage 
	}
	gu8Fps = u8Fps;
	u8BusyFlg = 0;
	return;
}







XM_S32 cmos_init_sensor_exp_function_sp140A(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_sp140A(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= NULL;    
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_gain_calc_table_com;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_gain_calc_table_com;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}


#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sp140A =
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
	{0x88,0x88,0x86,0x84,0x80,0x60,0x38,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
	
	/* Blc	*/
	{0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101,0x101},

	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32},
};

const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sp140A = {
	1, HD720P_LINES-4,	// minInt   	maxInt
	16,255, 		// minAgain maxAgain
	64,255,				// minDgain 	maxDgain
	0xFFF,				// maxIspGain
	0x01010002,			// Update
	{700,700,700,700},
	0x00
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sp140A =
{
	5000,
	{
		0x0000,	370, -140, 26,
		0x0000,	-65, 367, -46,
		0x0000,	16, -197, 437
	},
	4000,
	{
		0x0000, 345, -127, 38,
		0x0000, -94, 340, 10,
		0x0000, -4, -271, 531
	},
	2800,
	{
		0x0000, 310, -73, 19,
		0x0000, -99, 322, 33,
		0x0000, -29, -385, 670
	}
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_sp140A = {
	{0, 0, 4096, 2605, 3714, 1948, 1009, 4096},
	{4096, 3734, 0, 0, 3391, 2585, 4096, 1253},
	{-1220, -4096, -1903, -4096, -4096, -4096, -2591, -3081},
	213,
	0, 1826, 0, 3160,
	{0, 871, 1222, 1542, 1659, 1799, 2054, 2092, 3160, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1410, 1024, 1462, 0}
};

static const XM_U8 gau8SnsInit_sp140A[][2] = {
	{0xfd,0x00},
	{0x2f,0x20},
	{0x2e,0x01},
	{0xfd,0x01},
	{0x06,0x05},//gwc
	{0x09,0x02},
	{0x0a,0x08},
	{0x03,0x00},//0xd
	{0x04,0xe0},//0x82
	{0x24,0xff},
	{0x35,0x40},
	{0x36,0x80},//0xff
	{0x37,0x80},//0xff
	{0x38,0x80},//0xff
	{0x39,0x80},//0xff
	{0x01,0x01},
	{0xfb,0x73},
	{0xf9,0x1c},
	{0xfd,0x01},
	{0x11,0x80},
	{0x13,0x40},
	{0x33,0x30},
	{0x16,0x18},
	{0x1c,0xed},
	{0x21,0x03},
	{0x22,0x47},
	{0x28,0x04},
	{0x29,0x0e},
	{0x2a,0x1d},
	{0x2c,0xe0},
	{0x30,0x01},
	{0xfd,0x01},
	{0x55,0x10},
	{0x66,0x56},
	{0x68,0x58},
	{0x58,0x20},
	{0x5b,0x02},
	{0x69,0x0a},
	{0x71,0x60},
	{0x72,0x0a},
	{0x74,0x60},
	{0x75,0x0a},//0x20
	{0x78,0x3c},
	{0x84,0x70},
	{0x8a,0x33},
	{0x8b,0x33},
	{0xb1,0x00},
	{0xac,0x00},
	{0xfd,0x02},
	{0x37,0x40},//0xf0
	{0x3a,0xff},
	{0xfd,0x00},
	{0x1b,0x00},
	{0xfd,0x01},
	{0xe7,0x03},
	{0xe7,0x00},
#if 0
	// Add @20171228
	{0x24,0x20},
	{0xf7,0x10},
	{0xfb,0x62},
	{0x11,0xa0},
	{0x22,0x77},
	//{0x55,0x30},
	{0x74,0x40},
#endif
};

XM_U32 sensor_getlist_sp140A(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau8SnsInit_sp140A)/sizeof(gau8SnsInit_sp140A[0]);
	return (XM_U32)gau8SnsInit_sp140A;
}

#endif

