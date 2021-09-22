/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef _MI_ISP_3A_DATATYPE_H_
#define _MI_ISP_3A_DATATYPE_H_

typedef enum __attribute__ ((aligned (4)))
{
    SS_FALSE = 0,
    SS_TRUE = !SS_FALSE,
    SS_BOOL_MAX
} MI_ISP_BOOL_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_OP_TYP_AUTO = 0,
    SS_OP_TYP_MANUAL = !SS_OP_TYP_AUTO,
    SS_OP_TYP_MODE_MAX
} MI_ISP_OP_TYPE_e;

/************************************* AE  - Define the structure declaration - START *************************************/

typedef enum __attribute__ ((aligned (4)))
{
    SS_ISP_STATE_NORMAL = 0,
    SS_ISP_STATE_PAUSE = 1,
    SS_ISP_STATE_MAX
} MI_ISP_SM_STATE_TYPE_e;


typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_MODE_A,   // auto
    SS_AE_MODE_AV,  // aperture priority
    SS_AE_MODE_SV,
    SS_AE_MODE_TV,  // shutter priority
    SS_AE_MODE_M,   // manual mode
    SS_AE_MODE_MAX
} MI_ISP_AE_MODE_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_WEIGHT_AVERAGE = 0,
    SS_AE_WEIGHT_CENTER,
    SS_AE_WEIGHT_SPOT,
    SS_AE_WEIGHT_MAX
} MI_ISP_AE_WIN_WEIGHT_MODE_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_FLICKER_TYPE_DISABLE = 0,
    SS_AE_FLICKER_TYPE_60HZ = 1,
    SS_AE_FLICKER_TYPE_50HZ = 2,
    SS_AE_FLICKER_TYPE_AUTO = 3,
    //SS_AE_FLICKER_TYPE_DETECT_60HZ = 4,
    //SS_AE_FLICKER_TYPE_DETECT_50HZ = 5,
    SS_AE_FLICKER_TYPE_MAX
} MI_ISP_AE_FLICKER_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_FLICKER_TYPE_DETECT_60HZ = 0,
    SS_AE_FLICKER_TYPE_DETECT_50HZ = 1,
    SS_AE_FLICKER_TYPE_DETECT_MAX
} MI_ISP_AE_FLICKER_DETECT_TYPE_e;

//typedef struct MI_ISP_AE_FLICKER_INFO_TYPE_s
//{
//    MI_ISP_BOOL_e bIsEffective;
//    MI_ISP_AE_FLICKER_TYPE_e eFlickerType;
//    MI_U16 u16Score;
//} MI_ISP_AE_FLICKER_INFO_TYPE_t;

typedef struct MI_ISP_AE_FLICKER_EX_TYPE_s
{
    MI_ISP_BOOL_e                   bEnable;            // 0 ~ 1
    MI_ISP_OP_TYPE_e                enOpType;           // M_AUTO ~ (M_MODMAX-1)
    MI_U8                           u8AmpSensitivity;   // 1 ~ 100
    MI_U8                           u8ScoreThd;         // 1 ~ 100
    MI_U8                           uRefreshCycles;     // 1 ~ 10
    MI_U8                           u8ValidTimesThd;    // 1 ~ 10
    MI_ISP_AE_FLICKER_DETECT_TYPE_e eFlickerType;       // 60Hz = 0, 50Hz = 1
} MI_ISP_AE_FLICKER_EX_TYPE_t;

typedef struct MI_ISP_AE_FLICKER_EX_INFO_TYPE_s
{
    MI_ISP_BOOL_e                   bIsEffective;
    MI_ISP_AE_FLICKER_DETECT_TYPE_e eFlickerType;
    MI_U16                          u16Score;
} MI_ISP_AE_FLICKER_EX_INFO_TYPE_t;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_STRATEGY_BRIGHTTONE,
    SS_AE_STRATEGY_DARKTONE,
    SS_AE_STRATEGY_AUTO,
    SS_AE_STRATEGY_MAX
} MI_ISP_AE_STRATEGY_TYPE_e;

