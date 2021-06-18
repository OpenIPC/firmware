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


#ifndef MSTAR_ISP_GENERAL_H_
#define MSTAR_ISP_GENERAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef void* ISP_DBGBUF_HANDLE;

//#include <isp_types.h>
/// data type unsigned char, data length 1 byte
typedef unsigned char               MS_U8;                              // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short              MS_U16;                             // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned int                MS_U32;                             // 4 bytes
/// data type unsigned int, data length 8 byte
typedef unsigned long long          MS_U64;                             // 8 bytes
/// data type signed char, data length 1 byte
typedef signed char                 MS_S8;                              // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                MS_S16;                             // 2 bytes
/// data type signed int, data length 4 byte
typedef signed int                  MS_S32;                             // 4 bytes
/// data type signed int, data length 8 byte
typedef signed long long            MS_S64;                             // 8 bytes
/// data type float, data length 4 byte
typedef float                       MS_FLOAT;                           // 4 bytes

typedef unsigned char               MI_U8;                              // 1 byte
typedef unsigned short              MI_U16;                             // 2 bytes
typedef unsigned int                MI_U32;                             // 4 bytes
typedef unsigned long long          MI_U64;                             // 8 bytes
typedef signed char                 MI_S8;                              // 1 byte
typedef signed short                MI_S16;                             // 2 bytes
typedef signed int                  MI_S32;                             // 4 bytes
typedef signed long long            MI_S64;                             // 8 bytes
/*
typedef enum __attribute__ ((aligned (4)))
{
    SS_FALSE = 0,
    SS_TRUE = !SS_FALSE,
    SS_BOOL_MAX
} MI_ISP_BOOL_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_OP_TYP_AUTO = 0,
    SS_OP_TYP_MANUAL = !SS_OP_TYP_AUTO,
    SS_OP_TYP_MODE_MAX
} MI_ISP_OP_TYPE_e;
*/
typedef enum __attribute__ ((aligned (4)))
{
    MS_FALSE = 0,
    MS_TRUE  = !MS_FALSE,
    MS_BOOL_MAX = 0x7FFFFFFF
} MS_CAM_BOOL;

#define FAIL                    (-1)
#define SUCCESS                 (0)

#if !defined(NULL)
#define NULL                    (0)
#endif

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif

#define LIMIT(x, low, high)     (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifdef sign
#undef sign
#endif
#define sign(a)         (((a) > 0) ? (1) : (-1))

#define CAMERA_ISR_FREQ_SEC     (0)
#define CAMERA_ISR_FREQ_NSEC    (10000000) //(1000*1000*1000) // 1sec

#define PACK_ALIGN_TAG(n)       __attribute__((packed, aligned(n)))
#define LIMIT_BITS(x, bits)     ((x) > ((1<<(bits))-1) ? (((1<<(bits))-1)) : (x))
typedef void *(*THREAD_P)(void *);

#define AE_WIN_MAX_WIDTH        (32)//(16)
#define AE_WIN_MAX_HEIGHT       (32)//(24)
#define AE_HIST_WIN_MAX_WIDTH   (128)
#define AE_HIST_WIN_MAX_HEIGHT  (90)
#define AE_HIST_BIN0            (40)
#define AE_HIST_BINX            (128)
#define AE_IR_HIST_BIN          (256)
#define AE_HIST_OUT_BIN         (256)

#define HDR_STATS_LEN           (1024)

#define AWB_WIN_MAX_WIDTH       (128)
#define AWB_WIN_MAX_HEIGHT      (90)
#define AWB_BLOCK_MIN_VALID_X   (4)
#define AWB_BLOCK_MIN_VALID_Y   (2)

#define AWB_LV_CT_TBL_NUM          18
#define AWB_CT_TBL_NUM             10

typedef struct
{
    MS_U8 uAvgR;
    MS_U8 uAvgG;
    MS_U8 uAvgB;
    MS_U8 uAvgY;
} MS_AE_AVGS;

typedef struct
{
    MS_U8 uAvgR;
    MS_U8 uAvgG;
    MS_U8 uAvgB;
} MS_AWB_AVGS;

// crop range
typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    unsigned int degree;
    char         mirror_flip;
} ISP_CROP_RANGE;

