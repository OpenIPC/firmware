#ifndef _ISP_CUS3A_IF_H_
#define _ISP_CUS3A_IF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define CUS3A_VER_STR "CUS3A_V1.1"
#define CUS3A_VER_MAJOR 1
#define CUS3A_VER_MINOR 1

#include "cam_os_wrapper.h"

#if 0
typedef unsigned char   u8;
//typedef signed char   s8;
typedef unsigned short  u16;
//typedef signed short  s16;
typedef unsigned int   u32;
//typedef signed int   s32;
typedef unsigned long long u64;
//typedef signed long long s64;
#endif

#define MV_WIN_NUM                 25
#define CAM20_AUTO_NUM             16
#define CAM20_SDC_CTRL_ITEM_NUMBER 2
#define AWB_LV_CT_TBL_NUM          18
#define AWB_CT_TBL_NUM             10

#define MS_GAMMA_TBL               256
#define MS_ALSC_TBL_W              61
#define MS_ALSC_TBL_H              69
#define MS_ALSC_TBL                4209
#define MS_SDC_TBL                 1024
#define MS_FPN_TBL                 4000 //infinity5
#define MS_YUVGAMA_Y_TBL           256
#define MS_YUVGAMA_VU_TBL          128
#define MS_WDR_LOC_TBL             88

#define MS_CAM_AF_MAX_WIN_NUM 16 //
#define MAX_CUST_3A_CHINFO_NUM (16)

/*------AE/AWB interface--------*/
/*! @brief API error code*/
typedef enum CUS_3A_ERR_CODE
{
    CUS_3A_SUCCESS = 0, /**< operation successful */
    CUS_3A_ERROR = -1, /**< unspecified failure */
}CUS3A_ERR_CODE;

#define _3A_ROW             (128)   /**< number of 3A statistic blocks in a row */
#define _3A_COL             (90)    /**< number of 3A statistic blocks in a column */
#define _3A_HIST_BIN0    (40)    /**< histogram type0 resolution*/
#define _3A_HIST_BINX    (128)   /**< histogram type1 resolution*/
#define _3A_IR_HIST_BIN  (256)   /**< histogram type2 resolution*/

// AWB statistic , one sample
typedef struct
{
    u8 r;
    u8 g;
    u8 b;
} __attribute__((packed, aligned(1))) ISP_AWB_SAMPLE;

#if 0
/*! @brief AWB HW statistics data*/
typedef struct
{
    //ISP_AWB_SAMPLE data[_3A_ROW*_3A_COL];
    u32 AvgBlkX;
    u32 AvgBlkY;
    ISP_AWB_SAMPLE *avgs;
} __attribute__((packed, aligned(1))) ISP_AWB_INFO;
#else
/*! @brief AWB HW statistics data*/
typedef struct
{
    u32 Size;           /**< struct size*/
    u32 AvgBlkX;
    u32 AvgBlkY;
    u32 CurRGain;
    u32 CurGGain;
    u32 CurBGain;
    ISP_AWB_SAMPLE *avgs;   /*awb statis for linear frame or HDR long frame*/
    /*CUS3A V1.1*/
    u8  HDRMode;             /**< Noramal or HDR mode*/
    ISP_AWB_SAMPLE*  pAwbStatisShort; /**<awb statis for HDR short Shutter AWB statistic data */
    u32 u4BVx16384;      /**< From AE output, Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    u32 WeightY;                /**< frame brightness with ROI weight*/
} __attribute__((packed, aligned(1))) ISP_AWB_INFO;
#endif


/*! @brief AWB algorithm result*/
typedef struct isp_awb_result
{
    u32 Size; /**< struct size*/
    u32 Change; /**< if true, apply this result to hw register*/
    u32 R_gain; /**< AWB gain for R channel*/
    u32 G_gain; /**< AWB gain for G channel*/
    u32 B_gain; /**< AWB gain for B channel*/
    u32 ColorTmp; /**< Return color temperature*/
} ISP_AWB_RESULT;

