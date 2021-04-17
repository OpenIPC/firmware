/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: mpi_vi.h
 * Author:
 * Create: 2016-09-19
 */

#ifndef __MPI_VI_H__
#define __MPI_VI_H__

#include "hi_comm_vi.h"
#include "hi_comm_dis.h"
#include "hi_comm_gdc.h"
#include "hi_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 1 for vi device */
HI_S32 HI_MPI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);
HI_S32 HI_MPI_VI_GetDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);

HI_S32 HI_MPI_VI_SetDevAttrEx(VI_DEV ViDev, const VI_DEV_ATTR_EX_S *pstDevAttrEx);
HI_S32 HI_MPI_VI_GetDevAttrEx(VI_DEV ViDev, VI_DEV_ATTR_EX_S *pstDevAttrEx);

HI_S32 HI_MPI_VI_SetVSSignalAttr(VI_DEV ViDev, const VI_VS_SIGNAL_ATTR_S *pstVSSignalAttr);
HI_S32 HI_MPI_VI_GetVSSignalAttr(VI_DEV ViDev, VI_VS_SIGNAL_ATTR_S *pstVSSignalAttr);
HI_S32 HI_MPI_VI_TriggerVSSignal(VI_DEV ViDev, HI_BOOL bEnable);

HI_S32 HI_MPI_VI_EnableDev(VI_DEV ViDev);
HI_S32 HI_MPI_VI_DisableDev(VI_DEV ViDev);

HI_S32 HI_MPI_VI_SetMipiBindDev(VI_DEV ViDev, MIPI_DEV MipiDev);
HI_S32 HI_MPI_VI_GetMipiBindDev(VI_DEV ViDev, MIPI_DEV *pMipiDev);

HI_S32 HI_MPI_VI_SetDevBindPipe(VI_DEV ViDev, const VI_DEV_BIND_PIPE_S *pstDevBindPipe);
HI_S32 HI_MPI_VI_GetDevBindPipe(VI_DEV ViDev, VI_DEV_BIND_PIPE_S *pstDevBindPipe);

HI_S32 HI_MPI_VI_SetDevTimingAttr(VI_DEV ViDev, const VI_DEV_TIMING_ATTR_S *pstTimingAttr);
HI_S32 HI_MPI_VI_GetDevTimingAttr(VI_DEV ViDev, VI_DEV_TIMING_ATTR_S *pstTimingAttr);

/* 2 for vi pipe */
HI_S32 HI_MPI_VI_GetPipeCmpParam(VI_PIPE ViPipe, VI_CMP_PARAM_S *pCmpParam);

HI_S32 HI_MPI_VI_SetUserPic(VI_PIPE ViPipe, const VI_USERPIC_ATTR_S *pstUsrPic);
HI_S32 HI_MPI_VI_EnableUserPic(VI_PIPE ViPipe);
HI_S32 HI_MPI_VI_DisableUserPic(VI_PIPE ViPipe);

