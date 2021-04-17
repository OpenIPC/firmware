/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_comm_sns.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/7/6

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/7/6
  Author		: Lycai
  Modification	: creat
******************************************************************************/

#ifndef __XM_COMM_SNS_H__
#define __XM_COMM_SNS_H__

#include "xm_type.h"
#include "xm_comm_isp.h"


typedef struct xm_ISP_CMOS_AGC_TABLE_S
{
	XM_BOOL bValid;
	XM_U8  au8SharpenAltD[ISP_AUTO_STENGTH_NUM]; 		/* adjust image edge,different iso with different sharp strength */
	XM_U8  au8SharpenAltUd[ISP_AUTO_STENGTH_NUM]; 		/* adjust image texture, different iso with different strength */
	XM_U8  au8SharpenKd[ISP_AUTO_STENGTH_NUM]; 

	XM_U8  au8SnrThresh[ISP_AUTO_STENGTH_NUM];     	/* adjust 2Ddenoise strength, different iso with different strength */

	XM_U8  au83DTfStrength[ISP_AUTO_STENGTH_NUM]; 	/* adjust 3Ddenoise strength, different iso with different strength */
	XM_U8  au83DSfStrength[ISP_AUTO_STENGTH_NUM]; 

	XM_U8  au8DyDpc[ISP_AUTO_STENGTH_NUM]; 		/* adjust DynamicDpc strength, different iso with different strength */

	XM_U8  au8Saturation[ISP_AUTO_STENGTH_NUM];
	XM_U16 au16Blc[ISP_AUTO_STENGTH_NUM];			/* adjust blackLevel,different iso with different sharp strength */
	XM_U16  au16Ytrangamma[ISP_AUTO_STENGTH_NUM];		/*adjust  Y_Tran gamma,iso with different sharp strength*/
} ISP_CMOS_AGC_TABLE_S;
typedef struct xm_ISP_CMOS_NOISE_TABLE_S
{
    XM_BOOL bValid;
    
    XM_U8   au8NoiseProfileWeightLut[128];
    XM_U8   au8DemosaicWeightLut[128];
} ISP_CMOS_NOISE_TABLE_S;

typedef struct xm_ISP_CMOS_DEMOSAIC_S
{
    XM_BOOL bValid;
    
    XM_U8   u8VhSlope;
    XM_U8   u8AaSlope;
    XM_U8   u8VaSlope;
    XM_U8   u8UuSlope;
    XM_U8   u8SatSlope;
    XM_U8   u8AcSlope;
    XM_U16  u16VhThresh;
    XM_U16  u16AaThresh;
    XM_U16  u16VaThresh;
    XM_U16  u16UuThresh;
    XM_U16  u16SatThresh;
    XM_U16  u16AcThresh;
} ISP_CMOS_DEMOSAIC_S;

typedef struct xm_ISP_CMOS_DRC_S
{
    XM_U8   u8DrcBlack;
    XM_U8   u8DrcVs;         /* variance space */
    XM_U8   u8DrcVi;         /* variance intensity */
    XM_U8   u8DrcSm;         /* slope max */
    XM_U16  u16DrcWl;        /* white level */
} ISP_CMOS_DRC_S;

#define LUT_FACTOR (8)
#define GAMMA_FE_LUT_SIZE ((1<<LUT_FACTOR)+1)
typedef struct xm_ISP_CMOS_GAMMAFE_S
{
    XM_BOOL bValid;         /* wdr sensor should set */
    
    XM_U16  au16Gammafe[GAMMA_FE_LUT_SIZE];
} ISP_CMOS_GAMMAFE_S;

typedef struct xm_ISP_CMOS_DENOISE_S
{
    XM_U8   u8SinterThresh;     /* sinter threshold */
    
    XM_U8   u8NoiseProfile;     /* two different noise profile */ 
    XM_U16  u16Nr0;             /* nr0 for noise profile 2 */
    XM_U16  u16Nr1;             /* nr1 for noise profile 2 */
} ISP_CMOS_DENOISE_S;