// AE statistics data
typedef struct
{
    u8 r;
    u8 g;
    u8 b;
    u8 y;
} __attribute__((packed, aligned(1))) ISP_AE_SAMPLE;

typedef struct
{
    u16 u2HistY[_3A_HIST_BINX];
} __attribute__((packed, aligned(1))) ISP_HISTX;

typedef struct
{
    u16 u2IRHist[_3A_IR_HIST_BIN];
} __attribute__((packed, aligned(1))) ISP_IR_HISTX;

/*! @brief ISP report to AE, hardware statistic */
typedef struct
{
    u32 Size;       /**< struct size*/
    ISP_HISTX   *hist1;   /**< HW statistic histogram 1*/
    ISP_HISTX   *hist2;   /**< HW statistic histogram 2*/
    u32 AvgBlkX;  /**< HW statistics average block number*/
    u32 AvgBlkY;  /**< HW statistics average block number*/
    ISP_AE_SAMPLE * avgs; /**< HW statistics average block data*/
    u32 Shutter;                    /**< Current shutter in us*/
    u32 SensorGain;                 /**< Current Sensor gain, 1X = 1024 */
    u32 IspGain;                    /**< Current ISP gain, 1X = 1024*/
    u32 ShutterHDRShort;           /**< Current shutter in us*/
    u32 SensorGainHDRShort;        /**< Current Sensor gain, 1X = 1024 */
    u32 IspGainHDRShort;           /**< Current ISP gain, 1X = 1024*/

    /*CUS3A V1.1*/
    u32 PreAvgY;                   /**< Previous frame brightness*/
    u8  HDRCtlMode;                /**< 0 = Separate shutter/sensor gain settings; */
                                   /**< 1 = Separate shutter & Share sensor gain settings */
    u32 FNx10;                     /**< Aperture in FNx10*/
    u32 CurFPS;                    /**Current sensor FPS */
    u32 PreWeightY;             /**< Previous frame brightness with ROI weight*/
    ISP_IR_HISTX *histIR;           /**< HW statistic histogram IR*/
} __attribute__((packed, aligned(1))) ISP_AE_INFO;

//AE algorithm result
/*! @brief ISP ae algorithm result*/
typedef struct
{
    u32 Size;           /**< struct size*/
    u32 Change;         /**< if true, apply this result to hw register*/
    u32 Shutter;         /**< Shutter in us */
    u32 SensorGain;      /**< Sensor gain, 1X = 1024 */
    u32 IspGain;         /**< ISP gain, 1X = 1024 */
    u32 ShutterHdrShort;     /**< Shutter in us */
    u32 SensorGainHdrShort;  /**< Sensor gain, 1X = 1024 */
    u32 IspGainHdrShort;     /**< ISP gain, 1X = 1024 */
    u32 u4BVx16384;      /**< Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    u32 AvgY;            /**< frame brightness */
    u32 HdrRatio;   /**< hdr ratio, 1X = 1024 */
    /*CUS3A V1.1*/
    u32 FNx10;                     /**< F number * 10*/
    u32 DebandFPS;       /** Target fps when running auto debanding**/
    u32 WeightY;                /**< frame brightness with ROI weight*/
} __attribute__((packed, aligned(1))) ISP_AE_RESULT;

