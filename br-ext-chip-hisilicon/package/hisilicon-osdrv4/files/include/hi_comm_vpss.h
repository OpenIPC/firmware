/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: Vpss common interface
 * Author: Hisilicon multimedia software group
 * Create: 2016/09/27
 */

#ifndef __HI_COMM_VPSS_H__
#define __HI_COMM_VPSS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VPSS_SHARPEN_GAIN_NUM        (32)
#define VPSS_AUTO_ISO_STRENGTH_NUM   (16)
#define VPSS_YUV_SHPLUMA_NUM         (32)
#define VPSS_3DNR_MAX_AUTO_PARAM_NUM (16)

#define HI_ERR_VPSS_NULL_PTR         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VPSS_NOTREADY         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VPSS_INVALID_DEVID    HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VPSS_INVALID_CHNID    HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VPSS_EXIST            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_VPSS_UNEXIST          HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VPSS_NOT_SUPPORT      HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VPSS_NOT_PERM         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VPSS_NOMEM            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VPSS_NOBUF            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VPSS_SIZE_NOT_ENOUGH  HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SIZE_NOT_ENOUGH)
#define HI_ERR_VPSS_ILLEGAL_PARAM    HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VPSS_BUSY             HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VPSS_BUF_EMPTY        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#define VPSS_INVALID_FRMRATE         -1
#define VPSS_CHN0                    0
#define VPSS_CHN1                    1
#define VPSS_CHN2                    2
#define VPSS_CHN3                    3
#define VPSS_INVALID_CHN             -1

typedef enum hiVPSS_NR_TYPE_E {
    VPSS_NR_TYPE_VIDEO         = 0,
    VPSS_NR_TYPE_SNAP          = 1,
    VPSS_NR_TYPE_VIDEO_SPATIAL = 2,
    VPSS_NR_TYPE_VIDEO_ENHANCE = 3,
    VPSS_NR_TYPE_BUTT
} VPSS_NR_TYPE_E;

typedef enum hiNR_MOTION_MODE_E {
    NR_MOTION_MODE_NORMAL          = 0,        /* normal */
    NR_MOTION_MODE_COMPENSATE      = 1,        /* motion compensate */
    NR_MOTION_MODE_BUTT
} NR_MOTION_MODE_E;

typedef struct hiVPSS_NR_ATTR_S {
    VPSS_NR_TYPE_E      enNrType;
    COMPRESS_MODE_E     enCompressMode;   /* RW; Reference frame compress mode */
    NR_MOTION_MODE_E    enNrMotionMode;   /* RW; NR motion compensate mode. */
} VPSS_NR_ATTR_S;

typedef struct hiVPSS_GRP_ATTR_S {
    /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192] | Hi3516CV500 = [64, 2304] |
        Hi3516AV300 = [64, 3840] | Hi3516DV300 = [64, 2688] | Hi3556V200 = [64, 4608] | Hi3559V200 = [64, 4608] |
        Hi3516EV200 = [64, 4096]; Width of source image. */
    HI_U32                     u32MaxW;
    /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192] | Hi3516CV500 = [64, 2304] |
        Hi3516AV300 = [64, 3840] | Hi3516DV300 = [64, 2688] | Hi3556V200 = [64, 4608] | Hi3559V200 = [64, 4608] |
        Hi3516EV200 = [64, 4096]; Height of source image. */
    HI_U32                     u32MaxH;
    PIXEL_FORMAT_E             enPixelFormat;     /* RW; Pixel format of source image. */
    DYNAMIC_RANGE_E            enDynamicRange;    /* RW; DynamicRange of source image. */
    FRAME_RATE_CTRL_S          stFrameRate;       /* Grp frame rate contrl. */
    HI_BOOL                    bNrEn;             /* RW;Range: [0, 1];  NR enable. */
    VPSS_NR_ATTR_S             stNrAttr;          /* RW; NR attr. */
} VPSS_GRP_ATTR_S;

typedef enum hiVPSS_CHN_MODE_E {
    VPSS_CHN_MODE_USER  = 0,       /* User mode. */
    VPSS_CHN_MODE_AUTO  = 1        /* Auto mode. */

} VPSS_CHN_MODE_E;

typedef struct hiVPSS_CHN_ATTR_S {
    VPSS_CHN_MODE_E     enChnMode;          /* RW; Vpss channel's work mode. */
    /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192] | Hi3516CV500 = [64, 8192] |
        Hi3516AV300 = [64, 8192] | Hi3516DV300 = [64, 8192] | Hi3556V200 = [64, 8192] | Hi3559V200 = [64, 8192] |
        Hi3516EV200 = [64, 4096]; Width of target image. */
    HI_U32              u32Width;
    /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192] | Hi3516CV500 = [64, 8192] |
        Hi3516AV300 = [64, 8192] | Hi3516DV300 = [64, 8192] | Hi3556V200 = [64, 8192] | Hi3559V200 = [64, 8192] |
        Hi3516EV200 = [64, 4096]; Height of target image. */
    HI_U32              u32Height;
    VIDEO_FORMAT_E      enVideoFormat;      /* RW; Video format of target image. */
    PIXEL_FORMAT_E      enPixelFormat;      /* RW; Pixel format of target image. */
    DYNAMIC_RANGE_E     enDynamicRange;     /* RW; DynamicRange of target image. */
    COMPRESS_MODE_E     enCompressMode;     /* RW; Compression mode of the output. */
    FRAME_RATE_CTRL_S   stFrameRate;        /* Frame rate control info */
    HI_BOOL             bMirror;            /* RW; Mirror enable. */
    HI_BOOL             bFlip;              /* RW; Flip enable. */
    HI_U32              u32Depth;           /* RW; Range: [0, 8]; User get list depth. */
    ASPECT_RATIO_S      stAspectRatio;      /* Aspect Ratio info. */
} VPSS_CHN_ATTR_S;

