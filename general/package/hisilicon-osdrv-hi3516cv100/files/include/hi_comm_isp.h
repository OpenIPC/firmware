/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_isp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/12/20
  Description   : 
  History       :
  1.Date        : 2010/12/20
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_ISP_H__
#define __HI_COMM_ISP_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common.h"
#include "hi_isp_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
    
#define VREG_MAX_NUM        16

#define WEIGHT_ZONE_ROW     15
#define WEIGHT_ZONE_COLUMN  17
#define LUT_FACTOR          (8)
#define GAMMA_FE_LUT_SIZE   ((1<<LUT_FACTOR)+1)
#define GAMMA_NODE_NUMBER   257
#define SHADING_TABLE_NODE_NUMBER_MAX (129)
#define LIGHTSOURCE_NUM          4
#define NP_LUT_SIZE             128

#define ISP_REG_BASE        0x205A0000
#define ISP_REG_SIZE        0x7fff

#define ISP_WINDOW0_START   0x205A010C
#define ISP_WINDOW0_SIZE    0x205A0110
#define ISP_WINDOW2_START   0x205A0120
#define ISP_WINDOW2_SIZE    0x205A0124

#define ISP_BYPASS_BASE     0x205A0040

#define ISP_MAX_SNS_REGS    16
#define ISP_MAX_DEV_NUM     1

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/

typedef enum hiISP_ERR_CODE_E
{
    ERR_ISP_NOT_INIT                = 0x40,
    ERR_ISP_TM_NOT_CFG              = 0x41,
    ERR_ISP_ATTR_NOT_CFG            = 0x42,
    ERR_ISP_SNS_UNREGISTER          = 0x43,
    ERR_ISP_INVALID_ADDR            = 0x44,
    ERR_ISP_NOMEM                   = 0x45,
} ISP_ERR_CODE_E;


#define HI_ERR_ISP_NULL_PTR             HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_ISP_ILLEGAL_PARAM        HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)

#define HI_ERR_ISP_NOT_INIT             HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define HI_ERR_ISP_TM_NOT_CFG           HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_TM_NOT_CFG)
#define HI_ERR_ISP_ATTR_NOT_CFG         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define HI_ERR_ISP_SNS_UNREGISTER       HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define HI_ERR_ISP_INVALID_ADDR         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define HI_ERR_ISP_NOMEM                HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)


typedef enum hiISP_BAYER_FORMAT_E
{
    BAYER_RGGB    = 0,
    BAYER_GRBG    = 1,
    BAYER_GBRG    = 2,
    BAYER_BGGR    = 3,
    BAYER_BUTT    
} ISP_BAYER_FORMAT_E;

typedef enum hiISP_OP_TYPE_E
{
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_BUTT
} ISP_OP_TYPE_E;

#define ISP_AE_ROUTE_MAX_NODES 8
typedef struct hiISP_AE_ROUTE_NODE_S
{
    HI_U32  u32IntTime;
    HI_U32  u32SysGain;     /* shift is in the ISP_AE_ATTR_S's stSysGainRange. */
    HI_U32  u32ApePercent;  /* the percent of the iris's aperture, range is [0~100], dc-iris not support in auto mode. */
} ISP_AE_ROUTE_NODE_S;

