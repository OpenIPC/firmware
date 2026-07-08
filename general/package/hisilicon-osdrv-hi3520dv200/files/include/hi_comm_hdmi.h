/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_hdmi.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/12/21
  Description   : 
  History       :
  1.Date        : 2011/12/21
    Author      : n00168968
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_HDMI_H__
#define __HI_COMM_HDMI_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common.h"
#include "hi_comm_video.h"

#define HI_HDMI_MAX_AUDIO_CAP_COUNT     15 

#define HI_HDMI_MAX_AUDIO_SMPRATE_COUNT 10

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum hiHDMI_VIDEO_FMT_E
{
    HI_HDMI_VIDEO_FMT_1080P_60 = 0,
    HI_HDMI_VIDEO_FMT_1080P_50,
    HI_HDMI_VIDEO_FMT_1080P_30,
    HI_HDMI_VIDEO_FMT_1080P_25,
    HI_HDMI_VIDEO_FMT_1080P_24,

    HI_HDMI_VIDEO_FMT_1080i_60,
    HI_HDMI_VIDEO_FMT_1080i_50,

    HI_HDMI_VIDEO_FMT_720P_60,
    HI_HDMI_VIDEO_FMT_720P_50,

    HI_HDMI_VIDEO_FMT_576P_50,
    HI_HDMI_VIDEO_FMT_480P_60,

    HI_HDMI_VIDEO_FMT_PAL,                  /* B D G H I PAL */
    HI_HDMI_VIDEO_FMT_PAL_N,                /* (N)PAL        */
    HI_HDMI_VIDEO_FMT_PAL_Nc,               /* (Nc)PAL       */

    HI_HDMI_VIDEO_FMT_NTSC,                 /* (M)NTSC       */
    HI_HDMI_VIDEO_FMT_NTSC_J,               /* NTSC-J        */
    HI_HDMI_VIDEO_FMT_NTSC_PAL_M,           /* (M)PAL        */

    HI_HDMI_VIDEO_FMT_SECAM_SIN,            /**< SECAM_SIN*/
    HI_HDMI_VIDEO_FMT_SECAM_COS,            /**< SECAM_COS*/

    HI_HDMI_VIDEO_FMT_861D_640X480_60,
    HI_HDMI_VIDEO_FMT_VESA_800X600_60,
    HI_HDMI_VIDEO_FMT_VESA_1024X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X720_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X800_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X1024_60,
    HI_HDMI_VIDEO_FMT_VESA_1366X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_1680X1050_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1080_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_2560X1440_30,
    HI_HDMI_VIDEO_FMT_VESA_2048X1152_60,    /* Currently, not support */
    HI_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE, /* Currently, not support */

    HI_HDMI_VIDEO_FMT_BUTT
}HI_HDMI_VIDEO_FMT_E;

typedef enum hiHDMI_VIDEO_SAMPLE_TYPE_E
{
    HI_HDMI_VIDEO_SAMPLE_TYPE_UNKNOWN,
    HI_HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE,
    HI_HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,
    HI_HDMI_VIDEO_SAMPLE_TYPE_INFERED_PROGRESSIVE,
    HI_HDMI_VIDEO_SAMPLE_TYPE_INFERED_INTERLACE,

    HI_HDMI_VIDEO_SAMPLE_TYPE_BUTT
}HI_HDMI_VIDEO_SAMPLE_TYPE_E;

typedef enum hiHDMI_ASPECT_RATIO_E
{
    HI_HDMI_ASPECT_RATIO_UNKNOWN,           /**< unknown aspect ratio */
    HI_HDMI_ASPECT_RATIO_4TO3,              /**< 4:3 */
    HI_HDMI_ASPECT_RATIO_16TO9,             /**< 16:9 */
    HI_HDMI_ASPECT_RATIO_SQUARE,            /**< square */
    HI_HDMI_ASPECT_RATIO_14TO9,             /**< 14:9 */
    HI_HDMI_ASPECT_RATIO_221TO1,            /**< 221:100 */
    HI_HDMI_ASPECT_RATIO_ZOME,              /**< default not support, use source's aspect ratio to display */
    HI_HDMI_ASPECT_RATIO_FULL,              /**< default not support, full screen display */
    
    HI_HDMI_ASPECT_RATIO_BUTT
}HI_HDMI_ASPECT_RATIO_E;

typedef enum hiHDMI_SAMPLE_RATE_E
{
    HI_HDMI_SAMPLE_RATE_UNKNOWN=0,          /**< unknown sample rate    */
    HI_HDMI_SAMPLE_RATE_8K    = 8000,       /**< 8K sample rate      */
    HI_HDMI_SAMPLE_RATE_11K   = 11025,      /**< 11.025K sample rate */
    HI_HDMI_SAMPLE_RATE_12K   = 12000,      /**< 12K sample rate     */
    HI_HDMI_SAMPLE_RATE_16K   = 16000,      /**< 16K sample rate     */
    HI_HDMI_SAMPLE_RATE_22K   = 22050,      /**< 22.050K sample rate */
    HI_HDMI_SAMPLE_RATE_24K   = 24000,      /**< 24K sample rate     */
    HI_HDMI_SAMPLE_RATE_32K   = 32000,      /**< 32K sample rate     */
    HI_HDMI_SAMPLE_RATE_44K   = 44100,      /**< 44.1K sample rate   */
    HI_HDMI_SAMPLE_RATE_48K   = 48000,      /**< 48K sample rate     */
    HI_HDMI_SAMPLE_RATE_88K   = 88200,      /**< 88.2K sample rate   */
    HI_HDMI_SAMPLE_RATE_96K   = 96000,      /**< 96K sample rate     */
	HI_HDMI_SAMPLE_RATE_176K  = 176400,	    /**< 176K sample rate	 */
    HI_HDMI_SAMPLE_RATE_192K  = 192000,     /**< 192K sample rate    */

    HI_HDMI_SAMPLE_RATE_BUTT
}HI_HDMI_SAMPLE_RATE_E;

typedef enum hiHDMI_BIT_DEPTH_E
{
    HI_HDMI_BIT_DEPTH_UNKNOWN =0,           /**< unknown bit width  */
    HI_HDMI_BIT_DEPTH_8  = 8,               /**< 8 bits width  */
    HI_HDMI_BIT_DEPTH_16 = 16,              /**< 16 bits width */
    HI_HDMI_BIT_DEPTH_18 = 18,              /**< 18 bits width */
    HI_HDMI_BIT_DEPTH_20 = 20,              /**< 20 bits width */
    HI_HDMI_BIT_DEPTH_24 = 24,              /**< 24 bits width */
    HI_HDMI_BIT_DEPTH_32 = 32,              /**< 32 bits width */

    HI_HDMI_BIT_DEPTH_BUTT
}HI_HDMI_BIT_DEPTH_E;

typedef enum hiHDMI_SND_INTERFACE_E
{
    HI_HDMI_SND_INTERFACE_I2S,              /**<I2S（Inter-IC Sound） */
    HI_HDMI_SND_INTERFACE_SPDIF,            /**<SPDIF（Sony/Philips Digital Interface） */
    HI_HDMI_SND_INTERFACE_HBR,              /**<SPDIF（Sony/Philips Digital Interface） */
    HI_HDMI_SND_INTERFACE_BUTT
}HI_HDMI_SND_INTERFACE_E;

typedef enum hiHDMI_ID_E
{
    HI_HDMI_ID_0         = 0,               /**< HDMI 0 */
    HI_HDMI_ID_BUTT
} HI_HDMI_ID_E;

/** HDMI EVENT TYPE */
typedef enum hiHDMI_EVENT_TYPE_E
{
    HI_HDMI_EVENT_HOTPLUG = 0x10,           /**<HDMI hot-plug event */
    HI_HDMI_EVENT_NO_PLUG,                  /**<HDMI cable disconnection event */
    HI_HDMI_EVENT_EDID_FAIL,                /**<HDMI EDID read failure event */
    HI_HDMI_EVENT_HDCP_FAIL,                /**<HDCP verification failure event */
    HI_HDMI_EVENT_HDCP_SUCCESS,             /**<HDCP verification success event */
    HI_HDMI_EVENT_HDCP_USERSETTING,         /**<HDMI reset event,user should re-verification HDCP */
    HI_HDMI_EVENT_BUTT
}HI_HDMI_EVENT_TYPE_E;

typedef enum hiHDMI_VIDEO_MODE
{
    HI_HDMI_VIDEO_MODE_RGB444,
    HI_HDMI_VIDEO_MODE_YCBCR422,
    HI_HDMI_VIDEO_MODE_YCBCR444,
        
    HI_HDMI_VIDEO_MODE_BUTT    
}HI_HDMI_VIDEO_MODE_E;

typedef enum hiHDMI_DEEP_COLOR_E
{
    HI_HDMI_DEEP_COLOR_24BIT = 0x00,        /**< HDMI Deep Color 24bit mode  */
    HI_HDMI_DEEP_COLOR_30BIT,               /**< HDMI Deep Color 30bit mode  */
    HI_HDMI_DEEP_COLOR_36BIT,               /**< HDMI Deep Color 36bit mode  */  
    HI_HDMI_DEEP_COLOR_OFF   = 0xff,
    HI_HDMI_DEEP_COLOR_BUTT
}HI_HDMI_DEEP_COLOR_E;

typedef enum hiHDMI_BARINFO_E
{
   HDMI_BAR_INFO_NOT_VALID,                 /**< Bar Data not valid */
   HDMI_BAR_INFO_V,                         /**< Vertical bar data valid */
   HDMI_BAR_INFO_H,                         /**< Horizental bar data valid */
   HDMI_BAR_INFO_VH                         /**< Horizental and Vertical bar data valid */
}HI_HDMI_BARINFO_E;

typedef enum hiHDMI_SCANINFO_E
{
    HDMI_SCAN_INFO_NO_DATA      = 0,        /**< No Scan information*/
    HDMI_SCAN_INFO_OVERSCANNED  = 1,        /**< Scan information, Overscanned (for television) */
    HDMI_SCAN_INFO_UNDERSCANNED = 2,        /**< Scan information, Underscanned (for computer) */
    HDMI_SCAN_INFO_FUTURE
}HI_HDMI_SCANINFO_E;

typedef enum hiHDMI_PICTURE_SCALING_E
{
    HDMI_PICTURE_NON_UNIFORM_SCALING,       /**< No Known, non-uniform picture scaling  */
    HDMI_PICTURE_SCALING_H,                 /**< Picture has been scaled horizentally */
    HDMI_PICTURE_SCALING_V,                 /**< Picture has been scaled Vertically */
    HDMI_PICTURE_SCALING_HV                 /**< Picture has been scaled Horizentally and Vertically   */
 }HI_HDMI_PICTURE_SCALING_E;

typedef enum hiHDMI_COLORSPACE_E
{
    HDMI_COLORIMETRY_NO_DATA,
    HDMI_COLORIMETRY_ITU601,
    HDMI_COLORIMETRY_ITU709,
    HDMI_COLORIMETRY_EXTENDED,
    HDMI_COLORIMETRY_XVYCC_601,
    HDMI_COLORIMETRY_XVYCC_709
}HI_HDMI_COLORSPACE_E;

typedef enum hiHDMI_RGB_QUAN_RAGE_E
{
    HDMI_RGB_QUANTIZATION_DEFAULT_RANGE,    /**< Defaulr range, it depends on the video format */
    HDMI_RGB_QUANTIZATION_LIMITED_RANGE,    /**< Limited quantization range of 220 levels when receiving a CE video format*/
    HDMI_RGB_QUANTIZATION_FULL_RANGE        /**< Full quantization range of 256 levels when receiving an IT video format*/
}HI_HDMI_RGB_QUAN_RAGE_E;

typedef enum hiHDMI_YCC_QUAN_RAGE_E
{
    HDMI_YCC_QUANTIZATION_LIMITED_RANGE,    /**< Limited quantization range of 220 levels when receiving a CE video format*/
    HDMI_YCC_QUANTIZATION_FULL_RANGE        /**< Full quantization range of 256 levels when receiving an IT video format*/
}HI_HDMI_YCC_QUAN_RAGE_E;

typedef enum hiHDMI_CONTENT_TYPE_E
{
    HDMI_CONTNET_GRAPHIC,                   /**< Graphics type*/
    HDMI_CONTNET_PHOTO,                     /**< Photo type*/
    HDMI_CONTNET_CINEMA,                    /**< Cinema type*/
    HDMI_CONTNET_GAME                       /**< Game type*/
}HI_HDMI_CONTENT_TYPE_E;

typedef enum hiHDMI_CODING_TYPE_E
{
    HDMI_AUDIO_CODING_REFER_STREAM_HEAD,
    HDMI_AUDIO_CODING_PCM,
    HDMI_AUDIO_CODING_AC3,
    HDMI_AUDIO_CODING_MPEG1,
    HDMI_AUDIO_CODING_MP3,
    HDMI_AUDIO_CODING_MPEG2,
    HDMI_AUDIO_CODING_AAC,
    HDMI_AUDIO_CODING_DTS,
    HDMI_AUDIO_CODING_DDPLUS,
    HDMI_AUDIO_CODING_MLP,
    HDMI_AUDIO_CODING_WMA,
    HDMI_AUDIO_CODING_MAX
}HI_HDMI_CODING_TYPE_E;