typedef struct MI_ISP_AE_HIST_WEIGHT_Y_TYPE_s
{
    MI_U32 u32LumY;
    MI_U32 u32AvgY;
    MI_U32 u32Hits[128];
} MI_ISP_AE_HIST_WEIGHT_Y_TYPE_t;

typedef struct MI_ISP_AE_EXPO_VALUE_TYPE_s
{
    MI_U32 u32FNx10;
    MI_U32 u32SensorGain;
    MI_U32 u32ISPGain;
    MI_U32 u32US;
} MI_ISP_AE_EXPO_VALUE_TYPE_t;

typedef struct MI_ISP_AE_EXPO_INFO_TYPE_s
{
    MI_ISP_BOOL_e bIsStable;
    MI_ISP_BOOL_e bIsReachBoundary;
    MI_ISP_AE_EXPO_VALUE_TYPE_t stExpoValueLong;
    MI_ISP_AE_EXPO_VALUE_TYPE_t stExpoValueShort;
    MI_ISP_AE_HIST_WEIGHT_Y_TYPE_t stHistWeightY;
    MI_U32 u32LVx10;
    MI_S32 s32BV;
    MI_U32 u32SceneTarget;
} MI_ISP_AE_EXPO_INFO_TYPE_t;

typedef struct MI_ISP_AE_EV_COMP_TYPE_s
{
    MI_S32 s32EV;
    MI_U32 u32Grad;
} MI_ISP_AE_EV_COMP_TYPE_t;

typedef struct MI_ISP_AE_INTP_LUT_TYPE_s
{
    MI_U16 u16NumOfPoints;
    MI_U32 u32Y[16];
    MI_U32 u32X[16];
} MI_ISP_AE_INTP_LUT_TYPE_t;

typedef struct CONV_SPEED_PARAM_s
{
    MI_U32 u32SpeedX[4];
    MI_U32 u32SpeedY[4];
} CONV_SPEED_PARAM_t;

typedef struct CONV_THD_PARAM_s
{
    MI_U32 u32InThd;
    MI_U32 u32OutThd;
} CONV_THD_PARAM_t;

typedef struct MI_ISP_AE_CONV_CONDITON_TYPE_s
{
    CONV_THD_PARAM_t stConvThrd;
    CONV_SPEED_PARAM_t stConvSpeed;
} MI_ISP_AE_CONV_CONDITON_TYPE_t;

typedef struct MI_ISP_AE_EXPO_LIMIT_TYPE_s
{
    MI_U32 u32MinShutterUS;
    MI_U32 u32MaxShutterUS;
    MI_U32 u32MinFNx10;
    MI_U32 u32MaxFNx10;
    MI_U32 u32MinSensorGain;
    MI_U32 u32MinISPGain;
    MI_U32 u32MaxSensorGain;
    MI_U32 u32MaxISPGain;
} MI_ISP_AE_EXPO_LIMIT_TYPE_t;

typedef struct EXPO_POINT_PARAM_s
{
    MI_U32 u32FNumx10;
    MI_U32 u32Shutter;
    MI_U32 u32TotalGain;
    MI_U32 u32SensorGain;
} EXPO_POINT_PARAM_t;

typedef struct MI_ISP_AE_EXPO_TABLE_TYPE_s
{
    MI_U32 u32NumPoints;
    EXPO_POINT_PARAM_t stExpoTbl[16];   // LV from High to Low
} MI_ISP_AE_EXPO_TABLE_TYPE_t;

typedef struct WIN_WEIGHT_PARAM_s
{
    MI_U8 u8AverageTbl[32*32];
    MI_U8 u8CenterTbl[32*32];
    MI_U8 u8SpotTbl[32*32];
} WIN_WEIGHT_PARAM_t;

typedef struct MI_ISP_AE_WIN_WEIGHT_TYPE_s
{
    MI_ISP_AE_WIN_WEIGHT_MODE_TYPE_e eTypeID;
    WIN_WEIGHT_PARAM_t stParaAPI;
} MI_ISP_AE_WIN_WEIGHT_TYPE_t;

