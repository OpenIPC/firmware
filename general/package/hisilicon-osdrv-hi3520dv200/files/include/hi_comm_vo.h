/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/03/18
  Description   :
  History       :
  1.Date        : 2009/03/18
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_VO_H__
#define __HI_COMM_VO_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_video.h"

#define VO_DEF_CHN_BUF_LEN      8
#define VO_DEF_DISP_BUF_LEN		5
#define VO_DEF_VIRT_BUF_LEN		3
#define VO_DEF_WBC_DEPTH_LEN    8

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum hiEN_VOU_ERR_CODE_E
{
    EN_ERR_VO_DEV_NOT_CONFIG	  = 0x40,
    EN_ERR_VO_DEV_NOT_ENABLE      = 0x41,
    EN_ERR_VO_DEV_HAS_ENABLED     = 0x42,
    EN_ERR_VO_DEV_HAS_BINDED      = 0x43,
    EN_ERR_VO_DEV_NOT_BINDED      = 0x44,

    ERR_VO_NOT_ENABLE             = 0x45,
    ERR_VO_NOT_DISABLE            = 0x46,
    ERR_VO_NOT_CONFIG             = 0x47,

    ERR_VO_CHN_NOT_DISABLE        = 0x48,
    ERR_VO_CHN_NOT_ENABLE         = 0x49,
    ERR_VO_CHN_NOT_CONFIG         = 0x4a,
    ERR_VO_CHN_NOT_ALLOC          = 0x4b,

    ERR_VO_CCD_INVALID_PAT        = 0x4c,
    ERR_VO_CCD_INVALID_POS        = 0x4d,

    ERR_VO_WAIT_TIMEOUT           = 0x4e,
    ERR_VO_INVALID_VFRAME         = 0x4f,
    ERR_VO_INVALID_RECT_PARA      = 0x50,
    ERR_VO_SETBEGIN_ALREADY       = 0x51,
    ERR_VO_SETBEGIN_NOTYET        = 0x52,
    ERR_VO_SETEND_ALREADY         = 0x53,
    ERR_VO_SETEND_NOTYET          = 0x54,

    ERR_VO_GRP_INVALID_ID         = 0x55,
    ERR_VO_GRP_NOT_CREATE         = 0x56,
    ERR_VO_GRP_HAS_CREATED        = 0x57,
    ERR_VO_GRP_NOT_DESTROY        = 0x58,
    ERR_VO_GRP_CHN_FULL           = 0x59,
    ERR_VO_GRP_CHN_EMPTY          = 0x5a,
    ERR_VO_GRP_CHN_NOT_EMPTY      = 0x5b,
    ERR_VO_GRP_INVALID_SYN_MODE   = 0x5c,
    ERR_VO_GRP_INVALID_BASE_PTS   = 0x5d,
    ERR_VO_GRP_NOT_START          = 0x5e,
    ERR_VO_GRP_NOT_STOP           = 0x5f,
    ERR_VO_GRP_INVALID_FRMRATE    = 0x60,
    ERR_VO_GRP_CHN_HAS_REG        = 0x61,
    ERR_VO_GRP_CHN_NOT_REG        = 0x62,
    ERR_VO_GRP_CHN_NOT_UNREG      = 0x63,
    ERR_VO_GRP_BASE_NOT_CFG       = 0x64,

    ERR_GFX_NOT_DISABLE           = 0x65,
    ERR_GFX_NOT_BIND              = 0x66,
    ERR_GFX_NOT_UNBIND            = 0x67,
    ERR_GFX_INVALID_ID            = 0x68,

    ERR_VO_WBC_NOT_DISABLE        = 0x69,
    ERR_VO_WBC_NOT_CONFIG         = 0x6a,

    ERR_VO_CHN_AREA_OVERLAP       = 0x6b,

    /* new added */
    ERR_VO_BUTT

}EN_VOU_ERR_CODE_E;

/* System define error code */
#define HI_ERR_VO_BUSY                  HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VO_NO_MEM                HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VO_NULL_PTR              HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VO_SYS_NOTREADY          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VO_INVALID_DEVID         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VO_INVALID_CHNID         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VO_ILLEGAL_PARAM         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VO_NOT_SUPPORT          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VO_NOT_PERMIT            HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

