
/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vpss.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 
  Last Modified :
  Description   : common struct definition for vpss
  Function List :
  History       :
  1.Date        : 2011/6/16
    Author      : h105449/l183122/w63830
    Modification: Create
  
 
******************************************************************************/

#ifndef __HI_COMM_VPSS_H__
#define __HI_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#define HI_ERR_VPSS_NULL_PTR        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VPSS_NOTREADY        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VPSS_INVALID_DEVID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VPSS_INVALID_CHNID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VPSS_EXIST           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_VPSS_UNEXIST         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VPSS_NOT_SUPPORT     HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VPSS_NOT_PERM        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VPSS_NOMEM           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VPSS_NOBUF           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VPSS_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VPSS_BUSY            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
    
typedef   HI_S32 VPSS_GRP;
typedef   HI_S32 VPSS_CHN;

/*Define 4 video frame*/
typedef enum HI_VPSS_FRAME_WORK_E
{
    VPSS_FRAME_WORK_LEFT     =  0,
    VPSS_FRAME_WORK_RIGHT    =  1,
    VPSS_FRAME_WORK_BOTTOM   =  2,
    VPSS_FRAME_WORK_TOP      =  3,
    VPSS_FRAME_WORK_BUTT
}VPSS_FRAME_WORK_E;

/*Define de-interlace mode*/    
typedef enum  HI_VPSS_DIE_MODE_E
{
    VPSS_DIE_MODE_AUTO      = 0,
    VPSS_DIE_MODE_NODIE     = 1,
    VPSS_DIE_MODE_DIE       = 2,
    VPSS_DIE_MODE_BUTT
}VPSS_DIE_MODE_E;

/*Define size of space filter window */
typedef enum  HI_VPSS_SF_WINDOW_E
{
    VPSS_SF_WINDOW_3X3      = 0,
    VPSS_SF_WINDOW_5X5      = 1,
    VPSS_SF_WINDOW_7X7      = 2,
    VPSS_SF_WINDOW_9X9      = 3,
    VPSS_SF_WINDOW_BUTT
}VPSS_SF_WINDOW_E;

/*Define display mode*/
typedef enum  HI_VPSS_DISPLAY_MODE_E
{
    VPSS_DISPLAY_MODE_TV    = 0,
    VPSS_DISPLAY_MODE_PC    = 1,
    VPSS_DISPLAY_MODE
}VPSS_DISPLAY_MODE_E;

/*Define attributes of video frame*/
typedef struct HI_VPSS_FRAME_S
{
    HI_U32  u32Width[VPSS_FRAME_WORK_BUTT]; /*Width of 4 frames,0:L,1:R,2:B,3:T*/
    HI_U32  u32Color[VPSS_FRAME_WORK_BUTT]; /*Color of 4 frames,R/G/B*/
}VPSS_FRAME_S;

/*Define attributes of vpss channel*/
typedef struct HI_VPSS_CHN_ATTR_S
{
    HI_BOOL bSpEn;                 
    HI_BOOL bFrameEn;
    VPSS_FRAME_S  stFrame;     
}VPSS_CHN_ATTR_S;

/*Defined detailed image quality debug param*/
typedef struct HI_VPSS_IMG_QUALITY_PARAM_S
{
    HI_U32 u32SfStrength0;
    HI_U32 u32SfStrength1;
    HI_U32 u32SfStrength2;
    HI_U32 u32TfStrength;
    HI_U32 u32SfPostFlag;
    HI_U32 u32TfyProfile;
    HI_U32 u32CStrength;
    HI_U32 u32CMADSlope;
    HI_U32 u32CMADThresh;
    HI_U32 u32Reserved[6];
}VPSS_IMG_QUALITY_PARAM_S;

/*Defined private image quality debug config struct*/
typedef struct HI_VPSS_IMG_QUALITY_CFG_S
{
    HI_BOOL bEnable;
    VPSS_IMG_QUALITY_PARAM_S  stImageQualityParam;
}VPSS_IMG_QUALITY_CFG_S;

/*Define detailed params for group image process*/
typedef struct HI_VPSS_GRP_PARAM_S
{
    HI_U32 u32Luminance;    /*Luminance*/
    HI_U32 u32Contrast;     /*Contrast*/
    HI_U32 u32DarkEnhance;  /*Dark region enhancement*/
    HI_U32 u32BrightEnhance;/*Light region enhancement*/
    HI_U32 u32IeStrength;   /*IE strength*/
    HI_U32 u32IeSharp;      /*IE sharpness*/
    HI_U32 u32SfStrength;   /*Strength of space filter*/
    HI_U32 u32TfStrength;   /*Strength of space filter*/
    HI_U32 u32MotionThresh; /*Motion judgment threshold*/
    HI_U32 u32DiStrength;   /*de-interlace strength*/
    HI_U32 u32ChromaRange;  /*Chrominance amplitude*/
    HI_U32 u32NrWforTsr;    /*Time filtering weight*/
    VPSS_SF_WINDOW_E enSfWindow;  /*Size of the space filter window*/
    VPSS_DISPLAY_MODE_E enDisMode;/*Display mode*/
}VPSS_GRP_PARAM_S;

