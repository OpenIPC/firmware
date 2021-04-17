/******************************************************************************

  Copyright (C), 2018-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vdec.h
  Version       : Initial Draft
  Author        : Xm MPP Team
  Created       : 2018/3/20
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "xm_type.h"
#include "xm_common.h"
#include "xm_comm_vdec.h"
#include "xm_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

XM_S32 XM_MPI_VDEC_GetFd(VDEC_CHN VdChn);
XM_S32 XM_MPI_VDEC_ResetChn(VDEC_CHN VdChn);
XM_S32 XM_MPI_VDEC_CreateChn(VDEC_CHN VdChn,VDEC_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);
XM_S32 XM_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);
XM_S32 XM_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);
XM_S32 XM_MPI_VDEC_Query(VDEC_CHN VdChn,VDEC_CHN_STAT_S *pstStat);
XM_S32 XM_MPI_VDEC_QueryData(VDEC_CHN VdChn,XM_U32 *pData);
XM_S32 XM_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn,VDEC_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_VDEC_SendStream(VDEC_CHN VdChn,VDEC_STREAM_S *pstStream,XM_S32 u32BlockFLag);
XM_S32 XM_MPI_VDEC_SendStream_TimeOut(VDEC_CHN VdChn,VDEC_STREAM_S *pstStream,XM_U32 u32MilliSec);
XM_S32 XM_MPI_VDEC_GetUserData(VDEC_CHN VdChn,VDEC_USERDATA_S *pstData,XM_S32 s32MilliSec);
XM_S32 XM_MPI_VDEC_GetUserData_TimeOut(VDEC_CHN VdChn, VDEC_USERDATA_S *pstData, XM_U32 u32MilliSec);
XM_S32 XM_MPI_VDEC_ReleaseUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstData);
XM_S32 XM_MPI_VDEC_GetImage(VDEC_CHN VdChn,VIDEO_FRAME_S *pstFrameInfo,XM_S32 s32MilliSec);
XM_S32 XM_MPI_VDEC_GetImage_TimeOut(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo,XM_U32 u32MilliSec);
XM_S32 XM_MPI_VDEC_ReleaseImage(VDEC_CHN VdChn, VIDEO_FRAME_S *pstFrameInfo);
XM_S32 XM_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S *pstChnParam);
XM_S32 XM_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S *pstChnParam);
XM_S32 XM_MPI_VDEC_Bind(VDEC_CHN VdChn, MPP_CHN_S *pstDestChn);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
