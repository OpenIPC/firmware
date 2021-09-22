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

/*! @file drv_ms_cus_sensor.h
      @brief This file contains Infinity ISP sensor driver interface.
*/

/** @defgroup group1 ISP Sensor Driver Interface
* @{
*/

#ifndef DRV_MS_CUS_SENSOR_H_
#define DRV_MS_CUS_SENSOR_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <sensor_i2c_api.h>

#define I2C_RETRYTIME (5)

#ifndef SUCCESS
#define FAIL        (-1)
#define SUCCESS     0
#endif

#ifdef __cplusplus
#define EXPORT_CUS  extern "C"
#else
#define EXPORT_CUS
#endif

#define CUS_MSTART_CAMSENSOR_CAP_VERSION 0x0001

//#define usleep(usec)  CamOsMsSleep(usec*1000);
//#define usleep(usec)  udelay(usec)
//#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define SENSOR_DRIVER_MODE_NOT_SUUPORT                          (0xFFFF)

struct __ms_cus_sensor; /**< Sensor driver handle */
//struct __ISensorAPI; /**< Sensor to ISP control interface */

/*! @brief Resolution descriptor*/
typedef struct _cus_camsensor_res{
    u32 width;      /**< Image crop width */
    u32 height;     /**< Image crop height */
    u32 max_fps; /**< Max fps in this resolution */
    u32 min_fps;  /**< Min fps in this resolution*/
    u32 crop_start_x;
    u32 crop_start_y;
    u32 nOutputWidth; /**< Sensor actual output width */
    u32 nOutputHeight; /**< Sensor actual output height */
    char strResDesc[32];
} __attribute__((packed, aligned(4))) cus_camsensor_res;

/*! @brief Resolution list*/
typedef struct _cus_camsensor_res_list
{
    u32 num_res;                        /**< number of sensor resolution in list */
    u32 ulcur_res;                        /**< current sensor resolution*/
    cus_camsensor_res res[12];      /**< resolution list */
} __attribute__((packed, aligned(4))) cus_camsensor_res_list;


/*! @brief Select sensor type */
typedef enum {
    ISP_SOC,                /**< Not support */
    ISP_EXT                 /**< sensor without built-in ISP */
} CUS_CAMSENSOR_ISPTYPE;

/*! @brief Sensor bayer raw pixel order */
typedef enum {
    CUS_BAYER_RG = 0,       /**< bayer data start with R channel */
    CUS_BAYER_GR,             /**<  bayer data start with Gr channel */
    CUS_BAYER_BG,             /**<  bayer data start with B channel */
    CUS_BAYER_GB              /**<  bayer data start with Gb channel */
} CUS_SEN_BAYER;

typedef enum {
    CUS_RGBIR_R0 = 0,
    CUS_RGBIR_G0 = 1,
    CUS_RGBIR_B0 = 2,
    CUS_RGBIR_G1 = 3,
    CUS_RGBIR_G2 = 4,
    CUS_RGBIR_I0 = 5,
    CUS_RGBIR_G3 = 6,
    CUS_RGBIR_I1 = 7,
    CUS_RGBIR_NONE = 8
} CUS_SEN_RGBIR;

/*! @brief Set sensor image mirror and flip.*/
typedef enum {
    CUS_ORIT_M0F0,                  /**< mirror, flip unchanged */
    CUS_ORIT_M1F0,                  /**< mirror changed, flip unchanged */
    CUS_ORIT_M0F1,                  /**< mirror unchanged, flip changed */
    CUS_ORIT_M1F1,                  /**< mirror and flip changed */
} CUS_CAMSENSOR_ORIT;


/*! @brief Get input source type.*/
typedef enum {
    CUS_SNR_ANADEC_SRC_NO_READY,    /**< input no ready */
    CUS_SNR_ANADEC_SRC_PAL,         /**< input type is PAL */
    CUS_SNR_ANADEC_SRC_NTSC,        /**< input type is NTSC */
    CUS_SNR_ANADEC_SRC_HD,          /**< input source type is HD */
    CUS_SNR_ANADEC_SRC_FHD,         /**< input source type is FHD */
    CUS_SNR_ANADEC_SRC_DISCNT,      /**< input disconnect */
    CUS_SNR_ANADEC_SRC_MAX,         /**< input num max */
}CUS_SNR_ANADEC_SRC_TYPE;


/*! @brief ISP AE event notifycation*/
typedef enum {
    CUS_FRAME_INACTIVE = 0, /**< Frame end */
    CUS_FRAME_ACTIVE = 1,/**< Frame start */
} CUS_CAMSENSOR_AE_STATUS_NOTIFY;

/*! @brief Sensor bayer raw (8/10 bits) to 12bits mode control */
typedef enum {
    CUS_SEN_8TO12_7074,       /**< Do not use*/
    CUS_SEN_8TO12_7000,       /**< Sensor bayer raw is 8 bits*/
    CUS_SEN_8TO12_114118,   /**< Do not use*/
    CUS_SEN_8TO12_11400,     /**< Do not use*/
    CUS_SEN_10TO12_9098,     /**< Do not use*/
    CUS_SEN_10TO12_9000,     /**< Sensor bayer raw is 10 bits*/
    CUS_SEN_10TO12_1121110, /**< Do not use*/
    CUS_SEN_10TO12_11200    /**< Do not use*/
} CUS_SEN_FMT_CONV_MODE;