typedef enum hiHDMI_FORCE_ACTION_E
{
    HI_HDMI_FORCE_NULL,                     /**< Standard mode */
    HI_HDMI_FORCE_HDMI,                     /**< Forcibly output video in HDMI mode */
    HI_HDMI_FORCE_DVI,                      /**< Forcibly output video in DVI mode */
    HI_HDMI_INIT_BOOT_CONFIG                /**< Test Only */
}HI_HDMI_FORCE_ACTION_E;

typedef enum hiHI_HDMI_CSC_MATRIX_E
{
    HI_HDMI_CSC_MATRIX_IDENTITY = 0,
    
    HI_HDMI_CSC_MATRIX_BT601_TO_BT709,
    HI_HDMI_CSC_MATRIX_BT709_TO_BT601,

    HI_HDMI_CSC_MATRIX_BT601_TO_RGB_PC,
    HI_HDMI_CSC_MATRIX_BT709_TO_RGB_PC,

    HI_HDMI_CSC_MATRIX_RGB_TO_BT601_PC,
    HI_HDMI_CSC_MATRIX_RGB_TO_BT709_PC,

    HI_HDMI_CSC_MATRIX_BUTT
} HI_HDMI_CSC_MATRIX_E;


/** hdmi event handling callback function */
typedef void (*HI_HDMI_CallBack)(HI_HDMI_EVENT_TYPE_E event, HI_VOID *pPrivateData);

typedef struct hiHDMI_INIT_PARA_S
{
    HI_HDMI_CallBack pfnHdmiEventCallback;    /**< Event handling callback function */
    HI_VOID             *pCallBackArgs;       /**< Private data of the callback functions and parameters */
    HI_HDMI_FORCE_ACTION_E enForceMode;       /**< HDMI forcible output mode */
}HI_HDMI_INIT_PARA_S;

typedef struct hiHDMI_SINK_CAPABILITY_S
{
    HI_BOOL             bConnected;               /**< Whether the devices are connected. */
    HI_BOOL             bSupportHdmi;             /**< Whether the HDMI is supported by the device. If the HDMI is not supported by the device, the device is DVI.*/
    HI_BOOL             bIsSinkPowerOn;           /**< Whether the sink device is powered on. */
    HI_BOOL             bIsRealEDID;              /**< Whether the EDID obtains the flag from the sink device.HI_TRUE: The EDID information is correctly read.HI_FASE: default settings */

    HI_HDMI_VIDEO_FMT_E enNativeVideoFormat;      /**< Physical resolution of the display device. */
    HI_BOOL             bVideoFmtSupported[HI_HDMI_VIDEO_FMT_BUTT]; /**< Video capability set.HI_TRUE: This display format is supported.HI_FALSE: This display format is not supported. */
    HI_BOOL             bSupportYCbCr;            /**< Whether the YCBCR display is supported.HI_TRUE: The YCBCR display is supported.HI_FALSE: Only red-green-blue (RGB) is supported. */
    
    HI_BOOL             bSupportxvYCC601;         /**< Whether the xvYCC601 color format is supported. */
    HI_BOOL             bSupportxvYCC709;         /**< Whether the xvYCC709 color format is supported. */
    HI_U8               u8MDBit;                  /**< Transfer profile supported by xvYCC601. 1: P0; 2: P1; 4: P2. */
    
    HI_BOOL             bAudioFmtSupported[HI_HDMI_MAX_AUDIO_CAP_COUNT]; /**< Audio capability set. For details, see Table 37 in EIA-CEA-861-D.HI_TRUE: This display format is supported.HI_FALSE: This display format is not supported. */
    HI_U32              u32AudioSampleRateSupported[HI_HDMI_MAX_AUDIO_SMPRATE_COUNT]; /**< Audio sampling rate capability set. The value 0 is invalid. Other values are the supported audio sampling rates. */
    HI_U32              u32MaxPcmChannels;        /**< Maximum pulse code modulation (PCM) channel number of the audio. */
    HI_U8               u8Speaker;                /**< Speaker position. For details, see the definition of SpeakerDATABlock in EIA-CEA-861-D. */
    
    HI_U8               u8IDManufactureName[4];   /**< Device vendor flag */
    HI_U32              u32IDProductCode;         /**< Device ID. */
    HI_U32              u32IDSerialNumber;        /**< Device sequence number. */
    HI_U32              u32WeekOfManufacture;     /**< Device production data (week). */
    HI_U32              u32YearOfManufacture;     /**< Set the production data (year). */
    HI_U8               u8Version;                /**< Device version number */
    HI_U8               u8Revision;               /**< Device sub version number */
    HI_U8               u8EDIDExternBlockNum;     /**< EDID extended block number */

    HI_U8               u8IEERegId[3];            /**< 24-bit IEEE Registration Identifier (0x000C03) */
    HI_BOOL             bIsPhyAddrValid;          /**< Valid flag of the consumer electronics control (CEC) physical address */   
    HI_U8               u8PhyAddr_A;              /**< CEC physical address A */
    HI_U8               u8PhyAddr_B;              /**< CEC physical address B */  
    HI_U8               u8PhyAddr_C;              /**< CEC physical address C */  
    HI_U8               u8PhyAddr_D;              /**< CEC physical address D */  
    HI_BOOL             bSupportDVIDual;          /**< Whether to support the DVI dual-link operation. */
    HI_BOOL             bSupportDeepColorYCBCR444;/**< Whether to support the YCBCR 4:4:4 deep-color mode. */
    HI_BOOL             bSupportDeepColor30Bit;   /**< Whether to support the deep-color 30-bit mode.*/
    HI_BOOL             bSupportDeepColor36Bit;   /**< Whether to support the deep-color 36-bit mode. */
    HI_BOOL             bSupportDeepColor48Bit;   /**< Whether to support the deep-color 48-bit mode. */
    HI_BOOL             bSupportAI;               /**< Whether to support the Supports_AI mode. */
    HI_U32              u8MaxTMDSClock;           /**< Maximum TMDS clock. */
    HI_BOOL             bI_Latency_Fields_Present;/**< Delay flag bit. */
    HI_BOOL             bLatency_Fields_Present;  /**< whether Video_Latency and Audio_Latency fields are present */
    HI_BOOL             bHDMI_Video_Present;      /**< Special video format */
    HI_U8               u8Video_Latency;          /**< Video delay */
    HI_U8               u8Audio_Latency;          /**< Audio delay */
    HI_U8               u8Interlaced_Video_Latency;/**< Video delay in interlaced video mode */
    HI_U8               u8Interlaced_Audio_Latency;/**< Audio delay in interlaced video mode */
} HI_HDMI_SINK_CAPABILITY_S;

