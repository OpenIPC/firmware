/******************************************************************************

  Copyright (C), 2001-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_aio.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/5/5
  Description   :
  History       :
  1.Date        : 2009/5/5
    Author      : p00123320
    Modification: Created file
******************************************************************************/


#ifndef __HI_COMM_AIO_H__
#define __HI_COMM_AIO_H__

#include "hi_common.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define MAX_AUDIO_FRAME_NUM    300       /* max count of audio frame in Buffer */
#define MAX_AUDIO_POINT_BYTES  4        /* max bytes of one sample point(now 32bit max) */

#define MAX_VOICE_POINT_NUM    480      /* max sample per frame for voice encode */

#define MAX_AUDIO_POINT_NUM    2048     /* max sample per frame for all encoder(aacplus:2048) */
#define MAX_AO_POINT_NUM       4096     /* from h3;support 4096 framelen */
#define MIN_AUDIO_POINT_NUM    80       /* min sample per frame */
#define MAX_AI_POINT_NUM    2048     /* max sample per frame for all encoder(aacplus:2048) */

/* max length of audio frame by bytes, one frame contain many sample point */
#define MAX_AUDIO_FRAME_LEN    (MAX_AUDIO_POINT_BYTES*MAX_AO_POINT_NUM)

/* max length of audio stream by bytes */
#define MAX_AUDIO_STREAM_LEN   MAX_AUDIO_FRAME_LEN

#define MAX_AI_USRFRM_DEPTH     30      /* max depth of user frame buf */

#define MAX_AUDIO_FILE_PATH_LEN 256
#define MAX_AUDIO_FILE_NAME_LEN 256

/* The VQE EQ Band num. */
#define VQE_EQ_BAND_NUM  10
#define VQE_DRC_SECNUM 5

#define AI_RECORDVQE_MASK_HPF       0x1
#define AI_RECORDVQE_MASK_RNR       0x2
#define AI_RECORDVQE_MASK_HDR       0x4
#define AI_RECORDVQE_MASK_DRC       0x8
#define AI_RECORDVQE_MASK_EQ        0x10
#define AI_RECORDVQE_MASK_AGC       0x20

#define AI_TALKVQE_MASK_HPF     0x1
#define AI_TALKVQE_MASK_AEC     0x2
#define AI_TALKVQE_MASK_AGC     0x8
#define AI_TALKVQE_MASK_EQ      0x10
#define AI_TALKVQE_MASK_ANR     0x20

#define AO_VQE_MASK_HPF         0x1
#define AO_VQE_MASK_ANR         0x2
#define AO_VQE_MASK_AGC         0x4
#define AO_VQE_MASK_EQ          0x8


typedef enum hiAUDIO_SAMPLE_RATE_E {
    AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate */
    AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12K samplerate */
    AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate */
    AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate */
    AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate */
    AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate */
    AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate */
    AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate */
    AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate */
    AUDIO_SAMPLE_RATE_64000  = 64000,   /* 64K samplerate */
    AUDIO_SAMPLE_RATE_96000  = 96000,   /* 96K samplerate */
    AUDIO_SAMPLE_RATE_BUTT,
} AUDIO_SAMPLE_RATE_E;

typedef enum hiAUDIO_BIT_WIDTH_E {
    AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width */
    AUDIO_BIT_WIDTH_24  = 2,   /* 24bit width */
    AUDIO_BIT_WIDTH_BUTT,
} AUDIO_BIT_WIDTH_E;

typedef enum hiAIO_MODE_E {
    AIO_MODE_I2S_MASTER  = 0,   /* AIO I2S master mode */
    AIO_MODE_I2S_SLAVE,         /* AIO I2S slave mode */
    AIO_MODE_PCM_SLAVE_STD,     /* AIO PCM slave standard mode */
    AIO_MODE_PCM_SLAVE_NSTD,    /* AIO PCM slave non-standard mode */
    AIO_MODE_PCM_MASTER_STD,    /* AIO PCM master standard mode */
    AIO_MODE_PCM_MASTER_NSTD,   /* AIO PCM master non-standard mode */
    AIO_MODE_BUTT
} AIO_MODE_E;

typedef enum {
    AIO_I2STYPE_INNERCODEC = 0, /* AIO I2S connect inner audio CODEC */
    AIO_I2STYPE_INNERHDMI,      /* AIO I2S connect Inner HDMI */
    AIO_I2STYPE_EXTERN,         /* AIO I2S connect extern hardware */
} AIO_I2STYPE_E;

