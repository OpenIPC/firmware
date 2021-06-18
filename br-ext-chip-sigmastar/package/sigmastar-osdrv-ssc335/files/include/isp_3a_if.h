/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef ISP_3A_IF_H
#define ISP_3A_IF_H

#ifdef __cplusplus
extern "C"
{
#endif

//#include <linux/kernel.h>
#ifdef __USE_USERSPACE_3A__
#include "ispalgo_types.h"
#include "sigma_isp_general.h"
#else
#include <drv_ms_isp_general.h>
#include <drv_ms_rwcfg.h>
#include <hal_isp_shadow.h>
#include <drv_isp_shadow.h>
#include <cam_os_wrapper.h>
#include "libcamera_iq_struct.h"
#include "libcamera_error_code.h"
#include "iq_apiset.h"
#endif

#include "mi_isp_3a_datatype.h"

//#define ENABLE_DOUBLE_OPT           (0)

#define ENABLE_GET_PARAM_FORM_CFG   (1)

typedef void* IqCfgHandle; //debug buffer handle
typedef void* AeHandle;
typedef void* AwbHandle;
typedef void* AfHandle;

/******************* IQ_DEBUG_Mode ********************/
typedef enum
{
    DEBUG_ENABLE_IQ,     /**< Stop IQ update */
    DEBUG_LEVEL,         /**< enable show log of iq.debug.blkid */
    DEBUG_BLKID,         /**< show log for each IP */
    DEBUG_PARALEV,       /**< parameter level: SUB(0)/DBG(1)/HAL(2) */
    DEBUG_INDSHOW,       /**< show GainCheck result */
    DEBUG_TBLSCAL,       /**< set table scale for debug show (def: 16, only work for 8, 4, 2, 1)*/
    DEBUG_SETPROPEN,     /**< replace function by setprop (works by follow mark with $$) */
    DEBUG_ISOFIX,        /**< ($$)fix iso to present index */
    DEBUG_ISOIND,        /**< ($$)assign isoind under fix iso mode */
    DEBUG_IQ_TRIGGER,   /**< ($$)Force Trigger IQ by mod and blkid */
    DEBUG_IQ_TRIGMOD,   /**< ($$)Force Trigger IQ by mod and blkid */
    DEBUG_AUTOIQ,       /**< on/off autoiq (def:on) */
    DEBUG_SKIPCFGVC,    /**< skip cfg major version check */

    DEBUG_APISHOW,       /**< show debug message of API */
    DEBUG_APIRESET,      /**< reset all API setting of OFF */

    DEBUG_SKIPML,

    //---[WDR_GBL, WDR_LOC]
    DEBUG_FEATURE_LEVEL,
    DEBUG_DEFOG_ENABLE,
    DEBUG_DEFOG_MODE,
    DEBUG_DEFOG_AUTO_LEVEL,
    DEBUG_DEFOG_MANUAL_LEVEL,
    DEBUG_DRC_ENABLE,
    DEBUG_DRC_WHITE_LEVEL,
    DEBUG_DRC_BLACK_LEVEL,
    DEBUG_DRC_ASYMMETRY,
    DEBUG_DRC_BRIGNT_ENHANCE,
    DEBUG_DRC_EMODE,
    DEBUG_DRC_MANUAL_LEVEL,
    DEBUG_DRC_AUTO_LEVEL,
    DEBUG_LOCWDR_TblShow,
    DEBUG_LOCWDR_TblSmooth,


    //---[RGBIR]
    DEBUG_IRIDX_THRD_0,
    DEBUG_IRIDX_THRD_1,
    DEBUG_IRIDX_THRD_2,
    DEBUG_IRIDX_THRD_3,
    DEBUG_IRIDX_THRD_4,

    //---[MEMALLOC]
    DEBUG_NLM_MEMALLOC_EN,
    DEBUG_HIST_MEMALLOC_EN,

    DBG_PROP_MAX,
} DBG_PROP;

typedef struct
{
    //---[Enable IQ]
    MS_S32 IQ_DEBUG_ENABLE_IQ;     /**< Stop IQ update */
    MS_S32 IQ_DEBUG_LEVEL;         /**< enable show log of iq.debug.blkid */
    MS_S32 IQ_DEBUG_BLKID;         /**< show log for each IP */
    MS_S32 IQ_DEBUG_PARALEV;       /**< parameter level: SUB(0)/DBG(1)/HAL(2) */
    MS_S32 IQ_DEBUG_INDSHOW;       /**< show GainCheck result */
    MS_S32 IQ_DEBUG_TBLSCAL;       /**< set table scale for debug show (def: 16, only work for 8, 4, 2, 1)*/
    MS_S32 IQ_DEBUG_SETPROPEN;     /**< replace function by setprop (works by follow mark with $$) */
    MS_S32 IQ_DEBUG_ISOFIX;        /**< ($$)fix iso to present index */
    MS_S32 IQ_DEBUG_ISOIND;        /**< ($$)assign isoind under fix iso mode */
    MS_S32 IQ_DEBUG_IQ_TRIGGER;   /**< ($$)Force Trigger IQ by mod and blkid */
    MS_S32 IQ_DEBUG_IQ_TRIGMOD;   /**< ($$)Force Trigger IQ by mod and blkid */
    MS_S32 IQ_DEBUG_AUTOIQ;       /**< on/off autoiq (def:on) */
    MS_S32 IQ_DEBUG_SKIPCFGVC;    /**< skip cfg major version check */
    //---[API]
    MS_S32  IQ_DEBUG_APISHOW;      /**< show debug message of API */
    MS_S32  IQ_DEBUG_APIRESET;     /**< reset all API setting of OFF */
    //---[MLoad]
    MS_S32  IQ_DEBUG_SKIPML;
    //---[WDR_GBL, WDR_LOC]
    MS_S32  IQ_DEBUG_FEATURE_LEVEL;
    MS_S32  IQ_DEBUG_DEFOG_ENABLE;
    MS_S32 IQ_DEBUG_DEFOG_MODE;
    MS_S32 IQ_DEBUG_DEFOG_AUTO_LEVEL;
    MS_S32 IQ_DEBUG_DEFOG_MANUAL_LEVEL;
    MS_S32  IQ_DEBUG_DRC_ENABLE;
    MS_S32 IQ_DEBUG_DRC_WHITE_LEVEL;
    MS_S32 IQ_DEBUG_DRC_BLACK_LEVEL;
    MS_S32 IQ_DEBUG_DRC_ASYMMETRY;
    MS_S32 IQ_DEBUG_DRC_BRIGNT_ENHANCE;
    MS_S32 IQ_DEBUG_DRC_EMODE;
    MS_S32 IQ_DEBUG_DRC_MANUAL_LEVEL;
    MS_S32 IQ_DEBUG_DRC_AUTO_LEVEL;
    MS_S32  IQ_DEBUG_LOCWDR_TblShow;
    MS_U16  IQ_DEBUG_LOCWDR_TblSmooth;
    //---[RGBIR]
    MS_S32 IQ_DEBUG_IRIDX_THRD_0;
    MS_S32 IQ_DEBUG_IRIDX_THRD_1;
    MS_S32 IQ_DEBUG_IRIDX_THRD_2;
    MS_S32 IQ_DEBUG_IRIDX_THRD_3;
    MS_S32 IQ_DEBUG_IRIDX_THRD_4;
    //---[Memalloc]
    MS_S32  IQ_DEBUG_NLM_MEMALLOC_EN;
    MS_S32  IQ_DEBUG_HIST_MEMALLOC_EN;

} IQDEBUG_t;

typedef struct
{
    //---[Enable AE]
    MS_S32 AE_DEBUG_ENABLE_AE;          /**< Enable/Disable AE */
    MS_S32 AE_DEBUG_LEVEL;              /**< 0: disable, 1: Exposure 2: Algo parameters 4: Algo statistics 32: API debug */

} AEDEBUG_t;

typedef struct
{
    //---[Enable AWB]
    MS_S32 AWB_DEBUG_AWB_ENABLE;        /**< Enable/Disable AWB */
    MS_S32 AWB_DEBUG_MWB_ENABLE;        /**< Enable/Disable MWB */
    MS_S32 AWB_DEBUG_MWB_RGAIN;         /**< MWB Rgain */
    MS_S32 AWB_DEBUG_MWB_BGAIN;         /**< MWB Bgain */
    MS_S32 AWB_DEBUG_LEVEL;             /**< AWB Debug Mode: 1:show simple algo paramaters 2:always show detail algo parameters 3:show detail algo parameters 7:User Paramters*/

} AWBDEBUG_t;

/******************* AE *********************/
#define isp_3A_ROW          128   /**< number of 3A statistic blocks in a row */
#define isp_3A_COL          90    /**< number of 3A statistic blocks in a column */
#define isp_3A_HIST_BIN0    40    /**< histogram type0 resolution*/
#define isp_3A_HIST_BINX    128   /**< histogram type1 resolution*/
#define isp_3A_IR_HIST_BIN  256   /**< histogram type2 resolution*/
#define isp_DLC_HISTOGRAM_REPORT_NUM    8 /**< histogram type1 resolution(DRC)*/

typedef struct
{
    MS_U8  r;
    MS_U8  g;
    MS_U8  b;
    MS_U8  y;
} __attribute__((packed, aligned(1))) ISPAESample_t;

typedef struct
{
    MS_U16 u2HistY[isp_3A_HIST_BINX];
} __attribute__((packed, aligned(1))) ISPHistX_t;

typedef struct {
    MS_U16 u2IRHist[isp_3A_IR_HIST_BIN];
} __attribute__((packed, aligned(1)))ISPHistIR_t;

typedef enum
{
    AE_HDR_DISABLE = 0,
    AE_HDR_SEPARATE_GAIN_SHUTTER,
    AE_HDR_SHARE_GAIN_SEPARATE_SHUTTER,
    AE_HDR_SHARE_SHUTTER_SEPARATE_GAIN,
    AE_HDR_MODE_MAX
} AE_HDR_MODE;

typedef struct
{
    char sensor_id[32];             /**< sensor module id*/
    MS_U16 FNx10;                   /**< F number * 10*/
    MS_U16 shutter_min;             /**< shutter Shutter min us*/
    MS_U16 sensor_gain_min;         /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MS_U16 shutterHDRShort_min;     /**< shutter Shutter min us*/
    MS_U16 sensor_gainHDRShort_min; /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MS_U32 Size;                    /**< struct size*/
    MS_U32 fps;                     /**< initial frame per second*/
    MS_U32 shutter;                 /**< shutter Shutter in ns*/
    MS_U32 shutter_step;            /**< shutter Shutter step ns*/
    MS_U32 shutterHDRShort_step;    /**< shutter Shutter step ns*/
    MS_U32 sensor_gain;             /**< sensor_gain Sensor gain, 1X = 1024*/
    MS_U32 isp_gain;                /**< isp_gain Isp digital gain , 1X = 1024 */
    MS_U32 isp_gain_max;            /**< isp_gain Maximum Isp digital gain , 1X = 1024 */
    MS_U32 shutter_max;             /**< shutter Shutter max us*/
    MS_U32 sensor_gain_max;         /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    MS_U32 shutterHDRShort_max;     /**< shutter Shutter max us*/
    MS_U32 sensor_gainHDRShort_max; /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
}ISPAEInitParam_t,AeInitParam_t;

/*! @brief ISP report to AE, hardware statistic */
typedef struct
{
    MS_U32 Size;            /**< struct size*/
    ISPHistX_t *hist1;      /**< HW statistic histogram 1*/
    ISPHistX_t *hist2;      /**< HW statistic histogram 2*/
    ISPHistIR_t *histIR;    /**< HW statistic histogram IR*/
    MS_U32 AvgBlkX;         /**< HW statistics average block number*/
    MS_U32 AvgBlkY;         /**< HW statistics average block number*/
    ISPAESample_t *avgs;    /**< HW statistics average block data*/
    ISPAESample_t *avgs1;   /**< HW statistics average block data*/
    MS_U16 FNx10;           /**< Current Aperture in FNx10*/
    MS_U32 Shutter;         /**< Current shutter in ns*/
    MS_U32 SensorGain;      /**< Current Sensor gain, 1X = 1024 */
    MS_U32 IspGain;         /**< Current ISP gain, 1X = 1024*/
    MS_U32 ShutterHDR;      /**< Shutter in ns */
    MS_U32 SensorGainHDR;   /**< Sensor gain, 1X = 1024 */
    MS_U32 IspGainHDR;      /**< ISP gain, 1X = 1024 */
    MS_U32 PreCurYx10;
    MS_U32 PreAvgYx10;
    MS_U32 PreCurY1x10;
    MS_U32 PreAvgY1x10;
    MS_U8  HDRMode;         /**< 0 = Separate shutter/sensor gain settings; */
                            /**< 1 = Separate shutter & Share sensor gain settings */
    AEDEBUG_t *tAEDebug;    /**< AE debug -> unit test*/
    MS_U32 CurFPS;
} ISPAEInfo_t, AeInput_t;

/*! @brief ISP ae algorithm result*/
typedef struct
{
    MS_U32 Size;            /**< struct size*/
    MS_U32 Change;          /**< if true, apply this result to hw register*/
    MS_U32 Shutter;         /**< Shutter in ns */
    MS_U32 SensorGain;      /**< Sensor gain, 1X = 1024 */
    MS_U32 IspGain;         /**< ISP gain, 1X = 1024 */
    MS_U32 ShutterHDR;      /**< Shutter in ns */
    MS_U32 SensorGainHDR;   /**< Sensor gain, 1X = 1024 */
    MS_U32 IspGainHDR;      /**< ISP gain, 1X = 1024 */
    MS_S32 i4BVx16384;      /**< Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    MS_U32 CurYx10;         /**< frame brightness with weighting*/
    MS_U32 AvgYx10;         /**< frame brightness */
    MS_U32 CurY1x10;        /**< frame brightness with weighting*/
    MS_U32 AvgY1x10;        /**< frame brightness */
    MS_U32 SceneTargetx10;  /**< Current AE target for CurY */
    MS_CAM_BOOL IsStable;
    MS_CAM_BOOL IsReachBoundary;
    MS_U32 AeHdr_Num_BV[17];
    MS_U32 DebandFPS;
    MS_U16 FNx10;           /**< Aperture in FNx10*/
    MS_U8  HistMaxY;        /**< HistMaxY*/
    MS_U8  Reserved;        /**< Reserved*/
    MS_U16 GMBlendRatio;    /**< Adaptive Gamma Blending Ratio from AE**/
}__attribute__((packed, aligned(4))) ISPAEResult_t, AeOutput_t;

/*! @brief AE library initialization
    @param[in] CfgHandle IQ data handle
    @retval SUCCESS or FAIL if error occurs.
*/
AeHandle AeInit(ISP_DBGBUF_HANDLE CfgHandle, AeInitParam_t *init_param, AeInput_t *pInput, AeOutput_t *pOutput);

/*! @brief Release resources that allocated at AeInit
    @param[in] IQ data handle
    @retval SUCCESS or FAIL if error occurs.
*/
MS_S32 AeRelease(AeHandle Handle);

/*! @brief AE algorithm entry
    @param[in] handle AE Algorithm private data
    @param[in] pInput AE algorithm input.
@param[in] pDbgBuf Debug buffer handle.
    @param[out] pOutput AE algorithm result.
    @retval SUCCESS or FAIL if error occurs.
*/
MS_S32 DoAe(AeHandle Handle, AeInput_t *pInput, AeOutput_t *pOutput);

MS_S32 AeInitParamConversion(AeHandle Handle, AeInitParam_t *init_param);

/******************* AWB ********************/
typedef enum
{
    AWB_MODE_NORMAL,
    AWB_MODE_HDR,
    AWB_MODE_MAX
} AWBMode_e;

/*! @brief Input parameters of function DoAWB()*/
typedef struct
{
    MS_U32          nSize;            /**< Size of DoAwbInput_t */
    MS_U32          nFrameCount;      /**< Frame counts */
    MS_S32          nBV;              /**< Bright value from DoAE() result */
    MS_S32          CurYx10;
    MS_U8           nColorToGrayFlag; /**< ColorToGray Flag From IQ for AWB->MWB(x1 gain) */
    MS_U8           nGrayToColorFlag; /**< GrayToColor Flag From IQ for MWB->AWB */
    MS_U8           HDRMode;          /**< Noramal or HDR mode*/
    MS_U16          InitRgain;        /**< Init Rgain from Wifi */
    MS_U16          InitGgain;        /**< Init Ggain from Wifi */
    MS_U16          InitBgain;        /**< Init Bgain from Wifi */
    IspCropWin_t    tCrop;            /**< AWB statistic windows parameter */
    IspAwbWin_t     tBlock;           /**< AWB statistic block parameter */
    IspAwbStatis_t  *pAwbStatis;      /**< AWB statistic data */
    IspAwbStatis_t  *pAwbStatisShort; /**< Short Shutter AWB statistic data */
    AWBDEBUG_t      *tAWBDebug;       /**< AWB debug -> unit test*/
}AwbInput_t;

/*! @brief Output of function DoAWB()*/
typedef struct
{
    MS_U32          nSize;                  /** Size of DoAwbOutput_t */
    IspCropWin_t    tCrop;                  /**< New AWB statistic windows settings for next frame */
    IspAwbWin_t     tBlock;                 /**< New AWB statistic block settings for next frame */
    IspAwbGain_t    tGain;                  /**< AWB gain */
    MS_U16          nColorTmp;              /**< Color temperature */
    MS_CAM_BOOL     nMultiLS_CaliCCMUpdate; /**< Color calibration CCM update flag for multi-lightsource*/
    MS_U16          nMultiLS_CaliCCM[9];    /**< Color calibration CCM for multi-lightsource*/
}AwbOutput_t;


/*! @brief AWB algorithm entry
    @param[in] handle AWB Algorithm private data
    @param[in] AWB HW statistics
    @param[out] pOutput AWB algorithm result.
    @retval SUCCESS or FAIL if error occurs.
*/
MS_S32 DoAwb(AwbHandle Handle, AwbInput_t *pInput, AwbOutput_t *pOutput);

/*! @brief AWB library initialization
    @param[in] CfgHandle IQ data handle
    @retval SUCCESS or FAIL if error occurs.
*/
AwbHandle AwbInit(ISP_DBGBUF_HANDLE CfgHandle, AwbInput_t *pInput, AwbOutput_t *pOutput);

/*! @brief Release resources that allocated at AwbInit
    @param[in] IQ data handle
    @retval SUCCESS or FAIL if error occurs.
*/
MS_S32 AwbRelease(AwbHandle Handle);


/******************* IQ ********************/
/*! @brief Calculate IQ for current condition then output the ISP setting to shadow register buffer
    @param[in] pHandle IQ handle
    @param[out] pRegShd register shadow buffer
    @retval SUCCESS or FAIL if error occurs.
*/
/*! @brief Input of function DoIQ()*/
#ifndef __USE_USERSPACE_3A__

//

typedef struct
{
    MS_U16 dummy0;
    MS_U16 dummy1;
    MS_U16 dummy2;
    MS_U16 dummy3;
} IQNoiseEst_t;

typedef struct
{
    MS_U32 acc;
    MS_U16 n;
    MS_U32 rstracc;
    MS_U16 rstrn;
} IQWDRDefog_t;

typedef struct
{
    MS_U16 nalignGain[16];
} IQHDREst_t;

typedef enum
{
    IQ_MODE_NORMAL,
    IQ_MODE_HDR,
    IQ_MODE_HDR_EMBED,
    IQ_MODE_RGBIR,
    IQ_MODE_MAX
} IQMode_e;

typedef enum __attribute__ ((aligned (4)))   //IQ Trigger:
{
    IQ_PA_KEEP,
    IQ_PA_ALL, //all blocks update
    IQ_PA_ISO, //blocks change by ISO
    IQ_PA_CCT, //blocks change by CCT
    IQ_PA_SEL, //single block update
    IQ_PA_API, //blocks change by API
    IQ_PA_INIT,//init blocks update
    IQ_PA_MISS,//CFG parse fail make IND missing
    IQ_PA_SKIP,
    IQ_PA_MAX,
} IQ_PARA_CODE;

typedef struct
{
    MS_U32 nSize;               /** Size of IqInput_t **/
    MS_U32 nFrameCount;         /**< frame count*/
    MS_S32 nBV;                 /**< Bright value from DoAE() result */
    MS_U32 nIspGain;            /** ISP gain **/
    MS_U32 nIspGainHDR;         /** ISP gain P1 **/
    MS_U32 nTotalGain;          /** TotalGain = ISP gain + Sensor gain **/
    MS_S32 nChipTemp;           /** Chip temperature **/
    MS_S32 nSensorTemp;         /** Sensor temperature **/
    MS_U32 nColorTemp;          /** Color temperature **/
    MS_U32 nAwbRx1024;          /** Awb R gain 1024 base **/
    MS_U32 nAwbGx1024;          /** Awb G gain 1024 base **/
    MS_U32 nAwbBx1024;          /** Awb B gain 1024 base **/
    MS_U32 nWdrRatiox1024;      /** Wdr ratio 1024 base **/
    MS_U32 nIrindex;            /** RGBIR index **/
    MS_U32 nCropSizeW;          /** Image width after crop **/
    MS_U32 nCropSizeH;          /** Image height after crop **/
    MS_U32 nAvgBlkX;            /**< HW statistics average block number*/
    MS_U32 nAvgBlkY;            /**< HW statistics average block number*/
    MS_U16 nDegree;             /**< Sensor degree **/
    MS_U16 nGMBlendRatio;       /**< Adaptive Gamma Blending Ratio from AE**/
    IQMode_e       nMode;       /**< IQ Mode **/
    ISPAESample_t *tAvgs;       /**< HW statistics average block data*/
    IQDEBUG_t     *tIQDebug;    /**< IQ debug -> unit test*/
    ISP_HDR_STATS *pHdrStats;   /**< HDR statistic */
    MS_CAM_BOOL    nMultiLS_CaliCCMUpdate;  /**< Color calibration CCM update flag for multi-lightsource*/
    MS_U16         nMultiLS_CaliCCM[9];     /**< Color calibration CCM for multi-lightsource*/
    IQNoiseEst_t  *pNE0;        /**< Noise Est*/
    IQWDRDefog_t  *pDefog;
    IQHDREst_t    *pHDRDyn;     /**< HDR Dynamic Ratio*/
}IqInput_t;

typedef struct
{
    DrvIspShd_t   tIsp;
    DrvSclShd_t   tScl;
    DrvMloadShd_t tMload;
    MS_U8         nColorToGrayFlag; /**< ColorToGray Flag for AWB->MWB **/
    MS_U8         nGrayToColorFlag; /**< ColorToGray Flag for MWB->AWB **/
    MS_U16        u16HDRExpRatio;
}IqOutput_t;

/*! @brief IQ algorithm entry
    @param[in] handle IQ Algorithm private data
    @param[in] IQ HW statistics
    @param[out] pOutput IQ algorithm result.
    @retval SUCCESS or FAIL if error occurs.
*/
MS_S32 DoIq(IqCfgHandle pHandle, IqInput_t *input_param, IqOutput_t *output_param);
MS_S32 DoIqPerFrame(IqCfgHandle pHandle, IqInput_t *input_param, IqOutput_t *output_param);

/*! @brief IQ library initialization
    @param[in] CfgHandle IQ data handle
    @retval SUCCESS or FAIL if error occurs.
*/
IqCfgHandle IqInit(ISP_DBGBUF_HANDLE pIqDbgBuf, IqInput_t *input_param, IqOutput_t *output_param);

/*! @brief IQ resources that allocated at IQInit
    @param[in] IQ data handle
    @retval SUCCESS or FAIL if error occurs.
*/
MS_S32 IqRelease(IqCfgHandle Handle);

int IspAlgoReadIqBinH(void* pIqBuf, int nIqSize);

typedef struct
{
    AeInput_t   ae;
    AwbInput_t  awb;
    IqInput_t  iq;
}IspAlgoInputHandle;

#endif
/************************************* IQ API STRUCT START *************************************/

#if 1 //unused struct

typedef struct
{
    MS_U16 u2BlkSize_x;
    MS_U16 u2BlkSize_y;
    MS_U16 u2BlkNum_x;
    MS_U16 u2BlkNum_y;
} MS_AE_WIN;

typedef struct
{
    MS_U16 u2BlkSize_x;
    MS_U16 u2BlkSize_y;
    MS_U16 u2BlkNum_x;
    MS_U16 u2BlkNum_y;
} MS_AWB_WIN;

typedef struct
{
    MS_U8 r;
    MS_U8 g;
    MS_U8 b;
    MS_U8 y;
} __attribute__((packed, aligned(1))) AE_SAMPLE;

typedef struct
{
    AE_SAMPLE ae_data[128 * 90];
} __attribute__((packed, aligned(1))) MS_CAM_AE_STATIS;

typedef enum
{
    MS_ExposureControlOff = 0,
    MS_ExposureControlAuto,
    MS_ExposureControlNight,
    MS_ExposureControlBackLight,
    MS_ExposureControlSpotLight,
    MS_ExposureControlSports,
    MS_ExposureControlSnow,
    MS_ExposureControlBeach,
    MS_ExposureControlLargeAperture,
    MS_ExposureControlSmallApperture,
    MS_ExposureControlKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    MS_ExposureControlVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    MS_ExposureControlMax = 0x7FFFFFFF
} MS_CAM_AE_EXPOSURECONTROLTYPE;

typedef struct
{
    MS_U8 u8DebugLevel;
} MS_CAM_AE_DEBUG_LEVEL;

typedef enum __attribute__ ((aligned (1))) //MS_AWB_CONTROL_MODE_
{
    MS_AWB_CONTROL_OFF = 0,
    MS_AWB_CONTROL_AUTO,
    MS_AWB_CONTROL_SUNLIGHT,        // Referenced in JSR-234
    MS_AWB_CONTROL_CLOUDY,
    MS_AWB_CONTROL_SHADE,
    MS_AWB_CONTROL_TUNGSTEN,
    MS_AWB_CONTROL_FLUORESCENT,
    MS_AWB_CONTROL_INCANDESCENT,
    MS_AWB_CONTROL_FLASH,           // Optimal for device's integrated flash
    MS_AWB_CONTROL_HORIZON,
    MS_AWB_CONTROL_NUM
} MS_WHITEBALCONTROLTYPE;

typedef struct MS_CAM_WHITEBALCONTROLTYPE
{
    MS_WHITEBALCONTROLTYPE eWhiteBalControl;
    //AWB_SCENE eWhiteBalControl;
} MS_CAM_WHITEBALCONTROLTYPE;

#endif

#if 1 //export api
#ifndef __USE_USERSPACE_3A__
MS_S32 DrvAlgo_IF_IQ_SetMirrorFlip(IqCfgHandle hIq, MS_CAM_MIRRORFLIPTYPE *MirrorFlip);
MS_S32 DrvAlgo_IF_IQ_ForceTrigIQ(IqCfgHandle hIq, IQ_PARA_CODE mode, int block);
#endif //#ifdef __USE_KERNEL_3A__
MS_S32 DrvAlgo_IF_AE_SetFlicker(AeHandle hAe, MI_ISP_AE_FLICKER_TYPE_e *eFlicker);

MS_S32 DrvAlgo_IF_ApiSet(MS_U32 nCtlID, AeHandle pAEHandle, AwbHandle pAWBHandle, AfHandle pAFHandle, IqCfgHandle pIQHandle, MS_U32 size, const void* pData);
MS_S32 DrvAlgo_IF_ApiGet(MS_U32 nCtlID, AeHandle pAEHandle, AwbHandle pAWBHandle, AfHandle pAFHandle, IqCfgHandle pIQHandle, MS_U32 size, const void* pData);
#endif

#ifdef __USE_USERSPACE_3A__
int IspLoadIqCfg(ISP_DBGBUF_HANDLE *pHandle, void *pCfg, unsigned int Len);
void IspFreeIqCfg(ISP_DBGBUF_HANDLE *pHandle);
#endif

#ifdef __cplusplus
}
#endif

#endif