typedef struct hiISP_AE_ROUTE_S
{
    HI_U32 u32TotalNum;    
    ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

typedef enum hiISP_AE_MODE_E
{
    AE_MODE_LOW_NOISE   = 0,
    AE_MODE_FRAME_RATE  = 1,
    AE_MODE_BUTT
} ISP_AE_MODE_E;

typedef enum hiISP_AWB_ALG_TYPE_E
{
	AWB_ALG_DEFAULT = 0,
	AWB_ALG_ADVANCE = 1,
	AWB_ALG_BUTT
    
} ISP_AWB_ALG_TYPE_E;

typedef enum hiISP_WB_MODE_E
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

typedef struct hiISP_WINDOW_S
{
    HI_U16 u16Start;
    HI_U16 u16Length;
} ISP_WINDOW_S;

typedef enum hiISP_WIND_MODE_E
{
    ISP_WIND_NONE        = 0,
    ISP_WIND_HOR        = 1,
    ISP_WIND_VER        = 2,
    ISP_WIND_ALL        = 3,
    ISP_WIND_BUTT
} ISP_WIND_MODE_E;

typedef enum hiISP_IRIS_STATUS_E
{
    ISP_IRIS_KEEP  = 0,       /* Do nothing to Iris */
    ISP_IRIS_OPEN  = 1,       /* Open Iris to the max */
    ISP_IRIS_CLOSE = 2,       /* Close Iris to the min */
    ISP_IRIS_BUTT
} ISP_IRIS_STATUS_E;

typedef enum hiISP_TRIGGER_STATUS_E
{
    ISP_TRIGGER_INIT     = 0,  /* Initial status, before trigger */
    ISP_TRIGGER_SUCCESS  = 1,  /* Trigger finished successfully */
    ISP_TRIGGER_TIMEOUT  = 2,  /* Trigger finished because of time out */
    ISP_TRIGGER_BUTT
} ISP_TRIGGER_STATUS_E;

typedef enum hiISP_WDR_MODE_E
{
    ISP_SENSOR_LINEAR_MODE = 0,
    ISP_SENSOR_WDR_MODE,

    ISP_WDR_BUTT,
} ISP_WDR_MODE_E;

typedef struct hiISP_WDR_ATTR_S
{
    ISP_WDR_MODE_E  enWdrMode;
} ISP_WDR_ATTR_S;

typedef struct hiISP_INPUT_TIMING_S
{
    ISP_WIND_MODE_E enWndMode;
    HI_U16 u16HorWndStart;    /*RW, Range: [0x0, 0x780]*/
    HI_U16 u16HorWndLength;   /*RW, Range: [0x0, 0x780]*/
    HI_U16 u16VerWndStart;    /*RW, Range: [0x0, 0x4B0]*/
    HI_U16 u16VerWndLength;   /*RW, Range: [0x0, 0x4B0]*/
} ISP_INPUT_TIMING_S;

typedef struct hiISP_IMAGE_ATTR_S
{
    HI_U16 u16Width;        /*RW, Range: [0x0, 0x780]*/
    HI_U16 u16Height;       /*RW, Range: [0x0, 0x4B0]*/
    HI_U16 u16FrameRate;    /*RW, Range: [0x0, 0xFF]*/        
    ISP_BAYER_FORMAT_E  enBayer;    
} ISP_IMAGE_ATTR_S;

typedef enum hiISP_MOD_BYPASS_E
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


typedef enum hiISP_AE_FRAME_END_UPDATE_E
{
    ISP_AE_FRAME_END_UPDATE_0  = 0x0, //isp update gain and exposure  in the  same frame
    ISP_AE_FRAME_END_UPDATE_1  = 0x1, //isp update exposure one frame before  gain
    ISP_AE_FRAME_END_UPDATE_2  = 0x2, //isp update exposure one frame before  gain, isr config
    ISP_AE_FRAME_END_BUTT,
}ISP_AE_FRAME_END_UPDATE_E;

/* 4A settings                                                              */
typedef struct hiISP_AE_ATTR_S
{
    /* base parameter */
    ISP_AE_MODE_E enAEMode;     /*AE mode(lownoise/framerate)(onvif)*/
    HI_U16 u16ExpTimeMax;       /*RW, the exposure time's max and min value .(unit :line),Range:[0, 0xFFFF], it's related to specific sensor, usually the range max value is 1125  for 1080p sensor,and 750 for 720p sensor*/
    HI_U16 u16ExpTimeMin;       /*RW, Range: [0, u16ExpTimeMax]*/
    HI_U16 u16DGainMax;         /*RW,  the Dgain's  max value, Range : [0x1, 0xFF],it's related to specific sensor*/
    HI_U16 u16DGainMin;         /*RW, Range: [0x1, u16DainMax]*/
    HI_U16 u16AGainMax;         /*RW,  the Again's  max value, Range : [0x1, 0xFF],it's related to specific sensor*/
    HI_U16 u16AGainMin;         /*RW, Range: [0x1, u16AainMax]*/
    
    HI_U8  u8ExpStep;           /*RW, AE adjust step, Range: [0x0, 0xFF]*/
    HI_S16 s16ExpTolerance;     /*RW, AE adjust tolerance, Range: [0x0, 0xFF]*/
    HI_U8  u8ExpCompensation;   /*RW, AE compensation, Range: [0x0, 0xFF]*/ 
    ISP_AE_FRAME_END_UPDATE_E  enFrameEndUpdateMode;
    HI_BOOL bByPassAE;
    /*AE weighting table*/
    HI_U8 u8Weight[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN]; /*Range :  [0, 0xF]*/
    /* ae route strategy */
    //ISP_AE_ROUTE_S stRoute;     /* */
} ISP_AE_ATTR_S;

typedef enum hiISP_AE_STRATEGY_E
{
    AE_EXP_HIGHLIGHT_PRIOR = 0,
    AE_EXP_LOWLIGHT_PRIOR  = 1,
    AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;


typedef struct hiISP_AE_ATTR_EX_S
{
    /* base parameter */
    ISP_AE_MODE_E enAEMode;		/*AE mode(lownoise/framerate)(onvif)*/
    HI_U32 u32ExpTimeMax;       /*RW, the exposure time's max and min value .(unit :line),Range:[0, 0xFFFF], it's related to specific sensor, usually the range max value is 1125  for 1080p sensor,and 750 for 720p sensor*/
    HI_U32 u32ExpTimeMin;       /*RW, Range: [0, u16ExpTimeMax]*/
    HI_U32 u32DGainMax;         /*RW,  the Dgain's  max value, Range : [0x400, 0xFFFFFFFF],it's related to specific sensor*/
    HI_U32 u32DGainMin;         /*RW, Range: [0x400, u16DainMax]*/
    HI_U32 u32AGainMax;			/*RW,  the Again's  max value, Range : [0x400, 0xFFFFFFFF],it's related to specific sensor*/
    HI_U32 u32AGainMin;         /*RW, Range: [0x400, u16AainMax]*/
    HI_U32 u32ISPDGainMax;      /*RW,  the ISPDgain's  max value, Range : [0x400, 0xFFFFFFFF]*/
    HI_U32 u32SystemGainMax;    /*RW, the maximum gain of the system, Range: [0x400, 0xFFFFFFFF],it's related to specific sensor*/
	HI_U32 u32SystemGainMin;    /*RW, the minmum gain of the system, Range: [0x400, u32SystemGainMax],it's related to specific sensor*/
    HI_U32 u32GainThreshold;    /*RW, the setting of the system gain threshold  for low nosie mode */

    HI_U8  u8ExpStep;			/*RW, AE adjust step, Range: [0x0, 0xFF]*/
    HI_S16 s16ExpTolerance;		/*RW, AE adjust tolerance, Range: [0x0, 0xFF]*/
    HI_U8  u8ExpCompensation;	/*RW, AE compensation, Range: [0x0, 0xFF]*/ 
	HI_U16  u16EVBias;          /*RW, AE EV bias, Range: [0x100, 0x1000]*/
    ISP_AE_STRATEGY_E enAEStrategyMode;  /*RW, Support Highlight prior or Lowlight prior*/
    HI_U16  u16HistRatioSlope;       /*RW, Range: [0x0, 0xFFFF], AE hist ratio slope*/
    HI_U8   u8MaxHistOffset;         /*RW, Range: [0x0, 0xFF], Max hist offset*/
    ISP_AE_FRAME_END_UPDATE_E  enFrameEndUpdateMode;
    HI_BOOL bByPassAE;
    /*AE weighting table*/
    HI_U8 u8Weight[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN]; /*Range :  [0, 0xF]*/
    /* ae route strategy */
    //ISP_AE_ROUTE_S stRoute;     /* */
} ISP_AE_ATTR_EX_S;


typedef struct hiISP_AE_DELAY_S
{
    HI_U16 u16BlackDelayFrame;    /*RW, AE black delay frame count, Range: [0x0, 0xFFFF]*/
    HI_U16 u16WhiteDelayFrame;    /*RW, AE white delay frame count, Range: [0x0, 0xFFFF]*/
} ISP_AE_DELAY_S;


typedef struct hiISP_EXP_STA_INFO_S
{
    HI_U8  u8ExpHistThresh[4];              /*RW, Histogram threshold for bin 0/1 1/2 2/3 3/4 boundary, Range: [0x0, 0xFF]*/
    HI_U16 u16ExpStatistic[WEIGHT_ZONE_ROW ][WEIGHT_ZONE_COLUMN][5];    /*RO, zone exposure statistics,Range: [0x0, 0xFFFF]*/
    HI_U16 u16Exp_Hist256Value[256];        /*RO, 256 bins histogram,Range: [0x0, 0xFFFF]*/
    HI_U16 u16Exp_Hist5Value[5];            /*RO, 5 bins histogram, Range: [0x0, 0xFFFF]*/
    HI_U8  u8AveLum;                        /*RO, average lum,Range: [0x0, 0xFF]*/
    HI_U8  u8ExpHistTarget[5];              /*RW, Histogram target for bin 0/1/2/3/4 */
    HI_S16 s16HistError;   
}ISP_EXP_STA_INFO_S;

typedef struct hiISP_ME_ATTR_S
{
    HI_S32 s32AGain;            /*RW,  sensor analog gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    HI_S32 s32DGain;            /*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    HI_U32 u32ExpLine;          /*RW,  sensor exposure time (unit: line ), Range: [0x0, 0xFFFF],it's related to the specific sensor */

    HI_BOOL bManualExpLineEnable;
    HI_BOOL bManualAGainEnable;
    HI_BOOL bManualDGainEnable;
} ISP_ME_ATTR_S;

typedef struct hiISP_ME_ATTR_EX_S
{
    HI_S32 s32AGain;            /*RW,  sensor analog gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    HI_S32 s32DGain;            /*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
    HI_U32 u32ISPDGain;         /*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific isp   */
    HI_U32 u32ExpLine;          /*RW,  sensor exposure time (unit: line ), Range: [0x0, 0xFFFF],it's related to the specific sensor */

    HI_BOOL bManualExpLineEnable;
    HI_BOOL bManualAGainEnable;
    HI_BOOL bManualDGainEnable;
    HI_BOOL bManualISPGainEnable;
} ISP_ME_ATTR_EX_S;

typedef struct hiISP_AF_ATTR_S
{
    HI_S32 s32FocusDistanceMax; /* the focuse range */
    HI_S32 s32FocusDistanceMin;

    /* weighting table */
    HI_U8 u8Weight[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN];    
} ISP_AF_ATTR_S;

typedef struct hiISP_FOUCS_STA_INFO_S
{
    HI_U16 u16FocusMetrics;     /*RO, The integrated and normalized measure of contrast*/
    HI_U16 u16ThresholdRead;    /*RO, The ISP recommend value of AF threshold*/
    HI_U16 u16ThresholdWrite;   /*RW, The user defined value of AF threshold (or 0 to use threshold from previous frame)*/
    HI_U16 u16FocusIntensity;   /*RO, The average brightness*/
    HI_U8  u8MetricsShift;      /*RW, Metrics scaling factor:the bigger value for this register means all zones metrics go higher,0x03 is the default, Range: [0x0, 0xF]*/
    HI_U8  u8NpOffset;          /*RW, The AF noise profile offset, Range: [0x0, 0xFF]*/
	HI_U16 u16ZoneMetrics[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN]; /*RO, The zoned measure of contrast*/    
} ISP_FOCUS_STA_INFO_S;

typedef struct hiISP_MF_ATTR_S
{
    HI_S32 s32DefaultSpeed;     /* 1,default speed(unit:m/s).(onvif)*/    
} ISP_MF_ATTR_S;

typedef struct hiISP_AWB_CALIBRATION_S 
{ 
    HI_S32 as32CurvePara[6];  /*RW, Range: [0x0, 0xFFFF], AWB curve parameter*/ 
    HI_U16 au16StaticWB[4];  /*RW, Range: [0x0, 0xFFFF], Static white balance parameter*/
    HI_U16 u16RefTemp;   /*RW, Range: [0x0, 0xFFFF], the reference color temperature of calibration*/ 

} ISP_AWB_CALIBRATION_S; 

typedef struct hiISP_AWB_ATTR_S
{
    ISP_AWB_CALIBRATION_S stAWBCalibration; /*RW, AWB Calibration parameters*/
    HI_U16 u16Speed;             /*RW, Range: [0x0, 0xFFF], AWB converging speed*/ 
    HI_U8 u8RGStrength;        /*RW, Range: [0x0, 0xFF]*/
    HI_U8 u8BGStrength;        /*RW, Range: [0x0, 0xFF]*/
    HI_U8 u8ZoneSel;           /*RW,  A value of 0 or 0x3F means global AWB, A value between 0 and 0x3F means zoned AWB */
    HI_U8 u8HighColorTemp;     /*RW, AWB max temperature in K/100, Range: [0x0, 0xFF], Recommended: [85, 100] */
    HI_U8 u8LowColorTemp;      /*RW, AWB min temperature in K/100, Range: [0x0, u8HighColorTemp), Recommended: [20, 25] */
    /* weighting table */
    HI_U8 u8Weight[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN];  /*RW, Range :  [0, 0xF]*/
    
} ISP_AWB_ATTR_S;

typedef struct hiISP_AWB_IN_OUT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;
    HI_BOOL bOutdoorStatus;                /*in Auto mode, this is RO, in Manual mode, this is WO*/
    HI_U32 u32OutThresh;                   /*shutter time(in us) to judge indoor or outdoor */
    HI_U16 u16LowStart;                    /*5000K is recommend. [u8LowStart, u8HighStart] is the color temperature range of Day Light*/
    HI_U16 u16LowStop;                     /*4500K is recommend, u16LowStop < u8LowStart*/
    HI_U16 u16HighStart;                   /*6500K is recommend, u16HighStart > u8LowStart*/
    HI_U16 u16HighStop;                    /*8000K is recommend, u16HighStop > u8HighStart*/
    HI_BOOL bGreenEnhanceEn;               /*If this is enabled, Green channel will be enhanced based on the area of green plant, only take effect outdoor*/
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct hiISP_AWB_CT_LIMIT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;

    HI_U16 u16HighRgLimit;     /*RO, in Manual Mode, user should define the Max Rgain of High Color Temperature, u16HighRgLimit > u16LowRgLimit*/
    HI_U16 u16HighBgLimit;     /*RO, in Manual Mode, user should define the Min Bgain of High Color Temperature, u16HighBgLimit < u16LowBgLimit*/
    HI_U16 u16LowRgLimit;      /*RO, in Manual Mode, user should define the Min Rgain of Low Color Temperature*/
    HI_U16 u16LowBgLimit;      /*RO, in Manual Mode, user should define the Max Bgain of Low Color Temperature*/
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct hiISP_ADV_AWB_ATTR_S
{
    HI_BOOL bAccuPrior;         /*RW, recommend 0 for outdoor, 1 for indoor*/    
    HI_U8  u8Tolerance;         /*RW, Range:[0x0, 0xFF], AWB adjust tolerance,for outdoor, this value should be small,recomend 4*/   
    HI_U16 u16CurveLLimit;      /*RW, Range:[0x0, 0x100],   Left limit of AWB Curve, recomend for indoor 0xE0, outdoor 0xE0*/ 
    HI_U16 u16CurveRLimit;      /*RW, Range:[0x100, 0xFFF], Right Limit of AWB Curve,recomend for indoor 0x130, outdoor 0x120*/ 

    HI_BOOL bGainNormEn;     
    ISP_AWB_IN_OUT_ATTR_S       stInOrOut;    
    ISP_AWB_CT_LIMIT_ATTR_S     stCTLimit;
} ISP_ADV_AWB_ATTR_S;

typedef struct hiISP_AWB_LIGHTSOURCE_INFO_S
{
    HI_U16 u16WhiteRgain;        /*G/R of White points at this light source*/
    HI_U16 u16WhiteBgain;        /*G/B of White points at this light source*/
    HI_U16 u16ExpQuant;          /*shtter time * again * dgain >> 4, Not support Now*/
    HI_U8   u8LightStatus;       /*RW, 0: idle  1:add  2:delete */
    HI_U8   u8Radius;            /*Radius of light source*/           
} ISP_AWB_LIGHTSOURCE_INFO_S;

typedef struct hiISP_AWB_ADD_LIGHTSOURCE_S
{
    HI_BOOL  bLightEnable;                     /*Enable special light source function*/
    ISP_AWB_LIGHTSOURCE_INFO_S	stLightInfo[LIGHTSOURCE_NUM]; 
} ISP_AWB_ADD_LIGHTSOURCE_S;

typedef struct hiISP_WB_ZONE_STA_INFO_S
{
    HI_U16 u16Rg;               /*RO, Zoned WB output G/R, Range : [0x0, 0xFFF]*/
    HI_U16 u16Bg;               /*RO, Zoned WB output G/B, Range : [0x0, 0xFFF]*/
    HI_U32 u32Sum;              /*RO, Zoned WB output population,Range: [0x0, 0xFFFFFFFF]*/  
} ISP_WB_ZONE_STA_INFO_S;

typedef struct hiISP_WB_STA_INFO_S
{
    HI_U16 u16WhiteLevel;       /*RW, Upper limit of valid data for white region, Range: [0x0, 0xFFFF]*/
    HI_U16 u16BlackLevel;       /*RW, Lower limit of valid data for white region, Range: [0x0, u16WhiteLevel)*/
    HI_U16 u16CbMax;            /*RW, Maximum value of B/G for white region, Range: [0x0,0xFFF]*/
    HI_U16 u16CbMin;            /*RW, Minimum value of B/G for white region, Range: [0x0, u16CbMax)*/
    HI_U16 u16CrMax;            /*RW, Maximum value of R/G for white region, Range: [0x0, 0xFFF]*/
    HI_U16 u16CrMin;            /*RW, Minimum value of R/G for white region, Range: [0x0, u16CrMax)*/
    
    HI_U16 u16GRgain;           /*RO, Global WB output G/R, Range: [0x0, 0xFFFF]*/
    HI_U16 u16GBgain;           /*RO, Global WB output G/B, Range: [0x0, 0xFFFF]*/
    HI_U32 u32GSum;             /*RO, Global WB output population, Range: [0x0, 0xFFFF]*/

    HI_U32 u32Rgain;            /*RO, gain value of R channel for AWB, Range: [0x0, 0xFFF]*/
    HI_U32 u32Ggain;            /*RO, gain value of G channel for AWB, Range: [0x0, 0xFFF]*/
    HI_U32 u32Bgain;            /*RO, gain value of B channel for AWB, Range: [0x0, 0xFFF]*/
    
    ISP_WB_ZONE_STA_INFO_S stZoneSta[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN];  /*RO, Zoned WB statistics*/
} ISP_WB_STA_INFO_S;

typedef struct hiISP_MWB_ATTR_S        
{
    HI_U16 u16Rgain;            /*RW, Multiplier for R color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Ggain;            /*RW, Multiplier for G color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Bgain;            /*RW, Multiplier for B color channel, Range: [0x0, 0xFFF]*/
    
} ISP_MWB_ATTR_S;

typedef struct hiISP_COLORMATRIX_S
{   HI_U16 u16HighColorTemp;    /*RW,  Range: [u16MidColorTemp + 400,  10000]*/
    HI_U16 au16HighCCM[9];      /*RW,  Range: [0x0,  0xFFFF]*/
    HI_U16 u16MidColorTemp;     /*RW,  the MidColorTemp should be at least 400 smaller than HighColorTemp, Range: [u16LowColorTemp + 400,  u16HighColorTemp-400]*/
    HI_U16 au16MidCCM[9];       /*RW,  Range: [0x0,  0xFFFF]*/
    HI_U16 u16LowColorTemp;     /*RW,  the LowColorTemp should be at least 400 smaller than HighColorTemp, Range: [0,  u16MidColorTemp-400]*/
    HI_U16 au16LowCCM[9];       /*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_S;

typedef struct hiISP_COLORTONE_S
{
  HI_U16 u16RedCastGain;        /*RW,  Range: [0x100, 0xFFFF], adjust the final red channel  tone of the picture */
  HI_U16 u16GreenCastGain;      /*RW,  Range: [0x100, 0xFFFF], adjust the final green channel  tone of the picture*/
  HI_U16 u16BlueCastGain;       /*RW,  Range: [0x100, 0xFFFF], adjust the final blue channel  tone of the picture*/ 
  
}ISP_COLORTONE_S;

typedef struct hiISP_SATURATION_ATTR_S
{
    HI_BOOL bSatManualEnable;
    HI_U8   u8SatTarget;        /*RW,  Range: [0, 0xFF] */
    HI_U8   au8Sat[8];           /*RW,  Range: [0, 0xFF] */ 
}ISP_SATURATION_ATTR_S;

typedef enum hiISP_IRIS_TYPE_E
{
    ISP_IRIS_DC_TYPE = 0,
    ISP_IRIS_P_TYPE,
    
    ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;

typedef struct hiISP_AI_ATTR_S
{
    HI_BOOL bIrisEnable;        /*Auto iris  on/off*/
    HI_BOOL bIrisCalEnable;     /*iris calibration on/off*/
	HI_BOOL bIrisCalExit;       /*iris calibration exit, exit the calibration*/
    HI_U32  u32IrisHoldValue;   /*RW, iris hold value, Range: [0x0, 0x3E8]*/

    ISP_IRIS_STATUS_E enIrisStatus;         /*RW, status of Iris*/
    ISP_TRIGGER_STATUS_E enTriggerStatus;   /*RW, status of trigger*/
    HI_U16 u16IrisStopValue;    /*RW, the initial stop value for AI trigger, Range: [0x0,0x3E8]*/
    HI_U16 u16IrisCloseDrive;   /*RW, the drive value to close Iris, Range: [0x0,0x3E8], Recommended value: [700, 900]. A larger value means faster.*/
    HI_U16 u16IrisTriggerTime;  /*RW, frame numbers of AI trigger lasting time. > 600, [0x0, 0xFFF]*/
    HI_U8  u8IrisInertiaValue;  /*RW, frame numbers of  AI moment of inertia, Range: [0x0, 0xFF],the recommended value is between[0x3, 0xa]*/
} ISP_AI_ATTR_S;

typedef struct hiISP_MI_ATTR_S
{
    HI_BOOL bEnable;            /* manual iris  on/off*/
    HI_U32  u32IrisHoldValue;   /*RW, iris hold value, Range: [0x0, 0x3E8]*/
    HI_U16  u16ApePercent;      /* the percent of the iris's aperture, range is [0~100]. */
} ISP_MI_ATTR_S;

typedef struct hiISP_DRC_ATTR_S
{
    HI_BOOL bDRCEnable;        
    HI_BOOL bDRCManualEnable;        
    HI_U32  u32StrengthTarget;  /*RW,  Range: [0, 0xFF]. It is not the final strength used by ISP. 
                                     * It will be clipped to reasonable value when image is noisy. */
    HI_U32  u32SlopeMax;        /*RW,  Range: [0, 0xFF]. Not recommended to change. */
    HI_U32  u32SlopeMin;        /*RW,  Range: [0, 0xFF]. Not recommended to change. */
    HI_U32  u32WhiteLevel;      /*RW,  Range: [0, 0xFFF]. Not recommended to change. */
    HI_U32  u32BlackLevel;      /*RW,  Range: [0, 0xFFF]. Not recommended to change. */
    HI_U32  u32VarianceSpace;     /*RW,  Range: [0, 0xF]. Not recommended to change*/
    HI_U32  u32VarianceIntensity; /*RW,  Range: [0, 0xF].Not recommended to change*/
} ISP_DRC_ATTR_S;

typedef enum hiISP_ANTIFLICKER_MODE_E
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

typedef struct hiISP_ANTIFLICKER_S
{
    HI_BOOL bEnable;
    HI_U8   u8Frequency;          /*RW, Range: usually this value is 50 or 60  which is the frequency of the AC power supply*/

    ISP_ANTIFLICKER_MODE_E  enMode;
} ISP_ANTIFLICKER_S;

typedef struct hiISP_SUBFLICKER_S
{
    HI_BOOL bEnable;

    /* RW, Range: [0x0, 0x64], if subflicker mode enable, current luma is less than AE compensation plus LumaDiff, 
      AE will keep min antiflicker shutter time(for example: 1/100s or 1/120s) to avoid flicker. while current luma is 
      larger than AE compensation plus the LumaDiff, AE will reduce shutter time to avoid over-exposure and introduce 
      flicker in the pircture */
    HI_U8   u8LumaDiff;          
} ISP_SUBFLICKER_S;

typedef struct hiISP_DCIRIS_ATTR_S
{
    HI_S32 s32Para1;                 /*RW, Range: usually this value is larger than 1000 */
    HI_S32 s32Para2;                 /*RW, Range: usually this value is 1 */
    HI_S32 s32Para3;                 /*RW, Range: usually this value is larger than 1000 */
    HI_U32 u32MinPwmDuty;            /*RW, Range:[0, 1000], which is the min pwm duty of the dciris control algorithm */
    HI_U32 u32MaxPwmDuty;            /*RW, Range:[0, 1000], which is the max pwm duty of the dciris control algorithm */
    HI_U32 u32OpenPwmDuty;           /*RW, Range:[0, 1000], which is the open pwm duty of the dciris control algorithm */
} ISP_DCIRIS_ATTR_S;


typedef struct hiISP_DP_ATTR_S
{
    HI_BOOL bEnableDynamic;
    HI_U16  u16DynamicBadPixelSlope;       /*RW, Range: [0x0, 0xFFF], blending degree of bad pixel*/ 
    HI_U16  u16DynamicBadPixelThresh;         /*RW, Range: [0x0, 0xFFF], thresh hold in bad pixel detection*/ 
    HI_BOOL bEnableStatic;
    HI_BOOL bEnableDetect;
    ISP_TRIGGER_STATUS_E enTriggerStatus;   /*status of bad pixel trigger*/
    HI_U8   u8BadPixelStartThresh;          /*RW,  Range: [0, 0xFF] */
    HI_U8   u8BadPixelFinishThresh;         /*RW,  Range: [0, 0xFF] */
    HI_U16  u16BadPixelCountMax;            /*RW, limit of max number of bad pixel,  Range: [0, 0x3FF] */
    HI_U16  u16BadPixelCountMin;            /*RW, limit of min number of bad pixel, Range: [0, 0x3FF]*/
    HI_U16  u16BadPixelCount;               /*RW, limit of min number of bad pixel, Range: [0, 0x3FF]*/
    HI_U16  u16BadPixelTriggerTime;         /*RW, time limit for bad pixel trigger, in frame number ,Range: [0x0, 0x640]*/
    HI_U32  u32BadPixelTable[1024];         /*RW, Range: [0x0, 0x3FFFFF],the first 11 bits represents the X coordinate of the defect pixel, the second 11 bits represent the Y coordinate of the defect pixel*/
} ISP_DP_ATTR_S;

typedef struct hiISP_DIS_ATTR_S
{
    HI_BOOL bEnable;
} ISP_DIS_ATTR_S;

typedef struct hiISP_DIS_INFO_S
{
    HI_S8 s8Xoffset;         /*RW, Range: [0x00, 0x80]*/
    HI_S8 s8Yoffset;         /*RW, Range: [0x80, 0xFF]*/   
} ISP_DIS_INFO_S;


typedef struct hiISP_SHADING_ATTR_S
{
    HI_BOOL Enable;
} ISP_SHADING_ATTR_S;

typedef struct hiISP_SHADINGTAB_S
{
    HI_U16 u16ShadingCenterR_X;  /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingCenterR_Y;  /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingCenterG_X;  /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingCenterG_Y;  /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingCenterB_X;  /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingCenterB_Y;  /*RW, Range: [0x0, 0xFFFF]*/

    HI_U16 u16ShadingTable_R[SHADING_TABLE_NODE_NUMBER_MAX]; /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingTable_G[SHADING_TABLE_NODE_NUMBER_MAX]; /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingTable_B[SHADING_TABLE_NODE_NUMBER_MAX]; /*RW, Range: [0x0, 0xFFFF]*/

    HI_U16 u16ShadingOffCenter_R;  /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingOffCenter_G;  /*RW, Range: [0x0, 0xFFFF]*/
    HI_U16 u16ShadingOffCenter_B;  /*RW, Range: [0x0, 0xFFFF]*/

    HI_U16 u16ShadingTableNodeNumber;  /*RW, Range: [0x0, SHADING_TABLE_NODE_NUMBER_MAX]*/
} ISP_SHADINGTAB_S;

typedef struct hiISP_DENOISE_ATTR_S
{
    HI_BOOL bEnable;
    HI_BOOL bManualEnable;
    HI_U8 u8ThreshTarget;       /*RW,  Noise reduction effect for high spacial frequencies Range: [0x0, u8ThreshTarget]*/
    HI_U8 u8ThreshMax;          /*RW,  Noise reduction effect for high spacial frequencies, Range: [0x0, 0xFF] */
    HI_U8 u8SnrThresh[8];       /*RW,  Noise reduction target value array for  different iso, Range: [0x0, 0xFF],*/
} ISP_DENOISE_ATTR_S;
typedef struct hiISP_NOISEPROFILE_TABLE_S
{

   HI_U8 au8NoiseProfile[NP_LUT_SIZE];

}ISP_NOISEPROFILE_TABLE_S;

typedef struct hiISP_GAMMA_ATTR_S
{
    HI_BOOL bEnable;
} ISP_GAMMA_ATTR_S;

typedef enum hiISP_GAMMA_CURVE_E
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

typedef struct hiISP_GAMMA_TABLE_S
{
    ISP_GAMMA_CURVE_E enGammaCurve;
    HI_U16 u16Gamma[GAMMA_NODE_NUMBER];
    HI_U16 u16Gamma_FE[GAMMA_FE_LUT_SIZE];   /* only for WDR sensor mode */
} ISP_GAMMA_TABLE_S;

typedef struct hiISP_SHARPEN_ATTR_S
{
    HI_BOOL bEnable;
    HI_BOOL bManualEnable;
    HI_U8 u8StrengthTarget;     /*RW,  Range:[0, 0xFF]. */
    HI_U8 u8StrengthUdTarget;     /*RW,  Range:[0, 0xFF]. */
    HI_U8 u8StrengthMin;        /*RW,  Range:[0, u32StrengthTarget]. */
    HI_U8 u8SharpenAltD[8];     /*RW,  Range: [0, 0xFF].  */
    HI_U8 u8SharpenAltUd[8];    /*RW, Range: [0, 0xFF]. */
} ISP_SHARPEN_ATTR_S;

typedef struct hiISP_PARA_REC_S
{
    HI_BOOL bInit;
    HI_BOOL bTmCfg;
    HI_BOOL bAttrCfg;
    
    ISP_INPUT_TIMING_S stInputTiming;
    ISP_IMAGE_ATTR_S stImageAttr;

    HI_U32 u32ModFlag;

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
typedef struct hiISP_CR_ATTR_S
{
    HI_BOOL  bEnable;
    HI_U8    u8Strength[8];  /*Range: [0x0, 0xFF] */
    HI_U8    u8Sensitivity; /*Range: [0x0, 0xFF],this register is  not recommended  to change */
    HI_U16   u16Threshold;  /*Range: [0x0, 0xFFFF],this register is  not recommended  to change */
    HI_U16   u16Slope;      /*Range: [0x0, 0xFFFF],this register is  not recommended  to change */
}ISP_CR_ATTR_S;

typedef struct hiISP_ANTIFOG_S
{
    HI_BOOL bEnable;
    HI_U8   u8Strength;     /*Range: [0x0, 0xFF]*/
} ISP_ANTIFOG_S;

typedef struct hiISP_ANTI_FALSECOLOR_S
{
    HI_U8  u8Strength;      /* Range: [0x0, 0xFF], the recommended range is [0x0, 0x95], the normal 
                                * color will gradually be eroded when this register is larger than 0x95. */
} ISP_ANTI_FALSECOLOR_S;

/*users query ISP state information*/
typedef struct hiISP_INNER_STATE_INFO_S
{
    HI_U32 u32ExposureTime;         /* RO,  Range: [0x0, 0xFFFF] */                
    HI_U32 u32AnalogGain;           /* RO,Range: [0x0, 0xFFFF] */                
    HI_U32 u32DigitalGain;          /* RO,Range: [0x0, 0xFFFF] */            
    HI_U32 u32Exposure;             /* RO,Range: [0x0, 0xFFFFFFFF] */        
    HI_U16 u16AE_Hist256Value[256]; /* RO, 256 bins histogram */
    HI_U16 u16AE_Hist5Value[5];     /* RO, 5 bins histogram */
    HI_U8  u8AveLum;                /* RO, Range: [0x0, 0xFF] */    
    HI_BOOL bExposureIsMAX;
}ISP_INNER_STATE_INFO_S;

typedef struct hiISP_INNER_STATE_INFO_EX_S
{
    HI_U32 u32ExposureTime;         /*RO,  Range: [0x0, 0xFFFF] */
    HI_U32 u32AnalogGain;	        /*RO,Range: [0x0, 0xFFFFFFFF] */
    HI_U32 u32DigitalGain;	        /*RO,Range: [0x0, 0xFFFFFFFF] */
    HI_U32 u32ISPDigitalGain;	    /*RO,Range: [0x0, 0xFFFFFFFF] */
    HI_U32 u32Exposure;			    /*RO,Range: [0x0, 0xFFFFFFFF] */
    HI_U16 u16AE_Hist256Value[256];	/*RO, 256 bins histogram */
    HI_U16 u16AE_Hist5Value[5];		/*RO, 5 bins histogram */
    HI_U8  u8AveLum;			    /*RO, Range: [0x0, 0xFF]*/
    HI_BOOL bExposureIsMAX;
}ISP_INNER_STATE_INFO_EX_S;

/*Demosaic Attr*/
typedef struct hiISP_DEMOSAIC_ATTR_S
{
    HI_U8   u8VhSlope;              /*RW,Range: [0x0, 0xFF] */
    HI_U8   u8AaSlope;              /*RW,Range: [0x0, 0xFF] */
    HI_U8   u8VaSlope;              /*RW,Range: [0x0, 0xFF] */
    HI_U8   u8UuSlope;              /*RW,Range: [0x0, 0xFF] */
    HI_U16  u16VhThresh;            /*RW,Range: [0x0, 0xFFFF] */   
    HI_U16  u16AaThresh;            /*RW,Range: [0x0, 0xFFFF] */
    HI_U16  u16VaThresh;            /*RW,Range: [0x0, 0xFFFF] */
    HI_U16  u16UuThresh;            /*RW,Range: [0x0, 0xFFFF] */
    HI_U8   u8DemosaicConfig;       /*RW,Range: [0x0, 0xFF] */
    HI_U8   u8LumThresh[8];         /*RW, Range:[0x0, 0xFF] */
    HI_U8   u8NpOffset[8];          /*RW, Range:[0x0, 0xFF] */
}ISP_DEMOSAIC_ATTR_S;

typedef struct hiISP_BLACK_LEVEL_S 
{ 
    HI_U16 au16BlackLevel[4]; /*RW, Range: [0x0, 0xFFF]*/ 
} ISP_BLACK_LEVEL_S; 

/*ISP debug information*/
typedef struct hiISP_DEBUG_INFO_S
{
    HI_BOOL bDebugEn;       /*RW, 1:enable debug, 0:disable debug*/
    HI_U32  u32PhyAddr;     /*RW, phy address of debug info */
    HI_U32  u32Depth;       /*RW, depth of debug info */
} ISP_DEBUG_INFO_S;

typedef struct hiISP_DBG_ATTR_S
{
    HI_U32  u32Rsv;         /* need to add member */
} ISP_DBG_ATTR_S;

typedef struct hiISP_DBG_STATUS_S
{
    HI_U32  u32FrmNumBgn;
    HI_U32  u32Rsv;         /* need to add member */
    HI_U32  u32FrmNumEnd;
} ISP_DBG_STATUS_S;

typedef enum hiISP_SNS_TYPE_E
{
    ISP_SNS_I2C_TYPE = 0,
    ISP_SNS_SSP_TYPE,
    
    ISP_SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

typedef struct hiISP_I2C_DATA_S
{
    HI_BOOL bDelayCfg;
    
    HI_U8   u8DevAddr;
    HI_U32  u32RegAddr;
    HI_U32  u32AddrByteNum;
    HI_U32  u32Data;
    HI_U32  u32DataByteNum;
} ISP_I2C_DATA_S;

typedef struct hiISP_SSP_DATA_S
{
    HI_BOOL bDelayCfg;
    
    HI_U32  u32DevAddr;
    HI_U32  u32DevAddrByteNum;
    HI_U32  u32RegAddr;
    HI_U32  u32RegAddrByteNum;
    HI_U32  u32Data;
    HI_U32  u32DataByteNum;
} ISP_SSP_DATA_S;

typedef struct hiISP_SNS_REGS_INFO_S
{
    ISP_SNS_TYPE_E enSnsType;
    HI_U32  u32RegNum;
    HI_BOOL bDelayCfgIspDgain;

    union
    {
        ISP_I2C_DATA_S astI2cData[ISP_MAX_SNS_REGS];
        ISP_SSP_DATA_S astSspData[ISP_MAX_SNS_REGS];
    };
} ISP_SNS_REGS_INFO_S;

typedef struct hiISP_VD_INFO_S
{
  HI_U32  u32Reserved;          /*RO, Range: [0x0, 0xFFFFFFFF] */
}ISP_VD_INFO_S;


typedef struct hiISP_REG_ATTR_S
{
    HI_U32 u32IspRegAddr;
    HI_U32 u32IspRegSize;
    HI_U32 u32IspExtRegAddr;
    HI_U32 u32IspExtRegSize;
    HI_U32 u32AeExtRegAddr;
    HI_U32 u32AeExtRegSize;
    HI_U32 u32AwbExtRegAddr;
    HI_U32 u32AwbExtRegSize;
} ISP_REG_ATTR_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_ISP_H__ */

