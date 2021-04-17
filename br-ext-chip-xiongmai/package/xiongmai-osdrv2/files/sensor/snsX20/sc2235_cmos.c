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
	XM_U16 u16OfstV;

	u32Val = sensor_read_register(0x3221);
	
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
		u16OfstV = 0;
	}
	else
	{
		u32Val &= ~0x60;
		u16OfstV = 0;
	}
	sensor_write_register(0x3221, u32Val);
	VI_WinSet(1, u8Mirror, u8Flip, 0, u16OfstV);
	XM_MPI_MIPI_RefreshFV(100, 0);
	return XM_SUCCESS;
}

XM_VOID cmos_again_calc_table_sc2235(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
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
	pstAeSnsGainInfo->u32SnsTimes = (XM_U32)gau16GainTbl_SmatSns[pstAeSnsGainInfo->u32GainDb];
	return;
}

void shutLogic_sc2235(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	XM_U32 u32Tmp,u32Val1,u32Val2;
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;

	u32Val1 = 0x12;
	u32Val2 = 0x02;
	if(gSensorChip == SENSOR_CHIP_SC2235P)
	{
		u32Val1 = 0x14;
		u32Val2 = 0x04;	
	}
	else
	{
		sensor_write_register(0x3903, 0x84);
		sensor_write_register(0x3903, 0x04);
	}
	u32Tmp = ((u32IntTime&0x0ff0)>>4);
	if(u32Tmp < 0x05)
		sensor_write_register(0x3314,u32Val1);
	else if(u32Tmp > 0x0A)
		sensor_write_register(0x3314,u32Val2);
}


XM_VOID cmos_inttime_update_sc2235(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	XM_U32 u32Tmp;
	gu32ShutNow = u32IntTime;
	if((gSensorChip == SENSOR_CHIP_SC2235P)||(gSensorChip == SENSOR_CHIP_SC2145H))
	{
		u32IntTime = u32IntTime*2;
	}
	if(pfn_shutLogic)
	{
		(*pfn_shutLogic)(u32IntTime);
	}
	
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;
	u32Tmp = ((u32IntTime&0x0ff0)>>4);
	sensor_write_register(0x3e00, (u32IntTime>>12)&0x0f);
	sensor_write_register(0x3e01, u32Tmp);
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;
}


const static XM_U16 gau16LogicAddr[3] = {
0x3631, 0x3301, 0x366f
};
const static XM_U8 gau8Logic_sc2235[3][4][3] = {
	//  0x3631  0x3301  0x366f
	//CVBS PAL
	{
		{0x84, 0x05, 0x2f},
		{0x88, 0x15, 0x2f},
		{0x88, 0x19, 0x2f},
		{0x88, 0xff, 0x3a},
	},
	//CVBS NTSC
	{
		{0x84, 0x05, 0x2f},
		{0x88, 0x15, 0x2f},
		{0x88, 0x19, 0x2f},
		{0x88, 0xff, 0x3c},
	},
	//AHD
	{
		{0x84, 0x05, 0x2f},
		{0x88, 0x15, 0x2f},
		{0x88, 0x19, 0x2f},
		{0x88, 0xff, 0x3c},
	}
};

void gainLogic_sc2235(XM_U32 u32AllGain)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Vstd   [bit4~bit7]:Agc
	XM_U8 u8Idx,u8Idx2,u8i;
	if(gu8Fps==50)		u8Idx = 0;
	else if(gu8Fps==60) 	u8Idx = 1;
	else					u8Idx = 2;

	if(u32AllGain<32){
		u8Idx2 = 0;
	}
	else if(u32AllGain<128){
		u8Idx2 = 1;
	}
	else if(u32AllGain<248){
		u8Idx2 = 2;
	}
	else{
		u8Idx2 = 3;
	}
	if(	((su8Idx&0x0F) != u8Idx) ||
			((su8Idx>>4)&0x0F) != u8Idx2)
	{
			su8Idx = ((u8Idx2&0x0F)<<4)|(u8Idx&0x0F);
			sensor_write_register(0x3812, 0x00);
			for(u8i=0; u8i<3; u8i ++)
			{
				sensor_write_register((XM_U32)gau16LogicAddr[u8i], (XM_U32)gau8Logic_sc2235[u8Idx][u8Idx2][u8i]);
			}
			sensor_write_register(0x3812, 0x30);
	}
}
extern XM_U16 gu16ObTemp;	// ÎÂ¶È²ÎÊý(ObÓ³Éä)

