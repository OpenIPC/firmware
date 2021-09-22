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
#ifndef _MI_HDMI_H_
#define _MI_HDMI_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"
#include "mi_hdmi_datatype.h"

#define HDMI_MAJOR_VERSION 2
#define HDMI_SUB_VERSION 3
#define MACRO_TO_STR(macro) #macro
#define HDMI_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_hdmi_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_hdmi_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_hdmi_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_HDMI_API_VERSION HDMI_VERSION_STR(HDMI_MAJOR_VERSION,HDMI_SUB_VERSION)

MI_S32 MI_HDMI_Init(MI_HDMI_InitParam_t *pstInitParam);
MI_S32 MI_HDMI_DeInit(void);
MI_S32 MI_HDMI_Open(MI_HDMI_DeviceId_e eHdmi);
MI_S32 MI_HDMI_Close(MI_HDMI_DeviceId_e eHdmi);
MI_S32 MI_HDMI_SetAttr(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_Attr_t *pstAttr);
MI_S32 MI_HDMI_GetAttr(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_Attr_t *pstAttr);
MI_S32 MI_HDMI_Start(MI_HDMI_DeviceId_e eHdmi);
MI_S32 MI_HDMI_Stop(MI_HDMI_DeviceId_e eHdmi);
MI_S32 MI_HDMI_GetSinkInfo(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_SinkInfo_t *pstSinkInfo);
MI_S32 MI_HDMI_SetAvMute(MI_HDMI_DeviceId_e eHdmi, MI_BOOL bAvMute);
MI_S32 MI_HDMI_ForceGetEdid(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_Edid_t *pstEdidData);
MI_S32 MI_HDMI_SetDeepColor(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_DeepColor_e eDeepColor);
MI_S32 MI_HDMI_GetDeepColor(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_DeepColor_e *peDeepColor);
MI_S32 MI_HDMI_SetInfoFrame(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_InfoFrame_t *pstInfoFrame);
MI_S32 MI_HDMI_GetInfoFrame(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_InfoFrameType_e eInfoFrameType,
    MI_HDMI_InfoFrame_t *pstInfoFrame);
MI_S32 MI_HDMI_SetAnalogDrvCurrent(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_AnalogDrvCurrent_t *pstAnalogDrvCurrent);
MI_S32 MI_HDMI_InitDev(MI_HDMI_InitParam_t *pstInitParam);
MI_S32 MI_HDMI_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif ///_MI_HDMI_H_
