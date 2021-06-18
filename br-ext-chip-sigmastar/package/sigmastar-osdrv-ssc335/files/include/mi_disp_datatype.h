/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_DISP_DATATYPE_H_
#define _MI_DISP_DATATYPE_H_
#include "mi_sys_datatype.h"

#define MI_DISP_SUCCESS (0)
#define MI_DISP_FAIL    (1)
#define MI_ERR_DISP_INVALID_DEVID     (0xA00F8001) //The device ID does not fall within the value range.
#define MI_ERR_DISP_INVALID_CHNID     (0xA00F8002) //The channel ID does not fall within the value range.
#define MI_ERR_DISP_ILLEGAL_PARAM     (0xA00F8003) //The parameter value does not fall within the value range.
#define MI_ERR_DISP_NULL_PTR          (0xA00F8006) //The pointer is null.
#define MI_ERR_DISP_NOT_SUPPORT       (0xA00F8008) //The operation is not supported.
#define MI_ERR_DISP_NOT_PERMIT        (0xA00F8009) //The operation is forbidden.
#define MI_ERR_DISP_NO_MEM            (0xA00F800C) //The memory is insufficient.
#define MI_ERR_DISP_SYS_NOTREADY      (0xA00F8010) //The system is not initialized.
#define MI_ERR_DISP_BUSY              (0xA00F8012) //The resources are unavailable.
#define MI_ERR_DISP_DEV_NOT_CONFIG    (0xA00F8040) //The MI_DISP device is not configured.
#define MI_ERR_DISP_DEV_NOT_ENABLE    (0xA00F8041) //The MI_DISP device is not enabled.
#define MI_ERR_DISP_DEV_HAS_ENABLED   (0xA00F8042) //The MI_DISP device has been enabled.
#define MI_ERR_DISP_DEV_HAS_BINDED    (0xA00F8043) //The device has been bound.
#define MI_ERR_DISP_DEV_NOT_BINDED    (0xA00F8044) //The device is not bound.
#define MI_ERR_DISP_VIDEO_NOT_ENABLE  (0xA00F8045) //The video layer is not enabled.
#define MI_ERR_DISP_VIDEO_NOT_DISABLE (0xA00F8046) //The video layer is not disabled.
#define MI_ERR_DISP_VIDEO_NOT_CONFIG  (0xA00F8047) //The video layer is not configured.
#define MI_ERR_DISP_CHN_NOT_DISABLE   (0xA00F8048) //The MI_DISP input port is not disabled.
#define MI_ERR_DISP_CHN_NOT_ENABLE    (0xA00F8049) //No MI_DISP input port is enabled.
#define MI_ERR_DISP_CHN_NOT_CONFIG    (0xA00F804A) //The MI_DISP input port is not configured.
#define MI_ERR_DISP_CHN_NOT_ALLOC     (0xA00F804B) //No MI_DISP input port is allocated.
#define MI_ERR_DISP_INVALID_PATTERN   (0xA00F804C) //The pattern is invalid.
#define MI_ERR_DISP_INVALID_POSITION  (0xA00F804D) //The cascade position is invalid.
#define MI_ERR_DISP_WAIT_TIMEOUT      (0xA00F804E) //Waiting times out.
#define MI_ERR_DISP_INVALID_VFRAME    (0xA00F804F) //The video frame is invalid.
#define MI_ERR_DISP_INVALID_RECT_PARA (0xA00F8050) //The rectangle parameter is invalid.
#define MI_ERR_DISP_GFX_NOT_DISABLE   (0xA00F8065) //The graphics layer is not disabled.
#define MI_ERR_DISP_GFX_NOT_BIND      (0xA00F8066) //The graphics layer is not bound.
#define MI_ERR_DISP_GFX_NOT_UNBIND    (0xA00F8067) //The graphics layer is not unbound.
#define MI_ERR_DISP_GFX_INVALID_ID    (0xA00F8068) //The graphics layer ID does not fall within the value range.
#define MI_ERR_DISP_CHN_AREA_OVERLAP  (0xA00F806b) //The MI_DISP input port areas overlap.
#define MI_ERR_DISP_INVALID_LAYERID   (0xA00F806d) //The video layer ID does not fall within the value range.
#define MI_ERR_DISP_VIDEO_HAS_BINDED  (0xA00F806e) //The video layer has been bound.
#define MI_ERR_DISP_VIDEO_NOT_BINDED  (0xA00F806f) //The video layer is not bound