HI_S32 HI_MPI_VI_CreatePipe(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
HI_S32 HI_MPI_VI_DestroyPipe(VI_PIPE ViPipe);

HI_S32 HI_MPI_VI_SetPipeAttr(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
HI_S32 HI_MPI_VI_GetPipeAttr(VI_PIPE ViPipe, VI_PIPE_ATTR_S *pstPipeAttr);

HI_S32 HI_MPI_VI_StartPipe(VI_PIPE ViPipe);
HI_S32 HI_MPI_VI_StopPipe(VI_PIPE ViPipe);

HI_S32 HI_MPI_VI_SetPipeCrop(VI_PIPE ViPipe, const CROP_INFO_S *pstCropInfo);
HI_S32 HI_MPI_VI_GetPipeCrop(VI_PIPE ViPipe, CROP_INFO_S *pstCropInfo);

HI_S32 HI_MPI_VI_SetPipeFisheyeConfig(VI_PIPE ViPipe, const FISHEYE_CONFIG_S *pstFishEyeConfig);
HI_S32 HI_MPI_VI_GetPipeFisheyeConfig(VI_PIPE ViPipe, FISHEYE_CONFIG_S *pstFishEyeConfig);

HI_S32 HI_MPI_VI_FisheyePosQueryDst2Src(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32RegionIndex,
    const POINT_S *pstDstPointIn, POINT_S *pstSrcPointOut);

HI_S32 HI_MPI_VI_SetPipeDumpAttr(VI_PIPE ViPipe, const VI_DUMP_ATTR_S *pstDumpAttr);
HI_S32 HI_MPI_VI_GetPipeDumpAttr(VI_PIPE ViPipe, VI_DUMP_ATTR_S *pstDumpAttr);

HI_S32 HI_MPI_VI_SetPipeFrameSource(VI_PIPE ViPipe, const VI_PIPE_FRAME_SOURCE_E enSource);
HI_S32 HI_MPI_VI_GetPipeFrameSource(VI_PIPE ViPipe, VI_PIPE_FRAME_SOURCE_E *penSource);

HI_S32 HI_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_ReleasePipeFrame(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame);

HI_S32 HI_MPI_VI_SendPipeYUV(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_SendPipeRaw(HI_U32 u32PipeNum, VI_PIPE PipeId[], const VIDEO_FRAME_INFO_S *pstVideoFrame[],
                             HI_S32 s32MilliSec);

HI_S32 HI_MPI_VI_SetPipeNRXParam(VI_PIPE ViPipe, const VI_PIPE_NRX_PARAM_S *pstNrXParam);
HI_S32 HI_MPI_VI_GetPipeNRXParam(VI_PIPE ViPipe, VI_PIPE_NRX_PARAM_S *pstNrXParam);

HI_S32 HI_MPI_VI_SetPipeRepeatMode(VI_PIPE ViPipe, const VI_PIPE_REPEAT_MODE_E enPepeatMode);
HI_S32 HI_MPI_VI_GetPipeRepeatMode(VI_PIPE ViPipe, VI_PIPE_REPEAT_MODE_E *penPepeatMode);

HI_S32 HI_MPI_VI_QueryPipeStatus(VI_PIPE ViPipe, VI_PIPE_STATUS_S *pstStatus);

HI_S32 HI_MPI_VI_EnablePipeInterrupt(VI_PIPE ViPipe);
HI_S32 HI_MPI_VI_DisablePipeInterrupt(VI_PIPE ViPipe);

HI_S32 HI_MPI_VI_SetPipeVCNumber(VI_PIPE ViPipe, HI_U32 u32VCNumber);
HI_S32 HI_MPI_VI_GetPipeVCNumber(VI_PIPE ViPipe, HI_U32 *pu32VCNumber);

HI_S32 HI_MPI_VI_SetPipeFrameInterruptAttr(VI_PIPE ViPipe, const FRAME_INTERRUPT_ATTR_S *pstFrameIntAttr);
HI_S32 HI_MPI_VI_GetPipeFrameInterruptAttr(VI_PIPE ViPipe, FRAME_INTERRUPT_ATTR_S *pstFrameIntAttr);

HI_S32 HI_MPI_VI_SetPipeBNRRawDumpAttr(VI_PIPE ViPipe, const BNR_DUMP_ATTR_S *pstBnrDumpAttr);
HI_S32 HI_MPI_VI_GetPipeBNRRawDumpAttr(VI_PIPE ViPipe, BNR_DUMP_ATTR_S *pstBnrDumpAttr);

HI_S32 HI_MPI_VI_GetPipeBNRRaw(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_ReleasePipeBNRRaw(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame);

HI_S32 HI_MPI_VI_PipeAttachVbPool(VI_PIPE ViPipe, VB_POOL VbPool);
HI_S32 HI_MPI_VI_PipeDetachVbPool(VI_PIPE ViPipe);

HI_S32 HI_MPI_VI_GetPipeFd(VI_PIPE ViPipe);

/* 3 for vi chn */
HI_S32 HI_MPI_VI_SetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VI_GetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VI_EnableChn(VI_PIPE ViPipe, VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableChn(VI_PIPE ViPipe, VI_CHN ViChn);

HI_S32 HI_MPI_VI_SetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CROP_INFO_S *pstCropInfo);
HI_S32 HI_MPI_VI_GetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, VI_CROP_INFO_S *pstCropInfo);

HI_S32 HI_MPI_VI_SetChnRotation(VI_PIPE ViPipe, VI_CHN ViChn, const ROTATION_E enRotation);
HI_S32 HI_MPI_VI_GetChnRotation(VI_PIPE ViPipe, VI_CHN ViChn, ROTATION_E *penRotation);

HI_S32 HI_MPI_VI_SetChnRotationEx(VI_PIPE ViPipe, VI_CHN ViChn, const VI_ROTATION_EX_ATTR_S *pstViRotationExAttr);
HI_S32 HI_MPI_VI_GetChnRotationEx(VI_PIPE ViPipe, VI_CHN ViChn, VI_ROTATION_EX_ATTR_S *pstViRotationExAttr);

HI_S32 HI_MPI_VI_SetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LDC_ATTR_S *pstLDCAttr);
HI_S32 HI_MPI_VI_GetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LDC_ATTR_S *pstLDCAttr);

HI_S32 HI_MPI_VI_SetChnLDCV2Attr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LDCV2_ATTR_S *pstLDCV2Attr);
HI_S32 HI_MPI_VI_GetChnLDCV2Attr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LDCV2_ATTR_S *pstLDCV2Attr);

