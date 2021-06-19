/* Copyright (c) 2019-2019 Sigmastar Technology Corp.
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

#ifndef _MI_MIPITX_H_
#define _MI_MIPITX_H_

#include "mi_mipitx_datatype.h"


#define MIPITX_MAJOR_VERSION 2
#define MIPITX_SUB_VERSION 0
#define MACRO_TO_STR(macro) #macro
#define MIPITX_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_mipitx_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_mipitx_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_mipitx_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_MIPITX_API_VERSION MIPITX_VERSION_STR(MIPITX_MAJOR_VERSION,MIPITX_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_MipiTx_CreateChannel(MI_U32 u32ChannelId, MI_MipiTx_ChannelAttr_t *pstMipiTxChAttr);
MI_S32 MI_MipiTx_DestroyChannel(MI_U32 u32ChannelId);

MI_S32 MI_MipiTx_GetChannelAttr(MI_U32 u32ChannelId, MI_MipiTx_ChannelAttr_t *pstMipiTxChAttr);

MI_S32 MI_MipiTx_StartChannel(MI_U32 u32ChannelId);
MI_S32 MI_MipiTx_StopChannel(MI_U32 u32ChannelId);

MI_S32 MI_MipiTx_SetTimingConfig(MI_U32 u32ChannelId, MI_MipiTx_TimingConfig_t *pstMipiTimingCfg);
MI_S32 MI_MipiTx_GetTimingConfig(MI_U32 u32ChannelId, MI_MipiTx_TimingConfig_t *pstMipiTimingCfg);
MI_S32 MI_MipiTx_InitDev(MI_MipiTx_InitParam_t *pstInitParam);
MI_S32 MI_MipiTx_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif

