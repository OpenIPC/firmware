/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_HDMI_DATATYPE_H_
#define _MI_HDMI_DATATYPE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"

/*-------------------------------------------------------------------------------------------------
 * Enum
 ------------------------------------------------------------------------------------------------*/
typedef enum
{
    E_MI_HDMI_ID_0 = 0,
    E_MI_HDMI_ID_MAX
} MI_HDMI_DeviceId_e;

typedef enum
{
    E_MI_HDMI_EVENT_HOTPLUG = 0,
    E_MI_HDMI_EVENT_NO_PLUG,
    E_MI_HDMI_EVENT_MAX
} MI_HDMI_EventType_e;

typedef enum
{
    E_MI_HDMI_TIMING_480_60I         = 0,
    E_MI_HDMI_TIMING_480_60P         = 1,
    E_MI_HDMI_TIMING_576_50I         = 2,
    E_MI_HDMI_TIMING_576_50P         = 3,
    E_MI_HDMI_TIMING_720_50P         = 4,
    E_MI_HDMI_TIMING_720_60P         = 5,
    E_MI_HDMI_TIMING_1080_50I        = 6,
    E_MI_HDMI_TIMING_1080_50P        = 7,
    E_MI_HDMI_TIMING_1080_60I        = 8,
    E_MI_HDMI_TIMING_1080_60P        = 9,
    E_MI_HDMI_TIMING_1080_30P        = 10,
    E_MI_HDMI_TIMING_1080_25P        = 11,
    E_MI_HDMI_TIMING_1080_24P        = 12,
    E_MI_HDMI_TIMING_4K2K_30P        = 13,
    E_MI_HDMI_TIMING_1440_50P        = 14,
    E_MI_HDMI_TIMING_1440_60P        = 15,
    E_MI_HDMI_TIMING_1440_24P        = 16,
    E_MI_HDMI_TIMING_1440_30P        = 17,
    E_MI_HDMI_TIMING_1470_50P        = 18,
    E_MI_HDMI_TIMING_1470_60P        = 19,
    E_MI_HDMI_TIMING_1470_24P        = 20,
    E_MI_HDMI_TIMING_1470_30P        = 21,
    E_MI_HDMI_TIMING_1920x2205_24P   = 22,
    E_MI_HDMI_TIMING_1920x2205_30P   = 23,
    E_MI_HDMI_TIMING_4K2K_25P        = 24,
    E_MI_HDMI_TIMING_4K1K_60P        = 25,
    E_MI_HDMI_TIMING_4K2K_60P        = 26,
    E_MI_HDMI_TIMING_4K2K_24P        = 27,
    E_MI_HDMI_TIMING_4K2K_50P        = 28,
    E_MI_HDMI_TIMING_2205_24P        = 29,
    E_MI_HDMI_TIMING_4K1K_120P       = 30,
    E_MI_HDMI_TIMING_4096x2160_24P   = 31,
    E_MI_HDMI_TIMING_4096x2160_25P   = 32,
    E_MI_HDMI_TIMING_4096x2160_30P   = 33,
    E_MI_HDMI_TIMING_4096x2160_50P   = 34,
    E_MI_HDMI_TIMING_4096x2160_60P   = 35,
    E_MI_HDMI_TIMING_1024x768_60P    = 36,
    E_MI_HDMI_TIMING_1280x1024_60P   = 37,
    E_MI_HDMI_TIMING_1440x900_60P    = 38,
    E_MI_HDMI_TIMING_1600x1200_60P   = 39,
    E_MI_HDMI_TIMING_1280x800_60P    = 40,
    E_MI_HDMI_TIMING_1366x768_60P    = 41,
    E_MI_HDMI_TIMING_1680x1050_60P   = 42,
    E_MI_HDMI_TIMING_MAX,
} MI_HDMI_TimingType_e;

typedef enum
{
    E_MI_HDMI_COLOR_TYPE_RGB444 = 0,
    E_MI_HDMI_COLOR_TYPE_YCBCR422,
    E_MI_HDMI_COLOR_TYPE_YCBCR444,
    E_MI_HDMI_COLOR_TYPE_YCBCR420,
    E_MI_HDMI_COLOR_TYPE_MAX
} MI_HDMI_ColorType_e;

