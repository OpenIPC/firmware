/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_region.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/

#ifndef __MPI_REGION_H__
#define __MPI_REGION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#include "xm_comm_region.h"
XM_S32 XM_MPI_RGN_Init(void);

XM_S32 XM_MPI_RGN_SetPallet(VI_CHN ViChn, const RGN_PALLET_S *pstPallet);
XM_S32 XM_MPI_RGN_Create(VI_CHN ViChn, const RGN_ATTR_S *pstRegion);
XM_S32 XM_MPI_RGN_Destroy(VI_CHN ViChn, const RGN_ATTR_S *pstRegion);

XM_S32 XM_MPI_RGN_GetAttr(VI_CHN ViChn,RGN_ATTR_S *pstRegion);
XM_S32 XM_MPI_RGN_SetAttr(VI_CHN ViChn, const RGN_ATTR_S *pstRegion);

XM_S32 XM_MPI_RGN_SetBitMap(VI_CHN ViChn, const BITMAP_S *pstBitmap);

XM_S32 XM_MPI_RGN_Enable(VI_CHN ViChn, XM_U32 Handle, RGN_TYPE_E enType);
XM_S32 XM_MPI_RGN_Disable(VI_CHN ViChn, XM_U32 Handle, RGN_TYPE_E enType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __MPI_REGION_H__ */


