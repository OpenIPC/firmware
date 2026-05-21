/******************************************************************************
*
*  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.
*
*******************************************************************************
*  File Name     : mpi_region.h
*  Version       : Initial Draft
*  Author        : j00169368
*  Created       : 2010/12/13
*  Last Modified :
*  Description   : REGION MPI
*  Function List :
*
*
*  History:
* 
*       1.  Date         : 2010/12/13
*           Author       : j00169368
*           Modification : Created file
*
******************************************************************************/

#ifndef __MPI_REGION_H__
#define __MPI_REGION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#include "hi_comm_region.h"


HI_S32 HI_MPI_RGN_Create(RGN_HANDLE Handle,const RGN_ATTR_S *pstRegion);
HI_S32 HI_MPI_RGN_Destroy(RGN_HANDLE Handle);

HI_S32 HI_MPI_RGN_GetAttr(RGN_HANDLE Handle,RGN_ATTR_S *pstRegion);
HI_S32 HI_MPI_RGN_SetAttr(RGN_HANDLE Handle,const RGN_ATTR_S *pstRegion);

HI_S32 HI_MPI_RGN_SetBitMap(RGN_HANDLE Handle,const BITMAP_S *pstBitmap);

HI_S32 HI_MPI_RGN_SetAttachField(RGN_HANDLE Handle, RGN_ATTACH_FIELD_E enAttachField);
HI_S32 HI_MPI_RGN_GetAttachField(RGN_HANDLE Handle, RGN_ATTACH_FIELD_E *penAttachField);

HI_S32 HI_MPI_RGN_AttachToChn(RGN_HANDLE Handle,const MPP_CHN_S *pstChn,const RGN_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_RGN_DetachFrmChn(RGN_HANDLE Handle,const MPP_CHN_S *pstChn);

HI_S32 HI_MPI_RGN_SetDisplayAttr(RGN_HANDLE Handle,const MPP_CHN_S *pstChn,const RGN_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_RGN_GetDisplayAttr(RGN_HANDLE Handle,const MPP_CHN_S *pstChn,RGN_CHN_ATTR_S *pstChnAttr);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __MPI_REGION_H__ */


