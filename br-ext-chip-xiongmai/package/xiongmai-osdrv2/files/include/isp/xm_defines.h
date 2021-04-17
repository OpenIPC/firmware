/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_defines.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/6/27

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/6/27
  Author		: Lycai
  Modification	: creat
******************************************************************************/

#ifndef __XM_DEFINES_H__
#define __XM_DEFINES_H__


#include "xm_type.h"

#define LINE_LEN_BIT            5
#define LINE_LEN                (1<<LINE_LEN_BIT)
#define LINE_BASE_MASK          (~(LINE_LEN-1))

typedef enum {
	DSP_XM510 = 0x100,
	DSP_XM530,
	DSP_XM580,
	DSP_XM320,
	DSP_XM322,
	DSP_XM350,
	DSP_XM380,
	DSP_XM310 = 0x110,			// 提供给PQ_Tool
	DSP_XM310V300 = 0x113,		// 提供给PQ_Tool
	DSP_XM330V200 = 0x122,		// 提供给PQ_Tool
	DSP_XM550 = 0x155,
	NR_DSPCHIP
}XM_DSP_CHIP;

typedef enum {
	SENSORCLK_18M = 0,
	SENSORCLK_24M = 1,
	SENSORCLK_27M = 2,	
	SENSORCLK_37x125M = 3,	
	SENSORCLK_24M_42M = 4,	
	SENSORCLK_18x5625M = 5,	
}XM_SENSOR_CLK;

typedef enum {
	SNSIO_12V = 0,
	SNSIO_15V,
	SNSIO_18V,
	SNSIO_28V,
	SNSIO_33V,
}XM_SENSOR_IO;

typedef enum {
	MIPI_8BIT = 0,
	MIPI_10BIT,
	MIPI_12BIT,
}XM_MIPI_DEPTH;

typedef enum {
	MIPI_1LANE = 0,
	MIPI_2LANE,
	MIPI_4LANE,
}XM_MIPI_LANE;

typedef enum xm_XM_SENSOR_BWIDE
{
	SENSBWIDE_8BIT = 0,
	SENSBWIDE_10BIT ,
	SENSBWIDE_12BIT ,
}XM_SENSOR_BWIDE;

typedef enum xm_XM_SENSOR_CONT
{
	SENSCONT_DVP = 0,
	SENSCONT_MIPI = 1,
	SENSCONT_BUTT = 0xFF,
}XM_SENSOR_CONT;

typedef enum xm_XM_FUNEXT
{
	FUNBIT_XVI_MASK = 0,
	FUNBIT_AF_MASK = 1,
	FUNBIT_BUTT
}XM_FUNEXT;

typedef enum xm_PROFILE   
{
	
	P720_ = 0,
	P1080_ ,
	P960_ ,	
    P1536_ ,    
    P4M_ ,    
    P5M_ ,    
    P4K_ ,    
    PCVBS_=100,    
    P1080_S1 = 101,	// 1080*1080
    P1080_S2 = 102,	// 1280*1080
	PNULL ,
}PROFILE ;
typedef enum xm_VSTD
{
	VSTDNULL = 0,	
	PALS  ,
	NTSCS ,
	NRT15 ,
	NRT12p5 ,
}VSTD;

typedef enum xm_txvenc_type
{
	TXVENC_AHD = 0,
	TXVENC_CVI = 1,
	TXVENC_TVI = 2,
	TXVENC_CVBS = 3,
	TXVENC_AHD_15FPS = 4,
	//TXVENC_TVI_V1 = 7,
	TXVENC_BUTT
}TXVENC_TYPE;

typedef enum xm_ipcvenc_type
{
	IPC_Hx264 = 0,
	IPC_Hx265 = 1,
	IPCVENC_BUTT
}IPCVENC_TYPE;

typedef struct xm_UPSCALER_CFG
{
	PROFILE  VPIXEL;	 //  0:720P  1:1080P  2:960P	3:NULL
	VSTD	VSTDB;       //  0:NULL  1:PAL    2:NTSC
}UPSCALER_CFG;

typedef struct xm_SENSOR_IO_V
{
	XM_U8 SsDvdd;	
	XM_U8 SsAvdd;	
	XM_U8 SsDovdd;	
}XM_SENSOR_IO_V;

