/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_SYS_H_
#define _MI_SYS_H_

#include "mi_common.h"
#include "mi_sys_datatype.h"

#define SYS_MAJOR_VERSION 2
#define SYS_SUB_VERSION 18.1
#define MACRO_TO_STR(macro) #macro
#define SYS_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_sys_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_sys_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_sys_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_SYS_API_VERSION SYS_VERSION_STR(SYS_MAJOR_VERSION,SYS_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_SYS_Init(void);

MI_S32 MI_SYS_Exit (void);

MI_S32 MI_SYS_BindChnPort(MI_SYS_ChnPort_t *pstSrcChnPort, MI_SYS_ChnPort_t *pstDstChnPort , MI_U32 u32SrcFrmrate,  MI_U32 u32DstFrmrate);

MI_S32 MI_SYS_BindChnPort2(MI_SYS_ChnPort_t *pstSrcChnPort, MI_SYS_ChnPort_t *pstDstChnPort,MI_U32 u32SrcFrmrate,  MI_U32 u32DstFrmrate, MI_SYS_BindType_e eBindType, MI_U32 u32BindParam);

MI_S32 MI_SYS_UnBindChnPort(MI_SYS_ChnPort_t *pstSrcChnPort, MI_SYS_ChnPort_t *pstDstChnPort);

MI_S32 MI_SYS_GetBindbyDest (MI_SYS_ChnPort_t *pstDstChnPort, MI_SYS_ChnPort_t *pstSrcChnPort);

MI_S32 MI_SYS_GetVersion (MI_SYS_Version_t *pstVersion);

MI_S32 MI_SYS_GetCurPts (MI_U64 *pu64Pts);

MI_S32 MI_SYS_InitPtsBase (MI_U64 u64PtsBase);

MI_S32 MI_SYS_SyncPts (MI_U64 u64Pts);

MI_S32 MI_SYS_SetReg (MI_U32 u32RegAddr, MI_U16 u16Value, MI_U16 u16Mask);

MI_S32 MI_SYS_GetReg (MI_U32 u32RegAddr, MI_U16 *pu16Value);

MI_S32 MI_SYS_ReadUuid (MI_U64 *u64Uuid);

MI_S32 MI_SYS_SetChnMMAConf (MI_ModuleId_e eModId, MI_U32 u32DevId, MI_U32 u32ChnId, MI_U8 *pu8MMAHeapName);

MI_S32 MI_SYS_GetChnMMAConf (MI_ModuleId_e eModId,  MI_U32 u32DevId, MI_U32 u32ChnId, void  *data, MI_U32 u32Length);

MI_S32 MI_SYS_ChnInputPortGetBuf (MI_SYS_ChnPort_t *pstChnPort ,MI_SYS_BufConf_t *pstBufConf, MI_SYS_BufInfo_t *pstBufInfo , MI_SYS_BUF_HANDLE *bufHandle , MI_S32 s32TimeOutMs);

MI_S32 MI_SYS_ChnInputPortPutBuf (MI_SYS_BUF_HANDLE bufHandle  ,MI_SYS_BufInfo_t *pstBufInfo , MI_BOOL bDropBuf);

MI_S32 MI_SYS_ChnOutputPortGetBuf (MI_SYS_ChnPort_t *pstChnPort,MI_SYS_BufInfo_t *pstBufInfo, MI_SYS_BUF_HANDLE *bufHandle);

MI_S32 MI_SYS_ChnOutputPortPutBuf (MI_SYS_BUF_HANDLE hBufHandle);

MI_S32 MI_SYS_SetChnOutputPortDepth(MI_SYS_ChnPort_t *pstChnPort , MI_U32 u32UserFrameDepth , MI_U32 u32BufQueueDepth);

MI_S32 MI_SYS_ChnPortInjectBuf (MI_SYS_BUF_HANDLE handle ,MI_SYS_ChnPort_t *pstChnInputPort);

MI_S32 MI_SYS_GetFd(MI_SYS_ChnPort_t *pstChnPort , MI_S32 *s32Fd);

MI_S32 MI_SYS_CloseFd(MI_S32 s32ChnPortFd);

MI_S32 MI_SYS_ConfDevPubPools(MI_ModuleId_e  eModule, MI_U32 u32DevId, MI_VB_PoolListConf_t  stPoolListConf);

MI_S32 MI_SYS_ReleaseDevPubPools(MI_ModuleId_e  eModule, MI_U32 u32DevId);

