/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_common_isp.h
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
#ifndef __XM_COMM_ISP_H__
#define __XM_COMM_ISP_H__

#include "xm_type.h"
#include "xm_errno.h"
#include "xm_common.h"
#include "xm_isp_debug.h"
#include "Camera.h"



/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
    
#define VREG_MAX_NUM        16

#define AE_ZONE_ROW     	(7)
#define AE_ZONE_COLUMN  	(7)
#define LUT_FACTOR          (8)
#define GAMMA_FE_LUT_SIZE   ((1<<LUT_FACTOR)+1)
#if (defined CHIPID_XM530)||(defined CHIPID_XM350)
#define GAMMA_NODE_NUMBER   97
#else
#define GAMMA_NODE_NUMBER   61
#endif
#define SHADING_TABLE_NODE_NUMBER_MAX (129)

#define LIGHTSOURCE_NUM      (4)
#define STATIC_DP_COUNT_MAX  (2048)

#define ISP_REG_BASE        0x205A0000
#define ISP_REG_SIZE        0x7fff

#define ISP_WINDOW0_START   0x205A010C
#define ISP_WINDOW0_SIZE    0x205A0110
#define ISP_WINDOW2_START   0x205A0120
#define ISP_WINDOW2_SIZE    0x205A0124

#define ISP_BYPASS_BASE     0x205A0040

#define ISP_MAX_SNS_REGS    16
#define ISP_MAX_DEV_NUM     1


#define ISP_AUTO_STENGTH_NUM	(16)


// Statistics Attr (Isp V2)
#define ISP_V2_ZONE_ROW     		(16)
#define ISP_V2_ZONE_COLUMN  	(32)

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/

typedef enum xm_ISP_ERR_CODE_E
{
    ERR_ISP_NOT_INIT                = 0x40,
    ERR_ISP_TM_NOT_CFG              = 0x41,
    ERR_ISP_ATTR_NOT_CFG            = 0x42,
    ERR_ISP_SNS_UNREGISTER          = 0x43,
    ERR_ISP_INVALID_ADDR            = 0x44,
    ERR_ISP_NOMEM                   = 0x45,
} ISP_ERR_CODE_E;


#define XM_ERR_ISP_NULL_PTR             XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define XM_ERR_ISP_ILLEGAL_PARAM        XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)

#define XM_ERR_ISP_NOT_INIT             XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define XM_ERR_ISP_TM_NOT_CFG           XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_TM_NOT_CFG)
#define XM_ERR_ISP_ATTR_NOT_CFG         XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define XM_ERR_ISP_SNS_UNREGISTER       XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define XM_ERR_ISP_INVALID_ADDR         XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define XM_ERR_ISP_NOMEM                XM_DEF_ERR(XM_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)


typedef enum xm_ISP_BAYER_FORMAT_E
{
    BAYER_RGGB    = 0,
    BAYER_GRBG    = 1,
    BAYER_GBRG    = 2,
    BAYER_BGGR    = 3,
    OTHER_RGB_G  = 4,
    OTHER_RGB_IR = 5,
    BAYER_BUTT      
} ISP_BAYER_FORMAT_E;

typedef enum xm_ISP_OP_TYPE_E
{
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_KEY  = 2,  	//直接修改寄存器
    OP_TYPE_OTHER  = 3, 
    OP_TYPE_BUTT	
} ISP_OP_TYPE_E;

#define ISP_AE_ROUTE_MAX_NODES 8
typedef struct xm_ISP_AE_ROUTE_NODE_S
{
    XM_U32  u32IntTime;
    XM_U32  u32SysGain;     /* shift is in the ISP_AE_ATTR_S's stSysGainRange. */
    XM_U32  u32ApePercent;  /* the percent of the iris's aperture, range is [0~100], dc-iris not support in auto mode. */
} ISP_AE_ROUTE_NODE_S;