typedef struct xm_SENSOR_MIPI
{
	XM_U8 MpDepth;
	XM_U8 MpLane;
}XM_SENSOR_MIPI;

typedef struct xm_product_info
{
	XM_U32 u32ProductType;
	XM_U32 u32DSPType;
	XM_U32 u32SensorType;
	XM_U8	u8SensorClk;		
	XM_U8 	u8SnsCommMode;	// 0:I2C	1:SPI
	XM_SENSOR_IO_V   stSnsIO;		// 1.8v/3.3v
	XM_U8	u8StdType;		//0: unknow 1:PAL 2:NTSC
	PROFILE	u8RsltType;		//	0: 720P	1:1080P	2:960P 3:1536P		101:P1080_S1 	102:P1080_S2
	XM_BOOL bHsyncRecEn;	// 0:Disable	1:Enable
	XM_SENSOR_BWIDE SnsBwide;	//0:8bit 1:10bit 2:12bit
	XM_SENSOR_CONT	SnsConnect;	//0:DVP 1:MIPI
	XM_SENSOR_MIPI  SnsMipi;
	XM_U8	u8FunExt;		// XM_FUNEXT
	XM_U8	u8SnsHDRmark;	//0:normal 1:build-in 2:wdr
	XM_U8	u8Encoder;		//0:AHD 1:CVI 2:TVI 3;CVBS


/****************************
u8IRLed:  
	bit0:
		0  	普通模式（红外同步/自动同步... (硬件控制红外灯)）
		1	软件控制灯
	bit4:
		0	软光敏
		1	硬光敏
****************************/
	XM_U8 u8IRLed;
}XM_PRODUCT_INFO;


typedef enum xm_oem
{
	General = 0,
	JvFeng,
	XMJP,
}OEM;

#if 0
  inline void InvalidateDcache(unsigned long addr, unsigned long len)
{
    unsigned long end;
    //TODO: cache刷新需要重写
    return ;
    
    addr &= LINE_BASE_MASK;
    len >>= LINE_LEN_BIT;
    end   = addr + len*LINE_LEN;

    while(addr != end)
    {
        asm("mcr p15, 0, %0, c7, c6, 1"::"r"(addr));
        addr += LINE_LEN;
    }
    return;
}

  inline  void FlushDcache(unsigned long addr, unsigned long len)
{
    unsigned long end;

    //TODO: cache刷新需要重写
    return ;

    addr &= LINE_BASE_MASK;
    len >>= LINE_LEN_BIT;
    end   = addr + len*LINE_LEN;

    while(addr != end)
    {
        asm("mcr p15, 0, %0, c7, c10, 1"::"r"(addr));
        addr += LINE_LEN;
    }
    return;
}
#endif
/* For Sys */
#define DEFAULT_ALIGN    16
#define MAX_MMZ_NAME_LEN 16

#define MAX_NODE_NUM        16

/* For VDA */
#define VDA_MAX_NODE_NUM        32
#define VDA_MAX_INTERNAL        256
#define VDA_CHN_NUM_MAX         32
#define VDA_MAX_WIDTH           960
#define VDA_MAX_HEIGHT          576

/* For VENC */
#define VENC_MAX_NAME_LEN  16
#define VENC_MAX_CHN_NUM   64
#define VENC_MAX_GRP_NUM   64
#define H264E_MAX_WIDTH    1920
#define H264E_MAX_HEIGHT   2048
#define H264E_MIN_WIDTH    160
#define H264E_MIN_HEIGHT   64
#define JPEGE_MAX_WIDTH    8192
#define JPEGE_MAX_HEIGHT   8192
#define JPEGE_MIN_WIDTH    64
#define JPEGE_MIN_HEIGHT   64
#define VENC_MAX_ROI_NUM   8               /* 最大支持8个ROI区域 */
#define H264E_MIN_HW_INDEX 0
#define H264E_MAX_HW_INDEX 5
#define H264E_MIN_VW_INDEX 0
#define H264E_MAX_VW_INDEX 2

/* For VDEC, hi3518 not support */
#define VDEC_MAX_CHN_NUM    0

/* For Region */
#define RGN_HANDLE_MAX         1024
#define OVERLAY_MAX_NUM        8
#define COVEREX_MAX_NUM        16
#define COVER_MAX_NUM          4
#define OVERLAYEX_MAX_NUM      0 