typedef struct hiUNF_HDMI_ATTR_S
{
	HI_BOOL                 bEnableHdmi;         /**< Whether to forcibly output the video over the HDMI.  */

    HI_BOOL                 bEnableVideo;        /**< Whether to output video.The value must be HI_TRUE.If the value is HI_FALSE, the HDMI is forcibly set to HI_TRUE. */
    HI_HDMI_VIDEO_FMT_E     enVideoFmt;          /**< Video norm. This value of the video norm must be consistent with the norm of the video output. */

    HI_HDMI_VIDEO_MODE_E    enVidOutMode;        /**< HDMI output video output mode.VIDEO_MODE_YCBCR444，VIDEO_MODE_YCBCR422，VIDEO_MODE_RGB444 */
    HI_HDMI_DEEP_COLOR_E    enDeepColorMode;     /**< DeepColor output mode.It is HI_HDMI_DEEP_COLOR_24BIT by default. */
    HI_BOOL                 bxvYCCMode;          /**< Whether to enable the xvYCC output mode.It is HI_FALSE by default. */

    HI_BOOL                 bEnableAudio;        /**< Whether to enable the audio. */
    HI_HDMI_SND_INTERFACE_E enSoundIntf;         /**< HDMI audio source.It is set to HI_HDMI_SND_INTERFACE_I2S, which needs to be consistent with the VO interface. */
    HI_BOOL                 bIsMultiChannel;     /**< Multi-channel or stereo 0: stereo 1: eight-channel fixed for multi-channel */
    HI_HDMI_SAMPLE_RATE_E   enSampleRate;        /**< Audio sampling rate. This parameter needs to be consistent with that of the VO. */
    HI_U8                   u8DownSampleParm;    /**< Audio down sampling rate parameter. It is 0 by default. */
    
    HI_HDMI_BIT_DEPTH_E     enBitDepth;          /**< Audio bit width. It is 16 by default. This parameter needs to be consistent with that of the VO. */
    HI_U8                   u8I2SCtlVbit;        /**< Reserved. It is set to 0. I2S control (0x7A:0x1D) */

    HI_BOOL                 bEnableAviInfoFrame; /**< Whether to enable AVI InfoFrame.It is recommended to enable this function. */
    HI_BOOL                 bEnableAudInfoFrame; /**< Whether to enable AUDIO InfoFrame.It is recommended to enable this function. */
    HI_BOOL                 bEnableSpdInfoFrame; /**< Whether to enable SPD InfoFrame.It is recommended to disable this function. */
    HI_BOOL                 bEnableMpegInfoFrame;/**< Whether to enable MPEG InfoFrame.It is recommended to disable this function. */

    HI_U32                  bDebugFlag;          /**< Whether to enable the debug information in the HDMI.It is recommended to disable this function. */
    HI_BOOL                 bHDCPEnable;         /**< Whether to enable the HDCP.0: disabled 1: enabled */
    
    HI_BOOL                 b3DEnable;           /**< Whether to enable 3D mode. 0: disabled 1: enabled */
    HI_U32                  u83DParam;           /**< 3D Parameter. The default value is 0. */
} HI_HDMI_ATTR_S;

typedef struct hiHI_HDMI_EDID_S
{    
    HI_BOOL                 bEdidValid;
    HI_U32                  u32Edidlength;
    HI_U8                   u8Edid[512];
}HI_HDMI_EDID_S;

typedef struct hiHI_HDMI_CSC_S
{
    HI_HDMI_CSC_MATRIX_E enCscMatrix;
    HI_U32 u32Luma;                     /* luminance:   0 ~ 100 default: 50 */
    HI_U32 u32Contrast;                 /* contrast :   0 ~ 100 default: 50 */
    HI_U32 u32Hue;                      /* hue      :   0 ~ 100 default: 50 */
    HI_U32 u32Satuature;                /* satuature:   0 ~ 100 default: 50 */
} HI_HDMI_CSC_S;

typedef enum tagHI_HDMI_INFOFRAME_TYPE_E
{
    HI_INFOFRAME_TYPE_AVI,          /**< HDMI AVI InfoFrame */
    HI_INFOFRAME_TYPE_SPD,          /**< HDMI SPD InfoFrame */
    HI_INFOFRAME_TYPE_AUDIO,        /**< HDMI AUDIO InfoFrame */    
    HI_INFOFRAME_TYPE_MPEG,         /**< HDMI MPEG InfoFrame */
    HI_INFOFRAME_TYPE_VENDORSPEC,   /**< HDMI Vendor Specific InfoFrame */  
    HI_INFOFRAME_TYPE_BUTT 
}HI_HDMI_INFOFRAME_TYPE_E;

/** HDMI AVI InfoFrame struct, see EIA-CEA-861-D */
typedef struct hiUNF_HDMI_AVI_INFOFRAME_VER2_S
{
    HI_HDMI_VIDEO_FMT_E        enTimingMode;
    HI_HDMI_VIDEO_MODE_E       enOutputType;
    HI_BOOL                    bActive_Infor_Present;
    HI_HDMI_BARINFO_E          enBarInfo;
    HI_HDMI_SCANINFO_E         enScanInfo;
    HI_HDMI_COLORSPACE_E       enColorimetry;
    HI_HDMI_ASPECT_RATIO_E     enAspectRatio;
    HI_HDMI_ASPECT_RATIO_E     enActiveAspectRatio;
    HI_HDMI_PICTURE_SCALING_E  enPictureScaling;
    HI_HDMI_RGB_QUAN_RAGE_E    enRGBQuantization;
    HI_BOOL                    bIsITContent;
    HI_U32                     u32PixelRepetition;

    HI_HDMI_CONTENT_TYPE_E     enContentType;
    HI_HDMI_YCC_QUAN_RAGE_E    enYCCQuantization;
    
    HI_U32                     u32LineNEndofTopBar;
    HI_U32                     u32LineNStartofBotBar;
    HI_U32                     u32PixelNEndofLeftBar;
    HI_U32                     u32PixelNStartofRightBar;
}HI_HDMI_AVI_INFOFRAME_VER2_S;

