#ifndef _EXT_CMOS_H_
#define _EXT_CMOS_H_
#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"

//函数功能: 从配置文件读取数据 写入Sensor 
//u32Addr:  数据起始地址
//u8Num: 寄存器个数
extern XM_U8 gu8Fps;
void sensor_input_2(XM_U32 u32Addr,XM_U16 u16Num);
void sensor_input_1(XM_U32 u32Addr,XM_U16 u16Num);

int sensor_write_ex(XM_U8 u8Mode, XM_U32 u32Addr, XM_U16 u16Num);



XM_S32 cmos_set_mirror_flip_ex(XM_U8 u8Mirror,XM_U8 u8Flip);

XM_S32 cmos_get_ae_default_ext(AE_SENSOR_DEFAULT_V2_S *pstAeSnsDft);

XM_VOID cmos_again_calc_table_ex(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo);

XM_VOID cmos_dgain_calc_table_ex(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo);

XM_VOID cmos_gains_update_ex(XM_U32 u32Again, XM_U32 u32Dgain);

XM_VOID cmos_inttime_update_ex(XM_U32 u32IntTime);

XM_VOID cmos_fps_set_ex(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

void gainLogic_ex(XM_U32 u32AllGain);

void shutLogic_ex(XM_U32 u32IntTime);

#endif

