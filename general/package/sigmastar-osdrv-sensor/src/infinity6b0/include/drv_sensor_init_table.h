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

#ifndef _DRV_SENSOR_INIT_H_
#define _DRV_SENSOR_INIT_H_

//#define SENSOR_INIT_CMDQ_MODE
#define SENSOR_INIT_DRV_MODE

/* for sensor driver */
#define I2C_1A1B_W(reg,val) {(reg),(val)}
#define CMDQ_DELAY_MS(ms) {(0xFFFF),(ms)}

#define I2CM_1A1D_W(slave_addr,reg,val) \
        {(reg),(val)}

#define I2CM_1A1D_W_BURST2(slave_addr,reg,d0,d1) \
        {(reg),(d0)},\
        {(reg)+1,(d1)}

#define I2CM_1A1D_W_BURST3(slave_addr,reg,d0,d1,d2) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)}

#define I2CM_1A1D_W_BURST4(slave_addr,reg,d0,d1,d2,d3) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)},\
        {(reg)+3,(d3)}

#define I2CM_1A1D_W_BURST5(slave_addr,reg,d0,d1,d2,d3,d4) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)},\
        {(reg)+3,(d3)},\
        {(reg)+4,(d4)}

#define I2CM_1A1D_W_BURST6(slave_addr,reg,d0,d1,d2,d3,d4,d5) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)},\
        {(reg)+3,(d3)},\
        {(reg)+4,(d4)},\
        {(reg)+5,(d5)}

#define I2CM_1A1D_W_BURST7(slave_addr,reg,d0,d1,d2,d3,d4,d5,d6) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)},\
        {(reg)+3,(d3)},\
        {(reg)+4,(d4)},\
        {(reg)+5,(d5)},\
        {(reg)+6,(d6)}

#define I2CM_2A1D_W(slave_addr,reg,val) \
        {(reg),(val)}

#define I2CM_2A1D_W_BURST2(slave_addr,reg,d0,d1) \
        {(reg),(d0)},\
        {(reg)+1,(d1)}

#define I2CM_2A1D_W_BURST3(slave_addr,reg,d0,d1,d2) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)}

#define I2CM_2A1D_W_BURST4(slave_addr,reg,d0,d1,d2,d3) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)},\
        {(reg)+3,(d3)}

#define I2CM_2A1D_W_BURST5(slave_addr,reg,d0,d1,d2,d3,d4) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)},\
        {(reg)+3,(d3)},\
        {(reg)+4,(d4)}

#define I2CM_2A1D_W_BURST6(slave_addr,reg,d0,d1,d2,d3,d4,d5) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)},\
        {(reg)+3,(d3)},\
        {(reg)+4,(d4)},\
        {(reg)+5,(d5)}

#define I2CM_2A2D_W(slave_addr,reg,val) \
        {(reg),(val)}

#define I2CM_2A2D_W_BURST2(slave_addr,reg,d0,d1) \
        {(reg),(d0)},\
        {(reg)+1,(d1)}

#define I2CM_2A2D_W_BURST3(slave_addr,reg,d0,d1,d2) \
        {(reg),(d0)},\
        {(reg)+1,(d1)},\
        {(reg)+2,(d2)}

#define SENSOR_INIT_TABLE I2C_ARRAY

#endif
