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
extern XM_U8 gu8MaxShutterOfst;
extern const XM_U16 gau16GainTbl_SmatSns[64];
XM_U16 gu16SensorSwitch = 3;	//2: SC2145H_A    3: SC2145H_B(SC2145H1)
extern XM_U32 gau32AllGain;

extern XM_VOID cmos_gain_calc_table_com(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo);
static void gainLogic_sc2145H(XM_U32 u32AllGain,XM_U8 u8Mode);

// u8Mode: 1:Refresh
static void shutLogic_sc2145H(XM_U32 u32IntTimeReg,XM_U8 u8Mode)
{
	static XM_U8 su8RegVal = 0xFF;
	XM_U8 u8RegVal;
	u8RegVal = (su8RegVal==0xFF)?0x04:su8RegVal;

	if(u32IntTimeReg < 80)
	{
		u8RegVal = 0x14;
	}
	else if(u32IntTimeReg > 160)
	{
		u8RegVal = 0x04;
	}

	if(su8RegVal != u8RegVal)
	{
		su8RegVal = u8RegVal;
		sensor_write_register(0x3314,(XM_U32)u8RegVal);
	}
}

STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	XM_U32 u32Tmp;
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;

	u32IntTime = u32IntTime*2;
	u32Tmp = ((u32IntTime&0x0ff0)>>4);
	sensor_write_register(0x3903, 0x84);
	sensor_write_register(0x3903, 0x04);

	shutLogic_sc2145H(u32IntTime, 0);

	sensor_write_register(0x3e01, u32Tmp);
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;
}

STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;

	XM_U32 u32AllGain,u32Tmp;
	unsigned int tmp[2];
	u32Again = (u32Again>63)?63:u32Again;
	u32Dgain = (u32Dgain>4)?4:u32Dgain; 

	u32AllGain = gau16GainTbl_SmatSns[u32Again]*u32Dgain/64; 
	gau32AllGain = u32AllGain;
	if((su32AGain == u32Again) && (su32DGain == u32Dgain))
	{
		return;
	}
	su32AGain = u32Again;
	su32DGain = u32Dgain;
    // 1.BL
    sensor_write_register(0x3903, 0x84);
    sensor_write_register(0x3903, 0x04);
    // 2. Logic
	gainLogic_sc2145H(u32AllGain, 0);
    // 3.GainUpdate
    tmp[0] = (u32Again&0x0F) | 0x10;	
    u32Tmp = 0x03;
    u32Again = u32Again/16;
    for(;u32Again>0;u32Again--)
    {
        u32Tmp = (u32Tmp<<1)|0x01;
    }
    switch(u32Dgain)
    {
        case 2: u32Tmp |= 0x20;
                break;
        case 4: u32Tmp |= 0x60;
                break;
    }
    tmp[1] = u32Tmp;
    sensor_write_register(0x3e08, tmp[1]);
    sensor_write_register(0x3e09, tmp[0]);
	return;
}

static XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info  value!\n");
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

