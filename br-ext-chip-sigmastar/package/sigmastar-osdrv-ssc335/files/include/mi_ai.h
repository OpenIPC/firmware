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
#ifndef __MI_AI_H__
#define __MI_AI_H__

#include "mi_aio_datatype.h"
#include "mi_ai_datatype.h"


#define AI_MAJOR_VERSION 2
#define AI_SUB_VERSION 13
#define MACRO_TO_STR(macro) #macro
#define AI_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_ai_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_ai_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_ai_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_AI_API_VERSION AI_VERSION_STR(AI_MAJOR_VERSION,AI_SUB_VERSION)

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
MI_S32 MI_AI_SetPubAttr(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t *pstAttr);
MI_S32 MI_AI_GetPubAttr(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t*pstAttr);
MI_S32 MI_AI_Enable(MI_AUDIO_DEV AiDevId);
MI_S32 MI_AI_Disable(MI_AUDIO_DEV AiDevId);
MI_S32 MI_AI_EnableChn(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_DisableChn(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_GetFrame(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t*pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm , MI_S32 s32MilliSec);
MI_S32 MI_AI_ReleaseFrame(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t *pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm);
MI_S32 MI_AI_SetChnParam(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
MI_S32 MI_AI_GetChnParam(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
MI_S32 MI_AI_EnableReSmp(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_SampleRate_e eOutSampleRate);
MI_S32 MI_AI_DisableReSmp(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_SetVqeAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AI_VqeConfig_t *pstVqeConfig);
MI_S32 MI_AI_GetVqeAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn,  MI_AI_VqeConfig_t *pstVqeConfig);
MI_S32 MI_AI_EnableVqe(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_DisableVqe(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_ClrPubAttr(MI_AUDIO_DEV AiDevId);
MI_S32 MI_AI_SaveFile(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn,MI_AUDIO_SaveFileInfo_t *pstSaveFileInfo);
MI_S32 MI_AI_SetVqeVolume(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_S32 s32VolumeDb);
MI_S32 MI_AI_GetVqeVolume(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_S32 *ps32VolumeDb);
MI_S32 MI_AI_SetAencAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AencConfig_t * pstAencConfig);
MI_S32 MI_AI_GetAencAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AencConfig_t * pstAencConfig);
MI_S32 MI_AI_EnableAenc(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_DisableAenc(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_DupChn(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_SetAedAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AedConfig_t *pstAedConfig);
MI_S32 MI_AI_GetAedAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AedConfig_t *pstAedConfig);
MI_S32 MI_AI_EnableAed(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_DisableAed(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_GetAedResult(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AedResult_t *pstAedResult);
MI_S32 MI_AI_SetExtAecChn(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn,  MI_AI_CHN AiAECSndChn);
MI_S32 MI_AI_SetSslInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslInitAttr_t* pstSslInitAttr);
MI_S32 MI_AI_GetSslInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslInitAttr_t* pstSslInitAttr);
MI_S32 MI_AI_SetSslConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslConfigAttr_t* pstSslConfigAttr);
MI_S32 MI_AI_GetSslConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslConfigAttr_t* pstSslConfigAttr);
MI_S32 MI_AI_EnableSsl(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_DisableSsl(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_GetSslDoa(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_S32 *ps32SslDoa);
MI_S32 MI_AI_SetBfInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfInitAttr_t* pstBfInitAttr);
MI_S32 MI_AI_GetBfInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfInitAttr_t* pstBfInitAttr);
MI_S32 MI_AI_SetBfConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfConfigAttr_t* pstBfConfigAttr);
MI_S32 MI_AI_GetBfConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfConfigAttr_t* pstBfConfigAttr);
MI_S32 MI_AI_EnableBf(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_DisableBf(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
MI_S32 MI_AI_SetBfAngle(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_S32 s32BfAngle);
MI_S32 MI_AI_SetMute(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL bMute);
MI_S32 MI_AI_GetMute(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL *pbMute);
MI_S32 MI_AI_InitDev(MI_AI_InitParam_t *pstInitParam);
MI_S32 MI_AI_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif ///__MI_AI_H__