typedef struct MI_ISP_AE_STRATEGY_TYPE_s
{
    MI_ISP_AE_STRATEGY_TYPE_e   eAEStrategyMode;
    MI_U32                      u32Weighting;
    MI_ISP_AE_INTP_LUT_TYPE_t   stUpperOffset;
    MI_ISP_AE_INTP_LUT_TYPE_t   stLowerOffset;
    MI_U32                      u32BrightToneStrength;
    MI_U32                      u32BrightToneSensitivity;
    MI_U32                      u32DarkToneStrength;
    MI_U32                      u32DarkToneSensitivity;
    MI_U32                      u32AutoStrength;
    MI_U32                      u32AutoSensitivity;
} MI_ISP_AE_STRATEGY_TYPE_t;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_STRATEGY_EX_BRIGHTTONE,
    SS_AE_STRATEGY_EX_DARKTONE,
} MI_ISP_AE_STRATEGY_EX_PRIORITY_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_STRATEGY_EX_COUNT,
    SS_AE_STRATEGY_EX_TARGET,
} MI_ISP_AE_STRATEGY_EX_MODE_e;

typedef struct MI_ISP_AE_STRATEGY_EX_TYPE_s
{
    MI_ISP_BOOL_e                    bEnable;
    MI_ISP_AE_STRATEGY_EX_MODE_e     eMode;
    MI_ISP_AE_STRATEGY_EX_PRIORITY_e ePriority;
    MI_U16 u16BT_NodeNum;
    MI_S32 u32BT_NodeBV[16];
    MI_U32 u32BT_ThdY[16];
    MI_U32 u32BT_Percentx10[16];
    MI_U32 u32BT_TargetYx10[16];
    MI_U32 u32BT_MaxOffsetDown[16];
    MI_U16 u16DT_NodeNum;
    MI_S32 u32DT_NodeBV[16];
    MI_U32 u32DT_ThdY[16];
    MI_U32 u32DT_Percentx10[16];
    MI_U32 u32DT_TargetYx10[16];
    MI_U32 u32DT_MaxOffsetUp[16];
} MI_ISP_AE_STRATEGY_EX_TYPE_t;

typedef struct MI_ISP_AE_STRATEGY_EX_INFO_TYPE_s
{
    MI_U16 u16GMBlendRatio;
    MI_U16 u16UpperLimitTargetx10;
    MI_U16 u16LowerLimitTargetx10;
    MI_U16 u16BTCntPcntx10;
    MI_U16 u16DTCntPcntx10;
    MI_U16 u16BTYx10;
    MI_U16 u16DTYx10;
} MI_ISP_AE_STRATEGY_EX_INFO_TYPE_t;

typedef struct MI_ISP_AE_RGBIR_AE_TYPE_s
{
    MI_ISP_BOOL_e  bEnable;
    MI_U16       u16MaxYWithIR;
    MI_U16       u16MinISPGainCompRatio;
} MI_ISP_AE_RGBIR_AE_TYPE_t;

typedef struct MI_ISP_AE_HDR_TYPE_s
{
    MI_ISP_AE_INTP_LUT_TYPE_t stAeHdrRatio;
} MI_ISP_AE_HDR_TYPE_t;

typedef struct MI_ISP_AE_STABILIZER_TYPE_s
{
    MI_ISP_BOOL_e  bEnable;
    MI_U16         u16DiffThd;
    MI_U16         u16Percent;
} MI_ISP_AE_STABILIZER_TYPE_t;

/************************************* AE  - Define the structure declaration - END   *************************************/

/************************************* AWB - Define the structure declaration - START *************************************/

#define MI_ISP_AWB_LV_CT_TBL_NUM        (18)
#define MI_ISP_AWB_CT_TBL_NUM           (10)
#define MI_ISP_AWB_WEIGHT_WIN_NUM       (81)