typedef enum hiVPSS_CROP_COORDINATE_E {
    VPSS_CROP_RATIO_COOR = 0,   /* Ratio coordinate. */
    VPSS_CROP_ABS_COOR          /* Absolute coordinate. */
} VPSS_CROP_COORDINATE_E;

typedef struct hiVPSS_CROP_INFO_S {
    HI_BOOL                 bEnable;            /* RW; Range: [0, 1];  CROP enable. */
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /* RW;  Range: [0, 1]; Coordinate mode of the crop start point. */
    RECT_S                  stCropRect;         /* CROP rectangular. */
} VPSS_CROP_INFO_S;

/* Only used for Hi3519AV100/Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct hiVPSS_LDC_ATTR_S {
    HI_BOOL     bEnable;                        /* RW; Range: [0, 1]; Whether LDC is enbale */
    LDC_ATTR_S  stAttr;
} VPSS_LDC_ATTR_S;

/* Only used for Hi3516EV200 */
typedef struct hiVPSS_LDCV3_ATTR_S {
    HI_BOOL     bEnable;                        /* RW;Whether LDC is enbale */
    LDCV3_ATTR_S  stAttr;
} VPSS_LDCV3_ATTR_S;

typedef struct hiVPSS_ROTATION_EX_ATTR_S {
    HI_BOOL       bEnable;                      /* Whether ROTATE_EX_S is enbale */
    ROTATION_EX_S stRotationEx;                 /* Rotate Attribute */
} VPSS_ROTATION_EX_ATTR_S;

typedef struct hiVPSS_LOW_DELAY_INFO_S {
    HI_BOOL bEnable;          /* RW; Low delay enable. */
    HI_U32 u32LineCnt;        /* RW; Range: [16, 16384]; Low delay shoreline. */
} VPSS_LOW_DELAY_INFO_S;

typedef struct hiVPSS_EXT_CHN_ATTR_S {
    /* RW; Range: [0, 3]; Channel bind to. */
    VPSS_CHN           s32BindChn;
    /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192] | Hi3516CV500 = [64, 8192] |
        Hi3516AV300 = [64, 8192] | Hi3516DV300 = [64, 8192] | Hi3556V200 = [64, 8192] | Hi3559V200 = [64, 8192] |
        Hi3516EV200 = [64, 4096]; Width of target image. */
    HI_U32             u32Width;
    /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192] | Hi3516CV500 = [64, 8192] |
        Hi3516AV300 = [64, 8192] | Hi3516DV300 = [64, 8192] | Hi3556V200 = [64, 8192] | Hi3559V200 = [64, 8192] |
        Hi3516EV200 = [64, 4096]; Height of target image. */
    HI_U32             u32Height;
    /* RW; Video format of target image. */
    VIDEO_FORMAT_E     enVideoFormat;
    /* RW; Pixel format of target image. */
    PIXEL_FORMAT_E     enPixelFormat;
    /* RW; Dynamic range. */
    DYNAMIC_RANGE_E    enDynamicRange;
    /* RW; Compression mode of the output. */
    COMPRESS_MODE_E    enCompressMode;
    /* RW; Range: [0, 8]; User get list depth. */
    HI_U32             u32Depth;
    /* Frame rate control info */
    FRAME_RATE_CTRL_S  stFrameRate;
} VPSS_EXT_CHN_ATTR_S;

/* Only used for Hi3559AV100/Hi3519AV100 */
typedef struct hiVPSS_GRP_SHARPEN_MANUAL_ATTR_S {
    /* RW; Range: [0, 4095]; Undirectional sharpen strength for texture and detail enhancement */
    HI_U16 au16TextureStr[VPSS_SHARPEN_GAIN_NUM];
    /* RW; Range: [0, 4095]; Directional sharpen strength for edge enhancement */
    HI_U16 au16EdgeStr[VPSS_SHARPEN_GAIN_NUM];
    /* RW; Range: [0, 4095]; Texture frequency adjustment. Texture and detail will be finer when it increase */
    HI_U16 u16TextureFreq;
    /* RW; Range: [0, 4095]; Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    HI_U16 u16EdgeFreq;
    /* RW; Range: [0, 127]; u8OvershootAmt */
    HI_U8  u8OverShoot;
    /* RW; Range: [0, 127]; u8UndershootAmt */
    HI_U8  u8UnderShoot;
    /* RW; Range: [0, 255]; overshoot and undershoot suppression strength,
        the amplitude and width of shoot will be decrease when shootSupSt increase */
    HI_U8  u8ShootSupStr;
    /* RW; Range: [0, 255]; Different sharpen strength for detail and edge.
        When it is bigger than 128, detail sharpen strength will be stronger than edge. */
    HI_U8  u8DetailCtrl;
} VPSS_GRP_SHARPEN_MANUAL_ATTR_S;

