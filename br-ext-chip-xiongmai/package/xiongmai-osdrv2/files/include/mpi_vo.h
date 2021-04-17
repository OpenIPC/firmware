#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "xm_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/* Device Settings */

XM_S32 XM_MPI_VO_Init(void);
XM_S32 XM_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
XM_S32 XM_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

XM_S32 XM_MPI_VO_Enable (VO_DEV VoDev);
XM_S32 XM_MPI_VO_Disable(VO_DEV VoDev);

/*XM_S32 XM_MPI_VO_CloseFd(XM_VOID);*/

/* General Operation of Channel */

XM_S32 XM_MPI_VO_EnableChn (VO_LAYER VoLayer, VO_CHN VoChn);
XM_S32 XM_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

XM_S32 XM_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
XM_S32 XM_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);


XM_S32 XM_MPI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame, XM_S32 s32MilliSec);
XM_S32 XM_MPI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstFrame);

/*XM_S32 XM_MPI_VO_PauseChn (VO_LAYER VoLayer, VO_CHN VoChn);*/
/*XM_S32 XM_MPI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);*/
/*XM_S32 XM_MPI_VO_StepChn(VO_LAYER VoLayer, VO_CHN VoChn);*/
/*XM_S32 XM_MPI_VO_RefreshChn( VO_LAYER VoLayer, VO_CHN VoChn);*/

/*XM_S32 XM_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);*/
/*XM_S32 XM_MPI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);*/

/*XM_S32 XM_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, XM_S32 s32MilliSec);*/

/*XM_S32 XM_MPI_VO_ClearChnBuffer(VO_LAYER VoLayer, VO_CHN VoChn, XM_BOOL bClrAll);*/

/*XM_S32 XM_MPI_VO_GetChnRegionLuma(VO_LAYER VoLayer, VO_CHN VoChn, VO_REGION_INFO_S *pstRegionInfo,*/
/*XM_U32 *pu32LumaData, XM_S32 s32MilliSec);*/

/* Cascade setting */

/*XM_S32 XM_MPI_VO_SetCascadeAttr(const VO_CAS_ATTR_S *pstCasAttr);*/
/*XM_S32 XM_MPI_VO_GetCascadeAttr(VO_CAS_ATTR_S *pstCasAttr);*/

/*XM_S32 XM_MPI_VO_EnableCascadeDev (VO_DEV VoCasDev);*/
/*XM_S32 XM_MPI_VO_DisableCascadeDev(VO_DEV VoCasDev);*/

/*XM_S32 XM_MPI_VO_SetCascadePattern(VO_DEV VoCasDev, XM_U32 u32Pattern);*/
/*XM_S32 XM_MPI_VO_GetCascadePattern(VO_DEV VoCasDev, XM_U32 *pu32Pattern);*/

/*XM_S32 XM_MPI_VO_CascadePosBindChn(XM_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);*/
/*XM_S32 XM_MPI_VO_CascadePosUnBindChn(XM_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);*/

/*XM_S32 XM_MPI_VO_EnableCascade (XM_VOID);*/
/*XM_S32 XM_MPI_VO_DisableCascade(XM_VOID);*/

/* VGA setting */

/*XM_S32 XM_MPI_VO_GetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);*/
/*XM_S32 XM_MPI_VO_SetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);*/

/*XM_S32 XM_MPI_VO_SetDevFrameRate(VO_DEV VoDev, XM_U32 u32FrameRate);*/
/*XM_S32 XM_MPI_VO_GetDevFrameRate(VO_DEV VoDev, XM_U32 *pu32FrameRate);*/

/*XM_S32 XM_MPI_VO_EnableRecvFrameRateMatch (VO_LAYER VoLayer, VO_CHN VoChn);*/
/*XM_S32 XM_MPI_VO_DisableRecvFrameRateMatch (VO_LAYER VoLayer, VO_CHN VoChn);*/

/* HDMI setting */
/*XM_S32 XM_MPI_VO_GetHdmiParam(VO_DEV VoDev, VO_HDMI_PARAM_S *pstHdmiParam);*/
/*XM_S32 XM_MPI_VO_SetHdmiParam(VO_DEV VoDev, VO_HDMI_PARAM_S *pstHdmiParam);*/

/*XM_S32 XM_MPI_VO_SetVtth(VO_DEV VoDev, XM_U32 u32Vtth);*/
/*XM_S32 XM_MPI_VO_GetVtth(VO_DEV VoDev, XM_U32* pu32Vtth);*/

XM_S32 XM_MPI_VO_SetImageLayerAttr(VO_LAYER VoLayer, const VO_IMAGE_LAYER_ATTR_S *pstLayerAttr);
XM_S32 XM_MPI_VO_GetImageLayerAttr(VO_LAYER VoLayer, const VO_IMAGE_LAYER_ATTR_S *pstLayerAttr);
XM_S32 XM_MPI_VO_EnableImageLayer(VO_LAYER VoLayer);
XM_S32 XM_MPI_VO_DisableImageLayer(VO_LAYER VoLayer);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VO_H__ */

