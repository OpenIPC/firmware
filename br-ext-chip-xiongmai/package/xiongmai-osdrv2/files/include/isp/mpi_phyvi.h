/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: mpi_mipi.h
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
#ifndef __MPI_MIPI_H__
#define __MPI_MIPI_H__
#include "xm_defines.h"
#include "xm_common.h"


typedef struct _mipi_dev_attr_s
{
	XM_MIPI_LANE       	lane;                 /* MIPI lane num */
    XM_SENSOR_BWIDE     depth;          /* Depth: 8/10/12/ bit */
    
	XM_U32   snsMpInclk;	//ËÍ¸øsensorµÄmclkÊ±ÖÓ
	XM_U32   snsMpOutclk;	//sensorÊä³öµÄmipiÊ±ÖÓ
	XM_U32   snsAllLine;	//sensorÊä³ö×ÜÐÐÊý
	XM_U32   snsAllPixs;	//sensorÊä³ö×ÜµãÊý

	XM_U32   snsActiveLine;	//sensorÊä³öÓÐÐ§ÐÐÊý
	XM_U32   snsActivePixs;	//sensorÊä³öÓÐÐ§µãÊý

	XM_U32   bMpDvpclk;	//Ð¾Æ¬ÄÚ²¿²¢ÐÐÈ¡µãÊ±ÖÓ
	XM_U32   bAllPixs;	//Ð¾Æ¬ÄÚ²¿²¢ÐÐÈ¡µã×ÜµãÊý
	XM_U32   delay;		//mipiÄÚ²¿delay
	XM_U32   MipiCtrl;	//[23:16]:ctrl_dly(¼ì²â sotÊ±¼ä delay:ctrl_dly*2);[7:0]:sot_data(B8);[8]:clk_ctrl(0:CKp;1:CKn) [15:9]:±£Áô
}MIPI_DEV_ATTR_S;


typedef struct _combo_dev_attr_s
{
	XM_SENSOR_CONT input_mode;
	MIPI_DEV_ATTR_S mipi_attr;
}COMBO_DEV_ATTR_S;

typedef enum xm_mipi_cmd
{
	MIPI_SET_DEV_ATTR = 0,
	MIPI_SET_OUTPUT_CLK_EDGE = 1,
	MIPI_RESET_MIPI = 2,
	MIPI_UNRESET_MIPI = 3,
	MIPI_SET_RUN = 4,
//	MIPI_RESET_SENSOR = 5,
//	MIPI_UNRESET_SENSOR = 6,
	MIPI_BUTT
}MIPI_CMD;

typedef struct _xm_mipi_ck_s
{
	unsigned int mipi_ck_div;
	unsigned int mipi_ck_count;
	unsigned int mipi_ck_all_v;
	unsigned int mipi_ck_all_h;
	unsigned int mipi_ck_en;
}MIPI_CK_CMD;



XM_S32 XM_MPI_MIPI_GetDevAttr(ISP_DEV IspDev, COMBO_DEV_ATTR_S *pstComboDevAttr);
XM_S32 XM_MPI_MIPI_SetDevAttr(ISP_DEV IspDev, MIPI_CMD enCmd, const COMBO_DEV_ATTR_S *pstComboDevAttr);

/*****************************************************************************************************
å‡½æ•°åç§°: XM_MPI_MIPI_RefreshFV
å‡½æ•°åŠŸèƒ½: MIPIå¼ºåˆ¶åˆ·æ–°è¡Œåœºä¿¡å·
è¾“å…¥å‚æ•°: 		u32DelayMs:åˆ·æ–°æ—¶é—´(ms)
				>0: é…ç½®çš„æ—¶é—´
				0:  å†…éƒ¨è‡ªåŠ¨å†³å®š
			u32TotalSizeV
				>0: æ€»è¡Œæ•°
				=0: ä»…åˆ·æ–°ä¿¡å·
è¾“å‡ºå‚æ•°:	æ— 
è¿”å›žå‚æ•°:		0: æˆåŠŸ
			-1: å¤±è´¥
*****************************************************************************************************/
XM_S32 XM_MPI_MIPI_RefreshFV(XM_U32 u32DelayMs, XM_U32 u32TotalSizeV);
#endif /*__MPI_ISP_H__ */

	
