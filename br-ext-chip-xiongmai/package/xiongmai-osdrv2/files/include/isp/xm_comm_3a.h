/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_common_3a.h
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
#ifndef __XM_COMM_3A_H__
#define __XM_COMM_3A_H__

#include "xm_common.h"
#include "xm_comm_isp.h"



#define MAX_REGISTER_ALG_LIB_NUM 2

typedef enum xm_ISP_ALG_MOD_E
{
    ISP_ALG_AE = 0,
    ISP_ALG_AF,
    ISP_ALG_AWB,
    ISP_ALG_ANTIFOG,
    ISP_ALG_BLC,
    ISP_ALG_DP,
    ISP_ALG_DRC,
    ISP_ALG_DEMOSAIC,
    ISP_ALG_GAMMA,
    ISP_ALG_GAMMAFE,
    ISP_ALG_GE,
    ISP_ALG_NEW_ANTIFOG,
    ISP_ALG_NR,
    ISP_ALG_SHARPEN,
    ISP_ALG_SHADING,
    
    ISP_ALG_BUTT,
} ISP_ALG_MOD_E;

typedef enum xm_ISP_CTRL_CMD_E
{
    ISP_WDR_MODE_SET = 8000,
    ISP_PROC_WRITE,
    
    ISP_AE_FPS_BASE_SET,
    ISP_AWB_ISO_SET,  /* set iso, change saturation when iso change */
    ISP_AE_INTTIME_GET,
    ISP_AWB_INTTIME_SET,

    ISP_CHANGE_IMAGE_MODE_SET,
    ISP_CTRL_CMD_BUTT,
} ISP_CTRL_CMD_E;

typedef struct xm_ISP_CTRL_PROC_WRITE_S
{
    XM_CHAR *pcProcBuff;
    XM_U32   u32BuffLen;
    XM_U32   u32WriteLen;   /* The len count should contain '\0'. */
} ISP_CTRL_PROC_WRITE_S;

/********************************  AE  *************************************/
/* the init param of ae alg */
typedef struct xm_ISP_AE_PARAM_S
{
    SENSOR_ID SensorId;
    
    XM_U32 u32MaxIspDgain;
    XM_U32 u32MinIspDgain;
    XM_U32 u32IspDgainShift;
    XM_U8   u8FrameRate;
} ISP_AE_PARAM_S;

/* the statistics of ae alg */
typedef struct xm_ISP_AE_STAT_1_S
{
    XM_U32 au32MeteringHist[16];
} ISP_AE_STAT_1_S;

typedef struct xm_ISP_AE_STAT_2_S
{
	XM_U32 au32MeteringWin[AE_ZONE_ROW][AE_ZONE_COLUMN];	// Y_avg << 8
} ISP_AE_STAT_2_S;

typedef struct xm_ISP_AE_STAT_3_S
{    
    XM_U16  au16MeteringWinV2[ISP_V2_ZONE_ROW][ISP_V2_ZONE_COLUMN];
} ISP_AE_STAT_3_S;


typedef struct xm_ISP_AE_STAT_S
{
	XM_U8 u8FrameRate;
	XM_U32 u32FrameCnt;
	XM_U32 au32MeteringWin[AE_ZONE_ROW][AE_ZONE_COLUMN];	// Y_avg << 8
} ISP_AE_STAT_S;



typedef struct xm_ISP_AE_INFO_S
{
    XM_U32  u32FrameCnt;    /* the counting of frame */
    XM_U8  u8FrameRate;	
    ISP_AE_STAT_1_S stAeStat1;
    ISP_AE_STAT_2_S stAeStat2;
    ISP_AE_STAT_3_S stAeStat3;
} ISP_AE_INFO_S;



/* the final calculate of ae alg */
typedef struct xm_ISP_AE_RESULT_S
{
    XM_U32  u32IspDgain;
    XM_U32  u32IspDgainShift;
    XM_U32  u32Iso;
    XM_U32  u32Rsv;
/********************************
u32UpdateCfg:
	bit[0~7]  period
	bit[8~15] shut 
	bit[16~23] snsGain
	bit[24~31] ispGain
********************************/
    XM_U32  u32UpdateCfg;

} ISP_AE_RESULT_S;

