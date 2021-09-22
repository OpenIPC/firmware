/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_adec.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/

#ifndef _MPI_ADEC_H__
#define _MPI_ADEC_H__

#include "xm_common.h"
#include "xm_comm_aio.h"
#include "xm_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

XM_S32 XM_MPI_ADEC_CreateChn(ADEC_CHN AdChn, ADEC_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_ADEC_DestroyChn(ADEC_CHN AdChn);

XM_S32 XM_MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, XM_BOOL bBlock);
XM_S32 XM_MPI_ADEC_ReleaseBuf(ADEC_CHN AdChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

