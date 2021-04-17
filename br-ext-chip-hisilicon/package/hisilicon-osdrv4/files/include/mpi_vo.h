/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: Common defination of video output
* Author: Hisilicon multimedia software group
* Create: 2017/07/04
* Function List: VO device interface, video layer interface, graphic layer interface,
                 channel interface, wbc interface.
*/
#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "hi_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Device Relative Settings */
HI_S32 HI_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
HI_S32 HI_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

HI_S32 HI_MPI_VO_Enable(VO_DEV VoDev);
HI_S32 HI_MPI_VO_Disable(VO_DEV VoDev);

HI_S32 HI_MPI_VO_CloseFd(HI_VOID);
HI_S32 HI_MPI_VO_SetUserIntfSyncInfo(VO_DEV VoDev, VO_USER_INTFSYNC_INFO_S *pstUserInfo);

/* Video Relative Settings */
HI_S32 HI_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 HI_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

HI_S32 HI_MPI_VO_EnableVideoLayer(VO_LAYER VoLayer);
HI_S32 HI_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);

HI_S32 HI_MPI_VO_BindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);
HI_S32 HI_MPI_VO_UnBindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, HI_U32 u32Priority);
HI_S32 HI_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, HI_U32 *pu32Priority);

HI_S32 HI_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
HI_S32 HI_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

HI_S32 HI_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E enPartMode);
HI_S32 HI_MPI_VO_GetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E *penPartMode);

HI_S32 HI_MPI_VO_BatchBegin(VO_LAYER VoLayer);
HI_S32 HI_MPI_VO_BatchEnd(VO_LAYER VoLayer);

HI_S32 HI_MPI_VO_SetVideoLayerBoundary(VO_LAYER VoLayer, const VO_LAYER_BOUNDARY_S *pstLayerBoundary);
HI_S32 HI_MPI_VO_GetVideoLayerBoundary(VO_LAYER VoLayer, VO_LAYER_BOUNDARY_S *pstLayerBoundary);

HI_S32 HI_MPI_VO_SetVideoLayerParam(VO_LAYER VoLayer, const VO_LAYER_PARAM_S *pstLayerParam);
HI_S32 HI_MPI_VO_GetVideoLayerParam(VO_LAYER VoLayer, VO_LAYER_PARAM_S *pstLayerParam);

HI_S32 HI_MPI_VO_SetVideoLayerDecompress(VO_LAYER VoLayer, HI_BOOL bSupportDecompress);
HI_S32 HI_MPI_VO_GetVideoLayerDecompress(VO_LAYER VoLayer, HI_BOOL *pbSupportDecompress);

HI_S32 HI_MPI_VO_SetVideoLayerCrop(VO_LAYER VoLayer, const CROP_INFO_S *pstCropInfo);
HI_S32 HI_MPI_VO_GetVideoLayerCrop(VO_LAYER VoLayer, CROP_INFO_S *pstCropInfo);

/* Display relative operations */
HI_S32 HI_MPI_VO_SetPlayToleration(VO_LAYER VoLayer, HI_U32 u32Toleration);
HI_S32 HI_MPI_VO_GetPlayToleration(VO_LAYER VoLayer, HI_U32 *pu32Toleration);

HI_S32 HI_MPI_VO_GetScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, const VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 HI_MPI_VO_SetDisplayBufLen(VO_LAYER VoLayer, HI_U32 u32BufLen);
HI_S32 HI_MPI_VO_GetDisplayBufLen(VO_LAYER VoLayer, HI_U32 *pu32BufLen);

/* Channel Relative Operations */
HI_S32 HI_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VO_EnableChn(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_SetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_PARAM_S *pstChnParam);
HI_S32 HI_MPI_VO_GetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_PARAM_S *pstChnParam);

HI_S32 HI_MPI_VO_SetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, const POINT_S *pstDispPos);
HI_S32 HI_MPI_VO_GetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, POINT_S *pstDispPos);

HI_S32 HI_MPI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, HI_S32 s32ChnFrmRate);
HI_S32 HI_MPI_VO_GetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, HI_S32 *ps32ChnFrmRate);

HI_S32 HI_MPI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstFrame);

