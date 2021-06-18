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

/*
*   mi_isp.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _MI_ISP_GENERAL_H_
#define _MI_ISP_GENERAL_H_

#include "mi_syscall.h"
#include "mi_vpe_datatype.h"
#include "mi_common.h"
#include "mi_isp.h"
#include "mi_isp_datatype.h"

#ifdef CAM_OS_LINUX_USER
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LINUX_FLOW_ON_DUAL_OS
#define DUAL_OS_RUN_ON_LINUX 1
#define DUAL_OS_RUN_ON_RTOS 0
#else
#ifdef __RTOS__
#define DUAL_OS_RUN_ON_LINUX 0
#define DUAL_OS_RUN_ON_RTOS 1
#else
#define DUAL_OS_RUN_ON_LINUX 0
#define DUAL_OS_RUN_ON_RTOS 0
#endif
#endif

#define MI_ISP_MSG(args...)         printf(args)
#define MI_ISP_DMSG(args...)        do{}while(0)
#define MI_ISP_EMSG(args...)        printf(args)
#define MI_ISP_VMSG(args...)        do{}while(0)

#define MI_ISP_CALLOC    calloc
#define MI_ISP_FREE      free

extern MI_S32 MI_VPE_GetIspApiData(MI_VPE_IspApiHeader_t *pstIspDataHeader, void *pVirData);
extern MI_S32 MI_VPE_SetIspApiData(MI_VPE_IspApiHeader_t *pstIspDataHeader, void *pVirData);

MI_S32 _MI_ISP_SetIspApiData(MI_VPE_IspApiHeader_t *pstIspDataHeader, void *pVirData);
MI_S32 _MI_ISP_GetIspApiData(MI_VPE_IspApiHeader_t *pstIspDataHeader, void *pVirData);

#if (DUAL_OS_RUN_ON_LINUX)
#define MI_GETAPI(Channel, APIFuncID, PARAM_t, PtrData)\
    MI_S32 s32Ret = MI_ISP_OK;\
    MI_VPE_IspApiHeader_t stIspApiHeader;\
    PARAM_t  *pIspBuffer = NULL;\
    MI_VPE_Alloc_IspDataBuf(sizeof(PARAM_t), &pIspBuffer);\
    memcpy(pIspBuffer, PtrData, sizeof(PARAM_t));\
    stIspApiHeader.u32HeadSize  = sizeof(MI_VPE_IspApiHeader_t);\
    stIspApiHeader.u32DataLen   = sizeof(PARAM_t);\
    stIspApiHeader.u32CtrlID    = APIFuncID;\
    stIspApiHeader.u32Channel   = Channel;\
    stIspApiHeader.s32Ret       = 0;\
    MI_SYS_FlushInvCache((void *)pIspBuffer, sizeof(PARAM_t));\
    s32Ret = stIspApiHeader.s32Ret  = MI_VPE_GetIspApiData(&stIspApiHeader, pIspBuffer);\
    memcpy(PtrData, pIspBuffer, sizeof(PARAM_t));\
    MI_VPE_Free_IspDataBuf(pIspBuffer);\
    return s32Ret;\

#define MI_SETAPI(Channel, APIFuncID, PARAM_t, PtrData)\
    MI_S32 s32Ret = MI_ISP_OK;\
    MI_VPE_IspApiHeader_t stIspApiHeader;\
    PARAM_t  *pIspBuffer = NULL;\
    MI_VPE_Alloc_IspDataBuf(sizeof(PARAM_t), &pIspBuffer);\
    stIspApiHeader.u32HeadSize  = sizeof(MI_VPE_IspApiHeader_t);\
    stIspApiHeader.u32DataLen   = sizeof(PARAM_t);\
    stIspApiHeader.u32CtrlID    = APIFuncID;\
    stIspApiHeader.u32Channel   = Channel;\
    stIspApiHeader.s32Ret       = 0;\
    memcpy(pIspBuffer, PtrData, sizeof(PARAM_t));\
    MI_SYS_FlushInvCache((void *)pIspBuffer, sizeof(PARAM_t));\
    s32Ret = stIspApiHeader.s32Ret = MI_VPE_SetIspApiData(&stIspApiHeader, pIspBuffer);\
    MI_VPE_Free_IspDataBuf(pIspBuffer);\
    return s32Ret;\

#define MI_CALI_SETAPI(Channel, APIFuncID, PARAM_t, DataSize, PtrData)\
    MI_VPE_IspApiHeader_t stIspApiHeader;\
    char  *pIspBuffer = NULL;\
    MI_VPE_Alloc_IspDataBuf(DataSize, &pIspBuffer);\
    stIspApiHeader.u32HeadSize  = sizeof(MI_VPE_IspApiHeader_t);\
    stIspApiHeader.u32DataLen   = DataSize;\
    stIspApiHeader.u32CtrlID    = APIFuncID;\
    stIspApiHeader.u32Channel   = Channel;\
    stIspApiHeader.s32Ret       = 0;\
    memcpy(pIspBuffer, PtrData, DataSize);\
    MI_SYS_FlushInvCache((void *)pIspBuffer, DataSize);\
    stIspApiHeader.s32Ret = MI_VPE_SetIspApiData(&stIspApiHeader, pIspBuffer);\
    MI_VPE_Free_IspDataBuf(pIspBuffer);\

#else
#define MI_GETAPI(Channel, APIFuncID, PARAM_t, PtrData)\
    MI_S32 s32Ret = MI_ISP_OK;\
    MI_VPE_IspApiHeader_t stIspApiHeader;\
    stIspApiHeader.u32HeadSize  = sizeof(MI_VPE_IspApiHeader_t);\
    stIspApiHeader.u32DataLen   = sizeof(PARAM_t);\
    stIspApiHeader.u32CtrlID    = APIFuncID;\
    stIspApiHeader.u32Channel   = Channel;\
    stIspApiHeader.s32Ret       = 0;\
    /*s32Ret = stIspApiHeader.s32Ret = MI_VPE_GetIspApiData(&stIspApiHeader, PtrData);*/\
    s32Ret = stIspApiHeader.s32Ret = _MI_ISP_GetIspApiData(&stIspApiHeader, PtrData);\
    MI_ISP_DMSG("[%s] - (Channel,CtrlID, DataLenght, HeadSize) = (%d,%d,%d,%d)\n", __FUNCTION__, stIspApiHeader.u32Channel, stIspApiHeader.u32CtrlID, stIspApiHeader.u32DataLen, stIspApiHeader.u32HeadSize);\
    return s32Ret;\

