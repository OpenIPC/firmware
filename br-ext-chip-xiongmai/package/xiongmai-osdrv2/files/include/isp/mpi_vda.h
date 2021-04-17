/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: mpi_vda.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/6/27

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/6/27
  Author		: Lycai
  Modification	: creat
******************************************************************************/

#ifndef __MPI_VDA_H__
#define __MPI_VDA_H__

#include "xm_comm_vda.h"

XM_S32 XM_MPI_VDA_CreateChn(VDA_CHN VdaChn, const VDA_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_VDA_DestroyChn(VDA_CHN VdaChn);

XM_S32 XM_MPI_VDA_GetChnAttr(VDA_CHN VdaChn, VDA_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_VDA_SetChnAttr(VDA_CHN VdaChn, const VDA_CHN_ATTR_S *pstAttr);

XM_S32 XM_MPI_VDA_StartRecvPic(VDA_CHN VdaChn);
XM_S32 XM_MPI_VDA_StopRecvPic(VDA_CHN VdaChn);

XM_S32 XM_MPI_VDA_GetData(VDA_CHN VdaChn, VDA_DATA_S *pstVdaData, XM_S32 s32MilliSec);
XM_S32 XM_MPI_VDA_ReleaseData(VDA_CHN VdaChn, const VDA_DATA_S* pstVdaData);


XM_S32 XM_MPI_VDA_Query(VDA_CHN VdaChn, VDA_CHN_STAT_S *pstChnStat);


XM_S32 XM_MPI_VDA_UpdateRef(VDA_CHN VdaChn, const VIDEO_FRAME_INFO_S *pstRefFrame);



#endif /* End of #ifndef __MPI_VDA_H__ */