/*! @brief ISP initial status*/
typedef struct _isp_ae_init_param
{
    u32 Size; /**< struct size*/
    char sensor_id[32]; /**< sensor module id*/
    u32 shutter; /**< shutter Shutter in us*/
    u32 shutter_step; /**< shutter Shutter step ns*/
    u32 shutter_min; /**< shutter Shutter min us*/
    u32 shutter_max; /**< shutter Shutter max us*/
    u32 sensor_gain; /**< sensor_gain Sensor gain, 1X = 1024*/
    u32 sensor_gain_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    u32 sensor_gain_max; /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    u32 isp_gain; /**< isp_gain Isp digital gain , 1X = 1024 */
    u32 isp_gain_max; /**< isp_gain Maximum Isp digital gain , 1X = 1024 */
    u32 FNx10; /**< F number * 10*/
    u32 fps; /**< initial frame per second*/
    u32 shutterHDRShort_step;           /**< shutter Shutter step ns*/
    u32 shutterHDRShort_min;            /**< shutter Shutter min us*/
    u32 shutterHDRShort_max;            /**< shutter Shutter max us*/
    u32 sensor_gainHDRShort_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    u32 sensor_gainHDRShort_max;        /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    u32 AvgBlkX;  /**< HW statistics average block number*/
    u32 AvgBlkY;  /**< HW statistics average block number*/
} ISP_AE_INIT_PARAM;

typedef enum
{
    ISP_AE_FPS_SET, /**< ISP notify AE sensor FPS has changed*/
} ISP_AE_CTRL_CMD;

typedef struct
{
    u32 start_x; /*range : 0~1023*/
    u32 start_y; /*range : 0~1023*/
    u32 end_x;   /*range : 0~1023*/
    u32 end_y;   /*range : 0~1023*/
} ISP_AF_RECT;

/*! @brief ISP initial status*/
typedef struct _isp_af_init_param
{
    u32 Size; /**< struct size*/
    ISP_AF_RECT af_stats_win[16];

} ISP_AF_INIT_PARAM;

typedef enum
{
    ISP_AF_CMD_MAX,
} ISP_AF_CTRL_CMD;

typedef struct
{
    u8 high_iir[5*16];
    u8 low_iir[5*16];
    u8 luma[4*16];
    u8 sobel_v[5*16];
    u8 sobel_h[5*16];
    u8 ysat[3*16];
} ISP_AF_INFO_STATS_PARAM_t;
//Infinity5. Sync with ISP_AF_STATS

typedef struct
{
    ISP_AF_INFO_STATS_PARAM_t stParaAPI[MS_CAM_AF_MAX_WIN_NUM];
} ISP_AF_INFO_STATS;

/*! @brief ISP report to AF, hardware statistic */
typedef struct
{
    u32 Size; /**< struct size*/
    ISP_AF_INFO_STATS af_stats; /**< AF statistic*/
} __attribute__((packed, aligned(1))) ISP_AF_INFO;

typedef struct
{
    u32 Change; /**< if true, apply this result to hw*/
    u32 Focal_pos; /**< Focal position*/
} __attribute__((packed, aligned(1))) ISP_AF_RESULT;

/**@brief ISP AE interface*/
typedef struct isp_ae_interface
{
    void *pdata; /**< Private data for AE algorithm.*/

    /** @brief AE algorithm init callback
     @param[in] pdata AE algorithm private data
     @param[in] init_state ISP initial status.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function when AE algorithm initialize.
     */
    int (*init)(void* pdata, ISP_AE_INIT_PARAM *init_state);

    /** @brief AE algorithm close
     @param[in] pdata AE algorithm private data
     @remark ISP call this function when AE close.
     */
    void (*release)(void* pdata);

    /** @brief AE algorithm run
     @param[in] pdata AE algorithm private data
     @param[in] info ISP HW statistics
     @param[out] result AE algorithm return calculated result.
     @remark ISP call this function when AE close.
     */
    void (*run)(void* pdata, const ISP_AE_INFO *info, ISP_AE_RESULT *result);

    /** @brief AE algorithm control
     @param[in] pdata AE algorithm private data
     @param[in] cmd Control ID
     @param[in out] param Control parameter.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function to change parameter
     */
    int (*ctrl)(void* pdata, ISP_AE_CTRL_CMD cmd, void* param);
} ISP_AE_INTERFACE;