typedef enum
{
    E_MI_HDMI_OUTPUT_MODE_HDMI = 0,
    E_MI_HDMI_OUTPUT_MODE_HDMI_HDCP,
    E_MI_HDMI_OUTPUT_MODE_DVI,
    E_MI_HDMI_OUTPUT_MODE_DVI_HDCP,
    E_MI_HDMI_OUTPUT_MODE_MAX,
} MI_HDMI_OutputMode_e;

typedef enum
{
    E_MI_HDMI_DEEP_COLOR_24BIT = 0,
    E_MI_HDMI_DEEP_COLOR_30BIT,
    E_MI_HDMI_DEEP_COLOR_36BIT,
    E_MI_HDMI_DEEP_COLOR_48BIT,
    E_MI_HDMI_DEEP_COLOR_MAX,
} MI_HDMI_DeepColor_e;

typedef enum
{
    E_MI_HDMI_AUDIO_SAMPLERATE_UNKNOWN      = 0,
    E_MI_HDMI_AUDIO_SAMPLERATE_32K          = 1,
    E_MI_HDMI_AUDIO_SAMPLERATE_44K          = 2,
    E_MI_HDMI_AUDIO_SAMPLERATE_48K          = 3,
    E_MI_HDMI_AUDIO_SAMPLERATE_88K          = 4,
    E_MI_HDMI_AUDIO_SAMPLERATE_96K          = 5,
    E_MI_HDMI_AUDIO_SAMPLERATE_176K         = 6,
    E_MI_HDMI_AUDIO_SAMPLERATE_192K         = 7,
    E_MI_HDMI_AUDIO_SAMPLERATE_MAX,
} MI_HDMI_SampleRate_e;

typedef enum
{
    E_MI_HDMI_BIT_DEPTH_8   = 8,
    E_MI_HDMI_BIT_DEPTH_16  = 16,
    E_MI_HDMI_BIT_DEPTH_18  = 18,
    E_MI_HDMI_BIT_DEPTH_20  = 20,
    E_MI_HDMI_BIT_DEPTH_24  = 24,
    E_MI_HDMI_BIT_DEPTH_32  = 32,
    E_MI_HDMI_BIT_DEPTH_MAX
} MI_HDMI_BitDepth_e;

typedef enum
{
    E_MI_HDMI_ACODE_PCM = 0,
    E_MI_HDMI_ACODE_NON_PCM,
    E_MI_HDMI_ACODE_MAX
} MI_HDMI_AudioCodeType_e;

typedef enum
{
    E_MI_HDMI_INFOFRAME_TYPE_AVI = 0,
    E_MI_HDMI_INFOFRAME_TYPE_SPD,
    E_MI_HDMI_INFOFRAME_TYPE_AUDIO,
    E_MI_HDMI_INFOFRAME_TYPE_MAX
} MI_HDMI_InfoFrameType_e;

typedef enum
{
    E_MI_HDMI_VIDEO_AFD_SameAsPictureAR = 8, // 1000
    E_MI_HDMI_VIDEO_AFD_4_3_Center = 9,      // 1001
    E_MI_HDMI_VIDEO_AFD_16_9_Center = 10,    // 1010
    E_MI_HDMI_VIDEO_AFD_14_9_Center = 11,    // 1011
    E_MI_HDMI_VIDEO_AFD_Others = 15,         // 0000~ 0111, 1100 ~ 1111
} MI_HDMI_VideoAfdRatio_e;

typedef enum
{
    E_MI_HDMI_AUDIO_CODING_REFER_STREAM_HEAD = 0,
    E_MI_HDMI_AUDIO_CODING_PCM,
    E_MI_HDMI_AUDIO_CODING_AC3,
    E_MI_HDMI_AUDIO_CODING_MPEG1,
    E_MI_HDMI_AUDIO_CODING_MP3,
    E_MI_HDMI_AUDIO_CODING_MPEG2,
    E_MI_HDMI_AUDIO_CODING_AAC,
    E_MI_HDMI_AUDIO_CODING_DTS,
    E_MI_HDMI_AUDIO_CODING_DDPLUS,
    E_MI_HDMI_AUDIO_CODING_MLP,
    E_MI_HDMI_AUDIO_CODING_WMA,
    E_MI_HDMI_AUDIO_CODING_MAX
} MI_HDMI_AudioCodingType_e;

