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
#ifndef _MI_AIO_DEP_DATATYPE_H_
#define _MI_AIO_DEP_DATATYPE_H_

#include "mi_sys_datatype.h"

//=============================================================================
// Include files
//=============================================================================


//=============================================================================
// Extern definition
//=============================================================================

//=============================================================================
// Macro definition
//=============================================================================
#define MI_AUDIO_MAX_CHN_NUM            (16)



//=============================================================================
// Data type definition
//=============================================================================
typedef enum
{
    E_MI_AUDIO_ALGORITHM_MODE_DEFAULT,
    E_MI_AUDIO_ALGORITHM_MODE_USER,
    E_MI_AUDIO_ALGORITHM_MODE_MUSIC,
    E_MI_AUDIO_ALGORITHM_MODE_INVALID,
}MI_AUDIO_AlgorithmMode_e;

typedef enum
{
    E_MI_AUDIO_SAMPLE_RATE_8000 = 8000,         /* 8kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_11025 = 11025,       /* 11.025kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_12000 = 12000,       /* 12kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_16000 = 16000,       /* 16kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_22050 = 22050,       /* 22.05kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_24000 = 24000,       /* 24kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_32000 = 32000,       /* 32kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_44100 = 44100,       /* 44.1kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_48000 = 48000,       /* 48kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_96000 = 96000,       /* 96kHz sampling rate */
    E_MI_AUDIO_SAMPLE_RATE_INVALID,
}MI_AUDIO_SampleRate_e;

typedef enum
{
    E_MI_AUDIO_BIT_WIDTH_16 =0,                 /* 16bit width */
    E_MI_AUDIO_BIT_WIDTH_24 =1,                 /* 24bit width */
    E_MI_AUDIO_BIT_WIDTH_MAX,
}MI_AUDIO_BitWidth_e;

typedef enum
{
    E_MI_AUDIO_MODE_I2S_MASTER,                /* I2S master mode */
    E_MI_AUDIO_MODE_I2S_SLAVE,                 /* I2S slave mode */
    E_MI_AUDIO_MODE_TDM_MASTER,                /* TDM master mode */
    E_MI_AUDIO_MODE_TDM_SLAVE,                 /* TDM slave mode */
    E_MI_AUDIO_MODE_MAX,
}MI_AUDIO_Mode_e;

typedef enum
{
    E_MI_AUDIO_SOUND_MODE_MONO = 0,           /* mono */
    E_MI_AUDIO_SOUND_MODE_STEREO = 1,         /* stereo */
    E_MI_AUDIO_SOUND_MODE_QUEUE = 2,          /* all data in One chn */
    E_MI_AUDIO_SOUND_MODE_MAX,
}MI_AUDIO_SoundMode_e;

typedef enum
{
    E_MI_AUDIO_HPF_FREQ_80 = 80,             /* 80Hz */
    E_MI_AUDIO_HPF_FREQ_120 = 120,           /* 120Hz */
    E_MI_AUDIO_HPF_FREQ_150 = 150,           /* 150Hz */
    E_MI_AUDIO_HPF_FREQ_INVALID,
}MI_AUDIO_HpfFreq_e;

typedef enum {
    E_MI_AUDIO_NR_SPEED_LOW,
    E_MI_AUDIO_NR_SPEED_MID,
    E_MI_AUDIO_NR_SPEED_HIGH
}MI_AUDIO_NrSpeed_e;

typedef enum
{
    E_MI_AUDIO_I2S_FMT_I2S_MSB,
    E_MI_AUDIO_I2S_FMT_LEFT_JUSTIFY_MSB,
}MI_AUDIO_I2sFmt_e;

typedef enum{
    E_MI_AUDIO_I2S_MCLK_0,                              //not enable
    E_MI_AUDIO_I2S_MCLK_12_288M,                        //12.288M
    E_MI_AUDIO_I2S_MCLK_16_384M,                        //16.384M
    E_MI_AUDIO_I2S_MCLK_18_432M,                        //18.432M
    E_MI_AUDIO_I2S_MCLK_24_576M,                        //24.576M
    E_MI_AUDIO_I2S_MCLK_24M,                            // 24MHz
    E_MI_AUDIO_I2S_MCLK_48M,                            // 48MHz
}MI_AUDIO_I2sMclk_e;

typedef struct MI_AUDIO_I2sConfig_s
{
    MI_AUDIO_I2sFmt_e eFmt;
    MI_AUDIO_I2sMclk_e eMclk;
    MI_BOOL bSyncClock;                                 //I2s Tx BCLK, Rx BCLK use the same clock source
}MI_AUDIO_I2sConfig_t;

