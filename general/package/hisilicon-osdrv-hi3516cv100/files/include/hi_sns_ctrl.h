/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_sns_ctrl.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/01/10
  Description   : 
  History       :
  1.Date        : 2011/01/10
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_SNS_CTRL_H__
#define __HI_SNS_CTRL_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

void sensor_init();
void sensor_prog(int *rom);
int  sensor_write_register(int addr, int data);
int  sensor_read_register(int addr);
int  sensor_write_register_bit(int addr, int data, int mask);
int  sensor_register_callback(void); 
int sensor_unregister_callback(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_SNS_CTRL_H__ */

