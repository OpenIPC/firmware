#ifndef _MPI_CONFIG_H
#define _MPI_CONFIG_H

#include "xm_type.h"
#include "xm_math.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_isp.h"

#include "IspToolsFlash.h"
//#include "xm540_isp.h"

#include "xm_print.h"

typedef struct _xm_menu_cfgfile_
{
	XM_U8 u8MenuEn;			
	XM_U8 u8MenuExpMode;		
	XM_U8 u8MenuDncMode;		
	XM_U8 u8MenuBlcEn;	    
	XM_U8 u8MenuAeTgt;		  
	XM_U8 u8MenuDncThr;	
	XM_U8 u8MenuWdr;
	XM_U8 u8MenuImgStyle;
	XM_U8 u8MenuAgcEn;	
	XM_U8 u8MenuAgcMax;	
	XM_U8 u8MenuNrlvlCol;
	XM_U8 u8MenuNrlvlBw;
	XM_U8 u8MenuIrcutMode;
	XM_U8 u8MenuWbRb;      
	XM_U8 u8MenuWbMg;      
	XM_U8 u8MenuMirror;		
	XM_U8 u8MenuFlip;			
	XM_U8 u8MenuAntiflicker;
	XM_U8 u8MenuIrcutSwap;
	XM_U8 u8MenuBrightness;
	XM_U8 u8MenuContrast;	
	XM_U8 u8MenuSaturation;
	XM_U8 u8MenuHue;	
	XM_U8 u8MenuAcutance;
	XM_U8 u8MenuSawtooth;
	XM_U8 u8MenuAntiFc; 
	XM_U8 u8MenuBurstBw;
	XM_U8 u8MenuVStdType;
	XM_U8 u8MenuVEncType;
	XM_U8 u8MenuRsltType;
	XM_U8 u8MenuEShutter;	// 电子慢快门
	XM_U8 u8MenuHLC;		// 强光抑制
}XM_MENU_CFGFILE;

typedef enum {
    MODE_USE_MENU = 0,
    MODE_NO_MENU  = 1,	// DebugMode
} XM_MENU_MODE;


/**********************************************************************
函数功能:	配置模式(配置/读取)
输入参数:	enMode:
					0: 	正常使用菜单文件
					1:	无菜单(调试模式)
输出参数:	penMode:
					0: 	正常使用菜单文件
					1:	无菜单(调试模式)
返回参数:	-1		: 失败
				其他	: 成功
Note:			Lycai
**********************************************************************/
XM_S32 XM_MPI_MENU_SetMode(XM_MENU_MODE enMode);
XM_S32 XM_MPI_MENU_GetMode(XM_MENU_MODE *penMode);


/**********************************************************************
函数功能:	保存菜单配置文件(=> NowConfigFile)
输入参数:	u32Cmd:
					0		: ALL
					其他	: 单独某项
				s32Data:
					-1:		读取当前然后写入
					>0:		直接写入指定值
输出参数:	无
返回参数:	-1		: 失败
				其他	: 成功
Note:			Lycai
**********************************************************************/
XM_S32 XM_MPI_MENU_SaveCfg(XM_U32 u32Cmd, XM_S32 s32Data);


/****************************************************************************
函数功能:	保存配置(=> default ConfigFile)
输入参数:	pu32Cmd:	命令集
				pu16Data:	各命令值
				u8Num:		命令个数
输出参数:	无
返回参数:	0: Success
				-1: Failure
				>0:	WriteNum
Note:			Lycai
****************************************************************************/
XM_S32 XM_MPI_MENU_SaveDefCfg(XM_U32 *pu32Cmd, XM_U16 *pu16Data, XM_U8 u8Num);


/**********************************************************************
函数功能:	保存菜单配置文件(=> NowConfiFile)
输入参数:	u8Mode:	0	To NowCofnig(will Save to Falsh)
						1	To DefaultConfig(will Save to Falsh)
						0x10:	To NowCofnig(Not Save)
				u32Cmd:		指定(地址)
				*pu32Data:	对应值
				
输出参数:	无
返回参数:	-1		: 失败
				其他	: 成功
Note:			Lycai
**********************************************************************/
XM_S32 XM_MPI_MENU_SetCfg(XM_U8 u8Mode,
										XM_U32 u32Cmd, XM_U32 *pu32Data);


/**********************************************************************
函数功能:	配置文件同步
输入参数:	u8Mode
					0:	default configFile	-> Config
					1:	const default data	-> Config
					2:	Auto
**********************************************************************/
XM_S32 XM_MPI_MENU_CfgSync(XM_U8 u8Mode);



/**********************************************************************
函数功能:	加载菜单配置文件
输入参数:	u32Cmd
					0:		ALL
					其他: 	单独某项
输出参数:	无
返回参数:	-1		: 失败
				其他	: 成功
Note:			Lycai
**********************************************************************/
XM_S32 XM_MPI_MENU_LoadCfg(XM_U32 u32Cmd);



/**********************************************************************
函数功能:	配置文件同步
输入参数:	u8Mode
					0:	default configFile 	-> Config
					1:    const default data 	-> Config
输出参数:	无
返回参数:	0: Success
				-1: Failure
**********************************************************************/
XM_S32 XM_MPI_MENU_SetDefault(XM_U8 u8Mode);



#endif