/** HDMI AUDIO InfoFrame struct, see EIA-CEA-861-D */
typedef struct hiUNF_HDMI_AUD_INFOFRAME_VER1_S
{
    HI_U32                         u32ChannelCount;
    HI_HDMI_CODING_TYPE_E          enCodingType;
    HI_U32                         u32SampleSize;
    HI_U32                         u32SamplingFrequency;
    HI_U32                         u32ChannelAlloc;
    HI_U32                         u32LevelShift;
    HI_BOOL                        u32DownmixInhibit;
}HI_HDMI_AUD_INFOFRAME_VER1_S;

/** HDMI Source Product Description InfoFrame struct , see EIA-CEA-861-D */
typedef struct hiUNF_HDMI_SPD_INFOFRAME_S
{
    HI_U8                          u8VendorName[8];
    HI_U8                          u8ProductDescription[16];  
}HI_HDMI_SPD_INFOFRAME_S;

/** HDMI MEPG SOURCE Description InfoFrame struct , see EIA-CEA-861-D */
typedef struct hiUNF_HDMI_MPEGSOURCE_INFOFRAME_S
{
    HI_U32                         u32MPEGBitRate;
    HI_BOOL                        bIsFieldRepeated;
}HI_HDMI_MPEGSOURCE_INFOFRAME_S;

/** HDMI Vendor Specific InfoFrame struct , see EIA-CEA-861-D */
typedef struct hiUNF_HDMI_VENDORSPEC_INFOFRAME_S
{
    HI_U32                         u32RegistrationId;
}HI_HDMI_VENDORSPEC_INFOFRAME_S;

typedef union hiUNF_HDMI_INFOFRAME_UNIT_U
{
    HI_HDMI_AVI_INFOFRAME_VER2_S   stAVIInfoFrame;
    HI_HDMI_AUD_INFOFRAME_VER1_S   stAUDInfoFrame;
    HI_HDMI_SPD_INFOFRAME_S        stSPDInfoFrame;
    HI_HDMI_MPEGSOURCE_INFOFRAME_S stMPEGSourceInfoFrame;
    HI_HDMI_VENDORSPEC_INFOFRAME_S stVendorSpecInfoFrame;
}HI_HMDI_INFORFRAME_UNIT_U;

typedef struct hiUNF_HDMI_INFOFRAME_S
{
    HI_HDMI_INFOFRAME_TYPE_E    enInfoFrameType;  /**<InfoFrame type */
    HI_HMDI_INFORFRAME_UNIT_U   unInforUnit;      /**<InfoFrame date */  
}HI_HDMI_INFOFRAME_S;

typedef struct hiHI_HDMI_PARAM_S
{
    HI_HDMI_VIDEO_MODE_E    enVidInMode;        /**< HDMI output video input mode.VIDEO_MODE_YCBCR444，VIDEO_MODE_RGB444 */
}HI_HDMI_PARAM_S;

typedef enum hiUNF_HDMI_3D_FORMAT_E
{
    HI_3D_FRAME_PACKETING                 = 0x00,
    HI_3D_FIELD_ALTERNATIVE               = 0x01,
    HI_3D_LINE_ALTERNATIVE                = 0x02,
    HI_3D_SIDE_BY_SIDE_FULL               = 0x03,
    HI_3D_L_DEPTH                         = 0x04,
    HI_3D_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH = 0x05,
    HI_3D_TOP_AND_BOTTOM                  = 0x06,
    HI_3D_SIDE_BY_SIDE_HALF               = 0x08,
    HI_3D_MAX_BUTT
}HI_HDMI_3D_FORMAT_E;

