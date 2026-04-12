/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ao.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/5/5
  Description   : 
  History       :
  1.Date        : 2009/5/5
    Author      : p00123320
    Modification: Created file 
******************************************************************************/


#ifndef _MPI_AO_H__
#define _MPI_AO_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */


HI_S32 HI_MPI_AO_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
HI_S32 HI_MPI_AO_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);

HI_S32 HI_MPI_AO_Enable(AUDIO_DEV AudioDevId);
HI_S32 HI_MPI_AO_Disable(AUDIO_DEV AudioDevId);

HI_S32 HI_MPI_AO_EnableChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);
HI_S32 HI_MPI_AO_DisableChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);

HI_S32 HI_MPI_AO_SendFrame(AUDIO_DEV AudioDevId, AO_CHN AoChn, 
        const AUDIO_FRAME_S *pstData, HI_BOOL bBlock);

HI_S32 HI_MPI_AO_EnableReSmp(AUDIO_DEV AudioDevId, AO_CHN AoChn, AUDIO_RESAMPLE_ATTR_S *pstAttr);
HI_S32 HI_MPI_AO_DisableReSmp(AUDIO_DEV AudioDevId, AO_CHN AoChn);

HI_S32 HI_MPI_AO_EnableReSmpEx(AUDIO_DEV AudioDevId, AO_CHN AoChn, AUDIO_RESAMPLE_ATTR_EX_S *pstAttr);
HI_S32 HI_MPI_AO_DisableReSmpEx(AUDIO_DEV AudioDevId, AO_CHN AoChn);

HI_S32 HI_MPI_AO_ClearChnBuf(AUDIO_DEV AudioDevId ,AO_CHN AoChn);
HI_S32 HI_MPI_AO_QueryChnStat(AUDIO_DEV AudioDevId ,AO_CHN AoChn, AO_CHN_STATE_S *pstStatus);

HI_S32 HI_MPI_AO_PauseChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);
HI_S32 HI_MPI_AO_ResumeChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);

HI_S32 HI_MPI_AO_SetVolume(AUDIO_DEV AudioDevId, AO_CHN AoChn, HI_S32 s32VolumeDb);
HI_S32 HI_MPI_AO_GetVolume(AUDIO_DEV AudioDevId, AO_CHN AoChn, HI_S32 *ps32VolumeDb);

HI_S32 HI_MPI_AO_GetFd(AUDIO_DEV AudioDevId, AO_CHN AoChn);

HI_S32 HI_MPI_AO_ClrPubAttr(AUDIO_DEV AudioDevId);

HI_S32 HI_MPI_AO_SetVqe3Attr(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_VQE3_CONFIG_S *pstVqeConfig);
HI_S32 HI_MPI_AO_GetVqe3Attr(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_VQE3_CONFIG_S *pstVqeConfig);
HI_S32 HI_MPI_AO_EnableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);
HI_S32 HI_MPI_AO_DisableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

