/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ao.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/


#ifndef _MPI_AO_H__
#define _MPI_AO_H__

#include "xm_type.h"
#include "xm_common.h"
#include "xm_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */


XM_S32 XM_MPI_AO_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
XM_S32 XM_MPI_AO_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);

XM_S32 XM_MPI_AO_Enable(AUDIO_DEV AudioDevId);
XM_S32 XM_MPI_AO_Disable(AUDIO_DEV AudioDevId);

XM_S32 XM_MPI_AO_GetFd(AUDIO_DEV AudioDevId);

XM_S32 XM_MPI_AO_SetVolume(AUDIO_DEV AudioDevId, XM_S32 s32VolumeDb);
XM_S32 XM_MPI_AO_GetVolume(AUDIO_DEV AudioDevId, XM_S32 *ps32VolumeDb);

XM_S32 XM_MPI_AO_SetMute(AUDIO_DEV AudioDevId, XM_BOOL bEnable, AUDIO_FADE_S *pstFade);
XM_S32 XM_MPI_AO_GetMute(AUDIO_DEV AudioDevId, XM_BOOL *pbEnable, AUDIO_FADE_S *pstFade);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

