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


#ifndef _MI_VDEC_H_
#define _MI_VDEC_H_

#define MI_VDEC_GET_ESBUFFER_DIRECT

#include "mi_vdec_datatype.h"

#define VDEC_MAJOR_VERSION 2
#define VDEC_SUB_VERSION 7
#define MACRO_TO_STR(macro) #macro
#define VDEC_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_vdec_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_vdec_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_vdec_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_VDEC_API_VERSION VDEC_VERSION_STR(VDEC_MAJOR_VERSION,VDEC_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_VDEC_InitDev(MI_VDEC_InitParam_t *pstVdecInitParam);
MI_S32 MI_VDEC_DeInitDev(void);
MI_S32 MI_VDEC_CreateChn(MI_VDEC_CHN VdecChn, MI_VDEC_ChnAttr_t *pstChnAttr);
MI_S32 MI_VDEC_DestroyChn(MI_VDEC_CHN VdecChn);
MI_S32 MI_VDEC_GetChnAttr(MI_VDEC_CHN VdecChn, MI_VDEC_ChnAttr_t *pstChnAttr);
MI_S32 MI_VDEC_StartChn(MI_VDEC_CHN VdecChn);
MI_S32 MI_VDEC_StopChn(MI_VDEC_CHN VdecChn);
MI_S32 MI_VDEC_GetChnStat(MI_VDEC_CHN VdecChn, MI_VDEC_ChnStat_t *pstChnStat);
MI_S32 MI_VDEC_FlushChn(MI_VDEC_CHN VdecChn);
MI_S32 MI_VDEC_ResetChn(MI_VDEC_CHN VdecChn);
MI_S32 MI_VDEC_SetChnParam(MI_VDEC_CHN VdecChn, MI_VDEC_ChnParam_t *pstChnParam);
MI_S32 MI_VDEC_GetChnParam(MI_VDEC_CHN VdecChn, MI_VDEC_ChnParam_t *pstChnParam);
MI_S32 MI_VDEC_SendStream(MI_VDEC_CHN VdecChn, MI_VDEC_VideoStream_t *pstVideoStream, MI_S32 s32MilliSec);
MI_S32 MI_VDEC_GetUserData(MI_VDEC_CHN VdecChn, MI_VDEC_UserData_t *pstUserData, MI_S32 s32MilliSec);
MI_S32 MI_VDEC_ReleaseUserData(MI_VDEC_CHN VdecChn, MI_VDEC_UserData_t *pstUserData);
MI_S32 MI_VDEC_SetDisplayMode(MI_VDEC_CHN VdecChn, MI_VDEC_DisplayMode_e eDisplayMode);
MI_S32 MI_VDEC_GetDisplayMode(MI_VDEC_CHN VdecChn, MI_VDEC_DisplayMode_e *peDisplayMode);
MI_S32 MI_VDEC_SetOutputPortAttr(MI_VDEC_CHN VdecChn, MI_VDEC_OutputPortAttr_t *pstOutputPortAttr);
MI_S32 MI_VDEC_GetOutputPortAttr(MI_VDEC_CHN VdecChn, MI_VDEC_OutputPortAttr_t *pstOutputPortAttr);
MI_S32 MI_VDEC_SetOutputPortLayoutMode(MI_VDEC_OutbufLayoutMode_e eBufTileMode);
MI_S32 MI_VDEC_GetOutputPortLayoutMode(MI_VDEC_OutbufLayoutMode_e *peBufTileMode);

#ifdef __cplusplus
}
#endif

#endif///_MI_VDEC_H_