/* Only used for Hi3559AV100/Hi3519AV100 */
typedef struct hiVPSS_GRP_SHARPEN_AUTO_ATTR_S {
    /* RW; Range: [0, 4095]; Undirectional sharpen strength for texture and detail enhancement */
    HI_U16 au16TextureStr[VPSS_SHARPEN_GAIN_NUM][VPSS_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range: [0, 4095]; Directional sharpen strength for edge enhancement */
    HI_U16 au16EdgeStr[VPSS_SHARPEN_GAIN_NUM][VPSS_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range: [0, 4095]; Texture frequency adjustment. Texture and detail will be finer when it increase */
    HI_U16 au16TextureFreq[VPSS_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range: [0, 4095]; Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    HI_U16 au16EdgeFreq[VPSS_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range: [0, 127]; u8OvershootAmt */
    HI_U8  au8OverShoot[VPSS_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range: [0, 127]; u8UndershootAmt */
    HI_U8  au8UnderShoot[VPSS_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range: [0, 255]; overshoot and undershoot suppression strength,
        the amplitude and width of shoot will be decrease when shootSupSt increase */
    HI_U8  au8ShootSupStr[VPSS_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range: [0, 255]; Different sharpen strength for detail and edge.
        When it is bigger than 128, detail sharpen strength will be stronger than edge. */
    HI_U8  au8DetailCtrl[VPSS_AUTO_ISO_STRENGTH_NUM];
} VPSS_GRP_SHARPEN_AUTO_ATTR_S;

/* Only used for Hi3559AV100/Hi3519AV100 */
typedef struct hiVPSS_GRP_SHARPEN_ATTR_S {
    /* RW;  Range: [0, 1];Sharpen enable. */
    HI_BOOL                         bEnable;
    /* RW; Sharpen Operation mode. */
    OPERATION_MODE_E                enOpType;
    /* RW; Range: [0, 127]; sharpen weight based on loacal luma */
    HI_U8                           au8LumaWgt[VPSS_YUV_SHPLUMA_NUM];
    /* RW; Sharpen manual attribute */
    VPSS_GRP_SHARPEN_MANUAL_ATTR_S  stSharpenManualAttr;
    /* RW; Sharpen auto attribute */
    VPSS_GRP_SHARPEN_AUTO_ATTR_S    stSharpenAutoAttr;
} VPSS_GRP_SHARPEN_ATTR_S;

/* Only used for Hi3519AV100 */
typedef struct {
    /* IES0~4 ; Range: [0, 255]; The gains of edge and texture enhancement. 0~3 for different frequency response. */
    HI_U8  IES0, IES1, IES2, IES3;
    /* IEDZ   ; Range: [0, 999]; The threshold to control the generated artifacts. */
    HI_U16 IEDZ : 10, _rb_ : 6;
} tV56aIEy;

/* Only used for Hi3519AV100 */
typedef struct {
    /* SPN6; Range: [0,   4];  The selection of filters to be mixed for NO.6 filter. */
    /* SFR ; Range: [0,  31];  The relative NR strength to the SFi and SFk filter. */
    HI_U8  SPN6 : 3, SFR  : 5;
    /* SBN6; Range: [0,   4];  The selection of filters to be mixed for NO.6 filter. */
    /* PBR6; Range: [0,  16];  The mix ratio between SPN6 and SBN6. */
    HI_U8  SBN6 : 3, PBR6 : 5;
    /* SFS2, SFT2, SBR2; Range: [0, 255];  The NR strength parameters for NO.1 and NO.2 filters. */
    HI_U8  SFS2, SFT2, SBR2;
    /* SFS4, SFT4, SBR4; Range: [0, 255];  The NR strength parameters for NO.3 and NO.4 filters. */
    HI_U8  SFS4, SFT4, SBR4;

    /* STH1~3; Range: [0, 511]; The thresholds for protection of edges from blurring */
    /* SFN0~3; Range: [0,   6]; Filter selection for different image areas based on STH1~3. */
    /* NRyEn ; Range: [0,   1]; The NR switches */
    HI_U16 STH1 : 9,  SFN1 : 3, SFN0  : 3, NRyEn : 1;
    /* BWSF4 ; Range: [0,   1]; The NR window size for the NO.3 and NO.4 filters.  */
    /* KMode ; Range: [0,   3]; a. Selection of SFi and SFk type filter. b.
        The denoise mode based on image brightness. */
    HI_U16 STH2 : 9,  SFN2 : 3, BWSF4 : 1, kMode : 3;
    /* tEdge ; Range: [0,   3]; NR strength control mode for the image background */
    /* Trith ; Range: [0,   1]; The switch to choose 3 STH threshold or 2 STH threshold */
    HI_U16 STH3 : 9,  SFN3 : 3, tEdge : 2, TriTh : 1, _rb_  : 1;
} tV56aSFy;

/* Only used for Hi3519AV100 */
typedef struct {
    /* MADZ;   Range: [0, 999]; The blending ratio between MAI2 and MAI1 based on image statistics. */
    /* MAI0~2; Range: [0,   3]; The three blending results between spatial and temporal filtering. */
    HI_U16 MADZ : 10, MAI0 : 2, MAI1 : 2,  MAI2 : 2;
    /* MADK;   Range: [0, 255]; The blending ratio between MAI2 and MAI1 based on brightness. (low limit). */
    /* MABR;   Range: [0, 255]; The blending ratio between MAI2 and MAI1 based on brightness. (high limit). */
    HI_U8  MADK,      MABR;
    /* MATH;   Range: [0, 999]; The theshold for motion detection. */
    /* MATE;   Range: [0,   8]; The motion index for smooth image area. */
    /* MATW;   Range: [0,   3]; The motion index for prevention of motion ghost. */
    HI_U16 MATH : 10, MATE : 4, MATW : 2;
    /* MASW;   Range: [0,  15]; The motion index for low-frequency noises. */
    /* MABW;   Range: [0,   4]; The window size for motion detection. */
    /* MAXN;   Range: [0,   1]; Not for tunning. */
    HI_U8  MASW :  4, MABW : 3, MAXN : 1, _rB_;
} tV56aMDy;

/* Only used for Hi3519AV100 */
typedef struct {
    /* TFS;    Range: [0,  15]; The NR strength for temporal filtering. */
    /* TDZ;    Range: [0, 999]; Protection of the weak texture area from temporal filtering.  */
    /* TDX;    Range: [0,   3]; Not for tuning. */
    HI_U16 TFS : 4,  TDZ : 10, TDX : 2;
    /* TFR[5]; Range: [0,  31]; The temoproal NR strength control for background (static) area. */
    /* TSS;    Range: [0,  15]; The ratio for blending spatial NR with the temproal NR results. */
    /* TSI;    Range: [0,   1]; The selection of blending filter for TSS. */
    HI_U8  TFR[5],   TSS : 4,  TSI : 1, _rb_ : 2;
    /* SDZ;    Range: [0, 999]; The threshold of NR control for result MAI1. */
    /* STR;    Range: [0,  31]; The strength of NR control for result MAI1.  */
    /* bRef;   Range: [0,   1]; The switch for temproal filtering.  */
    HI_U16 SDZ : 10, STR : 5,  bRef : 1;
} tV56aTFy;

/* Only used for Hi3519AV100 */
typedef struct {
    /* SFC;    Range: [0, 255]; Spatial NR strength for the first level. */
    /* TFC;    Range: [0,  63]; Temporal NR strength. */
    HI_U8  SFC, _rb_ : 2, TFC : 6;
    /* CSFS;   Range: [0, 999]; Spatial NR strength for the second level. */
    /* CSFR;   Range: [0,  32]; Spatial NR strength control. */
    HI_U16 CSFS : 10,    CSFR : 6;
    /* CTFR;   Range: [0, 999]; Temporal NR strength control. */
    /* CTFS;   Range: [0,  15]; Temporal NR filtering strength */
    /* CIIR;   Range: [0,   1]; Spatial NR mode for the first level. */
    HI_U16 CTFR : 11,    CTFS : 4,    CIIR : 1;
} tV56aNRc;

/* Only used for Hi3519AV100 */
typedef struct {
    tV56aIEy IEy[2];
    tV56aSFy SFy[4];
    tV56aMDy MDy[2];
    tV56aTFy TFy[2];
    tV56aNRc NRc;

    /* SBSk2[32], SDSk2[32]; Range [0, 8192];  Spatial NR strength based on brightness. */
    HI_U16 SBSk2[32], SDSk2[32];
    /* SBSk3[32], SDSk3[32]; Range [0, 8192];  Spatial NR strength based on brightness. */
    HI_U16 SBSk3[32], SDSk3[32];
} VPSS_NRX_V1_S;

/* Only used for Hi3519AV100 */
typedef struct hiVPSS_NRX_PARAM_MANUAL_V1_S {
    VPSS_NRX_V1_S stNRXParam;
} VPSS_NRX_PARAM_MANUAL_V1_S;

/* Only used for Hi3519AV100 */
typedef struct hiVPSS_NRX_PARAM_AUTO_V1_S {
    HI_U32 u32ParamNum;
    HI_U32 *pau32ISO;
    VPSS_NRX_V1_S *pastNRXParam;
} VPSS_NRX_PARAM_AUTO_V1_S;

/* Only used for Hi3519AV100 */
typedef struct hiVPSS_NRX_PARAM_V1_S {
    OPERATION_MODE_E           enOptMode;           /* RW;Adaptive NR */
    VPSS_NRX_PARAM_MANUAL_V1_S stNRXManual;         /* RW;NRX V1 param for manual video */
    VPSS_NRX_PARAM_AUTO_V1_S   stNRXAuto;           /* RW;NRX V1 param for auto video */
} VPSS_NRX_PARAM_V1_S;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    /* IES0~4 ; Range: [0, 255]; The gains of edge and texture enhancement. 0~3 for different frequency response. */
    HI_U8  IES0, IES1, IES2, IES3;
    HI_U16 IEDZ : 10, _rb_ : 6;     /* IEDZ   ; Range: [0, 999]; The threshold to control the generated artifacts. */
} tV500_VPSS_IEy;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    /* SPN6; Range: [0,   5];  The selection of filters to be mixed for NO.6 filter. */
    /* SFR ; Range: [0,  31];  The relative NR strength in the SFi and SFk filter. */
    /* SBN6; Range: [0,   5];  The selection of filters to be mixed for NO.6 filter. */
    /* PBR6; Range: [0,  16];  The mix ratio between SPN6 and SBN6. */
    HI_U8  SPN6 : 3, SFR  : 5;
    HI_U8  SBN6 : 3, PBR6 : 5;

    /* JMODE;      Range: [0,   4]; The selection modes for the blending of spatial filters */
    /* STR0, STR1; Range: [0,  16]; The blending ratio of different filters. (Used in serial filtering mode (SFM).) */
    /* DeIdx;      Range: [3,   6]; The selection number of filters that textures and details will be added to. */
    HI_U16 SRT0 : 5, SRT1 : 5, JMODE : 3,  DeIdx : 3;
    /* DeRate;     Range: [0, 255]; The enhancement strength for the SFM (When DeRate > 0, the SFM will be activated) */
    /* SFR6;       Range: [0,  31]; The relative NR strength for NO.6 filter. (Effective when JMODE = 4) */
    HI_U8  DeRate, SFR6[3];

    /* SFS1, SFT1, SBR1; Range: [0, 255];  The NR strength parameters for NO.1 filter. */
    /* SFS2, SFT2, SBR2; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    /* SFS4, SFT4, SBR4; Range: [0, 255];  The NR strength parameters for NO.3 and NO.4 filters. */
    HI_U8  SFS1,  SFT1,  SBR1;
    HI_U8  SFS2,  SFT2,  SBR2;
    HI_U8  SFS4,  SFT4,  SBR4;

    /* STH1~3; Range: [0, 511]; The thresholds for protection of edges from blurring */
    /* NRyEn ; Range: [0,   1]; The NR switches */
    /* SFN0~3; Range: [0,   6]; Filter selection for different image areas based on STH1~3. */
    /* BWSF4 ; Range: [0,   1]; The NR window size for the NO.3 and NO.4 filters.  */
    /* KMode ; Range: [0,   3]; The denoise mode based on image brightness. */
    /* Trith ; Range: [0,   1]; The switch to choose 3 STH threshold or 2 STH threshold */
    HI_U16 STH1 : 9, SFN1 : 3, SFN0  : 3, NRyEn : 1;
    HI_U16 STH2 : 9, SFN2 : 3, BWSF4 : 1, kMode : 3;
    HI_U16 STH3 : 9, SFN3 : 3, TriTh : 1, _rb0_ : 3;

    /* SBSk[32], SDSk[32]; Range [0, 8191];  Spatial NR strength based on brightness. */
    HI_U16 SBSk[32], SDSk[32];
} tV500_VPSS_SFy;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    /* MADZ0, MADZ1;     Range: [0, 511]; The blending ratio between MAI2 and MAI1 based on image statistics. */
    /* MAI00~02,MAI10~12 Range: [0,   3]; The three blending results between spatial and temporal filtering. */
    /* MABR0, MABR1;  Range: [0, 255]; The blending ratio between MAI2 and MAI1 based on brightness.  */
    /* biPath;           Range: [0,   1]; The switch for single path or dual path. 0: single path; 1: dual path. */
    HI_U16 MADZ0   : 9,  MAI00    : 2,  MAI01  : 2,  MAI02    : 2,  biPath  : 1;
    HI_U16 MADZ1   : 9,  MAI10    : 2,  MAI11  : 2,  MAI12    : 2,  _rb0_ : 1;
    HI_U8  MABR0, MABR1;

    /* MATH0,MATH1;   Range: [0, 999]; The theshold for motion detection. */
    /* MATE0,MATE1;   Range: [0,   8]; The motion index for smooth image area. */
    /* MATW;   Range: [0,   3]; The motion index for prevention of motion ghost. */
    HI_U16 MATH0   : 10,  MATE0   : 4,  MATW   : 2;
    HI_U16 MATH1   : 10,  MATE1   : 4,  _rb1_  : 2;


    /* MASW;   Range: [0,  15]; The motion index for low-frequency noises. */
    /* MABW0,MABW1;   Range: [0,   9]; The window size for motion detection. */
    HI_U8  MASW    :  4,  _rb2_   : 4;
    HI_U8  MABW0   :  4,  MABW1   : 4;
} tV500_VPSS_MDy;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    /* TFS0,TFS1;        Range: [0,  15]; The NR strength for temporal filtering. */
    /* TDZ0,TDZ1;        Range: [0, 999]; Protection of the weak texture area from temporal filtering.  */
    /* TDX0,TDX1;        Range: [0,   3]; Not for tuning. */
    HI_U16 TFS0  :  4, TDZ0   : 10, TDX0    : 2;
    HI_U16 TFS1  :  4, TDZ1   : 10, TDX1    : 2;

    /* SDZ0,SDZ1;        Range: [0, 999]; The threshold of NR control for result MAI1. */
    /* STR0,STR1;        Range: [0,  31]; The strength of NR control for result MAI1.  */
    /* DZMode0, DZMode1; Range: [0,   1]; The selection mode for TDZ0 and TDZ1, respectively.  */
    HI_U16 SDZ0  : 10, STR0   : 5,  DZMode0 : 1;
    HI_U16 SDZ1  : 10, STR1   : 5,  DZMode1 : 1;

    /* TFR0,TFR1;        Range: [0,  31]; The temoproal NR strength control for background (static) area. */
    /* TSS0,TSS1;        Range: [0,  15]; The ratio for blending spatial NR with the temproal NR results. */
    /* TSI0,TSI1;        Range: [0,   1]; The selection of blending filter for TSS. */
    HI_U8  TFR0[6],    TSS0   : 4,  TSI0    : 4;
    HI_U8  TFR1[6],    TSS1   : 4,  TSI1    : 4;

    /* tEdge;            Range: [0,   3]; NR strength control mode for the updating background. */
    /* RFI;              Range: [0,   4]; Reference mode. (used in when NR_MOTION_MODE_COMPENSATE are selected). */
    /* bRef;             Range: [0,   1]; The Switch for temproal filtering. */
    HI_U8  RFI   : 3,  tEdge  : 2,  bRef    : 1,  _rb_ : 2;
} tV500_VPSS_TFy;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    /* advMATH;        Range: [0,   1]; The Switch for advanced motion dection.  */
    /* RFUI;           Range: [0,   4]; The modes for updating reference for NRy leve 2,
        (used in when NR_MOTION_MODE_COMPENSATE are selected). */
    HI_U16 advMATH : 1, RFDZ  : 9,    _rb_ : 6;
    /* RFDZ;           Rnage: [0, 511]; The threshold for RFI0 and RFI1 mode 3 and 4. */
    /* RFSLP;          Rnage: [0,  31]; The Strength for RFI0 and RFI1 mode 3 and 4. */
    HI_U8  RFUI    : 3, RFSLP : 5;
} tV500_VPSS_RFs;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    tV500_VPSS_IEy  IEy;
    tV500_VPSS_SFy  SFy;
    HI_U8 NRcEn : 1, _rb_ : 7;
} tV500_VPSS_NRc;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    /* SFC;    Range: [0, 255]; Spatial NR strength. */
    HI_U8  SFC;
    /* TFC;    Range: [0,  63]; Temporal NR strength relative to Spatial NR. */
    /* CTFS;   Range: [0,  15]; Absolute temporal NR strength. */
    HI_U16 CTFS : 4, TFC : 6, _rb_ : 6;

} tV500_VPSS_pNRc;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct {
    tV500_VPSS_IEy  IEy[3];
    tV500_VPSS_SFy  SFy[3];
    tV500_VPSS_MDy  MDy[2];
    tV500_VPSS_RFs  RFs;
    tV500_VPSS_TFy  TFy[2];
    tV500_VPSS_pNRc pNRc;
    tV500_VPSS_NRc  NRc;
} VPSS_NRX_V2_S;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct hiVPSS_NRX_PARAM_MANUAL_V2_S {
    VPSS_NRX_V2_S stNRXParam;
} VPSS_NRX_PARAM_MANUAL_V2_S;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct hiVPSS_NRX_PARAM_AUTO_V2_S {
    HI_U32 u32ParamNum;
    HI_U32 *pau32ISO;
    VPSS_NRX_V2_S *pastNRXParam;
} VPSS_NRX_PARAM_AUTO_V2_S;

/* Only used for Hi3516CV500/Hi3516AV300/Hi3516DV300/Hi3556V200/Hi3559V200 */
typedef struct hiVPSS_NRX_PARAM_V2_S {
    OPERATION_MODE_E           enOptMode;           /* RW;Adaptive NR */
    VPSS_NRX_PARAM_MANUAL_V2_S stNRXManual;         /* RW;NRX V2 param for manual video */
    VPSS_NRX_PARAM_AUTO_V2_S   stNRXAuto;           /* RW;NRX V2 param for auto video */
} VPSS_NRX_PARAM_V2_S;

/* Only used for Hi3516EV200 */
typedef struct {
    /* IES0~4 ; Range: [0, 255]; The gains of edge and texture enhancement. 0~3 for different frequency response. */
    HI_U8  IES0, IES1, IES2, IES3;
    /* IEDZ   ; Range: [0, 999]; The threshold to control the generated artifacts. */
    HI_U16  IEDZ : 10, IEEn : 1, _rb_ : 5;
} tV200_VPSS_IEy;

/* Only used for Hi3516EV200 */
typedef struct {
    HI_U8  SPN6 : 3, SFR  : 5;                                      /* SPN6, SBN6:  [0, 5]; */

    HI_U8  SBN6 : 3, PBR6 : 5;                                    /* SFR: [0,31];  PBR6: [0,15]; */

    /* JMODE;      Range: [0,   4]; The selection modes for the blending of spatial filters */
    /* STR0, STR1; Range: [0,  16]; The blending ratio of different filters. (Used in serial filtering mode (SFM).) */
    /* DeIdx;      Range: [3,   6]; The selection number of filters that textures and details will be added to. */
    HI_U16  SRT0 : 5, SRT1 : 5, JMODE : 3, DeIdx : 3;

    /* DeRate;     Range: [0, 255]; The enhancement strength for the SFM (When DeRate > 0, the SFM will be activated) */
    /* SFR6[4];    Range: [0,  31]; The relative NR strength for NO.6 filter. (Effective when JMODE = 4) */
    /* SBR6[2];    Range: [0,  15]; The control of overshoot and undershoot. */
    HI_U8  SFR6[4], SBR6[2], DeRate;

    /* SFS1, SFT1, SBR1; Range: [0, 255];  The NR strength parameters for NO.1 filter. */
    HI_U8  SFS1,  SFT1,  SBR1;
    /* SFS2, SFT2, SBR2; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    HI_U8  SFS2,  SFT2,  SBR2;
    /* SFS4, SFT4, SBR4; Range: [0, 255];  The NR strength parameters for NO.3 and NO.4 filters. */
    HI_U8  SFS4,  SFT4,  SBR4;

    /* STH1, STH2, STHd1, STHd2; Range: [0, 511]; The thresholds for protection of edges from blurring */
    /* NRyEn;      Range: [0,   1]; The NR switches */
    /* SFN0~2;     Range: [0,   6]; Filter selection for different image areas based on STH1~3. */
    /* KMode ;     Range: [0,   3]; The denoise mode based on image brightness. */
    /* SBSk[32], SDSk[32]; Range [0, 8191];  Spatial NR strength based on brightness. */
    HI_U16  STH1 : 9,  SFN1 : 3, SFN0  : 3, NRyEn   : 1;
    HI_U16  STHd1 : 9, _rb0_ : 7;
    HI_U16  STH2 : 9,  SFN2 : 3, kMode : 3, _rb1_   : 1;
    HI_U16  STHd2 : 9, _rb2_ : 7;
    HI_U16  SBSk[32], SDSk[32];
} tV200_VPSS_SFy;

