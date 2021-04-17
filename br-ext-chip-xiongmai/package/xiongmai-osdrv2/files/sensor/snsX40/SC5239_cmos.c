#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"
#include "Camera.h"

#include "xm_print.h"
#include "X50_cmos.h"

#ifdef SOC_NONE
#include "xm540_tmp.h"
#else
#include "xm510_tmp.h"
#endif
#include "mpi_phyvi.h"

#define HD4MP_25P_LINES		(1500)

extern const ISP_CMOS_GAMMA_S gstIspGamma; 
extern XM_VOID XM_MPI_ISP_Memset(XM_U8 *pu8Addr,XM_U8 u8Ch, XM_U32 u32Num);
extern XM_U32 gu32ShutNow;
extern XM_U8 gu8Fps;

static const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable = {
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

static XM_S32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
	if (XM_NULL == pstDef)
	{
		ERR("null pointer when get isp default value!\n");
		return -1;
	}
	pstDef->stGamma.bValid = XM_TRUE;
	XM_MPI_ISP_Memncpy((XM_U8*)&pstDef->stGamma, (XM_U8*)&gstIspGamma, sizeof(ISP_CMOS_GAMMA_S));
	XM_MPI_ISP_Memncpy((XM_U8 *)&pstDef->stAgcTbl, (XM_U8 *)&g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
	return 0;
}

static XM_VOID cmos_set_pixel_detect(XM_BOOL bEnable)
{
    return;
}

static XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
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
		u16OfstV = 0;
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

const static XM_U16 gau16GainTbl[64] =
{
	1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048,2176,2304,2432,2560,
	2688,2816,2944,3072,3200,3328,3456,3584,3712,3840,3968,4096,4352,4608,4864,5120,5376,5632,5888,6144,6400,
	6656,6912,7168,7424,7680,7936,8192,8704,9216,9728,10240,10752,11264,11776,12288,12800,13312,13824,14336,
	14848,15360,15872
};

static XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info	value!\n");
		return;
	}
    pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	XM_U8 u8i;
	if(u32InTimes >= gau16GainTbl[63])
	{
		pstAeSnsGainInfo->u32GainDb = 63;
	}
	else
	{
		pstAeSnsGainInfo->u32GainDb = 0;
		for(u8i = 0x1; u8i < 64; u8i ++)        
		{           
			if(u32InTimes <gau16GainTbl[u8i])            
			{                
				pstAeSnsGainInfo->u32GainDb = u8i-1;
				break;            
			}            
		}
	}
	pstAeSnsGainInfo->u32SnsTimes = gau16GainTbl[pstAeSnsGainInfo->u32GainDb];
	return;
}

static XM_VOID cmos_dgain_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("cmos_dgain_calc_table failed!\n");
		return;
	}
	XM_U8 u8i;
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

static XM_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	if (XM_NULL == pstAeSnsDft)
	{
		ERR("null pointer when get ae default value!\n");
		return -1;
	}
	pstAeSnsDft->u32FullLinesStd = HD4MP_25P_LINES;
	pstAeSnsDft->u8AeCompensation = 0x39;	
	pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	pstAeSnsDft->u32FlickerFreq = 0;

	pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-2;
	pstAeSnsDft->u32MinIntTime = 2;    

	pstAeSnsDft->u32MaxAgain = 15872;// 15.5
	pstAeSnsDft->u32MinAgain = 1024;

	pstAeSnsDft->u32MaxDgain = 512;  
	pstAeSnsDft->u32MinDgain = 128;

	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MaxISPDgain= 8 << pstAeSnsDft->u32ISPDgainShift;
	pstAeSnsDft->u32MinISPDgain= 1 << pstAeSnsDft->u32ISPDgainShift;

	pstAeSnsDft->bDelayCfg = XM_TRUE;
	return 0;
}

static XM_S32 cmos_get_sensor_max_resolution(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution)
{
	if (XM_NULL == pstSensorMaxResolution)
	{
	    ERR("null pointer when get sensor max resolution \n");
	    return -1;
	}
	pstSensorMaxResolution->u32MaxWidth  = 2560;
	pstSensorMaxResolution->u32MaxHeight = 1440;
	return 0;
}

static XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0xFFFFFFFF;
	if(su32IntTime == u32IntTime)
		return ;
	su32IntTime = u32IntTime;
	gu32ShutNow = u32IntTime;
	u32IntTime = u32IntTime*2;
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
	sensor_write_register(0x3e00, ((u32IntTime&0xFF000)>>12));
	sensor_write_register(0x3e01, ((u32IntTime&0x0ff0)>>4));
	sensor_write_register(0x3e02, ((u32IntTime&0xf)<<4));
	return;
}

const static XM_U16 gau16LogicAddr[4] = {
	0x3301,0x3633,0x3630,0x3622
};

const static XM_U8 gau8Logic_sc5239[4][5][4] = {
	//  0x3301  0x3633 0x3630 0x3622
	//CVBS PAL
	{
		{0x1e, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0xC0, 0x43, 0x82, 0x16}
	},
	//CVBS NTSC
	{
		{0x1e, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x43, 0x82, 0x16}
	},
	//AHD
	{
		{0x1e, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x90, 0x43, 0x82, 0x16}
	},
	// 15fps
	{
		{0x15, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0x50, 0x23, 0x80, 0xf6},
		{0xb8, 0x43, 0x82, 0x16}
	},
};

//u8Mode: 0:Auto  1:Refresh(Next)
static void gainLogic_sc5239(XM_U32 u32AllGain,XM_U8 u8Mode)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Vstd   [bit4~bit7]:Agc
	XM_U8 u8Idx,u8Idx2,u8i;
	if(gu8Fps==50)		u8Idx = 0;
	else if(gu8Fps==60) 	u8Idx = 1;
	else if(gu8Fps==15) 	u8Idx = 3;
	else					u8Idx = 2;

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
	if(	((su8Idx&0x0F) != u8Idx) ||
			((su8Idx>>4)&0x0F) != u8Idx2)
	{
			su8Idx = ((u8Idx2&0x0F)<<4)|(u8Idx&0x0F);
			sensor_write_register(0x3812, 0x00);
			for(u8i=0; u8i<4; u8i ++)
			{
				sensor_write_register((XM_U32)gau16LogicAddr[u8i], (XM_U32)gau8Logic_sc5239[u8Idx][u8Idx2][u8i]);
			}
			sensor_write_register(0x3812, 0x30);
	}
	if(u8Mode)
	{
		su8Idx = 0xFF;
	}
}

static XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	//static XM_U8 su8Reg0x5785 = 0xFF;
	XM_U8 u8i;
	XM_U32 u32AllGain,u32Tmp;
	unsigned int tmp[2];
	
	u32Again = (u32Again>63)?63:u32Again;
	for(u8i=0,u32Tmp = u32Dgain>>8;u32Tmp>0;u32Tmp=u32Tmp>>1,u8i++);
	u32AllGain = (u32Dgain&0xFF)<<u8i;
	u32AllGain = (XM_U64)gau16GainTbl[u32Again]*u32AllGain>>13; 
	gau32AllGain = u32AllGain;
	gainLogic_sc5239(gau32AllGain, 0);
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
	}
	return;
}

