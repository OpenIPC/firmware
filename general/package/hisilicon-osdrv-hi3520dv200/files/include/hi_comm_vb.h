/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_common.h
Version       : Initial Draft
Author        : Hi3511 MPP Team
Created       : 2006/11/09
Last Modified :
Description   : The common data type defination for VB module.
Function List :
History       :
 1.Date        : 2006/11/03
   Author      : c42025
   Modification: Created file

 2.Date        : 2007/11/30
   Author      : c42025
   Modification: modify according review comments

 3.Date        : 2008/06/18
   Author      : c42025
   Modification: add VB_UID_PCIV
  
 4.Date                :   2008/10/31
   Author              :   z44949
   Modification        :   Translate the chinese comment   
   
 5.Date                :   2008/10/31
   Author              :   p00123320
   Modification        :   change commentary of u32MaxPoolCnt in VB_CONF_S
******************************************************************************/
#ifndef __HI_COMM_VB_H__
#define __HI_COMM_VB_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

 
#define VB_MAX_POOLS 256
#define VB_MAX_COMM_POOLS 16



/* user ID for VB */
#define VB_MAX_USER   19

#define VB_UID_VIU     0 
#define VB_UID_VOU     1
#define VB_UID_DSU     2
#define VB_UID_VENC    3
#define VB_UID_VDEC    4
#define VB_UID_VDA      5
#define VB_UID_H264E   6
#define VB_UID_JPEGE   7
#define VB_UID_MPEG4E  8 
#define VB_UID_H264D   9 
#define VB_UID_JPEGD   10   
#define VB_UID_MPEG4D  11 
#define VB_UID_VPSS    12 
#define VB_UID_GRP     13
#define VB_UID_MPI     14 
#define VB_UID_PCIV    15
#define VB_UID_AI      16
#define VB_UID_AENC    17 
#define VB_UID_RC      18

#define VB_INVALID_POOLID (-1UL)
#define VB_INVALID_HANDLE (-1UL)

typedef HI_U32 VB_POOL;
typedef HI_U32 VB_BLK;

#define RESERVE_MMZ_NAME "window"


typedef struct hiVB_CONF_S
{
    HI_U32 u32MaxPoolCnt;     /* max count of pools, (0,VB_MAX_POOLS]  */    
    struct hiVB_CPOOL_S
    {
        HI_U32 u32BlkSize;
        HI_U32 u32BlkCnt;
        HI_CHAR acMmzName[MAX_MMZ_NAME_LEN];
    }astCommPool[VB_MAX_COMM_POOLS];
} VB_CONF_S;


typedef struct hiVB_POOL_STATUS_S
{
	HI_U32 bIsCommPool;
	HI_U32 u32BlkCnt;
	HI_U32 u32FreeBlkCnt;
}VB_POOL_STATUS_S;

#define HI_ERR_VB_NULL_PTR  HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VB_NOMEM     HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VB_NOBUF     HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VB_UNEXIST   HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VB_ILLEGAL_PARAM HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VB_NOTREADY  HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VB_BUSY      HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VB_NOT_PERM  HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define HI_ERR_VB_2MPOOLS HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUTT + 1)

#define HI_TRACE_VB(level,fmt...) HI_TRACE(level, HI_ID_VB,##fmt)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HI_COMm_VB_H_ */

