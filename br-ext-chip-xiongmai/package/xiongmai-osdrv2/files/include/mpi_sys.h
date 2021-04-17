/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_sys.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/
#ifndef __MPI_SYS_H__
#define __MPI_SYS_H__

/******************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "xm_common.h"
#include "xm_comm_sys.h"


XM_S32 XM_MPI_SYS_Init(XM_VOID); 
/*
** u64Base is the global PTS of the system.
** ADVICE:
** 1. Bester to call XM_MPI_SYS_GetCurPts on the host board to get the u64Base.
** 2. When the linux start up, call XM_MPI_SYS_InitPtsBase to set the init pts.
** 3. When media bussines is running, synchronize the PTS one time per minute.
**    And should call XM_MPI_SYS_SyncPts.
*/

/* alloc mmz memory in user context                                         */
XM_S32 XM_MPI_SYS_MmzAlloc(XM_U32 *pu32PhyAddr, XM_VOID **ppVirtAddr, 
        const XM_CHAR *strMmb, const XM_CHAR *strZone, XM_U32 u32Len);

XM_S32 XM_MPI_SYS_MmzAlloc_Cached(XM_U32 *pu32PhyAddr, void **ppVitAddr,
		const XM_CHAR *pstrMmb, const XM_CHAR *pstrZone, XM_U32 u32Len);

/* free mmz memory in user context                                          */
XM_S32 XM_MPI_SYS_MmzFree(XM_U32 u32PhyAddr, XM_VOID *pVirtAddr);
XM_S32 XM_MPI_SYS_MmzFlushCache(XM_U32 u32PhyAddr, XM_VOID *pVirAddr,
		XM_U32 u32Size);

/* fulsh cache */
XM_S32 XM_MPI_SYS_MmzFlushCache(XM_U32 u32PhyAddr, XM_VOID *pVirAddr,
		XM_U32 u32Size);
/*
** Call the mmap function to map physical address to virtual address
** The system function mmap is too complicated, so we packge it.
*/
XM_VOID * XM_MPI_SYS_Mmap(XM_U32 u32PhyAddr, XM_U32 u32Size);
XM_S32 XM_MPI_SYS_Munmap(XM_VOID* pVirAddr, XM_U32 u32Size);

XM_S32 XM_MPI_SYS_MmzReset(void);
XM_S32 XM_MPI_SYS_Bind(MPP_CHN_S *pstSrcChn, MPP_CHN_S *pstDestChn);
XM_S32 XM_MPI_SYS_UnBind(MPP_CHN_S *pstSrcChn, MPP_CHN_S *pstDestChn);

/******************************************/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
/******************************************/
#endif /*__MPI_SYS_H__ */


