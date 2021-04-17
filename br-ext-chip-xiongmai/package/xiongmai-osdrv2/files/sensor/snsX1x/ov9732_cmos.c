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
STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U16 u16OfstV;
	XM_U32 u32MirrorFlip;
	u16OfstV = 0;
	u32MirrorFlip = sensor_read_register(0x3820);
	if(u8Mirror)
	{
		u32MirrorFlip |= 0x08;
	}
	else
	{
		u32MirrorFlip &= 0xF7;
	}
		
	if(u8Flip)
	{
		u32MirrorFlip |= 0x04;
		u16OfstV = 0;
	}
	else
	{
		u32MirrorFlip &= 0xFB;
		u16OfstV = 0;
	}
	sensor_write_register(0x3820, u32MirrorFlip);
	
	VI_WinSet(1, u8Mirror, u8Flip, 0, u16OfstV);
	XM_MPI_MIPI_RefreshFV(100, 0);
	return XM_SUCCESS;
	
}


static const XM_U16 gau16GainTbl[64] ={
1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048,2176,2304,2432,2560,2688,2816,2944,
3072,3200,3328,3456,3584,3712,3840,3968,4096,4352,4608,4864,5120,5376,5632,5888,6144,6400,6656,6912,7168,7424,7680,7936,
8192,8704,9216,9728,10240,10752,11264,11776,12288,12800,13312,13824,14336,14848,15360,15872
};

STATIC XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
    int i;
    if(XM_NULL == pstAeSnsGainInfo)
    {
        //ERR("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    //Table
    if(u32InTimes >= gau16GainTbl[63])
    {
        pstAeSnsGainInfo->u32GainDb = 63;
    }
    else
    {
        pstAeSnsGainInfo->u32GainDb = 0;
        for(i = 0x1; i <= 63; i++)        
        {           
            if(u32InTimes <gau16GainTbl[i])            
            {                
                pstAeSnsGainInfo->u32GainDb = i-1;
                break;            
            }            
        }

    }
    pstAeSnsGainInfo->u32SnsTimes = gau16GainTbl[pstAeSnsGainInfo->u32GainDb];
    return;
}


STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U16 su16IntT = 0xFFFF;
	if(su16IntT == u32IntTime)	return;
	su16IntT = (XM_U16)u32IntTime;
	gu32ShutNow = u32IntTime;
	sensor_write_register(0x3501, (u32IntTime&0x0ff0)>>4);
	sensor_write_register(0x3502, (u32IntTime&0xf)<<4);
	return;
}

STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U8 su8Cnt = 0;
	static XM_U8 su8Again = 0xFF;
	static XM_U32 su32Dgain = 0xFFFF;
	XM_U8 u8Flg = 0;
	u32Again = (u32Again>63)? 63:u32Again;	

	if(su8Again != u32Again || su32Dgain != u32Dgain)
	{
		u8Flg = 1;
		sensor_write_register(0x3208, 0x00);
	}
	if(su8Again != u32Again)
	{
		su8Again = u32Again;

		if(u32Again < 16)		u32Again += 0x10;
		else if(u32Again < 32) u32Again = (u32Again-0x10)*2+0x20;
		else if(u32Again < 48) u32Again = (u32Again-0x20)*4+0x40;
		else if(u32Again < 64) u32Again = (u32Again-0x30)*8+0x80;
		else u32Again = 0xF8;
		sensor_write_register(0x350b, u32Again);
	}
	if(su32Dgain != u32Dgain)
	{
		su32Dgain = u32Dgain;

		sensor_write_register(0x5180, (u32Dgain>>8)&0xFF);
		sensor_write_register(0x5182, (u32Dgain>>8)&0xFF);
		sensor_write_register(0x5184, (u32Dgain>>8)&0xFF);

		sensor_write_register(0x5181, u32Dgain&0xFF);
		sensor_write_register(0x5183, u32Dgain&0xFF);
		sensor_write_register(0x5185, u32Dgain&0xFF);
	}
	if(u8Flg)
	{
		sensor_write_register(0x3208, 0x10);
		sensor_write_register(0x3208, 0xA0);
	}
#if 1
	if(++ su8Cnt > 100)
	{
		su8Cnt = 0;
		sensor_write_register(0x4010, 0xF4);
		sensor_write_register(0x4010, 0xF0);	
	}
#endif
	return;
}

