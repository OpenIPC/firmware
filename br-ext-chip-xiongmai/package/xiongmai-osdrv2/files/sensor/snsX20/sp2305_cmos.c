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

volatile static XM_U8 u8BusyFlg = 0;	// 0:free 1:busy 2:stop
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
    XM_U16 u16OfstH = 0;
    XM_U16 u16OfstV = 0;
    if(busyFlg_set() != XM_SUCCESS)
    {
        return XM_FAILURE;
    }
    sensor_write_register(0xfd,0x01);//RegPage 1
    if((u8Mirror)&&(!u8Flip))//Mirror_On_Flip_Off
    {
        sensor_write_register(0x3f, 0x01);
        sensor_write_register(0xf8, 0x00);
        sensor_write_register(0x01, 0x01);
        sensor_write_register(0xfd, 0x02);
        sensor_write_register(0x62, 0x01);
        sensor_write_register(0x63, 0x00);
        u16OfstH = 1;
    }
    else if((u8Mirror)&&(u8Flip))//Mirror_On_Flip_On 
    {
        sensor_write_register(0x3f, 0x03);
        sensor_write_register(0xf8, 0x02);
        sensor_write_register(0x01, 0x01);
        sensor_write_register(0xfd, 0x02);
        sensor_write_register(0x62, 0x48);
        sensor_write_register(0x63, 0x04);
        u16OfstH = 1;
        u16OfstV = 1;
    }
    else if((!u8Mirror)&&(u8Flip))//Mirror_Off_Flip_On 
    {
        sensor_write_register(0x3f, 0x02);
        sensor_write_register(0xf8, 0x02);
        sensor_write_register(0x01, 0x01);
        sensor_write_register(0xfd, 0x02);
        sensor_write_register(0x62, 0x48);
        sensor_write_register(0x63, 0x04);
        u16OfstV = 1;
    }
    else//Mirror_Off_Flip_Off  
    {
        sensor_write_register(0x3f, 0x00);
        sensor_write_register(0xf8, 0x02);
        sensor_write_register(0x01, 0x01);
        sensor_write_register(0xfd, 0x02);
        sensor_write_register(0x62, 0x01);
        sensor_write_register(0x63, 0x00);
    }
    
    sensor_write_register(0xfd, 0x01); // RegPage 1  
    VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfstV);
    u8BusyFlg = 0;
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

    pstAeSnsDft->u32MaxAgain = 496;//15.5X
    pstAeSnsDft->u32MinAgain = 16;

    pstAeSnsDft->u32MaxDgain = 64;  
    pstAeSnsDft->u32MinDgain = 64;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;

    pstAeSnsDft->bDelayCfg = XM_FALSE;
    return 0;
}

static XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
    pstAeSnsGainInfo->u32GainDb = u32InTimes;
    pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
    return;
}

static XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("cmos_dgain_calc_table failed!\n");
		return;
	}
    pstAeSnsGainInfo->u32GainDb = u32InTimes;
    pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
	return;
}

STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
    static XM_U32 su32IntTime = 0xFFFFFFFF;
	if(su32IntTime == u32IntTime)
		return ;
    gu32ShutNow = u32IntTime;
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
        sensor_write_register(0x38,(u32Again>>8)&0x01);   
		sensor_write_register(0x01, 1);  
	}
	if(su32Dgain != u32Dgain)
	{
		su32Dgain = u32Dgain;
		//sensor_write_register(0xfd, 0x01);
		//sensor_write_register(0x39, u32Dgain&0xFF);
	}
	if(u8BusyFlg != 2)
		u8BusyFlg = 0;
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
            sensor_write_register(0xfd,0x01);//RegPage 
            sensor_write_register(0x09,0x00); 
            sensor_write_register(0x0a,0x4f); 
			DEBUG("cmos_fps_set SP2305 2M 30fps\n");
			break;
		case 25:
            sensor_write_register(0xfd,0x01);//RegPage 
            sensor_write_register(0x09,0x01); 
            sensor_write_register(0x0a,0x2b); 
			DEBUG("cmos_fps_set SP2305 2M 25fps\n");
			break;
		default:
            DEBUG("cmos_fps_set SP2305 2M 25fps\n");
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
	XM_U16 u16Vb;
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
    if(u8BusyFlg)
		return;
	u8BusyFlg = 1;
	u16Vb = (u16FullLines<1125)?0x14:u16FullLines-1105;
    sensor_write_register(0xfd,0x01);//RegPage 
	sensor_write_register(0x05,(u16Vb>>8)&0xFF);
	sensor_write_register(0x06,u16Vb&0xFF);
	sensor_write_register(0x01, 0x01);	
	u8BusyFlg = 0;
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}


