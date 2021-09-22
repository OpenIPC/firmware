/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm540_tmp.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/6/27

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/6/27
  Author		: Lycai
  Modification	: creat
******************************************************************************/
#ifndef _XM540_TMP_H_
#define _XM540_TMP_H_
#include "xm_i2c.h"
#include "xm_ssp.h"
#include "xm_type.h"
#include "xm_ae_comm.h"
#include "xm_awb_comm.h"

#include "xm_comm_sns.h"
#include "xm_comm_3a.h"

#define VI_BASE		(0x00A00000)

#define VI_H_BEGIN	(VI_BASE+0x098)
#define VI_H_END		(VI_BASE+0x09C)
#define VI_V_BEGIN	(VI_BASE+0x0A0)
#define VI_V_END		(VI_BASE+0x0A4)

/*************************************************************************
函数功能:	配置VI裁剪参数(H、V)
输出参数:	u8Mode:
					0: Real Data(实际值)
					1: 基于标准值进行偏移
					2: Refresh
				u8Mirror:
					1: Mirror
				u8Flip:
					1: Flip
				u16ValH: 水平方向值(bit15:为符号)    [0, 0xFFFE]
					0~0x7FFF			:
					0x8000 ~ 0xFFFE	:  <0

				u16ValV: 垂直方向值(bit15:为符号)	  [0, 0xFFFE]
					0~0x7FFF			:
					0x8000 ~ 0xFFFE	:  <0	
note:  
	u16ValH/u16ValV = 0xFFFF 时标准不写入
*************************************************************************/
XM_S32 VI_WinSet(XM_U8 u8Mode, XM_U8 u8Mirror, XM_U8 u8Flip,
						XM_U16 u16ValH, XM_U16 u16ValV);

int XM_I2C_Ioctl(int cmd, I2C_DATA_S *pstI2CData);
int XM_SPI_Ioctl(int cmd, XM_U32* pu32Data);

void I2C_Write(XM_U32 addr, XM_U32 data);
void PrintHex(unsigned char u8Num, XM_U64 u64Data);
void PrintInt(unsigned char u8Num,int u32Data);
void SysDelay_ms(unsigned int nms);

XM_S32 SysGetProductInfo_Ptr(XM_PRODUCT_INFO **pstProductInfo);



/***********************************************************************
函数名称:	SysReadFromFlash
函数功能:	读取FLash配置
输入参数:	pu8Data: 缓存地址
				u32Addr: 读取地址
				u32Len:	读取长度
输出参数:	无
返回参数:	1:	成功
				0: 	出错
Note:Lycai
***********************************************************************/
XM_BOOL SysReadFromFlash(XM_U8 *pu8Data, XM_U32 u32Addr, XM_U32 u32Len);
/**********************************************************************
函数功能:	配置色同步
输入参数:	u8BurstMode:  0  No CSync
							1  have CSync
				u8ColorMode:	0 BW
							1 Color
输出参数:	无
返回参数:	0: Success
				-1: Failure
**********************************************************************/
XM_S32 XM_MPI_VENC_SetColor(XM_U8 u8BurstMode, XM_U8 u8ColorMode);

#endif