/*! @brief Sensor input raw data precision */
typedef enum {
    CUS_DATAPRECISION_8 = 0,    /**< raw data precision is 8bits */
    CUS_DATAPRECISION_10 = 1,   /**< raw data precision is 10bits */
    CUS_DATAPRECISION_16 = 2,    /**< raw data precision is 16bits */
    CUS_DATAPRECISION_12 = 3,   /**< raw data precision is 12bits */
    CUS_DATAPRECISION_14 = 4,   /**< raw data precision is 14bits */
} CUS_DATAPRECISION;

/*! @brief Select sensor data intarface */
typedef enum {
    CUS_SENIF_BUS_PARL = 0, /**< sensor data bus is parallel bus */
    CUS_SENIF_BUS_MIPI = 1,  /**<  sensor data bus is mipi */
    CUS_SENIF_BUS_BT601 = 2,
    CUS_SENIF_BUS_BT656 = 3,
    CUS_SENIF_BUS_BT1120 = 4,
} CUS_SENIF_BUS;

typedef enum {
    CUS_SEN_INPUT_FORMAT_YUV422,
    CUS_SEN_INPUT_FORMAT_RGB,
} CUS_SEN_INPUT_FORMAT;

/*! @brief Select pin polarity */
typedef enum {
    CUS_CLK_POL_POS = 0,    /**< High active */
    CUS_CLK_POL_NEG         /**< Low active */
} CUS_CLK_POL;

typedef enum
{
    CUS_SENSOR_YUV_ORDER_CY = 0,
    CUS_SENSOR_YUV_ORDER_YC = 1,
}CUS_SENSOR_YUV_ORDER;

/*! @brief Sensor master clock select */
typedef enum {
    CUS_CMU_CLK_27MHZ,
    CUS_CMU_CLK_21P6MHZ,
    CUS_CMU_CLK_12MHZ,
    CUS_CMU_CLK_5P4MHZ,
    CUS_CMU_CLK_36MHZ,
    CUS_CMU_CLK_54MHZ,
    CUS_CMU_CLK_43P2MHZ,
    CUS_CMU_CLK_61P7MHZ,
    CUS_CMU_CLK_72MHZ,
    CUS_CMU_CLK_48MHZ,
    CUS_CMU_CLK_24MHZ,
    CUS_CMU_CLK_37P125MHZ,
    CUS_CMU_CLK_LPLL_DIV1,
    CUS_CMU_CLK_LPLL_DIV2,
    CUS_CMU_CLK_LPLL_DIV4,
    CUS_CMU_CLK_LPLL_DIV8,
} CUS_MCLK_FREQ; //Depends on chip.

//Depends on chip definition.
typedef enum {
    CUS_SR0_PAR_DISABLE,
    CUS_SR0_PAR_MODE_1,
    CUS_SR0_PAR_MODE_2,
} CUS_SR0_PAR_MODE;

//Depends on chip definition.
typedef enum {
    CUS_SR0_BT656_DISABLE,
    CUS_SR0_BT656_MODE_1,
    CUS_SR0_BT656_MODE_2,
    CUS_SR0_BT656_MODE_3,
    CUS_SR0_BT656_MODE_4,
} CUS_SR0_BT656_MODE;

//Depends on chip definition.
typedef enum {
    CUS_SR1_BT656_DISABLE,
    CUS_SR1_BT656_MODE_1,
} CUS_SR1_BT656_MODE;

//Depends on chip definition.
typedef enum {
    CUS_SR0_BT601_DISABLE,
    CUS_SR0_BT601_MODE_1,
    CUS_SR0_BT601_MODE_2,
    CUS_SR0_BT601_MODE_3,
    CUS_SR0_BT601_MODE_4,
} CUS_SR0_BT601_MODE;

//Depends on chip definition.
typedef enum {
    CUS_SR0_MIPI_DISABLE,
    CUS_SR0_MIPI_MODE_1,
    CUS_SR0_MIPI_MODE_2,
} CUS_SR0_MIPI_MODE;

//Depends on chip definition.
typedef enum {
    CUS_SR1_MIPI_DISABLE,
    CUS_SR1_MIPI_MODE_1,
    CUS_SR1_MIPI_MODE_2,
    CUS_SR1_MIPI_MODE_3,
    CUS_SR1_MIPI_MODE_4,
} CUS_SR1_MIPI_MODE;

//Depends on chip definition.
typedef enum
{
    CUS_VIF_BT656_EAV_DETECT = 0,
    CUS_VIF_BT656_SAV_DETECT = 1,
}CUS_VIF_BT656_CHANNEL_SELECT;

//Depends on chip definition.
typedef enum
{
    CUS_VIF_BT656_VSYNC_DELAY_1LINE = 0,
    CUS_VIF_BT656_VSYNC_DELAY_2LINE = 1,
    CUS_VIF_BT656_VSYNC_DELAY_0LINE = 2,
    CUS_VIF_BT656_VSYNC_DELAY_AUTO = 3,
}CUS_VIF_BT656_VSYNC_DELAY;

