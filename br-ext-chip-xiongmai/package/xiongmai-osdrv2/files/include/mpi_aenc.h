/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_aenc.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/
#ifndef _MPI_AENC_H__
#define _MPI_AENC_H__

#include "xm_common.h"
#include "xm_comm_aio.h"
#include "xm_comm_aenc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define AENC_ADAPT_MAGIC 0Xfcfcfcfc

XM_S32 XM_MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_AENC_DestroyChn(AENC_CHN AeChn);

XM_S32 XM_MPI_AENC_SendFrame(AENC_CHN AeChn,
    const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);

XM_S32 XM_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream, XM_BOOL bBlock); 
XM_S32 XM_MPI_AENC_ReleaseStream(AENC_CHN AeChn, const AUDIO_STREAM_S *pstStream);

XM_S32 XM_MPI_AENC_GetFd(AENC_CHN AeChn);
XM_S32 XM_MPI_AENC_ReleaseBuf(AENC_CHN AeChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