/* Only used for Hi3516EV200 */
typedef struct {
    /* TFS0,TFS1;          Range: [0,  15]; The NR strength for temporal filtering. */
    /* TDZ0,TDZ1;          Range: [0, 999]; Protection of the weak texture area from temporal filtering.  */
    /* TDX0,TDX1;          Range: [0,   3]; Not for tuning. */
    HI_U16  TFS0 : 4,   TDZ0 : 10,  TDX0    : 2;
    HI_U16  TFS1 : 4,   TDZ1 : 10,  TDX1    : 2;

    /* SDZ0,SDZ1;          Range: [0, 999]; The threshold of NR control for result MAI1. */
    /* STR0,STR1;          Range: [0,  31]; The strength of NR control for result MAI1.  */
    /* DZMode0, DZMode1;   Range: [0,   1]; The selection mode for TDZ0 and TDZ1, respectively.  */
    HI_U16  SDZ0 : 10,  STR0 : 5,   DZMode0 : 1;
    HI_U16  SDZ1 : 10,  STR1 : 5,   DZMode1 : 1;

    /* TFR0,TFR1;          Range: [0,  31]; The temoproal NR strength control for background (static) area. */
    /* TSS0,TSS1;          Range: [0,  15]; The ratio for blending spatial NR with the temproal NR results. */
    /* TSI0,TSI1;          Range: [0,   1]; The selection of blending filter for TSS. */
    HI_U8  TSS0 : 4,   TSI0 : 4,  TFR0[6];
    HI_U8  TSS1 : 4,   TSI1 : 4,  TFR1[6];

    /* TED;  Range: [0,   3]; NR strength control mode for the updating background. */
    /* bRef;   Range: [0,   1]; The Switch for temproal filtering.  */
    /* TFRS;   Range: [0,  15]; Spatial filtering strength for static area. */
    HI_U8  TFRS : 4,   TED  : 2,   bRef    : 1,  _rb_ : 1;
} tV200_VPSS_TFy;