typedef struct MI_AUDIO_Attr_s
{
    MI_AUDIO_SampleRate_e eSamplerate;                  /* sample rate */
    MI_AUDIO_BitWidth_e eBitwidth;                      /* bitwidth */
    MI_AUDIO_Mode_e eWorkmode;                          /* I2s master or slave mode */
    MI_AUDIO_SoundMode_e eSoundmode;                    /* mono or stereo */
    MI_U32 u32FrmNum;                                   /* frame num in buffer */
    MI_U32 u32PtNumPerFrm;                              /* number of samples */
    MI_U32 u32CodecChnCnt;                              /* channel number on Codec */
    MI_U32 u32ChnCnt;
    union{
        MI_AUDIO_I2sConfig_t stI2sConfig;
    }WorkModeSetting;

}MI_AUDIO_Attr_t;

typedef struct MI_AUDIO_Frame_s
{
    MI_AUDIO_BitWidth_e eBitwidth;                     /* audio frame bitwidth */
    MI_AUDIO_SoundMode_e eSoundmode;                   /* audio frame momo or stereo mode */
    void *apVirAddr[MI_AUDIO_MAX_CHN_NUM];
    MI_U64 u64TimeStamp;                               /* audio frame timestamp */
    MI_U32 u32Seq;                                     /* audio frame seq */
    MI_U32 u32Len;                                     /* data lenth per channel in frame */
    MI_U32 au32PoolId[2];
                                                       /* Only use for Ai */
    void *apSrcPcmVirAddr[MI_AUDIO_MAX_CHN_NUM];       /* Ai original pcm data from ai channel */
    MI_U32 u32SrcPcmLen;                               /* length of Ai original pcm data */

}MI_AUDIO_Frame_t;

typedef struct MI_AUDIO_AecFrame_s
{
    MI_AUDIO_Frame_t stRefFrame;                      /* aec reference audio frame */
    MI_BOOL bValid;                                   /* whether frame is valid */
}MI_AUDIO_AecFrame_t;

typedef struct MI_AUDIO_SaveFileInfo_s
{
    MI_BOOL bCfg;
    MI_U8 szFilePath[256];
    MI_U32 u32FileSize; /*in KB*/
}MI_AUDIO_SaveFileInfo_t;

typedef struct MI_AUDIO_HpfConfig_s
{
    MI_AUDIO_AlgorithmMode_e eMode;
    MI_AUDIO_HpfFreq_e eHpfFreq;                     /*freq to be processed*/
}MI_AUDIO_HpfConfig_t;

typedef struct MI_AUDIO_AnrConfig_s
{
    MI_AUDIO_AlgorithmMode_e eMode;
    MI_U32   u32NrIntensity;
    MI_U32   u32NrSmoothLevel;
    MI_AUDIO_NrSpeed_e eNrSpeed;
}MI_AUDIO_AnrConfig_t;

typedef struct AgcGainInfo_s{
    MI_S32   s32GainMax;
    MI_S32   s32GainMin;
    MI_S32   s32GainInit;
}AgcGainInfo_t;

typedef struct MI_AUDIO_AgcConfig_s
{
    MI_AUDIO_AlgorithmMode_e eMode;
    AgcGainInfo_t stAgcGainInfo;
    MI_U32     u32DropGainMax;
    MI_U32     u32AttackTime;
    MI_U32     u32ReleaseTime;
    MI_S16     s16Compression_ratio_input[5];
    MI_S16     s16Compression_ratio_output[5];
    MI_S32     s32TargetLevelDb;
    MI_S32     s32NoiseGateDb;
    MI_U32     u32NoiseGateAttenuationDb;
}MI_AUDIO_AgcConfig_t;

typedef struct MI_AUDIO_EqConfig_s
{
    MI_AUDIO_AlgorithmMode_e eMode;
    MI_S16     s16EqGainDb[129];
}MI_AUDIO_EqConfig_t;

typedef enum{
    E_MI_AUDIO_G726_MODE_16 = 0,
    E_MI_AUDIO_G726_MODE_24,
    E_MI_AUDIO_G726_MODE_32,
    E_MI_AUDIO_G726_MODE_40,
    E_MI_AUDIO_G726_MODE_INVALID,
}MI_AUDIO_G726Mode_e;

//=============================================================================
// Variable definition
//=============================================================================


//=============================================================================
// Global function definition
//=============================================================================


#endif // _MI_AIO_DEP_DATATYPE_H_