HI_S32 HI_MPI_VO_PauseChn(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_StepChn(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_RefreshChn(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_SetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, const VO_ZOOM_ATTR_S *pstZoomAttr);
HI_S32 HI_MPI_VO_GetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, VO_ZOOM_ATTR_S *pstZoomAttr);

HI_S32 HI_MPI_VO_GetChnPTS(VO_LAYER VoLayer, VO_CHN VoChn, HI_U64 *pu64ChnPTS);
HI_S32 HI_MPI_VO_QueryChnStatus(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);

HI_S32 HI_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VO_ClearChnBuf(VO_LAYER VoLayer, VO_CHN VoChn, HI_BOOL bClrAll);

HI_S32 HI_MPI_VO_SetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, const VO_BORDER_S *pstBorder);
HI_S32 HI_MPI_VO_GetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, VO_BORDER_S *pstBorder);

HI_S32 HI_MPI_VO_SetChnBoundary(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_BOUNDARY_S *pstChnBoundary);
HI_S32 HI_MPI_VO_GetChnBoundary(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_BOUNDARY_S *pstChnBoundary);

HI_S32 HI_MPI_VO_SetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, HI_U32 u32Threshold);
HI_S32 HI_MPI_VO_GetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, HI_U32 *pu32Threshold);

HI_S32 HI_MPI_VO_SetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E enRotation);
HI_S32 HI_MPI_VO_GetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E *penRotation);

HI_S32 HI_MPI_VO_GetChnRegionLuma(VO_LAYER VoLayer, VO_CHN VoChn, VO_REGION_INFO_S *pstRegionInfo,
                                  HI_U64 *pu64LumaData, HI_S32 s32MilliSec);

/* WBC(Write Back Control) Relative Settings */
HI_S32 HI_MPI_VO_SetWBCSource(VO_WBC VoWBC, const VO_WBC_SOURCE_S *pstWBCSource);
HI_S32 HI_MPI_VO_GetWBCSource(VO_WBC VoWBC, VO_WBC_SOURCE_S *pstWBCSources);

HI_S32 HI_MPI_VO_SetWBCAttr(VO_WBC VoWBC, const VO_WBC_ATTR_S *pstWBCAttr);
HI_S32 HI_MPI_VO_GetWBCAttr(VO_WBC VoWBC, VO_WBC_ATTR_S *pstWBCAttr);

HI_S32 HI_MPI_VO_EnableWBC(VO_WBC VoWBC);
HI_S32 HI_MPI_VO_DisableWBC(VO_WBC VoWBC);

HI_S32 HI_MPI_VO_SetWBCMode(VO_WBC VoWBC, VO_WBC_MODE_E enWBCMode);
HI_S32 HI_MPI_VO_GetWBCMode(VO_WBC VoWBC, VO_WBC_MODE_E *penWBCMode);

HI_S32 HI_MPI_VO_SetWBCDepth(VO_WBC VoWBC, HI_U32 u32Depth);
HI_S32 HI_MPI_VO_GetWBCDepth(VO_WBC VoWBC, HI_U32 *pu32Depth);

HI_S32 HI_MPI_VO_GetWBCFrame(VO_WBC VoWBC, VIDEO_FRAME_INFO_S *pstVFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VO_ReleaseWBCFrame(VO_WBC VoWBC, const VIDEO_FRAME_INFO_S *pstVFrame);

/* Graphic Relative Settings */
HI_S32 HI_MPI_VO_BindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);
HI_S32 HI_MPI_VO_UnBindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, const VO_CSC_S *pstCSC);
HI_S32 HI_MPI_VO_GetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, VO_CSC_S *pstCSC);

HI_S32 HI_MPI_VO_SetDevFrameRate(VO_DEV VoDev, HI_U32 u32FrameRate);
HI_S32 HI_MPI_VO_GetDevFrameRate(VO_DEV VoDev, HI_U32 *pu32FrameRate);

/* Module Parameter Settings */
HI_S32 HI_MPI_VO_SetModParam(const VO_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VO_GetModParam(VO_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VO_SetVtth(VO_DEV VoDev, HI_U32 u32Vtth);
HI_S32 HI_MPI_VO_GetVtth(VO_DEV VoDev, HI_U32 *pu32Vtth);
HI_S32 HI_MPI_VO_SetVtth2(VO_DEV VoDev, HI_U32 u32Vtth);
HI_S32 HI_MPI_VO_GetVtth2(VO_DEV VoDev, HI_U32 *pu32Vtth);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VO_H__ */


