/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: hi_comm_vi.h
 * Author:
 * Create: 2016-09-19
 */

#ifndef __HI_COMM_VI_H__
#define __HI_COMM_VI_H__

#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"
#include "hi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VI_MAX_ADCHN_NUM              (4UL)

#define VI_PMFCOEF_NUM                (9UL)
#define VI_COMPMASK_NUM               (2UL)
#define VI_PRO_MAX_FRAME_NUM          (8UL)

#define VI_INVALID_FRMRATE            (-1)
#define VI_CHN0                       0
#define VI_CHN1                       1
#define VI_CHN2                       2
#define VI_CHN3                       3
#define VI_INVALID_CHN                (-1)

#define VI_MAX_VC_NUM                 4

typedef struct hiVI_LOW_DELAY_INFO_S {
    HI_BOOL bEnable;          /* RW; Low delay enable. */
    HI_U32 u32LineCnt;        /* RW; Range: [32, 16384]; Low delay shoreline. */
} VI_LOW_DELAY_INFO_S;

/* Information of raw data cmpresss param */
typedef struct hiVI_CMP_PARAM_S {
    HI_U8 au8CmpParam[VI_CMP_PARAM_SIZE];
} VI_CMP_PARAM_S;

typedef enum hi_VI_USERPIC_MODE_E {
    VI_USERPIC_MODE_PIC = 0,            /* YUV picture */
    VI_USERPIC_MODE_BGC,                /* Background picture only with a color */
    VI_USERPIC_MODE_BUTT,
} VI_USERPIC_MODE_E;

typedef struct hiVI_USERPIC_BGC_S {
    HI_U32 u32BgColor;
} VI_USERPIC_BGC_S;

typedef struct hiVI_USERPIC_ATTR_S {
    VI_USERPIC_MODE_E       enUsrPicMode;  /* User picture mode */
    union {
        VIDEO_FRAME_INFO_S  stUsrPicFrm;   /* Information about a YUV picture */
        VI_USERPIC_BGC_S    stUsrPicBg;    /* Information about a background picture only with a color */
    } unUsrPic;
} VI_USERPIC_ATTR_S;

typedef enum hiEN_VI_ERR_CODE_E {
    ERR_VI_FAILED_NOTENABLE = 64,       /* device or channel not enable */
    ERR_VI_FAILED_NOTDISABLE,           /* device not disable */
    ERR_VI_FAILED_CHNOTDISABLE,         /* channel not disable */
    ERR_VI_CFG_TIMEOUT,                 /* config timeout */
    ERR_VI_NORM_UNMATCH,                /* video norm of ADC and VIU is unmatch */
    ERR_VI_INVALID_WAYID,               /* invlalid way ID */
    ERR_VI_INVALID_PHYCHNID,            /* invalid phychn id */
    ERR_VI_FAILED_NOTBIND,              /* device or channel not bind */
    ERR_VI_FAILED_BINDED,               /* device or channel not unbind */
    ERR_VI_DIS_PROCESS_FAIL             /* dis process failed */
} EN_VI_ERR_CODE_E;

#define HI_ERR_VI_INVALID_PARA        HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VI_INVALID_DEVID       HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VI_INVALID_PIPEID      HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_PIPEID)
#define HI_ERR_VI_INVALID_STITCHGRPID HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_STITCHGRPID)
#define HI_ERR_VI_INVALID_CHNID       HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VI_INVALID_NULL_PTR    HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VI_FAILED_NOTCONFIG    HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define HI_ERR_VI_SYS_NOTREADY        HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VI_BUF_EMPTY           HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_VI_BUF_FULL            HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_VI_NOMEM               HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VI_NOT_SUPPORT         HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VI_BUSY                HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VI_NOT_PERM            HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define HI_ERR_VI_FAILED_NOTENABLE    HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTENABLE)
#define HI_ERR_VI_FAILED_NOTDISABLE   HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTDISABLE)
#define HI_ERR_VI_FAILED_CHNOTDISABLE HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_CHNOTDISABLE)
#define HI_ERR_VI_CFG_TIMEOUT         HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_CFG_TIMEOUT)
#define HI_ERR_VI_NORM_UNMATCH        HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_NORM_UNMATCH)
#define HI_ERR_VI_INVALID_WAYID       HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_WAYID)
#define HI_ERR_VI_INVALID_PHYCHNID    HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_PHYCHNID)
#define HI_ERR_VI_FAILED_NOTBIND      HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTBIND)
#define HI_ERR_VI_FAILED_BINDED       HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_BINDED)

#define HI_ERR_VI_PIPE_EXIST          HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_VI_PIPE_UNEXIST        HI_DEF_ERR(HI_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)

/* interface mode of video input */
typedef enum hiVI_INTF_MODE_E {
    VI_MODE_BT656 = 0,              /* ITU-R BT.656 YUV4:2:2 */
    VI_MODE_BT656_PACKED_YUV,       /* ITU-R BT.656 packed YUV4:2:2 */
    VI_MODE_BT601,                  /* ITU-R BT.601 YUV4:2:2 */
    VI_MODE_DIGITAL_CAMERA,         /* digatal camera mode */
    VI_MODE_BT1120_STANDARD,        /* BT.1120 progressive mode */
    VI_MODE_BT1120_INTERLEAVED,     /* BT.1120 interstage mode */
    VI_MODE_MIPI,                   /* MIPI RAW mode */
    VI_MODE_MIPI_YUV420_NORMAL,     /* MIPI YUV420 normal mode */
    VI_MODE_MIPI_YUV420_LEGACY,     /* MIPI YUV420 legacy mode */
    VI_MODE_MIPI_YUV422,            /* MIPI YUV422 mode */
    VI_MODE_LVDS,                   /* LVDS mode */
    VI_MODE_HISPI,                  /* HiSPi mode */
    VI_MODE_SLVS,                   /* SLVS mode */

    VI_MODE_BUTT
} VI_INTF_MODE_E;

