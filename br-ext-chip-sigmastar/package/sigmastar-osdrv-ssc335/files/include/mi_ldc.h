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

#ifndef __MI_LDC_H__
#define __MI_LDC_H__

#include "mi_ldc_datatype.h"

#define LDC_MAJOR_VERSION 2
#define LDC_SUB_VERSION 4
#define MACRO_TO_STR(macro) #macro
#define LDC_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_ldc_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_ldc_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_ldc_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_LDC_API_VERSION LDC_VERSION_STR(LDC_MAJOR_VERSION,LDC_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif


MI_S32 MI_LDC_CreateDevice(MI_LDC_DEV devId);
MI_S32 MI_LDC_DestroyDevice(MI_LDC_DEV devId);

MI_S32 MI_LDC_CreateChannel(MI_LDC_DEV devId, MI_LDC_CHN chnId);
MI_S32 MI_LDC_DestroyChannel(MI_LDC_DEV devId, MI_LDC_CHN chnId);

MI_S32 MI_LDC_StartChannel(MI_LDC_DEV devId, MI_LDC_CHN chnId);
MI_S32 MI_LDC_StopChannel(MI_LDC_DEV devId, MI_LDC_CHN chnId);

MI_S32 MI_LDC_GetOutputPortAttr(MI_LDC_DEV devId, MI_LDC_CHN chnId, MI_LDC_OutputPortAttr_t *pstOutputAttr);

MI_S32 MI_LDC_SetConfig(MI_LDC_DEV devId, MI_LDC_CHN chnId, void *pConfigAddr, MI_U32 u32ConfigSize);

MI_S32 MI_LDC_InitDev(MI_LDC_InitParam_t *pstInitParam);
MI_S32 MI_LDC_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif///_MI_VPE_H_
