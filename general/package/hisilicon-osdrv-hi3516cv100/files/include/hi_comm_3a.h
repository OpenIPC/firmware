/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_3a.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/18
  Description   : 
  History       :
  1.Date        : 2012/12/18
    Author      : n00168968
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_3A_H__
#define __HI_COMM_3A_H__

#include "hi_common.h"
#include "hi_comm_isp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define MAX_REGISTER_ALG_LIB_NUM 2

typedef enum hiISP_ALG_MOD_E
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

typedef enum hiISP_CTRL_CMD_E
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

typedef struct hiISP_CTRL_PROC_WRITE_S
{
    HI_CHAR *pcProcBuff;
    HI_U32   u32BuffLen;
    HI_U32   u32WriteLen;   /* The len count should contain '\0'. */
} ISP_CTRL_PROC_WRITE_S;

/********************************  AE  *************************************/
/* the init param of ae alg */
typedef struct hiISP_AE_PARAM_S
{
    SENSOR_ID SensorId;
    
    HI_U32 u32MaxIspDgain;
    HI_U32 u32MinIspDgain;
    HI_U32 u32IspDgainShift;
} ISP_AE_PARAM_S;

/* the statistics of ae alg */
typedef struct hiISP_AE_STAT_1_S
{
    HI_U8   au8MeteringHistThresh[4];
    HI_U16  au16MeteringHist[4];
} ISP_AE_STAT_1_S;

typedef struct hiISP_AE_STAT_2_S
{
    HI_U8   au8MeteringHistThresh[4];
    HI_U16  au16MeteringMemArrary[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN][5];
} ISP_AE_STAT_2_S;

typedef struct hiISP_AE_STAT_3_S
{    
    HI_U16  au16HistogramMemArray[256];
} ISP_AE_STAT_3_S;

typedef struct hiISP_AE_INFO_S
{
    HI_U32  u32FrameCnt;    /* the counting of frame */

    ISP_AE_STAT_1_S *pstAeStat1;
    ISP_AE_STAT_2_S *pstAeStat2;
    ISP_AE_STAT_3_S *pstAeStat3;
} ISP_AE_INFO_S;

typedef struct hiISP_AE_STAT_ATTR_S
{
    HI_BOOL bChange;

    HI_U8   au8MeteringHistThresh[4];
    HI_U8   au8WeightTable[15][17];
} ISP_AE_STAT_ATTR_S;

/* the final calculate of ae alg */
typedef struct hiISP_AE_RESULT_S
{
    HI_U32  u32IspDgain;
    HI_U32  u32IspDgainShift;
    HI_U32  u32Iso;
    
    ISP_AE_STAT_ATTR_S stStatAttr;
} ISP_AE_RESULT_S;

typedef struct hiISP_AE_EXP_FUNC_S
{
    HI_S32 (*pfn_ae_init)(HI_S32 s32Handle, const ISP_AE_PARAM_S *pstAeParam);
    HI_S32 (*pfn_ae_run)(HI_S32 s32Handle,
        const ISP_AE_INFO_S *pstAeInfo,
        ISP_AE_RESULT_S *pstAeResult,
        HI_S32 s32Rsv
        );
    HI_S32 (*pfn_ae_ctrl)(HI_S32 s32Handle, HI_U32 u32Cmd, HI_VOID *pValue);
    HI_S32 (*pfn_ae_exit)(HI_S32 s32Handle);
} ISP_AE_EXP_FUNC_S;

typedef struct hiISP_AE_REGISTER_S
{
    ISP_AE_EXP_FUNC_S stAeExpFunc;
} ISP_AE_REGISTER_S;

/********************************  AWB  *************************************/

/* the init param of awb alg */
typedef struct hiISP_AWB_PARAM_S
{
    SENSOR_ID SensorId;
    HI_S32 s32Rsv;
} ISP_AWB_PARAM_S;

/* the statistics of awb alg */
typedef struct hiISP_AWB_STAT_1_S
{
    HI_U16  u16MeteringAwbRg;
    HI_U16  u16MeteringAwbBg;
    HI_U32  u32MeteringAwbSum;
} ISP_AWB_STAT_1_S;

typedef struct hiISP_AWB_STAT_2_S
{
    HI_U16  au16MeteringMemArrayRg[255];
    HI_U16  au16MeteringMemArrayBg[255];
    HI_U16  au16MeteringMemArraySum[255];
} ISP_AWB_STAT_2_S;

typedef struct hiISP_AWB_INFO_S
{
    HI_U32  u32FrameCnt;

    ISP_AWB_STAT_1_S *pstAwbStat1;
    ISP_AWB_STAT_2_S *pstAwbStat2;
} ISP_AWB_INFO_S;