typedef enum hiAIO_SOUND_MODE_E {
    AUDIO_SOUND_MODE_MONO   = 0, /* mono */
    AUDIO_SOUND_MODE_STEREO = 1, /* stereo */
    AUDIO_SOUND_MODE_BUTT
} AUDIO_SOUND_MODE_E;

/*
An example of the packing scheme for G726-32 codewords is as shown, and bit A3 is the least significant bit of the first codeword:
RTP G726-32:
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|B B B B|A A A A|D D D D|C C C C| ...
|0 1 2 3|0 1 2 3|0 1 2 3|0 1 2 3|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

MEDIA G726-32:
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|A A A A|B B B B|C C C C|D D D D| ...
|3 2 1 0|3 2 1 0|3 2 1 0|3 2 1 0|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/
typedef enum hiG726_BPS_E {
    G726_16K = 0,       /* G726 16kbps, see RFC3551.txt  4.5.4 G726-16 */
    G726_24K,           /* G726 24kbps, see RFC3551.txt  4.5.4 G726-24 */
    G726_32K,           /* G726 32kbps, see RFC3551.txt  4.5.4 G726-32 */
    G726_40K,           /* G726 40kbps, see RFC3551.txt  4.5.4 G726-40 */
    MEDIA_G726_16K,     /* G726 16kbps for ASF ... */
    MEDIA_G726_24K,     /* G726 24kbps for ASF ... */
    MEDIA_G726_32K,     /* G726 32kbps for ASF ... */
    MEDIA_G726_40K,     /* G726 40kbps for ASF ... */
    G726_BUTT,
} G726_BPS_E;

typedef enum hiADPCM_TYPE_E {
    /* see DVI4 diiffers in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */
    ADPCM_TYPE_DVI4 = 0,    /* 32kbps ADPCM(DVI4) for RTP */
    ADPCM_TYPE_IMA,         /* 32kbps ADPCM(IMA),NOTICE:point num must be 161/241/321/481 */
    ADPCM_TYPE_ORG_DVI4,
    ADPCM_TYPE_BUTT,
} ADPCM_TYPE_E;

#define AI_EXPAND  0x01
#define AI_CUT     0x02

typedef struct hiAIO_ATTR_S {
    AUDIO_SAMPLE_RATE_E enSamplerate;   /* sample rate */
    AUDIO_BIT_WIDTH_E   enBitwidth;     /* bitwidth */
    AIO_MODE_E          enWorkmode;     /* master or slave mode */
    AUDIO_SOUND_MODE_E  enSoundmode;    /* momo or steror */
    HI_U32              u32EXFlag;      /* expand 8bit to 16bit, use AI_EXPAND(only valid for AI 8bit),use AI_CUT(only valid for extern Codec for 24bit) */
    HI_U32              u32FrmNum;      /* frame num in buf[2,MAX_AUDIO_FRAME_NUM] */
    HI_U32              u32PtNumPerFrm; /* point num per frame (80/160/240/320/480/1024/2048)
                                                (ADPCM IMA should add 1 point, AMR only support 160) */
    HI_U32              u32ChnCnt;      /* channle number on FS, valid value:1/2/4/8 */
    HI_U32              u32ClkSel;      /* 0: AI and AO clock is separate
                                                 1: AI and AO clock is inseparate, AI use AO's clock */
    AIO_I2STYPE_E       enI2sType;      /* i2s type */
} AIO_ATTR_S;

typedef struct hiAI_CHN_PARAM_S {
    HI_U32 u32UsrFrmDepth;
} AI_CHN_PARAM_S;

typedef struct hiAUDIO_FRAME_S {
    AUDIO_BIT_WIDTH_E   enBitwidth;     /* audio frame bitwidth */
    AUDIO_SOUND_MODE_E  enSoundmode;    /* audio frame momo or stereo mode */
    HI_U8*  u64VirAddr[2];
    HI_U64  u64PhyAddr[2];
    HI_U64  u64TimeStamp;                /* audio frame timestamp */
    HI_U32  u32Seq;                      /* audio frame seq */
    HI_U32  u32Len;                      /* data lenth per channel in frame */
    HI_U32  u32PoolId[2];
} AUDIO_FRAME_S;

typedef struct hiAEC_FRAME_S {
    AUDIO_FRAME_S   stRefFrame;    /* AEC reference audio frame */
    HI_BOOL         bValid;        /* whether frame is valid */
    HI_BOOL         bSysBind;      /* whether is sysbind */
} AEC_FRAME_S;