/* Only used for Hi3516EV200 */
typedef struct {
    /* PATH0 */
    /* MADZ0, MADZ1;     Range: [0, 511]; The blending ratio between MAI2 and MAI1 based on image statistics. */
    /* MAI00~02,MAI10~12 Range: [0,   3]; The three blending results between spatial and temporal filtering. */
    HI_U16  MADZ0   : 9,   MAI00 : 2,  MAI01  : 2, MAI02 : 2, _rb0_ : 1;
    HI_U16  MADZ1   : 9,   MAI10 : 2,  MAI11  : 2, MAI12 : 2, _rb1_ : 1;
    /* MABR0, MABR1;     Range: [0, 255]; The blending ratio between MAI2 and MAI1 based on brightness.  */
    HI_U8  MABR0, MABR1;

    /* MATH0,MATH1, MATHd0, MATHd1;     Range: [0, 999]; The theshold for motion detection. */
    /* MATE0,MATE1;   Range: [0,   8]; The motion index for smooth image area. */
    /* MATW;          Range: [0,   3]; The motion index for prevention of motion ghost. */
    /* MASW;          Range: [0,  15]; The motion index for low-frequency noises. */
    /* MABW0,MABW1;   Range: [0,   9]; The window size for motion detection. */
    HI_U16  MATH0   : 10,  MATE0 : 4,  MATW   : 2;
    HI_U16  MATHd0  : 10,  _rb2_ : 6;
    HI_U16  MATH1   : 10,  _rb3_ : 6;
    HI_U16  MATHd1  : 10,  _rb4_ : 6;
    HI_U8  MASW    :  4,  MATE1 : 4;
    HI_U8  MABW0   :  4,  MABW1 : 4;

    /* AdvMATH:  Range: [0, 1]; The switch to active the advanced mode. */
    /* AdvTH:    Range: [0, 999]; The threshold to control the effects of the AdvMATH. */
    HI_U16  AdvMATH : 1,   AdvTH : 12, _rb5_  : 3;
} tV200_VPSS_MDy;

