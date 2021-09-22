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
#ifndef __MI_AO_H__
#define __MI_AO_H__

#include "mi_aio_datatype.h"
#include "mi_ao_datatype.h"

#define AO_MAJOR_VERSION 2
#define AO_SUB_VERSION 11
#define MACRO_TO_STR(macro) #macro
#define AO_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_ao_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_ao_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_ao_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_AO_API_VERSION AO_VERSION_STR(AO_MAJOR_VERSION,AO_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Include files
//=============================================================================


//=============================================================================
// Extern definition
//=============================================================================

//=============================================================================
// Macro definition
//=============================================================================


//=============================================================================
// Data type definition
//=============================================================================


//=============================================================================
// Variable definition
//=============================================================================


//=============================================================================
// Global function definition
//=============================================================================
MI_S32 MI_AO_SetPubAttr(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
MI_S32 MI_AO_GetPubAttr(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
MI_S32 MI_AO_Enable(MI_AUDIO_DEV AoDevId);
MI_S32 MI_AO_Disable(MI_AUDIO_DEV AoDevId);
MI_S32 MI_AO_EnableChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_DisableChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_SendFrame(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AUDIO_Frame_t *pstData, MI_S32 s32MilliSec);
MI_S32 MI_AO_EnableReSmp(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AUDIO_SampleRate_e eInSampleRate);
MI_S32 MI_AO_DisableReSmp(MI_AUDIO_DEV AiDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_PauseChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_ResumeChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_ClearChnBuf(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_QueryChnStat(MI_AUDIO_DEV AoDevId , MI_AO_CHN AoChn, MI_AO_ChnState_t *pstStatus);
MI_S32 MI_AO_SetVolume(MI_AUDIO_DEV AoDevId, MI_S32 s32VolumeDb);
MI_S32 MI_AO_GetVolume(MI_AUDIO_DEV AoDevId, MI_S32 *ps32VolumeDb);
MI_S32 MI_AO_SetMute(MI_AUDIO_DEV AoDevId, MI_BOOL bEnable);
MI_S32 MI_AO_GetMute(MI_AUDIO_DEV AoDevId, MI_BOOL *pbEnable);
MI_S32 MI_AO_ClrPubAttr(MI_AUDIO_DEV AoDevId);
MI_S32 MI_AO_SetVqeAttr(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_VqeConfig_t *pstVqeConfig);
MI_S32 MI_AO_GetVqeAttr(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_VqeConfig_t *pstVqeConfig);
MI_S32 MI_AO_EnableVqe(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_DisableVqe(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_SetAdecAttr(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_AdecConfig_t *pstAdecConfig);
MI_S32 MI_AO_GetAdecAttr(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_AdecConfig_t *pstAdecConfig);
MI_S32 MI_AO_EnableAdec(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_DisableAdec(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
MI_S32 MI_AO_SetChnParam(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
MI_S32 MI_AO_GetChnParam(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
MI_S32 MI_AO_SetSrcGain(MI_AUDIO_DEV AoDevId, MI_S32 s32VolumeDb);
MI_S32 MI_AO_InitDev(MI_AO_InitParam_t *pstInitParam);
MI_S32 MI_AO_DeInitDev(void);
#ifdef __cplusplus
}
#endif

#endif///__MI_AO_H__
