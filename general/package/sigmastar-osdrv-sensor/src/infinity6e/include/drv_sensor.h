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

#ifndef DRV_SENSOR_H
#define DRV_SENSOR_H

#include <cam_os_wrapper.h>

#define SENSOR_DMSG(args...) do{}while(0)
#define SENSOR_EMSG(args...) CamOsPrintf(args)
#define SENSOR_IMSG(args...) CamOsPrintf(args)
#if 0
typedef enum {
    I2C_FMT_A8D8, /**< 8 bits Address, 8 bits Data */
    I2C_FMT_A16D8,/**< 16 bits Address 8 bits Data */
    I2C_FMT_A8D16,/**< 8 bits Address 16 bits Data */
    I2C_FMT_A16D16,/**< 16 bits Address 16 bits Data */
    I2C_FMT_END/**< Reserved */
} ISP_I2C_FMT;
#endif

extern s32 WriteRegisterPair(int slaveAddr, short reg, unsigned short value, ISP_I2C_FMT fmt);
extern u64 intlog10(u32 value);
extern u64 intlog2(u32 value);
extern s32 DrvRegisterSensorDriver(u32 nCamID, SensorInitHandle pfnSensorInitHandle);
extern s32 DrvRegisterPlaneDriver(u32 nCamID, u32 nPlaneID, SensorInitHandle pfnSensorInitHandle);
extern s32 DrvRegisterSensorDriverEx(u32 nSNRPadID, SensorInitHandle pfnSensorInitHandle, void *pPrivateData);
extern s32 DrvRegisterPlaneDriverEx(u32 nSNRPadID, u32 nPlaneID, SensorInitHandle pfnSensorInitHandle, void *pPrivateData);
extern s32 DrvSensorRelease(u32 nSNRPadID);
extern s32 DrvSensorEarlyInitSync(u32 nSNRPadID);
extern s32 DrvRegisterSensorI2CSlaveID(u32 nCamID, u32 Slaveid);
extern s32 DrvRegisterPlaneI2CSlaveID(u32 nCamID, u32 nPlaneID, u32 Slaveid);
extern s32 DrvSensorHandleVer(u32 version_major, u32 version_minor);
extern s32 DrvSensorIFVer(u32 version_major, u32 version_minor);
extern s32 DrvSensorI2CVer(u32 version_major, u32 version_minor);
#define DrvUnregisterSensorDriverEx(nSNRPadID) DrvSensorRelease((nSNRPadID))

#define SENSOR_USLEEP(us) CamOsUsSleep(us)
#define SENSOR_MSLEEP(ms) CamOsMsSleep(ms)
#define SENSOR_UDELAY(us) CamOsUsDelay(us)
#define SENSOR_MDELAY(ms) CamOsMsDelay(ms)
#endif
