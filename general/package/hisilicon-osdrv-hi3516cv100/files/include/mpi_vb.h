/******************************************************************************
 
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
 
 ******************************************************************************
  File Name     : mpi_vb.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2007/10/15
  Description   : 
  History       :
  1.Date        : 2007/10/15
    Author      : c42025
    Modification: Created file
******************************************************************************/
#ifndef __MPI_VB_H__
#define __MPI_VB_H__

#include "hi_comm_vb.h"

/******************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */
/******************************************/


VB_POOL HI_MPI_VB_CreatePool(HI_U32 u32BlkSize,HI_U32 u32BlkCnt,const HI_CHAR *pcMmzName);

HI_S32 HI_MPI_VB_DestroyPool(VB_POOL Pool);
VB_BLK HI_MPI_VB_GetBlock(VB_POOL Pool, HI_U32 u32BlkSize,const HI_CHAR *pcMmzName);
HI_S32 HI_MPI_VB_ReleaseBlock(VB_BLK Block);

HI_U32 HI_MPI_VB_Handle2PhysAddr(VB_BLK Block);
VB_POOL HI_MPI_VB_Handle2PoolId(VB_BLK Block);

HI_S32 HI_MPI_VB_Init (HI_VOID);
HI_S32 HI_MPI_VB_Exit (HI_VOID);
HI_S32 HI_MPI_VB_SetConf (const VB_CONF_S *pstVbConf);
HI_S32 HI_MPI_VB_GetConf (VB_CONF_S *pstVbConf);

HI_S32 HI_MPI_VB_MmapPool(VB_POOL Pool);
HI_S32 HI_MPI_VB_MunmapPool(VB_POOL Pool);
HI_S32 HI_MPI_VB_GetBlkVirAddr(VB_POOL Pool, HI_U32 u32PhyAddr, HI_VOID **ppVirAddr);


/******************************************/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
/******************************************/
#endif /*__MPI_VI_H__ */
