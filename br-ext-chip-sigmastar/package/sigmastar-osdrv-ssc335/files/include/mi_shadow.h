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
#ifndef _MI_SHADOW_H_
#define _MI_SHADOW_H_
#include "mi_shadow_datatype.h"

#define SHADOW_MAJOR_VERSION 2
#define SHADOW_SUB_VERSION 3
#define MACRO_TO_STR(macro) #macro
#define SHADOW_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_shadow_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_shadow_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_shadow_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_SHADOW_API_VERSION SHADOW_VERSION_STR(SHADOW_MAJOR_VERSION,SHADOW_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_SHADOW_RegisterDev(
    MI_SHADOW_RegisterDevParams_t *pstDevParams,
    MI_SHADOW_HANDLE *phShadow);
MI_S32 MI_SHADOW_UnRegisterDev(MI_SHADOW_HANDLE hShadow);
MI_S32 MI_SHADOW_GetOutputPortBuf(
    MI_SHADOW_HANDLE hShadow,
    MI_U32 u32ChnId,
    MI_U32 u32PortId,
    MI_SYS_BufConf_t *pstBufConfig,

    MI_BOOL *pbBlockedByRateCtrl,
    MI_SYS_BufInfo_t *pstBufInfo,
    MI_SYS_BUF_HANDLE *phBufHandle);
MI_S32 MI_SHADOW_GetInputPortBuf(
    MI_SHADOW_HANDLE hShadow,
    MI_U32 u32ChnId,
    MI_U32 u32PortId,

    MI_SYS_BufInfo_t *pstBufInfo,
    MI_SYS_BUF_HANDLE *phBufHandle);
MI_S32 MI_SHADOW_FinishBuf(MI_SHADOW_HANDLE hShadow, MI_SYS_BUF_HANDLE hBufHandle);
MI_S32 MI_SHADOW_RewindBuf(MI_SHADOW_HANDLE hShadow, MI_SYS_BUF_HANDLE hBufHandle);
MI_S32 MI_SHADOW_WaitOnInputTaskAvailable(MI_SHADOW_HANDLE hShadow ,MI_S32 u32TimeOutMs);

MI_S32 MI_SHADOW_EnableChannel(MI_SHADOW_HANDLE hShadow, MI_U32 u32ChnId);
MI_S32 MI_SHADOW_DisableChannel(MI_SHADOW_HANDLE hShadow, MI_U32 u32ChnId);
MI_S32 MI_SHADOW_EnableOutputPort(MI_SHADOW_HANDLE hShadow, MI_U32 u32ChnId, MI_U32 u32PortId);
MI_S32 MI_SHADOW_DisableOutputPort(MI_SHADOW_HANDLE hShadow, MI_U32 u32ChnId, MI_U32 u32PortId);
MI_S32 MI_SHADOW_EnableInputPort(MI_SHADOW_HANDLE hShadow, MI_U32 u32ChnId, MI_U32 u32PortId);
MI_S32 MI_SHADOW_DisableInputPort(MI_SHADOW_HANDLE hShadow, MI_U32 u32ChnId, MI_U32 u32PortId);
MI_S32 MI_SHADOW_InitDev(MI_SHADOW_InitParam_t *pstInitParam);
MI_S32 MI_SHADOW_DeInitDev(void);


MI_S32 MI_SHADOW_SetInputPortBufExtConf(
    MI_SHADOW_HANDLE hShadow,
    MI_U32 u32ChnId,
    MI_U32 u32PortId,
    MI_SYS_FrameBufExtraConfig_t * pstBufExtraConf
    );
MI_S32 MI_SHADOW_SetOutputPortBufExtConf(
    MI_SHADOW_HANDLE hShadow,
    MI_U32 u32ChnId,
    MI_U32 u32PortId,
    MI_SYS_FrameBufExtraConfig_t * pstBufExtraConf
    );


#ifdef __cplusplus
}
#endif
#endif ///_MI_SHADOW_H_
