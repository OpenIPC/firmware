/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

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
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#define MAX_AUDIO_FRAME_NUM    50       /*max count of audio frame in Buffer */
#define MAX_AUDIO_POINT_BYTES  4        /*max bytes of one sample point(now 32bit max)*/

#define MAX_VOICE_POINT_NUM    480      /*max sample per frame for voice encode */

#define MAX_AUDIO_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/
#define MAX_AO_POINT_NUM       4096     /*31/21 AO support 4096 framelen*/
#define MIN_AUDIO_POINT_NUM    80       /*min sample per frame*/

/*max length of audio frame by bytes, one frame contain many sample point */
#define MAX_AUDIO_FRAME_LEN    (MAX_AUDIO_POINT_BYTES*MAX_AO_POINT_NUM)  

/*max length of audio stream by bytes */
#define MAX_AUDIO_STREAM_LEN   MAX_AUDIO_FRAME_LEN

#define MAX_AI_USRFRM_DEPTH     30      /*max depth of user frame buf */

/*The VQE EQ Band num.*/
#define VQE3_EQ_BAND_NUM 10


typedef enum hiAUDIO_SAMPLE_RATE_E 
{ 
    AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate*/
    AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12K samplerate*/    
    AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate*/
    AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate*/
    AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate*/
    AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate*/
    AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate*/
    AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate*/
    AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate*/
    AUDIO_SAMPLE_RATE_BUTT,
} AUDIO_SAMPLE_RATE_E; 

typedef enum hiAUDIO_BIT_WIDTH_E
{
    AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width*/
    AUDIO_BIT_WIDTH_32  = 2,   /* 32bit width*/
    AUDIO_BIT_WIDTH_BUTT,
} AUDIO_BIT_WIDTH_E;

typedef enum hiAIO_MODE_E
{
    AIO_MODE_I2S_MASTER  = 0,   /* SIO I2S master mode */
    AIO_MODE_I2S_SLAVE,         /* SIO I2S slave mode */
    AIO_MODE_PCM_SLAVE_STD,     /* SIO PCM slave standard mode */
    AIO_MODE_PCM_SLAVE_NSTD,    /* SIO PCM slave non-standard mode */
    AIO_MODE_PCM_MASTER_STD,    /* SIO PCM master standard mode */
    AIO_MODE_PCM_MASTER_NSTD,   /* SIO PCM master non-standard mode */
    AIO_MODE_BUTT    
} AIO_MODE_E;