typedef enum
{
    ISP_AWB_MODE_SET,
} ISP_AWB_CTRL_CMD;
/**@brief ISP AWB interface*/

typedef struct isp_awb_interface
{
    void *pdata; /**< Private data for AE algorithm.*/

    /** @brief AWB algorithm init callback
     @param[in] pdata Algorithm private data
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function when AE algorithm initialize.
     */
    int (*init)(void *pdata);

    /** @brief AWB algorithm close
     @param[in] pdata Algorithm private data
     @remark ISP call this function when AE close.
     */
    void (*release)(void *pdata);

    /** @brief AWB algorithm run
     @param[in] pdata Algorithm private data
     @param[in] info ISP HW statistics
     @param[out] result AWB algorithm return calculated result.
     @remark ISP call this function when AE close.
     */
    void (*run)(void *pdata, const ISP_AWB_INFO *awb_info, ISP_AWB_RESULT *result);

    /** @brief AWB algorithm control
     @param[in] pdata Algorithm private data
     @param[in] cmd Control ID
     @param[in out] param Control parameter.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function to change parameter
     */
    int (*ctrl)(void *pdata, ISP_AWB_CTRL_CMD cmd, void* param);
} ISP_AWB_INTERFACE;

/**@brief ISP AF interface*/
typedef struct isp_af_interface
{
    void *pdata; /**< Private data for AF algorithm.*/

    /** @brief AF algorithm init callback
     @param[in] pdata Algorithm private data
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function when AF algorithm initialize.
     */
    int (*init)(void *pdata, ISP_AF_INIT_PARAM *param);

    /** @brief AF algorithm close
     @param[in] pdata Algorithm private data
     @remark ISP call this function when AF close.
     */
    void (*release)(void *pdata);

    /** @brief AF algorithm run
     @param[in] pdata Algorithm private data
     @param[in] info ISP HW statistics
     @param[out] result AF algorithm return calculated result.
     @remark ISP call this function when AF close.
     */
    void (*run)(void *pdata, const ISP_AF_INFO *af_info, ISP_AF_RESULT *result);

    /** @brief AF algorithm control
     @param[in] pdata Algorithm private data
     @param[in] cmd Control ID
     @param[in out] param Control parameter.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function to change parameter
     */
    int (*ctrl)(void *pdata, ISP_AF_CTRL_CMD cmd, void* param);
} ISP_AF_INTERFACE;


unsigned int CUS3A_GetVersion(char* pVerStr);
int CUS3A_Init(void);
void CUS3A_Release(void);
int CUS3A_RegInterface(u32 nCh,ISP_AE_INTERFACE *pAE,ISP_AWB_INTERFACE *pAWB,ISP_AF_INTERFACE *pAF);
int CUS3A_AERegInterface(u32 nCh,ISP_AE_INTERFACE *pAE);
int CUS3A_AWBRegInterface(u32 nCh,ISP_AWB_INTERFACE *pAWB);
int CUS3A_AFRegInterface(u32 nCh,ISP_AF_INTERFACE *pAF);
void* pAllocDmaBuffer(const char* pName, u32 nReqSize, u32 *pPhysAddr, u32 *pMiuAddr, u8 bCache);
int FreeDmaBuffer(const char* pName, u32 u32MiuAddr, void *pVirtAddr, u32 u32FreeSize);
int Cus3AOpenIspFrameSync(int *fd0, int *fd1);
int Cus3ACloseIspFrameSync(int fd0, int fd1);
unsigned int Cus3AWaitIspFrameSync(int fd0, int fd1, int timeout);
int CUS3A_AeAvgDownSample(const ISP_AE_SAMPLE *pInBuf, ISP_AE_SAMPLE *pOutBuf, unsigned int nInBlkX, unsigned int nInBlkY, unsigned int nOutBlkX, unsigned int nOutBlkY);
#ifdef __cplusplus
}
#endif

#endif