/* device relative error code */
#define HI_ERR_VO_DEV_NOT_CONFIG        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_CONFIG)
#define HI_ERR_VO_DEV_NOT_ENABLE        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_ENABLE)
#define HI_ERR_VO_DEV_HAS_ENABLED       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_ENABLED)
#define HI_ERR_VO_DEV_HAS_BINDED        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_BINDED)
#define HI_ERR_VO_DEV_NOT_BINDED        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_BINDED)

/* video relative error code */
#define HI_ERR_VO_VIDEO_NOT_ENABLE      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_ENABLE)
#define HI_ERR_VO_VIDEO_NOT_DISABLE     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_DISABLE)
#define HI_ERR_VO_VIDEO_NOT_CONFIG      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_CONFIG)

/*wbc error code*/
#define HI_ERR_VO_WBC_NOT_DISABLE     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_DISABLE)
#define HI_ERR_VO_WBC_NOT_CONFIG      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_CONFIG)


/* channel relative error code */
#define HI_ERR_VO_CHN_NOT_DISABLE       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_DISABLE)
#define HI_ERR_VO_CHN_NOT_ENABLE        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ENABLE)
#define HI_ERR_VO_CHN_NOT_CONFIG        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_CONFIG)
#define HI_ERR_VO_CHN_NOT_ALLOC         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ALLOC)
#define HI_ERR_VO_CHN_AREA_OVERLAP      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_AREA_OVERLAP)


/* cascade relatvie error code */
#define HI_ERR_VO_INVALID_PATTERN       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_PAT)
#define HI_ERR_VO_INVALID_POSITION      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_POS)

/* misc */
#define HI_ERR_VO_WAIT_TIMEOUT          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WAIT_TIMEOUT)
#define HI_ERR_VO_INVALID_VFRAME        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_VFRAME)
#define HI_ERR_VO_INVALID_RECT_PARA     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_RECT_PARA)
#define HI_ERR_VO_SETBEGIN_ALREADY      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_ALREADY)
#define HI_ERR_VO_SETBEGIN_NOTYET       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_NOTYET)
#define HI_ERR_VO_SETEND_ALREADY        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_ALREADY)
#define HI_ERR_VO_SETEND_NOTYET         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_NOTYET)

/* sync group relative error code */
#define HI_ERR_VO_GRP_INVALID_ID        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_ID)
#define HI_ERR_VO_GRP_NOT_CREATE        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_CREATE)
#define HI_ERR_VO_GRP_HAS_CREATED       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_HAS_CREATED)
#define HI_ERR_VO_GRP_NOT_DESTROY       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_DESTROY)
#define HI_ERR_VO_GRP_CHN_FULL          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_FULL)
#define HI_ERR_VO_GRP_CHN_EMPTY         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_EMPTY)
#define HI_ERR_VO_GRP_CHN_NOT_EMPTY     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_EMPTY)
#define HI_ERR_VO_GRP_INVALID_SYN_MODE  HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_SYN_MODE)
#define HI_ERR_VO_GRP_INVALID_BASE_PTS  HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_BASE_PTS)
#define HI_ERR_VO_GRP_NOT_START         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_START)
#define HI_ERR_VO_GRP_NOT_STOP          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_STOP)
#define HI_ERR_VO_GRP_INVALID_FRMRATE   HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_FRMRATE)
#define HI_ERR_VO_GRP_CHN_HAS_REG       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_HAS_REG)
#define HI_ERR_VO_GRP_CHN_NOT_REG       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_REG)
#define HI_ERR_VO_GRP_CHN_NOT_UNREG     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_UNREG)
#define HI_ERR_VO_GRP_BASE_NOT_CFG      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_BASE_NOT_CFG)


/* graphics relative error code */
#define HI_ERR_VO_GFX_NOT_DISABLE     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_DISABLE)
#define HI_ERR_VO_GFX_NOT_BIND        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_BIND)
#define HI_ERR_VO_GFX_NOT_UNBIND      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_UNBIND)
#define HI_ERR_VO_GFX_INVALID_ID      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_INVALID_ID)