//Reg0x3020 = 0x03
const static XM_U8 gau8Logic_0_sc2145H[4][5][5] = {
	//0x3633 0x3301 0x3622 0x3635 0x366f
	{//CVBS PAL
		{0x43, 0x0c, 0xd6, 0x8f, 0x2a},
		{0x42, 0x14, 0xd6, 0x88, 0x2c},
		{0x42, 0x16, 0xd6, 0x84, 0x2c},
		{0x42, 0x1a, 0x16, 0x82, 0x2c},
		{0x42, 0x48, 0x16, 0x80, 0x2c}
	},
	{//CVBS NTSC
		{0x43, 0x0a, 0xd6, 0x8f, 0x2a},
		{0x42, 0x14, 0xd6, 0x88, 0x2c},
		{0x42, 0x16, 0xd6, 0x84, 0x2c},
		{0x42, 0x18, 0x16, 0x82, 0x2c},
		{0x42, 0x1C, 0x16, 0x80, 0x2c}
	},
	{//AHD-PAL
		{0x43, 0x07, 0xd6, 0x8f, 0x2a},
		{0x42, 0x0e, 0xd6, 0x88, 0x2c},
		{0x42, 0x10, 0xd6, 0x84, 0x2c},
		{0x42, 0x18, 0x16, 0x82, 0x2c},
		{0x42, 0x32, 0x16, 0x80, 0x2c}
	},
	{//AHD-NTSC
		{0x43, 0x07, 0xd6, 0x8f, 0x2a},
		{0x42, 0x0e, 0xd6, 0x88, 0x2c},
		{0x42, 0x10, 0xd6, 0x84, 0x2c},
		{0x42, 0x18, 0x16, 0x82, 0x2c},
		{0x42, 0x20, 0x16, 0x80, 0x2c}
	}
};
//Reg0x3020 = 0x02
const static XM_U8 gau8Logic_1_sc2145H[4][5][3] = {
	//0x3633 0x3301 0x3622
	{//CVBS PAL
		{0x42, 0x05, 0xd6},
		{0x42, 0x09, 0xd6},
		{0x42, 0x0b, 0xd6},
		{0x42, 0x0d, 0x16},
		{0x42, 0x32, 0x16}
	},
	{//CVBS NTSC
		{0x42, 0x06, 0xd6},
		{0x42, 0x07, 0xd6},
		{0x42, 0x08, 0xd6},
		{0x42, 0x0d, 0x16},
		{0x42, 0x2a, 0x16}
	},
	{//AHD-PAL
		{0x42, 0x04, 0xd6},
		{0x42, 0x05, 0xd6},
		{0x42, 0x05, 0xd6},
		{0x42, 0x06, 0x16},
		{0x42, 0x32, 0x16}
	},
	{//AHD-NTSC
		{0x42, 0x04, 0xd6},
		{0x42, 0x05, 0xd6},
		{0x42, 0x05, 0xd6},
		{0x42, 0x06, 0x16},
		{0x42, 0x32, 0x16}
	}
};

// u8Mode: 1:Refresh
static void gainLogic_sc2145H(XM_U32 u32AllGain,XM_U8 u8Mode)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Vstd   [bit4~bit7]:Agc
	XM_U8 u8Idx,u8Idx2;
	if(gu8Fps==50)		u8Idx = 0;
	else if(gu8Fps==60) 	u8Idx = 1;
	else if(gu8Fps==25)		u8Idx = 2;
	else            		u8Idx = 3;
	
	if(u32AllGain<32){
		u8Idx2 = 0;
	}
	else if(u32AllGain<64){
		u8Idx2 = 1;
	}
	else if(u32AllGain<128){
		u8Idx2 = 2;
	}
	else if(u32AllGain<248){
		u8Idx2 = 3;
	}
	else{
		u8Idx2 = 4;
	}
	if((((su8Idx&0x0F) != u8Idx) || ((su8Idx>>4)&0x0F) != u8Idx2) || u8Mode)
	{
		su8Idx = ((u8Idx2&0x0F)<<4)|(u8Idx&0x0F);
		sensor_write_register(0x3812, 0x00);
		if(0x02 == gu16SensorSwitch)//SC1245-A
		{
			sensor_write_register(0x3633, (XM_U32)gau8Logic_1_sc2145H[u8Idx][u8Idx2][0]);
			sensor_write_register(0x3301, (XM_U32)gau8Logic_1_sc2145H[u8Idx][u8Idx2][1]);
			sensor_write_register(0x3622, (XM_U32)gau8Logic_1_sc2145H[u8Idx][u8Idx2][2]);
		}
		else//B
		{
			sensor_write_register(0x3633, (XM_U32)gau8Logic_0_sc2145H[u8Idx][u8Idx2][0]);
			sensor_write_register(0x3301, (XM_U32)gau8Logic_0_sc2145H[u8Idx][u8Idx2][1]);
			sensor_write_register(0x3622, (XM_U32)gau8Logic_0_sc2145H[u8Idx][u8Idx2][2]);
			sensor_write_register(0x3635, (XM_U32)gau8Logic_0_sc2145H[u8Idx][u8Idx2][3]);
			sensor_write_register(0x366f, (XM_U32)gau8Logic_0_sc2145H[u8Idx][u8Idx2][4]);
		}
		sensor_write_register(0x3812, 0x30);
	}
}

