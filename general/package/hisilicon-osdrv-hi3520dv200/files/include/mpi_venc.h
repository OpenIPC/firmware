/******************************************************************************

  Copyright (C), 2001-2012, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_venc.h
  Version       : Initial Draft
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2006/11/22
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_venc.h"
#include "hi_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_VENC_CreateGroup(VENC_GRP VeGroup);
HI_S32 HI_MPI_VENC_DestroyGroup(VENC_GRP VeGroup);

HI_S32 HI_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VENC_DestroyChn(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_RegisterChn(VENC_GRP VeGroup, VENC_CHN VeChn );
HI_S32 HI_MPI_VENC_UnRegisterChn(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_StartRecvPic(VENC_CHN VeChn);
HI_S32 HI_MPI_VENC_StartRecvPicEx(VENC_CHN VeChn, VENC_RECV_PIC_PARAM_S *pstRecvParam);
HI_S32 HI_MPI_VENC_StopRecvPic(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_Query(VENC_CHN VeChn, VENC_CHN_STAT_S *pstStat);

HI_S32 HI_MPI_VENC_SetChnAttr( VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VENC_GetChnAttr( VENC_CHN VeChn, VENC_CHN_ATTR_S *pstAttr);

HI_S32 HI_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, HI_BOOL bBlockFlag);
HI_S32 HI_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);

HI_S32 HI_MPI_VENC_InsertUserData(VENC_CHN VeChn, HI_U8 *pu8Data, HI_U32 u32Len);

HI_S32 HI_MPI_VENC_SendFrame(VENC_GRP VeGroup, VIDEO_FRAME_INFO_S *pstFrame);

HI_S32 HI_MPI_VENC_SetMaxStreamCnt(VENC_CHN VeChn, HI_U32 u32MaxStrmCnt);
HI_S32 HI_MPI_VENC_GetMaxStreamCnt(VENC_CHN VeChn, HI_U32 *pu32MaxStrmCnt);

HI_S32 HI_MPI_VENC_RequestIDR(VENC_CHN VeChn);
HI_S32 HI_MPI_VENC_RequestIDRInst(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_GetFd(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_SetRoiCfg(VENC_CHN VeChn, VENC_ROI_CFG_S *pstVencRoiCfg);
HI_S32 HI_MPI_VENC_GetRoiCfg(VENC_CHN VeChn, HI_U32 u32Index, VENC_ROI_CFG_S *pstVencRoiCfg);

HI_S32 HI_MPI_VENC_SetH264SliceSplit(VENC_CHN VeChn, const VENC_PARAM_H264_SLICE_SPLIT_S *pstSliceSplit);
HI_S32 HI_MPI_VENC_GetH264SliceSplit(VENC_CHN VeChn, VENC_PARAM_H264_SLICE_SPLIT_S *pstSliceSplit);

HI_S32 HI_MPI_VENC_SetH264InterPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTER_PRED_S *pstH264InterPred);
HI_S32 HI_MPI_VENC_GetH264InterPred(VENC_CHN VeChn, VENC_PARAM_H264_INTER_PRED_S *pstH264InterPred);

HI_S32 HI_MPI_VENC_SetH264IntraPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTRA_PRED_S *pstH264IntraPred);
HI_S32 HI_MPI_VENC_GetH264IntraPred(VENC_CHN VeChn, VENC_PARAM_H264_INTRA_PRED_S *pstH264IntraPred);

HI_S32 HI_MPI_VENC_SetH264Trans(VENC_CHN VeChn, const VENC_PARAM_H264_TRANS_S *pstH264Trans);
HI_S32 HI_MPI_VENC_GetH264Trans(VENC_CHN VeChn, VENC_PARAM_H264_TRANS_S *pstH264Trans);

HI_S32 HI_MPI_VENC_SetH264Entropy(VENC_CHN VeChn, const VENC_PARAM_H264_ENTROPY_S *pstH264EntropyEnc);
HI_S32 HI_MPI_VENC_GetH264Entropy(VENC_CHN VeChn, VENC_PARAM_H264_ENTROPY_S *pstH264EntropyEnc);

HI_S32 HI_MPI_VENC_SetH264Poc(VENC_CHN VeChn, const VENC_PARAM_H264_POC_S *pstH264Poc);
HI_S32 HI_MPI_VENC_GetH264Poc(VENC_CHN VeChn, VENC_PARAM_H264_POC_S *pstH264Poc);

HI_S32 HI_MPI_VENC_SetH264Dblk(VENC_CHN VeChn, const VENC_PARAM_H264_DBLK_S *pstH264Dblk);
HI_S32 HI_MPI_VENC_GetH264Dblk(VENC_CHN VeChn, VENC_PARAM_H264_DBLK_S *pstH264Dblk);

HI_S32 HI_MPI_VENC_SetH264Vui(VENC_CHN VeChn, const VENC_PARAM_H264_VUI_S *pstH264Vui);
HI_S32 HI_MPI_VENC_GetH264Vui(VENC_CHN VeChn, VENC_PARAM_H264_VUI_S *pstH264Vui);

HI_S32 HI_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_PARAM_JPEG_S *pstJpegParam);
HI_S32 HI_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_PARAM_JPEG_S *pstJpegParam);

HI_S32 HI_MPI_VENC_SetMpeg4Param(VENC_CHN VeChn, const VENC_PARAM_MPEG4_S *pstMpeg4Param);
HI_S32 HI_MPI_VENC_GetMpeg4Param(VENC_CHN VeChn, VENC_PARAM_MPEG4_S *pstMpeg4Param);

HI_S32 HI_MPI_VENC_SetMjpegParam(VENC_CHN VeChn, const VENC_PARAM_MJPEG_S *pstMjpegParam);
HI_S32 HI_MPI_VENC_GetMjpegParam(VENC_CHN VeChn, VENC_PARAM_MJPEG_S *pstMjpegParam);

HI_S32 HI_MPI_VENC_SetGrpFrmRate(VENC_GRP VeGroup, const GROUP_FRAME_RATE_S *pstGrpFrmRate);
HI_S32 HI_MPI_VENC_GetGrpFrmRate(VENC_GRP VeGroup, GROUP_FRAME_RATE_S *pstGrpFrmRate);

HI_S32 HI_MPI_VENC_GetRcPara(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcPara);
HI_S32 HI_MPI_VENC_SetRcPara(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcPara);

HI_S32 HI_MPI_VENC_SetH264eRefMode(VENC_CHN VeChn, VENC_ATTR_H264_REF_MODE_E enRefMode);
HI_S32 HI_MPI_VENC_GetH264eRefMode(VENC_CHN VeChn, VENC_ATTR_H264_REF_MODE_E *penRefMode);

HI_S32 HI_MPI_VENC_SetH264eRefParam( VENC_CHN VeChn, VENC_ATTR_H264_REF_PARAM_S* pstRefParam );
HI_S32 HI_MPI_VENC_GetH264eRefParam( VENC_CHN VeChn, VENC_ATTR_H264_REF_PARAM_S* pstRefParam );

HI_S32 HI_MPI_VENC_EnableIDR( VENC_CHN VeChn, HI_BOOL bEnableIDR );

HI_S32 HI_MPI_VENC_SetGrpColor2Grey(VENC_GRP VeGroup, const GROUP_COLOR2GREY_S *pstGrpColor2Grey);
HI_S32 HI_MPI_VENC_GetGrpColor2Grey(VENC_GRP VeGroup, GROUP_COLOR2GREY_S *pstGrpColor2Grey);

HI_S32 HI_MPI_VENC_SetColor2GreyConf(const GROUP_COLOR2GREY_CONF_S *pstGrpColor2GreyConf);
HI_S32 HI_MPI_VENC_GetColor2GreyConf(GROUP_COLOR2GREY_CONF_S *pstGrpColor2GreyConf);

HI_S32 HI_MPI_VENC_SetGrpCrop(VENC_GRP VeGroup, const GROUP_CROP_CFG_S *pstGrpCropCfg);
HI_S32 HI_MPI_VENC_GetGrpCrop(VENC_GRP VeGroup, GROUP_CROP_CFG_S *pstGrpCropCfg);

HI_S32 HI_MPI_VENC_SetJpegSnapMode(VENC_CHN VeChn, VENC_JPEG_SNAP_MODE_E enJpegSnapMode);
HI_S32 HI_MPI_VENC_GetJpegSnapMode(VENC_CHN VeChn, VENC_JPEG_SNAP_MODE_E *penJpegSnapMode);

HI_S32 HI_MPI_VENC_SetRcPriority(VENC_CHN VeChn, VENC_RC_PRIORITY_E enRcPriority);
HI_S32 HI_MPI_VENC_GetRcPriority(VENC_CHN VeChn, VENC_RC_PRIORITY_E *penRcPriority);
    
HI_S32 HI_MPI_VENC_SetLostFrameStrategy(VENC_CHN VeChn, VENC_PARAM_LOSTFRM_S *pstLostFrmParam);
HI_S32 HI_MPI_VENC_GetLostFrameStrategy(VENC_CHN VeChn, VENC_PARAM_LOSTFRM_S *pstLostFrmParam);

HI_S32 HI_MPI_VENC_AttachVbPool(VENC_CHN VeChn, VB_POOL hVbPool);
HI_S32 HI_MPI_VENC_DetachVbPool(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_SetH264VideoSignal(VENC_CHN VeChn, const VENC_PARAM_H264_VIDEO_SIGNAL_S *pstH264VideoSignal);
HI_S32 HI_MPI_VENC_GetH264VideoSignal(VENC_CHN VeChn, VENC_PARAM_H264_VIDEO_SIGNAL_S *pstH264VideoSignal);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VENC_H__ */