typedef struct hiAUDIO_FRAME_INFO_S {
    AUDIO_FRAME_S *pstFrame; /* frame ptr */
    HI_U32         u32Id;   /* frame id */
} AUDIO_FRAME_INFO_S;

typedef struct hiAUDIO_STREAM_S {
    HI_U8 ATTRIBUTE *pStream;         /* the virtual address of stream */
    HI_U64 ATTRIBUTE u64PhyAddr;      /* the physics address of stream */
    HI_U32 u32Len;          /* stream lenth, by bytes */
    HI_U64 u64TimeStamp;    /* frame time stamp */
    HI_U32 u32Seq;          /* frame seq, if stream is not a valid frame,u32Seq is 0 */
} AUDIO_STREAM_S;


typedef struct hiAO_CHN_STATE_S {
    HI_U32                  u32ChnTotalNum;    /* total number of channel buffer */
    HI_U32                  u32ChnFreeNum;     /* free number of channel buffer */
    HI_U32                  u32ChnBusyNum;     /* busy number of channel buffer */
} AO_CHN_STATE_S;

typedef enum hiAUDIO_TRACK_MODE_E {
    AUDIO_TRACK_NORMAL      = 0,
    AUDIO_TRACK_BOTH_LEFT   = 1,
    AUDIO_TRACK_BOTH_RIGHT  = 2,
    AUDIO_TRACK_EXCHANGE    = 3,
    AUDIO_TRACK_MIX         = 4,
    AUDIO_TRACK_LEFT_MUTE   = 5,
    AUDIO_TRACK_RIGHT_MUTE  = 6,
    AUDIO_TRACK_BOTH_MUTE   = 7,

    AUDIO_TRACK_BUTT
} AUDIO_TRACK_MODE_E;


typedef enum hiAUDIO_FADE_RATE_E {
    AUDIO_FADE_RATE_1   = 0,
    AUDIO_FADE_RATE_2   = 1,
    AUDIO_FADE_RATE_4   = 2,
    AUDIO_FADE_RATE_8   = 3,
    AUDIO_FADE_RATE_16  = 4,
    AUDIO_FADE_RATE_32  = 5,
    AUDIO_FADE_RATE_64  = 6,
    AUDIO_FADE_RATE_128 = 7,

    AUDIO_FADE_RATE_BUTT
} AUDIO_FADE_RATE_E;

typedef struct hiAUDIO_FADE_S {
    HI_BOOL         bFade;
    AUDIO_FADE_RATE_E enFadeInRate;
    AUDIO_FADE_RATE_E enFadeOutRate;
} AUDIO_FADE_S;

/* Defines the configure parameters of AEC. */
typedef struct hiAI_AEC_CONFIG_S {
    HI_BOOL bUsrMode;                            /* mode 0: auto mode 1: mannual. */
    HI_S8 s8CngMode;                             /* cozy noisy mode: 0 close, 1 open, recommend 1 */
    HI_S8 s8NearAllPassEnergy;                   /* the far-end energy threshold for judging whether unvarnished transmission: 0 -59dBm0,
    1 -49dBm0, 2 -39dBm0, recommend 1 */
    HI_S8 s8NearCleanSupEnergy;                  /* the energy threshold for compelling reset of near-end signal: 0 12dB,
    1 15dB, 2 18dB, recommend 2 */

    HI_S16 s16DTHnlSortQTh;                      /* the threshold of judging single or double talk, recommend 16384, [0, 32767] */

    HI_S16 s16EchoBandLow;                       /* voice processing band1,
    low frequency parameter, [1, 63) for 8k, [1, 127) for 16k, recommend 10 */
    HI_S16 s16EchoBandHigh;                      /* voice processing band1,
                                                 high frequency parameter, (s16EchoBandLow, 63] for 8k, (s16EchoBandLow, 127] for 16k, recommend 41 */
    /* s16EchoBandHigh must be greater than s16EchoBandLow */
    HI_S16 s16EchoBandLow2;                      /* voice processing band2,
    low frequency parameter, [1, 63) for 8k, [1, 127) for 16k, recommend 47 */
    HI_S16 s16EchoBandHigh2;                     /* voice processing band2,
                                                 high frequency parameter, (s16EchoBandLow2, 63] for 8k, (s16EchoBandLow2, 127] for 16k, recommend 72 */
    /* s16EchoBandHigh2 must be greater than s16EchoBandLow2 */

    HI_S16 s16ERLBand[6];                        /* ERL protect area,
                                                 [1, 63] for 8k, [1, 127] for 16k, frequency band calculated by s16ERLBand * 62.5 */
    /* besides, s16ERLBand[n+1] should be greater than s16ERLBand[n] */
    HI_S16 s16ERL[7];                            /* ERL protect value of ERL protect area,
    the smaller its value, the more strength its protect ability,[0, 18] */
    HI_S16 s16VioceProtectFreqL;                 /* protect area of near-end low frequency,
    [1, 63) for 8k, [1, 127) for 16k, recommend 3 */
    HI_S16 s16VioceProtectFreqL1;                /* protect area of near-end low frequency1,
    (s16VioceProtectFreqL, 63] for 8k, (s16VioceProtectFreqL, 127] for 16k, recommend 6 */
    HI_S32 s32Reserved;                          /* s16VioceProtectFreqL1 must be greater than s16VioceProtectFreqL */
} AI_AEC_CONFIG_S;

/* Defines the configure parameters of ANR. */
typedef struct hiAUDIO_ANR_CONFIG_S {
    HI_BOOL bUsrMode;   /* mode 0: auto, mode 1: manual. */

    HI_S16 s16NrIntensity;       /* noise reduce intensity, range: [0, 25] */
    HI_S16 s16NoiseDbThr;        /* noise threshold, range: [30, 60] */
    HI_S8  s8SpProSwitch;        /* switch for music probe, range: [0:close, 1:open] */

    HI_S32 s32Reserved;
} AUDIO_ANR_CONFIG_S;

/* Defines the configure parameters of AGC. */
typedef struct hiAUDIO_AGC_CONFIG_S {
    HI_BOOL bUsrMode;          /* mode 0: auto, mode 1: manual. */

    HI_S8 s8TargetLevel;       /* target voltage level, range: [-40, -1]dB */
    HI_S8 s8NoiseFloor;        /* noise floor, range: TalkVqe/AoVqe[-65, -20]dB, RecordVqe[-50, -20]dB */
    HI_S8 s8MaxGain;           /* max gain, range: [0, 30]dB */
    HI_S8 s8AdjustSpeed;       /* adjustable speed, range: [0, 10]dB/s */

    HI_S8 s8ImproveSNR;        /* switch for improving SNR, range: [0:close, 1:upper limit 3dB, 2:upper limit 6dB] */
    HI_S8 s8UseHighPassFilt;   /* switch for using high pass filt,
    range: [0:close, 1:80Hz, 2:120Hz, 3:150:Hz, 4:300Hz: 5:500Hz] */
    HI_S8 s8OutputMode;        /* output mode, mute when lower than noise floor, range: [0:close, 1:open] */
    HI_S16 s16NoiseSupSwitch;  /* switch for noise suppression, range: [0:close, 1:open] */


    HI_S32 s32Reserved;
} AUDIO_AGC_CONFIG_S;

/* Defines the configure parameters of HPF. */
typedef enum hiAUDIO_HPF_FREQ_E {
    AUDIO_HPF_FREQ_80   = 80,    /* 80Hz */
    AUDIO_HPF_FREQ_120  = 120,   /* 120Hz */
    AUDIO_HPF_FREQ_150  = 150,   /* 150Hz */
    AUDIO_HPF_FREQ_BUTT,
} AUDIO_HPF_FREQ_E;

typedef struct hiAUDIO_HPF_CONFIG_S {
    HI_BOOL bUsrMode;   /* mode 0: auto mode 1: mannual. */
    AUDIO_HPF_FREQ_E enHpfFreq; /* freq to be processed, value:HiFiVqe/TalkVqe/AoVqe(80Hz,120Hz,150Hz), RecordVqe(80Hz) */
} AUDIO_HPF_CONFIG_S;

typedef struct hiAI_RNR_CONFIG_S {
    HI_BOOL bUsrMode;                /* mode 0: auto, mode 1: mannual. */

    HI_S32  s32NrMode;               /* mode 0: floor noise; 1:ambient noise */

    HI_S32 s32MaxNrLevel;            /* max NR level range:[2,20]dB */

    HI_S32  s32NoiseThresh;          /* noise threshold, range:[-80, -20] */
} AI_RNR_CONFIG_S;

typedef struct hiAUDIO_EQ_CONFIG_S {
    HI_S8  s8GaindB[VQE_EQ_BAND_NUM];   /* EQ band, include 100, 200, 250, 350, 500, 800, 1.2k, 2.5k, 4k, 8k in turn,
                                        range:TalkVqe/AoVqe[-100, 20], RecordVqe[-50, 20] */
    HI_S32 s32Reserved;
} AUDIO_EQ_CONFIG_S;


/* Defines the configure parameters of UPVQE work state. */
typedef enum hiVQE_WORKSTATE_E {
    VQE_WORKSTATE_COMMON  = 0,   /* common environment, Applicable to the family of voice calls. */
    VQE_WORKSTATE_MUSIC   = 1,   /* music environment , Applicable to the family of music environment. */
    VQE_WORKSTATE_NOISY   = 2,   /* noisy environment , Applicable to the noisy voice calls.  */
} VQE_WORKSTATE_E;

/* Defines record type */
typedef enum hiVQE_RECORD_TYPE {
    VQE_RECORD_NORMAL        = 0,  /* <double micphone recording. */
    VQE_RECORD_BUTT,
} VQE_RECORD_TYPE;


/* HDR Set CODEC GAIN Function Handle type */
typedef HI_S32 (*pFuncGainCallBack)(HI_S32 s32SetGain);

typedef struct hiAI_HDR_CONFIG_S {
    HI_BOOL bUsrMode;               /* mode 0: auto mode 1: mannual. */

    HI_S32 s32MinGaindB;            /* the minimum of MIC(AI) CODEC gain, [0, 120] */
    HI_S32 s32MaxGaindB;            /* the maximum of MIC(AI) CODEC gain, [0, 120] */

    HI_S32 s32MicGaindB;            /* the current gain of MIC(AI) CODEC,[s32MinGaindB, s32MaxGaindB] */
    HI_S32 s32MicGainStepdB;        /* the step size of gain adjustment, [1, 3], recommemd 2 */
    pFuncGainCallBack pcallback;    /* the callback function pointer of CODEC gain adjustment */
} AI_HDR_CONFIG_S;

typedef struct hiAI_DRC_CONFIG_S {
    HI_BOOL bUsrMode;   /* enable user mode or not,default 0: disable user mode,1: user mode. */

    HI_S16  s16AttackTime;   /* time of signal change from large to small (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms */
    HI_S16  s16ReleaseTime;  /* time of signal change from small to large (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms */

    HI_S16 s16OldLevDb[VQE_DRC_SECNUM];  /* old curve level(dB), default[0, -472, -792, -960, -1280],range:[-1440, 0]ms,store from big to small,scale:Q4 */
    HI_S16 s16NewLevDb[VQE_DRC_SECNUM];  /* new curve level(dB), default[0, -174, -410, -608, -1021],range:[-1440, 0]ms,store from big to small,scale:Q4 */
} AI_DRC_CONFIG_S;