typedef enum
{
    CUS_SENSOR_FUNC_DISABLE = 0,
    CUS_SENSOR_FUNC_ENABLE = 1,
}CUS_SENSOR_FUNC;

typedef enum
{
    CUS_SENSOR_PAD_GROUP_A = 0,
    CUS_SENSOR_PAD_GROUP_B = 1,
}CUS_SENSOR_PAD_GROUP;

typedef enum
{
    CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL = 0,
    CUS_SENSOR_CHANNEL_MODE_REALTIME_HDR = 1,
    CUS_SENSOR_CHANNEL_MODE_RAW_STORE = 2,
    CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR = 3,
}CUS_SENSOR_CHANNEL_MODE;

typedef struct {
    unsigned int gain;
    unsigned int offset;
} CUS_GAIN_GAP_ARRAY;

//////////////////////////////////////
// sensor functions
//////////////////////////////////////

typedef struct {
    u32 length;     //header length
    u32 version;         //version
}CUS_CAMSENSOR_CAP;

/////////////////// ISP for SENSOR API ///////////////////
typedef enum {
    CUS_INT_TASK_AE     = (1<<0),
    CUS_INT_TASK_AWB    = (1<<1),
    CUS_INT_TASK_AF     = (1<<2),
    CUS_INT_TASK_VS     = (1<<3),
    CUS_INT_TASK_VDOS   = (1<<4),
} CUS_INT_TASK_TYPE;

#define MAX_RUN_ORDER 16
typedef struct {
    u8 RunLength;
    u8 Orders[MAX_RUN_ORDER];
    u8 CurTaskType;
} CUS_INT_TASK_ORDER;

/////////////////// Shutter Info ///////////////////////
/*! @brief Report shutter information */
typedef struct {
    u32 length;    /**< struct size */
    u32 max;       /**< maximun shutter in us*/
    u32 min;        /**< minimum shutter in us*/
    u32 step;       /**< shutter in step us*/
} CUS_SHUTTER_INFO;

////////////////// CSI CLOCK ////////////////////////
/*! @brief Select MIPI clock*/
typedef enum {
    CUS_CSI_CLK_DISABLE = -1, /**< Disable MIPI clock*/
    CUS_CSI_CLK_108M = 0,    /**< MIPI pixel rate 864Mbps (1-lane)*/
    CUS_CSI_CLK_86M = 1, /**< MIPI pixel rate 344Mbps(1-lane)*/
    CUS_CSI_CLK_144M = 2, /**< MIPI pixel rate 1152Mbps(1-lane)*/
    CUS_CSI_CLK_172M = 3,
    CUS_CSI_CLK_216M = 4,
    CUS_CSI_CLK_288M = 5,
    CUS_CSI_CLK_40M = 6,
    CUS_CSI_CLK_24M = 7,
	CUS_CSI_CLK_12M = 8,
	CUS_CSI_CLK_123M = 9
}CUS_CSI_CLK;

///////////////// SENSOR PIN CONFIG/////////////////
/*! @brief Sensor pin assignment*/
typedef enum {
    CUS_SR_PAD_CFG_1 = 1,       /**< Pin config for MIPI 1/2 lanes*/
    CUS_SR_PAD_CFG_MIPI = 1,  /**< Pin config for  MIPI 1/2 lanes*/
    CUS_SR_PAD_CFG_2 = 2,       /**< Reserved */
    CUS_SR_PAD_CFG_3 = 3,       /**< Reserved */
    CUS_SR_PAD_CFG_10BITS = 4,    /**< Pin config for parallel interface 10 bits */
    CUS_SR_PAD_CFG_4 = 4,        /**< Pin config for parallel interface 12 bits */
    CUS_SR_PAD_CFG_12BITS = 5,  /**< Pin config for parallel interface 12 bits */
    CUS_SR_PAD_CFG_5 = 5,       /**< Pin config for parallel interface 12 bits */
    CUS_SR_PAD_CFG_6 = 6       /**< Pin config for parallel interface 12 bits */
 }CUS_SR_PAD_CFG;

/*! @brief virtual channel0 hsync. mode */
typedef enum
{
    PACKET_HEADER_EDGE1 = 0,    /**< packet header edge */
    PACKET_HEADER_EDGE2 = 1,    /**< line end edge */
    PACKET_HEADER_EDGE3 = 2,    /**< line start edge */
    PACKET_FOOTER_EDGE = 3,      /**< packet footer edge */
}CUS_CSI_VC_HS_MODE;

typedef enum
{
    CUS_HDR_MODE_NONE = 0,
    CUS_HDR_MODE_SONY_DOL = 1,
    CUS_HDR_MODE_DCG = 2,
    CUS_HDR_MODE_EMBEDDED_RAW8 = 3,
    CUS_HDR_MODE_EMBEDDED_RAW10 = 4,
    CUS_HDR_MODE_EMBEDDED_RAW12 = 5,
    CUS_HDR_MODE_EMBEDDED_RAW16 = 6, //Only for OV2718?
    CUS_HDR_MODE_LI = 7,
    CUS_HDR_MODE_MULTI_VC = 8,
}CUS_HDR_MODE;

