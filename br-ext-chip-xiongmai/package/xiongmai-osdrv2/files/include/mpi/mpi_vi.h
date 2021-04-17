/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vi.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/

#ifndef __MPI_VI_H__
#define __MPI_VI_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "xm_comm_vi.h"
XM_S32 XM_MPI_VI_Init(void);
XM_S32 XM_MPI_VI_SetChnAttr(VI_CHN ViChn, const VI_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_VI_GetChnAttr(VI_CHN ViChn, VI_CHN_ATTR_S *pstAttr);

XM_S32 XM_MPI_VI_EnableChn(VI_CHN ViChn);
XM_S32 XM_MPI_VI_DisableChn(VI_CHN ViChn);

XM_S32 XM_MPI_VI_GetFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo);
XM_S32 XM_MPI_VI_ReleaseFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo);

XM_S32 XM_MPI_VI_SetExtChnAttr(VI_CHN ViChn, const VI_EXT_CHN_ATTR_S *pstExtChnAttr);
XM_S32 XM_MPI_VI_GetExtChnAttr(VI_CHN ViChn, VI_EXT_CHN_ATTR_S *pstExtChnAttr);

XM_S32 XM_MPI_VI_SetFrmRate(VI_CHN ViChn, XM_U32 srcFrmRate,XM_U32 dstFrmRate);

XM_S32 XM_MPI_VI_Bind(VI_CHN ViChn, MPP_CHN_S *pstDestChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VI_H__ */



