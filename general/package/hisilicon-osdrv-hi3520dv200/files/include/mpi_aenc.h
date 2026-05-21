/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : ai.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/6/15
  Description   : 
  History       :
  1.Date        : 2009/6/15
    Author      : p00123320
    Modification: Created file    
******************************************************************************/


#ifndef _MPI_AENC_H__
#define _MPI_AENC_H__

#include "hi_common.h"
#include "hi_comm_aio.h"
#include "hi_comm_aenc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define AENC_ADAPT_MAGIC 0Xfcfcfcfc

HI_S32 HI_MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_AENC_DestroyChn(AENC_CHN AeChn);

HI_S32 HI_MPI_AENC_SendFrame(AENC_CHN AeChn,
    const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);

HI_S32 HI_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream, HI_BOOL bBlock); 
HI_S32 HI_MPI_AENC_ReleaseStream(AENC_CHN AeChn, const AUDIO_STREAM_S *pstStream);

HI_S32 HI_MPI_AENC_GetFd(AENC_CHN AeChn);

HI_S32 HI_MPI_AENC_Save_File(AENC_CHN AeChn, AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);

HI_S32 HI_MPI_AENC_RegeisterEncoder(HI_S32 *ps32Handle, AENC_ENCODER_S *pstEncoder);
HI_S32 HI_MPI_AENC_UnRegisterEncoder(HI_S32 s32Handle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