typedef struct xm_ISP_AE_ROUTE_S
{
    XM_U32 u32TotalNum;    
    ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

typedef enum xm_ISP_AE_MODE_E
{
    AE_MODE_LOW_NOISE   = 0,
    AE_MODE_FRAME_RATE  = 1,
    AE_MODE_BUTT
} ISP_AE_MODE_E;

typedef enum xm_ISP_AWB_ALG_TYPE_E
{
	AWB_ALG_DEFAULT = 0,
	AWB_ALG_ADVANCE = 1,
	AWB_ALG_BUTT
    
} ISP_AWB_ALG_TYPE_E;

typedef enum xm_ISP_WB_MODE_E
{
    /* all auto*/
    WB_AUTO = 0,
    
    /* half auto */        
    WB_FLUORESCENT,        /*fluorescent*/
    WB_LAMP,                /*lamp*/
    WB_DAYLIGHT,            /*daylight*/
    WB_FLASH,                /*flash light*/
    WB_CLOUDY,                /*cloudy*/
    WB_SHADOW,                /*shadow*/
    WB_BUTT
} ISP_WB_MODE_E;

typedef struct xm_ISP_WINDOW_S
{
    XM_U16 u16Start;
    XM_U16 u16Length;
} ISP_WINDOW_S;

typedef enum xm_ISP_WIND_MODE_E
{
    ISP_WIND_NONE        = 0,
    ISP_WIND_HOR        = 1,
    ISP_WIND_VER        = 2,
    ISP_WIND_ALL        = 3,
    ISP_WIND_BUTT
} ISP_WIND_MODE_E;

typedef enum xm_ISP_IRIS_STATUS_E
{
    ISP_IRIS_KEEP  = 0,       /* Do nothing to Iris */
    ISP_IRIS_OPEN  = 1,       /* Open Iris to the max */
    ISP_IRIS_CLOSE = 2,       /* Close Iris to the min */
    ISP_IRIS_BUTT
} ISP_IRIS_STATUS_E;

typedef enum xm_ISP_TRIGGER_STATUS_E
{
    ISP_TRIGGER_INIT     = 0,  /* Initial status, before trigger */
    ISP_TRIGGER_SUCCESS  = 1,  /* Trigger finished successfully */
    ISP_TRIGGER_TIMEOUT  = 2,  /* Trigger finished because of time out */
    ISP_TRIGGER_BUTT
} ISP_TRIGGER_STATUS_E;

typedef enum xm_ISP_WDR_MODE_E
{
    ISP_SENSOR_LINEAR_MODE = 0,
    ISP_SENSOR_WDR_MODE,

    ISP_WDR_BUTT,
} ISP_WDR_MODE_E;

typedef struct xm_ISP_WDR_ATTR_S
{
    ISP_WDR_MODE_E  enWdrMode;
} ISP_WDR_ATTR_S;

typedef struct xm_ISP_INPUT_TIMING_S
{
    ISP_WIND_MODE_E enWndMode;
    XM_U16 u16HorWndStart;    /*RW, Range: [0x0, 0x780]*/
    XM_U16 u16HorWndLength;   /*RW, Range: [0x0, 0x780]*/
    XM_U16 u16VerWndStart;    /*RW, Range: [0x0, 0x4B0]*/
    XM_U16 u16VerWndLength;   /*RW, Range: [0x0, 0x4B0]*/
} ISP_INPUT_TIMING_S;

typedef struct xmISP_VF_ATTR_S
{
	ISP_OP_TYPE_E opType;
	SIZE_S stAutoAttr;
}ISP_VF_ATTR_S;


typedef struct xmISP_PUB_ATTR_S
{
	RECT_S 	stWndRect;				// RW
	XM_U8	u8FrameRate;			// RW
	XM_U8   u8Vpixel;				// 0:720P  1:1080P  2:960P
	XM_U8   u8Upscaleren;			// 0:disable  1:enble	
	ISP_BAYER_FORMAT_E	enBayer;		// RW
	ISP_VF_ATTR_S stVFAttr;
	XM_U32 u32PhyAddr;// 3D Buffer 
	XM_U32 u32ErrPhyAddr;	
	XM_U32 u32Size;	//
	XM_U32 u32ErrSize;	// bit0~bit15: SizeH  bit16~bit31:SizeV
	XM_U8  u8BufFNum;
	XM_U8  u8SnsBwide;   // 0:8bit  1:10bit 2:12bit
	XM_U8  u8SnsConnect;	//0:DVP 1:MIPI
	XM_U8  u8SnsHDRmark;	//0:normal 1:build-in 2:wdr
}ISP_PUB_ATTR_S;


typedef struct xm_ISP_IMAGE_ATTR_S
{
    XM_U16 u16Width;        /*RW, Range: [0x0, 0x780]*/
    XM_U16 u16Height;       /*RW, Range: [0x0, 0x4B0]*/
    XM_U16 u16FrameRate;    /*RW, Range: [0x0, 0xFF]*/        
    ISP_BAYER_FORMAT_E  enBayer;    
} ISP_IMAGE_ATTR_S;

typedef enum xm_ISP_MOD_BYPASS_E
{
    ISP_MOD_SHARPEN     = 0x8000,
    ISP_MOD_GAMMARGB    = 0x4000,
    ISP_MOD_COLORMATRIX = 0x2000,
    ISP_MOD_DEMOSAIC    = 0x1000,
    
    ISP_MOD_GAMMAPOST   = 0x0800,
    ISP_MOD_GAMMAPRE    = 0x0200,
    ISP_MOD_SHADING     = 0x0100,

    ISP_MOD_DRC         = 0x0080,
    ISP_MOD_GAIN        = 0x0040,
    ISP_MOD_SINTER      = 0x0008,
    ISP_MOD_HOTPIXEL    = 0x0004,
    ISP_MOD_GAMMAFE     = 0x0002,
    ISP_MOD_BALANCEFE   = 0x0001,
    ISP_MOD_BUTT
    
} ISP_MOD_BYPASS_E;


typedef enum xm_ISP_AE_FRAME_END_UPDATE_E
{
    ISP_AE_FRAME_END_UPDATE_0  = 0x0, //isp update gain and exposure  in the  same frame
    ISP_AE_FRAME_END_UPDATE_1  = 0x1, //isp update exposure one frame before  gain
    ISP_AE_FRAME_END_UPDATE_2  = 0x2, //isp update exposure one frame before  gain, isr config
    ISP_AE_FRAME_END_BUTT,
}ISP_AE_FRAME_END_UPDATE_E;

/* 4A settings                                                              */


typedef enum xmISP_AE_STRATEGY_E
{
	AE_EXP_NORMAL = 0,
	AE_EXP_HIGHLIGHT_PRIOR = 1,
	AE_EXP_LOWLIGHT_PRIOR  = 2,
	AE_EXP_CENTERLIGHT_PRIOR = 3,
	AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;


typedef struct xmISP_AE_RANGE_S
{
    XM_U32 u32Max;
    XM_U32 u32Min;
} ISP_AE_RANGE_S;

typedef enum xm_ISP_ANTIFLICKER_MODE_E
{
    /* The exposure time is fixed at several values. The minimum exposure time is 1/120 sec.
     * (60 Hz) or 1/100 sec. (50 Hz). Since the exposure time is synchronized with the fluorescent lighting,
     * rolling bar noise can be suppressed. In high-luminance environments, however, 
     * the exposure time is fixed to the minimum exposure time, it may lead to over exposure
     */
    ISP_ANTIFLICKER_MODE_0 = 0x0,

    /* In this mode, the exposure time is not fixed in high-luminance environments and the 
     * exposure time can be reduced  to the minimum . Use it when the exposure is to be controlled
     * in high-luminance environments. in low lumiance enviroment, the effect is the same as the above one
     */
    ISP_ANTIFLICKER_MODE_1 = 0x1,

    ISP_ANTIFLICKER_MODE_BUTT      
}ISP_ANTIFLICKER_MODE_E;

typedef struct xm_ISP_ANTIFLICKER_S
{
    XM_BOOL bEnable;
    XM_U8   u8Frequency;          /*RW, Range: usually this value is 50 or 60  which is the frequency of the AC power supply*/

    ISP_ANTIFLICKER_MODE_E  enMode;
} ISP_ANTIFLICKER_S;

typedef struct xm_ISP_AE_DELAY_S
{
    XM_U16 u16BlackDelayFrame;     // Need ToBrighter	delay = u16BlackDelayFrame x2 (FrameNums) 
    XM_U16 u16WhiteDelayFrame;    // Need ToDarker		delay = u16WhiteDelayFrame x2 (FrameNums) 
} ISP_AE_DELAY_S;

typedef struct xmISP_AE_ATTR_S
{
	/* base parameter */
	ISP_AE_RANGE_S stExpTimeRange;   /*RW,  sensor exposure time (unit: us ), Range: [0x0, 0xFFFFFFFF], it's related to the specific sensor */
	ISP_AE_RANGE_S stAGainRange;     /*RW,  sensor analog gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
	ISP_AE_RANGE_S stDGainRange;     /*RW,  sensor digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
	ISP_AE_RANGE_S stISPDGainRange;  /*RW,  ISP digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the ISP digital gain range */
	ISP_AE_RANGE_S stSysGainRange;   /*RW,  system gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor and ISP Dgain range */
	XM_U32 u32GainThreshold;         /*RW,  Gain threshold for slow shutter mode (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF] */
	XM_U32 u32LinesPer500ms;
	XM_S32 s32IspGainEx;
/********************************
u32UpdateCfg:
	bit[0~7]  period
	bit[8~15] shut 
	bit[16~23] snsGain
	bit[24~31] ispGain
********************************/
	XM_U32  u32UpdateCfg;	
	XM_U8  u8Speed;                  /*RW, Range: [0x0, 0xFF], AE adjust step*/
	XM_U8  u8Tolerance;              /*RW, Range: [0x0, 0xFF], AE adjust tolerance*/
	XM_U8  u8Compensation;           /*RW, Range: [0x0, 0xFF], AE compensation*/ 
	XM_U8   u8MaxHistOffset;         /*RW, Range: [0x0, 0xFF], Max hist offset*/
/********************************
u16AlgMode:
	0x00: fix V1 
		0x01: first V1, after stab bypass AE
	0x10: fix V2
		0x11: first V2, after stab bypass AE
	0x20: first V2, after stab V1 
********************************/
	XM_U16  u16AlgMode;
	ISP_AE_STRATEGY_E enAEStrategyMode;  /*RW, Support Highlight prior or Lowlight prior*/
	XM_U16  u16HistRatioSlope;       /*RW, Range: [0x0, 0xFFFF], AE hist ratio slope*/

	ISP_AE_MODE_E  enAEMode;         /*RW, AE mode(slow shutter/fix framerate)(onvif)*/
	ISP_ANTIFLICKER_S stAntiflicker;
	ISP_AE_DELAY_S stAEDelayAttr;

	/*AE weighting table*/
	XM_U8 au8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; /*Range :  [0, 0xF]*/
} ISP_AE_ATTR_S;


typedef struct xm_ISP_EXP_STA_INFO_S
{
	XM_U16 u16Exp_Hist16Value[16];	/*RO, 16 bins histogram,Range: [0x0, 0xFFFF]*/
	XM_U8  u8AveLum;                      	/*RO, average lum,Range: [0x0, 0xFF]*/
	XM_U8  u8ExpHistTarget[16];  		/*RW, Histogram target for bin 0/1/2/3/4 */
	XM_S16 s16HistError;   

	XM_U8 u8HisTarget;
}ISP_EXP_STA_INFO_S;

typedef struct xm_ISP_ME_ATTR_S
{
    XM_S32 s32AGain;            /*RW,  sensor analog gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    XM_S32 s32DGain;            /*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    XM_U32 u32ExpLine;          /*RW,  sensor exposure time (unit: line ), Range: [0x0, 0xFFFF],it's related to the specific sensor */
    XM_S32 s32IspGain;
    XM_S32 u32Exp; 
	
    XM_BOOL bManualExpLineEnable;
    XM_BOOL bManualAGainEnable;
    XM_BOOL bManualDGainEnable;
    XM_BOOL bManualIspGainEnable;
    XM_BOOL bManualExpEnable;
} ISP_ME_ATTR_S;

typedef struct xm_ISP_ME_ATTR_EX_S
{
    XM_S32 s32AGain;            /*RW,  sensor analog gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    XM_S32 s32DGain;            /*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    XM_U32 u32ISPDGain;         /*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific isp   */
    XM_U32 u32ExpLine;          /*RW,  sensor exposure time (unit: line ), Range: [0x0, 0xFFFF],it's related to the specific sensor */

    XM_BOOL bManualExpLineEnable;
    XM_BOOL bManualAGainEnable;
    XM_BOOL bManualDGainEnable;
    XM_BOOL bManualISPGainEnable;
} ISP_ME_ATTR_EX_S;

typedef struct xm_ISP_AF_ATTR_S
{
    XM_S32 s32FocusDistanceMax; /* the focuse range */
    XM_S32 s32FocusDistanceMin;

    /* weighting table */
    XM_U8 u8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN];    
} ISP_AF_ATTR_S;

typedef struct xm_ISP_FOUCS_STA_INFO_S
{
    XM_U16 u16FocusMetrics;     /*RO, The integrated and normalized measure of contrast*/
    XM_U16 u16ThresholdRead;    /*RO, The ISP recommend value of AF threshold*/
    XM_U16 u16ThresholdWrite;   /*RW, The user defined value of AF threshold (or 0 to use threshold from previous frame)*/
    XM_U16 u16FocusIntensity;   /*RO, The average brightness*/
    XM_U8  u8MetricsShift;      /*RW, Metrics scaling factor:the bigger value for this register means all zones metrics go higher,0x03 is the default, Range: [0x0, 0xF]*/
    XM_U8  u8NpOffset;          /*RW, The AF noise profile offset, Range: [0x0, 0xFF]*/
	XM_U16 u16ZoneMetrics[AE_ZONE_ROW][AE_ZONE_COLUMN]; /*RO, The zoned measure of contrast*/    
} ISP_FOCUS_STA_INFO_S;

typedef struct xm_ISP_MF_ATTR_S
{
    XM_S32 s32DefaultSpeed;     /* 1,default speed(unit:m/s).(onvif)*/    
} ISP_MF_ATTR_S;

typedef struct xm_ISP_AWB_CALIBRATION_S 
{ 
	XM_U32 u32WhiteNumLimit;
	XM_U16 au16KgOffset[3]; // 0: Min  1:512   2:Max
	XM_U8 au8Angle[4];   
} ISP_AWB_CALIBRATION_S; 

typedef struct xm_ISP_AWB_ATTR_S
{
	ISP_AWB_CALIBRATION_S stAWBCalibration; /*RW, AWB Calibration parameters*/
	XM_U16 u16SpeedToH;			/*RW, Range: [0x0, 0xFFF]  Ratio:[0x0, 0xFFFF]*/
	XM_U16 u16SpeedToL;			/*RW, Range: [0x0, 0xFFF]  Coef:[0x0, 0x400]*/
	XM_U8 u8RbOfst;        			/*RW, Range: [0x0, 0xFF]*/
	XM_U8 u8ROfst;				/*RW, Range: [0x0, 0xFF]*/
	XM_U8 u8BOfst;				/*RW, Range: [0x0, 0xFF]*/
	XM_U8 u8GmOfst;        			/*RW, Range: [0x0, 0xFF]*/
	XM_U8 u8ZoneSel;           		/*RW,  A value of 0 or 0x3F means global AWB, A value between 0 and 0x3F means zoned AWB */
	XM_U8 u8HighColorTemp;     /*RW, AWB max temperature in K/100, Range: [0x0, 0xFF], Recommended: [u8LowColorTemp, 100] */
	XM_U8 u8LowColorTemp;      /*RW, AWB min temperature in K/100, Range: [0x0, u8HighColorTemp), Recommended: [20, u8HighColorTemp] */

	/* weighting table */
	XM_U8 u8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN];  /*RW, Range :  [0, 0xF]*/
} ISP_AWB_ATTR_S;

