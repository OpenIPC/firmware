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
#ifndef _MI_VPE_H_
#define _MI_VPE_H_

#include "mi_vpe_datatype.h"

#define VPE_MAJOR_VERSION 2
#define VPE_SUB_VERSION 12
#define MACRO_TO_STR(macro) #macro
#define VPE_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_vpe_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_vpe_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_vpe_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_VPE_API_VERSION VPE_VERSION_STR(VPE_MAJOR_VERSION,VPE_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_VPE_CreateChannel(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
MI_S32 MI_VPE_DestroyChannel(MI_VPE_CHANNEL VpeCh);
MI_S32 MI_VPE_GetChannelAttr(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
MI_S32 MI_VPE_SetChannelAttr(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);

MI_S32 MI_VPE_StartChannel(MI_VPE_CHANNEL VpeCh);
MI_S32 MI_VPE_StopChannel(MI_VPE_CHANNEL VpeCh);

MI_S32 MI_VPE_SetChannelParam(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);
MI_S32 MI_VPE_GetChannelParam(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);

MI_S32 MI_VPE_SetChannelCrop(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
MI_S32 MI_VPE_GetChannelCrop(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
MI_S32 MI_VPE_GetChannelRegionLuma(MI_VPE_CHANNEL VpeCh, MI_VPE_RegionInfo_t *pstRegionInfo, MI_U32 *pu32LumaData,MI_S32 s32MilliSec);
MI_S32 MI_VPE_SetChannelRotation(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e eType);
MI_S32 MI_VPE_GetChannelRotation(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e *pType);

MI_S32 MI_VPE_EnablePort(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
MI_S32 MI_VPE_DisablePort(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
MI_S32 MI_VPE_SetPortMode(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
MI_S32 MI_VPE_GetPortMode(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
MI_S32 MI_VPE_SetPortCrop(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);
MI_S32 MI_VPE_GetPortCrop(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);

MI_S32 MI_VPE_SetPortShowPosition(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);
MI_S32 MI_VPE_GetPortShowPosition(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);

MI_S32 MI_VPE_SkipFrame(MI_VPE_CHANNEL VpeCh, MI_U32 u32FrameNum);

MI_S32 MI_VPE_LDCBegViewConfig(MI_VPE_CHANNEL VpeCh);
MI_S32 MI_VPE_LDCEndViewConfig(MI_VPE_CHANNEL VpeCh);
MI_S32 MI_VPE_LDCSetViewConfig(MI_VPE_CHANNEL VpeCh, void *pConfigAddr, MI_U32 u32ConfigSize);

MI_S32 MI_VPE_Alloc_IspDataBuf(MI_U32 u32Size,void **pUserVirAddr);
MI_S32 MI_VPE_Free_IspDataBuf(void *pUserBuf);


MI_S32 MI_VPE_InitDev(MI_VPE_InitParam_t *pstInitParam);
MI_S32 MI_VPE_DeInitDev(void);

MI_S32 MI_VPE_CallBackTask_Register(MI_VPE_CallBackParam_t *pstCallBackParam);
MI_S32 MI_VPE_CallBackTask_Unregister(MI_VPE_CallBackParam_t *pstCallBackParam);

#ifdef __cplusplus
}
#endif

#endif///_MI_VPE_H_