HI_S32 HI_MPI_VI_SetChnLDCV3Attr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LDCV3_ATTR_S *pstLDCV3Attr);
HI_S32 HI_MPI_VI_GetChnLDCV3Attr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LDCV3_ATTR_S *pstLDCV3Attr);

HI_S32 HI_MPI_VI_SetChnSpreadAttr(VI_PIPE ViPipe, VI_CHN ViChn, const SPREAD_ATTR_S *pstSpreadAttr);
HI_S32 HI_MPI_VI_GetChnSpreadAttr(VI_PIPE ViPipe, VI_CHN ViChn, SPREAD_ATTR_S *pstSpreadAttr);

HI_S32 HI_MPI_VI_SetChnLowDelayAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LOW_DELAY_INFO_S *pstLowDelayInfo);
HI_S32 HI_MPI_VI_GetChnLowDelayAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LOW_DELAY_INFO_S *pstLowDelayInfo);

HI_S32 HI_MPI_VI_GetChnRegionLuma(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_REGION_INFO_S *pstRegionInfo,
                                  HI_U64 *pu64LumaData, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_SetChnDISConfig(VI_PIPE ViPipe, VI_CHN ViChn, const DIS_CONFIG_S *pstDISConfig);
HI_S32 HI_MPI_VI_GetChnDISConfig(VI_PIPE ViPipe, VI_CHN ViChn, DIS_CONFIG_S *pstDISConfig);
HI_S32 HI_MPI_VI_SetChnDISAttr(VI_PIPE ViPipe, VI_CHN ViChn, const DIS_ATTR_S *pstDISAttr);
HI_S32 HI_MPI_VI_GetChnDISAttr(VI_PIPE ViPipe, VI_CHN ViChn, DIS_ATTR_S *pstDISAttr);

HI_S32 HI_MPI_VI_SetExtChnFisheye(VI_PIPE ViPipe, VI_CHN ViChn, const FISHEYE_ATTR_S *pstFishEyeAttr);
HI_S32 HI_MPI_VI_GetExtChnFisheye(VI_PIPE ViPipe, VI_CHN ViChn, FISHEYE_ATTR_S *pstFishEyeAttr);

HI_S32 HI_MPI_VI_SetExtChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_EXT_CHN_ATTR_S *pstExtChnAttr);
HI_S32 HI_MPI_VI_GetExtChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_EXT_CHN_ATTR_S *pstExtChnAttr);

HI_S32 HI_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_ReleaseChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

HI_S32 HI_MPI_VI_SetChnEarlyInterrupt(VI_PIPE ViPipe, VI_CHN ViChn, const VI_EARLY_INTERRUPT_S *pstEarlyInterrupt);
HI_S32 HI_MPI_VI_GetChnEarlyInterrupt(VI_PIPE ViPipe, VI_CHN ViChn, VI_EARLY_INTERRUPT_S *pstEarlyInterrupt);

HI_S32 HI_MPI_VI_SetChnAlign(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32Align);
HI_S32 HI_MPI_VI_GetChnAlign(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 *pu32Align);

HI_S32 HI_MPI_VI_ChnAttachVbPool(VI_PIPE ViPipe, VI_CHN ViChn, VB_POOL VbPool);
HI_S32 HI_MPI_VI_ChnDetachVbPool(VI_PIPE ViPipe, VI_CHN ViChn);

HI_S32 HI_MPI_VI_QueryChnStatus(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_STATUS_S *pstChnStatus);

HI_S32 HI_MPI_VI_GetChnFd(VI_PIPE ViPipe, VI_CHN ViChn);

/* 4 for vi stitch group */
HI_S32 HI_MPI_VI_SetStitchGrpAttr(VI_STITCH_GRP StitchGrp, const VI_STITCH_GRP_ATTR_S *pstStitchGrpAttr);
HI_S32 HI_MPI_VI_GetStitchGrpAttr(VI_STITCH_GRP StitchGrp, VI_STITCH_GRP_ATTR_S *pstStitchGrpAttr);

/* 5 for vi module */
HI_S32 HI_MPI_VI_SetModParam(const VI_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VI_GetModParam(VI_MOD_PARAM_S *pstModParam);

HI_S32 HI_MPI_VI_CloseFd(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VI_H__ */