typedef struct xm_ISP_AWB_IN_OUT_ATTR_S
{
    XM_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;
    XM_BOOL bOutdoorStatus;                /*in Auto mode, this is RO, in Manual mode, this is WO*/
    XM_U32 u32OutThresh;                   /*shutter time(in us) to judge indoor or outdoor */
    XM_U16 u16LowStart;                    /*5000K is recommend. [u8LowStart, u8HighStart] is the color temperature range of Day Light*/
    XM_U16 u16LowStop;                     /*4500K is recommend, u16LowStop < u8LowStart*/
    XM_U16 u16HighStart;                   /*6500K is recommend, u16HighStart > u8LowStart*/
    XM_U16 u16HighStop;                    /*8000K is recommend, u16HighStop > u8HighStart*/
    XM_BOOL bGreenEnhanceEn;               /*If this is enabled, Green channel will be enhanced based on the area of green plant, only take effect outdoor*/
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct xm_ISP_AWB_CT_LIMIT_ATTR_S
{
    XM_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;

    XM_U16 u16HighRgLimit;     /*RO, in Manual Mode, user should define the Max Rgain of High Color Temperature, u16HighRgLimit > u16LowRgLimit*/
    XM_U16 u16HighBgLimit;     /*RO, in Manual Mode, user should define the Min Bgain of High Color Temperature, u16HighBgLimit < u16LowBgLimit*/
    XM_U16 u16LowRgLimit;      /*RO, in Manual Mode, user should define the Min Rgain of Low Color Temperature*/
    XM_U16 u16LowBgLimit;      /*RO, in Manual Mode, user should define the Max Bgain of Low Color Temperature*/
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct xm_ISP_ADV_AWB_ATTR_S
{
    XM_BOOL bAccuPrior;         /*RW, recommend 0 for outdoor, 1 for indoor*/    
    XM_U8  u8Tolerance;         /*RW, Range:[0x0, 0xFF], AWB adjust tolerance,for outdoor, this value should be small,recomend 4*/   
    XM_U16 u16CurveLLimit;      /*RW, Range:[0x0, 0x100],   Left limit of AWB Curve, recomend for indoor 0xE0, outdoor 0xE0*/ 
    XM_U16 u16CurveRLimit;      /*RW, Range:[0x100, 0xFFF], Right Limit of AWB Curve,recomend for indoor 0x130, outdoor 0x120*/ 

    XM_BOOL bGainNormEn;     
    ISP_AWB_IN_OUT_ATTR_S       stInOrOut;    
    ISP_AWB_CT_LIMIT_ATTR_S     stCTLimit;
} ISP_ADV_AWB_ATTR_S;

typedef struct xm_ISP_AWB_LIGHTSOURCE_INFO_S
{
    XM_U16 u16WhiteRgain;        /*G/R of White points at this light source*/
    XM_U16 u16WhiteBgain;        /*G/B of White points at this light source*/
    XM_U16 u16ExpQuant;          /*shtter time * again * dgain >> 4, Not support Now*/
    XM_BOOL bLightStatus;         /*RW, 0: idle  1:busy */	
} ISP_AWB_LIGHTSOURCE_INFO_S;

typedef struct xm_ISP_AWB_ADD_LIGHTSOURCE_S
{
    XM_BOOL  bLightEnable;                     /*Enable special light source function*/
    ISP_AWB_LIGHTSOURCE_INFO_S	stLightInfo[LIGHTSOURCE_NUM]; 
} ISP_AWB_ADD_LIGHTSOURCE_S;

typedef struct xm_ISP_WB_ZONE_STA_INFO_S
{
    XM_U16 u16Rg;               /*RO, Zoned WB output G/R, Range : [0x0, 0xFFF]*/
    XM_U16 u16Bg;               /*RO, Zoned WB output G/B, Range : [0x0, 0xFFF]*/
    XM_U32 u32Sum;              /*RO, Zoned WB output population,Range: [0x0, 0xFFFFFFFF]*/  
} ISP_WB_ZONE_STA_INFO_S;

typedef struct xm_ISP_WB_STA_INFO_S
{
    XM_U16 u16WhiteLevel;       /*RW, Upper limit of valid data for white region, Range: [0x0, 0xFFFF]*/
    XM_U16 u16BlackLevel;       /*RW, Lower limit of valid data for white region, Range: [0x0, u16WhiteLevel)*/
    XM_U16 u16CbMax;            /*RW, Maximum value of B/G for white region, Range: [0x0,0xFFF]*/
    XM_U16 u16CbMin;            /*RW, Minimum value of B/G for white region, Range: [0x0, u16CbMax)*/
    XM_U16 u16CrMax;            /*RW, Maximum value of R/G for white region, Range: [0x0, 0xFFF]*/
    XM_U16 u16CrMin;            /*RW, Minimum value of R/G for white region, Range: [0x0, u16CrMax)*/
    
    XM_U16 u16GRgain;           /*RO, Global WB output G/R, Range: [0x0, 0xFFFF]*/
    XM_U16 u16GBgain;           /*RO, Global WB output G/B, Range: [0x0, 0xFFFF]*/
    XM_U32 u32GSum;             /*RO, Global WB output population, Range: [0x0, 0xFFFF]*/

    XM_U32 u32Rgain;            /*RO, gain value of R channel for AWB, Range: [0x0, 0xFFF]*/
    XM_U32 u32Ggain;            /*RO, gain value of G channel for AWB, Range: [0x0, 0xFFF]*/
    XM_U32 u32Bgain;            /*RO, gain value of B channel for AWB, Range: [0x0, 0xFFF]*/
    
    ISP_WB_ZONE_STA_INFO_S stZoneSta[AE_ZONE_ROW][AE_ZONE_COLUMN];  /*RO, Zoned WB statistics*/
} ISP_WB_STA_INFO_S;

typedef struct xm_ISP_MWB_ATTR_S        
{
    XM_U16 u16Rgain;            /*RW, Multiplier for R color channel, Range: [0x0, 0xFFF]*/
    XM_U16 u16Ggain;            /*RW, Multiplier for G color channel, Range: [0x0, 0xFFF]*/
    XM_U16 u16Bgain;            /*RW, Multiplier for B color channel, Range: [0x0, 0xFFF]*/
    
} ISP_MWB_ATTR_S;



// CCM
typedef struct xmISP_COLORMATRIX_AUTO_S
{   XM_U16 u16HighColorTemp;    /*RW,  Range: [u16MidColorTemp + 400,  10000]*/
    XM_U16 au16HighCCM[12];      /*RW,  Range: [0x0,  0xFFFF]*/
    XM_U16 u16MidColorTemp;     /*RW,  the MidColorTemp should be at least 400 smaller than HighColorTemp, Range: [u16LowColorTemp + 400,  u16HighColorTemp-400]*/
    XM_U16 au16MidCCM[12];       /*RW,  Range: [0x0,  0xFFFF]*/
    XM_U16 u16LowColorTemp;     /*RW,  the LowColorTemp should be at least 400 smaller than HighColorTemp, Range: [0,  u16MidColorTemp-400]*/
    XM_U16 au16LowCCM[12];       /*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_AUTO_S;


typedef struct xmISP_COLORMATRIX_MANUAL_S
{   
	XM_U16 au16CCM[12];      /*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_MANUAL_S;

typedef struct xmISP_COLORMATRIX_ATTR_S
{   
	XM_BOOL bByPass;
	ISP_OP_TYPE_E enOpType;
	ISP_COLORMATRIX_MANUAL_S stManual;
	ISP_COLORMATRIX_AUTO_S stAuto;
} ISP_COLORMATRIX_ATTR_S;





typedef struct xm_ISP_COLORTONE_S
{
  XM_U16 u16RedCastGain;        /*RW,  Range: [0x100, 0xFFFF], adjust the final red channel  tone of the picture */
  XM_U16 u16GreenCastGain;      /*RW,  Range: [0x100, 0xFFFF], adjust the final green channel  tone of the picture*/
  XM_U16 u16BlueCastGain;       /*RW,  Range: [0x100, 0xFFFF], adjust the final blue channel  tone of the picture*/ 
  
}ISP_COLORTONE_S;

typedef enum xm_ISP_SAT_MODE_E
{
    SAT_MODE_NOISE    = 0,	// Lower Noise
    SAT_MODE_COLOR  = 1,		// Better Color
    SAT_MODE_BUTT
} ISP_SAT_MODE_E;


typedef struct xmISP_SATURATION_AUTO_S
{
    XM_U8   au8Sat[ISP_AUTO_STENGTH_NUM];           /*RW,  Range: [0, 0xFF] */ 
}ISP_SATURATION_AUTO_S;

typedef struct xmISP_SATURATION_MANUAL_S
{
	XM_U8	u8Saturation;
}ISP_SATURATION_MANUAL_S;

typedef struct xmISP_SATURATION_ATTR_S
{
	ISP_SAT_MODE_E enSatMode;
	ISP_OP_TYPE_E enOpType;
	ISP_SATURATION_MANUAL_S	stManual;
	ISP_SATURATION_AUTO_S stAuto;
}ISP_SATURATION_ATTR_S;



typedef enum xm_ISP_IRIS_TYPE_E
{
    ISP_IRIS_DC_TYPE = 0,
    ISP_IRIS_P_TYPE,
    
    ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;



typedef struct xm_ISP_MI_ATTR_S
{
    XM_BOOL bEnable;            /* manual iris  on/off*/
    XM_U32  u32IrisHoldValue;   /*RW, iris hold value, Range: [0x0, 0x3E8]*/
    XM_U16  u16ApePercent;      /* the percent of the iris's aperture, range is [0~100]. */
} ISP_MI_ATTR_S;


typedef struct xmISP_DRC_MANUAL_ATTR_S 
{
	XM_U8 u8Strength;
}ISP_DRC_MANUAL_ATTR_S;

typedef struct xmISP_DRC_AUTO_ATTR_S 
{
	XM_U8 au8Sth[ISP_AUTO_STENGTH_NUM];           /*RW,  Range: [0, 0xFF] */ 
}ISP_DRC_AUTO_ATTR_S; 

#define DRC_IDX_NUM		(22)

typedef struct xm_ISP_DRC_ATTR_S
{
    XM_BOOL bEnable;        
	ISP_OP_TYPE_E enOpType; 
	XM_U8 u8GlobalSth;
	ISP_DRC_MANUAL_ATTR_S stManual; 
	ISP_DRC_AUTO_ATTR_S stAuto;
	XM_U16  au16ToneMappingValue[DRC_IDX_NUM]; /*RW, Range: [0x0, 0xffff] */
} ISP_DRC_ATTR_S;



typedef enum xmISP_STATIC_DP_TYPE_E{
    ISP_STATIC_DP_BRIGHT = 0x0,  
    ISP_STATIC_DP_DARK,    
    ISP_STATIC_DP_BUTT
} ISP_STATIC_DP_TYPE_E;

typedef struct xmISP_DYDPC_AUTO_S
{
	XM_U8 au8Sth[ISP_AUTO_STENGTH_NUM];           /*RW,  Range: [0, 0xFF] */ 
}ISP_DYDPC_AUTO_S;

typedef struct xmISP_DYDPC_MANUAL_S
{
	XM_U8 u8Sth;
}ISP_DYDPC_MANUAL_S;

typedef struct xmISP_DYDPC_ATTR_S
{
	ISP_OP_TYPE_E enOpType;
	ISP_DYDPC_MANUAL_S	stManual;
	ISP_DYDPC_AUTO_S stAuto;
}ISP_DYDPC_ATTR_S;

typedef struct xmISP_STDPC_ATTR_S
{
	// StaticDpc
	XM_BOOL bEnableStatic;
	XM_BOOL bEnableDetect;
	ISP_STATIC_DP_TYPE_E enStaticDPType;    /* Select static bright/dark defect-pixel calibration. */
    
	ISP_TRIGGER_STATUS_E enTriggerStatus;   /*R   status of bad pixel trigger*/
	XM_U16 	u16BadPixelThreshMin;          	/*RW,  Range: [0, 0xFFF] */
	XM_U16 	u16BadPixelThreshMax;        	/*RW,  Range: [0, 0xFFF] */
	XM_U16  u16BadPixelThresh;				/*R   Range: [0, 0xFFF] */
	XM_U16  u16BadPixelCountMax;            /*RW, limit of max number of bad pixel,  Range: [0, 0x3FF] */
	XM_U16  u16BadPixelCount;               /*R	 DP count:[0, 0x3FF]*/
	XM_U16  u16BadPixelTriggerTime;         /*RW, time limit for bad pixel trigger, in frame number ,Range: [0x0, 0x640]*/
	XM_U32  u32BadPixelTable[STATIC_DP_COUNT_MAX];         /*RW, Range: [0x0, 0x3FFFFF],the first 11 bits represents the X coordinate of the defect pixel, the second 11 bits represent the Y coordinate of the defect pixel*/
} ISP_STDPC_ATTR_S;

typedef struct xmISP_SRAMDPC_ATTR_S
{
	XM_U16	u16SbpcHighThr;
	XM_U16	u16SbpcLowThr;
	XM_U16	u16SbpcPixelCount;
	XM_U32* u16SbpcPdataaddr;
} ISP_SRAMDPC_ATTR_S;

typedef struct xmISP_FPN_ATTR_S
{
	XM_U8	u8FpnBayerSel;
	XM_U16	u16FpnCpuAddr;
	XM_U32  u16FpnCpuData;
} ISP_FPN_ATTR_S;
typedef struct xmISP_FPN_SAINFO_S
{
	XM_U32  fpn_enable;
	XM_U32	fpn_sensor_id;
	RECT_S  fpn_stWndRect;
}ISP_FPN_SAINFO_S;

typedef struct xm_ISP_DIS_ATTR_S
{
    XM_BOOL bEnable;
} ISP_DIS_ATTR_S;

typedef struct xm_ISP_DIS_INFO_S
{
    XM_S8 s8Xoffset;         /*RW, Range: [0x00, 0x80]*/
    XM_S8 s8Yoffset;         /*RW, Range: [0x80, 0xFF]*/   
} ISP_DIS_INFO_S;


typedef struct xm_ISP_SHADING_ATTR_S
{
    XM_BOOL Enable;
} ISP_SHADING_ATTR_S;

typedef struct xm_ISP_SHADINGTAB_S
{
    XM_U16 u16ShadingCenterR_X;  /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingCenterR_Y;  /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingCenterG_X;  /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingCenterG_Y;  /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingCenterB_X;  /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingCenterB_Y;  /*RW, Range: [0x0, 0xFFFF]*/

    XM_U16 u16ShadingTable_R[SHADING_TABLE_NODE_NUMBER_MAX]; /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingTable_G[SHADING_TABLE_NODE_NUMBER_MAX]; /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingTable_B[SHADING_TABLE_NODE_NUMBER_MAX]; /*RW, Range: [0x0, 0xFFFF]*/

    XM_U16 u16ShadingOffCenter_R;  /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingOffCenter_G;  /*RW, Range: [0x0, 0xFFFF]*/
    XM_U16 u16ShadingOffCenter_B;  /*RW, Range: [0x0, 0xFFFF]*/

    XM_U16 u16ShadingTableNodeNumber;  /*RW, Range: [0x0, SHADING_TABLE_NODE_NUMBER_MAX]*/
} ISP_SHADINGTAB_S;

typedef struct xm_ISP_DENOISE_ATTR_S
{
    XM_BOOL bEnable;
    XM_BOOL bManualEnable;
    XM_U8 u8ThreshTarget;       /*RW,  Noise reduction effect for high spacial frequencies Range: [0x0, u8ThreshTarget]*/
    XM_U8 u8ThreshMax;          /*RW,  Noise reduction effect for high spacial frequencies, Range: [0x0, 0xFF] */
    XM_U8 u8SnrThresh[8];       /*RW,  Noise reduction target value array for  different iso, Range: [0x0, 0xFF],*/
} ISP_DENOISE_ATTR_S;



typedef enum xm_ISP_GAMMA_CURVE_E
{
    ISP_GAMMA_CURVE_1_6 = 0x0,           /* 1.6 Gamma curve */
    ISP_GAMMA_CURVE_1_8 = 0x1,           /* 1.8 Gamma curve */
    ISP_GAMMA_CURVE_2_0 = 0x2,           /* 2.0 Gamma curve */
    ISP_GAMMA_CURVE_2_2 = 0x3,           /* 2.2 Gamma curve */
    ISP_GAMMA_CURVE_DEFAULT = 0x4,       /* default Gamma curve */
    ISP_GAMMA_CURVE_SRGB = 0x5,
    ISP_GAMMA_CURVE_USER_DEFINE = 0x6,   /* user defined Gamma curve, Gamma Table must be correct */
    ISP_GAMMA_CURVE_BUTT
} ISP_GAMMA_CURVE_E;

typedef struct xm_ISP_GAMMA_ATTR_S
{
	XM_BOOL bEnable;
	ISP_GAMMA_CURVE_E enCurveType;
	XM_U16 u16Table[GAMMA_NODE_NUMBER];
} ISP_GAMMA_ATTR_S;



typedef struct xm_ISP_PARA_REC_S
{
    XM_BOOL bInit;
    XM_BOOL bTmCfg;
    XM_BOOL bAttrCfg;
    
    ISP_INPUT_TIMING_S stInputTiming;
    ISP_IMAGE_ATTR_S stImageAttr;

    XM_U32 u32ModFlag;

    /* Exposure */
    ISP_OP_TYPE_E enExpType;
    ISP_AE_ATTR_S stAEAttr;
    ISP_ME_ATTR_S stMEAttr;

    /* White Balance */
    ISP_OP_TYPE_E enWBType;
    ISP_AWB_ATTR_S stAWBAttr;
    ISP_MWB_ATTR_S stMWBAttr;
} ISP_PARA_REC_S;

/*Crosstalk Removal*/
typedef struct xm_ISP_CR_ATTR_S
{
    XM_BOOL  bEnable;
    XM_U8    u8Strength[8];  /*Range: [0x0, 0xFF] */
    XM_U8    u8Sensitivity; /*Range: [0x0, 0xFF],this register is  not recommended  to change */
    XM_U16   u16Threshold;  /*Range: [0x0, 0xFFFF],this register is  not recommended  to change */
    XM_U16   u16Slope;      /*Range: [0x0, 0xFFFF],this register is  not recommended  to change */
}ISP_CR_ATTR_S;

typedef struct xm_ISP_ANTIFOG_S
{
    XM_BOOL bEnable;
    XM_U8   u8Strength;     /*Range: [0x0, 0xFF]*/
} ISP_ANTIFOG_S;

typedef struct xm_ISP_ANTI_FALSECOLOR_S
{
    XM_BOOL bEnable;
    XM_U8  u8Strength;      /* Range: [0x0, 0xFF], the recommended range is [0x0, 0x95], the normal 
                                * color will gradually be eroded when this register is larger than 0x95. */
} ISP_ANTI_FALSECOLOR_S;

/*users query ISP state information*/
typedef struct xm_ISP_INNER_STATE_INFO_S
{
	XM_U32 u32ExposureTime;         /* RO,  Range: [0x0, 0xFFFF] */                
	XM_U32 u32AnalogGain;           /* RO,Range: [0x0, 0xFFFF] */                
	XM_U32 u32DigitalGain;          /* RO,Range: [0x0, 0xFFFF] */  
	XM_U32 u32IspDGain;          /* RO,Range: [0x0, 0xFFFF] */  
	XM_U32 u32Exposure;             /* RO,Range: [0x0, 0xFFFFFFFF] */     
	XM_U32 u32AllGain;
	XM_U16 u16AE_Hist16Value[16]; /* RO, 256 bins histogram */
	XM_U8 u8LumError;   
	XM_U8  u8AveLum;                /* RO, Range: [0x0, 0xFF] */    
	XM_BOOL bExposureIsMAX;
}ISP_INNER_STATE_INFO_S;

/*Demosaic Attr*/
typedef struct xm_ISP_DEMOSAIC_ATTR_S
{
    XM_U8   u8VhSlope;              /*RW,Range: [0x0, 0xFF] */
    XM_U8   u8AaSlope;              /*RW,Range: [0x0, 0xFF] */
    XM_U8   u8VaSlope;              /*RW,Range: [0x0, 0xFF] */
    XM_U8   u8UuSlope;              /*RW,Range: [0x0, 0xFF] */
    XM_U16  u16VhThresh;            /*RW,Range: [0x0, 0xFFFF] */   
    XM_U16  u16AaThresh;            /*RW,Range: [0x0, 0xFFFF] */
    XM_U16  u16VaThresh;            /*RW,Range: [0x0, 0xFFFF] */
    XM_U16  u16UuThresh;            /*RW,Range: [0x0, 0xFFFF] */
    XM_U8   u8DemosaicConfig;       /*RW,Range: [0x0, 0xFF] */
    XM_U8   u8LumThresh[8];         /*RW, Range:[0x0, 0xFF] */
    XM_U8   u8NpOffset[8];          /*RW, Range:[0x0, 0xFF] */
}ISP_DEMOSAIC_ATTR_S;



typedef struct xmISP_BLACKLVL_AUTO_S
{
    XM_U16   au16Blc[ISP_AUTO_STENGTH_NUM];           /*RW,  Range: [0, 0xFFFF] */ 
}ISP_BLACKLVL_AUTO_S;

typedef struct xmISP_BLACKLVL_MANUAL_S
{
	XM_U16	u16Blc;
}ISP_BLACKLVL_MANUAL_S;

typedef struct xmISP_BLACKLVL_ATTR_S
{
	ISP_OP_TYPE_E enOpType;
	ISP_BLACKLVL_MANUAL_S	stManual;
	ISP_BLACKLVL_AUTO_S 		stAuto;
}ISP_BLACKLVL_ATTR_S;






/*ISP debug information*/
typedef struct xm_ISP_DEBUG_INFO_S
{
    XM_BOOL bDebugEn;       /*RW, 1:enable debug, 0:disable debug*/
    XM_U32  u32PhyAddr;     /*RW, phy address of debug info */
    XM_U32  u32Depth;       /*RW, depth of debug info */
} ISP_DEBUG_INFO_S;

typedef struct xm_ISP_DBG_ATTR_S
{
    XM_U32  u32Rsv;         /* need to add member */
} ISP_DBG_ATTR_S;

typedef struct xm_ISP_DBG_STATUS_S
{
    XM_U32  u32FrmNumBgn;
    XM_U32  u32Rsv;         /* need to add member */
    XM_U32  u32FrmNumEnd;
} ISP_DBG_STATUS_S;


typedef struct xm_ISP_VD_INFO_S
{
  XM_U32  u32Reserved;          /*RO, Range: [0x0, 0xFFFFFFFF] */
}ISP_VD_INFO_S;


typedef struct xm_ISP_REG_ATTR_S
{
    XM_U32 u32IspRegAddr;
    XM_U32 u32IspRegSize;
    XM_U32 u32IspExtRegAddr;
    XM_U32 u32IspExtRegSize;
    XM_U32 u32AeExtRegAddr;
    XM_U32 u32AeExtRegSize;
    XM_U32 u32AwbExtRegAddr;
    XM_U32 u32AwbExtRegSize;
} ISP_REG_ATTR_S;



typedef struct xmISP_EXPOSURE_ATTR_S
{    
    XM_BOOL         bByPass;
    ISP_OP_TYPE_E   enOpType;
    ISP_ME_ATTR_S   stManual;
    ISP_AE_ATTR_S   stAuto;    
} ISP_EXPOSURE_ATTR_S;


typedef struct xmISP_WB_ATTR_S
{
    XM_BOOL bByPass;
    ISP_OP_TYPE_E   enOpType;
    ISP_MWB_ATTR_S  stManual;
    ISP_AWB_ATTR_S  stAuto;
} ISP_WB_ATTR_S;

typedef enum xmISP_STATE_E
{
    ISP_STATE_INIT     = 0, 
    ISP_STATE_SUCCESS  = 1,
    ISP_STATE_TIMEOUT  = 2,
    ISP_STATE_BUTT
} ISP_STATUS_E;


typedef struct xmISP_AI_CALIBRATE_S
{
    XM_BOOL bEnable;     /*iris calibration on/off*/

    XM_U16 u16StopValue;    /*RW, the initial stop value for AI calibraiton, Range: [0x0,0x3E8]*/
    XM_U16 u16CloseValue;   /*RW, the drive value to close Iris, Range: [0x0,0x3E8], Recommended value: [700, 900]. A larger value means faster.*/
    XM_U16 u16CalibrateTime;  /*RW, frame numbers of AI calibraiton lasting time. > 600, [0x0, 0xFFF]*/
    XM_U8  u8InertiaValue;  /*RW, frame numbers of  AI moment of inertia, Range: [0x0, 0xFF],the recommended value is between[0x3, 0xa]*/

    XM_U32  u32HoldValue;    /*RO, Range: [0x0, 0x3E8], iris hold value*/
    ISP_STATUS_E enStatus;   /*RO, status of calibraiton*/
} ISP_AI_CALIBRATE_S;

typedef struct xmISP_AI_ATTR_S
{
    XM_U32  u32HoldValue;       /*RW, Range: [0x0, 0x3E8], iris hold value*/
} ISP_AI_ATTR_S;

typedef struct xmISP_IRIS_ATTR_S
{
    XM_BOOL bEnable;        /* iris enable/disable */
    ISP_OP_TYPE_E   enOpType;
    ISP_IRIS_STATUS_E enIrisStatus;  /*RW, status of Iris*/
    ISP_AI_ATTR_S   stAIAttr;
    ISP_MI_ATTR_S   stMIAttr;
} ISP_IRIS_ATTR_S;

typedef struct xmISP_SHARPEN_MANUAL_ATTR_S
{
    XM_U8 u8SharpenD;       /* RW,  Range:[0, 0xFF]. */	// _H
    XM_U8 u8SharpenUd;      /* RW,  Range:[0, 0xFF]. */	// _M
    XM_U8 u8SharpenKd;
} ISP_SHARPEN_MANUAL_ATTR_S;
typedef struct xmISP_SHARPEN_AUTO_ATTR_S
{
    XM_U8 u8GlobalSth;	// [0, 0xFF]
    XM_U8 au8SharpenD[ISP_AUTO_STENGTH_NUM];    /* RW, Range: [0, 0xFF]. */	// _H
    XM_U8 au8SharpenUd[ISP_AUTO_STENGTH_NUM];   /* RW, Range: [0, 0xFF]. */	// _M
    XM_U8 au8SharpenKd[ISP_AUTO_STENGTH_NUM];   /* RW, Range: [0, 0xFF]. */
} ISP_SHARPEN_AUTO_ATTR_S;

typedef struct xmISP_SHARPEN_SCANBLE_ATTR_S
{
	XM_U8 	enScanD;
}ISP_SHARPEN_SCANBLE_ATTR_S;

typedef struct xmISP_SHARPEN_ATTR_S
{
	XM_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_SHARPEN_SCANBLE_ATTR_S stScan;
	ISP_SHARPEN_MANUAL_ATTR_S stManual;
	ISP_SHARPEN_AUTO_ATTR_S   stAuto;
} ISP_SHARPEN_ATTR_S;


typedef struct xmISP_SHARPENV2_MANUAL_ATTR_S
{
	XM_U8 u8TextureThr;
	XM_U8 u8SharpenD;       /* RW,  Range:[0, 0xFF]. */	// _H
	XM_U8 u8SharpenUd;      /* RW,  Range:[0, 0xFF]. */	// _M
	XM_U8 u8SharpenKd;
	XM_U8 u8DetailThr;
	XM_U8 u8DetailCtrl;
} ISP_SHARPENV2_MANUAL_ATTR_S;

typedef struct xmISP_SHARPENV2_AUTO_ATTR_S
{
	XM_U8 u8GlobalSth;	// [0, 0xFF]
	XM_U8 au8TextureThr[ISP_AUTO_STENGTH_NUM];
	XM_U8 au8SharpenD[ISP_AUTO_STENGTH_NUM];    /* RW, Range: [0, 0xFF]. */	// _H
	XM_U8 au8SharpenUd[ISP_AUTO_STENGTH_NUM];   /* RW, Range: [0, 0xFF]. */	// _M
	XM_U8 au8SharpenKd[ISP_AUTO_STENGTH_NUM];   /* RW, Range: [0, 0xFF]. */
	XM_U8 au8DetailThr[ISP_AUTO_STENGTH_NUM];
	XM_U8 au8DetailCtrl[ISP_AUTO_STENGTH_NUM];
} ISP_SHARPENV2_AUTO_ATTR_S;

typedef struct xmISP_SHARPENV2_ATTR_S
{
	XM_U8 u8AlgChoice;	// 0:Use V1   1:Use V2
	XM_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_SHARPENV2_MANUAL_ATTR_S stManual;
	ISP_SHARPENV2_AUTO_ATTR_S   stAuto;
} ISP_SHARPENV2_ATTR_S;

typedef struct xmISP_2DNR_MANUAL_ATTR_S
{
    XM_U8 u8Thresh;                            /* RW, Range: [0x0, 0xFF]. Noise reduction effect for high spacial frequencies. */
} ISP_2DNR_MANUAL_ATTR_S;
typedef struct xmISP_2DNR_AUTO_ATTR_S
{
    XM_U8 u8GlobalSth;	// [0, 0xFF]
    XM_U8 au8Thresh[ISP_AUTO_STENGTH_NUM];     /* RW, Range: [0x0, 0xFF]. Noise reduction target value array for different ISO. */
} ISP_2DNR_AUTO_ATTR_S;

typedef struct xmISP_YCNR_MANUAL_ATTR_S
{
    XM_U8 u8Thresh;                            /* RW, Range: [0x0, 0xFF]. Noise reduction effect for high spacial frequencies. */
} ISP_YCNR_MANUAL_ATTR_S;
typedef struct xmISP_YCNR_AUTO_ATTR_S
{
    XM_U8 au8Thresh[ISP_AUTO_STENGTH_NUM];     /* RW, Range: [0x0, 0xFF]. Noise reduction target value array for different ISO. */
} ISP_YCNR_AUTO_ATTR_S;

typedef struct xmISP_3DNR_MANUAL_ATTR_S
{
    XM_U8 u8TfStrength;
    XM_U8 u8SfStrength;	
} ISP_3DNR_MANUAL_ATTR_S;
typedef struct xmISP_3DNR_AUTO_ATTR_S
{
    XM_U8 u8GlobalSth;	 // [0, 0xFF]
    XM_U8 au8TfStrength[ISP_AUTO_STENGTH_NUM];   
    XM_U8 au8SfStrength[ISP_AUTO_STENGTH_NUM];     
} ISP_3DNR_AUTO_ATTR_S;



typedef struct xmISP_2DNR_ATTR_S
{
    XM_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_2DNR_MANUAL_ATTR_S stManual;
    ISP_2DNR_AUTO_ATTR_S   stAuto;
} ISP_2DNR_ATTR_S;



typedef struct xmISP_YCNR_ATTR_S
{
	XM_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_YCNR_MANUAL_ATTR_S stManual;
	ISP_YCNR_AUTO_ATTR_S   stAuto;
} ISP_YCNR_ATTR_S;


typedef struct xmISP_3D_RGB_Y

{
	ISP_OP_TYPE_E enOpType;
	XM_U16	Manual_RgbY;
	XM_U16	Auto_RgbY;
} ISP_3DRGB_Y_RATIO;

typedef struct xmISP_3DNR_ATTR_S
{
    XM_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_3DNR_MANUAL_ATTR_S stManual;
    ISP_3DNR_AUTO_ATTR_S   stAuto;
	ISP_3DRGB_Y_RATIO	stRgbYamp;
} ISP_3DNR_ATTR_S;

typedef struct xmISP_NR_INFO_S
{
	XM_U8 au8Nr[4];
} ISP_NR_INFO_S;


typedef struct xmISP_3DNRV2_MANUAL_ATTR_S
{
#if 0
	XM_U8 u8MdTh;			// 动静判决阈值          v
	XM_U8 u8TfStrength_S;	// 静止区域时域          v
	XM_U8 u8TfStrength_M;	// 运动区域时域
	XM_U8 u8SfStrength_S;	// 静止区域空域
	XM_U8 u8SfStrength_M;	// 运动区域空域	       v
#else
	XM_U8 u8MdTh;			// 动静判决阈值
	XM_U8 u8TfOfst_S;	// 静止区域时域增强		   
	XM_U8 u8TfStrength;	// 等同V1
	XM_U8 u8SfStrength;	// 等同V1
	XM_U8 u8Rsv;		// 保留
#endif
} ISP_3DNRV2_MANUAL_ATTR_S;	

typedef struct xmISP_3DNRV2_AUTO_ATTR_S
{
#if 0
	XM_U8 u8GlobalSth;	 // [0, 0xFF]
	XM_U8 au8MdTh[ISP_AUTO_STENGTH_NUM];			// 动静判决阈值          v
	XM_U8 au8TfStrength_S[ISP_AUTO_STENGTH_NUM];		// 静止区域时域          v
	XM_U8 au8TfStrength_M[ISP_AUTO_STENGTH_NUM];		// 运动区域时域
	XM_U8 au8SfStrength_S[ISP_AUTO_STENGTH_NUM];		// 静止区域空域
	XM_U8 au8SfStrength_M[ISP_AUTO_STENGTH_NUM];		// 运动区域空域	       v
#else
	XM_U8 u8GlobalSth;	 // [0, 0xFF] 
	XM_U8 au8MdTh[ISP_AUTO_STENGTH_NUM];			// 动静判决阈值
	XM_U8 au8TfOfst_S[ISP_AUTO_STENGTH_NUM];			// 静止区域时域增强		 
	XM_U8 au8TfStrength[ISP_AUTO_STENGTH_NUM];		// 等同V1
	XM_U8 au8SfStrength[ISP_AUTO_STENGTH_NUM];		// 等同V1
	XM_U8 au8Rsv[ISP_AUTO_STENGTH_NUM]; 			// 保留
#endif
} ISP_3DNRV2_AUTO_ATTR_S;


typedef struct xmISP_3DNRV2_ATTR_S
{
	XM_U8 u8AlgChoice;	// 0:Use 3DNrV1   1:Use 3DNrV2
	XM_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_3DNRV2_MANUAL_ATTR_S stManual;
	ISP_3DNRV2_AUTO_ATTR_S   stAuto;
} ISP_3DNRV2_ATTR_S;


typedef union xmISP_MODULE_CTRL_U
{
    XM_U32  u32Key;
    struct
    {
        XM_U32  bitBypassVideoTest  : 1 ;   /* [0] */
		XM_U32  bitBypassDPC        : 1 ;   /* [1] */
        XM_U32  bitBypassBlc  		: 1 ;   /* [2] */
		
        XM_U32  bitBypassGammaFe 	: 1 ;   /* [3] */
		XM_U32  bitBypassShading    : 1 ;   /* [4] */
		XM_U32  bitBypassWBGain     : 1 ;   /* [5] */
		XM_U32  bitBypassNR2D   	: 1 ;   /* [6] */
		XM_U32  bitBypassNR3D  		: 1 ;   /* [7] */

       	XM_U32  bitBypassDRC        : 1 ;   /* [8] */
        XM_U32  bitBypassColorMatrix: 1 ;   /* [9] */
        XM_U32  bitBypassGamma      : 1 ;   /* [10] */
		
        XM_U32  bitDemoire          : 1 ;   /* [11] */
        XM_U32  bitCC				: 1 ;	/* [12] */
		XM_U32  bitBypassSharpen    : 1 ;   /* [13] */
		XM_U32  bitRsv1             : 1 ;   /* [14] */
        XM_U32  bitRsv2             : 1 ;   /* [15] */
        XM_U32  bit2Rsv3            : 2 ;   /* [16:17] */
        XM_U32  bit2Rsv4            : 2 ;   /* [18:19] */
        XM_U32  bit11Rsv5           : 11;   /* [20:30] */
        XM_U32  bitBypassAll        : 1 ;   /* [31] */
    };
}ISP_MODULE_CTRL_U;

typedef struct xmISP_WB_INFO_S
{
    XM_U16 u16Rgain;             /*RO, AWB result of R color channel, Range: [0x0, 0xFFF]*/
    XM_U16 u16Ggain;            /*RO, AWB result of Gr color channel, Range: [0x0, 0xFFF]*/
    XM_U16 u16Bgain;             /*RO, AWB result of B color channel, Range: [0x0, 0xFFF]*/
    XM_U16 u16Saturation;        /*RO, Current saturation, Range:[0x0, 0xFF]*/
    XM_U16 u16ColorTemp;         /*RO, Detect color temperature, maybe out of color cemeprature range*/
    XM_U16 au16CCM[12];           /*RO, Current color correction matrix*/

	XM_U8 u8RbOfst;
	XM_U8 u8GmOfst;
}ISP_WB_INFO_S;

typedef struct xmISP_WB_INFO_S_V2
{
	XM_U16 u16Rgain;
	XM_U16 u16Ggain;
	XM_U16 u16Bgain;
}ISP_WB_INFO_S_V2;

#if 0
typedef enum xmISP_CSC_TYPE_E
{
    ISP_CSC_TYPE_601 = 0,                /* CSC Type: 601 */
    ISP_CSC_TYPE_709,                    /* CSC Type: 709 */
    ISP_CSC_TYPE_BUTT,
} ISP_CSC_TYPE_E;
#endif
typedef struct xmISP_CSC_MANUAL_ATTR_S
{
	XM_U8 u8LumaVal;                  /* Luminance: [0 ~ 100] */
	XM_U8 u8ContrVal;                 /* Contrast: [0 ~ 100] */
	XM_U8 u8HueVal;                   /* Hue: [0 ~ 100] */
	XM_U8 u8SatuVal;                  /* Satuature: [0 ~ 100] */
}ISP_CSC_MANUAL_ATTR_S;

typedef struct xmISP_CSC_AUTO_ATTR_S
{
	XM_U8 au8LumaVal[ISP_AUTO_STENGTH_NUM];                  /* Luminance: [0 ~ 100] */
	XM_U8 au8ContrVal[ISP_AUTO_STENGTH_NUM];                 /* Contrast: [0 ~ 100] */
	XM_U8 au8HueVal[ISP_AUTO_STENGTH_NUM];                   /* Hue: [0 ~ 100] */
	XM_U8 au8SatuVal[ISP_AUTO_STENGTH_NUM];                  /* Satuature: [0 ~ 100] */
}ISP_CSC_AUTO_ATTR_S;

typedef struct xmISP_CSC_STATE_ATTR_S
{
	XM_U8 u8LumaVal;                  /* Luminance: [0 ~ 100] */
	XM_U8 u8ContrVal;                 /* Contrast: [0 ~ 100] */
	XM_U8 u8HueVal;                   /* Hue: [0 ~ 100] */
	XM_U8 u8SatuVal;                  /* Satuature: [0 ~ 100] */
}ISP_CSC_STATE_S;

typedef struct xmISP_CSC_ATTR_S
{
 //   ISP_CSC_TYPE_E enCscType;          /* 601 or 709 */
	XM_BOOL bEnable;
 	ISP_OP_TYPE_E enOpType;	
	ISP_CSC_MANUAL_ATTR_S stManual;
	ISP_CSC_AUTO_ATTR_S	stAuto;
	ISP_CSC_STATE_S	stState;
} ISP_CSC_ATTR_S;

typedef struct xmISP_CHN_ATTR_S
{
	XM_BOOL bMode;		// 0:Sensor	1: Isp
	XM_BOOL bMirror;            /*mirror enable*/
	XM_BOOL bFlip;              /*flip   enable*/
//    XM_S32  s32SrcFrameRate;    /* source frame rate */
//    XM_S32  s32DstFrameRate;    /* dest  frame rate */   
}ISP_CHN_ATTR_S;




typedef struct xm_ISP_CHROMA_ATTR_S
{
	XM_BOOL	bEnable;
	XM_U16 	u16OfstMg;
	XM_U16	u16OfstR;
	XM_U16 	u16OfstYe;
	XM_U16	u16OfstG;
	XM_U16 	u16OfstCy;
	XM_U16	u16OfstB;

	XM_U8 u8SthMg;
	XM_U8 u8SthR;	
	XM_U8 u8SthYe;	
	XM_U8 u8SthG;	
	XM_U8 u8SthCy;	
	XM_U8 u8SthB;	
}ISP_CHROMA_ATTR_S;



typedef struct xmISP_DEFOG_MANUAL_ATTR_S
{
	XM_U8 u8Strength;
}ISP_DEFOG_MANUAL_ATTR_S;

typedef struct xmISP_DEFOG_AUTO_ATTR_S
{
    XM_U8 au8Sth[ISP_AUTO_STENGTH_NUM];           /*RW,  Range: [0, 0xFF] */      
}ISP_DEFOG_AUTO_ATTR_S;
   
typedef struct xmISP_DEFOG_ATTR_S
{
    XM_BOOL            bEnable;
    ISP_OP_TYPE_E      enOpType;
    ISP_DEFOG_MANUAL_ATTR_S stManual;
    ISP_DEFOG_AUTO_ATTR_S   stAuto;
	XM_U8 u8GlobalSth;
   	XM_BOOL bUserLutEnable;    /*RW,Range:[0,1],0:Auto Lut 1:User Lut*/
    XM_U8 au8DefogLut[65];       
}ISP_DEFOG_ATTR_S;



typedef struct xmISP_DCI_MANUAL_ATTR_S
{
	XM_U8 u8Strength;
}ISP_DCI_MANUAL_ATTR_S;

typedef struct xmISP_DCI_AUTO_ATTR_S
{
    XM_U8 au8Sth[ISP_AUTO_STENGTH_NUM];           /*RW,  Range: [0, 0xFF] */      
}ISP_DCI_AUTO_ATTR_S;
   
typedef struct xmISP_DCI_ATTR_S
{
    XM_BOOL            bEnable;
    ISP_OP_TYPE_E      enOpType;
    ISP_DCI_MANUAL_ATTR_S stManual;
    ISP_DCI_AUTO_ATTR_S   stAuto;  
}ISP_DCI_ATTR_S;


//注册场中断回调函数
typedef struct xmISP_VSYNC_CALBAK_S
{
    XM_U8            u8Mode;	// 0: 处在Vsync流程最后        1:处在Vsync流程最前
	XM_S32(*pfn_vsync_deal_0)(XM_VOID);
	XM_S32(*pfn_vsync_deal_1)(XM_VOID);
	XM_S32(*pfn_vsync_deal_2)(XM_VOID);
	XM_S32(*pfn_vsync_deal_3)(XM_VOID);
}ISP_VSYNC_CALBAK_S;

typedef struct xmISP_WB_BAYER_STATISTICS_S 
{ 
	XM_U16 u16ZoneSizeRow;
	XM_U16 u16ZoneSizeColumn;
    const XM_U16 *pau16ZoneR;  
	const XM_U16 *pau16ZoneG;  
    const XM_U16 *pau16ZoneB;  
	const XM_U32 *pau32ChosenP;	// 16*int	(水平低位为左)
	const XM_U8 *pau8SatP;
} ISP_WB_BAYER_STATISTICS_S; 




typedef struct xm_ISP_SNAP_S
{
	XM_U8 u8Start;	// 1: Start
	XM_U8 u8Mode;
	XM_U16 u16Hight;
	XM_U16 u16Width;
	XM_U16 au16StartH[3];	// L R Normal
	XM_U32 u32BufAddrY;
	XM_U32 u32BufAddrC;
	XM_U32 au32Rsv[2];
	XM_S32 (*pfn_callback)(XM_S32);
} ISP_SNAP_S;


#endif /* __XM_COMM_ISP_H__ */