/* Only used for Hi3516EV200 */
typedef struct {
    /* SFC;    Range: [0, 255]; Spatial NR strength. */
    /* TFC;    Range: [0,  32]; Temporal NR strength relative to Spatial NR. */
    HI_U8  SFC, TFC : 6, _rb0_ : 2;
    /* TRC;    Range: [0, 255]; Control of color bleeding in */
    /* TPC;    Range: [0,  32]; Type of temporal NR. */
    HI_U8  TRC, TPC : 6, _rb1_ : 2;

    HI_BOOL MODE;
    /* PRESFC;    Range: [0,  32]; The strength for chroma pre spatial filter. */
    HI_U8  PRESFC : 6, _rb2_ : 2;
} tV200_VPSS_NRc;

/* Only used for Hi3516EV200 */
typedef struct {
    tV200_VPSS_IEy  IEy[5];
    tV200_VPSS_SFy  SFy[5];
    tV200_VPSS_MDy  MDy[2];
    tV200_VPSS_TFy  TFy[3];
    tV200_VPSS_NRc  NRc;
} VPSS_NRX_V3_S;

/* Only used for Hi3516EV200 */
typedef struct hiVPSS_NRX_PARAM_MANUAL_V3_S {
    VPSS_NRX_V3_S stNRXParam;
} VPSS_NRX_PARAM_MANUAL_V3_S;