XM_S32 cmos_init_sensor_exp_function_SP2305(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_SP2305(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_SP2305 =
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
	{0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107,0x107},
	/*Y_Tran gamma*/ 
	{0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x30,0x30,0x30,0x28,0x28,0x28,0x28,0x28}
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_SP2305 =
{
    5000,//0.1
	{
		0x0000, 287, -80, 49,
		0x0000, -21, 278, -1,
		0x0000, 42, -99, 313,
	},
	4000,//0.1
	{
		0x0000, 282, -98, 72,
		0x0000, -37, 264, 29,
		0x0000, 33, -146, 369,
	},
	2800,//0.1
	{
		0x0000, 224, -21, 53,
		0x0000, -97, 323, 30,
		0x0000, -64, -263, 583
	}
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_SP2305 ={
	{0, 0, 4096, 3063, 4096, 2254, 1026, 4096},
	{4096, 3984, 0, 0, 3644, 2638, 4096, 931},
	{-499, -4096, -1145, -4096, -2945, -4096, -1526, -1850},
	213,
	0, 1444, 0, 2572,
	{0, 749, 932, 1201, 1275, 1389, 1584, 1602, 2572, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1680, 1024, 1841, 0}
};



// ExtClk:27M
// 1080P30fps,12Bit,SPI,LVDS 2CH,ADC12Bit
// Nomal Mode register setting
static const XM_U8 gau8SnsInit_SP2305[][2] = {
#if 1
    
    {0xfd, 0x00},                                                       
    {0x20, 0x00}, 
    {0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 0x20},		// 100ms
    //;dela,y 3},ms                                                     
    {0xfd, 0x00},                                                       
    {0x2f, 0x21},//;10                                                  
    {0x34, 0x00},                                                       
    {0x30, 0x15},                                                       
    {0x33, 0x01},                                                       
    {0x35, 0x20},                                                       
    //;78 1b 00                                                         
    //;78 1d a5                                                         
    {0xfd, 0x01},                                                       
    {0x0d, 0x00},                                                       
    {0x30, 0x00},                                                       
    {0x03, 0x01},                                                       
    {0x04, 0x8f},                                                       
    {0x01, 0x01},                                                       
    {0x09, 0x00},                                                       
    {0x0a, 0x4f},//;20                                                  
    {0x06, 0x0a},                                                       
    {0x24, 0x10},                                                       
    {0x01, 0x01},                                                       
    {0xfb, 0x73},                                                       
    {0x01, 0x01},                                                       
    {0xfd, 0x01},                                                       
    {0x1a, 0x6b},                                                       
    {0x1c, 0xea},                                                       
    {0x16, 0x0c},                                                       
    {0x21, 0x00},                                                       
    {0x11, 0xe8},	//;63                                                   
    {0x19, 0xc3},                                                       
    {0x26, 0xda},                                                       
    {0x29, 0x01},                                                       
    {0x33, 0x6f},                                                       
    {0x2a, 0xd2},                                                       
    {0x2c, 0x40},                                                       
    {0xd0, 0x02},                                                       
    {0xd1, 0x01},                                                       
    {0xd2, 0x20},                                                       
    {0xd3, 0x03},	//;04                                                   
    {0xd4, 0xa4},	//;2a                                                   
    {0x50, 0x00},                                                       
    {0x51, 0x2c},                                                       
    {0x52, 0x29},                                                       
    {0x53, 0x00},                                                       
    {0x55, 0x44},                                                       
    {0x58, 0x29},                                                       
    {0x5a, 0x00},                                                       
    {0x5b, 0x00},                                                       
    {0x5d, 0x00},                                                       
    {0x64, 0x2f},                                                       
    {0x66, 0x62},                                                       
    {0x68, 0x5b},                                                       
    {0x75, 0x46},                                                       
    {0x76, 0xf0},                                                       
    {0x77, 0x4f},                                                       
    {0x78, 0xef},                                                       
    {0x72, 0xcf},                                                       
    {0x73, 0x36},                                                       
    {0x7d, 0x0d},                                                       
    {0x7e, 0x0d},                                                       
    {0x8a, 0x77},                                                       
    {0x8b, 0x77},                                                       
    {0xfd, 0x01},                                                       
    {0xb1, 0x83},	//;82;DPHY enable 8b                                    
    {0xb3, 0x09},	//;0d	;09;0b;09;1d                                      
    {0xb4, 0x15},	//;11;MIPI PLL enable;14;35;36                          
    {0x9d, 0x40},//;mipi hs dc level 40/03/55                             
    {0xa1, 0x05},//;speed/03                                              
    {0x94, 0x44},//;dphy time                                             
    {0x95, 0x33},//;dphy time                                             
    {0x96, 0x1f},//;dphy time                                             
    {0x98, 0x45},//;dphy time                                             
    {0x9c, 0x10},//;dphy time                                             
    {0xb5, 0x70},//;30                                                    
    {0xa0, 0x00},//;mipi enable                                           
    {0x25, 0xe0},                                                       
    {0x20, 0x7b},                                                       
    {0x8f, 0x88},                                                       
    {0x91, 0x40},                                                       
    {0xfd, 0x01},                                                       
    //;78 fd 02                                                         
    //;78 36 08	;invert Vsync polarity                                  
    //;78 5e 03                                                         
    {0xfd, 0x02},                                                       
    {0xa1, 0x04},                                                       
    {0xa3, 0x40},                                                       
    {0xa5, 0x02},                                                       
    {0xa7, 0xc4},                                                       
    {0xfd, 0x01},                                                       
    {0x86, 0x77},                                                       
    {0x89, 0x77},                                                       
    {0x87, 0x74},                                                       
    {0x88, 0x74},                                                       
    {0xfc, 0xe0},                                                       
    {0xfe, 0xe0},                                                       
    {0xf0, 0x40},                                                       
    {0xf1, 0x40},                                                       
    {0xf2, 0x40},                                                       
    {0xf3, 0x40},                                                       
    //;crop, to}, 1928x1088                                             
    {0xfd, 0x02},                                                 
    //;78 36 08	;invert Vsync polarity for Gaia                         
    {0xa0, 0x00},	//;Image vertical start MSB3bits                        
    {0xa1, 0x04},	//;Image vertical start LSB8bits                        
    {0xa2, 0x04},	//;image vertical size  MSB8bits                        
    {0xa3, 0x40},	//;image vertical size  LSB8bits                        
    {0xa4, 0x00},                                                       
    {0xa5, 0x02},	//;H start 8Lsb                                         
    {0xa6, 0x03},                                                       
    {0xa7, 0xc4},	//;Half H size Lsb8bits                                 
    {0xfd, 0x01},                                                       
    {0x8e, 0x07},                                                       
    {0x8f, 0x88},                                                       
    {0x90, 0x04},                                                       
    {0x91, 0x40},                                                       
    {0xfd, 0x03},                                                       
    {0xc0, 0x01},	//;enable transfer OTP BP information                   
    {0xfd, 0x04},                                                       
    {0x21, 0x14},                                                       
    {0x22, 0x14},                                                       
    {0x23, 0x14},	//;enhance normal and dummy BPC                         
    {0xfd, 0x01},                                                       
    {0x06, 0x14},	//;e0 insert dummy line , the frame rate is 30.01.      
    {0x01, 0x01},	                                                    
    //;78 fd 00                                                         
    //;78 1b 00	;enable output                                          
    {0xfd, 0x01},                                                   
    {0xa0, 0x01}, 
#else
    {0xfd, 0x00},                                                                       
    {0x20, 0x00},  
    {0xF5, 0xA5},		// SleepMask1
	{0x5A, 0x5F},		// SleepMask2
	{0x00, 0x05},		// 100ms
    //delay 3ms                                                                         
    {0xfd, 0x00},                                                                       
    {0x2f, 0x21},//10                                                                   
    {0x34, 0x00},                                                                       
    {0x30, 0x15},                                                                       
    {0x33, 0x01},                                                                       
    {0x35, 0x20},                                                                       
    //78 1b 00                                                                          
    {0x1d, 0xa5},                                                                       
    {0xfd, 0x01},                                                                       
    {0x0d, 0x00},                                                                       
    {0x30, 0x00},                                                                       
    {0x03, 0x01},                                                                       
    {0x04, 0x8f},                                                                       
    {0x01, 0x01},                                                                       
    {0x09, 0x01},//00                                                                   
    {0x0a, 0x2b},//20                                                                   
    {0x06, 0x0a},                                                                       
    {0x24, 0x10},                                                                       
    {0x01, 0x01},                                                                       
    {0xfb, 0x73},                                                                       
    {0x01, 0x01},                                                                       
    {0xfd, 0x01},                                                                       
    {0x1a, 0x6b},                                                                       
    {0x1c, 0xea},                                                                       
    {0x16, 0x0c},                                                                       
    {0x21, 0x00},                                                                       
    {0x11, 0xe8},   //63                                                                  
    {0x19, 0xc3},                                                                       
    {0x26, 0xda},                                                                       
    {0x29, 0x01},                                                                       
    {0x33, 0x6f},                                                                       
    {0x2a, 0xd2},                                                                       
    {0x2c, 0x40},                                                                       
    {0xd0, 0x02},                                                                       
    {0xd1, 0x01},                                                                       
    {0xd2, 0x20},                                                                       
    {0xd3, 0x03},   //04                                                                  
    {0xd4, 0xa4},   //2a                                                                  
    {0x50, 0x00},                                                                       
    {0x51, 0x2c},                                                                       
    {0x52, 0x29},                                                                       
    {0x53, 0x00},                                                                       
    {0x55, 0x44},                                                                       
    {0x58, 0x29},                                                                       
    {0x5a, 0x00},                                                                       
    {0x5b, 0x00},                                                                       
    {0x5d, 0x00},                                                                       
    {0x64, 0x2f},                                                                       
    {0x66, 0x62},                                                                       
    {0x68, 0x5b},                                                                       
    {0x75, 0x46},                                                                       
    {0x76, 0xf0},                                                                       
    {0x77, 0x4f},                                                                       
    {0x78, 0xef},                                                                       
    {0x72, 0xcf},                                                                       
    {0x73, 0x36},                                                                       
    {0x7d, 0x0d},                                                                       
    {0x7e, 0x0d},                                                                       
    {0x8a, 0x77},                                                                       
    {0x8b, 0x77},                                                                       
    {0xfd, 0x01},                                                                       
    {0xb1, 0x83},   //82//DPHY enable 8b                                                  
    {0xb3, 0x09},//0b//09//1d                                                           
    {0xb4, 0x15},//MIPI PLL enable//14//35//36                                          
    {0x9d, 0x40},//mipi hs dc level 40/03/55                                            
    {0xa1, 0x05},//speed/03                                                             
    {0x94, 0x44},//dphy time                                                            
    {0x95, 0x33},//dphy time                                                            
    {0x96, 0x1f},//dphy time                                                            
    {0x98, 0x45},//dphy time                                                            
    {0x9c, 0x10},//dphy time                                                            
    {0xb5, 0x70},//30                                                                   
    {0xa0, 0x00},//mipi enable                                                          
    {0x25, 0xe0},                                                                       
    {0x20, 0x7b},                                                                       
    {0x8f, 0x88},                                                                       
    {0x91, 0x40},                                                                       
    {0xfd, 0x01},                                                                       
    {0xfd, 0x02},                                                                       
    {0x36, 0x08},   //invert Vsync polarity                                               
    {0x5e, 0x03},                                                                       
    {0xfd, 0x02},                                                                       
    {0xa1, 0x04},                                                                       
    {0xa3, 0x40},                                                                       
    {0xa5, 0x02},                                                                       
    {0xa7, 0xc4},                                                                       
    {0xfd, 0x01},                                                                       
    {0x86, 0x77},                                                                       
    {0x89, 0x77},                                                                       
    {0x87, 0x74},                                                                       
    {0x88, 0x74},                                                                       
    {0xfc, 0xe0},                                                                       
    {0xfe, 0xe0},                                                                       
    {0xf0, 0x40},                                                                       
    {0xf1, 0x40},                                                                       
    {0xf2, 0x40},                                                                       
    {0xf3, 0x40},                                                                       
    //crop to 1928x1088                                                                 
    {0xfd, 0x02},                                                                       
    {0x36, 0x08},   //invert Vsync polarity for Gaia                                      
    {0xa0, 0x00},   //Image vertical start MSB3bits                                       
    {0xa1, 0x04},   //Image vertical start LSB8bits                                       
    {0xa2, 0x04},   //image vertical size  MSB8bits                                       
    {0xa3, 0x40},   //image vertical size  LSB8bits                                       
    {0xa4, 0x00},                                                                       
    {0xa5, 0x02},   //H start 8Lsb                                                        
    {0xa6, 0x03},                                                                       
    {0xa7, 0xc4},   //Half H size Lsb8bits                                                
                                                                                        
    {0xfd, 0x01},                                                                       
    {0x8e, 0x07},                                                                       
    {0x8f, 0x88},   //MIPI column number                                                  
    {0x90, 0x04},   //MIPI row number                                                     
    {0x91, 0x40},                                                                       
                                                                                        
    {0xfd, 0x03},                                                                       
    {0xc0, 0x01},   //enable transfer OTP BP information                                  
    {0xfd, 0x04},                                                                       
    {0x21, 0x14},                                                                       
    {0x22, 0x14},                                                                       
    {0x23, 0x14},   //enhance normal and dummy BPC                                        
                                                                                        
    {0xfd, 0x01},                                                                       
    {0x06, 0x14},   //e0 insert dummy line , the frame rate is 30.01.                     
    {0x01, 0x01},   //                                                                    
    //78 fd 00                                                                          
    //78 1b 00  //enable output                                                         
    {0xfd, 0x01},   //                                                                    
    {0xa0, 0x01},                                                                      
#if 1                                                                                        
    //@@ 3 31 Mirror_On_Flip_Off                                                          
    {0xfd, 0x01},                                                                       
                                                                                        
    {0x3f, 0x01}, //03                                                                    
    {0xf8, 0x00}, //02                                                                    
    {0x01, 0x01},                                                                       
    {0xfd, 0x02},                                                                       
    {0x62, 0x01},   //full size mode flip off row start for OTP BPC                       
    {0x63, 0x00},                                                                       
    {0xfd, 0x01},                                                                       
                                                                                        
    //@@ 3 32 Mirror_On_Flip_On                                                           
    {0xfd, 0x01},                                                                       
                                                                                        
    {0x3f, 0x03}, //03                                                                    
                                                                                        
    {0xf8, 0x02}, //02                                                                    
                                                                                        
    {0x01, 0x01},                                                                       
    {0xfd, 0x02},                                                                       
    {0x62, 0x48},                                                                       
    {0x63, 0x04},   //full size flip, row start:1096(0x448)                               
    {0xfd, 0x01},                                                                       
                                                                                        
    //@@ 3 33 Mirror_Off_Flip_Off                                                         
    {0xfd, 0x01},                                                                       
                                                                                        
    {0x3f, 0x00}, //03                                                                    
    {0xf8, 0x00}, //02                                                                    
                                                                                        
    {0x01, 0x01},                                                                       
    {0xfd, 0x02},                                                                       
    {0x62, 0x01},   //full size mode flip off row start for OTP BPC                       
    {0x63, 0x00},                                                                       
    {0xfd, 0x01},                                                                       
                                                                                        
    //@@ 3 34 Mirror_Off_Flip_On                                                          
    {0xfd, 0x01},                                                                       
                                                                                        
    {0x3f, 0x02}, //03                                                                    
    {0xf8, 0x02}, //02                                                                    
                                                                                        
    {0x01, 0x01},                                                                       
                                                                                        
    {0xfd, 0x02},                                                                       
    {0x62, 0x48},                                                                       
    {0x63, 0x04},   //full size flip, row start:1096(0x448)                               
    {0xfd, 0x01},                                                                       
#endif 
#endif
};

XM_U32 sensor_getlist_SP2305(XM_U16 *pu16Num)
{
    DEBUG("------------- SP2305 2M 25fps  init ok! (@20190909)----------------\n");
	*pu16Num = sizeof(gau8SnsInit_SP2305)/sizeof(gau8SnsInit_SP2305[0]);
	return (XM_U32)gau8SnsInit_SP2305;
}
#endif



