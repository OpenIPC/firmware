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

/*
*   mi_isp_datatype.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _MI_ISP_DATATYPE_H_
#define _MI_ISP_DATATYPE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mi_common.h"
#include "mi_sys_datatype.h"
#include "mi_isp_3a_datatype.h"
#include "mi_isp_iq_datatype.h"

/************************************* AF  - Define the structure declaration - START *************************************/

/************************************* AF  - Define the structure declaration - END   *************************************/

/************************** Cus3A ***********************/

typedef struct
{
    MI_U8 uAvgR;
    MI_U8 uAvgG;
    MI_U8 uAvgB;
    MI_U8 uAvgY;
} MI_ISP_AE_AVGS;

typedef struct
{
    MI_U32 nBlkX;
    MI_U32 nBlkY;
    MI_ISP_AE_AVGS nAvg[128*90];
}MI_ISP_AE_HW_STATISTICS_t;

typedef struct
{
    MI_U8 uAvgR;
    MI_U8 uAvgG;
    MI_U8 uAvgB;
} MI_ISP_AWB_AVGS;

typedef struct
{
    MI_U32 nBlkX;
    MI_U32 nBlkY;
    MI_ISP_AWB_AVGS nAvg[128*90];
} MI_ISP_AWB_HW_STATISTICS_t;

typedef struct
{
    MI_U16 nHisto[128];
} MI_ISP_HISTO_HW_STATISTICS_t;

typedef struct
{
    MI_U16 nHisto[256];
} MI_ISP_RGBIR_HISTO_HW_STATISTICS_t;

typedef struct
{
    MI_BOOL bAE;
    MI_BOOL bAWB;
    MI_BOOL bAF;
}Cus3AEnable_t;

typedef struct
{
    MI_U32 Size;                   /**< struct size*/
    char sensor_id[32];            /**< sensor module id*/
    MI_U32 shutter;                /**< shutter Shutter in ns*/
    MI_U32 shutter_step;           /**< shutter Shutter step ns*/
    MI_U32 shutter_min;            /**< shutter Shutter min us*/
    MI_U32 shutter_max;            /**< shutter Shutter max us*/
    MI_U32 sensor_gain;            /**< sensor_gain Sensor gain, 1X = 1024*/
    MI_U32 sensor_gain_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MI_U32 sensor_gain_max;        /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    MI_U32 isp_gain;               /**< isp_gain Isp digital gain , 1X = 1024 */
    MI_U32 isp_gain_max;           /**< isp_gain Maximum Isp digital gain , 1X = 1024 */
    MI_U32 FNx10;                  /**< F number * 10*/
    MI_U32 fps;                    /**< initial frame per second*/
    MI_U32 shutterHDRShort_step;           /**< shutter Shutter step ns*/
    MI_U32 shutterHDRShort_min;            /**< shutter Shutter min us*/
    MI_U32 shutterHDRShort_max;            /**< shutter Shutter max us*/
    MI_U32 sensor_gainHDRShort_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MI_U32 sensor_gainHDRShort_max;        /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    /*CUS3A v1.1*/
    MI_U32 AvgBlkX;                 /**< HW statistics average block number*/
    MI_U32 AvgBlkY;                 /**< HW statistics average block number*/
}CusAEInitParam_t,CusAeInitParam_t;

/*! @brief ISP report to AE, hardware statistic */
typedef struct
{
    MI_U32 Size;       /**< struct size*/
    void   *hist1;   /**< HW statistic histogram 1*/
    void   *hist2;   /**< HW statistic histogram 2*/
    MI_U32 AvgBlkX;  /**< HW statistics average block number*/
    MI_U32 AvgBlkY;  /**< HW statistics average block number*/
    void * avgs; /**< HW statistics average block data*/
    MI_U32 Shutter;                    /**< Current shutter in ns*/
    MI_U32 SensorGain;                 /**< Current Sensor gain, 1X = 1024 */
    MI_U32 IspGain;                    /**< Current ISP gain, 1X = 1024*/
    MI_U32 ShutterHDRShort;           /**< Current shutter in ns*/
    MI_U32 SensorGainHDRShort;        /**< Current Sensor gain, 1X = 1024 */
    MI_U32 IspGainHDRShort;           /**< Current ISP gain, 1X = 1024*/
    /*CUS3A V1.1*/
    MI_U32 PreAvgY;                   /**< Previous frame brightness*/
    MI_U8  HDRCtlMode;                /**< 0 = HDR off; */
                                      /**< 1 = Separate shutter & Separate sensor gain settings */
                                      /**< 2 = Separate shutter & Share sensor gain settings */
                                      /**< 3 = Share shutter & Separate sensor gain settings */
    MI_U32 FNx10;                     /**< Aperture in FNx10*/
    MI_U32 CurFPS;                    /**Current sensor FPS */
    MI_U32 PreWeightY;                /**< Previous frame brightness with ROI weight*/
} __attribute__((packed, aligned(1))) CusAEInfo_t, CusAeInput_t;