/* vo inteface type */
#define VO_INTF_CVBS     (0x01L<<0)
#define VO_INTF_YPBPR    (0x01L<<1)
#define VO_INTF_VGA      (0x01L<<2)
#define VO_INTF_BT656    (0x01L<<3)
#define VO_INTF_BT1120   (0x01L<<4)
#define VO_INTF_HDMI     (0x01L<<5)
#define VO_INTF_LCD      (0x01L<<6)
#define VO_INTF_BT656_H  (0x01L<<7)
#define VO_INTF_BT656_L  (0x01L<<8)


/* WBC channel id*/
#define VO_WBC_CHN_ID    (VO_MAX_CHN_NUM + 1)

#define VO_DEFAULT_CHN      -1          /* use vo buffer as pip buffer */

/*****************************************************************************
 * 3520 ADDed
 *****************************************************************************/
typedef HI_S32 VO_INTF_TYPE_E;

typedef HI_S32 VO_WBC_CHN;

typedef enum hiVO_INTF_SYNC_E
{
    VO_OUTPUT_PAL = 0,
    VO_OUTPUT_NTSC,
    
    VO_OUTPUT_1080P24,
    VO_OUTPUT_1080P25,
    VO_OUTPUT_1080P30,
    
    VO_OUTPUT_720P50, 
    VO_OUTPUT_720P60,   
    VO_OUTPUT_1080I50,
    VO_OUTPUT_1080I60,    
    VO_OUTPUT_1080P50,
    VO_OUTPUT_1080P60,            

    VO_OUTPUT_576P50,
    VO_OUTPUT_480P60,

    VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
    VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/
    VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_1920x2160_30,          /* 1920x2160_30 */
    VO_OUTPUT_2560x1440_30,          /* 2560x1440_30 */
    VO_OUTPUT_USER,
    VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;

typedef enum hiVO_SCREEN_HFILTER_E
{
	VO_SCREEN_HFILTER_DEF	= 0,
	VO_SCREEN_HFILTER_8M,
	VO_SCREEN_HFILTER_6M,
	VO_SCREEN_HFILTER_5M,
	VO_SCREEN_HFILTER_4M,
	VO_SCREEN_HFILTER_3M,
	VO_SCREEN_HFILTER_2M,
	VO_SCREEN_HFILTER_BUTT
    
} VO_SCREEN_HFILTER_E;

typedef enum hiVO_SCREEN_VFILTER_E
{
	VO_SCREEN_VFILTER_DEF	= 0,
	VO_SCREEN_VFILTER_8M,
	VO_SCREEN_VFILTER_6M,
	VO_SCREEN_VFILTER_5M,
	VO_SCREEN_VFILTER_4M,
	VO_SCREEN_VFILTER_3M,
	VO_SCREEN_VFILTER_2M,	
	VO_SCREEN_VFILTER_BUTT
    
} VO_SCREEN_VFILTER_E;

typedef enum hiVO_DISPLAY_FIELD_E
{
  VO_FIELD_TOP,                 /* top field*/
  VO_FIELD_BOTTOM,              /* bottom field*/
  VO_FIELD_BOTH,                /* top and bottom field*/
  VO_FIELD_BUTT
} VO_DISPLAY_FIELD_E;

typedef enum hiVOU_LAYER_DDR_E
{
    VOU_LAYER_DDR0 = 0,
    VOU_LAYER_DDR1 = 1,
    VOU_LAYER_DDR_BUTT
}VOU_LAYER_DDR_E;

typedef enum hiVOU_ZOOM_IN_E
{
    VOU_ZOOM_IN_RECT = 0,       /* zoom in by rect */
    VOU_ZOOM_IN_RATIO,          /* zoom in by ratio */
    VOU_ZOOM_IN_BUTT
} VOU_ZOOM_IN_E;

typedef enum hiVO_CSC_MATRIX_E
{
    VO_CSC_MATRIX_IDENTITY = 0,
    
    VO_CSC_MATRIX_BT601_TO_BT709,
    VO_CSC_MATRIX_BT709_TO_BT601,

    VO_CSC_MATRIX_BT601_TO_RGB_PC,
    VO_CSC_MATRIX_BT709_TO_RGB_PC,

    VO_CSC_MATRIX_RGB_TO_BT601_PC,
    VO_CSC_MATRIX_RGB_TO_BT709_PC,

    VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;

typedef struct hiVO_CHN_ATTR_S
{
    HI_U32  u32Priority;                /* video out overlay pri */
    RECT_S  stRect;                     /* rect of video out chn */
    HI_BOOL bDeflicker;                 /* deflicker or not */
}VO_CHN_ATTR_S;

typedef struct hiVO_QUERY_STATUS_S
{
    HI_U32 u32ChnBufUsed;       /* channel buffer that been occupied */
} VO_QUERY_STATUS_S;

typedef struct hiVO_SRC_ATTR_S
{
    HI_BOOL bInterlaced;        /* interlaced source */
} VO_SRC_ATTR_S;

typedef struct hiVO_SCALE_FILTER_S
{
    VO_SCREEN_HFILTER_E enHFilter;
    VO_SCREEN_VFILTER_E enVFilter;
    
} VO_SCREEN_FILTER_S;

typedef struct tagVO_SYNC_INFO_S
{
    HI_BOOL  bSynm;     /* sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    HI_BOOL  bIop;      /* interlaced or progressive display(0:i; 1:p) */
    HI_U8    u8Intfb;   /* interlace bit width while output */

    HI_U16   u16Vact ;  /* vertical active area */
    HI_U16   u16Vbb;    /* vertical back blank porch */
    HI_U16   u16Vfb;    /* vertical front blank porch */

    HI_U16   u16Hact;   /* herizontal active area */
    HI_U16   u16Hbb;    /* herizontal back blank porch */
    HI_U16   u16Hfb;    /* herizontal front blank porch */
    HI_U16   u16Hmid;   /* bottom herizontal active area */

    HI_U16   u16Bvact;  /* bottom vertical active area */
    HI_U16   u16Bvbb;   /* bottom vertical back blank porch */
    HI_U16   u16Bvfb;   /* bottom vertical front blank porch */

    HI_U16   u16Hpw;    /* horizontal pulse width */
    HI_U16   u16Vpw;    /* vertical pulse width */

    HI_BOOL  bIdv;      /* inverse data valid of output */
    HI_BOOL  bIhs;      /* inverse horizontal synch signal */
    HI_BOOL  bIvs;      /* inverse vertical synch signal */

} VO_SYNC_INFO_S;

typedef struct hiVO_PUB_ATTR_S
{
    HI_U32                   u32BgColor;          /* Background color of a device, in RGB format. */
    VO_INTF_TYPE_E           enIntfType;          /* Type of a VO interface */
    VO_INTF_SYNC_E           enIntfSync;          /* Type of a VO interface timing */
    VO_SYNC_INFO_S           stSyncInfo;          /* Information about VO interface timings */
    HI_BOOL                  bDoubleFrame;        /* Whether to double frames */  
} VO_PUB_ATTR_S;

typedef enum hiVO_WBC_DATASOURCE_E
{
    VO_WBC_DATASOURCE_MIXER = 0,            /* WBC data source is a mixer of layer and graphics*/
    VO_WBC_DATASOURCE_VIDEO,                /* WBC data source only from layer*/
    VO_WBC_DATASOURCE_BUTT,
} VO_WBC_DATASOURCE_E;


typedef struct hiVO_WBC_ATTR_S
{
    SIZE_S              stTargetSize;        /* WBC Zoom target size */ 
    PIXEL_FORMAT_E      enPixelFormat;       /* the pixel format of WBC output */
    HI_U32              u32FrameRate;        /* frame rate control */
    VO_WBC_DATASOURCE_E enDataSource;        /* WBC data source*/
} VO_WBC_ATTR_S;

typedef enum hiVO_WBC_MODE_E
{
    VO_WBC_MODE_NOMAL = 0,                  /* In this mode, wbc will capture frames according to dev frame rate
                                                    and wbc frame rate */
    VO_WBC_MODE_DROP_REPEAT,                /* In this mode, wbc will drop dev repeat frame, and capture the real frame
                                                    according to video layer's display rate and wbc frame rate */
    VO_WBC_MODE_PROG_TO_INTL,               /* In this mode, wbc will drop dev repeat frame which repeats more than 3 times,
                                                     and change two progressive frames to one interlace frame */
    
    VO_WBC_MODE_BUTT,
} VO_WBC_MODE_E;

typedef enum hiVO_CAS_MODE_E
{
    VO_CAS_MODE_SINGLE = 0,
    VO_CAS_MODE_DUAL,
    VO_CAS_MODE_BUTT,
} VO_CAS_MODE_E;

typedef enum hiVO_CAS_RGN_E
{
    VO_CAS_64_RGN = 0,
    VO_CAS_32_RGN,
    VO_CAS_RGN_BUTT,
} VO_CAS_RGN_E;

typedef struct hiVO_CAS_ATTR_S
{
    HI_BOOL         bSlave;
    VO_CAS_RGN_E    enCasRgn;
    VO_CAS_MODE_E   enCasMode;
} VO_CAS_ATTR_S;

typedef struct hiVO_WBC_ATTR_S_1
{
    VO_DEV              VoSourceDev;         /* WBC source is from which Dev */
    SIZE_S              stTargetSize;        /* WBC Zoom target size */ 
    PIXEL_FORMAT_E     enPixelFormat;       /* the pixel format of WBC output */
} VO_WBC_ATTR_S_1;


typedef struct hiVO_WBC_ATTR_S_2
{
    VO_CHN              VoWbcChn;            /* Wbc Channel id */
    SIZE_S              stTargetSize;        /* WBC Zoom target size */ 
    PIXEL_FORMAT_E      enPixelFormat;       /* the pixel format of WBC output */
} VO_WBC_ATTR_S_2;

typedef struct hiVO_WBC_ATTR_S_3
{
    SIZE_S              stTargetSize;        /* WBC Zoom target size */
    PIXEL_FORMAT_E     enPixelFormat;       /* the pixel format of WBC output */
} VO_WBC_ATTR_TEMP_S_3;

typedef struct hiVO_VIDEO_LAYER_ATTR_S
{
    RECT_S stDispRect;                  /* Display resolution */
    SIZE_S stImageSize;                 /* Canvas size of the video layer */
    HI_U32 u32DispFrmRt;                /* Display frame rate */
    PIXEL_FORMAT_E enPixFormat;         /* Pixel format of the video layer */
} VO_VIDEO_LAYER_ATTR_S;

typedef struct hiVO_ZOOM_RATIO_S
{
    HI_U32 u32XRatio;
    HI_U32 u32YRatio;
    HI_U32 u32WRatio;
    HI_U32 u32HRatio;    
} VO_ZOOM_RATIO_S;

typedef struct hiVO_ZOOM_ATTR_S
{
    VOU_ZOOM_IN_E   enZoomType;         /* choose the type of zoom in */
    union
    {
        RECT_S          stZoomRect;     /* zoom in by rect */
        VO_ZOOM_RATIO_S stZoomRatio;    /* zoom in by ratio */
    };
} VO_ZOOM_ATTR_S;

typedef struct hiVO_CSC_S
{
    VO_CSC_MATRIX_E enCscMatrix;
    HI_U32 u32Luma;                     /* luminance:   0 ~ 100 */
    HI_U32 u32Contrast;                 /* contrast :   0 ~ 100 */
    HI_U32 u32Hue;                      /* hue      :   0 ~ 100 */
    HI_U32 u32Satuature;                /* satuature:   0 ~ 100 */
} VO_CSC_S;

typedef struct hiVO_VGA_PARAM_S
{
    VO_CSC_MATRIX_E enCscMatrix;
    HI_U32 u32Luma;                     /* luminance:   0 ~ 100 */
    HI_U32 u32Contrast;                 /* contrast :   0 ~ 100 */
    HI_U32 u32Hue;                      /* hue      :   0 ~ 100 */
    HI_U32 u32Satuature;                /* satuature:   0 ~ 100 */
    HI_U32 u32Gain;                     /* current gain of VGA signals. [0, 64). */
} VO_VGA_PARAM_S;

typedef struct hiVO_USR_SEND_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S  stVFrame;
    HI_U32              u32MilliSec;
} VO_USR_SEND_TIMEOUT_S;

typedef enum hiVOU_GFX_BIND_LAYER_E
{
    GRAPHICS_LAYER_G4 = 0,
    GRAPHICS_LAYER_HC0,	
    GRAPHICS_LAYER_HC1,	
    GRAPHICS_LAYER_BUTT
}VOU_GFX_BIND_LAYER_E;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMM_VO_H__ */