// input source/output destination
typedef enum
{
    // source
    ISP_DATA_SRC_TV,
    ISP_DATA_SRC_SENSOR,
    ISP_DATA_SRC_RDMA,

    // destination
    ISP_DATA_DST_ICP,
    ISP_DATA_DST_WDMA_SIF,
    ISP_DATA_DST_WDMA_ISP,
    ISP_DATA_DST_WDMA_ISPDS,
    ISP_DATA_DST_WDMA_ISPSWAPYC,
    ISP_DATA_DST_WDMA_VDOS,
    ISP_DATA_DST_WDMA_ISP_GREY,
    ISP_DATA_DST_WDMA_ISP_OUT,
    ISP_DATA_DST_WDMA_RGBIR_IN
} ISP_DATA_SRCDST;
typedef enum
{
    ISP_SRC_SENSOR,
    ISP_SRC_RDMA,
} ISP_DATA_SRC;

typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} ISP_WDMA_CROP_RANGE;
// color space
typedef enum
{
    ISP_COLOR_SPACE_YUV,
    ISP_COLOR_SPACE_BAYER,
} ISP_COLOR_SPACE;

// data precision
typedef enum
{
    ISP_DATAPRECISION_8 = 0,
    ISP_DATAPRECISION_10 = 1,
    ISP_DATAPRECISION_16 = 2,
    ISP_DATAPRECISION_12 = 3,
    ISP_DATAPRECISION_1T4P_8 = 4,
    ISP_DATAPRECISION_1T4P_10 = 5,
    ISP_DATAPRECISION_1T4P_16 = 6,
    ISP_DATAPRECISION_1T4P_12 = 7
} ISP_DATAPRECISION;

typedef enum
{
    BAYER_RG = 0,
    BAYER_GR = 1,
    BAYER_BG = 2,
    BAYER_GB = 3
} SEN_BAYER;

typedef enum
{
    RGBIR_R0 = 0,
    RGBIR_G0 = 1,
    RGBIR_B0 = 2,
    RGBIR_G1 = 3,
    RGBIR_G2 = 4,
    RGBIR_I0 = 5,
    RGBIR_G3 = 6,
    RGBIR_I1 = 7,
    RGBIR_NONE = 8,
} SEN_RGBIR;


// input/output configuration
typedef struct
{
    ISP_CROP_RANGE          crop_range;
    ISP_DATA_SRCDST         srcdst;
    ISP_DATAPRECISION       data_precision;
    ISP_COLOR_SPACE         colorspace;
} ISP_INOUT_CONFIG;
/*
// data control path
typedef enum
{
    ISP_CTL_PATH_SENSOR_ISP_ICP,                        // preview
    ISP_CTL_PATH_SENSOR_WDMA_RDMA_ISP_ICP,              // preview with ring buffer
    ISP_CTL_PATH_SENSOR_WDMA,                           // capture raw
    ISP_CTL_PATH_RDMA_ISPDS_WDMA,                       // iterative processing raw
    ISP_CTL_PATH_RDMA_ISPDP_WDMA,                       // software trigger capture
    ISP_CTL_PATH_RDMA_ISP_ICP                           // software trigger capture
} ISP_CTL_PATH;
*/

/*
typedef enum {
    CAMERA_ID_REAR1     = 0,
    CAMERA_ID_REAR2     = 1,
    CAMERA_ID_FRONT1    = 2,
    CAMERA_ID_FRONT2    = 3,
    CAMERA_ID_TV1       = 4,
    CAMERA_ID_TV2       = 5,
    CAMERA_ID_PG        = 6,
    CAMERA_ID_AUTO      = 7,
    CAMERA_ID_END       = 8
} CameraModule_id_t;
*/

typedef enum
{
    IQ_OBC,
    IQ_WBG,
    IQ_WBG0,
    IQ_WBG1,
    IQ_LSC,
    IQ_CROSSTALK,
    IQ_AEAWB_STATS,
    IQ_DPC,
    IQ_SPIKE_NR,
    IQ_MEAN_FILTER,     // mean filter
    IQ_NOISE_MASKING,   // noise masking
    IQ_FALSECOLOR,
    IQ_CFAi,            // De-Mosaic
    IQ_CFAi_NR,         // Post Denoise
    IQ_DNR,
    IQ_DNR_LUM,
    IQ_AF_STATS,
    IQ_CCM,
    IQ_UVCM,
    IQ_GAMMA_RAW_LC,
    IQ_GAMMA_RGB_LC,
    IQ_GAMMA16TO16,
    IQ_ALSC,
    IQ_MOTION,
    IQ_ROTATE,
    IQ_END
} ISP_IQ_BLK;

typedef enum
{
    ISP_WDMA_ICP,
    ISP_WDMA_SIF,
    ISP_WDMA_ISP,
    ISP_WDMA_ISPDS,
    ISP_WDMA_ISPSWAPYC,
    ISP_WDMA_VDOS,
    ISP_WDMA_ISP_GREY,
    ISP_WDMA_ISP_OUT,
    ISP_WDMA_RGBIR_IN,
    ISP_WDMA_DEFAULT_SIF,
    ISP_WDMA_DEFAULT_YC
} WDMA_PATH;