/**<  CEC */
/**< CEC's Opcode:Please refer to CEC 15 Message Descriptions */
/**< General Protocol messages*/
#define CEC_OPCODE_FEATURE_ABORT                  0X00  /**< <Feature Abort> Used as a response to indicate that the device does not support the requested message type, or that it cannot execute it at the present time. */
#define CEC_OPCODE_ABORT_MESSAGE                  0XFF  /**< <Abort> Message This message is reserved for testing purposes. */
/**< One Touch Play Feature*/
#define CEC_OPCODE_ACTIVE_SOURCE                  0X82  /**< <Active Source> Used by a new source to indicate that it has started to transmit a stream OR used in response to a <Request Active Source>  */
#define CEC_OPCODE_IMAGE_VIEW_ON                  0X04  /**< <Image View On> Sent by a source device to the TV whenever it enters the active state (alternatively it may send <Text View On>). */
#define CEC_OPCODE_TEXT_VIEW_ON                   0X0D  /**< <Text View On> As <Image View On>, but should also remove any text, menus and PIP windows from the TV’s display. */
/**< Routing Control Feature*/
#define CEC_OPCODE_INACTIVE_SOURCE                0X9D  /**< <Inactive Source> Used by the currently active source to inform the TV that it has no video to be presented to the user, or is going into standby as the result of a local user command on the device. */
#define CEC_OPCODE_REQUEST_ACTIVE_SOURCE          0X85  /**< <Request Active Source> Used by a new device to discover the status of the system. */
#define CEC_OPCODE_ROUTING_CHANGE                 0X80  /**< <Routing Change> Sent by a CEC Switch when it is manually switched to inform all other devices on the network that the active route below the switch has changed. */
#define CEC_OPCODE_ROUTING_INFORMATION            0X81  /**< <Routing Information> Sent by a CEC Switch to indicate the active route below the switch. */
#define CEC_OPCODE_SET_STREAM_PATH                0X86  /**< <Set Stream Path> Used by the TV to request a streaming path from the specified physical address. */
/**< Standby Feature*/
#define CEC_OPCODE_STANDBY                        0X36  /**< <Standby> Switches one or all devices into standby mode. Can be used as a broadcast message or be addressed to a specific device. See section CEC 13.3 for important notes on the use of this message */
/**< One Touch Record Feature*/
#define CEC_OPCODE_RECORD_OFF                     0X0B  /**< <Record Off> Requests a device to stop a recording. */
#define CEC_OPCODE_RECORD_ON                      0X09  /**< <Record On> Attempt to record the specified source. */
#define CEC_OPCODE_RECORD_STATUS                  0X0A  /**< <Record Status> Used by a Recording Device to inform the initiator of the message <Record On> about its status. */
#define CEC_OPCODE_RECORD_TV_SCREEN               0X0F  /**< <Record TV Screen> Request by the Recording Device to record the presently displayed source. */
/**< Timer Programming Feature*/
#define CEC_OPCODE_CLEAR_ANALOGUE_TIMER           0X33  /**< <Clear Analogue Timer> Used to clear an Analogue timer block of a device. */
#define CEC_OPCODE_CLEAR_DIGITAL_TIMER            0X99  /**< <Clear Digital Timer> Used to clear a Digital timer block of a device. */
#define CEC_OPCODE_CLEAR_EXTERNAL_TIMER           0XA1  /**< <Clear External Timer> Used to clear an External timer block of a device. */
#define CEC_OPCODE_SET_ANALOGUE_TIMER             0X34  /**< <Set Analogue Timer> Used to set a single timer block on an Analogue Recording Device. */
#define CEC_OPCODE_SET_DIGITAL_TIMER              0X97  /**< <Set Digital Timer> Used to set a single timer block on a Digital Recording Device. */
#define CEC_OPCODE_SET_EXTERNAL_TIMER             0XA2  /**< <Set External Timer> Used to set a single timer block to record from an external device. */
#define CEC_OPCODE_SET_TIMER_PROGRAM_TITLE        0X67  /**< <Set Timer Program Title> Used to set the name of a program associated with a timer block. Sent directly after sending a <Set Analogue Timer> or <Set Digital Timer> message. The name is then associated with that timer block. */
#define CEC_OPCODE_TIMER_CLEARED_STATUS           0X43  /**< <Timer Cleared Status> Used to give the status of a <Clear Analogue Timer>, <Clear Digital Timer> or <Clear External Timer> message. */
#define CEC_OPCODE_TIMER_STATUS                   0X35  /**< <Timer Status> Used to send timer status to the initiator of a <Set Timer> message. */
/**< System Information Feature*/
#define CEC_OPCODE_CEC_VERSION                    0X9E  /**< <CEC Version> Used to indicate the supported CEC version, in response to a <Get CEC Version> */
#define CEC_OPCODE_GET_CEC_VERSION                0X9F  /**< <Get CEC Version> Used by a device to enquire which version of CEC the target supports */
#define CEC_OPCODE_GIVE_PHYSICAL_ADDRESS          0X83  /**< <Give Physical Address> A request to a device to return its physical address. */
#define CEC_OPCODE_REPORT_PHYSICAL_ADDRESS        0X84  /**< <Report Physical Address> Used to inform all other devices of the mapping between physical and logical address of the initiator. */
#define CEC_OPCODE_GET_MENU_LANGUAGE              0X91  /**< <Get Menu Language> Sent by a device capable of character generation (for OSD and Menus) to a TV in order to discover the currently selected Menu language. Also used by a TV during installation to discover the currently set menu language of other devices. */
#define CEC_OPCODE_SET_MENU_LANGUAGE              0X32  /**< <Set Menu Language> Used by a TV or another device to indicate the menu language. */
/**< Deck Control Feature*/
#define CEC_OPCODE_DECK_CONTROL                   0X42  /**< <Deck Control> Used to control a device’s media functions. */
#define CEC_OPCODE_DECK_STATUS                    0X1B  /**< <Deck Status> Used to provide a deck’s status to the initiator of the <Give Deck Status> message. */
#define CEC_OPCODE_GIVE_DECK_STATUS               0X1A  /**< <Give Deck Status> Used to request the status of a device, regardless of whether or not it is the current active source. */
#define CEC_OPCODE_PLAY                           0X41  /**< <Play> Used to control the playback behaviour of a source device. */
/**< Tuner Control Feature*/
#define CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS       0X08  /**< <Give Tuner Device Status> Used to request the status of a tuner device. */
#define CEC_OPCODE_SELECT_ANALOGUE_SERVICE        0X92  /**< <Select Analogue Service> Directly selects an Analogue TV service */
#define CEC_OPCODE_SELECT_DIGITAL_SERVICE         0X93  /**< <Select Digital Service> Directly selects a Digital TV, Radio or Data Broadcast Service */
#define CEC_OPCODE_TUNER_DEVICE_STATUS            0X07  /**< <Tuner Device Status> Use by a tuner device to provide its status to the initiator of the <Give Tuner Device Status> message. */
#define CEC_OPCODE_TUNER_STEP_DECREMENT           0X06  /**< <Tuner Step Decrement> Used to tune to next lowest service in a tuner’s service list. Can be used for PIP. */
#define CEC_OPCODE_TUNER_STEP_INCREMENT           0X05  /**< <Tuner Step Increment> Used to tune to next highest service in a tuner’s service list. Can be used for PIP. */
/**< Vendor Specific Command*/
#define CEC_OPCODE_DEVICE_VENDOR_ID               0X87  /**< <Device Vendor ID> Reports the vendor ID of this device. */
#define CEC_OPCODE_GIVE_DEVICE_VENDOR_ID          0X8C  /**< <Give Device Vendor ID> Requests the Vendor ID from a device. */
#define CEC_OPCODE_VENDOR_COMMAND                 0X89  /**< <Vendor Command> Allows vendor specific commands to be sent between two devices. */
#define CEC_OPCODE_VENDOR_COMMAND_WITH_ID         0XA0  /**< <Vendor Command With ID> Allows vendor specific commands to be sent between two devices or broadcast. */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN      0X8A  /**< <Vendor Remote Button Down> Indicates that a remote control button has been depressed. */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP        0X8B  /**< <Vendor Remote Button Up> Indicates that a remote control button (the last button pressed indicated by the Vendor Remote Button Down message) has been released. */
/**< OSD Display Feature*/
#define CEC_OPCODE_SET_OSD_STRING                 0X64  /**< <Set OSD String> Used to send a text message to output on a TV. */
#define CEC_OPCODE_GIVE_OSD_NAME                  0X46  /**< <Give OSD Name> Used to request the preferred OSD name of a device for use in menus associated with that device. */
#define CEC_OPCODE_SET_OSD_NAME                   0X47  /**< <Set OSD Name> Used to set the preferred OSD name of a device for use in menus associated with that device. */
/**< Device Menu Control Feature*/
#define CEC_OPCODE_MENU_REQUEST                   0X8D  /**< <Menu Request> A request from the TV for a device to show/remove a menu or to query if a device is currently showing a menu. */
#define CEC_OPCODE_MENU_STATUS                    0X8E  /**< <Menu Status> Used to indicate to the TV that the device is showing/has removed a menu and requests the remote control keys to be passed though. */
#define CEC_OPCODE_USER_CONTROL_PRESSED           0X44  /**< <User Control Pressed> Used to indicate that the user pressed a remote control button or switched from one remote control button to another. */
#define CEC_OPCODE_USER_CONTROL_RELEASED          0X45  /**< <User Control Released> Indicates that user released a remote control button (the last one indicated by the <User Control Pressed> message) */
/**< Power Status Feature*/
#define CEC_OPCODE_GIVE_DEVICE_POWER_STATUS       0X8F  /**< <Give Device Power Status> Used to determine the current power status of a target device */
#define CEC_OPCODE_REPORT_POWER_STATUS            0X90  /**< <Report Power Status> Used to inform a requesting device of the current power status */
/**< System Audio Control Feature*/
#define CEC_OPCODE_GIVE_AUDIO_STATUS              0X71  /**< <Give Audio Status> Requests an amplifier to send its volume and mute status */
#define CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS  0x7D  /**< <Give System Audio Mode Status> Requests the status of the System Audio Mode */
#define CEC_OPCODE_REPORT_AUDIO_STATUS            0X7A  /**< <Report Audio Status> Reports an amplifier’s volume and mute status */
#define CEC_OPCODE_SET_SYSTEM_AUDIO_MODE          0X72  /**< <Set System Audio Mode> Turns the System Audio Mode On or Off. */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST      0X70  /**< <System Audio Mode Request> A device implementing System Audio Control and which has volume control RC buttons (eg TV or STB) requests to use System Audio Mode to the amplifier */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS       0X7E  /**< <System Audio Mode Status> Reports the current status of the System Audio Mode */
/**< Audio Rate Control Feature*/
#define CEC_OPCODE_SET_AUDIO_RATE                 0X9A  /**< <Set Audio Rate> Used to control audio rate from Source Device. */

