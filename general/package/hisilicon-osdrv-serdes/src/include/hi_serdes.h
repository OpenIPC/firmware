/*
 * copyright (c) hisilicon technologies co., ltd. 2016-2019. all rights reserved.
 * description: hi_serdes.h
 * author: hisilicon multimedia software group
 * create: 2019-05-17
 */

#ifndef __HI_SERDES_H__
#define __HI_SERDES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /*end of #ifdef __cplusplus*/

typedef unsigned int serdes_dev_t;

typedef enum
{
    SERDES_MODE_4LANE_LINEAR = 0,
    SERDES_MODE_4LANE_WDR,
    SERDES_MODE_2LANE_LINEAR,
    SERDES_MODE_2LANE_WDR,

    SERDES_MODE_BUTT,
} serdes_mode_t;

typedef struct
{
    serdes_dev_t        devno;           /* I2C_DEV */
    serdes_mode_t       serdes_mode;
    unsigned char       sendes_en;
    unsigned char       sensor_i2c_addr; /* serdes connected sensor's i2c addr */
} serdes_dev_attr_t;

#define HI_SERDES_IOC_MAGIC         'n'

/* init data lane, input mode, data type */
#define HI_SERDES_START              _IOW(HI_SERDES_IOC_MAGIC, 0x01, serdes_dev_attr_t)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*end of #ifdef __cplusplus*/

#endif /*end of #ifdef __HI_SERDES_H__*/
