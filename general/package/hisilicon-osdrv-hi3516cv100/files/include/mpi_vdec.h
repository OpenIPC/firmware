/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vdec.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2007/01/18
  Description   : 
  History       :
  1.Date        : 2007/01/18
    Author      : z50825
    Modification: Created file
  2.Date        : 2008/1/8
    Author      : l59217
    Modification: Delete get wm attr interface
  3.Date        : 2008/3/4
    Author      : l59217
    Modification: modified HI_MPI_VDEC_GetCapability interface
  4.Date        : 2008/7/19
    Author      : c55300
    Modification: CR20080716007£¬HI_MPI_VDEC_ResetChn() is added.
******************************************************************************/
#ifndef  __MPI_VDEC_H__
#define  __MPI_VDEC_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


HI_S32 HI_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, HI_U32 u32BlockFlag);

HI_S32 HI_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);
HI_S32 HI_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);
HI_S32 HI_MPI_VDEC_Query(VDEC_CHN VdChn,VDEC_CHN_STAT_S *pstStat);

HI_S32 HI_MPI_VDEC_GetFd(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_ResetChn(VDEC_CHN VdChn);
HI_S32 HI_MPI_VDEC_QueryData(VDEC_CHN VdChn,HI_BOOL *pbIsData);

HI_S32 HI_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pstParam);
HI_S32 HI_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pstParam);

HI_S32 HI_MPI_VDEC_SetChnPrevMode(VDEC_CHN VdChn, const HI_BOOL bPrevMode);
HI_S32 HI_MPI_VDEC_GetChnPrevMode(VDEC_CHN VdChn, HI_BOOL *pbPrevMode);

//param about memory allocation and protocol type
HI_S32 HI_MPI_VDEC_SetPrtclParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pstParam);
HI_S32 HI_MPI_VDEC_GetPrtclParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pstParam);

HI_S32 HI_MPI_VDEC_SetDisplayFrameNum(VDEC_CHN VdChn, HI_S32  s32DisplayFrameNum);
HI_S32 HI_MPI_VDEC_GetDisplayFrameNum(VDEC_CHN VdChn, HI_S32 *ps32DisplayFrameNum);


// list of modified function
//separate userdata and image interface 
HI_S32 HI_MPI_VDEC_GetImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_U32 u32BlockFlag);
HI_S32 HI_MPI_VDEC_ReleaseImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo);
HI_S32 HI_MPI_VDEC_GetUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, HI_U32 u32BlockFlag);
HI_S32 HI_MPI_VDEC_ReleaseUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData);

HI_S32 HI_MPI_VDEC_SendStream_TimeOut(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, HI_U32 u32MilliSec);
HI_S32 HI_MPI_VDEC_GetImage_TimeOut(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo,HI_U32 u32MilliSec);
HI_S32 HI_MPI_VDEC_GetUserData_TimeOut(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, HI_U32 u32MilliSec);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __MPI_VDEC_H__ */

