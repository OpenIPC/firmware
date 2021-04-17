#ifndef __XM_COMM_VO_H__
#define __XM_COMM_VO_H__

#include "xm_type.h"
#include "xm_common.h"
#include "xm_comm_video.h"
#include "xm_errno.h"

#define VO_DEF_CHN_BUF_LEN      8
#define VO_DEF_DISP_BUF_LEN		5
#define VO_DEF_VIRT_BUF_LEN		3
#define VO_DEF_WBC_DEPTH_LEN    8

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum xmEN_VOU_ERR_CODE_E
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

    EN_ERR_INVALID_WBCID          = 0x6c,
    EN_ERR_INVALID_LAYERID        = 0x6d,
    EN_ERR_VO_VIDEO_HAS_BINDED    = 0x6e,
    EN_ERR_VO_VIDEO_NOT_BINDED    = 0x6f,
    ERR_VO_WBC_HAS_BIND           = 0x70,
    ERR_VO_WBC_HAS_CONFIG         = 0x71, 
    ERR_VO_WBC_NOT_BIND           = 0x72,

    /* new added */
    ERR_VO_BUTT

}EN_VOU_ERR_CODE_E;

#define XM_TRACE_VO(level, fmt...) XM_TRACE(level, XM_ID_VOU,##fmt)

/* System define error code */
#define XM_ERR_VO_BUSY                  XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define XM_ERR_VO_NO_MEM                XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define XM_ERR_VO_NULL_PTR              XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define XM_ERR_VO_SYS_NOTREADY          XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define XM_ERR_VO_INVALID_DEVID         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define XM_ERR_VO_INVALID_CHNID         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define XM_ERR_VO_ILLEGAL_PARAM         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define XM_ERR_VO_NOT_SUPPORT          XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define XM_ERR_VO_NOT_PERMIT            XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define XM_ERR_VO_INVALID_WBCID         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_WBCID)
#define XM_ERR_VO_INVALID_LAYERID        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_LAYERID)


/* device relative error code */
#define XM_ERR_VO_DEV_NOT_CONFIG        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_CONFIG)
#define XM_ERR_VO_DEV_NOT_ENABLE        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_ENABLE)
#define XM_ERR_VO_DEV_HAS_ENABLED       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_ENABLED)
#define XM_ERR_VO_DEV_HAS_BINDED        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_BINDED)
#define XM_ERR_VO_DEV_NOT_BINDED        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_BINDED)

/* video relative error code */
#define XM_ERR_VO_VIDEO_NOT_ENABLE      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_ENABLE)
#define XM_ERR_VO_VIDEO_NOT_DISABLE     XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_DISABLE)
#define XM_ERR_VO_VIDEO_NOT_CONFIG      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_CONFIG)
#define XM_ERR_VO_VIDEO_HAS_BINDED      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_HAS_BINDED)
#define XM_ERR_VO_VIDEO_NOT_BINDED      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_NOT_BINDED)

/*wbc error code*/
#define XM_ERR_VO_WBC_NOT_DISABLE     XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_DISABLE)
#define XM_ERR_VO_WBC_NOT_CONFIG      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_CONFIG)
#define XM_ERR_VO_WBC_HAS_CONFIG      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_HAS_CONFIG)
#define XM_ERR_VO_WBC_NOT_BIND        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_BIND)
#define XM_ERR_VO_WBC_HAS_BIND        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_HAS_BIND)

/* channel relative error code */
#define XM_ERR_VO_CHN_NOT_DISABLE       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_DISABLE)
#define XM_ERR_VO_CHN_NOT_ENABLE        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ENABLE)
#define XM_ERR_VO_CHN_NOT_CONFIG        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_CONFIG)
#define XM_ERR_VO_CHN_NOT_ALLOC         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ALLOC)
#define XM_ERR_VO_CHN_AREA_OVERLAP      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_AREA_OVERLAP)


/* cascade relatvie error code */
#define XM_ERR_VO_INVALID_PATTERN       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_PAT)
#define XM_ERR_VO_INVALID_POSITION      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_POS)

/* misc */
#define XM_ERR_VO_WAIT_TIMEOUT          XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WAIT_TIMEOUT)
#define XM_ERR_VO_INVALID_VFRAME        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_VFRAME)
#define XM_ERR_VO_INVALID_RECT_PARA     XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_RECT_PARA)
#define XM_ERR_VO_SETBEGIN_ALREADY      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_ALREADY)
#define XM_ERR_VO_SETBEGIN_NOTYET       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_NOTYET)
#define XM_ERR_VO_SETEND_ALREADY        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_ALREADY)
#define XM_ERR_VO_SETEND_NOTYET         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_NOTYET)

