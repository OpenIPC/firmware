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
#ifndef __MI_VDF_H__
#define __MI_VDF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "mi_vdf_datatype.h"

#define VDF_MAJOR_VERSION 2
#define VDF_SUB_VERSION 3
#define MACRO_TO_STR(macro) #macro
#define VDF_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_vdf_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_vdf_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_vdf_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_VDF_API_VERSION VDF_VERSION_STR(VDF_MAJOR_VERSION,VDF_SUB_VERSION)

MI_S32 MI_VDF_Init(void);
MI_S32 MI_VDF_Uninit(void);
MI_S32 MI_VDF_CreateChn(MI_VDF_CHANNEL VdfChn, const MI_VDF_ChnAttr_t* pstAttr);
MI_S32 MI_VDF_DestroyChn(MI_VDF_CHANNEL VdfChn);
MI_S32 MI_VDF_SetChnAttr(MI_VDF_CHANNEL VdfChn, const MI_VDF_ChnAttr_t* pstAttr);
MI_S32 MI_VDF_GetChnAttr(MI_VDF_CHANNEL VdfChn, MI_VDF_ChnAttr_t* pstAttr);
MI_S32 MI_VDF_EnableSubWindow(MI_VDF_CHANNEL VdfChn, MI_U8 u8Col, MI_U8 u8Row, MI_U8 u8Enable);
MI_S32 MI_VDF_Run(MI_VDF_WorkMode_e enWorkMode);
MI_S32 MI_VDF_Stop(MI_VDF_WorkMode_e enWorkMode);
MI_S32 MI_VDF_GetResult(MI_VDF_CHANNEL VdfChn, MI_VDF_Result_t* pstVdfResult, MI_S32 s32MilliSec);
MI_S32 MI_VDF_PutResult(MI_VDF_CHANNEL VdfChn, MI_VDF_Result_t* pstVdfResult);
MI_S32 MI_VDF_GetLibVersion(MI_VDF_CHANNEL VdfChn, MI_U32* u32VDFVersion);
MI_S32 MI_VDF_GetDebugInfo(MI_VDF_CHANNEL VdfChn, MI_VDF_DebugInfo_t *pstDebugInfo);

#ifdef __cplusplus
}
#endif

#endif /* __MI_VDF_H__ */