typedef enum
{
    E_MI_HDMI_SCAN_INFO_NO_DATA = 0,             /**< No Scan information*/
    E_MI_HDMI_SCAN_INFO_OVERSCANNED,             /**< Scan information, Overscanned (for television) */
    E_MI_HDMI_SCAN_INFO_UNDERSCANNED,            /**< Scan information, Underscanned (for computer) */
    E_MI_HDMI_SCAN_INFO_FUTURE,
    E_MI_HDMI_SCAN_INFO_MAX
} MI_HDMI_ScanInfo_e;

typedef enum
{
    E_MI_HDMI_COLORIMETRY_NO_DATA = 0,
    E_MI_HDMI_COLORIMETRY_SMPTE170M,
    E_MI_HDMI_COLORIMETRY_ITUR709,
    E_MI_HDMI_COLORIMETRY_EXTEND,
    E_MI_HDMI_COLORIMETRY_MAX,
} MI_HDMI_Colorimetry_e;

typedef enum
{
    E_MI_HDMI_EXT_COLORIMETRY_XVYCC601 = 0,
    E_MI_HDMI_EXT_COLORIMETRY_XVYCC709,
    E_MI_HDMI_EXT_COLORIMETRY_SYCC601,
    E_MI_HDMI_EXT_COLORIMETRY_ADOBEYCC601,
    E_MI_HDMI_EXT_COLORIMETRY_ADOBERGB,
    E_MI_HDMI_EXT_COLORIMETRY_BT2020CYCC, //mapping to ext. colorimetry format BT2020Y'cC'bcC'rc
    E_MI_HDMI_EXT_COLORIMETRY_BT2020YCC, //mapping to ext. colorimetry format BT2020 RGB or YCbCr
    E_MI_HDMI_EXT_COLORIMETRY_BT2020RGB, //mapping to ext. colorimetry format BT2020 RGB or YCbCr
    E_MI_HDMI_EXT_COLORIMETRY_MAX,
} MI_HDMI_ExtColorimetry_e;

typedef enum
{
    E_MI_HDMI_ASPECT_RATIO_INVALID = 0,       /**< unknown aspect ratio */
    E_MI_HDMI_ASPECT_RATIO_4TO3,              /**< 4:3 */
    E_MI_HDMI_ASPECT_RATIO_16TO9,             /**< 16:9 */
    E_MI_HDMI_ASPECT_RATIO_21TO9,             /**< 21:9 */
    E_MI_HDMI_ASPECT_RATIO_MAX
} MI_HDMI_AspectRatio_e;

typedef enum
{
    E_MI_HDMI_YCC_QUANTIZATION_LIMITED_RANGE = 0,    /**< Limited quantization range of 220 levels when receiving a CE video format*/
    E_MI_HDMI_YCC_QUANTIZATION_FULL_RANGE,           /**< Full quantization range of 256 levels when receiving an IT video format*/
    E_MI_HDMI_YCC_QUANTIZATION_MAX
} MI_HDMI_YccQuantRange_e;

typedef enum
{
    E_MI_HDMI_ERR_UNSUPPORT_TIMING = MI_HDMI_INITIAL_ERROR_CODE,
    E_MI_HDMI_ERR_UNSUPPORT_COLORTYPE,
    E_MI_HDMI_ERR_UNSUPPORT_COLORDEPTH,
    E_MI_HDMI_ERR_UNSUPPORT_OUTPUTMODE,
    E_MI_HDMI_ERR_UNSUPPORT_ACODETYPE,
    E_MI_HDMI_ERR_UNSUPPORT_AFREQ,
    E_MI_HDMI_ERR_EDID_HEADER_ERR,
    E_MI_HDMI_ERR_EDID_DATA_ERR,
    E_MI_HDMI_ERR_EDID_PRASE_ERR,
    E_MI_HDMI_ERR_MAX
} MI_HDMI_ErrCode_e;

/*-------------------------------------------------------------------------------------------------
 * Defines
 ------------------------------------------------------------------------------------------------*/
#define MI_HDMI_MAX_ACAP_CNT 8
#define MI_HDMI_MAX_AUDIO_SAMPLE_RATE_CNT 10

