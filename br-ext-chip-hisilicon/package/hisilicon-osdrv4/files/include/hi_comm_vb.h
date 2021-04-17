/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/

#ifndef __HI_COMM_VB_H__
#define __HI_COMM_VB_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VB_INVALID_POOLID              (-1U)
#define VB_INVALID_HANDLE              (-1U)

#define VB_MAX_COMM_POOLS              16
#define VB_MAX_MOD_COMM_POOLS          16

/* user ID for VB */
#define VB_MAX_USER                    VB_UID_BUTT

typedef enum hiVB_UID_E {
    VB_UID_VI = 0,
    VB_UID_VO = 1,
    VB_UID_VGS = 2,
    VB_UID_VENC = 3,
    VB_UID_VDEC = 4,
    VB_UID_H265E = 5,
    VB_UID_H264E = 6,
    VB_UID_JPEGE = 7,
    VB_UID_H264D = 8,
    VB_UID_JPEGD = 9,
    VB_UID_VPSS = 10,
    VB_UID_DIS = 11,
    VB_UID_USER = 12,
    VB_UID_PCIV = 13,
    VB_UID_AI = 14,
    VB_UID_AENC = 15,
    VB_UID_RC = 16,
    VB_UID_VFMW = 17,
    VB_UID_GDC = 18,
    VB_UID_AVS = 19,
    VB_UID_DPU_RECT = 20,
    VB_UID_DPU_MATCH = 21,
    VB_UID_MCF = 22,
    VB_UID_BUTT = 23,
} VB_UID_E;

/* Generall common pool use this owner id, module common pool use VB_UID as owner id */
#define POOL_OWNER_COMMON              -1

/* Private pool use this owner id */
#define POOL_OWNER_PRIVATE             -2

typedef HI_U32 VB_POOL;
typedef HI_U32 VB_BLK;

#define RESERVE_MMZ_NAME               "window"

typedef enum hiVB_REMAP_MODE_E {
    VB_REMAP_MODE_NONE = 0, /* no remap */
    VB_REMAP_MODE_NOCACHE = 1, /* no cache remap */
    VB_REMAP_MODE_CACHED = 2, /* cache remap, if you use this mode, you should flush cache by yourself */
    VB_REMAP_MODE_BUTT
} VB_REMAP_MODE_E;

typedef struct hiVB_POOL_CONFIG_S {
    HI_U64 u64BlkSize;
    HI_U32 u32BlkCnt;
    VB_REMAP_MODE_E enRemapMode;
    HI_CHAR acMmzName[MAX_MMZ_NAME_LEN];
} VB_POOL_CONFIG_S;

typedef struct hiVB_CONFIG_S {
    HI_U32 u32MaxPoolCnt;
    VB_POOL_CONFIG_S astCommPool[VB_MAX_COMM_POOLS];
} VB_CONFIG_S;

typedef struct hiVB_POOL_STATUS_S {
    HI_U32 bIsCommPool;
    HI_U32 u32BlkCnt;
    HI_U32 u32FreeBlkCnt;
} VB_POOL_STATUS_S;

#define VB_SUPPLEMENT_JPEG_MASK        0x1
#define VB_SUPPLEMENT_ISPINFO_MASK     0x2
#define VB_SUPPLEMENT_MOTION_DATA_MASK 0x4
#define VB_SUPPLEMENT_DNG_MASK         0x8

typedef struct hiVB_SUPPLEMENT_CONFIG_S {
    HI_U32 u32SupplementConfig;
} VB_SUPPLEMENT_CONFIG_S;

#define HI_ERR_VB_NULL_PTR             HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VB_NOMEM                HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VB_NOBUF                HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VB_UNEXIST              HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VB_ILLEGAL_PARAM        HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VB_NOTREADY             HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VB_BUSY                 HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VB_NOT_PERM             HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VB_SIZE_NOT_ENOUGH      HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_SIZE_NOT_ENOUGH)

#define HI_ERR_VB_2MPOOLS              HI_DEF_ERR(HI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUTT + 1)

#define HI_TRACE_VB(level, fmt, ...)                                                                         \
    do {                                                                                                     \
        HI_TRACE(level, HI_ID_VB, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_VB_H_ */

