/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/06/28
  Description   : Vou API
  History       :
  1.Date        : 2011/06/28
    Author      : z00185248
    Modification: Created file

******************************************************************************/

#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "hi_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */


/* Device Settings */

HI_S32 HI_MPI_VO_Enable (VO_DEV VoDev);
HI_S32 HI_MPI_VO_Disable(VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
HI_S32 HI_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

HI_S32 HI_MPI_VO_CloseFd(HI_VOID);


/* Video Settings */
HI_S32 HI_MPI_VO_EnableVideoLayer (VO_DEV VoDev);
HI_S32 HI_MPI_VO_DisableVideoLayer(VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetVideoLayerAttr(VO_DEV VoDev, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 HI_MPI_VO_GetVideoLayerAttr(VO_DEV VoDev, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

/* PIP LAYER Setting */

HI_S32 HI_MPI_VO_PipLayerBindDev(VO_DEV VoTargetDev);
HI_S32 HI_MPI_VO_PipLayerUnBindDev(VO_DEV VoTargetDev);

HI_S32 HI_MPI_VO_SetPipLayerAttr(const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 HI_MPI_VO_GetPipLayerAttr(VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

HI_S32 HI_MPI_VO_EnablePipLayer (HI_VOID);
HI_S32 HI_MPI_VO_DisablePipLayer(HI_VOID);

/* General Operation of Channel */

HI_S32 HI_MPI_VO_EnableChn (VO_DEV VoDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_DisableChn(VO_DEV VoDev, VO_CHN VoChn);


/* These two interfaces only support for Hi3518 */
HI_S32 HI_MPI_VO_EnableSdTdeBypass (VO_DEV VoDev);
HI_S32 HI_MPI_VO_DisableSdTdeBypass(VO_DEV VoDev);


HI_S32 HI_MPI_VO_SetChnAttr(VO_DEV VoDev, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VO_GetChnAttr(VO_DEV VoDev, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VO_SetChnDisplayWindow(VO_DEV VoDev, VO_CHN VoChn, const RECT_S  *pstWindow);
HI_S32 HI_MPI_VO_GetChnDisplayWindow(VO_DEV VoDev, VO_CHN VoChn, RECT_S  *pstWindow);

HI_S32 HI_MPI_VO_SetChnDispPos(VO_DEV VoDev, VO_CHN VoChn, const POINT_S *pstDispPos);
HI_S32 HI_MPI_VO_GetChnDispPos(VO_DEV VoDev, VO_CHN VoChn, POINT_S *pstDispPos);

HI_S32 HI_MPI_VO_SetChnField(VO_DEV VoDev, VO_CHN VoChn, const VO_DISPLAY_FIELD_E enField);
HI_S32 HI_MPI_VO_GetChnField(VO_DEV VoDev, VO_CHN VoChn, VO_DISPLAY_FIELD_E *pField);

HI_S32 HI_MPI_VO_SetChnFrameRate(VO_DEV VoDev, VO_CHN VoChn, HI_S32 s32ChnFrmRate);
HI_S32 HI_MPI_VO_GetChnFrameRate(VO_DEV VoDev, VO_CHN VoChn, HI_S32 *ps32ChnFrmRate);

HI_S32 HI_MPI_VO_GetChnFrame    (VO_DEV VoDev, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame);
HI_S32 HI_MPI_VO_ReleaseChnFrame(VO_DEV VoDev, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame);

HI_S32 HI_MPI_VO_ChnPause (VO_DEV VoDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_ChnResume(VO_DEV VoDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_ChnStep  (VO_DEV VoDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_ChnRefresh(VO_DEV VoDev, VO_CHN VoChn);

HI_S32 HI_MPI_VO_ChnShow(VO_DEV VoDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_ChnHide(VO_DEV VoDev, VO_CHN VoChn);

HI_S32 HI_MPI_VO_SetZoomInWindow(VO_DEV VoDev, VO_CHN VoChn, const VO_ZOOM_ATTR_S *pstZoomAttr);
HI_S32 HI_MPI_VO_GetZoomInWindow(VO_DEV VoDev, VO_CHN VoChn, VO_ZOOM_ATTR_S *pstZoomAttr);

HI_S32 HI_MPI_VO_GetChnPts   (VO_DEV VoDev, VO_CHN VoChn, HI_U64 *pu64ChnPts);
HI_S32 HI_MPI_VO_QueryChnStat(VO_DEV VoDev, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);

HI_S32 HI_MPI_VO_SendFrame(VO_DEV VoDev, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame);
HI_S32 HI_MPI_VO_SendFrameTimeOut(VO_DEV VoDev, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, HI_U32 u32MilliSec);

HI_S32 HI_MPI_VO_ClearChnBuffer(VO_DEV VoDev, VO_CHN VoChn, HI_BOOL bClrAll);


HI_S32 HI_MPI_VO_SetChnDispThreshold(VO_DEV VoDev, VO_CHN VoChn, HI_U32 u32Threshold);
HI_S32 HI_MPI_VO_GetChnDispThreshold(VO_DEV VoDev, VO_CHN VoChn, HI_U32 *pu32Threshold);

HI_S32 HI_MPI_VO_EnableChnDoubleFrame(VO_DEV VoDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_DisableChnDoubleFrame(VO_DEV VoDev, VO_CHN VoChn);

HI_S32 HI_MPI_VO_SetAttrBegin(VO_DEV VoDev);
HI_S32 HI_MPI_VO_SetAttrEnd  (VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetPlayToleration(VO_DEV VoDev, HI_U32 u32Toleration);
HI_S32 HI_MPI_VO_GetPlayToleration(VO_DEV VoDev, HI_U32 *pu32Toleration);

HI_S32 HI_MPI_VO_GetScreenFrame    (VO_DEV VoDev, VIDEO_FRAME_INFO_S *pstVFrame);
HI_S32 HI_MPI_VO_ReleaseScreenFrame(VO_DEV VoDev, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 HI_MPI_VO_SetDispBufLen(VO_DEV VoDev, HI_U32 u32BufLen);
HI_S32 HI_MPI_VO_GetDispBufLen(VO_DEV VoDev, HI_U32 *pu32BufLen);

HI_S32 HI_MPI_VO_EnableWbc(VO_DEV VoDev);
HI_S32 HI_MPI_VO_DisableWbc(VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetWbcAttr(VO_DEV VoDev, const VO_WBC_ATTR_S *pstWbcAttr);
HI_S32 HI_MPI_VO_GetWbcAttr(VO_DEV VoDev, VO_WBC_ATTR_S *pstWbcAttr);

HI_S32 HI_MPI_VO_SetWbcMode(VO_DEV VoDev, VO_WBC_MODE_E enWbcMode);
HI_S32 HI_MPI_VO_GetWbcMode(VO_DEV VoDev, VO_WBC_MODE_E *penWbcMode);

HI_S32 HI_MPI_VO_SetWbcDepth(VO_DEV VoDev, HI_U32 u32Depth);
HI_S32 HI_MPI_VO_GetWbcDepth(VO_DEV VoDev, HI_U32 *pu32Depth);
HI_S32 HI_MPI_VO_WbcGetScreenFrame(VO_DEV VoDev, VIDEO_FRAME_INFO_S *pstVFrame);
HI_S32 HI_MPI_VO_WbcReleaseScreenFrame(VO_DEV VoDev, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 HI_MPI_VO_GfxLayerBindDev(VOU_GFX_BIND_LAYER_E enGfxLayer, VO_DEV VoTargetDev);
HI_S32 HI_MPI_VO_GfxLayerUnBindDev(VOU_GFX_BIND_LAYER_E enGfxLayer, VO_DEV VoTargetDev);

HI_S32 HI_MPI_VO_SetGfxLayerCSC(HI_U32 u32Layer, const VO_CSC_S *pstCSC);
HI_S32 HI_MPI_VO_GetGfxLayerCSC(HI_U32 u32Layer, VO_CSC_S *pstCSC);

HI_S32 HI_MPI_VO_SetCascadeAttr(const VO_CAS_ATTR_S *pstCasAttr);
HI_S32 HI_MPI_VO_GetCascadeAttr(VO_CAS_ATTR_S *pstCasAttr);

HI_S32 HI_MPI_VO_EnableCascadeDev (VO_DEV VoCasDev);
HI_S32 HI_MPI_VO_DisableCascadeDev(VO_DEV VoCasDev);

HI_S32 HI_MPI_VO_SetCascadePattern(VO_DEV VoCasDev, HI_U32 u32Pattern);
HI_S32 HI_MPI_VO_GetCascadePattern(VO_DEV VoCasDev, HI_U32 *pu32Pattern);

HI_S32 HI_MPI_VO_CascadePosBindChn  (HI_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_CascadePosUnBindChn(HI_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);

HI_S32 HI_MPI_VO_EnableCascade (HI_VOID);
HI_S32 HI_MPI_VO_DisableCascade(HI_VOID);

HI_S32 HI_MPI_VO_GetDevCSC(VO_DEV VoDev, VO_CSC_S *pstDevCSC);
HI_S32 HI_MPI_VO_SetDevCSC(VO_DEV VoDev, VO_CSC_S *pstDevCSC);

HI_S32 HI_MPI_VO_GetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);
HI_S32 HI_MPI_VO_SetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);

HI_S32 HI_MPI_VO_SetVtth(VO_DEV VoDev, HI_U32 u32Vtth);
HI_S32 HI_MPI_VO_GetVtth(VO_DEV VoDev, HI_U32 *pu32Vtth);

HI_S32 HI_MPI_VO_SetDevFramerate(VO_DEV VoDev, HI_U32 u32Framerate);
HI_S32 HI_MPI_VO_GetDevFramerate(VO_DEV VoDev, HI_U32 *pu32Framerate);


HI_S32 HI_MPI_VO_EnableRecvFrameRateMatch (VO_DEV VoDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_DisableRecvFrameRateMatch (VO_DEV VoDev, VO_CHN VoChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /*__MPI_VO_H__ */