/* the statistics's attr of awb alg */
typedef struct hiISP_AWB_STAT_ATTR_S
{
    HI_BOOL bChange;

    HI_U16  u16MeteringWhiteLevelAwb;
    HI_U16  u16MeteringBlackLevelAwb;
    HI_U16  u16MeteringCrRefMaxAwb;
    HI_U16  u16MeteringCbRefMaxAwb;
    HI_U16  u16MeteringCrRefMinAwb;
    HI_U16  u16MeteringCbRefMinAwb;
} ISP_AWB_STAT_ATTR_S;

/* the final calculate of awb alg */
typedef struct hiISP_AWB_RESULT_S
{
    HI_U32  au32WhiteBalanceGain[4];
    HI_U16  au16ColorMatrix[9];
    
    ISP_AWB_STAT_ATTR_S stStatAttr;
} ISP_AWB_RESULT_S;

typedef struct hiISP_AWB_EXP_FUNC_S
{
    HI_S32 (*pfn_awb_init)(HI_S32 s32Handle, const ISP_AWB_PARAM_S *pstAwbParam);
    HI_S32 (*pfn_awb_run)(HI_S32 s32Handle,
        const ISP_AWB_INFO_S *pstAwbInfo,
        ISP_AWB_RESULT_S *pstAwbResult,
        HI_S32 s32Rsv
        );
    HI_S32 (*pfn_awb_ctrl)(HI_S32 s32Handle, HI_U32 u32Cmd, HI_VOID *pValue);
    HI_S32 (*pfn_awb_exit)(HI_S32 s32Handle);
} ISP_AWB_EXP_FUNC_S;

typedef struct hiISP_AWB_REGISTER_S
{    
    ISP_AWB_EXP_FUNC_S stAwbExpFunc;
} ISP_AWB_REGISTER_S;

/********************************  AF  *************************************/

/* the init param of af alg */
typedef struct hiISP_AF_PARAM_S
{
    SENSOR_ID SensorId;
    HI_S32 s32Rsv;
} ISP_AF_PARAM_S;

/* the statistics of af alg */
typedef struct hiISP_AF_STAT_S
{
    HI_U16 u16FocusMetrics;     /* The integrated and normalized measure of contrast*/
    HI_U16 u16ThresholdRead;    /* The ISP recommend value of AF threshold*/
    HI_U16 u16ThresholdWrite;   /* The user defined value of AF threshold (or 0 to use threshold from previous frame)*/
    HI_U16 u16FocusIntensity;   /* The average brightness*/
    HI_U8  u8MetricsShift;      /* Metrics scaling factor:the bigger value for this register means all zones metrics go higher,0x03 is the default, Range: [0x0, 0xF] */
    HI_U8  u8NpOffset;          /* The AF noise profile offset, Range: [0x0, 0xFF] */
    HI_U16 au16ZoneMetrics[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN]; /* The zoned measure of contrast*/ 
} ISP_AF_STAT_S;

typedef struct hiISP_AF_INFO_S
{
    HI_U32  u32FrameCnt;
    
    ISP_AF_STAT_S   *pstStatistics;
} ISP_AF_INFO_S;

typedef struct hiISP_AF_STAT_ATTR_S
{
    HI_BOOL bChange;

    HI_U16 u16ThresholdWrite;
    HI_U8  u8MetricsShift;
    HI_U8  u8NpOffset;
} ISP_AF_STAT_ATTR_S;

/* the final calculate of af alg */
typedef struct hiISP_AF_RESULT_S
{
    ISP_AF_STAT_ATTR_S stStatAttr;
    HI_S32 s32Rsv;
} ISP_AF_RESULT_S;

typedef struct hiISP_AF_EXP_FUNC_S
{
    HI_S32 (*pfn_af_init)(HI_S32 s32Handle, const ISP_AF_PARAM_S *pstAfParam);
    HI_S32 (*pfn_af_run)(HI_S32 s32Handle,
        const ISP_AF_INFO_S *pstAfInfo,
        ISP_AF_RESULT_S *pstAfResult,
        HI_S32 s32Rsv
        );
    HI_S32 (*pfn_af_ctrl)(HI_S32 s32Handle, HI_U32 u32Cmd, HI_VOID *pValue);
    HI_S32 (*pfn_af_exit)(HI_S32 s32Handle);
} ISP_AF_EXP_FUNC_S;

typedef struct hiISP_AF_REGISTER_S
{    
    ISP_AF_EXP_FUNC_S stAfExpFunc;
} ISP_AF_REGISTER_S;

typedef struct hiALG_LIB_S
{
    HI_S32  s32Id;
    HI_CHAR acLibName[20];
} ALG_LIB_S;

typedef struct hiISP_BIND_ATTR_S
{
    SENSOR_ID   SensorId;
    ALG_LIB_S   stAeLib;
    ALG_LIB_S   stAfLib;
    ALG_LIB_S   stAwbLib;
} ISP_BIND_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_COMM_SNS_H__ */

