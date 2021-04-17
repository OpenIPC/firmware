/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_ae_common.h
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
#ifndef __XM_AE_COMM_H__
#define __XM_AE_COMM_H__

#include "xm_type.h"


#define XM_AE_LIB_NAME 	"xm_ae_lib"
#define XM_AWB_LIB_NAME "xm_awb_lib"

/************************** ae ctrl cmd **************************************/
typedef enum xm_AE_CTRL_CMD_E
{
    AE_DEBUG_ATTR_SET,
    AE_DEBUG_ATTR_GET, 

    AE_CTRL_BUTT,
} AE_CTRL_CMD_E;

typedef struct xm_AE_DBG_ATTR_S
{
    XM_BOOL bAeBypass;
    XM_BOOL bFrameEndUpdateMode;
    XM_U32  u32MaxAgain;
    XM_U32  u32MinAgin;
    XM_U32  u32MaxDgain;
    XM_U32  u32MinDgain;
    XM_U32  u32MaxIspDgain;
    XM_U32  u32MinIspDgain;
    XM_U32  u32MaxIntTime;
    XM_U32  u32MinIntTime;
    XM_U32  u32Compensation;
#if 0
    XM_U32  BlackLevel_R;
    XM_U32  BlackLevel_Gr;
    XM_U32  BlackLevel_Gb;
    XM_U32  BlackLevel_B;
#endif
    XM_U32  u32Hist01;
    XM_U32  u32Hist12;
    XM_U32  u32Hist34;
    XM_U32  u32Hist45;
    XM_BOOL bManualExposureEn;
    XM_BOOL bManualAgainEn;
    XM_BOOL bManualDgainEn;
    XM_BOOL bManualIspDgainEn;
    XM_U32  u32ManualExposureLines;
    XM_U32  u32ManualAgain;
    XM_U32  u32ManualDgain;
    XM_U32  u32ManualIspDgain;
    XM_U32  au32AeWeights[255];    
}AE_DBG_ATTR_S;

typedef struct xm_AE_DBG_STATUS_S
{
    XM_U32  u32FrmNumBgn;
    XM_U32  u32FullLines;
    XM_U32  u32IntTime;
    XM_U32  u32Again;
    XM_U32  u32AgainShift;
    XM_U32  u32Dgain;
    XM_U32  u32DgainShift;
    XM_U32  u32IspDgain;
    XM_U32  u32IspDgainShift;
    XM_U32  u32Exposure;
    XM_U32  u32Increment;
    XM_U32  u32HistBalance;
    XM_S32  u32HistError;
    XM_U32  u32ExpoureStep;
    XM_U32  u32FrmNumEnd;   
}AE_DBG_STATUS_S;

/************************** sensor's interface to ae *********************/

/* eg: 0.35db, enAccuType=AE_ACCURACY_DB, f32Accuracy=0.35 
*  and the multiply of 0.35db is power(10, (0.35/20))
*  eg: 1/16, 2/16, 3/16 multiplies, enAccuType=AE_ACCURACY_LINEAR, f32Accuracy=0.0625
*  eg: 1,2,4,8,16 multiplies, enAccuType=AE_ACCURACY_DB, f32Accuracy=6
*/
typedef enum xm_AE_ACCURACY_E
{
    AE_ACCURACY_DB = 0,
    AE_ACCURACY_LINEAR,
    AE_ACCURACY_TABLE,
    
    AE_ACCURACY_BUTT,
} AE_ACCURACY_E;

typedef struct xm_AE_ACCURACY_S
{
    AE_ACCURACY_E enAccuType;
    float   f32Accuracy;
} AE_ACCURACY_S;

typedef struct xm_AE_SENSOR_DEFAULT_S
{
    XM_U8   u8AeCompensation;
	XM_U8 	u8UpdateMode;	// 0: Old	1:New(sns_regs)
    XM_U32  u32LinesPer500ms;
    XM_U32  u32FlickerFreq;

    XM_U32  u32FullLinesStd;
    XM_U32  u32MaxIntTime;     /* unit is line */
    XM_U32  u32MinIntTime;
    XM_U32  u32MaxIntTimeTarget;
    XM_U32  u32MinIntTimeTarget;
    
    XM_U32  u32MaxAgain;
    XM_U32  u32MinAgain;
    XM_U32  u32MaxAgainTarget;
    XM_U32  u32MinAgainTarget;
    AE_ACCURACY_S stAgainAccu;

    XM_U32  u32MaxDgain;
    XM_U32  u32MinDgain;
    XM_U32  u32MaxDgainTarget;
    XM_U32  u32MinDgainTarget;
    AE_ACCURACY_S stDgainAccu;

	XM_U32  u32MaxISPDgain;
    XM_U32  u32MinISPDgain;
    XM_U32  u32MaxISPDgainTarget;
    XM_U32  u32MinISPDgainTarget;
    XM_U32  u32ISPDgainShift;

/****** 新增加部分*****/
	XM_U32  u32LinePixel;		//  1行像素个数
	XM_U32  u32TimeFinePoint;	//  < u32TimeFinePoint考虑细调
/********************************
u32UpdateCfg:
	bit[0~7]  period
	bit[8~15] shut 
	bit[16~23] snsGain
	bit[24~31] ispGain
********************************/
	XM_U32  u32UpdateCfg;
	XM_U32  u32MaxAllgain;
	XM_U32  u32InitExp[4];
/********************************
u16AlgMode:
0x00: fix V1 
	0x01: first V1, after stab bypass AE
0x10: fix V2
	0x11: first V2, after stab bypass AE
0x20: first V2, after stab V1 
********************************/
	XM_U16 u16AlgMode;
	XM_BOOL	 bDelayCfg;
} AE_SENSOR_DEFAULT_S;

