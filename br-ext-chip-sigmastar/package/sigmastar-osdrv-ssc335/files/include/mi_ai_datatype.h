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
#ifndef __MI_AI_DATATYPE_H__
#define __MI_AI_DATATYPE_H__

#include "mi_common_datatype.h"

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
#define MI_AI_DEV_NUM_MAX   4
#define MI_AI_CHAN_NUM_MAX  4

#define MI_AI_ERR_INVALID_DEVID     (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID))
#define MI_AI_ERR_ILLEGAL_PARAM     (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM))
#define MI_AI_ERR_NOT_ENABLED       (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_ENABLE))
#define MI_AI_ERR_NULL_PTR          (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR))
#define MI_AI_ERR_INVALID_CHNID     (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID))
#define MI_AI_ERR_NOT_CONFIG        (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG))
#define MI_AI_ERR_NOT_SUPPORT       (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT))
#define MI_AI_ERR_NOT_PERM          (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM))
#define MI_AI_ERR_NOMEM             (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM))
#define MI_AI_ERR_NOBUF             (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOBUF))
#define MI_AI_ERR_BUF_EMPTY         (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY))
#define MI_AI_ERR_BUF_FULL          (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL))
#define MI_AI_ERR_SYS_NOTREADY      (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY))
#define MI_AI_ERR_BUSY              (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY))
#define MI_AI_ERR_VQE_ERR           (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_AI_VQE_ERR))
#define MI_AI_ERR_AENC_ERR          (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_AI_AENC_ERR))
#define MI_AI_ERR_AED_ERR           (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_AI_AED_ERR))
#define MI_AI_ERR_SSL_ERR           (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_AI_SSL_ERR))
#define MI_AI_ERR_BF_ERR           (MI_DEF_ERR( E_MI_MODULE_ID_AI, E_MI_ERR_LEVEL_ERROR, E_MI_AI_BF_ERR))

//=============================================================================
// Data type definition
//=============================================================================
typedef enum
{
    E_MI_AI_VQE_ERR = MI_AI_INITIAL_ERROR_CODE, /* VQE of AI error */
    E_MI_AI_AENC_ERR,
    E_MI_AI_AED_ERR,
    E_MI_AI_SSL_ERR,
    E_MI_AI_BF_ERR,
} MI_AI_ErrCode_e;

typedef struct MI_AI_ChnGainConfig_s
{
    MI_BOOL bEnableGainSet;
    MI_S16 s16FrontGain;
    MI_S16 s16RearGain;
} MI_AI_ChnGainConfig_t;

typedef struct MI_AI_ChnParam_s
{
    MI_AI_ChnGainConfig_t stChnGain;
    MI_U32 u32Reserved;
} MI_AI_ChnParam_t;

typedef struct MI_AI_AecConfig_s
{
    MI_BOOL bComfortNoiseEnable;
    MI_S16 s16DelaySample;
    MI_U32 u32AecSupfreq[6];
    MI_U32 u32AecSupIntensity[7];
    MI_S32 s32Reserved;
} MI_AI_AecConfig_t;

typedef struct MI_AI_VqeConfig_s
{
    MI_BOOL bHpfOpen;
    MI_BOOL bAecOpen;
    MI_BOOL bAnrOpen;
    MI_BOOL bAgcOpen;
    MI_BOOL bEqOpen;
    MI_U32 	u32ChnNum;
    MI_S32 s32WorkSampleRate;
    MI_S32 s32FrameSample;
    MI_AUDIO_HpfConfig_t stHpfCfg;
    MI_AI_AecConfig_t stAecCfg;
    MI_AUDIO_AnrConfig_t stAnrCfg;
    MI_AUDIO_AgcConfig_t stAgcCfg;
    MI_AUDIO_EqConfig_t stEqCfg;
} MI_AI_VqeConfig_t;


typedef enum{
    E_MI_AUDIO_AENC_TYPE_G711A = 0,
    E_MI_AUDIO_AENC_TYPE_G711U,
    E_MI_AUDIO_AENC_TYPE_G726,
    E_MI_AUDIO_AENC_TYPE_INVALID,
} MI_AUDIO_AencType_e;


typedef struct MI_AUDIO_AencG711Config_s{
    MI_AUDIO_SampleRate_e eSamplerate;
    MI_AUDIO_SoundMode_e eSoundmode;
} MI_AUDIO_AencG711Config_t;

typedef struct MI_AUDIO_AencG726Config_s{
    MI_AUDIO_SampleRate_e eSamplerate;
    MI_AUDIO_SoundMode_e eSoundmode;
    MI_AUDIO_G726Mode_e eG726Mode;
} MI_AUDIO_AencG726Config_t;

typedef struct MI_AI_AencConfig_s{
    MI_AUDIO_AencType_e  eAencType;
    union
    {
        MI_AUDIO_AencG711Config_t  stAencG711Cfg;
        MI_AUDIO_AencG726Config_t  stAencG726Cfg;
    };
} MI_AI_AencConfig_t;

typedef enum
{
    E_MI_AUDIO_AED_SEN_LOW,
    E_MI_AUDIO_AED_SEN_MID,
    E_MI_AUDIO_AED_SEN_HIGH,
    E_MI_AUDIO_AED_SEN_INVALID,
} MI_AUDIO_AedSensitivity_e;

typedef struct MI_AI_AedConfig_s
{
    MI_BOOL bEnableNr;
    MI_AUDIO_AedSensitivity_e eSensitivity;
    MI_S32 s32OperatingPoint;
    MI_S32 s32VadThresholdDb;
    MI_S32 s32LsdThresholdDb;
} MI_AI_AedConfig_t;

typedef struct MI_AI_AedResult_s
{
    MI_BOOL bAcousticEventDetected;
    MI_BOOL bLoudSoundDetected;
} MI_AI_AedResult_t;

typedef struct MI_AI_SslInitAttr_s
{
	MI_U32 u32MicDistance;
	MI_BOOL bBfMode;
} MI_AI_SslInitAttr_t;

typedef struct MI_AI_SslConfigAttr_s
{
	MI_S32 s32Temperature;
	MI_S32 s32NoiseGateDbfs;
	MI_S32 s32DirectionFrameNum;

} MI_AI_SslConfigAttr_t;

typedef struct MI_AI_BfInitAttr_s
{
	MI_U32 u32MicDistance;
	MI_U32 u32ChanCnt;
} MI_AI_BfInitAttr_t;

typedef struct MI_AI_BfConfigAttr_s
{
	MI_S32 s32Temperature;
	MI_S32 s32NoiseGateDbfs;
	MI_S32 s32NoiseSupressionMode;
	MI_S32 s32NoiseEstimation;
	MI_FLOAT outputGain;
} MI_AI_BfConfigAttr_t;

typedef struct MI_AI_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_AI_InitParam_t;

//=============================================================================
// Variable definition
//=============================================================================


//=============================================================================
// Global function definition
//=============================================================================


#ifdef __cplusplus
}
#endif

#endif //__MI_AI_DATATYPE_H__