/* number of channle and device on video input unit of chip
 * Note! VIU_MAX_CHN_NUM is NOT equal to VIU_MAX_DEV_NUM
 * multiplied by VIU_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VIU_MAX_DEV_NUM              1
#define VIU_MAX_WAY_NUM_PER_DEV      1
#define VIU_MAX_CHN_NUM_PER_DEV      1
#define VIU_MAX_PHYCHN_NUM           1
#define VIU_EXT_CHN_START            VIU_MAX_PHYCHN_NUM
#define VIU_MAX_EXT_CHN_NUM          16
#define VIU_MAX_EXTCHN_BIND_PER_CHN  8

#define VIU_MAX_CHN_NUM              (VIU_MAX_PHYCHN_NUM + VIU_MAX_EXT_CHN_NUM)


#define VIU_CHNID_DEV_FACTOR    2

/* 3518本不支持级联，这里宏定义这些信息，只是为了编译通过 */
#define VIU_MAX_CAS_CHN_NUM     2
#define VIU_SUB_CHN_START       16    /* 定义次通道起始宏*/
#define VIU_CAS_CHN_START       32    /* 定义级联通道起始宏*/

/* max number of VBI region*/
#define VIU_MAX_VBI_NUM         2
/* max length of one VBI region (by word)*/
#define VIU_MAX_VBI_LEN         8

#define VO_MIN_CHN_WIDTH    32      /* channel minimal width */
#define VO_MIN_CHN_HEIGHT   32      /* channel minimal height */

#define VO_MAX_ZOOM_RATIO   1000    /* max zoom ratio, 1000 means 100% scale */

#define VO_MAX_DEV_NUM          1      /* max dev num */
#define VO_MAX_LAYER_NUM        1      /* max layer num */
#define VO_MAX_CHN_NUM          32      /* mac chn num */
#define PIP_MAX_CHN_NUM         0
#define VHD_MAX_CHN_NUM         0       /* max VHD chn num */

#define VO_MAX_LAYER_IN_DEV     2       /* max layer num of each dev */

#define VO_CAS_MAX_PAT          128     /* cascade pattern max number */
#define VO_CAS_MAX_POS_32RGN    32      /* cascade position max number */
#define VO_CAS_MAX_POS_64RGN    64      /* cascade position max number */

#define VO_MAX_VIRT_DEV_NUM     4       /* max virtual dev num*/
#define VO_VIRT_DEV_0           3       /* virtual display device 1 */
#define VO_VIRT_DEV_1           4       /* virtual display device 2 */
#define VO_VIRT_DEV_2           5       /* virtual display device 3 */
#define VO_VIRT_DEV_3           6       /* virtual display device 4 */

#define VO_MAX_GFX_LAYER_PER_DEV 1
#define VOU_GRAPHICS_LAYER_NUM   1

#define VO_MIN_TOLERATE         1       /* min play toleration 1ms */
#define VO_MAX_TOLERATE         100000  /* max play toleration 100s */

#define AIO_MAX_CHN_NUM         2
#define AENC_MAX_CHN_NUM        32
#define ADEC_MAX_CHN_NUM        32

#define AI_DEV_MAX_NUM          1
#define AO_DEV_MIN_NUM          0
#define AO_DEV_MAX_NUM          1
#define SIO_MAX_NUM             1


#define VPSS_MAX_GRP_NUM   		128

#define VPSS_MAX_PHY_CHN_NUM	2
#define VPSS_MAX_EXT_CHN_NUM  	5
#define VPSS_MAX_CHN_NUM   		(VPSS_MAX_PHY_CHN_NUM + VPSS_MAX_EXT_CHN_NUM + 1)

#define VPSS_BSTR_CHN     		0
#define VPSS_LSTR_CHN     		1
#define VPSS_BYPASS_CHN   		2


#define PCIV_MAX_CHN_NUM        0       /* max pciv channel number in each pciv device */

#define RC_MAD_HIST_SIZE  64
#define RC_MSE_HIST_SIZE  128
#define RC_MAX_BLINK_QP   35
		 
#ifdef CHIPID_XM580
#define ISP_NUM_MAX		(2)	//(2)
#else
#define ISP_NUM_MAX		(1)	//(2)
#endif


#endif /* __XM_DEFINES_H__ */