#define MI_DISP_DEV_MAX 1
#define MI_DISP_LAYER_MAX 2
#define MI_DISP_INPUTPORT_MAX 16

//typedef MI_U32 MI_DISP_DEV;
//typedef MI_U32 MI_DISP_LAYER;
typedef MI_U32 MI_DISP_INPUTPORT;

typedef enum
{
    E_MI_DISP_INTF_CVBS = 0,
    E_MI_DISP_INTF_YPBPR,
    E_MI_DISP_INTF_VGA,
    E_MI_DISP_INTF_BT656,
    E_MI_DISP_INTF_BT1120,
    E_MI_DISP_INTF_HDMI,
    E_MI_DISP_INTF_LCD,
    E_MI_DISP_INTF_BT656_H,
    E_MI_DISP_INTF_BT656_L,
    E_MI_DISP_INTF_TTL,
    E_MI_DISP_INTF_MIPIDSI,
    E_MI_DISP_INTF_TTL_SPI_IF,
    E_MI_DISP_INTF_MAX,
}MI_DISP_Interface_e;

typedef enum
{
    E_MI_DISP_OUTPUT_PAL = 0,
    E_MI_DISP_OUTPUT_NTSC,
    E_MI_DISP_OUTPUT_960H_PAL,              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
    E_MI_DISP_OUTPUT_960H_NTSC,             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/

    E_MI_DISP_OUTPUT_480i60,
    E_MI_DISP_OUTPUT_576i50,
    E_MI_DISP_OUTPUT_480P60,
    E_MI_DISP_OUTPUT_576P50,
    E_MI_DISP_OUTPUT_720P50,
    E_MI_DISP_OUTPUT_720P60,
    E_MI_DISP_OUTPUT_1080P24,
    E_MI_DISP_OUTPUT_1080P25,
    E_MI_DISP_OUTPUT_1080P30,
    E_MI_DISP_OUTPUT_1080I50,
    E_MI_DISP_OUTPUT_1080I60,
    E_MI_DISP_OUTPUT_1080P50,
    E_MI_DISP_OUTPUT_1080P60,

    E_MI_DISP_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    E_MI_DISP_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    E_MI_DISP_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    E_MI_DISP_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    E_MI_DISP_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    E_MI_DISP_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    E_MI_DISP_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
    E_MI_DISP_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    E_MI_DISP_OUTPUT_1920x2160_30,          /* 1920x2160_30 */
    E_MI_DISP_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    E_MI_DISP_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/

    E_MI_DISP_OUTPUT_2560x1440_30,          /* 2560x1440_30 */
    E_MI_DISP_OUTPUT_2560x1600_60,          /* 2560x1600_60 */
    E_MI_DISP_OUTPUT_3840x2160_30,          /* 3840x2160_30 */
    E_MI_DISP_OUTPUT_3840x2160_60,          /* 3840x2160_60 */
    E_MI_DISP_OUTPUT_USER,
    E_MI_DISP_OUTPUT_MAX,
} MI_DISP_OutputTiming_e;

typedef enum
{
    E_MI_DISP_ROTATE_NONE,
    E_MI_DISP_ROTATE_90,
    E_MI_DISP_ROTATE_180,
    E_MI_DISP_ROTATE_270,
    E_MI_DISP_ROTATE_NUM,
}MI_DISP_RotateMode_e;

