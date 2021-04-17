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
	XM_U32 u32Tmp = 0;
	if(u8Mirror)	u32Tmp |= 0x01;
	if(u8Flip)		u32Tmp |= 0x02;
	sensor_write_register(0x0101, u32Tmp);
	return XM_SUCCESS;
}

static const XM_U16 gau16GainTbl[81] =
{     
1024 , 1060 ,  1097 ,  1136 ,  1176,  1217 , 1260 ,  1304,  1350 ,  1397 ,  1446 ,  1497 , 1550 , 1604 ,  1661 ,  1719 ,  
1780 , 1842 ,  1907 ,  1974 ,  2043,  2115 , 2189 ,  2266,  2346 ,  2428 ,  2514 ,  2602 , 2693 , 2788 ,  2886 ,  2987 ,      
3092 , 3201 ,  3314 ,  3430 ,  3551,  3675 , 3805 ,  3938,  4077 ,  4220 ,  4368 ,  4522 , 4681 , 4845 ,  5015 ,  5192 ,      
5374 , 5563 ,  5758 ,  5961 ,  6170,  6387 , 6611 ,  6844,  7084 ,  7333 ,  7591 ,  7858 , 8134 , 8420 ,  8716 ,  9022 ,      
9339 , 9667 , 10007 , 10359 , 10723, 11099 ,11489 , 11893, 12311 , 12744 , 13192 , 13655 ,14135 ,14632 , 15146 , 
15678, 16229     
};


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

	if (u32InTimes >= gau16GainTbl[80]) 	  
	{			 
		pstAeSnsGainInfo->u32GainDb = 80;
	}
	else
	{
		for(i = 0x0; i <= 80; i++)		  
		{			
			if(u32InTimes <gau16GainTbl[i]) 		   
			{				 
				break;			  
			}			 
			pstAeSnsGainInfo->u32GainDb = i;;
		}	 
	}
	pstAeSnsGainInfo->u32SnsTimes = gau16GainTbl[pstAeSnsGainInfo->u32GainDb];
	return;
}







STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32Time = 0xFFFF;
	u32IntTime = gu16FullLines - u32IntTime;
	if(su32Time == u32IntTime)		return ;
	su32Time = u32IntTime;
	sensor_write_register(0x203, u32IntTime & 0xFF);	
	sensor_write_register(0x202, (u32IntTime & 0xFF00) >> 8);
	return;
}


STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32Gain = 0xFFFF;
	XM_U32 u32Gain;
	u32Gain = u32Again + u32Dgain;
	if(u32Gain > 0x96)	u32Gain = 0x96;
	if(u32Gain == su32Gain) 	return;
	su32Gain = u32Gain;
	sensor_write_register(0x301E, u32Gain);
	return;
}




/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32LineLength = 1320;
	switch(u8Fps)
	{
		case 30:
			// Change the frame rate via changing the vertical blanking
			u32LineLength = 1100;
			break;
		case 25:
		default:
			// Change the frame rate via changing the vertical blanking
			u32LineLength = 1320;
			break;
	}		
	pstAeSnsDft->u32MaxIntTime = gu16FullLines-gu8MaxShutterOfst;
	pstAeSnsDft->u32FullLinesStd = gu16FullLines;
	pstAeSnsDft->u32LinesPer500ms = gu16FullLines * u8Fps / 2;
	sensor_write_register(0x0342, (u32LineLength>>8)&0xFF);	//LINE_LENGTH				   
	sensor_write_register(0x0343, u32LineLength&0xFF);		//LINE_LENGTH	  
	u32LineLength = u32LineLength>>1;
	sensor_write_register(0x309A, u32LineLength&0xFF);			   
	sensor_write_register(0x309B, (u32LineLength>>8)&0xFF);	  //2200   

	return;
}