typedef enum
{
    // Index 0
    SENSOR_DRIVER_MODE_VGA_30P_RESOLUTION,                    // 640*360 30P
    SENSOR_DRIVER_MODE_VGA_50P_RESOLUTION,                    // 640*360 50P
    SENSOR_DRIVER_MODE_VGA_60P_RESOLUTION,                    // 640*360 60P
    SENSOR_DRIVER_MODE_VGA_100P_RESOLUTION,                   // 640*360 100P
    SENSOR_DRIVER_MODE_VGA_120P_RESOLUTION,                   // 640*360 120P

    // Index 5
    SENSOR_DRIVER_MODE_HD_24P_RESOLUTION,                     // 1280*720 24P
    SENSOR_DRIVER_MODE_HD_30P_RESOLUTION,                     // 1280*720 30P
    SENSOR_DRIVER_MODE_HD_50P_RESOLUTION,                     // 1280*720 50P
    SENSOR_DRIVER_MODE_HD_60P_RESOLUTION,                     // 1280*720 60P
    SENSOR_DRIVER_MODE_HD_100P_RESOLUTION,                    // 1280*720 100P

    // Index 10
    SENSOR_DRIVER_MODE_HD_120P_RESOLUTION,                    // 1280*720 120P
    SENSOR_DRIVER_MODE_1600x900_30P_RESOLUTION,               // 1600*900 30P
    SENSOR_DRIVER_MODE_FULL_HD_15P_RESOLUTION,                // 1920*1080 15P
    SENSOR_DRIVER_MODE_FULL_HD_24P_RESOLUTION,                // 1920*1080 24P
    SENSOR_DRIVER_MODE_FULL_HD_25P_RESOLUTION,                // 1920*1080 25P

    // Index 15
    SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION,                // 1920*1080 30P
    SENSOR_DRIVER_MODE_FULL_HD_50P_RESOLUTION,                // 1920*1080 50P
    SENSOR_DRIVER_MODE_FULL_HD_60P_RESOLUTION,                // 1920*1080 60P
    SENSOR_DRIVER_MODE_SUPER_HD_30P_RESOLUTION,               // 2304*1296 30P
    SENSOR_DRIVER_MODE_SUPER_HD_25P_RESOLUTION,               // 2304*1296 25P

    // Index 20
    SENSOR_DRIVER_MODE_SUPER_HD_24P_RESOLUTION,               // 2304*1296 24P
    SENSOR_DRIVER_MODE_1440_30P_RESOLUTION,                   // 2560*1440 30P
    SENSOR_DRIVER_MODE_2D7K_15P_RESOLUTION,                   // 2704*1524 15P
    SENSOR_DRIVER_MODE_2D7K_30P_RESOLUTION,                   // 2704*1524 30P
    SENSOR_DRIVER_MODE_4K2K_15P_RESOLUTION,                   // 3840*2160 15P

    // Index 25
    SENSOR_DRIVER_MODE_4K2K_30P_RESOLUTION,                   // 3840*2160 30P
    SENSOR_DRIVER_MODE_4TO3_VGA_30P_RESOLUTION,               // 640*480   30P
    SENSOR_DRIVER_MODE_4TO3_1D2M_30P_RESOLUTION,              // 1280*960  30P
    SENSOR_DRIVER_MODE_4TO3_1D5M_30P_RESOLUTION,              // 1440*1080 30P
    SENSOR_DRIVER_MODE_4TO3_3M_15P_RESOLUTION,                // 2048*1536 15P

    // Index 30
    SENSOR_DRIVER_MODE_4TO3_3M_30P_RESOLUTION,                // 2048*1536 30P
    SENSOR_DRIVER_MODE_4TO3_5M_15P_RESOLUTION,                // 2560*1920 15P
    SENSOR_DRIVER_MODE_4TO3_5M_30P_RESOLUTION,                // 2560*1920 30P
    SENSOR_DRIVER_MODE_4TO3_8M_15P_RESOLUTION,                // 3264*2448 15P
    SENSOR_DRIVER_MODE_4TO3_8M_30P_RESOLUTION,                // 3264*2448 30P

    // Index 35
    SENSOR_DRIVER_MODE_4TO3_10M_15P_RESOLUTION,               // 3648*2736 15P
    SENSOR_DRIVER_MODE_4TO3_10M_30P_RESOLUTION,               // 3648*2736 30P
    SENSOR_DRIVER_MODE_4TO3_12M_15P_RESOLUTION,               // 4032*3024 15P
    SENSOR_DRIVER_MODE_4TO3_12M_30P_RESOLUTION,               // 4032*3024 30P
    SENSOR_DRIVER_MODE_4TO3_14M_15P_RESOLUTION,               // 4352*3264 15P

    // Index 40
    SENSOR_DRIVER_MODE_4TO3_14M_30P_RESOLUTION,               // 4352*3264 30P
    SENSOR_DRIVER_MODE_4K2K_24P_RESOLUTION,
    SENSOR_DRIVER_MODE_PAL_25P_RESOLUTION,
    SENSOR_DRIVER_MODE_NTSC_30P_RESOLUTION,

    // For Camera Preview
    SENSOR_DRIVER_MODE_BEST_CAMERA_PREVIEW_RESOLUTION,
    SENSOR_DRIVER_MODE_BEST_CAMERA_CAPTURE_16TO9_RESOLUTION,
    SENSOR_DRIVER_MODE_BEST_CAMERA_CAPTURE_4TO3_RESOLUTION,
    SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION_HDR,
} CUS_SNR_RESOLUTION;