typedef enum
{
    E_MI_DISP_CSC_MATRIX_BYPASS = 0,         /* do not change color space */

    E_MI_DISP_CSC_MATRIX_BT601_TO_BT709,       /* change color space from BT.601 to BT.709 */
    E_MI_DISP_CSC_MATRIX_BT709_TO_BT601,       /* change color space from BT.709 to BT.601 */

    E_MI_DISP_CSC_MATRIX_BT601_TO_RGB_PC,      /* change color space from BT.601 to RGB */
    E_MI_DISP_CSC_MATRIX_BT709_TO_RGB_PC,      /* change color space from BT.709 to RGB */

    E_MI_DISP_CSC_MATRIX_RGB_TO_BT601_PC,      /* change color space from RGB to BT.601 */
    E_MI_DISP_CSC_MATRIX_RGB_TO_BT709_PC,      /* change color space from RGB to BT.709 */

    E_MI_DISP_CSC_MATRIX_NUM
} MI_DISP_CscMattrix_e;

typedef enum
{
    E_MI_DISP_SYNC_MODE_INVALID = 0,
    E_MI_DISP_SYNC_MODE_CHECK_PTS,
    E_MI_DISP_SYNC_MODE_FREE_RUN,
    E_MI_DISP_SYNC_MODE_NUM,
} MI_DISP_SyncMode_e;

typedef enum
{
    E_MI_LAYER_INPUTPORT_STATUS_INVALID = 0,
    E_MI_LAYER_INPUTPORT_STATUS_PAUSE,
    E_MI_LAYER_INPUTPORT_STATUS_RESUME,
    E_MI_LAYER_INPUTPORT_STATUS_STEP,
    E_MI_LAYER_INPUTPORT_STATUS_REFRESH,
    E_MI_LAYER_INPUTPORT_STATUS_SHOW,
    E_MI_LAYER_INPUTPORT_STATUS_HIDE,
    E_MI_LAYER_INPUTPORT_STATUS_NUM,
} MI_DISP_InputPortStatus_e;

typedef struct MI_DISP_SyncInfo_s
{
    MI_BOOL  bSynm;     /* sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    MI_BOOL  bIop;      /* interlaced or progressive display(0:i; 1:p) */
    MI_U8    u8Intfb;   /* interlace bit width while output */

    MI_U16   u16Vact ;  /* vertical active area */
    MI_U16   u16Vbb;    /* vertical back blank porch */
    MI_U16   u16Vfb;    /* vertical front blank porch */

    MI_U16   u16Hact;   /* herizontal active area */
    MI_U16   u16Hbb;    /* herizontal back blank porch */
    MI_U16   u16Hfb;    /* herizontal front blank porch */
    MI_U16   u16Hmid;   /* bottom herizontal active area */

    MI_U16   u16Bvact;  /* bottom vertical active area */
    MI_U16   u16Bvbb;   /* bottom vertical back blank porch */
    MI_U16   u16Bvfb;   /* bottom vertical front blank porch */

    MI_U16   u16Hpw;    /* horizontal pulse width */
    MI_U16   u16Vpw;    /* vertical pulse width */

    MI_BOOL  bIdv;      /* inverse data valid of output */
    MI_BOOL  bIhs;      /* inverse horizontal synch signal */
    MI_BOOL  bIvs;      /* inverse vertical synch signal */
    MI_U32   u32FrameRate;
} MI_DISP_SyncInfo_t;


typedef struct MI_DISP_PubAttr_s
{
    MI_U32                   u32BgColor;          /* Background color of a device, in RGB format. */
    MI_DISP_Interface_e      eIntfType;         /* Type of a VO interface */
    MI_DISP_OutputTiming_e  eIntfSync;          /* Type of a VO interface timing */
    MI_DISP_SyncInfo_t       stSyncInfo;          /* Information about VO interface timings */
} MI_DISP_PubAttr_t;

typedef struct MI_DISP_CompressAttr_s
{
    MI_BOOL bSupportCompress;          /* Whether to support compress */
} MI_DISP_CompressAttr_t;

typedef struct MI_DISP_VidWin_Rect_s
{
    MI_U16 u16X;
    MI_U16 u16Y;
    MI_U16 u16Width;
    MI_U16 u16Height;
} MI_DISP_VidWinRect_t;

typedef struct MI_DISP_VideoLayerSize_s
{
    MI_U32 u16Width;
    MI_U32 u16Height;
} MI_DISP_VideoLayerSize_t;