typedef struct
{
    MS_U32 width;
    MS_U32 height;
    MS_U32 x;
    MS_U32 y;
    WDMA_PATH wdma_path;
    MS_U32  miu_addr;
} PACK_ALIGN_TAG(1) ISP_WDMA_ATTR;

// AE functions
typedef struct
{
    MS_U16 u2BlkSize_x;
    MS_U16 u2BlkSize_y;
    MS_U16 u2BlkNum_x;
    MS_U16 u2BlkNum_y;
} ISP_AE_WIN;

typedef struct
{
    MS_U16 Rgbir_stats[AE_IR_HIST_BIN];
} ISP_RGBIR_STATS;

typedef struct
{
    MS_U16 HDR_stats[HDR_STATS_LEN];
} ISP_HDR_STATS;

typedef struct
{
    MS_U8  uAvgR;
    MS_U8  uAvgG;
    MS_U8  uAvgB;
    MS_U8  uAvgY;
} ISP_AE_AVGS;

typedef struct
{
    MS_U16 u2HistR[AE_HIST_BIN0];
    MS_U16 u2HistG[AE_HIST_BIN0];
    MS_U16 u2HistB[AE_HIST_BIN0];
} ISP_AE_HIST0;

typedef struct
{
    MS_U16 u2HistY[AE_HIST_BINX];
} ISP_AE_HISTX;

typedef struct
{
    ISP_AE_HIST0    Hist0;
    ISP_AE_HISTX    Hist1;
    ISP_AE_HISTX    Hist2;
    ISP_RGBIR_STATS HistIR;
    ISP_AE_AVGS     Avgs[AE_WIN_MAX_WIDTH * AE_WIN_MAX_HEIGHT];
    ISP_AE_AVGS     Avgs1[AE_WIN_MAX_WIDTH * AE_WIN_MAX_HEIGHT];
} PACK_ALIGN_TAG(16) ISP_AE_STATIS;

#if 1
// crop config, isp_crop_cfg
typedef struct {
    int x_start;
    int y_start;
    int width;
    int height;
} ISP_CROP_WIN, IspCropWin_t;
#endif

#if 1
typedef struct {
    MS_U16     u2BlkNum_x;
    MS_U16     u2BlkNum_y;
    MS_U16     u2BlkVaild_x;
    MS_U16     u2BlkVaild_y;
    MS_U16     u2BlkValidLowLux_x;
    MS_U16     u2BlkValidLowLux_y;
    MS_U8      AccEnhanceRatio;
} ISP_AWB_WIN, IspAwbWin_t;
#endif

typedef struct
{
    MS_U16 u2Stawin_x_offset;
    MS_U16 u2Stawin_x_size;
    MS_U16 u2Stawin_y_offset;
    MS_U16 u2Stawin_y_size;
    //MS_BOOL roi_mode;
} ISP_HISTO_WIN;

typedef struct
{
    MS_U8  acc_sft;
    MS_U16 u2BlkSize_x;
    MS_U16 u2BlkSize_y;
    MS_U16 u2BlkNum_x;
    MS_U16 u2BlkNum_y;
    MS_U16 u2Blk_h_offset;
    MS_U16 u2Blk_v_offset;
} ISP_HISTO_ATTR;

typedef struct
{
    MS_U16  nRgain;     /** R  channel */
    MS_U16  nGrgain;    /** Gr channel */
    MS_U16  nGbgain;    /** Gb channel */
    MS_U16  nBgain;     /** B  channel */
} IspAwbGain_t;

typedef struct {
    MS_U8  uAvgR;
    MS_U8  uAvgG;
    MS_U8  uAvgB;
} PACK_ALIGN_TAG(1) ISP_AWB_AVGS, IspAwbBlkAvg_t;

typedef struct {
    IspAwbBlkAvg_t tRow[AWB_WIN_MAX_WIDTH];
} PACK_ALIGN_TAG(16) IspAwbAvgRow_t;

typedef struct {
    ISP_AWB_AVGS Avgs[AWB_WIN_MAX_WIDTH * AWB_WIN_MAX_HEIGHT];
    //IspAwbAvgRow_t Avgs[AWB_WIN_MAX_HEIGHT];
} PACK_ALIGN_TAG(16) ISP_AWB_STATIS,IspAwbStatis_t;

typedef enum
{
    SIF_BUS_PARL = 0,
    SIF_BUS_MIPI = 1
} SIF_BUS;

