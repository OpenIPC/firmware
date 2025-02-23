/*
 * copyright (c) hisilicon technologies co., ltd. 2016-2019. all rights reserved.
 * description: serdes_comm.h
 * author: hisilicon multimedia software group
 * create: 2019-05-17
 */

#ifndef __HI_SERDES_COMM_H__
#define __HI_SERDES_COMM_H__

#include "hi_osal.h"

#define THINE_DEVICE_TYPE   "241a-242"
#define THINE_DEVICE_ADDR   0x16

#define SERDES_MAX_NUM      2
extern unsigned char sensor_dev_addr[SERDES_MAX_NUM];

int serdes_i2c_write(unsigned char i2c_dev, unsigned char dev_addr,
                     unsigned int reg_addr, unsigned int reg_addr_num,
                     unsigned int data, unsigned int data_byte_num);

int serdes_i2c_read(unsigned char i2c_dev, const unsigned char dev_addr,
                    const unsigned int reg_addr, unsigned int *data);

void serdes_thine_init(int i2c_dev, serdes_mode_t serdes_mode);

void serdes_thine_exit(int i2c_dev);


#endif // __HI_SERDES_COMM_H__