/* sync group relative error code */
#define XM_ERR_VO_GRP_INVALID_ID        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_ID)
#define XM_ERR_VO_GRP_NOT_CREATE        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_CREATE)
#define XM_ERR_VO_GRP_HAS_CREATED       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_HAS_CREATED)
#define XM_ERR_VO_GRP_NOT_DESTROY       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_DESTROY)
#define XM_ERR_VO_GRP_CHN_FULL          XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_FULL)
#define XM_ERR_VO_GRP_CHN_EMPTY         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_EMPTY)
#define XM_ERR_VO_GRP_CHN_NOT_EMPTY     XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_EMPTY)
#define XM_ERR_VO_GRP_INVALID_SYN_MODE  XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_SYN_MODE)
#define XM_ERR_VO_GRP_INVALID_BASE_PTS  XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_BASE_PTS)
#define XM_ERR_VO_GRP_NOT_START         XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_START)
#define XM_ERR_VO_GRP_NOT_STOP          XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_STOP)
#define XM_ERR_VO_GRP_INVALID_FRMRATE   XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_FRMRATE)
#define XM_ERR_VO_GRP_CHN_HAS_REG       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_HAS_REG)
#define XM_ERR_VO_GRP_CHN_NOT_REG       XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_REG)
#define XM_ERR_VO_GRP_CHN_NOT_UNREG     XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_UNREG)
#define XM_ERR_VO_GRP_BASE_NOT_CFG      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_BASE_NOT_CFG)


/* graphics relative error code */
#define XM_ERR_VO_GFX_NOT_DISABLE     XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_DISABLE)
#define XM_ERR_VO_GFX_NOT_BIND        XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_BIND)
#define XM_ERR_VO_GFX_NOT_UNBIND      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_UNBIND)
#define XM_ERR_VO_GFX_INVALID_ID      XM_DEF_ERR(XM_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_INVALID_ID)

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
typedef XM_S32 VO_INTF_TYPE_E;

//typedef XM_S32 VO_WBC_CHN;