typedef enum hiAIO_SOUND_MODE_E
{
    AUDIO_SOUND_MODE_MONO   =0,/*mono*/
    AUDIO_SOUND_MODE_STEREO =1,/*stereo*/
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
typedef enum hiG726_BPS_E
{
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

typedef enum hiADPCM_TYPE_E
{
    /* see DVI4 diiffers in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */
    
    ADPCM_TYPE_DVI4 = 0,    /* 32kbps ADPCM(DVI4) for RTP */
    ADPCM_TYPE_IMA,         /* 32kbps ADPCM(IMA),NOTICE:point num must be 161/241/321/481 */
    ADPCM_TYPE_ORG_DVI4,
    ADPCM_TYPE_BUTT,
} ADPCM_TYPE_E;

#define AI_EXPAND  0x01

typedef struct hiAIO_ATTR_S
{
    AUDIO_SAMPLE_RATE_E enSamplerate;   /* sample rate*/
    AUDIO_BIT_WIDTH_E   enBitwidth;     /* bitwidth*/
    AIO_MODE_E          enWorkmode;     /* master or slave mode*/
    AUDIO_SOUND_MODE_E  enSoundmode;    /* momo or steror*/
    HI_U32              u32EXFlag;      /* expand 8bit to 16bit,use AI_EXPAND(only valid for AI 8bit) */
    HI_U32              u32FrmNum;      /* frame num in buf[2,MAX_AUDIO_FRAME_NUM]*/
    HI_U32              u32PtNumPerFrm; /* point num per frame (80/160/240/320/480/1024/2048)
                                            (ADPCM IMA should add 1 point, AMR only support 160)*/
    HI_U32              u32ChnCnt;      /* channle number on FS, valid value:2/4/8/16 */
    HI_U32              u32ClkSel;      /* clock select, meaning is diffrent when master and slave mode,
                                                if sio slave mode:
                                                    0: AD and DA clock of codec is separate 
                                                    1: AD and DA clock of codec is inseparate 
                                                else sio master mode:
                                                    0:ACKOUT clock is from sio 0
                                                    1:ACKOUT clock is from sio 1/2
                                                    */
} AIO_ATTR_S;

typedef struct hiAI_CHN_PARAM_S
{
    HI_U32 u32UsrFrmDepth;
    HI_S32 s32Rev;
} AI_CHN_PARAM_S;

typedef struct hiAUDIO_FRAME_S
{ 
    AUDIO_BIT_WIDTH_E   enBitwidth;     /*audio frame bitwidth*/
    AUDIO_SOUND_MODE_E  enSoundmode;    /*audio frame momo or stereo mode*/
    HI_VOID *pVirAddr[2];
    HI_U32  u32PhyAddr[2];
    HI_U64  u64TimeStamp;                /*audio frame timestamp*/
    HI_U32  u32Seq;                      /*audio frame seq*/
    HI_U32  u32Len;                      /*data lenth per channel in frame*/
    HI_U32  u32PoolId[2];
} AUDIO_FRAME_S; 

typedef struct hiAEC_FRAME_S
{
    AUDIO_FRAME_S   stRefFrame;    /* AEC reference audio frame */
    HI_BOOL         bValid;        /* whether frame is valid */
	HI_BOOL         bSysBind;        /* whether is sysbind */
} AEC_FRAME_S;

typedef struct hiAUDIO_FRAME_COMBINE_S
{
    AUDIO_FRAME_S stFrm;                /* audio frame */
    AEC_FRAME_S   stRefFrm;             /* AEC reference audio frame */
} AUDIO_FRAME_COMBINE_S;

typedef struct hiAUDIO_FRAME_INFO_S
{
    AUDIO_FRAME_S *pstFrame;/*frame ptr*/
    HI_U32         u32Id;   /*frame id*/
} AUDIO_FRAME_INFO_S;

typedef struct hiAUDIO_STREAM_S 
{ 
    HI_U8 *pStream;         /* the virtual address of stream */ 
    HI_U32 u32PhyAddr;      /* the physics address of stream */
    HI_U32 u32Len;          /* stream lenth, by bytes */
    HI_U64 u64TimeStamp;    /* frame time stamp*/
    HI_U32 u32Seq;          /* frame seq,if stream is not a valid frame,u32Seq is 0*/
} AUDIO_STREAM_S;

typedef enum hiAUDIO_RESAMPLE_TYPE_E
{
    AUDIO_RESAMPLE_1X2 = 0x1,
    AUDIO_RESAMPLE_2X1 = 0x2,
    AUDIO_RESAMPLE_1X4 = 0x3,
    AUDIO_RESAMPLE_4X1 = 0x4,
    AUDIO_RESAMPLE_1X6 = 0x5,
    AUDIO_RESAMPLE_6X1 = 0x6,
    AUDIO_RESAMPLE_BUTT
} AUDIO_RESAMPLE_TYPE_E;

typedef struct hiAUDIO_RESAMPLE_ATTR_S
{
    HI_U32                  u32InPointNum;     /* input point number of frame */
    AUDIO_SAMPLE_RATE_E     enInSampleRate;     /* input sample rate */
    AUDIO_RESAMPLE_TYPE_E   enReSampleType; /* resample type */
} AUDIO_RESAMPLE_ATTR_S;

typedef struct hiAUDIO_RESAMPLE_ATTR_EX_S
{
    HI_U32                  u32InPointNum;      /* input point number of frame */
    AUDIO_SAMPLE_RATE_E     enInSampleRate;     /* input sample rate */
    AUDIO_SAMPLE_RATE_E     enOutSampleRate;    /* output sample rate */
} AUDIO_RESAMPLE_ATTR_EX_S;

typedef struct hiAO_CHN_STATE_S
{
    HI_U32                  u32ChnTotalNum;    /* total number of channel buffer */
    HI_U32                  u32ChnFreeNum;     /* free number of channel buffer */
    HI_U32                  u32ChnBusyNum;     /* busy number of channel buffer */
} AO_CHN_STATE_S;

typedef struct hiAIO_RESMP_INFO_S
{
    HI_BOOL                 bReSmpEnable;      /* resample enable or disable */
    HI_BOOL                 bReSmpEnableEx;    /*advanced resample enable or disable */
    AUDIO_RESAMPLE_ATTR_S   stResmpAttr;
    AUDIO_RESAMPLE_ATTR_EX_S   stResmpAttrEx;
} AIO_RESMP_INFO_S;

typedef struct hiAI_ANR_INFO_S
{
    HI_BOOL                 bAnrEnable;      /* noise reduce enable or disable */
} AI_ANR_INFO_S;

typedef enum hiAUDIO_AEC_MODE_E
{
    AUDIO_AEC_MODE_RECEIVER	 = 0,
    AUDIO_AEC_MODE_SPEAKER	 = 1,
    AUDIO_AEC_MODE_HEADPHONE = 2,
    
    AUDIO_AEC_MODE_BUTT
} AUDIO_AEC_MODE_E; 

/**Defines the configure parameters of ALC.*/
typedef struct hiAI_ALC_CONFIG_S
{
    HI_S32 s32MaxLev;         /*s32MaxLev£º[-23dBm0, -4dBm0]¡£default: -4dBm0*/
    HI_S32 s32MinLev;         /*s32MinLev£º [-23dBm0, -4dBm0]¡£default: -16dBm0*/
    HI_U32 u32MaxGain;        /*u32MaxGain£º[3dB,12dB]¡£default: 12dB*/
} AI_ALC_CONFIG_S;

/**Defines the configure parameters of AEC.*/
typedef struct hiAI_AEC_CONFIG_S
{
    AUDIO_AEC_MODE_E  	enAecMode;       /* AEC mode, default is speaker,   0:receiver  1:speaker  2:headphone */
    HI_S32 				s32Reserved;      
} AI_AEC_CONFIG_S;


/**Defines the configure parameters of ANR.*/
typedef struct hiAI_ANR_CONFIG_S
{
    HI_S32 s32Reserved;
} AI_ANR_CONFIG_S;

/**Defines the configure parameters of HPF.*/
typedef enum hiAUDIO_HPF_FREQ_E
{
    AUDIO_HPF_FREQ_80   = 80,    /* 80Hz */
    AUDIO_HPF_FREQ_120  = 120,   /* 120Hz */
    AUDIO_HPF_FREQ_150  = 150,   /* 150Hz */
    AUDIO_HPF_FREQ_BUTT,
} AUDIO_HPF_FREQ_E;

typedef struct hiAI_HPF_CONFIG_S
{
    AUDIO_HPF_FREQ_E enHpfFreq; /*freq to be processed*/
} AI_HPF_CONFIG_S;
typedef struct hiAI_HPF_ATTR_S
{
    AI_HPF_CONFIG_S stHpfCfg;
    HI_S32          bHpfOpen;
} AI_HPF_ATTR_S;
/**Defines the configure parameters of VQE.*/
typedef struct hiAI_VQE_CONFIG_S
{
    HI_S32              bAecOpen;     
    HI_S32              bAnrOpen;
    HI_S32              bAlcOpen;
    HI_S32              s32SampleRate;  /* Sample Rate£º8KHz/11.025K/12K/16KHz¡£default: 8KHz*/
    HI_S32              s32FrameSample; /* VQE frame length£º
                                         sample rate 8KHz:  VQE frame length: 80/160/240/320/400/480£¬ default: 160;
                                         sample rate 11.025K/12K/16KHz: VQE frame length: 160/320/480/960£¬ default: 160 */
    AI_AEC_CONFIG_S stAecCfg;
    AI_ANR_CONFIG_S stAnrCfg;
    AI_ALC_CONFIG_S stAlcCfg;
} AI_VQE_CONFIG_S;

typedef struct hiAI_VQE_INFO_S
{
    AI_VQE_CONFIG_S         stVqeConfig;     /*vqe config*/
    HI_BOOL                 bVqeEnable;      /* vqe enable or disable */
    HI_BOOL                 bHpfOpen;      /* vqe enable or disable */
} AI_VQE_INFO_S;

typedef enum hiAUDIO_VQE3_AEC_MODE_E
{
    AUDIO_VQE3_AEC_MODE_CLOSE	 = 0,
    AUDIO_VQE3_AEC_MODE_OPEN	     = 1,

    AUDIO_VQE3_AEC_MODE_BUTT
} AUDIO_VQE3_AEC_MODE_E;

/**Defines the configure parameters of AGC.*/
typedef struct hiAUDIO_VQE3_AGC_CONFIG_S
{
    HI_BOOL bUsrMode;          /* mode 0: auto£¬mode 1: manual.*/

    HI_S8 s8TargetLevel;       /* target voltage level, range: [-40, -1]dB */
    HI_S8 s8NoiseFloor;        /* noise floor, range: [-65, -20]dB */
    HI_S8 s8MaxGain;           /* max gain, range: [0, 30]dB */
    HI_S8 s8AdjustSpeed;       /* adjustable speed, range: [0, 10]dB/s */

    HI_S8 s8ImproveSNR;        /* switch for improving SNR, range: [0:close, 1:upper limit 3dB, 2:upper limit 6dB] */
    HI_S8 s8UseHighPassFilt;   /* switch for using high pass filt, range: [0:close, 1:80Hz, 2:120Hz, 3:150:Hz, 4:300Hz: 5:500Hz] */
    HI_S8 s8OutputMode;        /* output mode, mute when lower than noise floor, range: [0:close, 1:open] */
    HI_S16 s16NoiseSupSwitch;  /* switch for noise suppression, range: [0:close, 1:open] */
    

    HI_S32 s32Reserved;
} AUDIO_VQE3_AGC_CONFIG_S;

/**Defines the configure parameters of AEC.*/
typedef struct hiAI_VQE3_AEC_CONFIG_S
{
    HI_BOOL bUsrMode;
    HI_S8 s8CngMode;     /* cosy-noisy mode:0 close,1 open, default 1*/
    HI_S32 s32Reserved;
} AI_VQE3_AEC_CONFIG_S;

/**Defines the configure parameters of ANR.*/
typedef struct hiAUDIO_VQE3_ANR_CONFIG_S
{
    HI_BOOL bUsrMode;   /* mode 0: auto£¬mode 1: manual.*/

    HI_S16 s16NrIntensity;       /* noise reduce intensity, range: [0, 25] */
    HI_S16 s16NoiseDbThr;        /* noise threshold, range: [30, 60] */
    HI_S8  s8SpProSwitch;        /* switch for music probe, range: [0:close, 1:open] */
   
    HI_S32 s32Reserved;
} AUDIO_VQE3_ANR_CONFIG_S;

/**Defines the configure parameters of HPF.*/

typedef struct hiAUDIO_VQE3_HPF_CONFIG_S
{
    HI_BOOL bUsrMode;   /* mode 0: auto mode 1: mannual.*/
    AUDIO_HPF_FREQ_E enHpfFreq; /*freq to be processed*/
} AUDIO_VQE3_HPF_CONFIG_S;

typedef struct hiAI_VQE3_RNR_CONFIG_S
{
    HI_BOOL bUsrMode;                /* mode 0: auto£¬mode 1: mannual.*/

    HI_S32  s32NrMode;               /*mode 0: floor noise; 1:ambient noise */

    HI_S32 s32MaxNrLevel;           /*max NR level range:[2,20]dB*/

    HI_S32  s32NoiseThresh;         /*noise threshold, range:[-80, -20]*/                     
} AI_VQE3_RNR_CONFIG_S;

typedef struct hiAUDIO_VQE3_EQ_CONFIG_S
{
    HI_S8  s8GaindB[VQE3_EQ_BAND_NUM];   /*EQ band, include 100,200,250,350,500,800,1.2k,2.5k,4k,8k in turn, range:[-100, 20]*/
    HI_S32 s32Reserved;
} AUDIO_VQE3_EQ_CONFIG_S;


/**Defines the configure parameters of UPVQE work state.*/
typedef enum hiVQE3_WORKSTATE_E
{
    VQE3_WORKSTATE_COMMON  = 0,   /* common environment, Applicable to the family of voice calls. */
    VQE3_WORKSTATE_MUSIC   = 1,   /* music environment , Applicable to the family of music environment. */
    VQE3_WORKSTATE_NOISY   = 2,   /* noisy environment , Applicable to the noisy voice calls.  */
} VQE3_WORKSTATE_E;

/**Defines the configure parameters of VQE.*/
typedef struct hiAI_VQE3_CONFIG_S
{
    HI_S32              bHpfOpen;
	HI_S32              bAecOpen;     
    HI_S32              bAnrOpen;
    HI_S32              bAgcOpen;
    HI_S32              bEqOpen;

	HI_S32              s32InSampleRate;  /* Sample Rate£º8KHz-48KHz¡£*/
    HI_S32              s32WorkSampleRate;  /* Sample Rate£º8KHz/16KHz/48KHz¡£default: 8KHz*/
    HI_S32              s32FrameSample; /* VQE frame length£º 80-4096 */
    VQE3_WORKSTATE_E     enWorkstate;

                                       
    AUDIO_VQE3_HPF_CONFIG_S  stHpfCfg;
 	AI_VQE3_AEC_CONFIG_S     stAecCfg;
    AUDIO_VQE3_ANR_CONFIG_S  stAnrCfg;
    AUDIO_VQE3_AGC_CONFIG_S  stAgcCfg;  
    AUDIO_VQE3_EQ_CONFIG_S   stEqCfg;
} AI_VQE3_CONFIG_S;

typedef struct hiAO_VQE3_CONFIG_S
{
    HI_S32              bHpfOpen;
    HI_S32              bAnrOpen;      /*Anr and Rnr can't enable at the same time,Anr used in voice noise reducing*/
    HI_S32              bAgcOpen;
    HI_S32              bEqOpen;
    
	HI_S32              s32InSampleRate;  /* Sample Rate£º8KHz-48KHz¡£*/
    HI_S32              s32WorkSampleRate;  /* Sample Rate£º8KHz/16KHz/48KHz¡£default: 8KHz*/
    HI_S32              s32FrameSample;     /* VQE frame length£º 80-4096 */
    VQE3_WORKSTATE_E     enWorkstate;

    AUDIO_VQE3_HPF_CONFIG_S stHpfCfg;
    AUDIO_VQE3_ANR_CONFIG_S stAnrCfg;
    AUDIO_VQE3_AGC_CONFIG_S stAgcCfg;
    AUDIO_VQE3_EQ_CONFIG_S  stEqCfg;
} AO_VQE3_CONFIG_S;


typedef struct hiAI_VQE3_INFO_S
{
    HI_BOOL                 bVqeEnable;      /* vqe enable or disable */
	AI_VQE3_CONFIG_S        stAiVqeCfg;
} AI_VQE3_INFO_S;

typedef struct hiAO_VQE3_INFO_S
{
    HI_BOOL                 bVqeEnable;      /* vqe enable or disable */
	AO_VQE3_CONFIG_S        stAoVqeCfg;
} AO_VQE3_INFO_S;

/**Defines the configure parameters of AI saving file.*/
typedef struct hiAUDIO_SAVE_FILE_INFO_S
{
    HI_BOOL               bCfg;
    HI_CHAR  aFilePath[256];
    //AI_SAVE_FILE_COMMOND_E eSaveFileCommond;
    HI_U32 u32FileSize;  /*in KB*/
} AUDIO_SAVE_FILE_INFO_S;

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
/* system is not ready,had not initialed or loaded*/
#define HI_ERR_AI_SYS_NOTREADY      HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define HI_ERR_AI_BUSY              HI_DEF_ERR(HI_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

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
/* system is not ready,had not initialed or loaded*/
#define HI_ERR_AO_SYS_NOTREADY      HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define HI_ERR_AO_BUSY              HI_DEF_ERR(HI_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMM_AI_H__ */

