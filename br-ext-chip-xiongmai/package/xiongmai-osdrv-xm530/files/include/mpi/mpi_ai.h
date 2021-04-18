/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ai.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/


#ifndef _MPI_AI_H__
#define _MPI_AI_H__

#include "xm_type.h"
#include "xm_common.h"
#include "xm_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define FRAME_SIZE 160

XM_S32 XM_MPI_AI_SetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, AI_VQE_CONFIG_S *pstVqeConfig);
XM_S32 XM_MPI_AI_GetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_VQE_CONFIG_S *pstVqeConfig);

XM_S32 XM_MPI_AI_SetVolume(AUDIO_DEV AudioDevId, XM_S32 s32VolumeDb);
XM_S32 XM_MPI_AI_GetVolume(AUDIO_DEV AudioDevId, XM_S32 *ps32VolumeDb);

XM_S32 XM_MPI_AUDIO_Init(void);
XM_S32 XM_MPI_AI_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
XM_S32 XM_MPI_AI_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);

XM_S32 XM_MPI_AI_Enable(AUDIO_DEV AudioDevId);
XM_S32 XM_MPI_AI_Disable(AUDIO_DEV AudioDevId);

XM_S32 XM_MPI_AI_GetFrame(AUDIO_DEV AudioDevId, AI_CHN AiChn, 
    AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, XM_BOOL bBlock);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