typedef struct xm_ISP_AE_EXP_FUNC_S
{
    XM_S32 (*pfn_ae_init)(ISP_DEV IspDev, XM_S32 s32Handle, const ISP_AE_PARAM_S *pstAeParam);
    XM_S32 (*pfn_ae_run)(ISP_DEV IspDev, XM_S32 s32Handle,
        const ISP_AE_INFO_S *pstAeInfo,
        ISP_AE_RESULT_S *pstAeResult,
        XM_S32 s32Rsv
        );
    XM_S32 (*pfn_ae_ctrl)(ISP_DEV IspDev, XM_S32 s32Handle, XM_U32 u32Cmd, XM_VOID *pValue);
    XM_S32 (*pfn_ae_exit)(ISP_DEV IspDev, XM_S32 s32Handle);
} ISP_AE_EXP_FUNC_S;

typedef struct xm_ISP_AE_REGISTER_S
{
    ISP_AE_EXP_FUNC_S stAeExpFunc;
} ISP_AE_REGISTER_S;


/********************************  AWB  *************************************/
/* the statistics of awb alg */
typedef struct xm_ISP_AWB_STAT_ZONE_S
{
	XM_U32  u32Data1;	// Wp
	XM_U32  u32Data2;	// Cb
	XM_U32  u32Data3;	// Cr
} ISP_AWB_STAT_ZONE_S;



typedef struct xm_ISP_AWB_STAT_1_S
{
	ISP_AWB_STAT_ZONE_S stStat1[4][4];	// 4:Chn  // 4:Zone
} ISP_AWB_STAT_1_S;

typedef struct xm_ISP_AWB_STAT_2_S
{
    ;
} ISP_AWB_STAT_2_S;

typedef struct xm_ISP_AWB_INFO_S
{
	XM_U32 u32FrameCnt;
	XM_U32 u32Rsv;
	ISP_AWB_STAT_1_S *pstAwbStat1;
	ISP_AWB_STAT_2_S *pstAwbStat2;
} ISP_AWB_INFO_S;

/* the statistics's attr of awb alg */
typedef struct xm_ISP_AWB_FT_L_ATTR_S
{
	XM_BOOL bChange;

	XM_U32  u32Lh;
	XM_U32  u32Lv;
	XM_U32  u32L45;
	XM_U32  u32L135;
} ISP_AWB_FT_L_ATTR_S;

typedef struct xm_ISP_AWB_FT_K_ATTR_S
{
	XM_BOOL bChange;

	XM_U32  u32KCbCr1;
	XM_U32  u32KCbCr2;
	XM_U32  u32KCbCr3;
	XM_U32  u32KCbCr4;
} ISP_AWB_FT_K_ATTR_S;

typedef struct xm_ISP_AWB_WDW_ATTR_S
{
	XM_BOOL bChange;
	XM_U8 u8Mode;
	XM_U32  u32VBegin;
	XM_U32  u32VEnd;
	XM_U32  u32HBegin;
	XM_U32  u32HEnd;
} ISP_AWB_WDW_ATTR_S;

typedef struct xm_ISP_AWB_BG_ATTR_S
{
	XM_BOOL bChange;
	XM_U32  u32R_Ch[4];
	XM_U32  u32B_Ch[4];
} ISP_AWB_BG_ATTR_S;

typedef struct xm_ISP_AWB_RESULT_BK_S
{
	ISP_AWB_FT_L_ATTR_S stFtL;
	ISP_AWB_FT_K_ATTR_S stFtK;
	ISP_AWB_WDW_ATTR_S stWin;
	ISP_AWB_BG_ATTR_S stBg;
} ISP_AWB_RESULT_BK_S;

typedef struct xm_ISP_AWB_RESULT_GAIN_S
{
	XM_BOOL bChange;
	XM_U32  au32WhiteBalanceGain[3];
} ISP_AWB_RESULT_GAIN_S;

typedef struct xm_ISP_AWB_RESULT_CC_S
{
	XM_BOOL bChange;
	XM_U16  au16ColorMatrix[12];
} ISP_AWB_RESULT_CC_S;

/* the final calculate of awb alg */
typedef struct xm_ISP_AWB_RESULT_S
{
	ISP_AWB_RESULT_GAIN_S *pstRlt_Gain;
	ISP_AWB_RESULT_CC_S *pstRlt_Cc;
	ISP_AWB_RESULT_BK_S *pstRlt_Bk;
	XM_U16 *pstRlt_RGB;
} ISP_AWB_RESULT_S;

