/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_VENC_H_
#define _MI_VENC_H_

#include "mi_venc_datatype.h"

#define VENC_MAJOR_VERSION 2
#define VENC_SUB_VERSION 16
#define MACRO_TO_STR(macro) #macro
#define VENC_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_venc_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_venc_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_venc_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_VENC_API_VERSION VENC_VERSION_STR(VENC_MAJOR_VERSION,VENC_SUB_VERSION)

#ifdef __cplusplus
extern "C"
{
#endif

MI_S32 MI_VENC_GetChnDevid(MI_VENC_CHN VeChn, MI_U32 *pu32Devid);
MI_S32 MI_VENC_SetModParam(MI_VENC_ModParam_t *pstModParam);
MI_S32 MI_VENC_GetModParam(MI_VENC_ModParam_t *pstModParam);
MI_S32 MI_VENC_CreateChn(MI_VENC_CHN VeChn, MI_VENC_ChnAttr_t *pstAttr);
MI_S32 MI_VENC_DestroyChn(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_ResetChn(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_StartRecvPic(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_StartRecvPicEx(MI_VENC_CHN VeChn, MI_VENC_RecvPicParam_t *pstRecvParam);
MI_S32 MI_VENC_StopRecvPic(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_Query(MI_VENC_CHN VeChn, MI_VENC_ChnStat_t *pstStat);
MI_S32 MI_VENC_SetChnAttr(MI_VENC_CHN VeChn, MI_VENC_ChnAttr_t *pstAttr);
MI_S32 MI_VENC_GetChnAttr(MI_VENC_CHN VeChn, MI_VENC_ChnAttr_t *pstAttr);
MI_S32 MI_VENC_GetStream(MI_VENC_CHN VeChn, MI_VENC_Stream_t *pstStream, MI_S32 s32MilliSec);
MI_S32 MI_VENC_ReleaseStream(MI_VENC_CHN VeChn, MI_VENC_Stream_t *pstStream);
MI_S32 MI_VENC_InsertUserData(MI_VENC_CHN VeChn, MI_U8 *pu8Data, MI_U32 u32Len);
MI_S32 MI_VENC_SetMaxStreamCnt(MI_VENC_CHN VeChn, MI_U32 u32MaxStrmCnt);
MI_S32 MI_VENC_GetMaxStreamCnt(MI_VENC_CHN VeChn, MI_U32 *pu32MaxStrmCnt);
MI_S32 MI_VENC_RequestIdr(MI_VENC_CHN VeChn, MI_BOOL bInstant);
MI_S32 MI_VENC_EnableIdr(MI_VENC_CHN VeChn, MI_BOOL bEnableIdr);
MI_S32 MI_VENC_SetH264IdrPicId(MI_VENC_CHN VeChn, MI_VENC_H264IdrPicIdCfg_t *pstH264eIdrPicIdCfg);
MI_S32 MI_VENC_GetH264IdrPicId(MI_VENC_CHN VeChn, MI_VENC_H264IdrPicIdCfg_t *pstH264eIdrPicIdCfg);
MI_S32 MI_VENC_GetFd(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_CloseFd(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_SetRoiCfg(MI_VENC_CHN VeChn, MI_VENC_RoiCfg_t *pstVencRoiCfg);
MI_S32 MI_VENC_GetRoiCfg(MI_VENC_CHN VeChn, MI_U32 u32Index, MI_VENC_RoiCfg_t *pstVencRoiCfg);
MI_S32 MI_VENC_SetRoiBgFrameRate(MI_VENC_CHN VeChn, MI_VENC_RoiBgFrameRate_t *pstRoiBgFrmRate);
MI_S32 MI_VENC_GetRoiBgFrameRate(MI_VENC_CHN VeChn, MI_VENC_RoiBgFrameRate_t *pstRoiBgFrmRate);
MI_S32 MI_VENC_SetH264SliceSplit(MI_VENC_CHN VeChn, MI_VENC_ParamH264SliceSplit_t *pstSliceSplit);
MI_S32 MI_VENC_GetH264SliceSplit(MI_VENC_CHN VeChn, MI_VENC_ParamH264SliceSplit_t *pstSliceSplit);
MI_S32 MI_VENC_SetH264InterPred(MI_VENC_CHN VeChn, MI_VENC_ParamH264InterPred_t *pstH264InterPred);
MI_S32 MI_VENC_GetH264InterPred(MI_VENC_CHN VeChn, MI_VENC_ParamH264InterPred_t *pstH264InterPred);
MI_S32 MI_VENC_SetH264IntraPred(MI_VENC_CHN VeChn, MI_VENC_ParamH264IntraPred_t *pstH264IntraPred);
MI_S32 MI_VENC_GetH264IntraPred(MI_VENC_CHN VeChn, MI_VENC_ParamH264IntraPred_t *pstH264IntraPred);
MI_S32 MI_VENC_SetH264Trans(MI_VENC_CHN VeChn, MI_VENC_ParamH264Trans_t *pstH264Trans);
MI_S32 MI_VENC_GetH264Trans(MI_VENC_CHN VeChn, MI_VENC_ParamH264Trans_t *pstH264Trans);
MI_S32 MI_VENC_SetH264Entropy(MI_VENC_CHN VeChn, MI_VENC_ParamH264Entropy_t *pstH264EntropyEnc);
MI_S32 MI_VENC_GetH264Entropy(MI_VENC_CHN VeChn, MI_VENC_ParamH264Entropy_t *pstH264EntropyEnc);
MI_S32 MI_VENC_SetH264Dblk(MI_VENC_CHN VeChn, MI_VENC_ParamH264Dblk_t *pstH264Dblk);
MI_S32 MI_VENC_GetH264Dblk(MI_VENC_CHN VeChn, MI_VENC_ParamH264Dblk_t *pstH264Dblk);
MI_S32 MI_VENC_SetH264Vui(MI_VENC_CHN VeChn, MI_VENC_ParamH264Vui_t*pstH264Vui);
MI_S32 MI_VENC_GetH264Vui(MI_VENC_CHN VeChn, MI_VENC_ParamH264Vui_t *pstH264Vui);
MI_S32 MI_VENC_SetH265SliceSplit(MI_VENC_CHN VeChn, MI_VENC_ParamH265SliceSplit_t *pstSliceSplit);
MI_S32 MI_VENC_GetH265SliceSplit(MI_VENC_CHN VeChn, MI_VENC_ParamH265SliceSplit_t *pstSliceSplit);
MI_S32 MI_VENC_SetH265InterPred(MI_VENC_CHN VeChn, MI_VENC_ParamH265InterPred_t *pstH265InterPred);
MI_S32 MI_VENC_GetH265InterPred(MI_VENC_CHN VeChn, MI_VENC_ParamH265InterPred_t *pstH265InterPred);
MI_S32 MI_VENC_SetH265IntraPred(MI_VENC_CHN VeChn, MI_VENC_ParamH265IntraPred_t *pstH265IntraPred);
MI_S32 MI_VENC_GetH265IntraPred(MI_VENC_CHN VeChn, MI_VENC_ParamH265IntraPred_t *pstH265IntraPred);
MI_S32 MI_VENC_SetH265Trans(MI_VENC_CHN VeChn, MI_VENC_ParamH265Trans_t *pstH265Trans);
MI_S32 MI_VENC_GetH265Trans(MI_VENC_CHN VeChn, MI_VENC_ParamH265Trans_t *pstH265Trans);
MI_S32 MI_VENC_SetH265Dblk(MI_VENC_CHN VeChn, MI_VENC_ParamH265Dblk_t *pstH265Dblk);
MI_S32 MI_VENC_GetH265Dblk(MI_VENC_CHN VeChn, MI_VENC_ParamH265Dblk_t *pstH265Dblk);
MI_S32 MI_VENC_SetH265Vui(MI_VENC_CHN VeChn, MI_VENC_ParamH265Vui_t*pstH265Vui);
MI_S32 MI_VENC_GetH265Vui(MI_VENC_CHN VeChn, MI_VENC_ParamH265Vui_t *pstH265Vui);
MI_S32 MI_VENC_SetJpegParam(MI_VENC_CHN VeChn, MI_VENC_ParamJpeg_t *pstJpegParam);
MI_S32 MI_VENC_GetJpegParam(MI_VENC_CHN VeChn, MI_VENC_ParamJpeg_t *pstJpegParam);
MI_S32 MI_VENC_SetRcParam(MI_VENC_CHN VeChn, MI_VENC_RcParam_t *pstRcParam);
MI_S32 MI_VENC_GetRcParam(MI_VENC_CHN VeChn, MI_VENC_RcParam_t *pstRcParam);
MI_S32 MI_VENC_SetRefParam(MI_VENC_CHN VeChn, MI_VENC_ParamRef_t *pstRefParam);
MI_S32 MI_VENC_GetRefParam(MI_VENC_CHN VeChn, MI_VENC_ParamRef_t *pstRefParam);
MI_S32 MI_VENC_SetCrop(MI_VENC_CHN VeChn, MI_VENC_CropCfg_t *pstCropCfg);
MI_S32 MI_VENC_GetCrop(MI_VENC_CHN VeChn, MI_VENC_CropCfg_t *pstCropCfg);
MI_S32 MI_VENC_SetFrameLostStrategy(MI_VENC_CHN VeChn, MI_VENC_ParamFrameLost_t *pstFrmLostParam);
MI_S32 MI_VENC_GetFrameLostStrategy(MI_VENC_CHN VeChn, MI_VENC_ParamFrameLost_t *pstFrmLostParam);
MI_S32 MI_VENC_SetSuperFrameCfg(MI_VENC_CHN VeChn, MI_VENC_SuperFrameCfg_t *pstSuperFrmParam);
MI_S32 MI_VENC_GetSuperFrameCfg(MI_VENC_CHN VeChn, MI_VENC_SuperFrameCfg_t *pstSuperFrmParam);
MI_S32 MI_VENC_SetRcPriority(MI_VENC_CHN VeChn, MI_VENC_RcPriority_e *peRcPriority);
MI_S32 MI_VENC_GetRcPriority(MI_VENC_CHN VeChn, MI_VENC_RcPriority_e *peRcPriority);
MI_S32 MI_VENC_DupChn(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_SetInputSourceConfig(MI_VENC_CHN VeChn, MI_VENC_InputSourceConfig_t *pstInputSourceConfig);
MI_S32 MI_VENC_AllocCustomMap (MI_VENC_CHN VeChn, MI_PHY *pPhyAddr, void **ppCpuAddr);
MI_S32 MI_VENC_ApplyCustomMap (MI_VENC_CHN VeChn, MI_PHY PhyAddr);
MI_S32 MI_VENC_GetLastHistoStaticInfo (MI_VENC_CHN VeChn, MI_VENC_FrameHistoStaticInfo_t** ppFrmHistoStaticInfo);
MI_S32 MI_VENC_ReleaseHistoStaticInfo(MI_VENC_CHN VeChn);
MI_S32 MI_VENC_SetAdvCustRcAttr(MI_VENC_CHN VeChn, MI_VENC_AdvCustRcAttr_t *pstAdvCustRcAttr);
MI_S32 MI_VENC_SetSmartDetInfo(MI_VENC_CHN VeChn, MI_VENC_SmartDetInfo_t *pstSmartDetInfo);
MI_S32 MI_VENC_SetIntraRefresh(MI_VENC_CHN VeChn, MI_VENC_IntraRefresh_t *pstIntraRefresh);
MI_S32 MI_VENC_GetIntraRefresh(MI_VENC_CHN VeChn, MI_VENC_IntraRefresh_t *pstIntraRefresh);
MI_S32 MI_VENC_InitDev(MI_VENC_InitParam_t *pstInitParam);
MI_S32 MI_VENC_DeInitDev(void);
#ifdef __cplusplus
}
#endif

#endif