#define MI_SETAPI(Channel, APIFuncID, PARAM_t, PtrData)\
    MI_S32 s32Ret = MI_ISP_OK;\
    MI_VPE_IspApiHeader_t stIspApiHeader;\
    stIspApiHeader.u32HeadSize  = sizeof(MI_VPE_IspApiHeader_t);\
    stIspApiHeader.u32DataLen   = sizeof(PARAM_t);\
    stIspApiHeader.u32CtrlID    = APIFuncID;\
    stIspApiHeader.u32Channel   = Channel;\
    stIspApiHeader.s32Ret       = 0;\
    /*s32Ret = stIspApiHeader.s32Ret = MI_VPE_SetIspApiData(&stIspApiHeader, PtrData);*/\
    s32Ret = stIspApiHeader.s32Ret = _MI_ISP_SetIspApiData(&stIspApiHeader, PtrData);\
    MI_ISP_DMSG("[%s] - (Channel,CtrlID, DataLenght, HeadSize) = (%d,%d,%d,%d)\n", __FUNCTION__, stIspApiHeader.u32Channel, stIspApiHeader.u32CtrlID, stIspApiHeader.u32DataLen, stIspApiHeader.u32HeadSize);\
    return s32Ret;\

#define MI_CALI_SETAPI(Channel, APIFuncID, PARAM_t, DataSize, PtrData)\
    MI_VPE_IspApiHeader_t stIspApiHeader;\
    stIspApiHeader.u32HeadSize  = sizeof(MI_VPE_IspApiHeader_t);\
    stIspApiHeader.u32DataLen   = DataSize;\
    stIspApiHeader.u32CtrlID    = APIFuncID;\
    stIspApiHeader.u32Channel   = Channel;\
    stIspApiHeader.s32Ret       = 0;\
    s32Ret = stIspApiHeader.s32Ret = MI_VPE_SetIspApiData(&stIspApiHeader, PtrData);\
    MI_ISP_DMSG("[%s] - (Channel,CtrlID, DataLenght, HeadSize) = (%d,%d,%d,%d)\n", __FUNCTION__, stIspApiHeader->u32Channel, stIspApiHeader->u32CtrlID, stIspApiHeader->u32DataLen, stIspApiHeader->u32HeadSize);\

#endif

#define MI_ISP_SET(APIFunc, PARAM_t) \
    if(ApiLen != sizeof(PARAM_t)) \
    { \
        MI_ISP_MSG("[%s][%s] APIID:%d error param \n", __FILE__, __func__, ApiId); \
        break; \
    } \
    ret = APIFunc(Channel, (PARAM_t*)pApiBuf); \

#define MI_ISP_GET(APIFunc, PARAM_t) \
    *ApiLen = sizeof(PARAM_t); \
    ret = APIFunc(Channel, (PARAM_t*)pApiBuf); \

#define MI_CALL_SETAPI(Channel, APIFunc, PARAM_t)\
    MI_S32 s32Ret = MI_ISP_OK;\
    PARAM_t *stParam = MI_ISP_CALLOC(1, sizeof(PARAM_t));\
    memcpy(stParam, param_ary[0], sizeof(PARAM_t));\
    s32Ret = APIFunc(Channel, stParam);\
    MI_ISP_DMSG("[%s][%s] param_num = %d\n", __FILE__, __FUNCTION__, param_num);\
    MI_ISP_FREE(stParam);\
    return s32Ret;\

#define MI_RESET_API(Channel, APIFunc, PARAM_t)\
    PARAM_t *stParam = MI_ISP_CALLOC(1, sizeof(PARAM_t));\
    memset(stParam, 0x00, sizeof(PARAM_t));\
    s32Ret = APIFunc(Channel, stParam);\
    MI_ISP_DMSG("[%s][%s] param_num = %d\n", __FILE__, __FUNCTION__, param_num);\
    MI_ISP_FREE(stParam);\

#ifdef __cplusplus
}   //end of extern C
#endif

#endif  //_MI_ISP_GENERAL_H_
