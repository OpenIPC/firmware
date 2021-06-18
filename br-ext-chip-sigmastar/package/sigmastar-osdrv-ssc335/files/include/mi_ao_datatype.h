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
#ifndef __MI_AO_DATATYPE_H__
#define __MI_AO_DATATYPE_H__

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
#define MI_AO_DEV_NUM_MAX   4
#define MI_AO_CHAN_NUM_MAX  1

#define MI_AO_ERR_INVALID_DEVID     (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID))
#define MI_AO_ERR_ILLEGAL_PARAM     (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM))
#define MI_AO_ERR_NOT_ENABLED       (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_ENABLE))
#define MI_AO_ERR_NULL_PTR          (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR))
#define MI_AO_ERR_INVALID_CHNID     (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID))
#define MI_AO_ERR_NOT_CONFIG        (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG))
#define MI_AO_ERR_NOT_SUPPORT       (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT))
#define MI_AO_ERR_NOT_PERM          (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM))
#define MI_AO_ERR_NOMEM             (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM))
#define MI_AO_ERR_NOBUF             (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOBUF))
#define MI_AO_ERR_BUF_EMPTY         (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY))
#define MI_AO_ERR_BUF_FULL          (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL))
#define MI_AO_ERR_SYS_NOTREADY      (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY))
#define MI_AO_ERR_BUSY              (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY))
#define MI_AO_ERR_VQE_ERR           (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_AO_VQE_ERR))
#define MI_AO_ERR_ADEC_ERR          (MI_DEF_ERR( E_MI_MODULE_ID_AO, E_MI_ERR_LEVEL_ERROR, E_MI_AO_ADEC_ERR))
//=============================================================================
// Data type definition
//=============================================================================
typedef enum
{
    E_MI_AO_VQE_ERR = MI_AO_INITIAL_ERROR_CODE, /* VQE of AO error */
    E_MI_AO_ADEC_ERR,                           //ADEC of AO error
}MI_AO_ErrCode_e;


typedef struct MI_AO_ChnState_s
{
    MI_U32 u32ChnTotalNum;
    MI_U32 u32ChnFreeNum;
    MI_U32 u32ChnBusyNum;
} MI_AO_ChnState_t;

typedef struct MI_AO_VqeConfig_s
{
    MI_BOOL              bHpfOpen;
    MI_BOOL              bAnrOpen;
    MI_BOOL              bAgcOpen;
    MI_BOOL              bEqOpen;
    MI_S32               s32WorkSampleRate;
    MI_S32               s32FrameSample;
    MI_AUDIO_HpfConfig_t stHpfCfg;
    MI_AUDIO_AnrConfig_t stAnrCfg;
    MI_AUDIO_AgcConfig_t stAgcCfg;
    MI_AUDIO_EqConfig_t  stEqCfg;
}MI_AO_VqeConfig_t;

typedef enum{
    E_MI_AUDIO_ADEC_TYPE_G711A = 0,
    E_MI_AUDIO_ADEC_TYPE_G711U,
    E_MI_AUDIO_ADEC_TYPE_G726,
    E_MI_AUDIO_ADEC_TYPE_INVALID,
}MI_AUDIO_AdecType_e;


typedef struct MI_AUDIO_AdecG711Config_s{
    MI_AUDIO_SampleRate_e eSamplerate;
    MI_AUDIO_SoundMode_e eSoundmode;
}MI_AUDIO_AdecG711Config_t;

typedef struct MI_AUDIO_AdecG726Config_s{
    MI_AUDIO_SampleRate_e eSamplerate;
    MI_AUDIO_SoundMode_e eSoundmode;
    MI_AUDIO_G726Mode_e eG726Mode;
}MI_AUDIO_AdecG726Config_t;

typedef struct MI_AO_AdecConfig_s{
    MI_AUDIO_AdecType_e  eAdecType;
    union
    {
        MI_AUDIO_AdecG711Config_t  stAdecG711Cfg;
        MI_AUDIO_AdecG726Config_t  stAdecG726Cfg;
    };
}MI_AO_AdecConfig_t;

typedef struct MI_AO_ChnGainConfig_s
{
    MI_BOOL bEnableGainSet;
    MI_S16 s16Gain;
}MI_AO_ChnGainConfig_t;

typedef struct MI_AO_ChnParam_s
{
    MI_AO_ChnGainConfig_t stChnGain;
    MI_U32 u32Reserved;
} MI_AO_ChnParam_t;

typedef struct MI_AO_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_AO_InitParam_t;

//=============================================================================
// Variable definition
//=============================================================================


//=============================================================================
// Global function definition
//=============================================================================


#ifdef __cplusplus
}
#endif

#endif //__MI_AO_DATATYPE_H__
