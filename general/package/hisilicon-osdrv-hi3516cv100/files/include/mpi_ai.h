/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sio.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/5/5
  Description   : 
  History       :
  1.Date        : 2009/5/5
    Author      : p00123320
    Modification: Created file 
******************************************************************************/


#ifndef _MPI_AI_H__
#define _MPI_AI_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_AI_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
HI_S32 HI_MPI_AI_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);

HI_S32 HI_MPI_AI_Enable(AUDIO_DEV AudioDevId);
HI_S32 HI_MPI_AI_Disable(AUDIO_DEV AudioDevId);

HI_S32 HI_MPI_AI_EnableChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);
HI_S32 HI_MPI_AI_DisableChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);

HI_S32 HI_MPI_AI_GetFrame(AUDIO_DEV AudioDevId, AI_CHN AiChn, 
    AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, HI_BOOL bBlock);
HI_S32 HI_MPI_AI_ReleaseFrame(AUDIO_DEV AudioDevId, AI_CHN AiChn,
    AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm);
HI_S32 HI_MPI_AI_SetChnParam(AUDIO_DEV AudioDevId, AI_CHN AiChn, AI_CHN_PARAM_S *pstChnParam);
HI_S32 HI_MPI_AI_GetChnParam(AUDIO_DEV AudioDevId, AI_CHN AiChn, AI_CHN_PARAM_S *pstChnParam);

HI_S32 HI_MPI_AI_EnableAec(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn);
HI_S32 HI_MPI_AI_DisableAec(AUDIO_DEV AiDevId, AI_CHN AiChn);

HI_S32 HI_MPI_AI_EnableReSmp(AUDIO_DEV AudioDevId, AI_CHN AiChn, AUDIO_RESAMPLE_ATTR_S *pstAttr);
HI_S32 HI_MPI_AI_DisableReSmp(AUDIO_DEV AudioDevId, AI_CHN AiChn);

HI_S32 HI_MPI_AI_EnableAnr(AUDIO_DEV AudioDevId, AI_CHN AiChn);
HI_S32 HI_MPI_AI_DisableAnr(AUDIO_DEV AudioDevId, AI_CHN AiChn);

HI_S32 HI_MPI_AI_GetFd(AUDIO_DEV AudioDevId, AI_CHN AiChn);

HI_S32 HI_MPI_AI_SetHpfAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_HPF_ATTR_S *pstHpfAttr);
HI_S32 HI_MPI_AI_GetHpfAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_HPF_ATTR_S *pstHpfAttr);

HI_S32 HI_MPI_AI_SetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, AI_VQE_CONFIG_S *pstVqeConfig);
HI_S32 HI_MPI_AI_GetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_VQE_CONFIG_S *pstVqeConfig);
HI_S32 HI_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
HI_S32 HI_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

HI_S32 HI_MPI_AI_EnableReSmpEx(AUDIO_DEV AudioDevId, AI_CHN AiChn, AUDIO_RESAMPLE_ATTR_EX_S *pstAttr);
HI_S32 HI_MPI_AI_DisableReSmpEx(AUDIO_DEV AudioDevId, AI_CHN AiChn);

HI_S32 HI_MPI_AI_Save_File(AUDIO_DEV AudioDevId, AI_CHN AiChn,AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);

HI_S32 HI_MPI_AI_ClrPubAttr(AUDIO_DEV AudioDevId);

HI_S32 HI_MPI_AI_SetVqe3Attr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, AI_VQE3_CONFIG_S *pstVqeConfig);
HI_S32 HI_MPI_AI_GetVqe3Attr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_VQE3_CONFIG_S *pstVqeConfig);

HI_S32 HI_MPI_AI_SetVqe3Volume(AUDIO_DEV AudioDevId, AI_CHN AiChn, HI_S32 s32VolumeDb);
HI_S32 HI_MPI_AI_GetVqe3Volume(AUDIO_DEV AudioDevId, AI_CHN AiChn, HI_S32 *ps32VolumeDb);

HI_S32 HI_MPI_AI_EnableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn);
HI_S32 HI_MPI_AI_DisableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