MI_S32 MI_SYS_ConfGloPubPools(MI_VB_PoolListConf_t  stPoolListConf);

MI_S32 MI_SYS_ReleaseGloPubPools(void);

MI_S32 MI_SYS_Set_Vdec_UseVbPool_Flag(MI_U32 u32Value);

MI_S32 MI_SYS_ConfigPrivateMMAPool(MI_SYS_GlobalPrivPoolConfig_t *pstGlobalPrivPoolConf);

MI_S32 MI_SYS_MemsetPa(MI_PHY phyPa, MI_U32 u32Val, MI_U32 u32Lenth);

MI_S32 MI_SYS_MemcpyPa(MI_PHY phyDst, MI_PHY phySrc, MI_U32 u32Lenth);

MI_S32 MI_SYS_BufFillPa(MI_SYS_FrameData_t *pstBuf, MI_U32 u32Val, MI_SYS_WindowRect_t *pstRect);

MI_S32 MI_SYS_BufBlitPa(MI_SYS_FrameData_t *pstDstBuf, MI_SYS_WindowRect_t *pstDstRect, MI_SYS_FrameData_t *pstSrcBuf, MI_SYS_WindowRect_t *pstSrcRect);

MI_S32 MI_SYS_PrivateDevChnHeapAlloc(MI_ModuleId_e eModule, MI_U32 u32Devid, MI_S32 s32ChnId, MI_U8 *pu8BufName, MI_U32 u32blkSize, MI_PHY *pphyAddr, MI_BOOL bTailAlloc);

MI_S32 MI_SYS_PrivateDevChnHeapFree(MI_ModuleId_e eModule, MI_U32 u32Devid, MI_S32 s32ChnId, MI_PHY phyAddr);

MI_S32 MI_SYS_EnableChnOutputPortLowLatency(MI_SYS_ChnPort_t *pstChnPort, MI_BOOL bEnable, MI_U32 u32Param);

MI_S32 MI_SYS_InitDev(MI_SYS_InitParam_t *pstInitParam);

MI_S32 MI_SYS_DeInitDev(void);

MI_S32 MI_SYS_DupBuf(MI_SYS_BUF_HANDLE srcBufHandle , MI_SYS_BUF_HANDLE *pDupTargetBufHandle);

MI_S32 MI_SYS_Va2Pa (void *pVirtualAddress, MI_PHY *pPhyAddr);

/*
N.B.
below MMAHeapName can only be NULL or real mma heap name, do not set it with random character string.
you can get mma heap name xxx from "mma_heap=xxx," of cat /proc/cmdline.
*/
MI_S32 MI_SYS_MMA_Alloc(MI_U8 *pstMMAHeapName, MI_U32 u32BlkSize ,MI_PHY *phyAddr);

MI_S32 MI_SYS_MMA_Free(MI_PHY  phyAddr);
MI_S32 MI_SYS_Mmap(MI_U64 phyAddr, MI_U32 u32Size , void **ppVirtualAddress , MI_BOOL bCache);

MI_S32 MI_SYS_Munmap(void *pVirtualAddress, MI_U32 u32Size);
MI_S32 MI_SYS_FlushInvCache(void *pVirtualAddress, MI_U32 u32Length);

#ifdef __USER__
#define DEBUG_YUV_USER_API
#ifdef DEBUG_YUV_USER_API
typedef  FILE* FILE_HANDLE;
//return value of the following API is not standard file operation return value.

/*
return :
FILE_HANDLE type in fact is a pointer,
NULL:fail
not NULL:success
*/
FILE_HANDLE open_yuv_file(const char *pathname, int open_for_write);

/*
return value:
0:success
-1:fail
*/
int read_yuv_file(FILE_HANDLE filehandle, MI_SYS_FrameData_t framedata);

/*
return value:
0:success
-1:fail
*/
int write_yuv_file(FILE_HANDLE filehandle, MI_SYS_FrameData_t framedata);

void close_yuv_file(FILE_HANDLE filehandle);

/*
return value:
0:success
-1:fail
*/
int reset_yuv_file(FILE_HANDLE filehandle);

/*
return value:
0:success
-1:fail
*/
int is_in_yuv_file_end(FILE_HANDLE filehandle);
#endif
#else
/*
do nothing,kernel space debug API is in mi_sys_internal.h/mi_sys_internal.c
*/
#endif

#ifdef __cplusplus
}
#endif

#endif ///_MI_SYS_H_