/* Only used for Hi3516EV200 */
typedef struct hiVPSS_NRX_PARAM_AUTO_V3_S {
    HI_U32 u32ParamNum;
    HI_U32 *pau32ISO;
    VPSS_NRX_V3_S *pastNRXParam;
} VPSS_NRX_PARAM_AUTO_V3_S;

/* Only used for Hi3516EV200 */
typedef struct hiVPSS_NRX_PARAM_V3_S {
    OPERATION_MODE_E           enOptMode;           /* RW;Adaptive NR */
    VPSS_NRX_PARAM_MANUAL_V3_S stNRXManual;         /* RW;NRX V3 param for manual video */
    VPSS_NRX_PARAM_AUTO_V3_S   stNRXAuto;           /* RW;NRX V3 param for auto video */
} VPSS_NRX_PARAM_V3_S;

/* Not support for Hi3559AV100 */
typedef enum hiVPSS_NR_VER_E {
    VPSS_NR_V1 = 1,
    VPSS_NR_V2 = 2,
    VPSS_NR_V3 = 3,
    VPSS_NR_V4 = 4,
    VPSS_NR_BUTT
} VPSS_NR_VER_E;

/* Not support for Hi3559AV100 */
typedef struct hiVPSS_GRP_NRX_PARAM_S {
    VPSS_NR_VER_E enNRVer;
    union {
        VPSS_NRX_PARAM_V1_S stNRXParam_V1;   /* interface X V1 for Hi3519AV100 */
        VPSS_NRX_PARAM_V2_S stNRXParam_V2;   /* interface X V2 for Hi3516CV500 */
        VPSS_NRX_PARAM_V3_S stNRXParam_V3;   /* interface X V3 for Hi3516EV200 */
    };

} VPSS_GRP_NRX_PARAM_S;

typedef struct hiVPSS_CHN_BUF_WRAP_S {
    HI_BOOL bEnable;
    HI_U32  u32BufLine;             /* RW; Range: [128, H]; Chn buffer allocated by line. */
    HI_U32  u32WrapBufferSize; /* RW; Whether to allocate buffer according to compression. */
} VPSS_CHN_BUF_WRAP_S;

typedef struct hiVPSS_PARAM_MOD_S {
    HI_BOOL bOneBufForLowDelay;
    HI_U32  u32VpssVbSource;
    HI_U32  u32VpssSplitNodeNum;
    HI_BOOL bHdrSupport;
    HI_BOOL bNrQuickStart;
} VPSS_MOD_PARAM_S;

typedef enum hiVPSS_CHN_PROC_MODE_E {
    VPSS_CHN_PROC_MODE_VIDEO = 0,
    VPSS_CHN_PROC_MODE_SNAP = 1,
    VPSS_CHN_PROC_MODE_BUTT
} VPSS_CHN_PROC_MODE_E;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_VPSS_H__ */