typedef struct MI_ISP_AWB_QUERY_INFO_TYPE_s
{
    MI_ISP_BOOL_e bIsStable;
    MI_U16 u16Rgain;
    MI_U16 u16Grgain;
    MI_U16 u16Gbgain;
    MI_U16 u16Bgain;
    MI_U16 u16ColorTemp;
    MI_U8  u8WPInd;
    MI_ISP_BOOL_e bMultiLSDetected;
    MI_U8  u8FirstLSInd;
    MI_U8  u8SecondLSInd;
} MI_ISP_AWB_QUERY_INFO_TYPE_t;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AWB_MODE_AUTO,
    SS_AWB_MODE_MANUAL,
    SS_AWB_MODE_CTMANUAL,
    SS_AWB_MODE_MAX
} MI_ISP_AWB_MODE_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AWB_ALG_GRAYWORLD = 0,
    SS_AWB_ALG_NORMAL    = 1,
    SS_AWB_ALG_BALANCE   = 2,
    SS_AWB_ALG_FOCUS     = 3,
    SS_AWB_ALG_MAX       = 0xffffffff
} MI_ISP_AWB_ALG_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AWB_ADV_DEFAULT = 0,
    SS_AWB_ADV_ADVANCE = 1,
    SS_AWB_ADV_MAX     = 0xffffffff
} MI_ISP_AWB_ADV_TYPE_e;

typedef struct CT_LIMIT_PARAM_s
{
    MI_U16 u16MaxRgain;                                         //RW, Maximum  RGain, Range: [0, 8191]
    MI_U16 u16MinRgain;                                         //RW, Miniimum RGain, Range: [0, 8191]
    MI_U16 u16MaxBgain;                                         //RW, Maximum  BGain, Range: [0, 8191]
    MI_U16 u16MinBgain;                                         //RW, Miniimum BGain, Range: [0, 8191]
} CT_LIMIT_PARAM_t;

typedef struct CT_WEIGHT_PARAM_s
{
    MI_U16 u16Weight[MI_ISP_AWB_CT_TBL_NUM];                    //RW, Light CT Weight, Range: [1, 255]
} CT_WEIGHT_PARAM_t;

typedef struct CT_RATIO_PARAM_s
{
    MI_U16 u16Ratio[MI_ISP_AWB_CT_TBL_NUM];                     //RW, CT Prefer Ratio, Range: [1, 255]
} CT_RATIO_PARAM_t;

typedef struct AWB_ATTR_PARAM_s
{
    MI_U8  u8Speed;                                             //RW, AWB converging speed, Range: [0x1, 0x64]
    MI_U8  u8ConvInThd;                                         //RW, AWB converging threshold, Range:[0, 255], Recommended: [32]
    MI_U8  u8ConvOutThd;                                        //RW, AWB converging threshold, Range:[0, 255], Recommended: [64]
    MI_ISP_AWB_ALG_TYPE_e eAlgType;                             //RW, AWB 0:GrayWorld, 1:Normal, 2:Balance 3,Focus
    MI_ISP_AWB_ADV_TYPE_e eAdvType;                             //RW, AWB advance mode type
    MI_U8  u8RGStrength;                                        //RW, AWB adjust RG ratio, Range:[0, 255]
    MI_U8  u8BGStrength;                                        //RW, AWB adjust BG ratio, Range:[0, 255]
    CT_LIMIT_PARAM_t stCTLimit;                                 //RW, AWB limitation when envirnoment ct is out of boundary
    CT_WEIGHT_PARAM_t stLvWeight[MI_ISP_AWB_LV_CT_TBL_NUM];     //RW, AWB Lv Ct Weight, Range: [0, 255]
    CT_RATIO_PARAM_t stPreferRRatio[MI_ISP_AWB_LV_CT_TBL_NUM];  //RW, AWB prefer R gain, Range: [0, 255]
    CT_RATIO_PARAM_t stPreferBRatio[MI_ISP_AWB_LV_CT_TBL_NUM];  //RW, AWB prefer B gain, Range: [0, 255]
    MI_U16 u16WpWeight[MI_ISP_AWB_CT_TBL_NUM];
    MI_U32 u32WeightWin[MI_ISP_AWB_WEIGHT_WIN_NUM];             //RW, AWB region gain, Range: [0, 16]
} AWB_ATTR_PARAM_t;

