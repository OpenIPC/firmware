/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: Vpss mpi interface
 * Author: Hisilicon multimedia software group
 * Create: 2016/09/27
 */

#ifndef __MPI_VPSS_H__
#define __MPI_VPSS_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vb.h"
#include "hi_comm_vpss.h"
#include "hi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Group Settings */
HI_S32 HI_MPI_VPSS_CreateGrp(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_StartGrp(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_StopGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_ResetGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S *pstGrpAttr);

HI_S32 HI_MPI_VPSS_SetGrpCrop(VPSS_GRP VpssGrp, const VPSS_CROP_INFO_S *pstCropInfo);
HI_S32 HI_MPI_VPSS_GetGrpCrop(VPSS_GRP VpssGrp, VPSS_CROP_INFO_S *pstCropInfo);

HI_S32 HI_MPI_VPSS_SendFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe,
    const VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VPSS_GetGrpFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe, VIDEO_FRAME_INFO_S *pstVideoFrame);
HI_S32 HI_MPI_VPSS_ReleaseGrpFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe,
    const VIDEO_FRAME_INFO_S *pstVideoFrame);

HI_S32 HI_MPI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_SetGrpSharpen(VPSS_GRP VpssGrp, const VPSS_GRP_SHARPEN_ATTR_S *pstGrpSharpenAttr);
HI_S32 HI_MPI_VPSS_GetGrpSharpen(VPSS_GRP VpssGrp, VPSS_GRP_SHARPEN_ATTR_S *pstGrpSharpenAttr);

HI_S32 HI_MPI_VPSS_SetGrpDelay(VPSS_GRP VpssGrp, HI_U32 u32Delay);
HI_S32 HI_MPI_VPSS_GetGrpDelay(VPSS_GRP VpssGrp, HI_U32 *pu32Delay);

HI_S32 HI_MPI_VPSS_SetGrpFisheyeConfig(VPSS_GRP VpssGrp, const FISHEYE_CONFIG_S *pstFisheyeConfig);
HI_S32 HI_MPI_VPSS_GetGrpFisheyeConfig(VPSS_GRP VpssGrp, FISHEYE_CONFIG_S *pstFisheyeConfig);

HI_S32 HI_MPI_VPSS_EnableUserFrameRateCtrl(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_DisableUserFrameRateCtrl(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_SetGrpFrameInterruptAttr(VPSS_GRP VpssGrp, const FRAME_INTERRUPT_ATTR_S *pstFrameIntAttr);
HI_S32 HI_MPI_VPSS_GetGrpFrameInterruptAttr(VPSS_GRP VpssGrp, FRAME_INTERRUPT_ATTR_S *pstFrameIntAttr);

/* Chn Settings */
HI_S32 HI_MPI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 HI_MPI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

HI_S32 HI_MPI_VPSS_SetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CROP_INFO_S *pstCropInfo);
HI_S32 HI_MPI_VPSS_GetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CROP_INFO_S *pstCropInfo);

HI_S32 HI_MPI_VPSS_SetChnRotation(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATION_E enRotation);
HI_S32 HI_MPI_VPSS_GetChnRotation(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATION_E *penRotation);

HI_S32 HI_MPI_VPSS_SetChnRotationEx(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
    const VPSS_ROTATION_EX_ATTR_S *pstRotationExAttr);
HI_S32 HI_MPI_VPSS_GetChnRotationEx(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_ROTATION_EX_ATTR_S *pstRotationExAttr);

HI_S32 HI_MPI_VPSS_SetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_LDC_ATTR_S *pstLDCAttr);
HI_S32 HI_MPI_VPSS_GetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LDC_ATTR_S *pstLDCAttr);

HI_S32 HI_MPI_VPSS_SetChnLDCV3Attr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_LDCV3_ATTR_S *pstLDCV3Attr);
HI_S32 HI_MPI_VPSS_GetChnLDCV3Attr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LDCV3_ATTR_S *pstLDCV3Attr);

HI_S32 HI_MPI_VPSS_SetChnSpreadAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const SPREAD_ATTR_S *pstSpreadAttr);
HI_S32 HI_MPI_VPSS_GetChnSpreadAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, SPREAD_ATTR_S *pstSpreadAttr);

HI_S32 HI_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
    VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VIDEO_FRAME_INFO_S *pstVideoFrame);

HI_S32 HI_MPI_VPSS_GetRegionLuma(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VIDEO_REGION_INFO_S *pstRegionInfo,
                                 HI_U64 *pu64LumaData, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VPSS_SetLowDelayAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_LOW_DELAY_INFO_S *pstLowDelayInfo);
HI_S32 HI_MPI_VPSS_GetLowDelayAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LOW_DELAY_INFO_S *pstLowDelayInfo);

HI_S32 HI_MPI_VPSS_SetChnBufWrapAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CHN_BUF_WRAP_S *pstVpssChnBufWrap);
HI_S32 HI_MPI_VPSS_GetChnBufWrapAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_BUF_WRAP_S *pstVpssChnBufWrap);

HI_S32 HI_MPI_VPSS_TriggerSnapFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32FrameCnt);

HI_S32 HI_MPI_VPSS_AttachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VB_POOL hVbPool);
HI_S32 HI_MPI_VPSS_DetachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

HI_S32 HI_MPI_VPSS_EnableBufferShare(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 HI_MPI_VPSS_DisableBufferShare(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

HI_S32 HI_MPI_VPSS_SetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32Align);
HI_S32 HI_MPI_VPSS_GetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 *pu32Align);

HI_S32 HI_MPI_VPSS_SetChnProcMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_PROC_MODE_E enVpssChnProcMode);
HI_S32 HI_MPI_VPSS_GetChnProcMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_PROC_MODE_E *penVpssChnProcMode);

/* ExtChn Settings */
HI_S32 HI_MPI_VPSS_SetExtChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_EXT_CHN_ATTR_S *pstExtChnAttr);
HI_S32 HI_MPI_VPSS_GetExtChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_EXT_CHN_ATTR_S *pstExtChnAttr);

HI_S32 HI_MPI_VPSS_SetExtChnFisheye(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const FISHEYE_ATTR_S *pstFishEyeAttr);
HI_S32 HI_MPI_VPSS_GetExtChnFisheye(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, FISHEYE_ATTR_S *pstFishEyeAttr);

HI_S32 HI_MPI_VPSS_FisheyePosQueryDst2Src(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32RegionIndex,
                                          const POINT_S *pstDstPointIn, POINT_S *pstSrcPointOut);

/* 3DNR */
HI_S32 HI_MPI_VPSS_SetGrpNRXParam(VPSS_GRP VpssGrp, const VPSS_GRP_NRX_PARAM_S *pstNRXParam);
HI_S32 HI_MPI_VPSS_GetGrpNRXParam(VPSS_GRP VpssGrp, VPSS_GRP_NRX_PARAM_S *pstNRXParam);

/* Module Param Settings */
HI_S32 HI_MPI_VPSS_SetModParam(const VPSS_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VPSS_GetModParam(VPSS_MOD_PARAM_S *pstModParam);

HI_S32 HI_MPI_VPSS_GetChnFd(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 HI_MPI_VPSS_CloseFd(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */


