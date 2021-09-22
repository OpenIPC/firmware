/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_sns_ctrl.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/7/6

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/7/6
  Author		: Lycai
  Modification	: creat
******************************************************************************/


#ifndef __XM_SNS_CTRL_H__
#define __XM_SNS_CTRL_H__

#include "xm_type.h"
#include "xm_comm_sns.h"
#include "xm_ae_comm.h"
#include "xm_i2c.h"
#include "xm_defines.h"
#include "mpi_phyvi.h"
#define HD720P_LINES  	(750)
#define HD1080P_LINES 	(1125)
#define CVBS1080_60P_LINES   572
#define CVBS1080_50P_LINES  840 
#define CVBS720_60P_LINES   780
#define CVBS720_50P_LINES   750

void sensor_init();
void sensor_init_boot();
void sensor_prog(int *rom);
XM_S32 sensor_write_register(XM_U32 addr, XM_U32 data);
XM_S32  sensor_read_register(XM_U32 addr);
XM_S32 sensor_write_register_sch(XM_U8,XM_U32 addr, XM_U32 data);
XM_S32  sensor_read_register_sch(XM_U8,XM_U32 addr);
int  sensor_write_register_bit(int addr, int data, int mask);
int sensor_register_callback(ISP_DEV IspDev); 
int sensor_unregister_callback(ISP_DEV IspDev);
int  sensor_get_chip(I2C_DATA_S *pstI2CData);
void  sensor_set_chip(XM_S32 s32SnsChip);
int sensor_mode_set(XM_U8 u8Mode);
int  sensor_mode_get();

int  sensor_get_temp(XM_S16 *ps16Temp);
void  sensor_set_dnMode(XM_U8 u8Mode);
int	sensor_set_encoderinfo(XM_U8 *u8pDta);
void sensor_set_fps(XM_U8 channel , XM_U8 frame);
void sensor_set_isp_para(XM_U8 u8Mode);
void sensor_set_ioAbility(XM_U8 u8Level);
void sensor_set_VicutByNickname(XM_U32 u32Nickname);
XM_S32 sensor_set_reslotionType(PROFILE enResType);
void sensor_restart(XM_U8 u8Mode);
XM_S32 cmos_set_sns_attr(ISP_CMOS_SNS_ATTR_S *pstSnsAttr);
XM_S32 cmos_get_sns_attr(ISP_CMOS_SNS_ATTR_S *pstSnsAttr);


#endif /* __XM_SNS_CTRL_H__ */