/* Input mode */
typedef enum hiVI_INPUT_MODE_E {
    VI_INPUT_MODE_BT656 = 0,        /* ITU-R BT.656 YUV4:2:2 */
    VI_INPUT_MODE_BT601,            /* ITU-R BT.601 YUV4:2:2 */
    VI_INPUT_MODE_DIGITAL_CAMERA,   /* digatal camera mode */
    VI_INPUT_MODE_INTERLEAVED,      /* interstage mode */
    VI_INPUT_MODE_MIPI,             /* MIPI mode */
    VI_INPUT_MODE_LVDS,             /* LVDS mode */
    VI_INPUT_MODE_HISPI,            /* HiSPi mode */
    VI_INPUT_MODE_SLVS,             /* SLVS mode */

    VI_INPUT_MODE_BUTT
} VI_INPUT_MODE_E;

/* Work mode */
typedef enum hiVI_WORK_MODE_E {
    VI_WORK_MODE_1Multiplex = 0,    /* 1 Multiplex mode */
    VI_WORK_MODE_2Multiplex,        /* 2 Multiplex mode */
    VI_WORK_MODE_3Multiplex,        /* 3 Multiplex mode */
    VI_WORK_MODE_4Multiplex,        /* 4 Multiplex mode */

    VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;

/* whether an input picture is interlaced or progressive */
typedef enum hiVI_SCAN_MODE_E {
    VI_SCAN_INTERLACED  = 0,        /* interlaced mode */
    VI_SCAN_PROGRESSIVE,            /* progressive mode */

    VI_SCAN_BUTT
} VI_SCAN_MODE_E;

/* Sequence of YUV data */
typedef enum hiVI_YUV_DATA_SEQ_E {
    VI_DATA_SEQ_VUVU = 0,   /* The input sequence of the second component(only contains u and v) in BT.
                            1120 mode is VUVU */
    VI_DATA_SEQ_UVUV,       /* The input sequence of the second component(only contains u and v) in BT.
                            1120 mode is UVUV */

    VI_DATA_SEQ_UYVY,       /* The input sequence of YUV is UYVY */
    VI_DATA_SEQ_VYUY,       /* The input sequence of YUV is VYUY */
    VI_DATA_SEQ_YUYV,       /* The input sequence of YUV is YUYV */
    VI_DATA_SEQ_YVYU,       /* The input sequence of YUV is YVYU */

    VI_DATA_SEQ_BUTT
} VI_YUV_DATA_SEQ_E;

/* Clock edge mode */
typedef enum hiVI_CLK_EDGE_E {
    VI_CLK_EDGE_SINGLE_UP = 0,         /* single-edge mode and in rising edge */
    VI_CLK_EDGE_SINGLE_DOWN,           /* single-edge mode and in falling edge */

    VI_CLK_EDGE_BUTT
} VI_CLK_EDGE_E;

/* Component mode */
typedef enum hiVI_COMPONENT_MODE_E {
    VI_COMPONENT_MODE_SINGLE = 0,           /* single component mode */
    VI_COMPONENT_MODE_DOUBLE,               /* double component mode */

    VI_COMPONENT_MODE_BUTT
} VI_COMPONENT_MODE_E;

/* Y/C composite or separation mode */
typedef enum hiVI_COMBINE_MODE_E {
    VI_COMBINE_COMPOSITE = 0,     /* Composite mode */
    VI_COMBINE_SEPARATE,          /* Separate mode */

    VI_COMBINE_BUTT
} VI_COMBINE_MODE_E;

/* Attribute of the vertical synchronization signal */
typedef enum hiVI_VSYNC_E {
    VI_VSYNC_FIELD = 0,           /* Field/toggle mode:a signal reversal means a new frame or a field */
    VI_VSYNC_PULSE,               /* Pusle/effective mode:a pusle or an effective signal means a new frame or a field */

    VI_VSYNC_BUTT
} VI_VSYNC_E;

/* Polarity of the vertical synchronization signal */
typedef enum hiVI_VSYNC_NEG_E {
    VI_VSYNC_NEG_HIGH = 0,        /* if VIU_VSYNC_E = VIU_VSYNC_FIELD, then the vertical synchronization signal of
                                    even field is high-level,
                                    if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization
                                    pulse is positive pulse. */
    VI_VSYNC_NEG_LOW,             /* if VIU_VSYNC_E = VIU_VSYNC_FIELD, then the vertical synchronization signal
                                    of even field is low-level,
                                    if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization
                                    pulse is negative pulse. */
    VI_VSYNC_NEG_BUTT
} VI_VSYNC_NEG_E;

/* Attribute of the horizontal synchronization signal */
typedef enum hiVI_HSYNC_E {
    VI_HSYNC_VALID_SINGNAL = 0,   /* the horizontal synchronization is valid signal mode */
    VI_HSYNC_PULSE,               /* the horizontal synchronization is pulse mode, a new pulse means
                                    the beginning of a new line */

    VI_HSYNC_BUTT
} VI_HSYNC_E;

/* Polarity of the horizontal synchronization signal */
typedef enum hiVI_HSYNC_NEG_E {
    VI_HSYNC_NEG_HIGH = 0,        /* if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL, then the valid horizontal
                                    synchronization signal is high-level;
                                    if VI_HSYNC_E = VI_HSYNC_PULSE,then the horizontal synchronization
                                    pulse is positive pulse */
    VI_HSYNC_NEG_LOW,             /* if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL, then the valid horizontal
                                    synchronization signal is low-level;
                                    if VI_HSYNC_E = VI_HSYNC_PULSE, then the horizontal synchronization
                                    pulse is negative pulse */
    VI_HSYNC_NEG_BUTT
} VI_HSYNC_NEG_E;

/* Attribute of the valid vertical synchronization signal */
typedef enum hiVI_VSYNC_VALID_E {
    VI_VSYNC_NORM_PULSE = 0,      /* the vertical synchronization is pusle mode, a pusle means a new frame or field  */
    VI_VSYNC_VALID_SINGAL,        /* the vertical synchronization is effective mode, a effective signal
                                    means a new frame or field */

    VI_VSYNC_VALID_BUTT
} VI_VSYNC_VALID_E;

/* Polarity of the valid vertical synchronization signal */
typedef enum hiVI_VSYNC_VALID_NEG_E {
    VI_VSYNC_VALID_NEG_HIGH = 0,  /* if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE, a positive pulse means vertical
                                    synchronization pulse;
                                    if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL, the valid vertical synchronization
                                    signal is high-level */
    VI_VSYNC_VALID_NEG_LOW,       /* if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE, a negative pulse
                                    means vertical synchronization pulse;
                                    if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL, the valid vertical
                                    synchronization signal is low-level */
    VI_VSYNC_VALID_NEG_BUTT
} VI_VSYNC_VALID_NEG_E;

/* Blank information of the input timing */
typedef struct hiVI_TIMING_BLANK_S {
    HI_U32 u32HsyncHfb ;    /* RW;Horizontal front blanking width */
    HI_U32 u32HsyncAct ;    /* RW;Horizontal effetive width */
    HI_U32 u32HsyncHbb ;    /* RW;Horizontal back blanking width */
    HI_U32 u32VsyncVfb ;    /* RW;Vertical front blanking height of one frame or odd-field frame picture */
    HI_U32 u32VsyncVact ;   /* RW;Vertical effetive width of one frame or odd-field frame picture */
    HI_U32 u32VsyncVbb ;    /* RW;Vertical back blanking height of one frame or odd-field frame picture */
    HI_U32 u32VsyncVbfb ;   /* RW;Even-field vertical front blanking height when input mode is interlace
                            (invalid when progressive input mode) */
    HI_U32 u32VsyncVbact ;  /* RW;Even-field vertical effetive width when input mode is interlace
                            (invalid when progressive input mode) */
    HI_U32 u32VsyncVbbb ;   /* RW;Even-field vertical back blanking height when input mode is interlace
                            (invalid when progressive input mode) */
} VI_TIMING_BLANK_S;

/* synchronization information about the BT.601 or DC timing */
typedef struct hiVI_SYNC_CFG_S {
    VI_VSYNC_E              enVsync;
    VI_VSYNC_NEG_E          enVsyncNeg;
    VI_HSYNC_E              enHsync;
    VI_HSYNC_NEG_E          enHsyncNeg;
    VI_VSYNC_VALID_E        enVsyncValid;
    VI_VSYNC_VALID_NEG_E    enVsyncValidNeg;
    VI_TIMING_BLANK_S       stTimingBlank;
} VI_SYNC_CFG_S;

/* the highest bit of the BT.656 timing reference code */
typedef enum hiVI_BT656_FIXCODE_E {
    VI_BT656_FIXCODE_1 = 0,       /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 1. */
    VI_BT656_FIXCODE_0,           /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 0. */

    VI_BT656_FIXCODE_BUTT
} VI_BT656_FIXCODE_E;

/* Polarity of the field indicator bit (F) of the BT.656 timing reference code */
typedef enum hiVI_BT656_FIELD_POLAR_E {
    VI_BT656_FIELD_POLAR_STD = 0, /* the standard BT.656 mode,the first filed F=0,the second filed F=1 */
    VI_BT656_FIELD_POLAR_NSTD,    /* the non-standard BT.656 mode,the first filed F=1,the second filed F=0 */

    VI_BT656_FIELD_POLAR_BUTT
} VI_BT656_FIELD_POLAR_E;

/* synchronization information about the BT.656 */
typedef struct hiVI_BT656_SYNC_CFG_S {
    VI_BT656_FIXCODE_E     enFixCode;
    VI_BT656_FIELD_POLAR_E enFieldPolar;
} VI_BT656_SYNC_CFG_S;

/* Input data type */
typedef enum hiVI_DATA_TYPE_E {
    VI_DATA_TYPE_YUV = 0,
    VI_DATA_TYPE_RGB,

    VI_DATA_TYPE_BUTT
} VI_DATA_TYPE_E;

typedef enum hiVI_REPHASE_MODE_E {
    VI_REPHASE_MODE_NONE = 0,
    VI_REPHASE_MODE_SKIP_1_2,            /* skip 1/2 */
    VI_REPHASE_MODE_SKIP_1_3,            /* skip 1/3 */
    VI_REPHASE_MODE_BINNING_1_2,          /* binning 1/2 */
    VI_REPHASE_MODE_BINNING_1_3,        /* binning 1/3 */

    VI_REPHASE_MODE_BUTT
} VI_REPHASE_MODE_E;

typedef struct hiVI_BAS_REPHASE_ATTR_S {
    VI_REPHASE_MODE_E enHRephaseMode;
    VI_REPHASE_MODE_E enVRephaseMode;
} VI_BAS_REPHASE_ATTR_S;

/* Attribute of bas scale */
typedef struct hiVI_BAS_SCALE_ATTR_S {
    SIZE_S stBasSize; /* RW;bayer scale size. */
} VI_BAS_SCALE_ATTR_S;

/* Attribute of bayer scale */
typedef struct hiVI_BAS_ATTR_S {
    VI_BAS_SCALE_ATTR_S     stSacleAttr;
    VI_BAS_REPHASE_ATTR_S   stRephaseAttr;
} VI_BAS_ATTR_S;

/* Attribute of wdr */
typedef struct hiVI_WDR_ATTR_S {
    WDR_MODE_E  enWDRMode;          /* RW; WDR mode. */
    HI_U32      u32CacheLine;       /* RW; WDR cache line. */
} VI_WDR_ATTR_S;

/* the extended attributes of VI device */
typedef struct hiVI_DEV_ATTR_EX_S {
    VI_INPUT_MODE_E         enInputMode;                    /* RW;Input mode */
    VI_WORK_MODE_E          enWorkMode;                     /* RW; Work mode */

    VI_COMBINE_MODE_E       enCombineMode;                  /* RW;Y/C composite or separation mode */
    VI_COMPONENT_MODE_E     enComponentMode;                /* RW;Component mode (single-component or dual-component) */
    VI_CLK_EDGE_E           enClkEdge;                      /* RW;Clock edge mode (sampling on the rising or
                                                            falling edge) */

    HI_U32                  au32ComponentMask[VI_COMPMASK_NUM]; /* RW;Component mask */

    VI_SCAN_MODE_E          enScanMode;                     /* RW;Input scanning mode (progressive or interlaced) */
    HI_S32                  as32AdChnId[VI_MAX_ADCHN_NUM];  /* RW;AD channel ID. Typically, the default
                                                            value -1 is recommended */

    VI_YUV_DATA_SEQ_E       enDataSeq;                  /* RW;Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S           stSynCfg;                       /* RW;Sync timing. This member must be configured in BT.
                                                            601 mode or DC mode */

    VI_BT656_SYNC_CFG_S     stBT656SynCfg;                  /* RW;Sync timing. This member must be configured in BT.
                                                            656 mode */

    VI_DATA_TYPE_E          enInputDataType;                /* RW;RGB: CSC-709 or CSC-601, PT YUV444 disable;
                                                            YUV: default yuv CSC coef PT YUV444 enable. */

    HI_BOOL                 bDataReverse;                   /* RW;Data reverse */

    SIZE_S                  stSize;                         /* RW;Input size */

    VI_BAS_ATTR_S           stBasAttr;                      /* RW;Attribute of BAS */

    VI_WDR_ATTR_S           stWDRAttr;                      /* RW;Attribute of WDR */

    DATA_RATE_E             enDataRate;                     /* RW;Data rate of Device */
} VI_DEV_ATTR_EX_S;

/* The attributes of a VI device */
typedef struct hiVI_DEV_ATTR_S {
    VI_INTF_MODE_E      enIntfMode;                     /* RW;Interface mode */
    VI_WORK_MODE_E      enWorkMode;                     /* RW;Work mode */

    HI_U32              au32ComponentMask[VI_COMPMASK_NUM];  /* RW;Component mask */
    VI_SCAN_MODE_E      enScanMode;                     /* RW;Input scanning mode (progressive or interlaced) */
    HI_S32              as32AdChnId[VI_MAX_ADCHN_NUM];  /* RW;AD channel ID. Typically, the default value -1
                                                        is recommended */

    /* The below members must be configured in BT.601 mode or DC mode and are invalid in other modes */
    VI_YUV_DATA_SEQ_E   enDataSeq;                      /* RW;Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S       stSynCfg;                       /* RW;Sync timing. This member must be configured in BT.
                                                        601 mode or DC mode */

    VI_DATA_TYPE_E      enInputDataType;                /* RW;RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default
                                                        yuv CSC coef PT YUV444 enable. */

    HI_BOOL             bDataReverse;                   /* RW;Data reverse */

    SIZE_S              stSize;                         /* RW;Input size */

    VI_BAS_ATTR_S       stBasAttr;                      /* RW;Attribute of BAS */

    VI_WDR_ATTR_S       stWDRAttr;                      /* RW;Attribute of WDR */

    DATA_RATE_E         enDataRate;                     /* RW;Data rate of Device */
} VI_DEV_ATTR_S;

/* Information of pipe binded to device */
typedef struct hiVI_DEV_BIND_PIPE_S {
    HI_U32  u32Num;                                     /* RW;Range [1,VI_MAX_PHY_PIPE_NUM] */
    VI_PIPE PipeId[VI_MAX_PHY_PIPE_NUM];                /* RW;Array of pipe ID */
} VI_DEV_BIND_PIPE_S;

/* Source of 3DNR reference frame */
typedef enum hiVI_NR_REF_SOURCE_E {
    VI_NR_REF_FROM_RFR = 0,                             /* Reference frame from reconstruction frame */
    VI_NR_REF_FROM_CHN0,                                /* Reference frame from CHN0's frame */

    VI_NR_REF_FROM_BUTT
} VI_NR_REF_SOURCE_E;

typedef enum hiVI_PIPE_BYPASS_MODE_E {
    VI_PIPE_BYPASS_NONE,
    VI_PIPE_BYPASS_FE,
    VI_PIPE_BYPASS_BE,

    VI_PIPE_BYPASS_BUTT
} VI_PIPE_BYPASS_MODE_E;

/* The attributes of 3DNR */
typedef struct hiVI_NR_ATTR_S {
    PIXEL_FORMAT_E      enPixFmt;                       /* RW;Pixel format of reference frame */
    DATA_BITWIDTH_E     enBitWidth;                     /* RW;Bit Width of reference frame */
    VI_NR_REF_SOURCE_E  enNrRefSource;                  /* RW;Source of 3DNR reference frame */
    COMPRESS_MODE_E     enCompressMode;                 /* RW;Reference frame compress mode */
} VI_NR_ATTR_S;

/* The attributes of pipe */
typedef struct hiVI_PIPE_ATTR_S {
    VI_PIPE_BYPASS_MODE_E enPipeBypassMode;
    HI_BOOL               bYuvSkip;               /* RW;YUV skip enable */
    HI_BOOL               bIspBypass;             /* RW;Range:[0, 1];ISP bypass enable */
    HI_U32                u32MaxW;                /* RW;Range:[0, 1];Range[VI_PIPE_MIN_WIDTH, VI_PIPE_MAX_WIDTH];
                                                Maximum width */
    HI_U32                u32MaxH;                /* RW;Range[VI_PIPE_MIN_HEIGHT, VI_PIPE_MAX_HEIGHT];Maximum height */
    PIXEL_FORMAT_E        enPixFmt;               /* RW;Pixel format */
    COMPRESS_MODE_E       enCompressMode;         /* RW;Range:[0, 4];Compress mode. */
    DATA_BITWIDTH_E       enBitWidth;             /* RW;Range:[0, 4];Bit width */
    HI_BOOL               bNrEn;                  /* RW;Range:[0, 1];3DNR enable */
    VI_NR_ATTR_S          stNrAttr;               /* RW;Attribute of 3DNR */
    HI_BOOL               bSharpenEn;             /* RW;Range:[0, 1];Sharpen enable */
    FRAME_RATE_CTRL_S     stFrameRate;            /* RW;Frame rate */
    HI_BOOL               bDiscardProPic;         /* RW;Range:[0, 1];when professional mode snap, whether to discard
                                                long exposure picture in the video pipe. */
} VI_PIPE_ATTR_S;

typedef enum hiVI_STITCH_ISP_CFG_MODE_E {
    VI_STITCH_ISP_CFG_NORMAL = 0,
    VI_STITCH_ISP_CFG_SYNC,
    VI_STITCH_ISP_CFG_BUTT
} VI_STITCH_ISP_CFG_MODE_E;

/* Information of stitch group */
typedef struct hiVI_STITCH_GRP_ATTR_S {
    HI_BOOL                     bStitch;
    VI_STITCH_ISP_CFG_MODE_E    enMode;
    HI_U32                      u32MaxPTSGap;            /* RW;MAX PTS Gap between frame of pipe,unit:us */
    HI_U32                      u32PipeNum;              /* RW;Range [2, VI_MAX_PIPE_NUM] */
    VI_PIPE                     PipeId[VI_MAX_PIPE_NUM]; /* RW;Array of pipe ID */
} VI_STITCH_GRP_ATTR_S;

typedef enum hiVI_PIPE_REPEAT_MODE_E {
    VI_PIPE_REPEAT_NONE = 0,
    VI_PIPE_REPEAT_ONCE = 1,
    VI_PIPE_REPEAT_BUTT
} VI_PIPE_REPEAT_MODE_E;

typedef struct {
    HI_U8   IES;              /* RW; Range:[0, 255];Format 8.0;the absolute strength of image enhancement for edge */
    HI_U8   IESS;             /* RW; Range:[0, 255];Format 8.0;the absolute strength of image enhancement for
                            texture and shadow */
    HI_U16  IEDZ;             /* RW; Range:[0, 8192];Format 14.0;the threshold of image enhancement
                            for controlling noise */
} tV59aIEy;

typedef struct {
    HI_U8   SBF     : 2;      /* RW; Range:[0, 3];Format 2.0;the band type of spatial filter, notice: SBF0, SBF1
                            range is [2, 3];SBF2,SBF3,SBF4 range is [0,3], where SBF4 is related to SBFk */
    HI_U8   STR     : 4;      /* RW; Range:[0, 13];Format 4.0;the relative strength of spatial filter refer to
                            the previous frame */
    HI_U8   STHp    : 2;      /* RW; Range:[0, 2];Format 2.0;Not recommended for debugging */
    HI_U8   SFT     : 5;      /* RW; Range:[0, 31];Format 5.0;Not recommended for debugging */
    HI_U8   kPro    : 3;      /* RW; Range:[0, 7];Format 3.0;notice: the kPro of SFy2 range is [0, 7], the kPro of
                            SFy3 range is [0, 4] */

    HI_U16  STH[3];           /* RW; Range:[0, 999];Format 10.0;the edge-preserve threshold for spatial filter */
    HI_U16  SBS[3];           /* RW; Range:[0, 9999];Format 14.0;the noise reduction strength of spatial filter
                            for the relative bright pixel */
    HI_U16  SDS[3];           /* RW; Range:[0, 9999];Format 14.0;the noise reduction strength of spatial filter
                            for the relative dark pixel */

} tV59aSFy;

typedef struct {
    HI_U16  MATH    : 10;    /* RW; Range:[0, 1023];Format 10.0;the motion detection threshold for temporal filter */
    HI_U16  MATE    : 4;     /* RW; Range:[0, 11];Format 4.0;the motion detection index of flat area
                            for temporal filter */
    HI_U16  MATW    : 2;     /* RW; Range:[0, 3];Format 2.0;the index of suppressing trailing for temporal filter */
    HI_U8   MASW    : 4;     /* RW; Range:[0, 12];Format 4.0;the index of suppressing raindrop noise
                            for temporal filter */
    HI_U8   MABW    : 3;     /* RW; Range:[0, 4];Format 3.0;the window of motion detection for temporal filter */
    HI_U8   MAXN    : 1;     /* RW; Range:[0, 1];Format 1.0;Not recommended for debugging */

} tV59aMDy;

typedef struct {
    HI_U8   TFR[4];         /* RW; Range:[0, 255];Format 8.0;the relative strength of temporal
                            filter for the static area */
    HI_U16  TDZ : 14;       /* RW; Range:[0, 999];Format 10.0;the threshold of dead-area of temporal filter */
    HI_U16  TDX : 2;        /* RW; Range:[0, 2];Format 2.0;Not recommended for debugging */
    HI_U16  TFS : 6;        /* RW; Range:[0, 63];Format 6.0;the absolute strength of temporal filter  */
} tV59aTFy;

typedef struct {
    HI_U16  SFC     : 10;   /* RW; Range:[0, 1023];Format 10.0;the strength of spatial filter for NRC0 */
    HI_U16  TFC     : 6;    /* RW; Range:[0, 63];Format 6.0;the strength of temporal filter for NRC0 */
    HI_U16  CSFS    : 14;   /* RW; Range:[0, 999];Format 10.0;the strength of spatial filter for NRC1 */
    HI_U16  CSFk    : 2;    /* RW; Range:[0, 3];Format 2.0;Not recommended for debugging */
    HI_U16  CTFS    : 4;    /* RW; Range:[0, 15];Format 4.0;the strength of temporal filter for NRC1 */
    HI_U16  CIIR    : 1;    /* RW; Range:[0, 1];Format 1.0;the mode of spatial filter for NRC1 */
    HI_U16  CTFR    : 11;   /* RW; Range:[0, 999];Format 10.0;the relative strength of temporal filter for NRC1 */

} tV59aNRc;

/* 3DNR Spatial Filter: SFy0,SFy1,SFy2,SFy3; Temporal Filter:TFy0,TFy1;Chroma Noise Reduction: NRC0,NRC1 */
typedef struct {
    tV59aIEy  IEy;
    tV59aSFy  SFy[5];
    tV59aMDy  MDy[2];
    tV59aTFy  TFy[2];

    HI_U16  HdgType : 1;   /* RW; Range:[0, 1];Format 1.0;the type of complexed mixed spatial filter whether
                            is SFi or SFk */
    HI_U16  BriType : 1;   /* RW; Range:[0, 1];Format 1.0;the mode decide SFy3 whether is SFk type or SFi type */
    HI_U16  HdgMode : 2;   /* RW; Range:[0, 3];Format 2.0;the mode decide complexed mixed spatial filter band
                            for flat area */
    HI_U16  kTab2   : 1;   /* RW; Range:[0, 1];Format 1.0;the parameter decide SFy2 whether or not based on the image
                            absolute luminance */
    HI_U16  HdgWnd  : 1;   /* RW; Range:[0, 1];Format 1.0;the sampling window of complexed mixed spatial filter for
                            noise detection */
    HI_U16  kTab3   : 1;   /* RW; Range:[0, 1];Format 1.0;the parameter decide SFy3 whether or not based on the image
                            absolute luminance */
    HI_U16  HdgSFR  : 4;   /* RW; Range:[0, 13];Format 4.0;the trend of the noise reduction of complexed mixed spatial
                            filter for flat area */
    HI_U16  nOut    : 5;   /* RW; Range:[0, 27];Format 5.0;the parameter for output intermediate result of SFy3 */
    HI_U8   HdgIES;        /* RW; Range:[0, 255];Format 8.0;the strength of image enhancement for complexed
                            mixed spatial filter */
    HI_U8   nRef    : 1;   /* RW; Range:[0, 1];Format 1.0;Not recommended for debugging */

    HI_U8   IEyMode : 1;   /* RW; Range:[0, 1];Format 1.0;the image enhancement mode selection. */
    HI_U8   IEyEx[4];      /* RW; Range:[0, 255];Format 8.0;the image enhancement strength for different frequency. */

    HI_U8   SFRi[4];       /* RW; Range:[0, 255];Format 8.0;the relative strength of SFy3 when the filter type is SFi */
    HI_U8   SFRk[4];       /* RW; Range:[0, 255];Format 8.0;the relative strength of SFy3 when the filter type is SFk */
    HI_U16  SBSk2[32];     /* RW; Range:[0, 9999];Format 14.0;the noise reduction strength of SFy2 for the
                            relative bright pixel based on the image absolute luminance */
    HI_U16  SBSk3[32];     /* RW; Range:[0, 9999];Format 14.0;the noise reduction strength of SFy3 for the
                            relative bright pixel based on the image absolute luminance */
    HI_U16  SDSk2[32];     /* RW; Range:[0, 9999];Format 14.0;the noise reduction strength of SFy2 for the
                            relative dark pixel based on the image absolute luminance */
    HI_U16  SDSk3[32];     /* RW; Range:[0, 9999];Format 14.0;the noise reduction strength of SFy3 for the
                            relative dark pixel based on the image absolute luminance */
    HI_U16  BriThr[16];    /* RW; Range:[0, 1024];Format 11.0;the threshold decide SFy3 choose the SFi type filter
                            or SFk type filter in dark and bright area */

    tV59aNRc NRc;
} VI_PIPE_NRX_PARAM_V1_S;

typedef enum hiVI_NR_VERSION_E {
    VI_NR_V1 = 1,
    VI_NR_V2 = 2,
    VI_NR_V3 = 3,
    VI_NR_V4 = 4,
    VI_NR_BUTT
} VI_NR_VERSION_E;

typedef struct hiNRX_PARAM_MANUAL_V1_S {
    VI_PIPE_NRX_PARAM_V1_S stNRXParamV1;
} NRX_PARAM_MANUAL_V1_S;

typedef struct hiNRX_PARAM_AUTO_V1_S {
    HI_U32                              u32ParamNum;
    HI_U32                  ATTRIBUTE   *pau32ISO;
    VI_PIPE_NRX_PARAM_V1_S  ATTRIBUTE   *pastNRXParamV1;
} NRX_PARAM_AUTO_V1_S;

typedef struct hiNRX_PARAM_V1_S {
    OPERATION_MODE_E        enOptMode;            /* RW;Adaptive NR */
    NRX_PARAM_MANUAL_V1_S   stNRXManualV1;        /* RW;NRX V1 param for manual */
    NRX_PARAM_AUTO_V1_S     stNRXAutoV1;          /* RW;NRX V1 param for auto */
} NRX_PARAM_V1_S;

typedef struct {
    HI_U8  IES0, IES1, IES2, IES3;  /* IES0~4 ; Range: [0, 255]; The gains of edge and texture enhancement.
                                    0~3 for different frequency response. */
    HI_U16 IEDZ : 10, _rb_ : 6;     /* IEDZ   ; Range: [0, 999]; The threshold to control the generated artifacts. */
} tV500_VI_IEy;

typedef struct {
    HI_U8  SPN6 : 3, SFR  : 5;      /* SPN6; Range: [0, 5];  The selection of filters to be mixed for NO.6 filter. */
                                    /* SFR ; Range: [0, 31];  The relative NR strength in the SFi and SFk filter. */
    HI_U8  SBN6 : 3, PBR6 : 5;      /* SBN6; Range: [0, 5];  The selection of filters to be mixed for NO.6 filter. */
                                    /* PBR6; Range: [0, 16];  The mix ratio between SPN6 and SBN6. */
    HI_U16 SRT0 : 5, SRT1 : 5, JMODE : 3, DeIdx : 3;    /* JMODE;      Range: [0, 4]; The selection modes
                                                         for the blending of spatial filters */
                                                        /* STR0, STR1; Range: [0, 16]; The blending ratio
                                                         of different filters. (Used in serial filtering mode (SFM).) */
                                                        /* DeIdx;      Range: [3, 6]; The selection number of
                                                         filters that textures and details will be added to. */
    HI_U8  DeRate, SFR6[3];                             /* DeRate;     Range: [0, 255]; The enhancement strength
                                                         for the SFM (When DeRate > 0, the SFM will be activated) */
                                                        /* SFR6;       Range: [0, 31]; The relative NR strength
                                                         for NO.6 filter. (Effective when JMODE = 4) */
    HI_U8  SFS1, SFT1, SBR1;                            /* SFS1, SFT1, SBR1; Range: [0, 255];  The NR strength
                                                         parameters for NO.1 filter. */
    HI_U8  SFS2, SFT2, SBR2;                            /* SFS2, SFT2, SBR2; Range: [0, 255];  The NR strength
                                                         parameters for NO.2 filter. */
    HI_U8  SFS4, SFT4, SBR4;                            /* SFS4, SFT4, SBR4; Range: [0, 255];  The NR strength
                                                         parameters for NO.3 and NO.4 filters. */

    HI_U16 STH1 : 9, SFN1 : 3, NRyEn : 1, SFN0  : 3;    /* STH1~3; Range: [0, 511]; The thresholds for protection
                                                         of edges from blurring */
                                                        /* NRyEn ; Range: [0, 1]; The NR switches */
    HI_U16 STH2 : 9, SFN2 : 3, BWSF4 : 1, kMode : 3;    /* SFN0~3; Range: [0, 6]; Filter selection for different
                                                         image areas based on STH1~3. */
                                                        /* BWSF4 ; Range: [0, 1]; The NR window size for the
                                                         NO.3 and NO.4 filters.  */
    HI_U16 STH3 : 9, SFN3 : 3, TriTh : 1, _rb0_ : 3;    /* KMode ; Range: [0, 3]; The denoise mode based
                                                         on image brightness. */
                                                        /* Trith ; Range: [0, 1]; The switch to choose 3 STH
                                                         threshold or 2 STH threshold */
} tV500_VI_SFy;

typedef struct {
    tV500_VI_IEy IEy;
    tV500_VI_SFy SFy;
} VI_PIPE_NRX_PARAM_V2_S;

typedef struct hiNRX_PARAM_MANUAL_V2_S {
    VI_PIPE_NRX_PARAM_V2_S stNRXParamV2;
} NRX_PARAM_MANUAL_V2_S;

typedef struct hiNRX_PARAM_AUTO_V2_S {
    HI_U32                              u32ParamNum;
    HI_U32                  ATTRIBUTE   *pau32ISO;
    VI_PIPE_NRX_PARAM_V2_S  ATTRIBUTE   *pastNRXParamV2;
} NRX_PARAM_AUTO_V2_S;

typedef struct hiNRX_PARAM_V2_S {
    OPERATION_MODE_E        enOptMode;           /* RW;Adaptive NR */
    NRX_PARAM_MANUAL_V2_S   stNRXManualV2;       /* RW;NRX V2 param for manual */
    NRX_PARAM_AUTO_V2_S     stNRXAutoV2;         /* RW;NRX V2 param for auto */
} NRX_PARAM_V2_S;

typedef struct hiVI_PIPE_NRX_PARAM_S {
    VI_NR_VERSION_E enNRVersion; /* RW;3DNR Version */
    union {
        NRX_PARAM_V1_S stNRXParamV1; /* RW;3DNR X param version 1 */
        NRX_PARAM_V2_S stNRXParamV2; /* RW;3DNR X param version 2 */
    };
} VI_PIPE_NRX_PARAM_S;

/* The attributes of channel */
typedef struct hiVI_CHN_ATTR_S {
    SIZE_S              stSize;             /* RW;Channel out put size */
    PIXEL_FORMAT_E      enPixelFormat;      /* RW;Pixel format */
    DYNAMIC_RANGE_E     enDynamicRange;     /* RW;Dynamic Range */
    VIDEO_FORMAT_E      enVideoFormat;      /* RW;Video format */
    COMPRESS_MODE_E     enCompressMode;     /* RW;256B Segment compress or no compress. */
    HI_BOOL             bMirror;            /* RW;Mirror enable */
    HI_BOOL             bFlip;              /* RW;Flip enable */
    HI_U32              u32Depth;           /* RW;Range [0,8];Depth */
    FRAME_RATE_CTRL_S   stFrameRate;        /* RW;Frame rate */
} VI_CHN_ATTR_S;

/* The status of pipe */
typedef struct hiVI_PIPE_STATUS_S {
    HI_BOOL bEnable;                        /* RO;Whether this pipe is enabled */
    HI_U32  u32IntCnt;                      /* RO;The video frame interrupt count */
    HI_U32  u32FrameRate;                   /* RO;Current frame rate */
    HI_U32  u32LostFrame;                   /* RO;Lost frame count */
    HI_U32  u32VbFail;                      /* RO;Video buffer malloc failure */
    SIZE_S  stSize;                         /* RO;Current pipe output size */
} VI_PIPE_STATUS_S;

/* VS signal output mode */
typedef enum hiVI_VS_SIGNAL_MODE_E {
    VI_VS_SIGNAL_ONCE = 0,                      /* output one time */
    VI_VS_SIGNAL_FREQ,                          /* output frequently */

    VI_VS_SIGNAL_MODE_BUTT
} VI_VS_SIGNAL_MODE_E;

/* The attributes of VS signal */
typedef struct hiVI_VS_SIGNAL_ATTR_S {
    VI_VS_SIGNAL_MODE_E enMode;             /* RW;output one time, output frequently */
    HI_U32              u32StartTime;       /* RW;output start time,unit: sensor pix clk. */
    HI_U32              u32Duration;        /* RW;output high duration, unit: sensor pix clk. */
    HI_U32              u32CapFrmIndex;     /* RW;VS signal will be output after trigger by which vframe,
                                            default is 0. */
    HI_U32              u32Interval;        /* RW;output frequently interval, unit: frame */
} VI_VS_SIGNAL_ATTR_S;

typedef struct hiBNR_DUMP_ATTR_S {
    HI_BOOL bEnable;
    HI_U32 u32Depth;
} BNR_DUMP_ATTR_S;

typedef enum hiVI_EXT_CHN_SOURCE_E {
    VI_EXT_CHN_SOURCE_TAIL,
    VI_EXT_CHN_SOURCE_HEAD,

    VI_EXT_CHN_SOURCE_BUTT
} VI_EXT_CHN_SOURCE_E;

typedef struct hiVI_EXT_CHN_ATTR_S {
    VI_EXT_CHN_SOURCE_E enSource;
    VI_CHN              s32BindChn;     /* RW;Range [VI_CHN0, VI_MAX_PHY_CHN_NUM);The channel num which extend
                                        channel will bind to */
    SIZE_S              stSize;         /* RW;Channel out put size */
    PIXEL_FORMAT_E      enPixFormat;    /* RW;Pixel format */
    DYNAMIC_RANGE_E     enDynamicRange; /* RW;Dynamic Range */
    COMPRESS_MODE_E     enCompressMode; /* RW;256B Segment compress or no compress. */
    HI_U32              u32Depth;       /* RW;Range [0,8];Depth */
    FRAME_RATE_CTRL_S   stFrameRate;    /* RW;Frame rate */
} VI_EXT_CHN_ATTR_S;

typedef enum hiVI_CROP_COORDINATE_E {
    VI_CROP_RATIO_COOR = 0,             /* Ratio coordinate */
    VI_CROP_ABS_COOR,                   /* Absolute coordinate */
    VI_CROP_BUTT
} VI_CROP_COORDINATE_E;

/* Information of chn crop */
typedef struct hiVI_CROP_INFO_S {
    HI_BOOL                 bEnable;            /* RW;CROP enable */
    VI_CROP_COORDINATE_E    enCropCoordinate;   /* RW;Coordinate mode of the crop start point */
    RECT_S                  stCropRect;         /* RW;CROP rectangular */
} VI_CROP_INFO_S;

/* The attributes of LDC */
typedef struct hiVI_LDC_ATTR_S {
    HI_BOOL bEnable; /* RW;Range [0,1];Whether LDC is enbale */
    LDC_ATTR_S stAttr;
} VI_LDC_ATTR_S;

/* The attributes of LDCV2 */
typedef struct hiVI_LDCV2_ATTR_S {
    HI_BOOL bEnable; /* RW;Whether LDC is enbale */
    LDCV2_ATTR_S stAttr;
} VI_LDCV2_ATTR_S;

/* The attributes of LDCV3 */
typedef struct hiVI_LDCV3_ATTR_S {
    HI_BOOL bEnable; /* RW;Whether LDC is enbale */
    LDCV3_ATTR_S stAttr;
} VI_LDCV3_ATTR_S;

typedef struct hiVI_ROTATION_EX_ATTR_S {
    HI_BOOL       bEnable;                 /* RW;Range [0,1];Whether ROTATE_EX_S is enbale */
    ROTATION_EX_S stRotationEx;
} VI_ROTATION_EX_ATTR_S;

/* The status of chn */
typedef struct hiVI_CHN_STATUS_S {
    HI_BOOL bEnable;                    /* RO;Whether this channel is enabled */
    HI_U32  u32FrameRate;               /* RO;current frame rate */
    HI_U32  u32LostFrame;               /* RO;Lost frame count */
    HI_U32  u32VbFail;                  /* RO;Video buffer malloc failure */
    SIZE_S  stSize;                     /* RO;chn output size */

} VI_CHN_STATUS_S;

typedef struct hiVI_PMF_ATTR_S {
    HI_BOOL bEnable;                            /* RW;Whether PMF is enable */
    SIZE_S  stDestSize;                         /* RW;Target size */
    HI_S64  as64PMFCoef[VI_PMFCOEF_NUM];        /* RW; Array of PMF coefficients */
} VI_PMF_ATTR_S;

typedef enum hiVI_DUMP_TYPE_E {
    VI_DUMP_TYPE_RAW = 0,
    VI_DUMP_TYPE_YUV = 1,
    VI_DUMP_TYPE_IR = 2,
    VI_DUMP_TYPE_BUTT
} VI_DUMP_TYPE_E;

typedef struct hiVI_DUMP_ATTR_S {
    HI_BOOL         bEnable;              /* RW;Whether dump is enable */
    HI_U32          u32Depth;             /* RW;Range [0,8];Depth */
    VI_DUMP_TYPE_E  enDumpType;
} VI_DUMP_ATTR_S;

typedef enum hiVI_PIPE_FRAME_SOURCE_E {
    VI_PIPE_FRAME_SOURCE_DEV = 0, /* RW;Source from dev */
    VI_PIPE_FRAME_SOURCE_USER_FE, /* RW;User send to FE */
    VI_PIPE_FRAME_SOURCE_USER_BE, /* RW;User send to BE */

    VI_PIPE_FRAME_SOURCE_BUTT
} VI_PIPE_FRAME_SOURCE_E;

typedef struct hi_VI_RAW_INFO_S {
    VIDEO_FRAME_INFO_S      stVideoFrame;
    ISP_CONFIG_INFO_S       stIspInfo;
} VI_RAW_INFO_S;

/* module params */
typedef struct hiVI_MOD_PARAM_S {
    HI_S32      s32DetectErrFrame;
    HI_U32      u32DropErrFrame;
    VB_SOURCE_E enViVbSource;
} VI_MOD_PARAM_S;

typedef struct hiVI_DEV_TIMING_ATTR_S {
    HI_BOOL bEnable;               /* RW;Range:[0,1];Whether enable VI generate timing */
    HI_S32  s32FrmRate;            /* RW;Range:(0,0xffffff];;Generate timing Frame rate */
} VI_DEV_TIMING_ATTR_S;

typedef struct hiVI_EARLY_INTERRUPT_S {
    HI_BOOL bEnable;
    HI_U32 u32LineCnt;
} VI_EARLY_INTERRUPT_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


