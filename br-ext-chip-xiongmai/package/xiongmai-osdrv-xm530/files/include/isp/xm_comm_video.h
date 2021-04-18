/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_common_video.h
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
#ifndef __XM_COMM_VIDEO_H__
#define __XM_COMM_VIDEO_H__

#include "xm_type.h"
#include "xm_common.h"



typedef enum xm_PIC_SIZE_E
{
    PIC_QCIF = 0,
    PIC_CIF,    
    PIC_2CIF,    
    PIC_HD1, 
    PIC_D1,
    PIC_960H,
    
    PIC_QVGA,    /* 320 * 240 */
    PIC_VGA,     /* 640 * 480 */    
    PIC_XGA,     /* 1024 * 768 */   
    PIC_SXGA,    /* 1400 * 1050 */    
    PIC_UXGA,    /* 1600 * 1200 */    
    PIC_QXGA,    /* 2048 * 1536 */

    PIC_WVGA,    /* 854 * 480 */
    PIC_WSXGA,   /* 1680 * 1050 */      
    PIC_WUXGA,   /* 1920 * 1200 */
    PIC_WQXGA,   /* 2560 * 1600 */
    
    PIC_HD720,   /* 1280 * 720 */
    PIC_HD1080,  /* 1920 * 1080 */
    
    PIC_BUTT
}PIC_SIZE_E;

typedef enum xm_VIDEO_NORM_E
{
    VIDEO_ENCODING_MODE_PAL=0,
    VIDEO_ENCODING_MODE_NTSC,
    VIDEO_ENCODING_MODE_AUTO,
    VIDEO_ENCODING_MODE_BUTT
} VIDEO_NORM_E;

typedef enum xm_VIDEO_CONTROL_MODE_E
{
    VIDEO_CONTROL_MODE_SLAVER=0,
    VIDEO_CONTROL_MODE_MASTER,
    VIDEO_CONTROL_MODE_BUTT
}VIDEO_CONTROL_MODE_E;


/* we ONLY define picture format used, all unused will be deleted!*/
typedef enum xm_PIXEL_FORMAT_E
{   
    PIXEL_FORMAT_RGB_1BPP = 0,
    PIXEL_FORMAT_RGB_2BPP,
    PIXEL_FORMAT_RGB_4BPP,
    PIXEL_FORMAT_RGB_8BPP,
    PIXEL_FORMAT_RGB_444,
    PIXEL_FORMAT_RGB_4444,
    PIXEL_FORMAT_RGB_555,
    PIXEL_FORMAT_RGB_565,
    PIXEL_FORMAT_RGB_1555,

    /*  9 reserved */
    PIXEL_FORMAT_RGB_888,
    PIXEL_FORMAT_RGB_8888,
    PIXEL_FORMAT_RGB_PLANAR_888,
    PIXEL_FORMAT_RGB_BAYER,

    PIXEL_FORMAT_YUV_A422,
    PIXEL_FORMAT_YUV_A444,

    PIXEL_FORMAT_YUV_PLANAR_422,
    PIXEL_FORMAT_YUV_PLANAR_420,
    PIXEL_FORMAT_YUV_PLANAR_444,

    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    PIXEL_FORMAT_UYVY_PACKAGE_422,
    PIXEL_FORMAT_YUYV_PACKAGE_422,
    PIXEL_FORMAT_VYUY_PACKAGE_422,
    PIXEL_FORMAT_YCbCr_PLANAR,

    PIXEL_FORMAT_RGB_422,
    PIXEL_FORMAT_RGB_420,
    
    PIXEL_FORMAT_BUTT   
} PIXEL_FORMAT_E;

typedef struct xm_VIDEO_VBI_INFO_S
{
    XM_U32 au32Data[VIU_MAX_VBI_LEN];
    XM_U32 u32Len;
}VIDEO_VBI_INFO_S;

typedef enum xm_VIDEO_FIELD_E
{
    VIDEO_FIELD_TOP         = 0x01,    /* even field */
    VIDEO_FIELD_BOTTOM      = 0x02,    /* odd field */
    VIDEO_FIELD_INTERLACED  = 0x03,    /* two interlaced fields */
    VIDEO_FIELD_FRAME       = 0x04,    /* frame */

    VIDEO_FIELD_BUTT
} VIDEO_FIELD_E;

typedef struct xm_VIDEO_FRAME_S
{    
    XM_U32          u32Width;
    XM_U32          u32Height;    
    VIDEO_FIELD_E   u32Field;
    PIXEL_FORMAT_E  enPixelFormat;

    XM_U32          u32PhyAddr[3];
    XM_VOID         *pVirAddr[3];
    XM_U32          u32Stride[3];
	/*********************************************************
	u32Stride:
	--- u32Stride[0]: YSize
	--- u32Stride[2]: ErrSize(bit0~bit15: SizeH  bit16~bit31:SizeV)
	*********************************************************/

    XM_U16          u16OffsetTop;       /* top offset of show area */
    XM_U16          u16OffsetBottom;   /* bottom offset of show area */
    XM_U16          u16OffsetLeft;       /* left offset of show area */
    XM_U16          u16OffsetRight;    /* right offset of show area */

    XM_U64          u64pts;
    XM_U32          u32TimeRef;

    XM_U32          u32PrivateData;
    //VIDEO_VBI_INFO_S astVbiInfo[VIU_MAX_VBI_NUM];
}VIDEO_FRAME_S;

typedef struct xm_VIDEO_FRAME_INFO_S
{
    VIDEO_FRAME_S stVFrame;
    XM_U32 u32PoolId;
} VIDEO_FRAME_INFO_S;


