#include "xm_type.h"
#include "xm_i2c.h"
#include "xm_print.h"
#include "xm_sns_ctrl.h"
#include "extCfg.h"

//函数功能: 从配置文件读取数据 写入Sensor 
//u32Addr:  数据起始地址
//u16Num: 寄存器个数
//u8Mode: 0:Read From E2		1:Read From RAM
extern int sensor_write_ex(XM_U8 u8Mode, XM_U32 u32Addr, XM_U16 u16Num);

XM_VOID sensor_init_ext()
{
	XM_U32 u32Addr;
	XM_U16 u16Num = 0;
#ifdef DEBUG_ON
	DEBUG("sensor_init_ext start!\n");
#endif
	if(ExtCfg_BaseAddrGet(0, ROM_FUN_SNSINIT, &u32Addr) == XM_SUCCESS)
	{
		ExtCfg_Read_V2(2,u32Addr, (XM_U8*)&u16Num);
		sensor_write_ex(0, u32Addr+2, u16Num);
	}
#ifdef DEBUG_ON
	DEBUG("sensor_init_ext end!\n");
#endif
}

