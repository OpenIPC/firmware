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

/*! @file isp_i2c_api.h
      @brief This file contains Infinity ISP I2C basic API.
*/

/** @defgroup group1 ISP I2C API
* @{
*/

#ifndef ISP_I2C_API_H
#define ISP_I2C_API_H

#define SENSOR_I2C_SUCCESS (0)
#define SENSOR_I2C_FAIL (-1)
#define SENSOR_I2C_NOT_SUPPORT (-2)

/*! @brief I2C API handle.*/
struct __i2c_handle_t;

/*! @brief I2C batch read/write data.*/
typedef struct _I2C_ARRAY{
    u16 reg; /**< Register address.*/
    u16 data; /**< Data.*/
} I2C_ARRAY;

/*! @brief I2C burst read/write data.*/
typedef struct _I2C_ARRAY_BURST{
    u16 reg; /**< Register address.*/
    u16 *data; /**< Data.*/
} I2C_ARRAY_BURST;

/*! @brief I2C data direction*/
typedef enum {
    I2C_RW_R, /**< Data direction read.*/
    I2C_RW_W,/**< Data direction write.*/
} I2C_RW;

/*! @brief Use for i2c_array_rw only */
typedef struct _I2C_CPX_ARRAY{
    I2C_RW  rw; /**< Data direction */
    u16 reg; /**< Device register address */
    u16 data; /**< Data write to device or read from device*/
}__attribute__((packed, aligned(1))) I2C_CPX_ARRAY;

/*! @brief Internal use for I2C API*/
typedef enum {
    I2C_FMT_A8D8, /**< 8 bits Address, 8 bits Data */
    I2C_FMT_A16D8,/**< 16 bits Address 8 bits Data */
    I2C_FMT_A8D16,/**< 8 bits Address 16 bits Data */
    I2C_FMT_A16D16,/**< 16 bits Address 16 bits Data */
    I2C_FMT_END/**< Reserved */
} ISP_I2C_FMT;

/*! @brief ISP_I2C_MODE Internal use for I2C API*/
typedef enum {
    I2C_LEGACY_MODE,    /**< Do not use */
    I2C_NORMAL_MODE     /**< Sensor driver can only use I2C_NORMAL_MODE */
} ISP_I2C_MODE;


/*! @brief app_i2c_cfg I2C setting for sensor and bus.*/
typedef struct _app_i2c_cfg{
    ISP_I2C_MODE        mode;   //!< I2C_NORMAL_MODE only
    ISP_I2C_FMT         fmt;        //!< I2C data format
    u32                 speed;          //!< I2C clock in Hz
    u16                 address;       //!< Sensor slave address , bit[7~1] are available, bit[0] user don't care
    u16  reserved;
}__attribute__((packed, aligned(1))) app_i2c_cfg;

/*! @brief The interface of I2C APIs export to user*/
typedef struct _i2c_handle_t {
    int version;
    void *pdata; //i2c_private_data

    u32 nSensorID;
    /** @brief Open isp i2c port. This function must be called before using isp I2C  APIs.
    Call i2c_close to close isp i2c port and allocated resource.
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C initial configuration.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_open)(struct _i2c_handle_t* handle, app_i2c_cfg *cfg);

    /** @brief Close isp I2C port. Call this functon to release resource which allocated form i2c_open.
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C configuration mode and spped are necessary in this stage.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_close)(struct _i2c_handle_t* handle );

    /** @brief Write single data to device.
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C config, fmd and address are necessary in this stage.
    @param[in] reg Device register address address width depend on cfg->fmt.
    @param[in] data Data to write, data width depend on cfg->fmt.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_tx)(struct _i2c_handle_t* handle , app_i2c_cfg cfg, short reg, short data);

    /** @brief Read single data from device.
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C config, fmd and address are necessary in this stage.
    @param[in] reg Device register address address width depend on cfg->fmt.
    @param[out] data Data buffer for read, data width depend on cfg->fmt.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_rx)(struct _i2c_handle_t* handle, app_i2c_cfg cfg, short reg, volatile short *data);

    /** @brief Batch write to device.
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C config, fmd and address are necessary in this stage.
    @param[in] pdata Data array.
    @param[in] len Array size.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_array_tx)(struct _i2c_handle_t* handle, app_i2c_cfg cfg, I2C_ARRAY *pdata, int len);

    /** @brief Burst write to device.
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C config, fmd and address are necessary in this stage.
    @param[in] pdata Data array.
    @param[in] len Array size.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_burst_tx)(struct _i2c_handle_t* handle, app_i2c_cfg cfg, I2C_ARRAY_BURST *pdata, int len);

    /** @brief Batch read from device.
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C config, fmd and address are necessary in this stage.
    @param[out] pdata Data array.
    @param[in] len Array size.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_array_rx)(struct _i2c_handle_t* handle, app_i2c_cfg cfg, I2C_ARRAY *pdata, int len);

    /** @brief Composite batch read/write
    @param[in] handle Handle to isp i2c api.
    @param[in] cfg I2C config, fmd and address are necessary in this stage.
    @param[in][out] pdata Data description array.
    @param[in] len Array size.
    @retval SENSOR_I2C_SUCCESS or SENSOR_I2C_FAIL if error occurs.
    */
    int (*i2c_array_rw)(struct _i2c_handle_t* handle, app_i2c_cfg cfg, I2C_CPX_ARRAY *pdata, int len);
} i2c_handle_t;

/** @} */ // end of ISP I2C API

#endif