/*! @brief ISP ae algorithm result*/
typedef struct
{
    MI_U32 Size;           /**< struct size*/
    MI_U32 Change;         /**< if true, apply this result to hw register*/
    MI_U32 Shutter;         /**< Shutter in ns */
    MI_U32 SensorGain;      /**< Sensor gain, 1X = 1024 */
    MI_U32 IspGain;         /**< ISP gain, 1X = 1024 */
    MI_U32 ShutterHdrShort;     /**< Shutter in ns */
    MI_U32 SensorGainHdrShort;  /**< Sensor gain, 1X = 1024 */
    MI_U32 IspGainHdrShort;     /**< ISP gain, 1X = 1024 */
    MI_U32 u4BVx16384;      /**< Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    MI_U32 AvgY;            /**< frame brightness */
    MI_U32 HdrRatio;   /**< hdr ratio, 1X = 1024 */
    /*CUS3A V1.1*/
    MI_U32 FNx10;                     /**< F number * 10*/
    MI_U32 DebandFPS;       /** Target fps when running auto debanding**/
    MI_U32 WeightY;                /**< frame brightness with ROI weight*/
}__attribute__((packed, aligned(1))) CusAEResult_t, CusAeOutput_t;

/*! @brief AWB HW statistics data*/
typedef struct
{
    MI_U32 Size;           /**< struct size*/
    MI_U32 AvgBlkX;
    MI_U32 AvgBlkY;
    MI_U32 CurRGain;
    MI_U32 CurGGain;
    MI_U32 CurBGain;
    void *avgs; //ISP_AWB_SAMPLE
    /*CUS3A V1.1*/
    MI_U8  HDRMode;          /**< Noramal or HDR mode*/
    void*  *pAwbStatisShort; /**< Short Shutter AWB statistic data */
    MI_U32 u4BVx16384;      /**< From AE output, Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    MI_S32 WeightY;         /**< frame brightness with ROI weight*/
}__attribute__((packed, aligned(1))) CusAWBInfo_t, CusAWBInput_t;

/*! @brief AWB algorithm result*/
typedef struct
{
    MI_U32 Size;       /**< struct size*/
    MI_U32 Change;    /**< if true, apply this result to hw register*/
    MI_U32 R_gain;     /**< AWB gain for R channel*/
    MI_U32 G_gain;     /**< AWB gain for G channel*/
    MI_U32 B_gain;     /**< AWB gain for B channel*/
    MI_U32 ColorTmp;   /**< Return color temperature*/
}CusAWBResult_t, CusAwbOutput_t;;

typedef enum __attribute__ ((aligned (1)))
{
    SS_AE_16x24 = 0,
    SS_AE_32x24,
    SS_AE_64x48,
    SS_AE_64x45,
    SS_AE_128x80,
    SS_AE_128x90,
    SS_AE_32x32
} MS_CUST_AE_WIN_BLOCK_NUM_TYPE_e;

typedef struct {
    MI_U16 u2Stawin_x_offset;
    MI_U16 u2Stawin_x_size;
    MI_U16 u2Stawin_y_offset;
    MI_U16 u2Stawin_y_size;
    MI_U16 u2WinIdx;
} CusAEHistWin_t;

typedef struct
{
    MI_U32 SizeX;
    MI_U32 SizeY;
    MI_U32 IncRatio;
}CusAWBSample_t;

typedef struct
{
    MI_U8 iir_1[5*16];  //[5]: iir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 iir_2[5*16];  //[5]: iir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 luma[4*16];   //[4]: luma 32bit, use 4*u8 datatype, [16]: 16wins
    MI_U8 fir_v[5*16];  //[5]: fir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 fir_h[5*16];  //[5]: fir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 ysat[3*16];   //[3]: ysat 22bit, use 3*u8 datatype,  [16]: 16wins
} AF_STATS_PARAM_t;

typedef struct
{
    AF_STATS_PARAM_t stParaAPI[16];
} CusAFStats_t;

typedef struct AF_WINDOW_PARAM_s
{
    MI_U32 u32StartX;                                           /*range : 0~1023*/
    MI_U32 u32StartY;                                           /*range : 0~1023*/
    MI_U32 u32EndX;                                             /*range : 0~1023*/
    MI_U32 u32EndY;                                             /*range : 0~1023*/
} AF_WINDOW_PARAM_t;

typedef struct
{
    MI_U8 u8WindowIndex;
    AF_WINDOW_PARAM_t stParaAPI;
} CusAFWin_t;

