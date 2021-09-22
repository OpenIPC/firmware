/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_awb_common.h
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

#ifndef __XM_AWB_COMM_H__
#define __XM_AWB_COMM_H__

#include "xm_type.h"
#include "xm_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define XM_AWB_LIB_NAME "xm_awb_lib"

/************************** isp ctrl cmd *************************************/
typedef enum xmAWB_CTRL_CMD_E
{
    AWB_SATURATION_SET,
    AWB_SATURATION_GET,
    
    AWB_DEBUG_ATTR_SET,
    AWB_DEBUG_ATTR_GET,

    AWB_CTRL_BUTT,
} AWB_CTRL_CMD_E;

typedef struct xmAWB_DBG_ATTR_S
{
    XM_U16 u16WhiteLevel;
    XM_U16 u16BlackLevel;
    XM_U16 u16CrMax;
    XM_U16 u16CrMin;
    XM_U16 u16CbMax;
    XM_U16 u16CbMin;
    XM_U16 u16CrHigh;
    XM_U16 u16CrLow;
    XM_U16 u16CbHigh;
    XM_U16 u16CbLow;
    XM_U16 u16RawWhiteLevel;
    XM_U16 u16RawBlackLevel;
    XM_U16 u16RawCrMax;
    XM_U16 u16RawCrMin;
    XM_U16 u16RawCbMax;
    XM_U16 u16RawCbMin;
    XM_U16 u16RawCrHigh;
    XM_U16 u16RawCrLow;
    XM_U16 u16RawCbHigh;
    XM_U16 u16RawCbLow;

    XM_U16 u16WDRMode;
    XM_U16 u16Enable;
    XM_U16 u16ManualEnable;
    XM_U16 u16Zone;
    XM_U16 u16HighTemp;
    XM_U16 u16LowTemp;
    XM_U16 u16RefTemp;
    XM_U16 u16RgainBase;
    XM_U16 u16GgainBase;
    XM_U16 u16BgainBase;
    XM_S32 s32p1;
    XM_S32 s32p2;
    XM_S32 s32q;
    XM_S32 s32a;
    XM_S32 s32c;
    
    XM_U16 u16ManSatEnable;
    XM_U16 u16SatTarget;
} AWB_DBG_ATTR_S;

typedef struct xmAWB_ZONE_DBG_S
{
    XM_U16 u16Sum;
    XM_U16 u16Rg;
    XM_U16 u16Bg;
    XM_U16 u16CountAll;
    XM_U16 u16CountMin;
    XM_U16 u16CountMax;
    XM_U16 u16RawRAvg;
    XM_U16 u16RawGAvg;
    XM_U16 u16RawBAvg;
    XM_U16 u16TK;
    XM_U16 u16Weight;
    XM_S16 s16Shift;
}AWB_ZONE_DBG_S;

typedef struct xmAWB_DBG_STATUS_S
{
    XM_U32 u32FrmNumBgn;
    XM_U32 u32GlobalSum;
    XM_U16 u16GlobalRgSta;
    XM_U16 u16GlobalBgSta;
    XM_U16 u16GlobalCountAll;
    XM_U16 u16GlobalCountMin;
    XM_U16 u16GlobalCountMax;
    XM_U16 u16GlobalRAvg;
    XM_U16 u16GlobalGAvg;
    XM_U16 u16GlobalBAvg;
    XM_U16 u16TK;
    XM_U16 u16Rgain;
    XM_U16 u16Ggain;
    XM_U16 u16Bgain;
    XM_U16 au16CCM[9];

    XM_U32 au32HistInfo[256];
    AWB_ZONE_DBG_S astZoneDebug[255];

    XM_U32 u32FrmNumEnd;    
} AWB_DBG_STATUS_S;

/************************** sensor's interface to awb *********************/
typedef struct xmAWB_AGC_TABLE_S
{
	XM_BOOL bValid;
	XM_U8   au8Saturation[16];   /* adjust saturation, different iso with different saturation */
} AWB_AGC_TABLE_S;

typedef struct xmAWB_COEF_TABLE_S
{
	XM_U16   u16R;
	XM_U16   u16B;
} AWB_COEF_TABLE_S;


typedef struct xmISP_AWB_CALIBRATION_V2_S
{
    XM_S16 A[8];
    XM_S16 B[8];
    XM_S16 C[8];
    XM_U16 key;
    XM_U16 ini_x;
    XM_U16 ini_y;
    XM_U16 dis_min;
    XM_U16 dis_max;
    XM_U16 dis[16];	
    XM_U16 val[16];	
    XM_U16 init_gain[4];	
}ISP_AWB_CALIBRATION_V2_S;


typedef struct xmAWB_SENSOR_DEFAULT_S
{
	ISP_COLORMATRIX_AUTO_S stCcm;
	ISP_AWB_CALIBRATION_S stAwbCal;
	const AWB_COEF_TABLE_S* pstRbTable;	// Number Must is 161
} AWB_SENSOR_DEFAULT_S;

typedef struct xmAWB_SENSOR_EXP_FUNC_S
{
    XM_S32(*pfn_cmos_get_awb_default)(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft);
} AWB_SENSOR_EXP_FUNC_S;

typedef struct xmAWB_SENSOR_REGISTER_S
{
    AWB_SENSOR_EXP_FUNC_S stSnsExp;
} AWB_SENSOR_REGISTER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
