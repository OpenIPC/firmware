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
#ifndef _MI_SENSOR_H_
#define _MI_SENSOR_H_

#include "mi_sensor_datatype.h"

#define SENSOR_MAJOR_VERSION 2
#define SENSOR_SUB_VERSION 4
#define MACRO_TO_STR(macro) #macro
#define SENSOR_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_sensor_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_sensor_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_sensor_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_SENSOR_API_VERSION SENSOR_VERSION_STR(SENSOR_MAJOR_VERSION,SENSOR_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_SNR_Enable(MI_SNR_PAD_ID_e     ePADId);
MI_S32 MI_SNR_Disable(MI_SNR_PAD_ID_e      ePADId);  // Stop the sensor operation, streaming, mclk

MI_S32 MI_SNR_GetPadInfo(MI_SNR_PAD_ID_e       ePADId, MI_SNR_PADInfo_t  *pstPadInfo);
MI_S32 MI_SNR_GetPlaneInfo(MI_SNR_PAD_ID_e       ePADId, MI_U32  u32PlaneID, MI_SNR_PlaneInfo_t *pstPlaneInfo);

MI_S32 MI_SNR_GetFps(MI_SNR_PAD_ID_e      ePADId, MI_U32 *pFps);
MI_S32 MI_SNR_SetFps(MI_SNR_PAD_ID_e      ePADId, MI_U32  u32Fps);

MI_S32 MI_SNR_GetBT656SrcType(MI_SNR_PAD_ID_e        ePADId, MI_U32 u32PlaneID, MI_SNR_Anadec_SrcType_e *psttype);

MI_S32 MI_SNR_QueryResCount(MI_SNR_PAD_ID_e        ePADId, MI_U32 *pu32ResCount);
MI_S32 MI_SNR_GetRes(MI_SNR_PAD_ID_e      ePADId, MI_U8 u8ResIdx, MI_SNR_Res_t *pstRes);
MI_S32 MI_SNR_GetCurRes(MI_SNR_PAD_ID_e       ePADId, MI_U8 *pu8CurResIdx, MI_SNR_Res_t  *pstCurRes);
MI_S32 MI_SNR_SetRes(MI_SNR_PAD_ID_e      ePADId, MI_U8 u8ResIdx);

MI_S32 MI_SNR_SetOrien(MI_SNR_PAD_ID_e      ePADId, MI_BOOL bMirror, MI_BOOL bFlip);
MI_S32 MI_SNR_GetOrien(MI_SNR_PAD_ID_e      ePADId, MI_BOOL *pbMirror, MI_BOOL *pbFlip);

MI_S32 MI_SNR_SetPlaneMode(MI_SNR_PAD_ID_e      ePADId, MI_BOOL bEnable);
MI_S32 MI_SNR_GetPlaneMode(MI_SNR_PAD_ID_e      ePADId, MI_BOOL *pbEnable);

MI_S32 MI_SNR_CustFunction(MI_SNR_PAD_ID_e ePADId, MI_U32 u32CmdId, MI_U32 u32DataSize, void *pCustData, MI_SNR_CUST_DIR_e eDir);
MI_S32 MI_SNR_InitDev(MI_SNR_InitParam_t *pstInitParam);
MI_S32 MI_SNR_DeInitDev(void);


#ifdef __cplusplus
}
#endif

#endif