#define CEC_OPCODE_POLLING_MESSAGE                0XFE/**<  POLL message have no opcode, So, we just use this value */

typedef enum hiUNF_CEC_LOGICALADD_S
{
    HI_CEC_LOGICALADD_TV               = 0X00,
    HI_CEC_LOGICALADD_RECORDDEV_1      = 0X01,
    HI_CEC_LOGICALADD_RECORDDEV_2      = 0X02,
    HI_CEC_LOGICALADD_TUNER_1          = 0X03,
    HI_CEC_LOGICALADD_PLAYDEV_1        = 0X04,
    HI_CEC_LOGICALADD_AUDIOSYSTEM      = 0X05,
    HI_CEC_LOGICALADD_TUNER_2          = 0X06,
    HI_CEC_LOGICALADD_TUNER_3          = 0X07,
    HI_CEC_LOGICALADD_PLAYDEV_2        = 0X08,
    HI_CEC_LOGICALADD_RECORDDEV_3      = 0X09,
    HI_CEC_LOGICALADD_TUNER_4          = 0X0A,
    HI_CEC_LOGICALADD_PLAYDEV_3        = 0X0B,
    HI_CEC_LOGICALADD_RESERVED_1       = 0X0C,
    HI_CEC_LOGICALADD_RESERVED_2       = 0X0D,
    HI_CEC_LOGICALADD_SPECIALUSE       = 0X0E,
    HI_CEC_LOGICALADD_BROADCAST        = 0X0F,
    HI_CEC_LOGICALADD_BUTT
}HI_CEC_LOGICALADD_S;

typedef enum hiUNF_CEC_CMDTYPE_E
{
    HI_CEC_STRUCTCOMMAND,
    HI_CEC_RAWCOMMAND,
    HI_CEC_BUTT
}HI_CEC_CMDTYPE_E;

typedef struct hiUNF_CEC_RAWDATA_S
{
    HI_U8                              u8Length;
    HI_U8                              u8Data[15];    
}HI_CEC_RAWDATA_S;

typedef enum hiUNF_CEC_UICMD_E                   /**< User Interface Command Opcode */
{
    HI_CEC_UICMD_SELECT                      = 0x00,
    HI_CEC_UICMD_UP                          = 0x01,
    HI_CEC_UICMD_DOWN                        = 0x02,
    HI_CEC_UICMD_LEFT                        = 0x03,
    HI_CEC_UICMD_RIGHT                       = 0x04,
    HI_CEC_UICMD_RIGHT_UP                    = 0x05,
    HI_CEC_UICMD_RIGHT_DOWN                  = 0x06,
    HI_CEC_UICMD_LEFT_UP                     = 0x07,
    HI_CEC_UICMD_LEFT_DOWN                   = 0x08,
    HI_CEC_UICMD_ROOT_MENU                   = 0x09,
    HI_CEC_UICMD_SETUP_MENU                  = 0x0A,
    HI_CEC_UICMD_CONTENTS_MENU               = 0x0B,
    HI_CEC_UICMD_FAVORITE_MENU               = 0x0C,
    HI_CEC_UICMD_EXIT                        = 0x0D,
    HI_CEC_UICMD_NUM_0                       = 0x20,
    HI_CEC_UICMD_NUM_1                       = 0x21,
    HI_CEC_UICMD_NUM_2                       = 0x22,
    HI_CEC_UICMD_NUM_3                       = 0x23,
    HI_CEC_UICMD_NUM_4                       = 0x24,
    HI_CEC_UICMD_NUM_5                       = 0x25,
    HI_CEC_UICMD_NUM_6                       = 0x26,
    HI_CEC_UICMD_NUM_7                       = 0x27,
    HI_CEC_UICMD_NUM_8                       = 0x28,
    HI_CEC_UICMD_NUM_9                       = 0x29,
    HI_CEC_UICMD_DOT                         = 0x2A,
    HI_CEC_UICMD_ENTER                       = 0x2B,
    HI_CEC_UICMD_CLEAR                       = 0x2C,
    HI_CEC_UICMD_NEXT_FAVORITE               = 0x2F,
    HI_CEC_UICMD_CHANNEL_UP                  = 0x30,
    HI_CEC_UICMD_CHANNEL_DOWN                = 0x31,
    HI_CEC_UICMD_PREVIOUS_CHANNEL            = 0x32,
    HI_CEC_UICMD_SOUND_SELECT                = 0x33,
    HI_CEC_UICMD_INPUT_SELECT                = 0x34,
    HI_CEC_UICMD_DISPLAY_INFORMATION         = 0x35,
    HI_CEC_UICMD_HELP                        = 0x36,
    HI_CEC_UICMD_PAGE_UP                     = 0x37,
    HI_CEC_UICMD_PAGE_DOWN                   = 0x38,
    HI_CEC_UICMD_POWER                       = 0x40,
    HI_CEC_UICMD_VOLUME_UP                   = 0x41,
    HI_CEC_UICMD_VOLUME_DOWN                 = 0x42,
    HI_CEC_UICMD_MUTE                        = 0x43,
    HI_CEC_UICMD_PLAY                        = 0x44,
    HI_CEC_UICMD_STOP                        = 0x45,
    HI_CEC_UICMD_PAUSE                       = 0x46,
    HI_CEC_UICMD_RECORD                      = 0x47,
    HI_CEC_UICMD_REWIND                      = 0x48,
    HI_CEC_UICMD_FAST_FORWARD                = 0x49,
    HI_CEC_UICMD_EJECT                       = 0x4A,
    HI_CEC_UICMD_FORWARD                     = 0x4B,
    HI_CEC_UICMD_BACKWARD                    = 0x4C,
    HI_CEC_UICMD_STOP_RECORD                 = 0x4D,
    HI_CEC_UICMD_PAUSE_RECORD                = 0x4E,
    HI_CEC_UICMD_ANGLE                       = 0x50,
    HI_CEC_UICMD_SUBPICTURE                  = 0x51,
    HI_CEC_UICMD_VIDEO_ON_DEMAND             = 0x52,
    HI_CEC_UICMD_ELECTRONIC_PROGRAM_GUIDE    = 0x53,
    HI_CEC_UICMD_TIMER_PROGRAMMING           = 0x54,
    HI_CEC_UICMD_INITIAL_CONFIGURATION       = 0x55,
    HI_CEC_UICMD_PLAY_FUNCTION               = 0x60,
    HI_CEC_UICMD_PAUSE_PLAY_FUNCTION         = 0x61,
    HI_CEC_UICMD_RECORD_FUNCTION             = 0x62,
    HI_CEC_UICMD_PAUSE_RECORD_FUNCTION       = 0x63,
    HI_CEC_UICMD_STOP_FUNCTION               = 0x64,
    HI_CEC_UICMD_MUTE_FUNCTION               = 0x65,
    HI_CEC_UICMD_RESTORE_VOLUME_FUNCTION     = 0x66,
    HI_CEC_UICMD_TUNE_FUNCTION               = 0x67,
    HI_CEC_UICMD_SELECT_MEDIA_FUNCTION       = 0x68,
    HI_CEC_UICMD_SELECT_AV_INPUT_FUNCTION    = 0x69,
    HI_CEC_UICMD_SELECT_AUDIO_INPUT_FUNCTION = 0x6A,
    HI_CEC_UICMD_POWER_TOGGLE_FUNCTION       = 0x6B,
    HI_CEC_UICMD_POWER_OFF_FUNCTION          = 0x6C,
    HI_CEC_UICMD_POWER_ON_FUNCTION           = 0x6D,
    HI_CEC_UICMD_F1_BLUE                     = 0x71,
    HI_CEC_UICMD_F2_RED                      = 0x72,
    HI_CEC_UICMD_F3_GREEN                    = 0x73,
    HI_CEC_UICMD_F4_YELLOW                   = 0x74,
    HI_CEC_UICMD_F5                          = 0x75,
    HI_CEC_UICMD_DATA                        = 0x76
}HI_CEC_UICMD_E;                                     /* User Control Code */