#ifdef __cplusplus
extern "C"
#endif
/**@brief ISP sensor interface control API */
typedef struct __ISensorIfAPI //isp sensor interface API
{
    /**@brief Reserved */
    void* pdata;

    /** @brief Set sensor power down pin.
    @param[in] idx Sensor pad ID.
    @param[in] pol pin polarity.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*PowerOff)(u32 idx, CUS_CLK_POL pol);

    /** @brief Set sensor power reset pin.
    @param[in] idx Sensor pad ID.
    @param[in] pol pin polarity.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*Reset)(u32 idx, CUS_CLK_POL pol);

    /** @brief Configure sensor master clock.
    @param[in] idx Sensor pad ID.
    @param[in] bONOFF Clock ON/OFF control.
    @param[in] mclk Clock frequency Hz.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*MCLK)(u32 idx, u8 bONOFF, CUS_MCLK_FREQ mclk);

    /** @brief Query sensor master clock.
    @param[in] idx Sensor pad ID.
    @param[in] mclk Query if clock frequency Hz is available.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*QueryMCLK)(u32 idx, CUS_MCLK_FREQ mclk);

    /** @brief Query MIPI lane number.
    @param[in] idx Sensor pad ID.
    @param[in] lane_num Query max lane number.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*QueryLaneNum)(u32 idx, u8 *max_lane);
#if 0
    /** @brief Select pixel clock source
    @remarks Parallel interface only
    @param[in] handle Handle to sensor driver.
    @param[in] pclk_source Clock source.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*PCLK)(CUS_PCLK_SOURCE pclk_source);		//senect sensor

    /** @brief Configure HSYNC pin polarity.
    @remarks Parallel interface only
    @param[in] handle Handle to sensor driver.
    @param[in] pol pin polarity.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*HsyncPol)(CUS_CLK_POL pol);

    /** @brief [parallel interface only] Configure VSYNC pin polarity.
    @param[in] handle Handle to sensor driver.
    @param[in] pol pin polarity.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*VsyncPol)(CUS_CLK_POL pol);

    /** @brief Configure PCLK pin polarity.
    @param[in] handle Handle to sensor driver.
    @param[in] pol pin polarity.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*ClkPol)(CUS_CLK_POL pol);

    /** @brief Configure sensor bayer raw pixel order
    @param[in] handle Handle to sensor driver.
    @param[in] bayer_id Select pixel order
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*BayerFmt)(CUS_SEN_BAYER bayer_id);

    /** @brief Configure sensor RGBIR (8 or 10bits) convert to 12bit bits isp input, This config will be ignore for 12/16 bits sensor bayer raw input.
    @param[in] handle Handle to sensor driver.
    @param[in] mode Mode select
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*FmtConv)(CUS_SEN_FMT_CONV_MODE mode);

    /** @brief Select sensor image data interface
    @param[in] handle Handle to sensor driver.
    @param[in] bus Sensor interface
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*DataBus)(CUS_SENIF_BUS bus);

    /** @brief Configure sensor bayer raw pixel bits
    @param[in] handle Handle to sensor driver.
    @param[in] prec Select pixel bits
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*DataPrecision)(CUS_DATAPRECISION prec);

    /** @brief Wait isp frame start event
    @param[in] handle Handle to sensor driver.
    @param[in] ms Max timeout in ms
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*WaitVStart)(int ms);

    /** @brief Wait isp frame end event
    @param[in] handle Handle to sensor driver.
    @param[in] ms Max timeout in ms
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*WaitVEnd)(int ms);

    /** @brief Configure 3A task order table
    @param[in] handle Handle to sensor driver.
    @param[in] tasks Task order table
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*Set3ATaskOrder)(CUS_INT_TASK_ORDER tasks);
#endif
    /** @brief Select sensor IO pin assignment
    @param[in] idx Sensor pad ID.
    @param[in] ulSnrType Interface type.
    @param[in] ulSnrPadCfg Pin config.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetIOPad)(u32 idx, CUS_SENIF_BUS ulSnrType, u32 ulSnrPadCfg);

    //FOR CSI

    /** @brief Set maximum mipi data rate (amount of all lans)
    @remarks MIPI interface only.
    @param[in] idx Sensor pad ID.
    @param[in] clk Max data rate.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetCSI_Clk)(u32 idx, CUS_CSI_CLK clk);

    /** @brief Set number of MIPI lanes
    @remarks MIPI interface only.
    @param[in] idx Sensor pad ID.
    @param[in] num_lan Number of lanes.
    @param[in] bon_off Clock ON/OFF control.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetCSI_Lane)(u32 idx, u16 num_lan, u8 bon_off);

    /** @brief Enable long packet type
    @remarks MIPI interface only
    @param[in] idx Sensor pad ID.
    @param[in] ctl_cfg0_15 Control flag bit[0:15]
    @param[in] ctl_cfg16_31 Control flag bit[16:31]
    @param[in] ctl_cfg32_47 Control flag bit[32:47]
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetCSI_LongPacketType)(u32 idx, u16 ctl_cfg0_15, u16 ctl_cfg16_31, u16 ctl_cfg32_47);

    /** @brief Virtual channel0 hsync mode
    @remarks MIPI interface only
    @param[in] idx Sensor pad ID.
    @param[in] mode HSYNC mode.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetCSI_VC0HSmode)(CUS_CSI_VC_HS_MODE mode);

    /** @brief Configure MIPI capture start timing
    @remarks MIPI interface only
    @param[in] idx Sensor pad ID.
    @param[in] rx_clk_skip_ns
    @param[in] rx_data_skip_ns
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetCSI_clk_data_skip)(u32 idx, u8 rx_clk_skip_ns,u8 rx_data_skip_ns);

    /** @brief Configure MIPI hdr mode
    @remarks MIPI interface only
    @param[in] idx Sensor pad ID.
    @param[in] hdr_mode HDR mode.
    @param[in] bon_off Clock ON/OFF control.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetCSI_hdr_mode)(u32 idx, CUS_HDR_MODE hdr_mode, u8 bon_off);

#if 0
    /** @brief Configure sensor RGBIR raw pixel order
    @param[in] handle Handle to sensor driver.
    @param[in] bayer_id Select pixel order
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*RGBIRFmt)(CUS_SEN_RGBIR RGBIR_id);

    /** @brief Skip input frame
    @param[in] handle Handle to sensor driver.
    @param[in] number of frames to skip
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*SetSkipFrame)(int cnt);
#endif
}ISensorIfAPI;

typedef union {
    //Parallel sensor
    struct {
        u32 parallel_reserved; //No additional attribute.
    } attr_parallel;

    //MIPI sensor
    struct {
        u32 mipi_lane_num;
        u32 mipi_data_format; //0: YUV 422 format. 1: RGB pattern.
        u32 mipi_yuv_order; //YUYV or UYVY
        u32 mipi_hsync_mode;
        u32 mipi_sampling_delay; /** < MIPI start sampling delay */ /*bit 0~7: clk_skip_ns. bit 8~15: data_skip_ns*/
        CUS_HDR_MODE mipi_hdr_mode;
        u32 mipi_hdr_virtual_channel_num;
    } attr_mipi;

    //BT656 sensor
    struct {
        u32 bt656_total_ch;
        u32 bt656_cur_ch;
        u32 bt656_ch_det_en;
        CUS_VIF_BT656_CHANNEL_SELECT bt656_ch_det_sel;
        u32 bt656_bit_swap;
        u32 bt656_8bit_mode;
        CUS_VIF_BT656_VSYNC_DELAY bt656_vsync_delay;
        u32 bt656_hsync_inv;
        u32 bt656_vsync_inv;
        u32 bt656_clamp_en;
    } attr_bt656;
}  InterfaceAttr_u;