static const XM_U16 gau16FpsRegAddr[21] = {
	0x3035,
	0x3039,
	0x3235,
	0x3213,
	0x3302,
	0x3236,
	0x3f04,
	0x3f05,
	0x3301,
	0x3303,
	0x3306,
	0x330b,
	0x3309,
	0x330e,
	0x3367,
	0x3622,
	0x3633,
	0x3635,
	0x3637,
	0x366f,
	0x3908,
};

static const XM_U8 gau8FpsRegData[2][21][3] = {
//Reg	SD_P SD_N HD    	2145H1
//CVBS	P N AHD P N
{
/*0x3035,*/	{0xe2,0x9a,0xe2},
/*0x3039,*/	{0x21,0x21,0x51},
/*0x3235,*/	{0x05,0x06,0x05},
/*0x3213,*/	{0x08,0x08,0x04},
/*0x3302,*/	{0x1d,0x0d,0x1d},
/*0x3236,*/	{0xda,0x02,0xda},
/*0x3f04,*/	{0x03,0x02,0x02},
/*0x3f05,*/	{0x9c,0xe8,0xfc},
/*0x3301,*/	{0x0c,0x0a,0x07},
/*0x3303,*/	{0x18,0x18,0x18},
/*0x3306,*/	{0x40,0x38,0x24},
/*0x330b,*/	{0xc8,0xa0,0x66},
/*0x3309,*/	{0x98,0x98,0x98},
/*0x330e,*/	{0x38,0x18,0x38},
/*0x3367,*/	{0x04,0x04,0x04},
/*0x3622,*/	{0xd6,0xd6,0xd6},
/*0x3633,*/	{0x43,0x43,0x43},
/*0x3635,*/	{0x8f,0x8f,0x8f},
/*0x3637,*/	{0x18,0x18,0x18},
/*0x366f,*/	{0x2a,0x2a,0x2a},
/*0x3908,*/	{0x41,0x41,0x41},
},
{							// 2145H
/*0x3035,*/	{0xe2,0x9a,0xe2},
/*0x3039,*/	{0x21,0x21,0x51},
/*0x3235,*/	{0x05,0x06,0x05},
/*0x3213,*/	{0x08,0x08,0x04},
/*0x3302,*/	{0x3d,0x3d,0x7d},
/*0x3236,*/	{0xda,0x02,0xda},
/*0x3f04,*/	{0x03,0x02,0x02},
/*0x3f05,*/	{0x9c,0xe8,0xfc},
/*0x3301,*/	{0x05,0x04,0x04},
/*0x3303,*/	{0x20,0x20,0x20},
/*0x3306,*/	{0x38,0x38,0x1c},
/*0x330b,*/	{0xcc,0xa0,0x5e},
/*0x3309,*/	{0xa0,0xa0,0xa0},
/*0x330e,*/	{0x30,0x30,0x30},
/*0x3367,*/	{0x08,0x08,0x08},
/*0x3622,*/	{0xd6,0xd6,0xd6},
/*0x3633,*/	{0x42,0x42,0x42},
/*0x3635,*/	{0x84,0x84,0x84},
/*0x3637,*/	{0x1a,0x1a,0x1a},
/*0x366f,*/	{0x2c,0x2c,0x2c},
/*0x3908,*/	{0x41,0x41,0x41},
}
};

STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32TotalSizeV,u32ExpNow,u32Tmp;
	XM_U32 SnsSwitch,u32Pixs=1920;
	XM_U8 u8i,u8Idx = 2;
	
	SnsSwitch = (gu16SensorSwitch==3)?0:1;
	
	u32ExpNow = sensor_read_register(0x3e01);
	u32Tmp = sensor_read_register(0x3e02);
	u32ExpNow = ((u32ExpNow<<4)|((u32Tmp>>4)&0x0F))>>1;
	u32TotalSizeV = 750;
	switch(u8Fps)
	{
		case 30:
			u32Pixs = 1600;
			u8Idx = 2;
			break;
		case 25:
			u32Pixs = 1920;
			u8Idx = 2;
			break;
		case 50:
			u32Pixs = 1920;
			u8Idx = 0;
			u32TotalSizeV = 750;
			break;
		case 60:
			u32Pixs = 1540;
			u8Idx = 1;
			u32TotalSizeV = 780;
			break;
		default:
			//ERR("cmos_fps_set: fps err!\n");
			return;
			break;
	}		

	if(pstAeSnsDft != NULL) 
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
	if(u32ExpNow>u32TotalSizeV-4)
		cmos_inttime_update(u32TotalSizeV-4);
	for(u8i=0;u8i<21;u8i ++)
	{
		sensor_write_register((XM_U32)gau16FpsRegAddr[u8i], (XM_U32)gau8FpsRegData[SnsSwitch][u8i][u8Idx]);
	}
	sensor_write_register(0x320c, (u32Pixs>>8)&0xFF);
	sensor_write_register(0x320d, u32Pixs&0xFF);
	sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);

	gu8Fps = u8Fps;

	SysDelay_ms(20);
	// Logic
	shutLogic_sc2145H(u32ExpNow<<1, 1);
	gainLogic_sc2145H(gau32AllGain, 1);
	return;
}

STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U32 u32Val,u32Reg3901 = 0;
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
		u32Val |= 0xe0;
		u16OfstV = 1;
		u32Reg3901 = 4;
	}
	else
	{
		u32Val &= ~0xe0;
		u16OfstV = 0;
	}
	sensor_write_register(0x3221, u32Val);
	sensor_write_register(0x3901, u32Reg3901);
	VI_WinSet(1, u8Mirror, u8Flip, 0, u16OfstV);
	XM_MPI_MIPI_RefreshFV(100, 0);
	return XM_SUCCESS;
}

