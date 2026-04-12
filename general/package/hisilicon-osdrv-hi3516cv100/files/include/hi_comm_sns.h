/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_sns.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/01/05
  Description   : 
  History       :
  1.Date        : 2011/01/05
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_SNS_H__
#define __HI_COMM_SNS_H__

#include "hi_type.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiISP_CMOS_BLACK_LEVEL_S
{
    HI_BOOL bUpdate;
    
    HI_U16  au16BlackLevel[4];
} ISP_CMOS_BLACK_LEVEL_S;

typedef struct hiISP_CMOS_AGC_TABLE_S
{
    HI_BOOL bValid;
    
    HI_U8   au8SharpenAltD[8];          /* adjust image edge,different iso with different sharp strength */
    HI_U8   au8SharpenAltUd[8];         /* adjust image texture, different iso with different strength */
    HI_U8   au8SnrThresh[8];            /* adjust denoise strength, different iso with different strength */
    HI_U8   au8DemosaicLumThresh[8];  
    HI_U8   au8DemosaicNpOffset[8];
    HI_U8   au8GeStrength[8];
} ISP_CMOS_AGC_TABLE_S;

typedef struct hiISP_CMOS_NOISE_TABLE_S
{
    HI_BOOL bValid;
    
    HI_U8   au8NoiseProfileWeightLut[128];
    HI_U8   au8DemosaicWeightLut[128];
} ISP_CMOS_NOISE_TABLE_S;

typedef struct hiISP_CMOS_DEMOSAIC_S
{
    HI_BOOL bValid;
    
    HI_U8   u8VhSlope;
    HI_U8   u8AaSlope;
    HI_U8   u8VaSlope;
    HI_U8   u8UuSlope;
    HI_U8   u8SatSlope;
    HI_U8   u8AcSlope;
    HI_U16  u16VhThresh;
    HI_U16  u16AaThresh;
    HI_U16  u16VaThresh;
    HI_U16  u16UuThresh;
    HI_U16  u16SatThresh;
    HI_U16  u16AcThresh;
} ISP_CMOS_DEMOSAIC_S;

typedef struct hiISP_CMOS_DRC_S
{
    HI_U8   u8DrcBlack;
    HI_U8   u8DrcVs;         /* variance space */
    HI_U8   u8DrcVi;         /* variance intensity */
    HI_U8   u8DrcSm;         /* slope max */
    HI_U16  u16DrcWl;        /* white level */
} ISP_CMOS_DRC_S;

#define LUT_FACTOR (8)
#define GAMMA_FE_LUT_SIZE ((1<<LUT_FACTOR)+1)
typedef struct hiISP_CMOS_GAMMAFE_S
{
    HI_BOOL bValid;         /* wdr sensor should set */
    
    HI_U16  au16Gammafe[GAMMA_FE_LUT_SIZE];
} ISP_CMOS_GAMMAFE_S;

typedef struct hiISP_CMOS_DENOISE_S
{
    HI_U8   u8SinterThresh;     /* sinter threshold */
    
    HI_U8   u8NoiseProfile;     /* two different noise profile */ 
    HI_U16  u16Nr0;             /* nr0 for noise profile 2 */
    HI_U16  u16Nr1;             /* nr1 for noise profile 2 */
} ISP_CMOS_DENOISE_S;

typedef struct hiISP_CMOS_COMM_S
{
    HI_U8   u8Rggb;             /* rggb start sequence */    
    HI_U8   u8BalanceFe;
} ISP_CMOS_COMM_S;

#define CMOS_SHADING_TABLE_NODE_NUMBER_MAX (129)
typedef struct hiISP_CMOS_SHADING_S
{
    HI_BOOL bValid;
    
    HI_U16 u16RCenterX;
    HI_U16 u16RCenterY;
    HI_U16 u16GCenterX;
    HI_U16 u16GCenterY;
    HI_U16 u16BCenterX;
    HI_U16 u16BCenterY;

    HI_U16 au16RShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    HI_U16 au16GShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    HI_U16 au16BShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];

    HI_U16 u16ROffCenter;
    HI_U16 u16GOffCenter;
    HI_U16 u16BOffCenter;

    HI_U16 u16TblNodeNum;
} ISP_CMOS_SHADING_S;

#define GAMMA_NODE_NUMBER   257
typedef struct hiISP_CMOS_GAMMA_S
{
    HI_BOOL bValid;
    
    HI_U16  au16Gamma[GAMMA_NODE_NUMBER];
} ISP_CMOS_GAMMA_S;

typedef struct hiISP_CMOS_DEFAULT_S
{    
    ISP_CMOS_COMM_S         stComm;
    ISP_CMOS_DENOISE_S      stDenoise;
    ISP_CMOS_DRC_S          stDrc;
    ISP_CMOS_AGC_TABLE_S    stAgcTbl;
    ISP_CMOS_NOISE_TABLE_S  stNoiseTbl;
    ISP_CMOS_DEMOSAIC_S     stDemosaic;
    ISP_CMOS_GAMMAFE_S      stGammafe;
    ISP_CMOS_GAMMA_S        stGamma;
    ISP_CMOS_SHADING_S      stShading;
} ISP_CMOS_DEFAULT_S;

typedef enum hiISP_CMOS_MODE_E
{
    ISP_CMOS_MODE_PIXEL_DETECT = 0,
    ISP_CMOS_MODE_WDR,
    ISP_CMOS_MODE_RESOLUTION,

    ISP_CMOS_MODE_BUTT,
} ISP_CMOS_MODE_E;

typedef struct hiISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
  HI_U32  u32MaxWidth;
  HI_U32  u32MaxHeight;

}ISP_CMOS_SENSOR_MAX_RESOLUTION;


typedef struct hiISP_CMOS_SENSOR_IMAGE_MODE_S
{
  HI_U16 u16Width;
  HI_U16 u16Height;
  HI_U16 u16Fps;
  
}ISP_CMOS_SENSOR_IMAGE_MODE;


typedef struct hiISP_SENSOR_EXP_FUNC_S
{
    HI_VOID(*pfn_cmos_sensor_init)(HI_VOID);
    HI_VOID(*pfn_cmos_sensor_global_init)(HI_VOID);
    /* the algs get data which is associated with sensor, except 3a */
    HI_U32(*pfn_cmos_get_isp_default)(ISP_CMOS_DEFAULT_S *pstDef);
    HI_U32(*pfn_cmos_get_isp_black_level)(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
    HI_S32(*pfn_cmos_get_sensor_max_resolution)(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution);

    /* the function of sensor set pixel detect */
    HI_VOID(*pfn_cmos_set_pixel_detect)(HI_BOOL bEnable);
    HI_VOID(*pfn_cmos_set_wdr_mode)(HI_U8 u8Mode);
    HI_VOID(*pfn_cmos_set_resolution)(HI_U32 u32ResolutionMode);
    HI_S32(*pfn_cmos_set_image_mode)(ISP_CMOS_SENSOR_IMAGE_MODE *pstSensorImageMode);
  
} ISP_SENSOR_EXP_FUNC_S;

typedef struct hiISP_SENSOR_REGISTER_S
{
    ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_COMM_SNS_H__ */