typedef enum xmVO_INTF_SYNC_E
{
    VO_OUTPUT_PAL = 0,
    VO_OUTPUT_NTSC,
    VO_OUTPUT_960H_PAL,              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
    VO_OUTPUT_960H_NTSC,             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/
    
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

    VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/    
    VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x2160_30,          /* 1920x2160_30 */
    VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/ 
    VO_OUTPUT_2560x1440_30,          /* 2560x1440_30 */
    VO_OUTPUT_2560x1600_60,          /* 2560x1600_60 */
    VO_OUTPUT_3840x2160_30,          /* 3840x2160_30 */
    VO_OUTPUT_3840x2160_60,          /* 3840x2160_60 */
	VO_OUTPUT_480x272_60,			 /* 480x272_60*/
    VO_OUTPUT_USER,
    VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;

typedef enum xmVO_DISPLAY_FIELD_E
{
  VO_FIELD_TOP,                 /* top field*/
  VO_FIELD_BOTTOM,              /* bottom field*/
  VO_FIELD_BOTH,                /* top and bottom field*/
  VO_FIELD_BUTT
} VO_DISPLAY_FIELD_E;

typedef enum xmVOU_ZOOM_IN_E
{
    VOU_ZOOM_IN_RECT = 0,       /* zoom in by rect */
    VOU_ZOOM_IN_RATIO,          /* zoom in by ratio */
    VOU_ZOOM_IN_BUTT
} VOU_ZOOM_IN_E;

typedef enum xmVO_CSC_MATRIX_E
{
    VO_CSC_MATRIX_IDENTITY = 0,         /* do not change color space */
    
    VO_CSC_MATRIX_BT601_TO_BT709,       /* change color space from BT.601 to BT.709 */
    VO_CSC_MATRIX_BT709_TO_BT601,       /* change color space from BT.709 to BT.601 */

    VO_CSC_MATRIX_BT601_TO_RGB_PC,      /* change color space from BT.601 to RGB */
    VO_CSC_MATRIX_BT709_TO_RGB_PC,      /* change color space from BT.709 to RGB */

    VO_CSC_MATRIX_RGB_TO_BT601_PC,      /* change color space from RGB to BT.601 */
    VO_CSC_MATRIX_RGB_TO_BT709_PC,      /* change color space from RGB to BT.709 */

    VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;

typedef struct xmVO_CHN_ATTR_S
{
    XM_U32  u32Priority;                /* video out overlay pri sd */
    RECT_S  stRect;                     /* rect of video out chn */
    XM_BOOL bDeflicker;                 /* deflicker or not sd */
}VO_CHN_ATTR_S;

typedef struct tagVO_SYNC_INFO_S
{
    XM_BOOL  bSynm;     /* sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    XM_BOOL  bIop;      /* interlaced or progressive display(0:i; 1:p) */
    XM_U8    u8Intfb;   /* interlace bit width while output */

    XM_U16   u16Vact ;  /* vertical active area */
    XM_U16   u16Vbb;    /* vertical back blank porch */
    XM_U16   u16Vfb;    /* vertical front blank porch */

    XM_U16   u16Hact;   /* herizontal active area */
    XM_U16   u16Hbb;    /* herizontal back blank porch */
    XM_U16   u16Hfb;    /* herizontal front blank porch */
    XM_U16   u16Hmid;   /* bottom herizontal active area */

    XM_U16   u16Bvact;  /* bottom vertical active area */
    XM_U16   u16Bvbb;   /* bottom vertical back blank porch */
    XM_U16   u16Bvfb;   /* bottom vertical front blank porch */

    XM_U16   u16Hpw;    /* horizontal pulse width */
    XM_U16   u16Vpw;    /* vertical pulse width */

    XM_BOOL  bIdv;      /* inverse data valid of output */
    XM_BOOL  bIhs;      /* inverse horizontal synch signal */
    XM_BOOL  bIvs;      /* inverse vertical synch signal */

} VO_SYNC_INFO_S;

typedef struct xmVO_PUB_ATTR_S
{
    XM_U32                   u32BgColor;          /* Background color of a device, in RGB format. */
    VO_INTF_TYPE_E           enIntfType;          /* Type of a VO interface */
    VO_INTF_SYNC_E           enIntfSync;          /* Type of a VO interface timing */
    VO_SYNC_INFO_S           stSyncInfo;          /* Information about VO interface timings */
} VO_PUB_ATTR_S;

typedef struct xmVO_WBC_ATTR_S
{
    SIZE_S              stTargetSize;        /* WBC Zoom target size */ 
    PIXEL_FORMAT_E      enPixelFormat;       /* the pixel format of WBC output */
    XM_U32              u32FrameRate;        /* frame rate control */    
} VO_WBC_ATTR_S;

typedef enum xmVO_WBC_MODE_E
{
    VO_WBC_MODE_NOMAL = 0,                  /* In this mode, wbc will capture frames according to dev frame rate
                                                    and wbc frame rate */
    VO_WBC_MODE_DROP_REPEAT,                /* In this mode, wbc will drop dev repeat frame, and capture the real frame
                                                    according to video layer's display rate and wbc frame rate */
    VO_WBC_MODE_PROG_TO_INTL,               /* In this mode, wbc will drop dev repeat frame which repeats more than 3 times,
                                                     and change two progressive frames to one interlace frame */
    
    VO_WBC_MODE_BUTT,
} VO_WBC_MODE_E;


typedef enum xmVO_WBC_SOURCE_TYPE_E
{
    VO_WBC_SOURCE_DEV      = 0x0,          /* WBC source is device */
    VO_WBC_SOURCE_VIDEO    = 0x1,          /* WBC source is video layer */
    VO_WBC_SOURCE_GRAPHIC  = 0x2,          /* WBC source is graphic layer, not support */
    
    VO_WBC_SOURCE_BUTT
} VO_WBC_SOURCE_TYPE_E;

typedef struct xmVO_WBC_SOURCE_S
{
    VO_WBC_SOURCE_TYPE_E enSourceType;    /* the type of WBC source */
    XM_U32 u32SourceId;                   /* the device, video layer or graphic layer */
} VO_WBC_SOURCE_S;

typedef enum xmVO_CAS_MODE_E
{
    VO_CAS_MODE_SINGLE = 0,        /* cascade mode is single */
    VO_CAS_MODE_DUAL,              /* cascade mode is dual */
    VO_CAS_MODE_BUTT,
} VO_CAS_MODE_E;

typedef enum xmVO_CAS_DATA_TRAN_MODE_E
{
    VO_CAS_DATA_SINGLE_TRAN_MODE = 0, /* single transition,clock rising edge or clock falling edge tigger transition */
    VO_CAS_DATA_DOUBLE_TRAN_MODE,     /* double transition,clock rising edge and clock falling edge tigger transition */
    VO_CAS_DATA_MODE_BUTT,
} VO_CAS_DATA_TRAN_MODE_E;

typedef enum xmVO_CAS_RGN_E
{
    VO_CAS_64_RGN = 0,        
    VO_CAS_32_RGN,            
    VO_CAS_RGN_BUTT,
} VO_CAS_RGN_E;            /* cascade region number */

typedef struct xmVO_CAS_ATTR_S
{
    XM_BOOL         bSlave;                         /* XM_TRUE: slave mode, XM_FALSE: master mode */
    VO_CAS_RGN_E    enCasRgn;                       /* cascade region number */
    VO_CAS_MODE_E   enCasMode;                      /* cascade mode */
    VO_CAS_DATA_TRAN_MODE_E enCasDataTranMode;      /* cascade data transition mode  */
} VO_CAS_ATTR_S;

typedef enum xmVO_PART_MODE_E
{
    VO_PART_MODE_SINGLE	= 0,		/* single partition, which use software to make multi-picture in one hardware cell */
	VO_PART_MODE_MULTI	= 1,		/* muliti partition, each partition is a hardware cell */
	VO_PART_MODE_BUTT
	
} VO_PART_MODE_E;

typedef struct xmVO_COMPRESS_ATTR_S
{
    XM_BOOL bSupportCompress;          /* Whether to support compress */
}VO_COMPRESS_ATTR_S;

typedef struct xmVO_VIDEO_LAYER_ATTR_S
{
    RECT_S stDispRect;                  /* Display resolution */
    SIZE_S stImageSize;                 /* Canvas size of the video layer */
    XM_U32 u32DispFrmRt;                /* Display frame rate */
    PIXEL_FORMAT_E enPixFormat;         /* Pixel format of the video layer */
    XM_BOOL bDoubleFrame;               /* Whether to double frames */  
    XM_BOOL bClusterMode;               /* Whether to take Cluster way to use memory*/
} VO_VIDEO_LAYER_ATTR_S;

typedef enum xmVOU_LAYER_DDR_E
{
    VOU_LAYER_DDR0 = 0,
    VOU_LAYER_DDR1 = 1,
    VOU_LAYER_DDR_BUTT
}VOU_LAYER_DDR_E;

typedef struct xmVO_ZOOM_RATIO_S
{
    XM_U32 u32XRatio;
    XM_U32 u32YRatio;
    XM_U32 u32WRatio;
    XM_U32 u32HRatio;    
} VO_ZOOM_RATIO_S;

typedef struct xmVO_ZOOM_ATTR_S
{
    VOU_ZOOM_IN_E   enZoomType;         /* choose the type of zoom in */
    union
    {
        RECT_S          stZoomRect;     /* zoom in by rect */
        VO_ZOOM_RATIO_S stZoomRatio;    /* zoom in by ratio */
    };
} VO_ZOOM_ATTR_S;

typedef struct xmVO_CSC_S
{
    VO_CSC_MATRIX_E enCscMatrix;
    XM_U32 u32Luma;                     /* luminance:   0 ~ 100 default: 50 */
    XM_U32 u32Contrast;                 /* contrast :   0 ~ 100 default: 50 */
    XM_U32 u32Hue;                      /* hue      :   0 ~ 100 default: 50 */
    XM_U32 u32Saturation;               /* saturation:  0 ~ 100 default: 50 */
} VO_CSC_S;

typedef struct xmVO_VGA_PARAM_S
{
    VO_CSC_S stCSC;                     /* color space */
    XM_U32 u32Gain;                     /* current gain of VGA signals. [0, 64). default:0x30 */
    XM_S32 s32SharpenStrength;          /* current sharpen strength of VGA signals. [0, 255]. default:0x80 */
} VO_VGA_PARAM_S;

typedef struct xmVO_HDMI_PARAM_S
{
    VO_CSC_S stCSC;                    /* color space */
} VO_HDMI_PARAM_S;

typedef struct xmVO_REGION_INFO_S
{
    RECT_S *pstRegion;         /*region attribute*/
    XM_U32 u32RegionNum;       /*count of the region*/
}VO_REGION_INFO_S;

typedef struct xmVO_IMAGE_LAYER_ATTR_S
{
    RECT_S stDispRect;                  /* Display resolution */
    PIXEL_FORMAT_E enPixFormat;         /* Pixel format of the video layer */
	XM_U32  u32PhyAddr;
	XM_U32  u32Effect;                  /*0-7 tran;8-15 tran0;16-23 tran1; 24-27 HUP;28-31VUP */
} VO_IMAGE_LAYER_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __XM_COMM_VO_H__ */