/* Defines the configure parameters of Record VQE. */
typedef struct hiAI_RECORDVQE_CONFIG_S {
    HI_U32              u32OpenMask;

    HI_S32              s32WorkSampleRate;  /* Sample Rate:16KHz/48KHz */
    HI_S32              s32FrameSample; /* VQE frame length:80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    HI_S32                s32InChNum;
    HI_S32                s32OutChNum;
    VQE_RECORD_TYPE       enRecordType;

    AUDIO_HPF_CONFIG_S  stHpfCfg;
    AI_RNR_CONFIG_S     stRnrCfg;
    AI_HDR_CONFIG_S     stHdrCfg;
    AI_DRC_CONFIG_S     stDrcCfg;
    AUDIO_EQ_CONFIG_S   stEqCfg;
    AUDIO_AGC_CONFIG_S  stAgcCfg;
} AI_RECORDVQE_CONFIG_S;

/* Defines the configure parameters of Talk VQE. */
typedef struct hiAI_TALKVQE_CONFIG_S {
    HI_U32              u32OpenMask;

    HI_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz. default: 8KHz */
    HI_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S  stHpfCfg;
    AI_AEC_CONFIG_S     stAecCfg;
    AUDIO_ANR_CONFIG_S  stAnrCfg;
    AUDIO_AGC_CONFIG_S  stAgcCfg;
    AUDIO_EQ_CONFIG_S   stEqCfg;
} AI_TALKVQE_CONFIG_S;

typedef struct hiAO_VQE_CONFIG_S {
    HI_U32              u32OpenMask;

    HI_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz/48KHz. default: 8KHz */
    HI_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S stHpfCfg;
    AUDIO_ANR_CONFIG_S stAnrCfg;
    AUDIO_AGC_CONFIG_S stAgcCfg;
    AUDIO_EQ_CONFIG_S  stEqCfg;
} AO_VQE_CONFIG_S;

/* Defines the module register configure of VQE. */
typedef struct hiVQE_MODULE_CONFIG_S {
    HI_VOID *pHandle;
} VQE_MODULE_CONFIG_S;

typedef struct hiAUDIO_VQE_REGISTER_S {
    VQE_MODULE_CONFIG_S stResModCfg;
    VQE_MODULE_CONFIG_S stHpfModCfg;
    VQE_MODULE_CONFIG_S stHdrModCfg;
    VQE_MODULE_CONFIG_S stGainModCfg;

    // Record VQE
    VQE_MODULE_CONFIG_S stRecordModCfg;

    // Talk VQE
    VQE_MODULE_CONFIG_S stAecModCfg;
    VQE_MODULE_CONFIG_S stAnrModCfg;
    VQE_MODULE_CONFIG_S stAgcModCfg;
    VQE_MODULE_CONFIG_S stEqModCfg;

    // HiFi VQE
    VQE_MODULE_CONFIG_S stRnrModCfg;
    VQE_MODULE_CONFIG_S stDrcModCfg;
    VQE_MODULE_CONFIG_S stPeqModCfg;
} AUDIO_VQE_REGISTER_S;

/* Defines the configure parameters of AI saving file. */
typedef struct hiAUDIO_SAVE_FILE_INFO_S {
    HI_BOOL     bCfg;
    HI_CHAR     aFilePath[MAX_AUDIO_FILE_PATH_LEN];
    HI_CHAR     aFileName[MAX_AUDIO_FILE_NAME_LEN];
    HI_U32      u32FileSize;  /* in KB */
} AUDIO_SAVE_FILE_INFO_S;

/* Defines whether the file is saving or not . */
typedef struct hiAUDIO_FILE_STATUS_S {
    HI_BOOL     bSaving;
} AUDIO_FILE_STATUS_S;

/* Defines audio clksel type */
typedef enum hiAUDIO_CLKSEL_E {
    AUDIO_CLKSEL_BASE       = 0,  /* <Audio base clk. */
    AUDIO_CLKSEL_SPARE,           /* <Audio spare clk. */

    AUDIO_CLKSEL_BUTT,
} AUDIO_CLKSEL_E;

/* Defines audio mode parameter. */
typedef struct hiAUDIO_MOD_PARAM_S {
    AUDIO_CLKSEL_E enClkSel;
} AUDIO_MOD_PARAM_S;


typedef enum hiEN_AIO_ERR_CODE_E {
    AIO_ERR_VQE_ERR        = 65, /* vqe error */
} EN_AIO_ERR_CODE_E;

/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_AIO_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_AIO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HI_ERR_AIO_NULL_PTR          HI_DEF_ERR(HI_ID_AIO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* operation is not supported by NOW */
#define HI_ERR_AIO_NOT_PERM          HI_DEF_ERR(HI_ID_AIO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* vqe  err */
#define HI_ERR_AIO_REGISTER_ERR      HI_DEF_ERR(HI_ID_AIO, EN_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)

/* invlalid device ID */
#define HI_ERR_AI_INVALID_DEVID     HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HI_ERR_AI_INVALID_CHNID     HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_AI_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HI_ERR_AI_NULL_PTR          HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_AI_NOT_CONFIG        HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_AI_NOT_SUPPORT       HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_AI_NOT_PERM          HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the devide is not enabled  */
#define HI_ERR_AI_NOT_ENABLED       HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define HI_ERR_AI_NOMEM             HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_AI_NOBUF             HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_AI_BUF_EMPTY         HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_AI_BUF_FULL          HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_AI_SYS_NOTREADY      HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define HI_ERR_AI_BUSY              HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* vqe  err */
#define HI_ERR_AI_VQE_ERR       HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)

/* invlalid device ID */
#define HI_ERR_AO_INVALID_DEVID     HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HI_ERR_AO_INVALID_CHNID     HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_AO_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HI_ERR_AO_NULL_PTR          HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_AO_NOT_CONFIG        HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_AO_NOT_SUPPORT       HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_AO_NOT_PERM          HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the devide is not enabled  */
#define HI_ERR_AO_NOT_ENABLED       HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define HI_ERR_AO_NOMEM             HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_AO_NOBUF             HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_AO_BUF_EMPTY         HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_AO_BUF_FULL          HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_AO_SYS_NOTREADY      HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define HI_ERR_AO_BUSY              HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* vqe err */
#define HI_ERR_AO_VQE_ERR       HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMM_AI_H__ */

