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

#ifndef _SENSOR_OS_WRAPPER_H_
#define _SENSOR_OS_WRAPPER_H_

#if defined(__KERNEL__)
#define CAM_OS_LINUX_KERNEL
#endif

#include <cam_os_wrapper.h>
#include <drv_ms_cus_sensor.h>
#include <drv_sensor.h>

/******************************************************** RTK ***********************************************************/
#if defined(CAM_OS_RTK)
typedef unsigned char bool;
#ifndef true
#define true                        1
#define false                       0
#endif

#ifndef _SENSOR_SLAVE_ID_
#define _SENSOR_SLAVE_ID_ 0
#endif

#define SENSOR_DRV_ENTRY_IMPL_BEGIN(NAME)

#define SENSOR_DRV_ENTRY_IMPL_END(Name,LinearEntry,HdrSefEntry,HdrLefEntry)\
int Name##_init_driver(unsigned char chmap)\
{\
    int nCamID=0;\
    /*To avoid compile warning*/\
    void* p0 = LinearEntry;\
    void* p1 = HdrSefEntry;\
    void* p2 = HdrLefEntry;\
    for(nCamID=0;nCamID<4;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){ DrvRegisterSensorDriver(nCamID, LinearEntry);\
                DrvRegisterSensorI2CSlaveID(nCamID, (_SENSOR_SLAVE_ID_>>(nCamID*8))&0xFF);\
                SENSOR_DMSG("Connect %s linear to sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
            if(p1){ DrvRegisterPlaneDriver(nCamID, 1, HdrSefEntry);\
                DrvRegisterPlaneI2CSlaveID(nCamID, 1, (_SENSOR_SLAVE_ID_>>(nCamID*8))&0xFF);\
                SENSOR_DMSG("Connect %s SEF to vif sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
            if(p2){ DrvRegisterPlaneDriver(nCamID, 0, HdrLefEntry);\
                DrvRegisterPlaneI2CSlaveID(nCamID, 0, (_SENSOR_SLAVE_ID_>>(nCamID*8))&0xFF);\
                SENSOR_DMSG("Connect %s LEF to sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
        }\
    }\
    return 0;\
}

static volatile int lane_num = 2;
static volatile int hdr_lane_num=2;

#define SENSOR_DRV_PARAM_MCLK() "NONE"
#define SENSOR_USLEEP_(us) {CamOsUsDelay(us);}
#define SENSOR_MSLEEP_(ms) {CamOsMsDelay(ms);}

/*Extension version*/
#define SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(NAME) SENSOR_DRV_ENTRY_IMPL_BEGIN(NAME)

#define MAX_CAMID_LEN 4
#define SENSOR_DRV_ENTRY_IMPL_END_EX(Name,LinearEntry,HdrSefEntry,HdrLefEntry,PrivateDataType)\
static PrivateDataType* g_pData[2][MAX_CAMID_LEN] = {{0,0,0,0}, {0,0,0,0}};\
static PrivateDataType g_PrivData[2][MAX_CAMID_LEN];\
int Name##_init_driver(unsigned char chmap)\
{\
    int nCamID=0;\
    /*To avoid compile warning*/\
    void* p0 = LinearEntry;\
    void* p1 = HdrSefEntry;\
    void* p2 = HdrLefEntry;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){\
                /*void* pData = CamOsMemAlloc(sizeof(PrivateDataType));*/\
                void* pData = &g_PrivData[0][nCamID];/*Change private data to static var*/\
                memset(pData,0,sizeof(PrivateDataType));\
                DrvRegisterSensorDriverEx(nCamID, LinearEntry,pData);\
                DrvRegisterSensorI2CSlaveID(nCamID, (_SENSOR_SLAVE_ID_>>(nCamID*8))&0xFF);\
                SENSOR_DMSG("Connect %s linear to sensor pad %d\n",__FUNCTION__, nCamID);\
                g_pData[0][nCamID] = pData;\
            }\
            if(p1||p2){\
                /*void* pData = CamOsMemAlloc(sizeof(PrivateDataType));*/\
                void* pData = &g_PrivData[1][nCamID];/*Change private data to static var*/\
                memset(pData,0,sizeof(PrivateDataType));\
                if(p1){\
                    DrvRegisterPlaneDriverEx(nCamID, 1, HdrSefEntry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 1, (_SENSOR_SLAVE_ID_>>(nCamID*8))&0xFF);\
                    SENSOR_DMSG("Connect %s SEF to vif sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                if(p2){\
                    DrvRegisterPlaneDriverEx(nCamID, 0, HdrLefEntry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 0, (_SENSOR_SLAVE_ID_>>(nCamID*8))&0xFF);\
                    SENSOR_DMSG("Connect %s LEF to sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                g_pData[1][nCamID] = pData;\
            }\
            DrvSensorEarlyInitSync(nCamID);\
        }\
    }\
    return 0;\
}\
\
int Name##_deinit_driver(unsigned char chmap)\
{\
    int nCamID=0;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(g_pData[0][nCamID] || g_pData[1][nCamID])\
            {\
                DrvUnregisterSensorDriverEx(nCamID);\
                if(g_pData[0][nCamID]){\
                    /*CamOsMemRelease((void*)g_pData[0][nCamID]);*/\
                    g_pData[0][nCamID] = 0;\
                }\
                if(g_pData[1][nCamID]){\
                    /*CamOsMemRelease((void*)g_pData[1][nCamID]);*/\
                    g_pData[1][nCamID] = 0;\
                }\
            }\
        }\
    }\
    return 0;\
}
/******************************************************** LINUX ***********************************************************/
#elif defined(CAM_OS_LINUX_KERNEL)
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>
#define SENSOR_DRV_ENTRY_IMPL_BEGIN(Name) \
int chmap = 0;\
module_param(chmap, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);\
MODULE_PARM_DESC(chmap, "VIF channel mapping");\
char *mclk = "use default parameter";\
module_param(mclk, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);\
MODULE_PARM_DESC(mclk, "Assign MCLK");
int lane_num = 2;\
module_param(lane_num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);\
MODULE_PARM_DESC(lane_num, "sensor output lane number");
int hdr_lane_num = 4;\
module_param(hdr_lane_num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);\
MODULE_PARM_DESC(hdr_lane_num, "sensor output lane number");
int i2c_slave_id = 0;\
module_param(i2c_slave_id, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);\
MODULE_PARM_DESC(i2c_slave_id, "sensor i2c slave id");

#define SENSOR_DRV_ENTRY_IMPL_END(Name,LinearEntry,HdrSefEntry,HdrLefEntry) \
static int __init Name##_init_driver(void)\
{\
    int nCamID=0;\
    void* p0 = LinearEntry;\
    void* p1 = HdrSefEntry;\
    void* p2 = HdrLefEntry;\
    if(DrvSensorHandleVer(CUS_CAMSENSOR_HANDLE_MAJ_VER, CUS_CAMSENSOR_HANDLE_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorIFVer(CUS_CAMSENSORIF_MAJ_VER, CUS_CAMSENSORIF_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorI2CVer(CUS_CAMSENSOR_I2C_MAJ_VER, CUS_CAMSENSOR_I2C_MIN_VER)==FAIL)\
    return FAIL;\
    for(nCamID=0;nCamID<4;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){ DrvRegisterSensorDriver(nCamID, LinearEntry);\
            DrvRegisterSensorI2CSlaveID(nCamID, (i2c_slave_id>>(nCamID*8))&0xFF);\
            pr_info("Connect %s linear to sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
            if(p1){ DrvRegisterPlaneDriver(nCamID, 1, HdrSefEntry);\
            DrvRegisterPlaneI2CSlaveID(nCamID, 1, (i2c_slave_id>>(nCamID*8))&0xFF);\
            pr_info("Connect %s SEF to vif sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
            if(p2){ DrvRegisterPlaneDriver(nCamID, 0, HdrLefEntry);\
            DrvRegisterPlaneI2CSlaveID(nCamID, 0, (i2c_slave_id>>(nCamID*8))&0xFF);\
            pr_info("Connect %s LEF to sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
        }\
    }\
    return 0;\
}\
static void __exit Name##_exit_driver(void)\
{\
    pr_info("sensordrv exit");\
}\
subsys_initcall(Name##_init_driver);\
module_exit(Name##_exit_driver);\
MODULE_DESCRIPTION("Sensor_"#Name);\
MODULE_AUTHOR("SigmaStar");\
MODULE_LICENSE("Proprietary");

#define SENSOR_DRV_PARAM_MCLK() (mclk)
#define SENSOR_USLEEP_(us) CamOsUsSleep(us)
#define SENSOR_MSLEEP_(ms) CamOsMsSleep(ms)

/*Extension version*/
#define SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(Name) SENSOR_DRV_ENTRY_IMPL_BEGIN(Name)

#define MAX_CAMID_LEN 4
#define SENSOR_DRV_ENTRY_IMPL_END_EX(Name,LinearEntry,HdrSefEntry,HdrLefEntry,PrivateDataType) \
static PrivateDataType* g_pData[2][MAX_CAMID_LEN] = {{0,0,0,0}, {0,0,0,0}};\
static int __init Name##_init_driver(void)\
{\
    int nCamID=0;\
    void* p0 = LinearEntry;\
    void* p1 = HdrSefEntry;\
    void* p2 = HdrLefEntry;\
    if(DrvSensorHandleVer(CUS_CAMSENSOR_HANDLE_MAJ_VER, CUS_CAMSENSOR_HANDLE_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorIFVer(CUS_CAMSENSORIF_MAJ_VER, CUS_CAMSENSORIF_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorI2CVer(CUS_CAMSENSOR_I2C_MAJ_VER, CUS_CAMSENSOR_I2C_MIN_VER)==FAIL)\
    return FAIL;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){\
                void* pData = CamOsMemAlloc(sizeof(PrivateDataType));\
                memset(pData,0,sizeof(PrivateDataType));\
                DrvRegisterSensorDriverEx(nCamID, LinearEntry,pData);\
                DrvRegisterSensorI2CSlaveID(nCamID, (i2c_slave_id>>(nCamID*8))&0xFF);\
                pr_info("Connect %s linear to sensor pad %d\n",__FUNCTION__, nCamID);\
                g_pData[0][nCamID] = pData;\
            }\
            if(p1||p2){\
                void* pData = CamOsMemAlloc(sizeof(PrivateDataType));\
                memset(pData,0,sizeof(PrivateDataType));\
                if(p1){\
                    DrvRegisterPlaneDriverEx(nCamID, 1, HdrSefEntry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 1, (i2c_slave_id>>(nCamID*8))&0xFF);\
                    pr_info("Connect %s SEF to vif sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                if(p2){\
                    DrvRegisterPlaneDriverEx(nCamID, 0, HdrLefEntry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 0, (i2c_slave_id>>(nCamID*8))&0xFF);\
                    pr_info("Connect %s LEF to sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                g_pData[1][nCamID] = pData;\
            }\
        }\
    }\
    return 0;\
}\
static void __exit Name##_exit_driver(void)\
{\
    int nCamID=0;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(g_pData[0][nCamID] || g_pData[1][nCamID])\
            {\
                DrvUnregisterSensorDriverEx(nCamID);\
                if(g_pData[0][nCamID]){\
                    CamOsMemRelease((void*)g_pData[0][nCamID]);\
                    g_pData[0][nCamID] = 0;\
                }\
                if(g_pData[1][nCamID]){\
                    CamOsMemRelease((void*)g_pData[1][nCamID]);\
                    g_pData[1][nCamID] = 0;\
                }\
            }\
        }\
    }\
}\
subsys_initcall(Name##_init_driver);\
module_exit(Name##_exit_driver);\
MODULE_DESCRIPTION("Sensor_"#Name);\
MODULE_AUTHOR("SigmaStar");\
MODULE_LICENSE("Proprietary");
                
//define 3 ch AHD application impl
#define SENSOR_DRV_ENTRY_3CHAHD_IMPL_END_EX(Name,LinearEntry,AhdCh0Entry,AhdCh1Entry, AhdCh2Entry,PrivateDataType)\
static PrivateDataType* g_pData[2][MAX_CAMID_LEN] = {{0,0,0,0}, {0,0,0,0}};\
static int __init Name##_init_driver(void)\
{\
    int nCamID=0;\
    void* p0 = LinearEntry;\
    void* p1 = AhdCh0Entry;\
    void* p2 = AhdCh1Entry;\
    void* p3 = AhdCh2Entry;\
    if(DrvSensorHandleVer(CUS_CAMSENSOR_HANDLE_MAJ_VER, CUS_CAMSENSOR_HANDLE_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorIFVer(CUS_CAMSENSORIF_MAJ_VER, CUS_CAMSENSORIF_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorI2CVer(CUS_CAMSENSOR_I2C_MAJ_VER, CUS_CAMSENSOR_I2C_MIN_VER)==FAIL)\
    return FAIL;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){\
                void* pData = CamOsMemAlloc(sizeof(PrivateDataType));\
                memset(pData,0,sizeof(PrivateDataType));\
                DrvRegisterSensorDriverEx(nCamID, LinearEntry,pData);\
                DrvRegisterSensorI2CSlaveID(nCamID, (i2c_slave_id>>(nCamID*8))&0xFF);\
                pr_info("Connect %s linear to sensor pad %d\n",__FUNCTION__, nCamID);\
                g_pData[0][nCamID] = pData;\
            }\
            if(p1||p2||p3){\
                void* pData = CamOsMemAlloc(sizeof(PrivateDataType));\
                memset(pData,0,sizeof(PrivateDataType));\
                if(p1){\
                    DrvRegisterPlaneDriverEx(nCamID, 0, AhdCh0Entry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 0, (i2c_slave_id>>(nCamID*8))&0xFF);\
                    pr_info("Connect %s AHD0 to vif sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                if(p2){\
                    DrvRegisterPlaneDriverEx(nCamID, 1, AhdCh1Entry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 1, (i2c_slave_id>>(nCamID*8))&0xFF);\
                    pr_info("Connect %s AHD1 to sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                if(p3){\
                    DrvRegisterPlaneDriverEx(nCamID, 2, AhdCh2Entry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 2, (i2c_slave_id>>(nCamID*8))&0xFF);\
                    pr_info("Connect %s AHD2 to sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                g_pData[1][nCamID] = pData;\
            }\
        }\
    }\
    return 0;\
}\
static void __exit Name##_exit_driver(void)\
{\
    int nCamID=0;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(g_pData[0][nCamID] || g_pData[1][nCamID])\
            {\
                DrvUnregisterSensorDriverEx(nCamID);\
                if(g_pData[0][nCamID]){\
                    CamOsMemRelease((void*)g_pData[0][nCamID]);\
                    g_pData[0][nCamID] = 0;\
                }\
                if(g_pData[1][nCamID]){\
                    CamOsMemRelease((void*)g_pData[1][nCamID]);\
                    g_pData[1][nCamID] = 0;\
                }\
            }\
        }\
    }\
}\
subsys_initcall(Name##_init_driver);\
module_exit(Name##_exit_driver);\
MODULE_DESCRIPTION("Sensor_"#Name);\
MODULE_AUTHOR("SigmaStar");\
MODULE_LICENSE("Proprietary");

#endif //end of CAM_OS_LINUX_KERNEL


#endif //endif _SENSOR_OS_WRAPPER_H_
