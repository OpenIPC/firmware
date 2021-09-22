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
#ifndef _MI_VDISP_H_
#define _MI_VDISP_H_
#include "mi_vdisp_datatype.h"

#define VDISP_MAJOR_VERSION 2
#define VDISP_SUB_VERSION 5
#define MACRO_TO_STR(macro) #macro
#define VDISP_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_vdisp_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_vdisp_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_vdisp_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_VDISP_API_VERSION VDISP_VERSION_STR(VDISP_MAJOR_VERSION,VDISP_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_VDISP_Init(void);
MI_S32 MI_VDISP_Exit(void);

MI_S32 MI_VDISP_OpenDevice(MI_VDISP_DEV DevId);
MI_S32 MI_VDISP_CloseDevice(MI_VDISP_DEV DevId);

MI_S32 MI_VDISP_SetOutputPortAttr(MI_VDISP_DEV DevId,
               MI_VDISP_PORT PortId,
               MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);
MI_S32 MI_VDISP_GetOutputPortAttr(MI_VDISP_DEV DevId,
               MI_VDISP_PORT PortId,
               MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);

MI_S32 MI_VDISP_SetInputChannelAttr(MI_VDISP_DEV DevId,
               MI_VDISP_CHN ChnId,
               MI_VDISP_InputChnAttr_t *pstInputChnAttr);
MI_S32 MI_VDISP_GetInputChannelAttr(MI_VDISP_DEV DevId,
               MI_VDISP_CHN ChnId,
               MI_VDISP_InputChnAttr_t *pstInputChnAttr);

MI_S32 MI_VDISP_EnableInputChannel(MI_VDISP_DEV DevId,
               MI_VDISP_CHN ChnId);
MI_S32 MI_VDISP_DisableInputChannel(MI_VDISP_DEV DevId,
               MI_VDISP_CHN ChnId);

MI_S32 MI_VDISP_StartDev(MI_VDISP_DEV DevId);
MI_S32 MI_VDISP_StopDev(MI_VDISP_DEV DevId);

MI_S32 MI_VDISP_InitDev(MI_VDISP_InitParam_t *pstInitParam);
MI_S32 MI_VDISP_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif///_MI_VDISP_H_
