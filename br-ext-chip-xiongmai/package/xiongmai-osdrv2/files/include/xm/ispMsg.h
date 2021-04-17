/*
**********************************************************************************
**Camera.c
**XM510/XM540
**
**(c) Copyright  HZXM  
**All Rights Reserved
**
**File		: ispMsg.h
**Description	: API to application
**Modify		: 2015-01	Lycai		Create the file
**********************************************************************************
*/


#ifndef _ISP_MSG_H_
#define _ISP_MSG_H_

#ifdef SOC_SYSTEM
#include <sys/ipc.h>
#include <sys/shm.h>				
#include <sys/msg.h>	
#include <sys/types.h>	
#endif 

#include "xm_type.h"

typedef enum {
	C_EXP_LEVEL 	= 0,
	C_DN_MODE 		= 1,
	C_BLC_MODE 	= 2,
	C_SCENE_MODE 	= 3,
	C_AE_TGT 		= 4,
	C_DN_THR 		= 5,
	C_DWDR			= 6,
	C_AE_SENS		= 7,
	C_DN_SENS		= 8,
	C_IMG_STYLE	= 9,
	C_AUTO_GAIN	= 10,
	C_EE_SHUTTER	= 11,
	C_IRMODE		= 12,
	C_NR_DAY		= 13,
	C_NR_NIGHT		= 14,
	C_DEBUG		= 15,
	C_SAVE			= 16,
	C_MIRROR		= 17,
	C_FLIP			= 18,
	C_DEFLIKER		= 19,
	C_IRSWAP		= 20,
	C_SETCOLOR		= 21,
	C_VSTD			= 22,
	C_IR_STATUS	= 23,
	E_FORMAT		= 24,
	C_LEDMODE		= 25,
	C_LEDTYPE		= 26,
	C_SNAP			= 27,
	C_AE_WEIGHT		= 28,	// 焦点（人形）防过曝使能
	C_GPIO_SET		= 29,
	C_AUTO_IRIS		= 30,
	C_SOFTIR_LVL	= 31,
	C_DEFOG			= 32,
	C_INFRARED_SWAP	= 33,
	VDA_MDOD		= 48,
	VDAM_MDOD		= 49,
	BLIND_DETECT	= 50,

	E_LIGHT_CTRL		= 51,
	E_COORD			= 52,
	IAPED_INFO		= 60,
	IAPG_INFO		= 61,
	E_PRODUCT		= 255,
} CAMERA_SETTINT;

typedef enum {
	VDA_CREAT = 1,
	VDA_DESTORY ,
	VDA_GETSTATE,
	VDA_SETPARAMETER,
	VDA_GETCAPS,
}CAMERA_VDACMD_MODE;

typedef enum {
	MSG_WRITE 	= 1,
	MSG_READ 	= 2,
	MSG_ACK		= 3,
} CAMERA_SETTINT_MODE;



typedef struct _xm_msg_data_
{
	XM_S32 s32Cmd;
	XM_S32 s32Mode;
	XM_S32 s32DataNum;
	XM_S32 s32Data[32];
}XM_MSG_DATA;


typedef struct _xm_msg_	
{
	char as8Head[4];
	XM_MSG_DATA stData;
	char term_path[32];	//当前运行2asndmsg 命令的终端名	
}XM_MSG;	



//白天黑夜降噪等级
typedef struct _nf_lenvl
{
	XM_S32 day_level;
	XM_S32 night_level;
}sNF_LEVEL;



typedef struct exposure_paramer
{
	XM_U32 u32TimeMode;
	XM_U32 u32TimeMax;
	XM_U32 u32TimeMin;

	XM_U32 u32GainMode;
	XM_U32 u32GainLevel;
	XM_U8 u8ExposureMode;	// 0:自动曝光   1: 手动曝光
}sEXPOSUREPARAMER;

typedef struct wdr_paramer
{
	XM_U8 u8Enable;
	XM_U8 u8Level;
}sWDRPARAMER;

typedef struct camera_paramer
{
	XM_U8 u8StdType;
	XM_U8 u8SceneMode;
	XM_U8 u8CameraDnc;
	XM_U8 u8LumTarget;
	sEXPOSUREPARAMER sCameraExposurePrm;
	XM_U8 u8Mirror;
	XM_U8 u8Flip;
	XM_U8 u8FlickerMode;
	XM_U8 u8EShutterLevel;
	XM_U8 u8DNCThreshold;
	XM_U8 u8DNCMode;
	XM_U8 u8DNCDelayLevel;
	sNF_LEVEL sNFLevel;
	XM_U8 u8IRCutDirectioin;
	XM_U8 u8IRCutSyncMode;
	sWDRPARAMER sWDRParamer;
	XM_U8 u8StyleMode;
	XM_U8 u8BlcMode;
	XM_U8 u8AeSensitivity;
	XM_U8 u8AtFalseColor;	
	XM_U8 u8sawtooth;
	XM_U8 u8BwBurst;
	XM_U8 u8LedMode;
	XM_U8 u8LedType;
	XM_U8 u8HlcMode;
	XM_U8 u8AeWeight;	// 焦点（人形）防过曝使能
	XM_U8 u8SoftIrLvl;	// 软光敏阈值
	sWDRPARAMER stDefog;
	XM_U8 u8InfraredSwap;	//灯板输入信号反向 (0:默认  1: 反向)
}sCAMERA_PARAMER;


typedef struct _video_paramer
{
	XM_U32 changed_flag;	//改变标记
	XM_U32 lum;			//亮度
	XM_U32 contrast;		//对比度
	XM_U32 saturation;	//饱和度
	XM_U32 hue;			//饱和度
	XM_U32 autance;		//锐度

	XM_S32 autogain_mode;		//自动增益模式
	XM_S32 autogain_level;		//自动增益等级
	
}VIDEO_PARAMER;



#define FILE_AEWMSG		"/tmp/ispmsg"
#define FILE_AEWMSG2	"/tmp/ispmsg2"



#endif