typedef struct MI_DISP_VideoLayerAttr_s
{
    MI_DISP_VidWinRect_t     stVidLayerDispWin;                  /* Display resolution */
// TODO: dev
    MI_DISP_VideoLayerSize_t stVidLayerSize;                 /* Canvas size of the video layer */
    // TODO: Tommy delete pixel format & doc
    MI_SYS_PixelFormat_e    ePixFormat;         /* Pixel format of the video layer */
} MI_DISP_VideoLayerAttr_t;

typedef struct MI_DISP_Csc_s
{
    MI_DISP_CscMattrix_e eCscMatrix;
    MI_U32 u32Luma;                     /* luminance:   0 ~ 100 default: 50 */
    MI_U32 u32Contrast;                 /* contrast :   0 ~ 100 default: 50 */
    MI_U32 u32Hue;                      /* hue      :   0 ~ 100 default: 50 */
    MI_U32 u32Saturation;               /* saturation:  0 ~ 100 default: 40 */
} MI_DISP_Csc_t;

typedef struct MI_DISP_VideoFrame_s
{
    MI_U32          u32Width;
    MI_U32          u32Height;
    MI_SYS_PixelFormat_e ePixelFormat;
    MI_PHY          aphyAddr;
    void *         pavirAddr;
    MI_U64          u64pts;
    MI_U32          u32PrivateData;
    MI_U32          u32Size;
    MI_U32          u32Stride;
} MI_DISP_VideoFrame_t;
/* General Operation of InputPort */
typedef struct MI_DISP_InputPortAttr_s
{
    MI_DISP_VidWinRect_t stDispWin;                     /* rect of video out chn */
    MI_U16 u16SrcWidth;
    MI_U16 u16SrcHeight;
} MI_DISP_InputPortAttr_t;
typedef struct MI_DISP_Position_s
{
    MI_U16 u16X;
    MI_U16 u16Y;
} MI_DISP_Position_t;

typedef struct MI_DISP_QueryChannelStatus_s
{
    MI_BOOL bEnable;
    MI_DISP_InputPortStatus_e eStatus;
} MI_DISP_QueryChannelStatus_t;

typedef struct MI_DISP_VgaParam_s
{
    MI_DISP_Csc_t stCsc;                     /* color space */
    MI_U32 u32Gain;                          /* current gain of VGA signals. [0, 64). default:0x30 */
    MI_U32 u32Sharpness;
} MI_DISP_VgaParam_t;

typedef struct MI_DISP_HdmiParam_s
{
    MI_DISP_Csc_t stCsc;                     /* color space */
    MI_U32 u32Gain;                          /* current gain of HDMI signals. [0, 64). default:0x30 */
    MI_U32 u32Sharpness;
} MI_DISP_HdmiParam_t;

typedef struct MI_DISP_LcdParam_s
{
    MI_DISP_Csc_t stCsc;                     /* color space */
    MI_U32 u32Sharpness;
} MI_DISP_LcdParam_t;

typedef struct MI_DISP_CvbsParam_s
{
    MI_BOOL bEnable;
} MI_DISP_CvbsParam_t;


typedef struct MI_DISP_RotateConfig_s
{
    MI_DISP_RotateMode_e eRotateMode;
}MI_DISP_RotateConfig_t;

typedef struct
{
    MI_U16 u16RedOffset;
    MI_U16 u16GreenOffset;
    MI_U16 u16BlueOffset;

    MI_U16 u16RedColor;  // 00~FF, 0x80 is no change
    MI_U16 u16GreenColor;// 00~FF, 0x80 is no change
    MI_U16 u16BlueColor; // 00~FF, 0x80 is no change
}MI_DISP_ColorTemperature_t;

typedef struct
{
    MI_BOOL bEn;
    MI_U16 u16EntryNum;
    MI_U8 *pu8ColorR;
    MI_U8 *pu8ColorG;
    MI_U8 *pu8ColorB;
}MI_DISP_GammaParam_t;

typedef struct MI_DISP_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_DISP_InitParam_t;

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#endif///_MI_DISP_DATATYPE_H_