static XM_S32 sensorOb_task()
{
	XM_U32 u32Tmp = 0;
	u32Tmp = sensor_read_register(0x3911);
	u32Tmp = (u32Tmp<<8) | sensor_read_register(0x3912);
	gu16ObTemp = (XM_U16)u32Tmp;

	return XM_SUCCESS;
}

XM_VOID cmos_gains_update_sc2235(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	static XM_U8 su8Reg0x5785 = 0;
	XM_U8 u8DpcFlg,u8BlcFlg;
	XM_U32 u32Tmp;
	unsigned int tmp[2];
	
    sensorOb_task();
	u8DpcFlg = 1;
	u8BlcFlg = 1;
	u32Again = (u32Again>63)?63:u32Again;
	if((gu32AGainNow !=0 ) && (gu32AGainMin != 0))
	{
		gau32AllGain = gu32AGainNow*16 / gu32AGainMin;
	}
	else
	{
		gau32AllGain = gau16GainTbl_SmatSns[u32Again] / 64;
	}
	// gainLogic
	if(gSensorChip == SENSOR_CHIP_SC2145H)
	{
		u8DpcFlg = 0;
	}
	else if(gSensorChip == SENSOR_CHIP_SC2235P)
	{
		//u8DpcFlg = 0;
		u8BlcFlg = 0;
	}
	else if(gSensorChip == SENSOR_CHIP_SC1235)
	{
		u32Dgain = (u32Dgain>4)?4:u32Dgain; 
		if((gu32DGainNow !=0 ) && (gu32DGainMin != 0))
		{
			gau32AllGain = gau32AllGain*gu32DGainNow/gu32DGainMin;
		}
		else
		{
			gau32AllGain = gau32AllGain*u32Dgain;
		}
	}
	if(pfn_gainLogic)
	{
		(*pfn_gainLogic)(gau32AllGain);
	}
	if(gu8BlcDpcFlg)
	{
		u8BlcFlg = (gu8BlcDpcFlg>>4) &0x01;
		u8DpcFlg = (gu8BlcDpcFlg>>5) &0x01;
#ifdef DEBUG_ON
		DBG("BlcLogic:  ");
		PrintInt(8, (XM_U32)u8BlcFlg); ENTER();
		DBG("DpcLogic:  ");
		PrintInt(8, (XM_U32)u8DpcFlg); ENTER();
#endif

	}
	if((su32AGain != u32Again) || (su32DGain != u32Dgain))
	{
		su32AGain = u32Again;
		su32DGain = u32Dgain;
		// 1.BL
		if(u8BlcFlg)
		{
			sensor_write_register(0x3903, 0x84);
			sensor_write_register(0x3903, 0x04);
		}

		// 2.GainUpdate
		tmp[0] = (u32Again&0x0F) | 0x10;	
		u32Tmp = 0x03;
		u32Again = u32Again/16;
		for(;u32Again>0;u32Again--)
		{
			u32Tmp = (u32Tmp<<1)|0x01;
		}
		if(gSensorChip == SENSOR_CHIP_SC1235)
		{
			switch(u32Dgain)
			{
					case 2:	u32Tmp |= 0x20;
							break;
					case 4:	u32Tmp |= 0x60;
							break;
			}
		}
		tmp[1] = u32Tmp;
		sensor_write_register(0x3e08, tmp[1]);
		sensor_write_register(0x3e09, tmp[0]);

		// 3.DPC
		if(u8DpcFlg)
		{
			if(gau32AllGain<160)
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
	static XM_U16 su16Temp = 0xFFFF;
    if((gu16ObTemp != su16Temp)&&(gSensorChip == SENSOR_CHIP_SC2235P))
	{
        //5¡¢HighTemp
		if(gu16ObTemp <= 0x105B)
            sensor_write_register(0x3908, 0x11);
        else if(gu16ObTemp <= 0x1090)
            sensor_write_register(0x3908, 0x14);
        else if(gu16ObTemp <= 0x1120)
            sensor_write_register(0x3908, 0x16);
        else 
            sensor_write_register(0x3908, 0x18);
        su16Temp = gu16ObTemp;
	}
	return;
}

XM_VOID cmos_fps_set_sc2235(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
//	XM_U32 u32Tmp,u32ExpNow,;
	XM_U32 u32TotalSizeV,u32Pixs=2640;
//	XM_U8 u8Num,u8i,u8Idx = 2;
//	const XM_U16 *pau16RegAddr;
//	const XM_U8 *pau8RegVal;

//	u32ExpNow = sensor_read_register(0x3e01);
//	u32Tmp = sensor_read_register(0x3e02);
//	u32ExpNow = ((u32ExpNow<<4)|((u32Tmp>>4)&0x0F));
	u32TotalSizeV = HD1080P_LINES;
	switch(u8Fps)
	{
		case 30:
			u32Pixs = 2200;
			break;
		case 25:
			u32Pixs = 2640;
			break;
		default:
			//ERR("cmos_fps_set: fps err!\n");
			return;
			break;
	}		

	if(pstAeSnsDft != NULL) 
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-gu8MaxShutterOfst;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
//	if(u32ExpNow>u32TotalSizeV-gu8MaxShutterOfst)
//		cmos_inttime_update_sc2235(u32TotalSizeV-gu8MaxShutterOfst);

	sensor_write_register(0x320c, (u32Pixs>>8)&0xFF);
	sensor_write_register(0x320d, u32Pixs&0xFF);
	sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);
	gu8Fps = u8Fps;
	return;
}







XM_S32 cmos_init_sensor_exp_function_sc2235(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
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

XM_S32 cmos_init_ae_exp_function_sc2235(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_set			= cmos_fps_set_sc2235;
	pstExpFuncs->pfn_cmos_slow_framerate_set= NULL;    
	pstExpFuncs->pfn_cmos_inttime_update	= cmos_inttime_update_sc2235;
	pstExpFuncs->pfn_cmos_gains_update		= cmos_gains_update_sc2235;
	pstExpFuncs->pfn_cmos_again_calc_table	= cmos_again_calc_table_sc2235;
	pstExpFuncs->pfn_cmos_dgain_calc_table	= cmos_gain_calc_table_com;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}


#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc2235 =
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	//[0~7]:Normal						
	/* sharpen_D	H	*/
	{0x18,0x18,0x18,0x18,0x18,0x14,0x10,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	
	/* sharpen_Ud M */
	{0x1C,0x1C,0x1C,0x1C,0x1C,0x18,0x10,0x10,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},

	/* sharpen_Kd  */
	{0x2C,0x2C,0x2C,0x2C,0x28,0x18,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	
	/* snr_thresh 2DNr		*/
	{0x00,0x01,0x04,0x10,0x20,0x28,0x30,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},

	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x06,0x0A,0x0E,0x12,0x15,0x17,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18},

	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x00,0x00,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},  

	/* DyDpc_thresh 		*/
	{0x00,0x00,0x00,0x00,0x00,0xC0,0xD0,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8},
	
	/* saturation_strength */
	{0x99,0x99,0x96,0x90,0x88,0x78,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
	
	/* Blc	*/
	{0x4A,0x46,0x44,0x44,0x40,0x3E,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38},

	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x31,0x2F,0x2D,0x2A,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28},
};

const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sc2235 = {
	2, HD1080P_LINES-4,	// minInt   	maxInt
	1024,15872, 		// minAgain maxAgain
	128,128,				// minDgain 	maxDgain
	0xFFF,				// maxIspGain
	0x01010002,			// Update
	{700,700,700,700},
	0x00
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc2235 =
{
	5000,
	{
		0x000,332, -58, -18,
		0x000,-50, 391, -85,
		0x000,	5, -100, 351
	},
	4000,
	{
		0x000,327, -72,  1,
		0x000,-46, 362,-60,
		0x000, -2,-149,407
	},
	2800,
	{
		0x000, 265,  24,-33,
		0x000,-102, 374,-16,
		0x000, -54,-272,582
	}
};
const ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2235 = {
    {0, 0, 4096, 2564, 3732, 1900, 1059, 4096},
    {4096, 3827, 0, 0, 2755, 2723, 4096, 928},
    {-1392, -4096, -1912, -4096, -4096, -4096, -2768, -2784},
    213,
    0, 1843, 0, 3236,
    {0, 928, 1215, 1588, 1712, 1895, 2112, 2221, 3236, 0, 0, 0, 0, 0, 0, 0},
    {2000, 2500, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
    {1388, 1024, 1275, 0} 
};

// ExtClk: 37.125M
static const XM_U16 gau16SnsInit_sc2235[][2] = {
	{0x0100,0x00}, 
	{0x3621,0x28}, 
	{0x33b5,0x10}, 
	{0x3303,0x20}, 
	{0x331e,0x0d}, 
	{0x3320,0x0f}, 
	{0x3633,0x42}, 
	{0x3634,0x42}, 
	{0x320c,0x0A}, 
	{0x320d,0x50}, 
	{0x3364,0x05}, 
	{0x4500,0x59}, 
	{0x3d08,0x02},//0x00 
	{0x3908,0x11}, 
	{0x3e01,0x46}, 
	{0x3381,0x0a}, 
	{0x3380,0x04}, 
	{0x3340,0x06}, 
	{0x3341,0x50}, 
	{0x3632,0x88}, 
	{0x3309,0xa0}, 
	{0x331f,0x8d}, 
	{0x3321,0x8f}, 
	{0x335e,0x01}, 
	{0x335f,0x03}, 
	{0x337c,0x04}, 
	{0x337d,0x06}, 
	{0x33a0,0x05}, 
	{0x3301,0x05}, 
	{0x3670,0x08}, 
	{0x367e,0x07}, 
	{0x367f,0x0f}, 
	{0x3677,0x2f}, 
	{0x3679,0x43}, 
	{0x337f,0x03}, 
	{0x3368,0x02}, 
	{0x3369,0x00}, 
	{0x336a,0x00}, 
	{0x336b,0x00}, 
	{0x3367,0x08}, 
	{0x330e,0x30}, 
	{0x3366,0x7c}, 
	{0x3635,0xc1}, 
	{0x363b,0x09}, 
	{0x363c,0x07}, 
	{0x391e,0x00}, 
	{0x3637,0x14}, 
	{0x366e,0x08}, 
	{0x366f,0x2f}, 
	{0x3631,0x84}, 
	{0x3630,0x48}, 
	{0x3622,0x06}, 
	{0x3638,0x1f}, 
	{0x3625,0x02}, 
	{0x3636,0x24}, 
	{0x3e03,0x0b}, 
	{0x3802,0x01}, 
	{0x3235,0x04}, 
	{0x3236,0x63}, 
	{0x3348,0x07}, 
	{0x3349,0x80}, 
	{0x391b,0x4d}, 
	{0x3342,0x04}, 
	{0x3343,0x20}, 
	{0x334a,0x04}, 
	{0x334b,0x20}, 
	{0x3222,0x29}, 
	{0x3901,0x02}, 
	{0x3f00,0x07}, 
	{0x3f04,0x08}, 
	{0x3f05,0x74}, 
	{0x3306,0x4a}, 
	{0x330b,0xca}, 
	{0x3639,0x09}, 
	{0x3039,0x31}, 
	{0x303a,0xc6}, 
	{0x3034,0x01}, 
	{0x3035,0xc2}, 
	{0x3208,0x07}, 
	{0x3209,0x8a}, 
	{0x320a,0x04}, 
	{0x320b,0x42}, 
	{0x3204,0x07}, 
	{0x3205,0x8e}, 
	{0x3206,0x04}, 
	{0x3207,0x47}, 
	{0x3213,0x02}, 
	{0x3211,0x02}, 
	{0x5780,0xff}, 
	{0x5781,0x04}, 
	{0x5785,0x18}, 
	{0x3313,0x05}, 
	{0x3678,0x42}, 
	
	{0x3d08, 0x03},
	{0x3640, 0x02},
	{0x3641, 0x02},

	{0x0100,0x01}, 
};

XM_U32 sensor_getlist_sc2235(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau16SnsInit_sc2235)/sizeof(gau16SnsInit_sc2235[0]);
	return (XM_U32)gau16SnsInit_sc2235;
}

#endif