XM_S32 cmos_init_sensor_exp_function_imx323(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
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

XM_S32 cmos_init_ae_exp_function_imx323(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= NULL;    
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_gain_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_gain_calc_table;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}



#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_imx323 =
{
	/* bvalid */
	1,
	/* 100, 200, 400, 800, 1600, 3200, 6400, 12800; 100, 200, 400, 800, 1600, 3200, 6400, 12800 */
	//[0~7]:Normal						
	/* sharpen_D	H	*/
	{0x18,0x18,0x10,0x0C,0x0A,0x08,0x06,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
	
	/* sharpen_Ud M */
	{0x1A,0x18,0x14,0x10,0x0A,0x09,0x08,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},

	/* sharpen_Kd  */
	{0x2A,0x28,0x1A,0x18,0x14,0x0A,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	
	/* snr_thresh 2DNr		*/
	{0x00,0x02,0x06,0x10,0x18,0x28,0x30,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},

	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x06,0x0A,0x0E,0x12,0x15,0x17,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18},

	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x00,0x00,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},  

	/* DyDpc_thresh 		*/
	{0x97,0xA8,0xD0,0xD4,0xD8,0xDC,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0},
	
	/* saturation_strength */
	{0x90,0x90,0x89,0x80,0x78,0x6c,0x48,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
	
	/* Blc	*/
	{0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0},

	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x31,0x2F,0x2D,0x2A,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28},
};

const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_imx323 = {
	2, HD1080P_LINES-4,	// minInt   	maxInt
	1024,11489,				// minAgain	maxAgain
	1024,16229,				// minDgain     maxDgain
	0xFFF,				// maxIspGain
	0x01010002,			// Update
	{700,700,700,700},
	0x00
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_imx323 =
{
	5082,
	{
		0x0000, 373, -91, -26,
		0x0000, -50, 368, -62,
		0x0000, 32, -97, 321,
	},
	4400,
	{
		0x0000, 348, -76,  -16,
		0x0000, -104, 370, -10,
		0x0000, 48, -123, 331,
	},
	2800,
	{
		0x0000, 267, -23, 12,
		0x0000, -98, 359, -5,
		0x0000, 42, -182, 396,
	}
};
const ISP_AWB_CALIBRATION_V2_S gstAwbCal_imx323 = {
	{0, 0, 4096, 3269, 3256, 2570, 1421, 4096},
	{4096, 4096, 0, 0, 4096, 3674, 4096, 1632},
	{-600, -3104, -1556, -4096, -3396, -4096, -2054, -2608},
	213,
	0, 1301, 0, 2350,
	{0, 475, 683, 919, 1044, 1230, 1400, 1488, 2350, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2500, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1703, 1024, 1898, 0}
};

// ExtClk: 37.125M
static const XM_U16 gau16SnsInit_imx323[][2] = {
	{0x3000, 0x31}, 	   //TESTEN 					
	{0x0008, 0x00}, 	   //I2CBLKLEVEL				  
	{0x0009, 0xF0}, 	   //I2CBLKLEVEL				  
	{0x0112, 0x0C}, 	   //RAW_FMT					  
	{0x0113, 0x0C}, 	   //RAW_FMT				 
	{0x0340, 0x04}, 	   //FRM_LENGTH 				  
	{0x0341, 0x65}, 	   //FRM_LENGTH
	{0x0342, 0x05}, 	   //LINE_LENGTH				  
	{0x0343, 0x28}, 	   //LINE_LENGTH	  
	{0x3002, 0x0F}, 	   //MODE 
	{0x3011, 0x00}, 	   //FRSEL/OPORSEL/M12BEN		  
	{0x3012, 0x82}, 	   //ADRES:12bit				
	{0x3013, 0x40}, 	   
	{0x3016, 0x3C}, 	   //WINPV	
	{0x301A, 0x51},
	{0x301F, 0x73},
	{0x3022, 0x40}, 	   //720PMODE:1080p mode   
	{0x3027, 0x20},
	// 25fps
	{0x309A, 0x94}, 			  
	{0x309B, 0x02},   //2640		
	{0x3117, 0x0D}, 	   // 
	{0x302C, 0x00},
	{0x0100, 0x01}, 	   //Standby cancel
//	{0x0202, 0x05},
	{0x30AF, 0x0A},
	{0x303F, 0x0A},
	{0x301E, 0x00},
	{0x202, 0x02},
	{0x203, 0x32}
};

XM_U32 sensor_getlist_imx323(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau16SnsInit_imx323)/sizeof(gau16SnsInit_imx323[0]);
	return (XM_U32)gau16SnsInit_imx323;
}

#endif