typedef struct xm_AE_SENSOR_DEFAULT_V2_S
{
	XM_U32  u32MinIntTime;
	XM_U32  u32MaxIntTime;     /* unit is line */
	XM_U32  u32MinAgain;
	XM_U32  u32MaxAgain;
	XM_U32  u32MinDgain;
	XM_U32  u32MaxDgain;
	XM_U32  u32MaxISPDgain;
/********************************
u32UpdateCfg:
	bit[0~7]  period
	bit[8~15] shut 
	bit[16~23] snsGain
	bit[24~31] ispGain
********************************/
	XM_U32	u32UpdateCfg;
	XM_U32  u32InitExp[4];
/********************************
u16AlgMode:
0x00: fix V1 
	0x01: first V1, after stab bypass AE
0x10: fix V2
	0x11: first V2, after stab bypass AE
0x20: first V2, after stab V1 
********************************/
	XM_U16 u16AlgMode;
} AE_SENSOR_DEFAULT_V2_S;


typedef struct xm_AE_SENSOR_GAININFO_S
{
    XM_U32  u32SnsTimes;  //10bit precision
    XM_U32  u32GainDb;    // gain step in db
    
} AE_SENSOR_GAININFO_S;

typedef struct xm_AE_SENSOR_SHUTINFO_S
{
    XM_U64  u64Exp;  	//10bit precision
    XM_U32  u32Ofst;    //10bit precision    
    XM_U32	u32ShutDb;
} AE_SENSOR_SHUTINFO_S;

typedef enum xm_ISP_SNS_TYPE_E
{
    ISP_SNS_I2C_TYPE = 0,
    ISP_SNS_SSP_TYPE,
    
    ISP_SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

typedef struct hiISP_I2C_DATA_S
{
	XM_BOOL bUpdate;
	XM_U8   u8DelayFrmNum;
	XM_U8   u8IntPos;
	XM_U32  u32RegAddr;
	XM_U32  u32Data;
} ISP_I2C_DATA_S;

typedef struct hiISP_SSP_DATA_S
{
	XM_BOOL bUpdate;
	XM_U8   u8DelayFrmNum;
	XM_U8   u8IntPos;
	XM_U32  u32DevAddr;
	XM_U32  u32DevAddrByteNum;
	XM_U32  u32RegAddr;
	XM_U32  u32RegAddrByteNum;
	XM_U32  u32Data;
	XM_U32  u32DataByteNum;
} ISP_SSP_DATA_S;


typedef struct xm_ISP_SNS_REGS_INFO_S
{
    ISP_SNS_TYPE_E enSnsType;
    XM_U32  u32RegNum;
    XM_U8   u8Cfg2ValidDelayMax;

    union
    {
        ISP_I2C_DATA_S *pstI2CData;
        ISP_SSP_DATA_S *pstSspData;
    };
} ISP_SNS_REGS_INFO_S;

typedef struct xm_AE_SENSOR_EXP_FUNC_S
{
    XM_S32(*pfn_cmos_get_ae_default)(AE_SENSOR_DEFAULT_S *pstAeSnsDft);

    /* the function of sensor set fps */
    XM_VOID(*pfn_cmos_fps_set)(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft);
    XM_S32(*pfn_cmos_fps_get)(XM_U8 *pu8Fps);
    XM_VOID(*pfn_cmos_slow_framerate_set)(XM_U16 u16SlowFrameRate, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

    /* while isp notify ae to update sensor regs, ae call these funcs. */
    XM_VOID(*pfn_cmos_inttime_update)(XM_U32 u32IntTime);
    XM_VOID(*pfn_cmos_gains_update)(XM_U32 u32Again, XM_U32 u32Dgain);
	XM_VOID(*pfn_cmos_shut_calc_table)(XM_S32 s32IntTime,AE_SENSOR_SHUTINFO_S *pstAeSnsShutInfo);
	XM_VOID(*pfn_cmos_gains_update2)(XM_S32 s32DGain,const AE_SENSOR_DEFAULT_S *pstAeSnsDft);

    XM_VOID (*pfn_cmos_again_calc_table)(XM_U32 u32InTimes, AE_SENSOR_GAININFO_S *pstAeSnsGainInfo);
    XM_VOID (*pfn_cmos_dgain_calc_table)(XM_U32 u32InTimes, AE_SENSOR_GAININFO_S *pstAeSnsGainInfo);

    XM_S32(*pfn_cmos_get_sns_reg_info)(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);
    XM_S32(*pfn_cmos_write_register)(XM_U32 addr, XM_U32 data);
    XM_S32(*pfn_cmos_read_register)(XM_U32 addr);	
} AE_SENSOR_EXP_FUNC_S;

typedef struct xm_AE_SENSOR_REGISTER_S
{
    AE_SENSOR_EXP_FUNC_S stSnsExp;
} AE_SENSOR_REGISTER_S;


typedef struct xm_ISP_AE_VARLED_REGISTER_S
{
	XM_BOOL	bEnable;
	ISP_OP_TYPE_E   enOpType;
	XM_U32 u32ManualLvl;
	XM_U32 u32LvlNow;		//RO
	XM_U32 u32AutoSpeed;	//0~255(default:128)
	XM_U32 u32MaxLvl;
	XM_U32 u32MinLvl;
	XM_U32 u32ExpThreshold_1;	// <:   weaken
	XM_U32 u32ExpThreshold_2;	// >:	enhance
	XM_S32(*pfn_varLED_init)(XM_VOID);
	XM_VOID(*pfn_varLED_update)(XM_U32 u32Lvl);
	XM_VOID(*pfn_varLED_calc_table)(XM_U32 u32InTimes, AE_SENSOR_GAININFO_S *pstAeSnsGainInfo);
}ISP_AE_VARLED_REGISTER_S;

#endif
