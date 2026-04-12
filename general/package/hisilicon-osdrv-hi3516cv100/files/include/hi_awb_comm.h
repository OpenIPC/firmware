/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_awb_comm.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/19
  Description   : 
  History       :
  1.Date        : 2012/12/19
    Author      : n00168968
    Modification: Created file

******************************************************************************/
#ifndef __HI_AWB_COMM_H__
#define __HI_AWB_COMM_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_AWB_LIB_NAME "hisi_awb_lib"

/************************** isp ctrl cmd *************************************/
typedef enum hiAWB_CTRL_CMD_E
{
    AWB_SATURATION_SET,
    AWB_SATURATION_GET,
    
    AWB_DEBUG_ATTR_SET,
    AWB_DEBUG_ATTR_GET,

    AWB_CTRL_BUTT,
} AWB_CTRL_CMD_E;

typedef struct hiAWB_DBG_ATTR_S
{
    HI_U16 u16Enable;
    HI_U16 u16ManualEnable;
    HI_U32 u32HighTemp;
    HI_U32 u32LowTemp;
    HI_U32 u32RefTemp;
    HI_U32 u32Zone;
    HI_U32 u32DevEnable;
    HI_U32 u32ShiftLimit;
    HI_U32 u32WhiteLevel;
    HI_U32 u32BlackLevel;
    HI_U32 u32CrMax;
    HI_U32 u32CrMin;
    HI_U32 u32CbMax;
    HI_U32 u32CbMin;
    HI_U32 u32RgainBase;
    HI_U32 u32GgainBase;
    HI_U32 u32BgainBase;
    HI_S32 s32p1;
    HI_S32 s32p2;
    HI_S32 s32q;
    HI_S32 s32a;
    HI_S32 s32c;
    
    HI_U16 u16ManSatEnable;
    HI_U16 u16SatTarget;
} AWB_DBG_ATTR_S;

typedef struct hiAWB_ZONE_DBG_S
{
    HI_U16 u16Num;
    HI_U32 u32Sum;
    HI_U16 u16Rg;
    HI_U16 u16Bg;
    HI_U32 u32TK;
    HI_S32 s32Shift;
    HI_U32 u32Weight;
    HI_S32 s32Dev;    
}AWB_ZONE_DBG_S;

typedef struct hiAWB_DBG_STATUS_S
{
    HI_U32 u32FrmNumBgn;
    HI_U32 u32GlobalSum;
    HI_U32 u32GlobalRgSta;
    HI_U32 u32GlobalBgSta;
    HI_U32 u32TK;
    HI_U16 u16Rgain;
    HI_U16 u16Ggain;
    HI_U16 u16Bgain;
    HI_U16 au16CCM[9];
    
    AWB_ZONE_DBG_S astZoneDebug[255];

    HI_U32 u32FrmNumEnd;    
} AWB_DBG_STATUS_S;

/************************** sensor's interface to awb *********************/
typedef struct hiAWB_CCM_S
{    
    HI_U16 u16HighColorTemp;    /* D50 lighting source is  recommended */
    HI_U16 au16HighCCM[9];
    HI_U16 u16MidColorTemp;     /* D32 lighting source is  recommended */
    HI_U16 au16MidCCM[9];       
    HI_U16 u16LowColorTemp;     /* A lighting source is  recommended */
    HI_U16 au16LowCCM[9];
}AWB_CCM_S;

typedef struct hiAWB_AGC_TABLE_S
{
    HI_BOOL bValid;
    
    HI_U8   au8Saturation[8];   /* adjust saturation, different iso with different saturation */
} AWB_AGC_TABLE_S;

typedef struct hiAWB_SENSOR_DEFAULT_S
{
    HI_U16  u16WbRefTemp;       /* reference color temperature for WB  */    
    HI_U16  au16GainOffset[4];  /* gain offset for white balance */
    HI_S32  as32WbPara[6];      /* parameter for wb curve,p1,p2,q1,a1,b1,c1 */

    AWB_AGC_TABLE_S stAgcTbl;
    AWB_CCM_S stCcm;
} AWB_SENSOR_DEFAULT_S;

typedef struct hiAWB_SENSOR_EXP_FUNC_S
{
    HI_S32(*pfn_cmos_get_awb_default)(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft);
} AWB_SENSOR_EXP_FUNC_S;

typedef struct hiAWB_SENSOR_REGISTER_S
{
    AWB_SENSOR_EXP_FUNC_S stSnsExp;
} AWB_SENSOR_REGISTER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