typedef struct xmBITMAP_S
{
	XM_U32 u32Handle;
	XM_U32 u32Width;
	XM_U32 u32Height;
	XM_U32 u32Format;
	XM_VOID *pData;
} BITMAP_S;



typedef struct XM_VPP_CFG_S
{
    XM_BOOL bVppEn;
    
    XM_BOOL bIeEn;
    XM_BOOL bDnEn;
    XM_BOOL bSpEn;
    XM_BOOL bIencEn;
    
    XM_S32 s32IeSth;       /* IE Strength [0,10] */
    XM_S32 s32SpSth;       /* SP Strength [-4,5] */
    XM_S32 s32DnSfCosSth;  /* coarse DN sf Strength [0,3] */
    XM_S32 s32DnSfIncSth;  /* Inching of DN sf Strength [0,255] */
    XM_S32 s32DnTfSth;     /* DN tf Strength [0,4] */
    
} VPP_CFG_S;

typedef struct xm_SCALE_CFG_S
{
    XM_BOOL bScaleEn;

    XM_S32  s32DstWidth;    /* the dest width after scale */
    XM_S32  s32DstHeight;   /* the dest height after scale */
} SCALE_CFG_S;

/* Ie */
typedef struct XM_VPP_IE_EX_S
{
    XM_U32  u32RefRange;//
    XM_U32  u32IeStrength;//  
    XM_U32  u32Black;
    XM_U32  u32White;
} VPP_IE_EX_S;

/*dn*/
typedef struct
{
  XM_U16  saMask[2];
  XM_U16  weight[2];

} tVppCalcWnd;

typedef struct
{
    XM_U8   wWndX, hWndX; 
    XM_U8   wWndY, hWndY; 
    XM_U8   wWndC, hWndC; 
    XM_U8   wWndD, hWndD; //

    XM_U16  sfyMask[2];
    XM_U8   sfyRange[2]; //
    XM_U8   sfyStrength; //
    XM_U8   sfyLimitedFlag; //

    XM_U8   MADpExDirectSub : 1; 
    XM_U8   MADpExMask : 4; 
    XM_U8   _reserved_ : 3; 

    XM_U8   sfcStrength;
    XM_U8   sfyMADpThresh,   sfyMADpRate;/**/
    XM_U16  sfyMSEiThresh[8];

    XM_U8   rsfyThresh, rsfyRate, rsfyStrength, tfyStrength;
    XM_U8   rsfcThresh, rsfcRate, rsfcStrength, tfcStrength;/**/

    XM_U8   tfyMADpThresh,  tfyMADpRate;
    XM_U8   tfySNRpThresh,  tfySNRpRate;

    XM_U16  tfyMSEiThresh[8];
    XM_U16  tfyMSEpThresh[8];

    XM_U8   tfyMaxStrength;//
    XM_U8   tfcMaxStrength;/**/

    XM_U8   tfcMADpThresh,  tfcMADpRate;
    XM_U16  tfcMSEiThresh[8];

    XM_U16  diyMSEiThresh[8];

    XM_U8   tfyLimit;
    XM_U8   noiseMADpThresh;
    XM_U16  noiseMSDpThresh;

    XM_U8   log2hStepMADp, log2hStepMSEi;

    XM_U8   histMinMADp, histMaxMADp;
    XM_U16  histMinMSEi, histMaxMSEi;

    /*---------------------------------------------*/
    tVppCalcWnd  yWnd[2];
    tVppCalcWnd  cWnd[2];
} tVppDnCfg;
typedef  tVppDnCfg VPP_DN_EX_S;

/* Sp */
typedef struct
{
    XM_U8  strength;//
    XM_U8  limit;//
    XM_U8  black;
    XM_U8  white;
} tVppSharpenCfg;
typedef  tVppSharpenCfg VPP_SP_EX_S;


typedef struct xm_VPP_CFG_EX_S
{
    VPP_IE_EX_S     stIE_Ex;
    VPP_DN_EX_S     stDN_Ex;
    VPP_SP_EX_S     stSP_Ex;
} VPP_CFG_EX_S;


/* VI Mix-Capture info. */
typedef struct xm_VI_MIXCAP_STAT_S
{
    XM_BOOL bMixCapMode;    /* In mix-capture mode or not. */
    XM_BOOL bHasDownScale;  /* VI Frame is downscaled or not. */
} VI_MIXCAP_STAT_S;

/* VI output frame info. */
typedef struct xm_VI_FRAME_INFO_S
{
    VI_MIXCAP_STAT_S stMixCapState; /* VI Mix-Capture info. */
    VIDEO_FRAME_INFO_S stViFrmInfo; /* Video frame info. */
    XM_BOOL bFlashed;               /* Flashed Video frame or not. */
}VI_FRAME_INFO_S;

typedef enum xm_LDC_VIEW_TYPE_E
{
    LDC_VIEW_TYPE_ALL = 0,  /* View scale all but x and y independtly, this will keep both x and y axis ,but corner maybe lost*/
    LDC_VIEW_TYPE_CROP,     /* Not use view scale, this will lost some side and corner */
        
    LDC_VIEW_TYPE_BUTT,
} LDC_VIEW_TYPE_E;

typedef struct xm_LDC_ATTR_S 
{
    LDC_VIEW_TYPE_E enViewType;
    
    XM_S32 s32CenterXOffset;        /* Horizontal offset of the image distortion center relative to image center. [-28,28]. */
    XM_S32 s32CenterYOffset;        /* Vertical offset of the image distortion center relative to image center. [-14,14]. */
    XM_S32 s32Ratio;                /* Distortion ratio. [0, 511]. */
} LDC_ATTR_S;



#endif /* _XM_COMM_VIDEO_H_ */ 