typedef struct xm_ISP_CMOS_COMM_S
{
    XM_U8   u8Rggb;             /* rggb start sequence */    
    XM_U8   u8BalanceFe;
} ISP_CMOS_COMM_S;

#define CMOS_SHADING_TABLE_NODE_NUMBER_MAX (129)
typedef struct xm_ISP_CMOS_SHADING_S
{
    XM_BOOL bValid;
    
    XM_U16 u16RCenterX;
    XM_U16 u16RCenterY;
    XM_U16 u16GCenterX;
    XM_U16 u16GCenterY;
    XM_U16 u16BCenterX;
    XM_U16 u16BCenterY;

    XM_U16 au16RShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    XM_U16 au16GShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    XM_U16 au16BShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];

    XM_U16 u16ROffCenter;
    XM_U16 u16GOffCenter;
    XM_U16 u16BOffCenter;

    XM_U16 u16TblNodeNum;
} ISP_CMOS_SHADING_S;
#if (defined CHIPID_XM530)||(defined CHIPID_XM350)
#define GAMMA_NODE_NUMBER   97
#else
#define GAMMA_NODE_NUMBER   61
#endif
typedef struct xm_ISP_CMOS_GAMMA_S
{
    XM_BOOL bValid;
    
    XM_U16  au16Gamma[GAMMA_NODE_NUMBER];
} ISP_CMOS_GAMMA_S;

typedef struct xm_ISP_CMOS_DEFAULT_S
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

typedef enum xm_ISP_CMOS_MODE_E
{
    ISP_CMOS_MODE_PIXEL_DETECT = 0,
    ISP_CMOS_MODE_WDR,
    ISP_CMOS_MODE_RESOLUTION,

    ISP_CMOS_MODE_BUTT,
} ISP_CMOS_MODE_E;

typedef struct xm_ISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
  XM_U32  u32MaxWidth;
  XM_U32  u32MaxHeight;

}ISP_CMOS_SENSOR_MAX_RESOLUTION;


typedef struct xm_ISP_CMOS_SENSOR_IMAGE_MODE_S
{
  XM_U16 u16Width;
  XM_U16 u16Height;
  XM_U16 u16Fps;
  
}ISP_CMOS_SENSOR_IMAGE_MODE;


typedef struct xm_ISP_SENSOR_EXP_FUNC_S
{
	XM_VOID(*pfn_cmos_sensor_init)(XM_VOID);
	XM_VOID(*pfn_cmos_sensor_global_init)(XM_VOID);

	XM_S32(*pfn_cmos_get_isp_default)(ISP_CMOS_DEFAULT_S *pstDef);
	XM_S32(*pfn_cmos_get_sensor_max_resolution)(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution);

	/* the function of sensor set pixel detect */
	XM_VOID(*pfn_cmos_set_pixel_detect)(XM_BOOL bEnable);
	XM_VOID(*pfn_cmos_set_wdr_mode)(XM_U8 u8Mode);
	XM_VOID(*pfn_cmos_set_resolution)(XM_U32 u32ResolutionMode);
	XM_S32(*pfn_cmos_set_image_mode)(ISP_CMOS_SENSOR_IMAGE_MODE *pstSensorImageMode);
	XM_S32(*pfn_cmos_set_mirror_flip)(XM_U8 u8Mirror,XM_U8 u8Flip);
} ISP_SENSOR_EXP_FUNC_S;

typedef struct xm_ISP_SENSOR_REGISTER_S
{
    ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;

/****** ERR_shift  Noise_level*********/
#define SHIFTLEVEL_NUMBER   34

typedef struct xm_ISP_SENSOR_SHIFTLEVEL_S
{
	XM_U8 u8Shift;
	XM_U8 u8Level;
} ISP_SENSOR_SHIFTLEVEL_S;


typedef struct xm_ISP_CMOS_SNS_ATTR_S
{
	XM_U8 u8InputMode;	// 0:DVP 1:MIPI
	XM_U8 u8Rsv[7];
	XM_U32 u32Rsv[14];
} ISP_CMOS_SNS_ATTR_S;


#endif /*__XM_COMM_SNS_H__ */