XM_S32 cmos_init_sensor_exp_function_sc2145H(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
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

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
XM_S32 cmos_init_ae_exp_function_sc2145H(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
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
const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc2145H =
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
	{0x30,0x2C,0x2C,0x2C,0x28,0x1C,0x18,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
	
	/* snr_thresh 2DNr		*/
	{0x00,0x01,0x04,0x10,0x20,0x28,0x30,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40},

	/* snr_thresh 3DNr	 Tf	*/
	{0x04,0x06,0x0A,0x0E,0x12,0x15,0x17,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18},

	/* snr_thresh 3DNr	 Sf	*/
	{0x00,0x00,0x00,0x04,0x0C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},  

	/* DyDpc_thresh 		*/
	{0xB4,0xB8,0xD4,0xD8,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0},
	
	/* saturation_strength */
	{0x9A,0x9A,0x94,0x90,0x88,0x70,0x50,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
	
	/* Blc	*/
	{0x110,0x110,0x110,0x110,0x110,0x104,0x100,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0},

	/*Y_Tran gamma*/ 
	{0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E},
};

const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc2145H =
{
	5000,
	{
		0x000,383, -157, 30,
		0x000,-50,300, 6,
		0x000, 11,-276, 521
	},
	4000,
	{
		0x000,305, -124, 75,
		0x000,-39, 213,  82,
		0x000,  19,-311,548
	},
	2800,
	{
		0x000,305, -113, 64,
		0x000,-49, 200, 105,
		0x000,  0, -483, 739
	}
};

const ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2145H_A = {
	{0, 0, 4096, 2868, 3796, 2117, 953, 4096},
	{4096, 4096, 0, 0, 4096, 2782, 4096, 1921},
	{-791, -4043, -1543, -4096, -3650, -4096, -1837, -2766},
	213,
	0, 1644, 0, 2748,
	{0, 830, 1074, 1288, 1384, 1520, 1727, 1769, 2748, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1493, 1024, 1813, 0}
};
	
const ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2145H_B = {
	{0, 0, 4096, 2827, 4096, 2091, 1137, 4096},
	{4096, 3503, 0, 0, 3722, 2390, 4096, 1824},
	{-1106, -4096, -1450, -4096, -4017, -4096, -2433, -2880},
	213,
	0, 1683, 0, 3030,
	{0, 926, 1200, 1439, 1548, 1722, 1965, 2018, 3030, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1519, 1024, 1437, 0}
};

// ExtClk: 24M  Pclk:36M
static const XM_U16 gau16SnsInit_sc2145H[][2] = {
	{0x0100,0x00},
	{0x3039,0x80},
	{0x3034,0x06},
	{0x3035,0xe2},
	{0x3038,0x84},
	{0x303a,0xba},
	{0x3208,0x05},
	{0x3209,0x08},
	{0x320a,0x02},
	{0x320b,0xd8},
	{0x320e,0x02},
	{0x320f,0xee},
	{0x3211,0x04},
	{0x3213,0x04},
	{0x3235,0x05},
	{0x3236,0xda},
	{0x3302,0x7d},
	{0x3308,0x1c},
	{0x331b,0x83},
	{0x331e,0x11},
	{0x331f,0x91},
	{0x3320,0x07},
	{0x3321,0x9c},
	{0x3326,0x00},
	{0x3333,0x30},
	{0x3334,0x40},
	{0x3357,0x51},
	{0x335e,0x01},
	{0x335f,0x02},
	{0x3366,0x7c},
	{0x3368,0x04},
	{0x3369,0x00},
	{0x336a,0x00},
	{0x336b,0x00},
	{0x337c,0x03},
	{0x337d,0x05},
	{0x337f,0x03},
	{0x33a0,0x04},
	{0x3620,0x28},
	{0x3621,0x20},
	{0x3622,0xd6},
	{0x3625,0x02},
	{0x3630,0x20},
	{0x3631,0x86},
	{0x3632,0x28},
	{0x3636,0x25},
	{0x3638,0x1f},
	{0x3639,0x09},
	{0x363b,0x0c},
	{0x366e,0x08},
	{0x3670,0x02},
	{0x3674,0x20},
	{0x3675,0x2f},
	{0x3676,0x2f},
	{0x367c,0x07},
	{0x367d,0x0f},
	{0x3802,0x01},
	{0x3e00,0x00},
	{0x3e01,0x5d},
	{0x3e02,0x80},
	{0x3e03,0x0b},
	{0x3e07,0x00},
	{0x3e08,0x03},
	{0x3e09,0x10},
	{0x3f00,0x07},
	{0x3f04,0x02},
	{0x3f05,0xfc},
	{0x4500,0x51},	
	{0x3039,0x51},
	{0x3d08,0x03},	// Zgb
	{0x3640,0x02},	// Lycai
	{0x3641,0x02},	// Lycai
	{0x391b,0x50},	// 85度高温竖条问题
	{0x391c,0x98},	// Blc闪问题
	{0x3902,0xc5},

	{0x3301,0x07},
	{0x3303,0x18},
	{0x3306,0x24},
	{0x3309,0x98},
	{0x330b,0x66},
	{0x330e,0x38},
	{0x3367,0x04},
	{0x3622,0xd6},
	{0x3633,0x43},
	{0x3635,0x8f},
	{0x3637,0x18},
	{0x366f,0x2a},
	{0x3908,0x41},
	{0x0100,0x01},
};

XM_U32 sensor_getlist_sc2145H(XM_U16 *pu16Num)
{
	*pu16Num = sizeof(gau16SnsInit_sc2145H)/sizeof(gau16SnsInit_sc2145H[0]);
	return (XM_U32)gau16SnsInit_sc2145H;
}

#endif