/* HDMI Module ErrorCode */
#define MI_ERR_HDMI_INVALID_PARAM MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM)
#define MI_ERR_HDMI_INVALID_DEVID MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID)
#define MI_ERR_HDMI_DRV_FAILED MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED)
#define MI_ERR_HDMI_NOT_INIT MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_INIT)
#define MI_ERR_HDMI_NOT_SUPPORT MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT)
#define MI_ERR_HDMI_UNSUPPORT_TIMING MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_HDMI_ERR_UNSUPPORT_TIMING)
#define MI_ERR_HDMI_UNSUPPORT_COLORTYPE MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_HDMI_ERR_UNSUPPORT_COLORTYPE)
#define MI_ERR_HDMI_UNSUPPORT_ACODETYPE MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_HDMI_ERR_UNSUPPORT_ACODETYPE)
#define MI_ERR_HDMI_UNSUPPORT_AFREQ MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_HDMI_ERR_UNSUPPORT_AFREQ)
#define MI_ERR_HDMI_EDID_HEADER_ERR MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_HDMI_ERR_EDID_HEADER_ERR)
#define MI_ERR_HDMI_EDID_DATA_ERR MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_HDMI_ERR_EDID_DATA_ERR)
#define MI_ERR_HDMI_EDID_PRASE_ERR MI_DEF_ERR(E_MI_MODULE_ID_HDMI, E_MI_ERR_LEVEL_ERROR, E_MI_HDMI_ERR_EDID_PRASE_ERR)

/*-------------------------------------------------------------------------------------------------
 * Callback
 ------------------------------------------------------------------------------------------------*/
typedef MI_S32 (* MI_HDMI_EventCallBack)(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_EventType_e event, void *pEventParam, void *pUsrParam);

/*-------------------------------------------------------------------------------------------------
 * Structures
 ------------------------------------------------------------------------------------------------*/
typedef struct MI_HDMI_InitParam_s
{
    MI_HDMI_EventCallBack pfnHdmiEventCallback;
    void *pCallBackArgs;
} MI_HDMI_InitParam_t;

typedef struct MI_HDMI_VideoAttr_s
{
    MI_BOOL bEnableVideo;
    MI_HDMI_TimingType_e eTimingType;
    MI_HDMI_OutputMode_e eOutputMode;
    MI_HDMI_ColorType_e eColorType;
    MI_HDMI_DeepColor_e eDeepColorMode;
} MI_HDMI_VideoAttr_t;

typedef struct MI_HDMI_AudioAttr_s
{
    MI_BOOL bEnableAudio;
    MI_BOOL bIsMultiChannel;// 0->2channel 1->8channel
    MI_HDMI_SampleRate_e eSampleRate;
    MI_HDMI_BitDepth_e eBitDepth;
    MI_HDMI_AudioCodeType_e eCodeType;
} MI_HDMI_AudioAttr_t;

typedef struct MI_HDMI_EnInfoFrame_s
{
    MI_BOOL bEnableAviInfoFrame;
    MI_BOOL bEnableAudInfoFrame;
    MI_BOOL bEnableSpdInfoFrame;
} MI_HDMI_EnInfoFrame_t;

typedef struct MI_HDMI_Attr_s
{
    MI_BOOL bConnect;
    MI_HDMI_VideoAttr_t stVideoAttr;
    MI_HDMI_AudioAttr_t stAudioAttr;
    MI_HDMI_EnInfoFrame_t stEnInfoFrame;
} MI_HDMI_Attr_t;

typedef struct MI_HDMI_Edid_s
{
    MI_BOOL bEdidValid;
    MI_U32 u32Edidlength;
    MI_U8 au8Edid[512]; /* EDID buffer */
} MI_HDMI_Edid_t;