/* Define image feild select mode */
typedef enum hiVPSS_CAPSEL_E
{
    VPSS_CAPSEL_BOTH = 0,             /* top and bottom field */
    VPSS_CAPSEL_TOP,                  /* top field */
    VPSS_CAPSEL_BOTTOM,               /* bottom field */
    
    VPSS_CAPSEL_BUTT
} VPSS_CAPSEL_E;
/*Define coordinate mode*/
typedef enum HI_VPSS_CROP_COORDINATE_E   
{
    VPSS_CROP_RITIO_COOR = 0,   /*Ratio coordinate*/
    VPSS_CROP_ABS_COOR          /*Absolute coordinate*/
}VPSS_CROP_COORDINATE_E;

/*Define attributes of CLIP function*/
typedef struct HI_VPSS_CROP_INFO_S
{
    HI_BOOL bEnable;        /*CROP enable*/
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /*Coordinate mode of the crop start point*/
    RECT_S  stCropRect;     /*CROP rectangular*/
    VPSS_CAPSEL_E   enCapSel;                   /*field select*/
}VPSS_CROP_INFO_S;

/*Define attributes of vpss GROUP*/
typedef struct HI_VPSS_GRP_ATTR_S
{
    /*statistic attributes*/
    HI_U32  u32MaxW;                     
    HI_U32  u32MaxH;
    PIXEL_FORMAT_E enPixFmt; 
    
    /*dynamic attributes*/
    HI_BOOL bDrEn;
    HI_BOOL bDbEn;
    HI_BOOL bIeEn;
    HI_BOOL bNrEn;
    HI_BOOL bHistEn;
    VPSS_DIE_MODE_E enDieMode;
}VPSS_GRP_ATTR_S;

/*Define vpss channel's work mode*/
typedef enum HI_VPSS_CHN_MODE_E   
{
    VPSS_CHN_MODE_AUTO = 0, /*Auto mode*/
    VPSS_CHN_MODE_USER  /*User mode*/
}VPSS_CHN_MODE_E;

/*Define attributes of vpss channel's work mode*/
typedef struct HI_VPSS_CHN_MODE_S
{
    VPSS_CHN_MODE_E  enChnMode;   /*Vpss channel's work mode*/
    HI_U32 u32Width;              /*Width of target image*/
    HI_U32 u32Height;             /*Height of target image*/
    HI_BOOL bDouble;              /*Field-frame transfer£¬only valid for VPSS_PRE0_CHN*/
    PIXEL_FORMAT_E  enPixelFormat;/*Pixel format of target image*/
}VPSS_CHN_MODE_S;

typedef struct HI_VPSS_FRAME_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S stVideoFrame;
    HI_U32 u32MilliSec;
}VPSS_FRAME_TIMEOUT_S;

typedef struct HI_VPSS_GET_GRP_FRAME_S
{
    HI_U32 u32FrameIndex;
    VIDEO_FRAME_INFO_S *pstVideoFrame;
}VPSS_GET_GRP_FRAME_S;

/*********************************************************************************************/

/*Define detailed NR params for channel image process*/
typedef struct
{
    HI_U32 u32SfStrength;   /*Strength of space filter*/
    HI_U32 u32TfStrength;   /*Strength of space filter*/
    HI_U32 u32MotionThresh; /*Motion judgment threshold*/
    HI_U32 u32ChromaRange;  /*Chrominance amplitude*/
    HI_U32 u32NrWforTsr;    /*Time filtering weight*/   
}VPSS_CHN_NR_PARAM_S;

/*Define detailed SP params for channel image process*/
typedef struct
{
  HI_U32 u32LumaGain;           /*Luma gain of sharp function*/   
}VPSS_CHN_SP_PARAM_S;

/*Define detailed params for channel image process*/
typedef struct
{
  VPSS_CHN_SP_PARAM_S  stVpssChnSpParam;           /*Luma gain of sharp function*/  
  VPSS_CHN_NR_PARAM_S  stVpssChnNrParam;
} VPSS_CHN_PARAM_S;

/*Define vpss prescale info*/
typedef struct
{
    HI_BOOL bPreScale;            /*prescale enable*/
    VPSS_CAPSEL_E     enCapSel;   /*feild select*/
    SIZE_S          stDestSize;   /*destination size*/     
}VPSS_PRESCALE_INFO_S;

/*Define vpss filter info*/
typedef struct
{
    HI_BOOL bSizer;
    SIZE_S  stSize;
}VPSS_SIZER_INFO_S;

/*Define vpss frame control info*/
typedef struct 
{
    HI_S32  s32SrcFrmRate;          /* Input frame rate of a  group*/
    HI_S32  s32DstFrmRate;        /* Output frame rate of a channel group */
} VPSS_FRAME_RATE_S;

/*********************************************************************************************/

/*Define attributes of vpss extend channel*/
typedef struct HI_VPSS_EXT_CHN_ATTR_S
{
    VPSS_CHN        s32BindChn;             /*channel bind to*/
    HI_U32          u32Width;               /*Width of target image*/
    HI_U32          u32Height;              /*Height of target image*/
    HI_S32          s32SrcFrameRate;        /*Frame rate of source*/
    HI_S32          s32DstFrameRate;        /*Frame rate of extend chn input&output*/
    PIXEL_FORMAT_E  enPixelFormat;          /*Pixel format of target image*/
}VPSS_EXT_CHN_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_VPSS_H__ */


