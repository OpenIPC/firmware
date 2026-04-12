/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_isp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/12/20
  Description   : 
  History       :
  1.Date        : 2010/12/20
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __MPI_ISP_H__
#define __MPI_ISP_H__

#include "hi_comm_isp.h"
#include "hi_comm_sns.h"
#include "hi_comm_3a.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Firmware Main Operation */
HI_S32 HI_MPI_ISP_Init(HI_VOID);
HI_S32 HI_MPI_ISP_Run(HI_VOID);
HI_S32 HI_MPI_ISP_Exit(HI_VOID);
HI_S32 HI_MPI_ISP_SensorRegCallBack(SENSOR_ID SensorId, ISP_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_SensorUnRegCallBack(SENSOR_ID SensorId);

/* if have registered multy libs, set bind attr to appoint the active lib. */
HI_S32 HI_MPI_ISP_SetBindAttr(const ISP_BIND_ATTR_S *pstBindAttr);
HI_S32 HI_MPI_ISP_GetBindAttr(ISP_BIND_ATTR_S *pstBindAttr);
HI_S32 HI_MPI_ISP_AeLibRegCallBack(ALG_LIB_S *pstAeLib,
        ISP_AE_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AwbLibRegCallBack(ALG_LIB_S *pstAwbLib,
        ISP_AWB_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AfLibRegCallBack(ALG_LIB_S *pstAfLib,
        ISP_AF_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AeLibUnRegCallBack(ALG_LIB_S *pstAeLib);
HI_S32 HI_MPI_ISP_AwbLibUnRegCallBack(ALG_LIB_S *pstAwbLib);
HI_S32 HI_MPI_ISP_AfLibUnRegCallBack(ALG_LIB_S *pstAfLib);

HI_S32 HI_MPI_ISP_SetInputTiming(const ISP_INPUT_TIMING_S *pstInputTiming);
HI_S32 HI_MPI_ISP_GetInputTiming(ISP_INPUT_TIMING_S *pstInputTiming);

HI_S32 HI_MPI_ISP_SetImageAttr(const ISP_IMAGE_ATTR_S *pstImageAttr);
HI_S32 HI_MPI_ISP_GetImageAttr(ISP_IMAGE_ATTR_S *pstImageAttr);

HI_S32 HI_MPI_ISP_SetWdrAttr(const ISP_WDR_ATTR_S *pstWdrAttr);
HI_S32 HI_MPI_ISP_GetWdrAttr(ISP_WDR_ATTR_S *pstWdrAttr);

HI_S32 HI_MPI_ISP_SetDebug(const ISP_DEBUG_INFO_S * pstIspDebug);
HI_S32 HI_MPI_ISP_GetDebug(ISP_DEBUG_INFO_S * pstIspDebug);
HI_S32 HI_MPI_ISP_FreezeFmw(HI_BOOL bFreeze);

HI_S32 HI_MPI_ISP_SetModuleControl(HI_U32 u32ModFlag);
HI_S32 HI_MPI_ISP_GetModuleControl(HI_U32 *pu32ModFlag);

/* General Function Settings */
HI_S32 HI_MPI_ISP_SetDRCAttr(const ISP_DRC_ATTR_S *pstDRC);
HI_S32 HI_MPI_ISP_GetDRCAttr(ISP_DRC_ATTR_S *pstDRC);

HI_S32 HI_MPI_ISP_SetDefectPixelAttr(const ISP_DP_ATTR_S *pstDPAttr);
HI_S32 HI_MPI_ISP_GetDefectPixelAttr(ISP_DP_ATTR_S *pstDPAttr);

HI_S32 HI_MPI_ISP_SetDISAttr(const ISP_DIS_ATTR_S *pstDISAttr);
HI_S32 HI_MPI_ISP_GetDISAttr(ISP_DIS_ATTR_S *pstDISAttr);
HI_S32 HI_MPI_ISP_GetDISInfo(ISP_DIS_INFO_S *pstDISInfo);

HI_S32 HI_MPI_ISP_SetShadingAttr(const ISP_SHADING_ATTR_S *pstShadingAttr);
HI_S32 HI_MPI_ISP_GetShadingAttr(ISP_SHADING_ATTR_S *pstShadingAttr);

HI_S32 HI_MPI_ISP_SetShadingTable(const ISP_SHADINGTAB_S *pstShadingTab);
HI_S32 HI_MPI_ISP_GetShadingTable(ISP_SHADINGTAB_S *pstShadingTab);

HI_S32 HI_MPI_ISP_SetDenoiseAttr(const ISP_DENOISE_ATTR_S *pstDenoiseAttr);
HI_S32 HI_MPI_ISP_GetDenoiseAttr(ISP_DENOISE_ATTR_S *pstDenoiseAttr);

HI_S32 HI_MPI_ISP_SetGammaAttr(const ISP_GAMMA_ATTR_S* pstGammaAttr);
HI_S32 HI_MPI_ISP_GetGammaAttr(ISP_GAMMA_ATTR_S* pstGammaAttr);

HI_S32 HI_MPI_ISP_SetGammaTable(const ISP_GAMMA_TABLE_S* pstGammaAttr);
HI_S32 HI_MPI_ISP_GetGammaTable(ISP_GAMMA_TABLE_S* pstGammaAttr);

HI_S32 HI_MPI_ISP_SetGammaFETable(const ISP_GAMMA_TABLE_S* pstGammaAttr);
HI_S32 HI_MPI_ISP_GetGammaFETable(ISP_GAMMA_TABLE_S* pstGammaAttr);

HI_S32 HI_MPI_ISP_SetSharpenAttr(const ISP_SHARPEN_ATTR_S *pstSharpenAttr);
HI_S32 HI_MPI_ISP_GetSharpenAttr(ISP_SHARPEN_ATTR_S *pstSharpenAttr);

HI_S32 HI_MPI_ISP_SetCfg(HI_U32 u32Addr, HI_U32 u32Value);
HI_S32 HI_MPI_ISP_GetCfg(HI_U32 u32Addr, HI_U32 *pu32Value);

HI_S32 HI_MPI_ISP_SetCrosstalkAttr(const ISP_CR_ATTR_S *pstCRAttr);
HI_S32 HI_MPI_ISP_GetCrosstalkAttr(ISP_CR_ATTR_S *pstCRAttr);

HI_S32 HI_MPI_ISP_SetNoiseProfileTable(ISP_NOISEPROFILE_TABLE_S *pstNoiseProfileTable);
HI_S32 HI_MPI_ISP_GetNoiseProfileTable(ISP_NOISEPROFILE_TABLE_S *pstNoiseProfileTable);



// TODO:
HI_S32 HI_MPI_ISP_SetAntiFogAttr(const ISP_ANTIFOG_S *pstAntiFog);
HI_S32 HI_MPI_ISP_GetAntiFogAttr(ISP_ANTIFOG_S *pstAntiFog);

/* ANTI_FALSECOLOR */
// TODO:
HI_S32 HI_MPI_ISP_SetAntiFalseColorAttr(const ISP_ANTI_FALSECOLOR_S *pstAntiFC);
HI_S32 HI_MPI_ISP_GetAntiFalseColorAttr(ISP_ANTI_FALSECOLOR_S *pstAntiFC);

HI_S32 HI_MPI_ISP_SetDemosaicAttr(const ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);
HI_S32 HI_MPI_ISP_GetDemosaicAttr(ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);

HI_S32 HI_MPI_ISP_SetBlackLevelAttr(const ISP_BLACK_LEVEL_S *pstBlackLevel);
HI_S32 HI_MPI_ISP_GetBlackLevelAttr(ISP_BLACK_LEVEL_S *pstBlackLevel);

HI_S32 HI_MPI_ISP_SnsRegsCfg(const ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);

HI_S32 HI_MPI_ISP_GetVDTimeOut(ISP_VD_INFO_S *pstIspVdInfo, HI_U32 u32MilliSec);

HI_S32 HI_MPI_ISP_GetISPRegAttr(ISP_REG_ATTR_S * pstIspRegAttr);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_ISP_H__ */