typedef union hiUNF_CEC_Operand_t
{
    HI_CEC_RAWDATA_S               stRawData;
    HI_CEC_UICMD_E                 stUIOpcode;
}HI_CEC_Operand_t;

typedef struct hiUNF_HDMI_CEC_CMD_S
{
    HI_CEC_LOGICALADD_S  enSrcAdd;
    HI_CEC_LOGICALADD_S  enDstAdd;
    HI_U8                    u8Opcode;
    HI_CEC_Operand_t     unOperand;   
}HI_HDMI_CEC_CMD_S;

typedef struct hiUNF_HDMI_CEC_STATUS_S
{
    HI_BOOL bEnable;
    HI_U8   u8PhysicalAddr[4];                        /**< CEC physic addr */
    HI_U8   u8LogicalAddr;                            /**< CEC logic addr, default:0x03. */
    HI_U8   u8Network[HI_CEC_LOGICALADD_BUTT];    /**< CEC's net work，1:this device can answer cec command */
}HI_HDMI_CEC_STATUS_S;


typedef enum hiERR_HDMI_CODE_E
{
    ERR_HDMI_NOT_INIT             = 1,
    ERR_HDMI_INVALID_PARA         = 2,
    ERR_HDMI_NUL_PTR              = 3,
    ERR_HDMI_DEV_NOT_OPEN         = 4,
    ERR_HDMI_DEV_NOT_CONNECT      = 5,
    ERR_HDMI_READ_SINK_FAILED     = 6,
    ERR_HDMI_INIT_ALREADY         = 7,
    ERR_HDMI_CALLBACK_ALREADY     = 8,
    ERR_HDMI_INVALID_CALLBACK     = 9,
    ERR_HDMI_FEATURE_NO_SUPPORT   = 10,
    ERR_HDMI_BUS_BUSY             = 11,
    ERR_HDMI_READ_EVENT_FAILED    = 12,
    ERR_HDMI_NOT_START            = 13,
    ERR_HDMI_READ_EDID_FAILED     = 14,
    ERR_HDMI_READ_DVI_EDID_SUCCESS = 15,

    ERR_HDMI_BUTT,
} ERR_HDMI_CODE_E;

#define HI_ERR_HDMI_NOT_INIT            HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_NOT_INIT)
#define HI_ERR_HDMI_INVALID_PARA        HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_INVALID_PARA)
#define HI_ERR_HDMI_NUL_PTR             HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_NUL_PTR)
#define HI_ERR_HDMI_DEV_NOT_OPEN        HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_DEV_NOT_OPEN)
#define HI_ERR_HDMI_DEV_NOT_CONNECT     HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_DEV_NOT_CONNECT)
#define HI_ERR_HDMI_READ_SINK_FAILED    HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_READ_SINK_FAILED)
#define HI_ERR_HDMI_INIT_ALREADY        HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_INIT_ALREADY)
#define HI_ERR_HDMI_CALLBACK_ALREADY    HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_CALLBACK_ALREADY)
#define HI_ERR_HDMI_INVALID_CALLBACK    HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_INVALID_CALLBACK)
#define HI_ERR_HDMI_FEATURE_NO_SUPPORT  HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_FEATURE_NO_SUPPORT)
#define HI_ERR_HDMI_BUS_BUSY            HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_BUS_BUSY)
#define HI_ERR_HDMI_READ_EVENT_FAILED   HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_READ_EVENT_FAILED)
#define HI_ERR_HDMI_NOT_START           HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_NOT_START)
#define HI_ERR_HDMI_READ_EDID_FAILED    HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_READ_EDID_FAILED)
/*HSCP201311290001 l00183122  20131210 DVI模式下，有输出，但是读不到EDID
  理论上，DVI模式下是可以读取前128位EDID，但是如果读取后返回成功，将改变原有流程，风险不可控；
  因此为了不改变原有流程，设置一个特殊的错误码，在DVI模式下使用，既可以读到EDID，但流程上又按读不到处理，只影响用户读取EDID的结果*/
#define HI_ERR_HDMI_READ_DVI_EDID_SUCCESS    HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_READ_DVI_EDID_SUCCESS)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMM_VO_H__ */