/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32TotalSizeH = 1920;
	switch(u8Fps)
	{
		case 30:
			u32TotalSizeH = 1600;
			break;
		case 25:
		default:
			u32TotalSizeH = 1920;
			break;
	}		
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = gu16FullLines;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-gu8MaxShutterOfst;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	sensor_write_register(0x380c,(u32TotalSizeH>>8)&0xFF);//HTS 1600*750
	sensor_write_register(0x380d,u32TotalSizeH&0xFF);
	//sensor_write_register(0x3803,u32StartV);
	//sensor_write_register(0x3807,u32StartV+0xD9);
	gu8Fps = u8Fps;
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
    sensor_write_register(0x380e,(u16FullLines>>8)&0xFF);
    sensor_write_register(0x380f,u16FullLines&0xFF);

	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
	return;
}


XM_S32 cmos_init_sensor_exp_function_OV9732(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
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

XM_S32 cmos_init_ae_exp_function_OV9732(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;    
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_gain_calc_table_com;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}


#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_OV9732 =
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

const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_OV9732 = {
	1, HD720P_LINES-4,	// minInt   	maxInt
	1024,15872, 		// minAgain maxAgain
	1024,1024,				// minDgain 	maxDgain
	0xFFF,				// maxIspGain
	0x01000002,			// Update
	{700,700,700,700},
	0x00
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_OV9732 = {
	5000,
	{
		0x000,286, -23, -7,
		0x000,-33, 312, -23,
		0x000, 29,-98,325
	},
	4000,
	{
		0x000,275, -51, 32,
		0x000,-52, 311, -3,
		0x000, 20,-143,379
	},
	2800,
	{
		0x000, 197, 76,-17,
		0x000,-74, 322, 8,
		0x000,-52,-203,511 
	}
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_OV9732 = {
	{0, 0, 4096, 3144, 4096, 2469, 3205, 4096}, 
	{4096, 4096, 0, 0, 3324, 2766, 4096, 880}, 
	{-700, -4011, -1473, -4096, -3421, -4096, -3281, -2192}, 
	213,
	0, 1337, 0, 2572,
	{0, 673, 899, 1141, 1234, 1390, 1552, 1568, 2572, 0, 0, 0, 0, 0, 0, 0}, 
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0}, 
	{1798, 1024, 1649, 0}
};

static const XM_U16 gau16SnsInit_OV9732_mipi[][2] = {
    {0x0103, 0x01}, 
    {0x00F5,0xA5},      // SleepMask1
    {0x005A,0x5F},      // SleepMask2
    {0x0000,0x64},      // 100ms
    {0x0100, 0x00}, 
    {0x3001, 0x00}, 
    {0x3002, 0x00}, 
    {0x3007, 0x1f}, 
    {0x3008, 0xff}, 
    {0x3009, 0x02}, 
    {0x3010, 0x00}, 
    {0x3011, 0x08}, 
    {0x3014, 0x22}, 
    {0x301e, 0x15}, 
    {0x3030, 0x19}, 
    {0x3080, 0x02}, 
    {0x3081, 0x3c}, 
    {0x3082, 0x04}, 
    {0x3083, 0x00}, 
    {0x3084, 0x02}, 
    {0x3085, 0x01}, 
    {0x3086, 0x01}, 
    {0x3089, 0x01}, 
    {0x308a, 0x00}, 
    {0x3103, 0x01}, 
    {0x3600, 0xf6}, 
    {0x3601, 0x72}, 
    {0x3605, 0x66}, 
    {0x3610, 0x0c}, 
    {0x3611, 0x60}, 
    {0x3612, 0x35}, 
    {0x3654, 0x10}, 
    {0x3655, 0x77}, 
    {0x3656, 0x77}, 
    {0x3657, 0x07}, 
    {0x3658, 0x22}, 
    {0x3659, 0x22}, 
    {0x365a, 0x02}, 
    {0x3700, 0x1f}, 
    {0x3701, 0x10}, 
    {0x3702, 0x0c}, 
    {0x3703, 0x0b}, 
    {0x3704, 0x3c}, 
    {0x3705, 0x51}, 
    {0x370d, 0x20}, 
    {0x3710, 0x0d}, 
    {0x3782, 0x58}, 
    {0x3783, 0x60}, 
    {0x3784, 0x05}, 
    {0x3785, 0x55}, 
    {0x37c0, 0x07}, 
    {0x3800, 0x00}, 
    {0x3801, 0x03}, //;04
    {0x3802, 0x00}, 
    {0x3803, 0x03}, //;04
    {0x3804, 0x05}, 
    {0x3805, 0x0c}, //;0b
    {0x3806, 0x02}, 
    {0x3807, 0xdc}, //;db
    {0x3808, 0x05}, 
    {0x3809, 0x08}, //;00
    {0x380a, 0x02}, 
    {0x380b, 0xd8}, //;d0
    {0x380c, 0x07}, //;05
    {0x380d, 0x80}, //;c6
    {0x380e, 0x02}, //;03
    {0x380f, 0xee}, //;22
    {0x3810, 0x00}, 
    {0x3811, 0x01}, //;04
    {0x3812, 0x00}, 
    {0x3813, 0x01}, //;04
    {0x3816, 0x00}, 
    {0x3817, 0x00}, 
    {0x3818, 0x00}, 
    {0x3819, 0x04}, 
    {0x3820, 0x10}, 
    {0x3821, 0x00}, 
    {0x382c, 0x06}, 
    {0x3500, 0x00}, 
    {0x3501, 0x31}, 
    {0x3502, 0x00}, 
    {0x3503, 0x03}, 
    {0x3504, 0x00}, 
    {0x3505, 0x00}, 
    {0x3509, 0x10}, 
    {0x350a, 0x00}, 
    {0x350b, 0x40}, 
    {0x3d00, 0x00}, 
    {0x3d01, 0x00}, 
    {0x3d02, 0x00}, 
    {0x3d03, 0x00}, 
    {0x3d04, 0x00}, 
    {0x3d05, 0x00}, 
    {0x3d06, 0x00}, 
    {0x3d07, 0x00}, 
    {0x3d08, 0x00}, 
    {0x3d09, 0x00}, 
    {0x3d0a, 0x00}, 
    {0x3d0b, 0x00}, 
    {0x3d0c, 0x00}, 
    {0x3d0d, 0x00}, 
    {0x3d0e, 0x00}, 
    {0x3d0f, 0x00}, 
    {0x3d80, 0x00}, 
    {0x3d81, 0x00}, 
    {0x3d82, 0x38},// 
    {0x3d83, 0xa4},// 
    {0x3d84, 0x00}, 
    {0x3d85, 0x00}, 
    {0x3d86, 0x1f}, //
    {0x3d87, 0x03}, //
    {0x3d8b, 0x00}, 
    {0x3d8f, 0x00}, 
    {0x4001, 0xe0},// 
    {0x4004, 0x00}, 
    {0x4005, 0x02}, 
    {0x4006, 0x01}, 
    {0x4007, 0x40}, 
    {0x4009, 0x0b}, 
    {0x4300, 0x03}, 
    {0x4301, 0xff}, 
    {0x4304, 0x00}, 
    {0x4305, 0x00}, 
    {0x4309, 0x00}, 
    {0x4600, 0x00}, 
    {0x4601, 0x04}, 
    {0x4800, 0x00}, 
    {0x4805, 0x00}, 
    {0x4821, 0x50}, 
    {0x4823, 0x50}, 
    {0x4837, 0x2d}, 
    {0x4a00, 0x00}, 
    {0x4f00, 0x80}, 
    {0x4f01, 0x10}, 
    {0x4f02, 0x00}, 
    {0x4f03, 0x00}, 
    {0x4f04, 0x00}, 
    {0x4f05, 0x00}, 
    {0x4f06, 0x00}, 
    {0x4f07, 0x00}, 
    {0x4f08, 0x00}, 
    {0x4f09, 0x00}, 
    {0x5000, 0x07}, 
    {0x500c, 0x00}, 
    {0x500d, 0x00}, 
    {0x500e, 0x00}, 
    {0x500f, 0x00}, 
    {0x5010, 0x00}, 
    {0x5011, 0x00}, 
    {0x5012, 0x00}, 
    {0x5013, 0x00}, 
    {0x5014, 0x00}, 
    {0x5015, 0x00}, 
    {0x5016, 0x00}, 
    {0x5017, 0x00}, 
    {0x5080, 0x00}, 
    {0x5180, 0x01}, 
    {0x5181, 0x00}, 
    {0x5182, 0x01}, 
    {0x5183, 0x00}, 
    {0x5184, 0x01}, 
    {0x5185, 0x00}, 
    {0x5708, 0x06}, 
    {0x5781, 0x0e}, 
    {0x5783, 0x0f}, 
    {0x3603, 0x70}, 
    {0x3620, 0x1e}, 
    {0x400a, 0x01}, 
    {0x400b, 0xc0}, 
    {0x0100, 0x01}    	
};

XM_U32 sensor_getlist_OV9732(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau16SnsInit_OV9732_mipi)/sizeof(gau16SnsInit_OV9732_mipi[0]);
	DEBUG("SP1405 mipi 1 lane10bit!\n");
	return (XM_U32)gau16SnsInit_OV9732_mipi;
}
#endif