typedef struct xm_ISP_AWB_EXP_FUNC_S
{
    XM_S32 (*pfn_awb_init)(XM_S32 s32Handle, ISP_AWB_RESULT_BK_S* const pstAwbParam,XM_U32 Width,XM_U32 Height);
    XM_S32 (*pfn_awb_run)(XM_S32 s32Handle,
        const ISP_AWB_INFO_S *pstAwbInfo,
        ISP_AWB_RESULT_S *pstAwbResult,
        XM_S32 s32Rsv
        );
    XM_S32 (*pfn_awb_ctrl)(XM_S32 s32Handle, XM_U32 u32Cmd, XM_VOID *pValue);
    XM_S32 (*pfn_awb_exit)(XM_S32 s32Handle);
} ISP_AWB_EXP_FUNC_S;

typedef struct xm_ISP_AWB_REGISTER_S
{    
    ISP_AWB_EXP_FUNC_S stAwbExpFunc;
} ISP_AWB_REGISTER_S;

/********************************  AF  *************************************/

/* the init param of af alg */
typedef struct xm_ISP_AF_PARAM_S
{
    SENSOR_ID SensorId;
    XM_S32 s32Rsv;
} ISP_AF_PARAM_S;

/* the statistics of af alg */
typedef struct xm_ISP_AF_STAT_S
{
    XM_U16 u16FocusMetrics;     /* The integrated and normalized measure of contrast*/
    XM_U16 u16ThresholdRead;    /* The ISP recommend value of AF threshold*/
    XM_U16 u16ThresholdWrite;   /* The user defined value of AF threshold (or 0 to use threshold from previous frame)*/
    XM_U16 u16FocusIntensity;   /* The average brightness*/
    XM_U8  u8MetricsShift;      /* Metrics scaling factor:the bigger value for this register means all zones metrics go higher,0x03 is the default, Range: [0x0, 0xF] */
    XM_U8  u8NpOffset;          /* The AF noise profile offset, Range: [0x0, 0xFF] */
    XM_U16 au16ZoneMetrics[AE_ZONE_ROW][AE_ZONE_COLUMN]; /* The zoned measure of contrast*/ 
} ISP_AF_STAT_S;

typedef struct xm_ISP_AF_INFO_S
{
    XM_U32  u32FrameCnt;
    
    ISP_AF_STAT_S   *pstStatistics;
} ISP_AF_INFO_S;

typedef struct xm_ISP_AF_STAT_ATTR_S
{
    XM_BOOL bChange;

    XM_U16 u16ThresholdWrite;
    XM_U8  u8MetricsShift;
    XM_U8  u8NpOffset;
} ISP_AF_STAT_ATTR_S;

/* the final calculate of af alg */
typedef struct xm_ISP_AF_RESULT_S
{
    ISP_AF_STAT_ATTR_S stStatAttr;
    XM_S32 s32Rsv;
} ISP_AF_RESULT_S;

typedef struct xm_ISP_AF_EXP_FUNC_S
{
    XM_S32 (*pfn_af_init)(XM_S32 s32Handle, const ISP_AF_PARAM_S *pstAfParam);
    XM_S32 (*pfn_af_run)(XM_S32 s32Handle,
        const ISP_AF_INFO_S *pstAfInfo,
        ISP_AF_RESULT_S *pstAfResult,
        XM_S32 s32Rsv
        );
    XM_S32 (*pfn_af_ctrl)(XM_S32 s32Handle, XM_U32 u32Cmd, XM_VOID *pValue);
    XM_S32 (*pfn_af_exit)(XM_S32 s32Handle);
} ISP_AF_EXP_FUNC_S;

typedef struct xm_ISP_AF_REGISTER_S
{    
    ISP_AF_EXP_FUNC_S stAfExpFunc;
} ISP_AF_REGISTER_S;

typedef struct xm_ALG_LIB_S
{
    XM_S32  s32Id;
    XM_CHAR acLibName[20];
} ALG_LIB_S;

typedef struct xm_ISP_BIND_ATTR_S
{
    SENSOR_ID   SensorId;
    ALG_LIB_S   stAeLib;
    ALG_LIB_S   stAfLib;
    ALG_LIB_S   stAwbLib;
} ISP_BIND_ATTR_S;


#endif /*__XM_COMM_SNS_H__ */