typedef enum
{
    CLK_POL_POS = 0,
    CLK_POL_NEG
} CLK_POL;

typedef enum
{
    ISP_DNR_PATH_BEFORE_HDR = 0,
    ISP_DNR_PATH_AFTER_HDR
} ISP_DNR_PATH_SEL;

typedef struct
{
    MS_U32 x;
    MS_U32 y;
    MS_U32 h;
    MS_U32 w;
}RECT_t;

typedef enum
{
    eIspInOutMode_RealTimeModeNoHDR = 0,
    eIspInOutMode_RealTimeModeHDR = 1,
    eIspInOutMode_FrameModeNoHDR = 2,
    eIspInOutMode_FrameModeHDR = 3,
    eIspInOutMode_RealTimeModeYuv = 4,
    eIspInOutMode_FrameModeYuv = 5,
}IspInOutMode_e;

typedef enum
{
    eIspHDRType_OFF,
    eIspHDRType_VC,                 //virtual channel mode HDR, vc0->long, vc1->short
    eIspHDRType_DOL,
    eIspHDRType_EMB,  //compressed HDR mode
    eIspHDRType_LI,                //Line interlace HDR
    eIspHDRType_MAX
}IspHDRType_e;

typedef enum
{
    eIspBayerID_RG = 0,
    eIspBayerID_GR = 1,
    eIspBayerID_BG = 2,
    eIspBayerID_GB = 3,
}IspBayerID_e;

typedef enum
{
    eIspPixelFmt_8Bit = 0,
    eIspPixelFmt_10Bit = 1,
    eIspPixelFmt_16Bit = 2,
    eIspPixelFmt_12Bit = 3,
}IspPixelFmt_e;

//infinity5
typedef enum
{
    eIsp3DNRCompressLevel_0 = 0, //12-bit
    eIsp3DNRCompressLevel_1 = 1,  //10-bit
    eIsp3DNRCompressLevel_2 = 2,  //8-bit
    eIsp3DNRCompressLevel_3 = 3, //reserved
}Isp3DNRCompressLevel_e;

//infinity6
typedef enum
{
    eIspDNRBitDepth_8 = 0, //8-bit
    eIspDNRBitDepth_12 = 1,  //12-bit
}IspDNRBitDepth_e;

typedef enum
{
    eIspDNRBlockMode = 0,  // block mode
    eIspDNRLinearMode = 1, // linear mode
}IspDNRDataArrange_e;

typedef enum
{
    eIspDNRBlock_128 = 0,  //  for 128 byte block width
    eIspDNRBlock_256 = 1, //  for 256 byte block width
}IspDNRBlockWidth_e;

typedef enum
{
    eIspDNRDramRow_2K = 0,  //  for 2K byte/Row in DRAM
    eIspDNRDramRow_1K = 1, //  for 1K byte/Row in DRAM
}IspDNRDramRowMode_e;

typedef struct
{
    MS_U16 u16InWidth;
    MS_U16 u16InHeight;
    SEN_BAYER eBayerId;
    Isp3DNRCompressLevel_e eBitDepth;
    IspDNRDataArrange_e eDataMode;
    IspDNRBlockWidth_e eBlockWidth;
    IspDNRDramRowMode_e eRowMode;
    MS_U32 u32HistBaseAddr;
    MS_U32 u32RefBaseAddr[4];
    MS_CAM_BOOL bRingMode;
}IspDNRAttr_t;

typedef struct
{
    MS_U16 u16InWidth;
    MS_U16 u16InHeight;
    IspDNRBitDepth_e eBitDepth;
    IspDNRDataArrange_e eDataMode;
    IspDNRBlockWidth_e eBlockWidth;
    IspDNRDramRowMode_e eRowMode;
    MS_U32 u32BaseAddr[4];
    MS_U8 u8MiuReqNum;
    MS_U8 u8MiuReqCycle;
}IspROTAttr_t;

typedef struct
{
    IspInOutMode_e eMode;
    IspHDRType_e eHdrType;
    MS_U16 u16InWidth;
    MS_U16 u16InHeight;
    RECT_t tCrop;
    SEN_BAYER eBayerID;
    ISP_DATAPRECISION ePixelFmt;
    ISP_AE_WIN tAeWin;
    IspAwbWin_t tAwbWin;
    SEN_RGBIR eRGBIRID; //infinity5.
    MS_CAM_BOOL bRoation;
    MS_U16 u16IQWidth;
    MS_U16 u16IQHeight;
}IspInOutAttr_t;

#ifdef __cplusplus
}
#endif
#endif /* MSTAR_ISP_GENERAL_H_ */