typedef struct
{
    MI_U16 u16IIR1_a0;
    MI_U16 u16IIR1_a1;
    MI_U16 u16IIR1_a2;
    MI_U16 u16IIR1_b1;
    MI_U16 u16IIR1_b2;
    MI_U16 u16IIR1_1st_low_clip;
    MI_U16 u16IIR1_1st_high_clip;
    MI_U16 u16IIR1_2nd_low_clip;
    MI_U16 u16IIR1_2nd_high_clip;
    MI_U16 u16IIR2_a0;
    MI_U16 u16IIR2_a1;
    MI_U16 u16IIR2_a2;
    MI_U16 u16IIR2_b1;
    MI_U16 u16IIR2_b2;
    MI_U16 u16IIR2_1st_low_clip;
    MI_U16 u16IIR2_1st_high_clip;
    MI_U16 u16IIR2_2nd_low_clip;
    MI_U16 u16IIR2_2nd_high_clip;
} CusAFFilter_t;

typedef struct
{
    MI_BOOL bSobelYSatEn;
    MI_U16  u16SobelYThd;

    MI_BOOL bIIRSquareAccEn;
    MI_BOOL bSobelSquareAccEn;

    MI_U16  u16IIR1Thd;
    MI_U16  u16IIR2Thd;
    MI_U16  u16SobelHThd;
    MI_U16  u16SobelVThd;
    MI_U8   u8AFTblX[12];
    MI_U16  u16AFTblY[13];
} CusAFFilterSq_t;


typedef enum __attribute__ ((aligned (1)))
{
    AF_ROI_MODE_NORMAL,
    AF_ROI_MODE_MATRIX
} ISP_AF_ROI_MODE_e;

typedef struct
{
    ISP_AF_ROI_MODE_e mode;
    MI_U32 u32_vertical_block_number;
} CusAFRoiMode_t;

/* Raw store control */
typedef enum
{
    eRawStoreNode_P0HEAD = 0, /* Control by VIF, Do not use */
    eRawStoreNode_P1HEAD = 1, /* Control by VIF, Do not use */
    eRawStoreNode_P0TAIL = 2,
    eRawStoreNode_P1TAIL = 3,
    eRawStoreNode_ISPOUT = 4,
    eRawStoreNode_VDOS   = 5,
    eRawStoreNode_ISPOUT_BEFORE_YEE = 6,
    eRawStoreNode_RGBIR_IR_ONLY = 7
}CameraRawStoreNode_e;

typedef struct
{
    MI_U32 u32image_width;
    MI_U32 u32image_height;
    MI_U32 u32Node;
    MI_U32 u32PixelDepth;
} CusImageResolution_t;

typedef struct
{
    MI_U32 u32enable;
    MI_U32 u32image_width;
    MI_U32 u32image_height;
    MI_U32 u32physical_address;
    MI_U32 u32Node;
} CusISPOutImage_t;

typedef struct
{
    MI_U32 u32enable;
    MI_U32 u32image_width;
    MI_U32 u32image_height;
    MI_U32 u32physical_address;
    MI_U32 u32Node;
    MI_U32 u32PixelDepth;
} CusHdrRawImage_t;

typedef struct
{
    MI_U64 u64Pts;  /** frame PTS */
    MI_U32 u32Shutter;         /**< Shutter in us */
    MI_U32 u32SensorGain;      /**< Sensor gain, 1X = 1024 */
    MI_U32 u32ColorTmp;   /**< Return color temperature*/
} IspFrameMetaInfo_t;

typedef struct
{
    MI_U32 u32SlaveAddr;  //sensor slave address
    MI_U32 u32RegLen;      //sensor register length , 1 or 2 bytes
    MI_U32 u32DataLen;     //sensor register data length, 1 or 2 bytes
    MI_U32 u32I2cSpeed;   //i2c speed , 100/200/300/400 KHz
    MI_U32 u32Reg;        //sensor register address
    MI_U32 u32Data;       //sensor register data
}CusSensorI2cParam_t;

/***************************** end of Cus3A ****************************/

// COMMON API
typedef struct MI_ISP_API_CHANNEL_ID_TYPE_s
{
    MI_U32 u32ChannelID;
} MI_ISP_API_CHANNEL_ID_TYPE_t;

typedef struct MI_ISP_API_USERSPACE3A_ATTR_s
{
    MI_SYS_PixelFormat_e ePixelFmt;
    MI_U32 eSensorBindId;
} MI_ISP_API_USERSPACE3A_ATTR_t;

typedef struct
{
    MI_S8 strIspRoot[64];
} MI_ISP_ROOT_PATH_T;

#ifdef __cplusplus
}   //end of extern C
#endif

#endif  //_MI_ISP_DATATYPE_H_
