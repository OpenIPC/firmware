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

#ifndef _MI_IPU_H_
#define _MI_IPU_H_

#include "mi_common.h"
#include "mi_common_datatype.h"
#include "mi_sys_datatype.h"
#include "cam_os_wrapper.h"
#include "mi_ipu_datatype.h"

//#define PERFORMANCE_DEBUG
//#define IPU_DEBUG
#define ipu_err(fmt, args...) CamOsPrintf(fmt, ##args)

#ifdef IPU_DEBUG
#define ipu_info(fmt, args...) CamOsPrintf(fmt,##args)
#else
#define ipu_info(fmt, args...)
#endif


#define IPU_MAJOR_VERSION 1
#define IPU_SUB_VERSION 0
#define MACRO_TO_STR(macro) #macro
#define IPU_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_ipu_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_ipu_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_ipu_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_IPU_API_VERSION IPU_VERSION_STR(IPU_MAJOR_VERSION,IPU_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_IPU_CreateDevice(MI_IPU_DevAttr_t *pstIPUDevAttr, SerializedReadFunc pReadFunc, char *pReadCtx, MI_U32 FWSize);
MI_S32 MI_IPU_DestroyDevice(void);
MI_S32 MI_IPU_CreateCHN(MI_IPU_CHN *ptChnId, MI_IPUChnAttr_t *pstIPUChnAttr,SerializedReadFunc pReadFunc, char *pReadCtx);
MI_S32 MI_IPU_DestroyCHN(MI_IPU_CHN u32ChnId);
MI_S32 MI_IPU_GetInOutTensorDesc(MI_IPU_CHN u32ChnId, MI_IPU_SubNet_InputOutputDesc_t *pstDesc);
MI_S32 MI_IPU_GetInputTensors(MI_IPU_CHN u32ChnId, MI_IPU_TensorVector_t *pstInputTensorVector);
MI_S32 MI_IPU_PutInputTensors(MI_IPU_CHN u32ChnId, MI_IPU_TensorVector_t *pstInputTensorVector);
MI_S32 MI_IPU_GetOutputTensors(MI_IPU_CHN u32ChnId, MI_IPU_TensorVector_t *pstInputTensorVector);
MI_S32 MI_IPU_PutOutputTensors(MI_IPU_CHN u32ChnId, MI_IPU_TensorVector_t *pstInputTensorVector);
MI_S32 MI_IPU_Invoke(MI_IPU_CHN u32ChnId, MI_IPU_TensorVector_t *pstInputTensorVector, MI_IPU_TensorVector_t *pstOuputTensorVector);
MI_S32 MI_IPU_GetOfflineModeStaticInfo(SerializedReadFunc pReadFunc, char *pReadCtx, MI_IPU_OfflineModelStaticInfo_t *pStaticInfo);


#ifdef __cplusplus
}
#endif

#endif
