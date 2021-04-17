/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_type.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2017/04/07

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2017/04/07
  Author		: Lycai
  Modification	: creat
******************************************************************************/
#ifndef __MPI_XMISP_H__
#define __MPI_XMISP_H__
typedef struct xmIR_DNC_ATTR_s
{
	int s32ColorExp;		// <s32ColorExp: to Color Mode
	int s32ColorDlt;
	int s32ColorDlt2;
	int s32BwExp;			// >s32BwExp: to BW Mode
	
}IR_DNC_ATTR_S;
typedef struct xmIR_DNC_INFO_s
{
#if(defined SOC_NONE) || (defined CHIPID_XM530)
	int s32Data[16];
#else
	int s32Data[6];
#endif
}IR_DNC_INFO_S;

typedef enum {
    tFALSE = 0,
    tTRUE  = 1,
} tBOOL;


typedef struct xmIR_FUNC_S
{
/**************************************
函数功能: 控制灯板光源
输入参数:   u8Mode
			0: 关闭
			1: 打开
返回参数:	-1: 	出错
		0:		操作完成
		1:		操作无效
**************************************/
    int(*pfn_led_ctrl)(unsigned char u8Mode);

/**************************************
函数功能: 获取灯板状态
输入参数:  无
输出参数:	*ps32Mode: 0:Auto 1:Manual
		* ps32State: 0:Close 1:Open
		
返回参数:	-1: 	出错
		0:		成功
**************************************/
	int(*pfn_led_get)(int *ps32Mode, int* ps32State);

/**************************************
函数功能: 日夜状态获取
输入参数: *ps32State:  0: Day 1:Night
输出参数: 无
返回参数:	-1: 	出错
		0:		成功
**************************************/
	int(*pfn_dnStatusExt_get)(int* ps32State);

} IR_FUNC_S;

typedef struct xmIR_CTRL_ATTR_S
{
	unsigned char u8Delay2Color;		// (s)
	unsigned char u8Delay2Bw;			// (s)
	unsigned char u8DelayLedReOpen;	// (s)
	unsigned char u8Mode;				// 0: No PhotoR  1: Have PhotoR  2: No PhotoR_V2  3: Have PhotoR_V2
	unsigned short u16PeriodMs;		// 调用周期(ms)
} IR_CTRL_ATTR_S;


/*********************************************************************************
函数功能:	获取日夜状态
输入参数:	u8DnNow: 当前日夜模式(实际生效)
					0: 彩色
					1: 黑白
				pstIrDncAttr
					s32ColorExp:  <=该值进入彩色模式
					s32ColorDlt:  	>=该值进入彩色模式
					s32ColorDlt2:	>=该值进入彩色模式(Example: 50)
					s32BwExp:	>=该值进入黑白模式
输出参数:	无
返回参数:	-1:	出错
				0:	彩色
				1:	黑白
*********************************************************************************/
int XM_MPI_IR_GetDNCStatus(unsigned char u8DnNow, IR_DNC_ATTR_S* pstIrDncAttr);


/*********************************************************************************
函数功能:	获取统计信息
输入参数:	无
输出参数:	pstDncInfo: 统计信息
返回参数:	-1:	出错
				0:	彩色
				1:	黑白
*********************************************************************************/
int XM_MPI_IR_GetInfo(IR_DNC_INFO_S *pstDncInfo);

/*********************************************************************************
函数功能:	注册灯板控制接口
输入参数:	pstRegister: 指向接口地址
输出参数:	无
返回参数:	-1:	出错
		0:     成功
*********************************************************************************/
int XM_MPI_IR_FunCallBack(IR_FUNC_S *pstRegister);


/*********************************************************************************
函数功能:	配置控制参数
输入参数:	pstCtrlAttr: 指向参数属性
输出参数:	无
返回参数:	-1:	出错
		0:     成功
*********************************************************************************/
int XM_MPI_IR_SetCtrlAttr(IR_CTRL_ATTR_S *pstCtrlAttr);


/*********************************************************************************
函数功能:	获取控制参数
输入参数:	无
输出参数:	pstCtrlAttr: 指向参数属性
返回参数:	-1:	出错
		0:     成功
*********************************************************************************/
int XM_MPI_IR_GetCtrlAttr(IR_CTRL_ATTR_S *pstCtrlAttr);


/*********************************************************************************
函数功能:	切换模式
输入参数:	u8Mode: 
					0: IR
					1: WL
输出参数:	无
返回参数:	-1:	出错
				0:	成功
*********************************************************************************/
XM_S32 XM_MPI_IR_SwitchMode(XM_U8 u8Mode);


/*************************************************************************
函数功能: OSD 调试打印使能
输入参数: bEnable:
				0: Close
				1: Open
输出参数: 无
返回参数: 0: 成功
			  其他: 出错
*************************************************************************/
int XM_MPI_DBG_En(tBOOL bEnable);

/*************************************************************************
函数功能: OSD 调试打印
输入参数: 	u8Num:  打印数据个数
				*pu32Data: 指向打印数据
					依次在第1,2,3...u8Num行
输出参数: 无
返回参数: 0: 成功
			  其他: 出错
*************************************************************************/
int XM_MPI_DBG_Task(unsigned char u8Num, unsigned int *pu32Data);

/*************************************************************************
函数功能: OSD 调试释放内存
输入参数: 无
输出参数: 无
返回参数: 无
*************************************************************************/
int XM_MPI_DBG_Release(XM_VOID);
#endif /* __MPI_XMISP_H__ */