///////////////////////////////////////////////////////

/** @brief Sensor driver interface \n
The function here are implemented by sensor driver.
*/
typedef struct __ms_cus_sensor{
    short     lib_idx;      /**< Reserved , Do not use */
    char    model_id[32]; /**< Please fill the sensor modle id string here then libcamera user can read model_id by using cameraGetSensorModelID() .*/
    void      *sen_handle;  /**< Reserved , Do not use */
    void      *private_data;/**< sensor driver dependent variables should store in private_data and free when release */

    app_i2c_cfg i2c_cfg;    /**< Sensor i2c setting */
    i2c_handle_t  *i2c_bus; /**< Handle to sensor i2c API. */
    ISensorIfAPI sensor_if_api;   /**< sensor interface API */
    // ae parameters
    int ae_gain_delay; /**< How many frame delay from writing AE gain to take effect*/
    int ae_shutter_delay;/**< How many frame delay from writing AE shutter to take effect*/

    int ae_gain_ctrl_num;
    int ae_shutter_ctrl_num;

    // polarity
    CUS_CLK_POL reset_POLARITY;   /** < Sensor REST pin polarity */
    CUS_CLK_POL pwdn_POLARITY;   /** < Sensor Power Down pin polarity */
    CUS_CLK_POL VSYNC_POLARITY;   /** < Sensor VSYNC polarity */
    CUS_CLK_POL HSYNC_POLARITY;   /** < Sensor HSYNC pin polarity */
    CUS_CLK_POL PCLK_POLARITY;   //** < Sensor Plxel Clock pin polarity */

    // sensor data
    CUS_CAMSENSOR_ISPTYPE       isp_type;    /** < Always use ISP_EXT */
    CUS_SEN_BAYER               bayer_id;    /** < Sensor bayer raw pixel order */
    CUS_SEN_RGBIR               RGBIR_id;    /** < Sensor bayer raw pixel order */
    CUS_SENIF_BUS               sif_bus;     /** < Select sensor interface */
    CUS_DATAPRECISION           data_prec;   /** < Raw data bits */
    CUS_SEN_FMT_CONV_MODE       data_mode;   /** < Data convert mode*/
    CUS_CAMSENSOR_ORIT          orient;      /** < Sensor mirror/flip */
    u32                   mclk;        /** < Sensor master clock frequency */

    cus_camsensor_res_list video_res_supported; /** < Resolution list */
   //sensor calibration
    u32 sat_mingain;    /**< Minimum AE gain */

    CUS_SENSOR_PAD_GROUP snr_pad_group;
    u32 channel_num; //Which VIF/ISP channel is occupied.
    u32 channel_mode; //real-time, RAW store, HDR...

    InterfaceAttr_u interface_attr;

    ////////////////////////////////////////////////
    // system functions
    ////////////////////////////////////////////////

    /** @brief Sensor power on sequence, I2C must be ready after calling this function
    @param[in] handle Handle to sensor driver.
    @remark Following configuration need to set up at this stage \n
    @ref __ISensorIfAPI::Reset Reset sensor \n
    @ref __ISensorIfAPI::PowerOff Sensor power down pin \n
    @ref __ISensorIfAPI::MCLK Sensor master clock \n
    @ref __ISensorIfAPI::SetIOPad ISP sensor IO \n
    @ref __ISensorIfAPI::SetCSI_Clk  [MIPI sensor only] MIPI clock\n
    @ref __ISensorIfAPI::HsyncPol Hsync polarity\n
    @ref __ISensorIfAPI::VsyncPol Vsync polarity\n
    @ref __ISensorIfAPI::ClkPol [Parallel sensor only]  Pixel clock polarity\n
    @ref __ISensorIfAPI::BayerFmt Raw data format\n
    @ref __ISensorIfAPI::DataPrecision Raw data pixel bits\n
    @ref __ISensorIfAPI::FmtConv Raw data to ISP pixel convert\n
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_poweron)(struct __ms_cus_sensor* handle, u32 idx);

    /** @brief Sensor power off
    @param[in] handle Handle to sensor driver.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_poweroff)(struct __ms_cus_sensor* handle, u32 idx);

    /** @brief Sensor initialization
    @param[in] handle Handle to sensor driver.
    @retval SUCCESS or FAIL if error occurs.
    @remark Fill sensor initial table here, Sensor beginning to output raw images after calling this function .
    */
    int (*pCus_sensor_init)(struct __ms_cus_sensor* handle);

    /** @brief Release resources those allocated in cus_camsensor_init_handle()
    @param[in] handle Handle to sensor driver.
    @retval SUCCESS or FAIL if error occurs.
    @remark Release resource allocated in \ref cus_camsensor_init_handle
    */
    int (*pCus_sensor_release)(struct __ms_cus_sensor* handle);

    /** @brief Enter sensor suspend mode
    @param[in] handle Handle to sensor driver.
    @retval SUCCESS or FAIL if error occurs.
    @remark ISP call this function before enter power saving mode
    */
    int (*pCus_sensor_suspend)(struct __ms_cus_sensor* handle);

    /** @brief Sensor wakeup
    @param[in] handle Handle to sensor driver.
    @retval SUCCESS or FAIL if error occurs.
    @remark ISP call this function after exit power saving mode
    */
    int (*pCus_sensor_resume)(struct __ms_cus_sensor* handle);

    /** @brief Enable sensor pattern mode if sensor hardward supported
    @param[in] handle Handle to sensor driver.
    @param[in] mode Pattern select, if 0 disable pattern mode.
    @retval SUCCESS or FAIL if error occurs.
    @remark This function is optional
    */
    int (*pCus_sensor_SetPatternMode)(struct __ms_cus_sensor* handle,u32 mode);

    // Normal

    /** @brief Check sensor ID and report to ISP sensor match or not
    @param[in] handle Handle to sensor driver.
    @param[out] id Receive 4 bytes customer defined sensor ID.
    @retval Return SUCCESS if sensor matched or Retuen FAIL if sensor mismatch.
    @remark Read sensor ID through I2C
    */
    int (*pCus_sensor_GetSensorID)(struct __ms_cus_sensor* handle, u32 *id);

     /** @brief Get resolution list
    @param[in] handle Handle to sensor driver.
    @param[out] id Receive supported resolution list
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_GetVideoRes)(struct __ms_cus_sensor* handle, u32 res_idx, cus_camsensor_res **res);

     /** @brief Get resolution list
    @param[in] handle Handle to sensor driver.
    @param[out] id Receive supported resolution list
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_GetCurVideoRes)(struct __ms_cus_sensor* handle, u32 *cur_idx, cus_camsensor_res **res);

     /** @brief Select a sensor output resolution sensor list
    @param[in] handle Handle to sensor driver.
    @param[in] res_id Resolution id
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_SetVideoRes)(struct __ms_cus_sensor* handle, u32 res_id);

     /** @brief Get sensor current mirror flip setting
    @param[in] handle Handle to sensor driver.
    @param[out] ori Receive Mirror/Flip setting.
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_GetOrien)(struct __ms_cus_sensor* handle, CUS_CAMSENSOR_ORIT *ori);

     /** @brief Select a sensor mirror flip
    @param[in] handle Handle to sensor driver.
    @param[in] ori Mirror/Flip configuration.
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_SetOrien)(struct __ms_cus_sensor* handle, CUS_CAMSENSOR_ORIT ori);

     /** @brief Get sensor capability
    @param[in] handle Handle to sensor driver.
    @param[out] cap Receive sensor capability
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_GetSensorCap)(struct __ms_cus_sensor* handle, CUS_CAMSENSOR_CAP *cap);

    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds

     /** @brief AE/Frame status change notification
    @param[in] handle Handle to sensor driver.
    @param[in] status Current status
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_AEStatusNotify)(struct __ms_cus_sensor* handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);

     /** @brief Get sensor shutter setting in us
    @param[in] handle Handle to sensor driver.
    @param[out] us Receive current shutter setting
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_GetAEUSecs)(struct __ms_cus_sensor* handle, u32 *us);

     /** @brief Set sensor shutter in us
    @param[in] handle Handle to sensor driver.
    @param[in] us Shutter setting in us
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_sensor_SetAEUSecs)(struct __ms_cus_sensor* handle, u32 us);

    // Gain: 1x = 1024
    /** @brief Get sensor current AE gain
    @param[in] handle Handle to sensor driver.
    @param[out] gain Receive current AE gain
    @retval Return SUCCESS or FAIL if error occurs.
    @remark gain: 1x = 1024
    */
    int (*pCus_sensor_GetAEGain)(struct __ms_cus_sensor* handle, u32* gain);

    /** @brief Set sensor AE gain
    @param[in] handle Handle to sensor driver.
    @param[in] gain AE gain
    @retval Return SUCCESS or FAIL if error occurs.
    @remark gain: 1x = 1024
    */
    int (*pCus_sensor_SetAEGain)(struct __ms_cus_sensor* handle, u32 gain);

    /** @brief Get supported shutter range
    @param[in] handle Handle to sensor driver.
    @param[out] min Receive minimum shutter which sensor can supported
    @param[out] min Receive maxiimum shutter which sensor can supported
    @retval Return SUCCESS or FAIL if error occurs.
    @remark gain: 1x = 1024
    */
    int (*pCus_sensor_GetAEMinMaxUSecs)(struct __ms_cus_sensor* handle, u32 *min, u32 *max);

    /** @brief Get supported AE gain range
    @param[in] handle Handle to sensor driver.
    @param[out] min Receive minimum gain which sensor can supported
    @param[out] min Receive maxiimum gain which sensor can supported
    @retval Return SUCCESS or FAIL if error occurs.
    @remark gain: 1x = 1024
    */
    int (*pCus_sensor_GetAEMinMaxGain)(struct __ms_cus_sensor* handle, u32 *min, u32 *max);

    // frame rate control
    /** @brief Get current fps
    @param[in] handle Handle to sensor driver.
    @retval Return current frame rate per second
    */
    int (*pCus_sensor_GetFPS)(struct __ms_cus_sensor* handle);

    /** @brief Set sensor output fps
    @param[in] handle Handle to sensor driver.
    @param[in] fps
    @retval Return SUCCESS or FAIL if fps is out of range.
    */
    int (*pCus_sensor_SetFPS)(struct __ms_cus_sensor* handle, u32 fps);

    //[OPTIONAL] sensor calibration
    /** @brief Optional function */
    int (*pCus_sensor_SetAEGain_cal)(struct __ms_cus_sensor* handle, u32);

    /** @brief Optional function */
    int (*pCus_sensor_setCaliData_gain_linearity)(struct __ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray ,u32 num);

    //Get shutter information
    /** @brief Get shutter information
    @param[in] handle Handle to sensor driver.
    @param[out] info return shutter information.
    @retval Return current frame rate per second
    */
    int (*pCus_sensor_GetShutterInfo)(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info);

    /** @brief Get resolution list number
   @param[in] handle Handle to sensor driver.
   @param[out] ulres_num resolution list number
   @retval Return SUCCESS or FAIL if error occurs.
   */
   int (*pCus_sensor_GetVideoResNum)(struct __ms_cus_sensor* handle, u32 *ulres_num);

   //Get shutter information
   /** @brief Sensor vendor command
   @param[in] handle Handle to sensor driver.
   @param[in] reserved
   @param[in] param Command input
   @param[out] out Command output
   @retval Return SUCCESS or FAIL if error occurs.
   */
   int (*pCus_sensor_CustDefineFunction)(struct __ms_cus_sensor* handle,u32 cmd_id, void *param);

   //Get Source Type
   /** @brief Get Source Type
   @param[in] handle Handle to sensor driver.
   @param[out] psrc_type info
   @retval Return SUCCESS or FAIL if error occurs.
   */
   int (*pCus_sensor_GetSrcType)(struct __ms_cus_sensor* handle, CUS_SNR_ANADEC_SRC_TYPE *psrc_type);

} ms_cus_sensor;

/** @brief Sensor driver entry. ISP call this function before start using sensor driver. \n
ISP pass \ref ms_cus_sensor struct to obtain the driver information and function entries. \n
And all allocated resources here should be released at \ref __ms_cus_sensor::pCus_sensor_release.
Every sensor driver must implement this api.
@param[in] drv_handle A uninitialized \ref ms_cus_sensor struct from ISP, Sensor driver fill the driver information and function entries to drv_handle.
@retval SUCCESS or FAIL if error occurs.
*/

typedef int (*SensorInitHandle)(ms_cus_sensor* handle);

#ifdef __cplusplus
}
#endif

#endif /* DRV_MS_CUS_SENSOR_H_ */
/** @} */ // end of ISP Sensor Driver Interface