static const XM_U16 gau16FpsSet[][5]=
{
	// Reg    HD   SD(50) SD(60) 15fps
	{0x0100,0x00,0x00,0x00,0x00},
	{0x36e9,0xA4,0x84,0xC6,0xD4},	// Bypass PLL
	{0x36f9,0x84,0x81,0x81,0x81},	// Bypass PLL
	{0x3201,0x00,0x04,0x04,0x00},
	{0x3203,0x22,0x34,0x34,0x30},
	{0x3205,0x0F,0x0b,0x0b,0x0f},
	{0x3207,0xd7,0xdb,0xdb,0xdf},
	{0x3208,0x09,0x04,0x04,0x09},
	{0x3209,0x00,0x80,0x80,0x00},
	{0x320a,0x05,0x02,0x02,0x05},
	{0x320b,0xb0,0xd0,0xd0,0xa4},
	{0x3211,0x04,0x04,0x04,0x08},
	{0x3213,0x04,0x04,0x04,0x08},
	{0x3215,0x11,0x31,0x31,0x11},
	{0x3220,0x10,0x14,0x14,0x10},
	{0x3235,0x0b,0x05,0x06,0x0b},
	{0x3236,0xb0,0xda,0x02,0xb6},
	{0x36e9,0x24,0x04,0x46,0x54},
	{0x36ea,0x33,0x31,0x2c,0x2c},
	{0x36eb,0x0e,0x06,0x06,0x06},
	{0x36ec,0x1e,0x1e,0x1e,0x0e},
	{0x36ed,0x23,0x03,0x03,0x05},
	{0x36f9,0x04,0x01,0x01,0x01},
	{0x36fa,0xca,0x72,0x62,0xd6},
	{0x3946,0x00,0x20,0x20,0x20},
	{0x3947,0x00,0x18,0x18,0x18},
	{0x3948,0x00,0x06,0x06,0x06},
	{0x3949,0x00,0x06,0x06,0x06},
	{0x394A,0x00,0x18,0x18,0x18},
	{0x394B,0x00,0x2c,0x2c,0x2c},
	{0x3952,0x10,0x78,0x78,0x78},
	{0x3953,0x10,0x48,0x48,0x48},
	{0x3954,0x08,0x18,0x18,0x18},
	{0x3955,0x08,0x18,0x18,0x18},
	{0x3956,0x10,0x48,0x48,0x48},
	{0x3957,0x14,0x80,0x80,0x80},
	{0x3958,0x0a,0x10,0x10,0x10},
	{0x3959,0x10,0x20,0x20,0x20},
	{0x395a,0x18,0x38,0x38,0x38},
	{0x395b,0x18,0x38,0x38,0x38},
	{0x395c,0x10,0x20,0x20,0x20},
	{0x395d,0x0a,0x10,0x10,0x10},
	{0x3962,0x0d,0x0d,0x0d,0x04},
	{0x3f04,0x05,0x05,0x05,0x06},
	{0x3f05,0x00,0x7c,0x00,0x4e},
	{0x4837,0x20,0x4a,0x20,0x35},
	{0x5000,0x06,0x46,0x46,0x06},
	//{0x5780,0xFF,0x7F,0x7F,0x7F},
	{0x5900,0x01,0xf1,0xf1,0x01},
	{0x5901,0x00,0x04,0x04,0x00},
	{0x0100,0x01,0x01,0x01,0x01},
};
extern XM_PRODUCT_INFO gstProductInfo;
/* the function of sensor set fps */
static XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 u32Pixs=3960;
	XM_U32 u32TotalSizeV;
	XM_U8 u8i,u8Num,u8Idx;
	u32TotalSizeV = HD4MP_25P_LINES;
	u8Idx = 1;
	switch(u8Fps)
	{
		// CVBS_PAL   CVBS_NTSC
		case 50:
            u32Pixs = 2880;
			u32TotalSizeV = 750;
			u8Idx = 2;
			DEBUG("cmos_fps_set 4M 50fps\n");
			break;
		case 60:// 1560*770*59.94
            u32Pixs = 2600;
			u32TotalSizeV = 770;
			u8Idx = 3;
			DEBUG("cmos_fps_set 4M 50/60fps\n");
			break;
		case 25:// 4M_25Fps 3120*1500
		    u32Pixs = 3120;
		    DEBUG("cmos_fps_set 4M 25fps\n");
			break;
		case 30:// 4M_30Fps 2600*1500
			u32Pixs = 2600;
			DEBUG("cmos_fps_set 4M 25fps\n");
			break;
		case 15:// 5M_15Fps
			u32Pixs = 3120*25/15;
			u8Fps = 15;
			DEBUG("cmos_fps_set 1M 15/12fps\n");
			break;
		case 10:// 5M_10Fps
			u32Pixs = 3120*25/10;
			DEBUG("cmos_fps_set 5M 10fps\n");
			break;
		case 20:// 5M_20Fps
		default:
			u32Pixs = 3120*25/20;
			DEBUG("cmos_fps_set 5M 20fps\n");
			break;
	}		
	gu8Fps = u8Fps;
	// Change the frame rate via changing the vertical blanking
	if(pstAeSnsDft != NULL)
	{
		pstAeSnsDft->u32FullLinesStd = u32TotalSizeV;
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDft->u32FullLinesStd-4;
		pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd * u8Fps / 2;
	}
    u8Num = sizeof(gau16FpsSet)/sizeof(gau16FpsSet[0]);
	for(u8i=0;u8i<u8Num;u8i ++)
	{
		sensor_write_register((XM_U32)gau16FpsSet[u8i][0], (XM_U32)gau16FpsSet[u8i][u8Idx]);
	}
	sensor_write_register(0x320c, (u32Pixs>>8)&0xFF);
	sensor_write_register(0x320d, u32Pixs&0xFF);
	sensor_write_register(0x320e, (u32TotalSizeV>>8)&0xFF);
	sensor_write_register(0x320f, u32TotalSizeV&0xFF);
	SysDelay_ms(20);
	gainLogic_sc5239(gau32AllGain, 1);

	return;
}