typedef struct MWB_ATTR_PARAM_s
{
    MI_U16 u16Rgain;                                            //RW, Multiplier for R  color channel, Range: [0, 0x2000]
    MI_U16 u16Grgain;                                           //RW, Multiplier for Gr color channel, Range: [0, 0x2000]
    MI_U16 u16Gbgain;                                           //RW, Multiplier for Gb color channel, Range: [0, 0x2000]
    MI_U16 u16Bgain;                                            //RW, Multiplier for B  color channel, Range: [0, 0x2000]
} MWB_ATTR_PARAM_t;

typedef struct MI_ISP_AWB_ATTR_TYPE_s
{
    MI_ISP_SM_STATE_TYPE_e eState;
    MI_ISP_AWB_MODE_TYPE_e eOpType;
    MWB_ATTR_PARAM_t stManualParaAPI;
    AWB_ATTR_PARAM_t stAutoParaAPI;
} MI_ISP_AWB_ATTR_TYPE_t;

typedef struct AWB_ATTR_EXTRA_LIGHTSOURCE_PARAM_s
{
    MI_U16 u16WhiteRgain;                                       //RW, RGain of white Point Location , Range: [256, 4095]
    MI_U16 u16WhiteBgain;                                       //RW, RGain of white Point Location , Range: [256, 4095]
    MI_U8  u8AreaSize;                                          //RW, Light Area Size , Range: [1, 32]
    MI_ISP_BOOL_e bExclude;                                     //RW, Include or exclude Uaer light Area, 0: include, 1:exclude
} AWB_ATTR_EXTRA_LIGHTSOURCE_PARAM_t;

typedef struct MI_ISP_AWB_ATTR_EX_TYPE_s
{
    MI_ISP_BOOL_e bExtraLightEn;
    AWB_ATTR_EXTRA_LIGHTSOURCE_PARAM_t stLightInfo[4];
} MI_ISP_AWB_ATTR_EX_TYPE_t;

typedef struct MI_ISP_AWB_MULTILS_LS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;
    MI_U8  u8Sensitive;
    MI_U8  u8CaliStrength;
    MI_U16 u16CcmForLow[9];
    MI_U16 u16CcmForHigh[9];
} MI_ISP_AWB_MULTILS_LS_TYPE_t;

typedef struct MI_ISP_AWB_CT_WEIGHT_TYPE_s
{
    MI_U16 u16LvIndex;
    CT_WEIGHT_PARAM_t stParaAPI;
} MI_ISP_AWB_CT_WEIGHT_TYPE_t;

typedef struct MI_ISP_AWB_CT_CALI_TYPE_s
{
    MI_U16 u16StartIdx;                                         //RW, Light area start index, Range: [0, u2EndIdx]
    MI_U16 u16EndIdx;                                           //RW, Light area end index, Range: [u2StartIdx, 9]
    MI_U16 u16CtParams[40];                                     //RW, Color temperature of calibration paramters , Range: [1, 1000]
} MI_ISP_AWB_CT_CALI_TYPE_t;

typedef struct MI_ISP_AWB_CUR_CT_CALI_TYPE_s
{
    MI_U16 u16CtParams[40];                                     //RW, Color temperature of calibration paramters , Range: [1, 1000]
} MI_ISP_AWB_CUR_CT_CALI_TYPE_t;

typedef struct MI_ISP_AWB_CT_STATISTICS_TYPE_s
{
    MI_U16 u16Width;                                            //RW, Effective range
    MI_U16 u16Height;                                           //RW, Effective range
    MI_U16 u16StatisX[5760];                                    //RW, Color Temperature Curve Domain Statistics X, max is 64x90
    MI_U16 u16StatisY[5760];                                    //RW, Color Temperature Curve Domain Statistics Y, max is 64x90
} MI_ISP_AWB_CT_STATISTICS_TYPE_t;

typedef struct MI_ISP_AWB_HW_STATISTICS_TYPE_s
{
    MI_U8 u8AwbBuffer[34560]; // 128 * 90 * 3
} MI_ISP_AWB_HW_STATISTICS_TYPE_t;

typedef struct MI_ISP_AWB_CTMWB_PARAM_s
{
    MI_U32 u32CT;
} MI_ISP_AWB_CTMWB_PARAM_t;

/************************************* AWB - Define the structure declaration - END   *************************************/

#endif //_MI_ISP_3A_DATATYPE_H_