typedef struct MI_HDMI_Sink_Info_s
{
    MI_BOOL bConnected;
    MI_BOOL bSupportHdmi;
    MI_HDMI_TimingType_e eNativeTimingType;
    MI_BOOL abVideoFmtSupported[E_MI_HDMI_TIMING_MAX];
    MI_BOOL bSupportYCbCr444;
    MI_BOOL bSupportYCbCr422;
    MI_BOOL bSupportYCbCr;
    MI_BOOL bSupportxvYcc601;
    MI_BOOL bSupportxvYcc709;
    MI_U8   u8MdBit;
    MI_BOOL abAudioFmtSupported[MI_HDMI_MAX_ACAP_CNT];
    MI_U32  au32AudioSampleRateSupported[MI_HDMI_MAX_AUDIO_SAMPLE_RATE_CNT];
    MI_U32  u32MaxPcmChannels;
    MI_U8   u8Speaker;
    MI_U8   au8IdManufactureName[4];
    MI_U32  u32IdProductCode;
    MI_U32  u32IdSerialNumber;
    MI_U32  u32WeekOfManufacture;
    MI_U32  u32YearOfManufacture;
    MI_U8 u8Version;
    MI_U8 u8Revision;
    MI_U8 u8EdidExternBlockNum;
    MI_U8 au8IeeRegId[3];//IEEE registeration identifier
    MI_U8 u8PhyAddr_A;
    MI_U8 u8PhyAddr_B;
    MI_U8 u8PhyAddr_C;
    MI_U8 u8PhyAddr_D;
    MI_BOOL bSupportDviDual;
    MI_BOOL bSupportDeepColorYcbcr444;
    MI_BOOL bSupportDeepColor30Bit;
    MI_BOOL bSupportDeepColor36Bit;
    MI_BOOL bSupportDeepColor48Bit;
    MI_BOOL bSupportAi;
    MI_U32  u8MaxTmdsClock;
    MI_BOOL bILatencyFieldsPresent;
    MI_BOOL bLatencyFieldsPresent;
    MI_BOOL bHdmiVideoPresent;
    MI_U8 u8VideoLatency;
    MI_U8 u8AudioLatency;
    MI_U8 u8InterlacedVideoLatency;
    MI_U8 u8InterlacedAudioLatency;
} MI_HDMI_SinkInfo_t;

typedef struct MI_HDMIAviInforFrameVer_s
{
    MI_BOOL bEnableAfdOverWrite;
    MI_U8 A0Value;
    MI_HDMI_ColorType_e eColorType;
    MI_HDMI_Colorimetry_e eColorimetry;
    MI_HDMI_ExtColorimetry_e eExtColorimetry;
    MI_HDMI_YccQuantRange_e eYccQuantRange;
    MI_HDMI_TimingType_e eTimingType; //trans to MS_VIDEO_TIMING in impl
    MI_HDMI_VideoAfdRatio_e eAfdRatio;
    MI_HDMI_ScanInfo_e eScanInfo;
    MI_HDMI_AspectRatio_e eAspectRatio;
} MI_HDMI_AviInfoFrameVer_t;

typedef struct MI_HDMI_AudInfoFrameVer_s
{
    MI_U32 u32ChannelCount; //2 4 6 8 channels
    MI_HDMI_AudioCodeType_e eAudioCodeType;//PCM NON-PCM
    MI_HDMI_SampleRate_e eSampleRate;
} MI_HDMI_AudInfoFrameVer_t;

typedef struct MI_HDMI_SpdInfoFrame_s
{
    MI_U8 au8VendorName[8];
    MI_U8 au8ProductDescription[16];
} MI_HDMI_SpdInfoFrame_t;

typedef union
{
    MI_HDMI_AviInfoFrameVer_t     stAviInfoFrame;
    MI_HDMI_AudInfoFrameVer_t     stAudInfoFrame;
    MI_HDMI_SpdInfoFrame_t        stSpdInfoFrame;
} MI_HDMI_InfoFrameUnit_u;

typedef struct MI_HDMI_InfoFrame_s
{
    MI_HDMI_InfoFrameType_e eInfoFrameType; /* InfoFrame type */
    MI_HDMI_InfoFrameUnit_u unInforUnit;    /* InfoFrame date */
} MI_HDMI_InfoFrame_t;

typedef struct  MI_HDMI_AnalogDrvCurrent_s
{
    MI_U8 u8DrvCurTap1Ch0;
    MI_U8 u8DrvCurTap1Ch1;
    MI_U8 u8DrvCurTap1Ch2;
    MI_U8 u8DrvCurTap1Ch3;
    MI_U8 u8DrvCurTap2Ch0;
    MI_U8 u8DrvCurTap2Ch1;
    MI_U8 u8DrvCurTap2Ch2;
    MI_U8 u8DrvCurTap2Ch3;
} MI_HDMI_AnalogDrvCurrent_t;

#ifdef __cplusplus
}
#endif
#endif //_MI_HDMI_DATATYPE_H_