static XM_VOID cmos_slow_framerate_set(XM_U16 u16FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    static XM_U16 preU16FullLine = 0xffff;
    XM_U32 MaxIntTime = 0xffff;
    if(preU16FullLine == u16FullLines)
        return;

    preU16FullLine = u16FullLines;
    u16FullLines = (u16FullLines>=4096)?4000:u16FullLines;
    MaxIntTime = u16FullLines;
    if((gstProductInfo.u8StdType == 2)&&(gu8Fps == 25))
    {
        MaxIntTime = (u16FullLines/1250)*1250+4;
    }
    //DEBUG("cmos_slow_framerate_set u32MaxIntTime %d\n",u16FullLines);
    pstAeSnsDft->u32MaxIntTime = MaxIntTime - 4;
    SysDelay_ms(100);
    if(gu32ShutNow>pstAeSnsDft->u32MaxIntTime)
    {
        cmos_inttime_update(pstAeSnsDft->u32MaxIntTime);
    }
	sensor_write_register(0x320e, (u16FullLines&0xff00)>>8);	
	sensor_write_register(0x320f, u16FullLines&0xff);      
       
	XM_MPI_MIPI_RefreshFV(0, (XM_U32)u16FullLines);
}


/****************************************************************************
* AWB
****************************************************************************/
static const ISP_COLORMATRIX_AUTO_S g_stAwbCcm =
{
	5000,
	{
		0x000,356,  -135, 35,
		0x000,-59,  327, -12,
        0x000, 5,  -145, 396//0x000, -35, -130, 421
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
static const ISP_AWB_CALIBRATION_V2_S gstAwbCal = {
#if 0
	{0, 0, 4096, 2529, 3487, 1967, 1233, 4096},
	{4096, 2985, 0, 0, 3458, 2671, 4096, 1354},
	{-1495, -4096, -1933, -4096, -4096, -4096, -2783, -2907},
	213,
	0, 1925, 0, 3252,
	{0, 890, 1211, 1606, 1727, 1854, 2145, 2156, 3252, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
	{1350, 1024, 1486, 0}
#else
	{0, 0, 4096, 2834, 3994, 2058, 1186, 4096},
	{4096, 3790, 0, 0, 4096, 2544, 4096, 1412},
	{-891, -4096, -1489, -4096, -3949, -4096, -2243, -2662},
	213,
	0, 1688, 0, 2900,
	{0, 921, 1207, 1443, 1528, 1662, 1845, 1882, 2900, 0, 0, 0, 0, 0, 0, 0},
	{2000, 2150, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
 	{1529, 1024, 1589, 0}
#endif
};

static XM_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	if(XM_NULL == pstAwbSnsDft)
	{
		ERR("null pointer when get awb default value!\n");
		return XM_FAILURE;
	}
	XM_MPI_ISP_Memset((XM_U8*)pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
	pstAwbSnsDft->pstRbTable = (const  AWB_COEF_TABLE_S*)(&gstAwbCal);
	// CCM
	XM_MPI_ISP_Memncpy((XM_U8*)&(pstAwbSnsDft->stCcm), (XM_U8*)&g_stAwbCcm, sizeof(ISP_COLORMATRIX_AUTO_S));
	return XM_SUCCESS;
}


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
XM_S32 cmos_init_sensor_exp_function_SC5239(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	//    XM_MPI_ISP_Memset((char *)pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;//cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;//cmos_get_sensor_max_resolution;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_SC5239(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
//    XM_MPI_ISP_Memset((char *)pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_fps_get = NULL;
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

XM_S32 cmos_init_awb_exp_function_SC5239(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	XM_MPI_ISP_Memset((XM_U8*)pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}









