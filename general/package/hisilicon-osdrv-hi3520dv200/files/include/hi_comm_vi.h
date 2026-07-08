/******************************************************************************

Copyright (C), 2004-2020, Hisilicon Tech. Co., Ltd.

******************************************************************************
File Name     : hi_comm_vi.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2009/3/9
Last Modified :
Description   :
Function List :
History       :
1.Date        : 2010/11/16
  Author      : p00123320/w54723/n168968
  Modification: Created file

2.Date        : 2011/06/16
  Author      : w54723/l00181524/c00186004
  Modification: Created file


******************************************************************************/

#ifndef __HI_COMM_VI_H__
#define __HI_COMM_VI_H__

#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define VI_INVALID_FRMRATE  (-1UL)
#define VIU_MAX_USER_FRAME_DEPTH 8

typedef enum hiEN_VIU_ERR_CODE_E
{
    ERR_VI_FAILED_NOTENABLE = 64,   /* device or channel not enable*/
    ERR_VI_FAILED_NOTDISABLE,       /* device not disable*/
    ERR_VI_FAILED_CHNOTDISABLE,     /* channel not disable*/
    ERR_VI_CFG_TIMEOUT,             /* config timeout*/
    ERR_VI_NORM_UNMATCH,            /* video norm of ADC and VIU is unmatch*/
    ERR_VI_INVALID_WAYID,           /* invlalid way ID     */
    ERR_VI_INVALID_PHYCHNID,        /* invalid phychn id*/
    ERR_VI_FAILED_NOTBIND,          /* device or channel not bind */
    ERR_VI_FAILED_BINDED,           /* device or channel not unbind */
} EN_VIU_ERR_CODE_E;

#define HI_ERR_VI_INVALID_PARA          HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VI_INVALID_DEVID         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VI_INVALID_CHNID         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VI_INVALID_NULL_PTR      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VI_FAILED_NOTCONFIG      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define HI_ERR_VI_SYS_NOTREADY          HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VI_BUF_EMPTY             HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_VI_BUF_FULL              HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_VI_NOMEM                 HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VI_NOT_SUPPORT           HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VI_BUSY                  HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VI_NOT_PERM              HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define HI_ERR_VI_FAILED_NOTENABLE      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTENABLE)/* 0xA0108040*/
#define HI_ERR_VI_FAILED_NOTDISABLE     HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTDISABLE)/* 0xA0108041*/
#define HI_ERR_VI_FAILED_CHNOTDISABLE   HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_CHNOTDISABLE)/* 0xA0108042*/
#define HI_ERR_VI_CFG_TIMEOUT           HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_CFG_TIMEOUT)/* 0xA0108043*/
#define HI_ERR_VI_NORM_UNMATCH          HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_NORM_UNMATCH)/* 0xA0108044*/
#define HI_ERR_VI_INVALID_WAYID         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_WAYID)/* 0xA0108045*/
#define HI_ERR_VI_INVALID_PHYCHNID      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_PHYCHNID)/* 0xA0108046*/
#define HI_ERR_VI_FAILED_NOTBIND        HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTBIND)/* 0xA0108047*/
#define HI_ERR_VI_FAILED_BINDED         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_BINDED)/* 0xA0108048*/


/*get the subchn index by main chn */
#define  SUBCHN(ViChn)   (ViChn + 16)

/* define cascade chn */
#define VI_CAS_CHN_1   32
#define VI_CAS_CHN_2   33


/* interface mode of video input */
typedef enum hiVI_INTF_MODE_E
{
    VI_MODE_BT656 = 0,              /* ITU-R BT.656 YUV4:2:2 */
    VI_MODE_BT601,                  /* ITU-R BT.601 YUV4:2:2 */
    VI_MODE_DIGITAL_CAMERA,         /* digatal camera mode */
    VI_MODE_BT1120_STANDARD,        /* BT.1120 progressive mode */
    VI_MODE_BT1120_INTERLEAVED,     /* BT.1120 interstage mode */

    VI_MODE_BUTT
} VI_INTF_MODE_E;


/* Input mode */
typedef enum hiVI_INPUT_MODE_E
{
    VI_INPUT_MODE_BT656 = 0,        /* ITU-R BT.656 YUV4:2:2 */
    VI_INPUT_MODE_BT601,            /* ITU-R BT.601 YUV4:2:2 */
    VI_INPUT_MODE_DIGITAL_CAMERA,   /* digatal camera mode */
    VI_INPUT_MODE_INTERLEAVED,

    VI_INPUT_MODE_BUTT
} VI_INPUT_MODE_E;

typedef enum hiVI_WORK_MODE_E
{
    VI_WORK_MODE_1Multiplex = 0,    /* 1 Multiplex mode */
    VI_WORK_MODE_2Multiplex,        /* 2 Multiplex mode */
    VI_WORK_MODE_4Multiplex,        /* 4 Multiplex mode */

    VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;



/* whether an input picture is interlaced or progressive */
typedef enum hiVI_SCAN_MODE_E
{
    VI_SCAN_INTERLACED  = 0,
    VI_SCAN_PROGRESSIVE,

    VI_SCAN_BUTT,
} VI_SCAN_MODE_E;

typedef enum hiVI_DATA_YUV_SEQ_E
{
   /*The input sequence of the second component(only contains u and v) in BT.1120 mode */
    VI_INPUT_DATA_VUVU = 0,
    VI_INPUT_DATA_UVUV,

   /* The input sequence for yuv */
    VI_INPUT_DATA_UYVY = 0,
    VI_INPUT_DATA_VYUY,
    VI_INPUT_DATA_YUYV,
    VI_INPUT_DATA_YVYU,

    VI_DATA_YUV_BUTT
} VI_DATA_YUV_SEQ_E;

typedef enum hiVI_CLK_EDGE_E
{
    VI_CLK_EDGE_SINGLE_UP = 0,         /* single-edge mode and in rising edge */
    VI_CLK_EDGE_SINGLE_DOWN,           /* single-edge mode and in falling edge */
    //VI_CLK_EDGE_DOUBLE ,                 /* Double edge mode */

    VI_CLK_EDGE_BUTT
} VI_CLK_EDGE_E;

typedef enum hiVI_COMP_MODE_E
{
    VI_COMP_MODE_SINGLE = 0,           /* in single component mode */
    VI_COMP_MODE_DOUBLE = 1,           /* in double component mode */
    VI_COMP_MODE_BUTT,
}VI_COMP_MODE_E;

/* Y/C composite or separation mode */
typedef enum hiVI_COMBINE_MODE_E
{
    VI_COMBINE_COMPOSITE = 0,     /* Composite mode */
    VI_COMBINE_SEPARATE,          /* Separate mode */
    VI_COMBINE_BUTT,
} VI_COMBINE_MODE_E;

/* Attribute of the vertical synchronization signal */
typedef enum hiVI_VSYNC_E
{
    VI_VSYNC_FIELD = 0,           /* Field/toggle mode:a signal reversal means a new frame or a field */
    VI_VSYNC_PULSE,               /* Pusle/effective mode:a pusle or an effective signal means a new frame or a field */
} VI_VSYNC_E;

/* Polarity of the vertical synchronization signal */
typedef enum hiVI_VSYNC_NEG_E
{
    VI_VSYNC_NEG_HIGH = 0,        /*if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the vertical synchronization signal of even field is high-level,
                                        if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization pulse is positive pulse.*/
    VI_VSYNC_NEG_LOW              /*if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the vertical synchronization signal of even field is low-level,
                                        if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization pulse is negative pulse.*/
} VI_VSYNC_NEG_E;

/* Attribute of the horizontal synchronization signal */
typedef enum hiVI_HSYNC_E
{
    VI_HSYNC_VALID_SINGNAL = 0,   /* the horizontal synchronization is valid signal mode */
    VI_HSYNC_PULSE,               /* the horizontal synchronization is pulse mode, a new pulse means the beginning of a new line */
} VI_HSYNC_E;

/* Polarity of the horizontal synchronization signal */
typedef enum hiVI_HSYNC_NEG_E
{
    VI_HSYNC_NEG_HIGH = 0,        /*if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid horizontal synchronization signal is high-level;
                                        if VI_HSYNC_E = VI_HSYNC_PULSE,then the horizontal synchronization pulse is positive pulse */
    VI_HSYNC_NEG_LOW              /*if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid horizontal synchronization signal is low-level;
                                        if VI_HSYNC_E = VI_HSYNC_PULSE,then the horizontal synchronization pulse is negative pulse */
} VI_HSYNC_NEG_E;

/* Attribute of the valid vertical synchronization signal */
typedef enum hiVI_VSYNC_VALID_E
{
    VI_VSYNC_NORM_PULSE = 0,      /* the vertical synchronization is pusle mode, a pusle means a new frame or field  */
    VI_VSYNC_VALID_SINGAL,        /* the vertical synchronization is effective mode, a effective signal means a new frame or field */
} VI_VSYNC_VALID_E;

/* Polarity of the valid vertical synchronization signal */
typedef enum hiVI_VSYNC_VALID_NEG_E
{
    VI_VSYNC_VALID_NEG_HIGH = 0,  /*if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a positive pulse means vertical synchronization pulse;
                                        if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,the valid vertical synchronization signal is high-level */
    VI_VSYNC_VALID_NEG_LOW        /*if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a negative pulse means vertical synchronization pulse;
                                        if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,the valid vertical synchronization signal is low-level */
} VI_VSYNC_VALID_NEG_E;



/* Blank information of the input timing */
typedef struct hiVI_TIMING_BLANK_S
{
    HI_U32 u32HsyncHfb ;    /* Horizontal front blanking width */
    HI_U32 u32HsyncAct ;    /* Horizontal effetive width */
    HI_U32 u32HsyncHbb ;    /* Horizontal back blanking width */
    HI_U32 u32VsyncVfb ;    /* Vertical front blanking height of one frame or odd-field frame picture */
    HI_U32 u32VsyncVact ;   /* Vertical effetive width of one frame or odd-field frame picture */
    HI_U32 u32VsyncVbb ;    /* Vertical back blanking height of one frame or odd-field frame picture */
    HI_U32 u32VsyncVbfb ;   /* Even-field vertical front blanking height when input mode is interlace (invalid when progressive input mode) */
    HI_U32 u32VsyncVbact ;  /* Even-field vertical effetive width when input mode is interlace (invalid when progressive input mode) */
    HI_U32 u32VsyncVbbb ;   /* Even-field vertical back blanking height when input mode is interlace (invalid when progressive input mode) */
}VI_TIMING_BLANK_S;

/* synchronization information about the BT.601 or DC timing */
typedef struct hiVI_SYNC_CFG_S
{
    VI_VSYNC_E             enVsync;
    VI_VSYNC_NEG_E         enVsyncNeg;
    VI_HSYNC_E             enHsync;
    VI_HSYNC_NEG_E         enHsyncNeg;
    VI_VSYNC_VALID_E       enVsyncValid;
    VI_VSYNC_VALID_NEG_E   enVsyncValidNeg;
    VI_TIMING_BLANK_S      stTimingBlank;
} VI_SYNC_CFG_S;

/* the highest bit of the BT.656 timing reference code*/
typedef enum hiBT656_FIXCODE_E
{
    BT656_FIXCODE_1 = 0,       /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 1.*/
    BT656_FIXCODE_0            /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 0.*/
}BT656_FIXCODE_E;

/* Polarity of the field indicator bit (F) of the BT.656 timing reference code */
typedef enum hiBT656_FIELD_POLAR_E
{
    BT656_FIELD_POLAR_STD = 0, /* the standard BT.656 mode,the first filed F=0,the second filed F=1*/
    BT656_FIELD_POLAR_NSTD     /* the non-standard BT.656 mode,the first filed F=1,the second filed F=0*/
}BT656_FIELD_POLAR_E;

typedef struct hiVI_BT656_SYNC_CFG_S
{
    BT656_FIXCODE_E           enFixCode;
    BT656_FIELD_POLAR_E         enFieldPolar;
}VI_BT656_SYNC_CFG_S;

typedef enum hiVI_VBI_LOCAL_E
{
    VI_VBI_LOCAL_ODD_FRONT = 0,
    VI_VBI_LOCAL_ODD_END,
    VI_VBI_LOCAL_EVEN_FRONT,
    VI_VBI_LOCAL_EVEN_END,
    VI_VBI_LOCAL_BUTT
} VI_VBI_LOCAL_E;

typedef struct hiVI_VBI_ATTR_S
{
    VI_VBI_LOCAL_E enLocal; /* location of VBI */
    HI_S32 s32X;            /* horizontal original position of the VBI data */
    HI_S32 s32Y;            /* vertical original position of the VBI data */
    HI_U32 u32Len;          /* length of VBI data, by word(4 Bytes) */
} VI_VBI_ATTR_S;

typedef enum hiVI_DATA_TYPE_E
{
    VI_DATA_TYPE_YUV = 0,
    VI_DATA_TYPE_RGB = 1,
    VI_DATA_TYPE_BUTT
} VI_DATA_TYPE_E;

typedef enum hiVI_DATA_PATH_E
{
    VI_PATH_BYPASS    = 0,        /* ISP bypass */
    VI_PATH_ISP       = 1,        /* ISP enable */
    VI_PATH_RAW       = 2,        /* Capture raw data, for debug */
    VI_PATH_BUTT
}VI_DATA_PATH_E;

/* the extended attributes of VI device */
typedef struct hiVI_DEV_ATTR_EX_S
{
    VI_INPUT_MODE_E     enInputMode;        /* Input mode */
    VI_WORK_MODE_E      enWorkMode;         /*1-, 2-, or 4-channel multiplexed work mode */

    VI_COMBINE_MODE_E   enCombineMode;      /* Y/C composite or separation mode */
    VI_COMP_MODE_E      enCompMode;         /* Component mode (single-component or dual-component) */
    VI_CLK_EDGE_E       enClkEdge;          /* Clock edge mode (sampling on the rising or falling edge) */

    HI_U32              au32CompMask[2];    /* Component mask */

    VI_SCAN_MODE_E      enScanMode;         /* Input scanning mode (progressive or interlaced) */
    HI_S32              s32AdChnId[4];      /* AD channel ID. Typically, the default value -1 is recommended */

    VI_DATA_YUV_SEQ_E   enDataSeq;          /* Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S       stSynCfg;           /* Sync timing. This member must be configured in BT.601 mode or DC mode */

    VI_BT656_SYNC_CFG_S stBT656SynCfg;      /* Sync timing. This member must be configured in BT.656 mode */

    VI_DATA_PATH_E      enDataPath;         /* ISP enable or bypass */
    VI_DATA_TYPE_E      enInputDataType;    /* RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable. */

    HI_BOOL             bDataRev;           /* Data reverse */
} VI_DEV_ATTR_EX_S;

/* the attributes of a VI device */
typedef struct hiVI_DEV_ATTR_S
{
    VI_INTF_MODE_E      enIntfMode;         /* Interface mode */
    VI_WORK_MODE_E      enWorkMode;         /*1-, 2-, or 4-channel multiplexed work mode */

    HI_U32              au32CompMask[2];    /* Component mask */
    VI_SCAN_MODE_E      enScanMode;         /* Input scanning mode (progressive or interlaced) */
    HI_S32              s32AdChnId[4];      /* AD channel ID. Typically, the default value -1 is recommended */

    /* The below members must be configured in BT.601 mode or DC mode and are invalid in other modes */
    VI_DATA_YUV_SEQ_E   enDataSeq;          /* Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S       stSynCfg;           /* Sync timing. This member must be configured in BT.601 mode or DC mode */

    VI_DATA_PATH_E      enDataPath;         /* ISP enable or bypass */
    VI_DATA_TYPE_E      enInputDataType;    /* RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable. */

    HI_BOOL             bDataRev;           /* Data reverse */
} VI_DEV_ATTR_S;


typedef struct hiVI_CHN_BIND_ATTR_S
{
    VI_DEV ViDev;
    VI_WAY ViWay;
} VI_CHN_BIND_ATTR_S;


/* the attributes of a VI way */
typedef struct hiVI_WAY_ATTR_S
{
   HI_S32 s32AdChnId;
} VI_WAY_ATTR_S;


/* captrue selection of video input */
typedef enum hiVI_CAPSEL_E
{
    VI_CAPSEL_TOP = 0,                  /* top field */
    VI_CAPSEL_BOTTOM,                   /* bottom field */
    VI_CAPSEL_BOTH,                     /* top and bottom field */
    VI_CAPSEL_BUTT
} VI_CAPSEL_E;


/* the attributes of a VI channel */
typedef struct hiVI_CHN_ATTR_S
{
    RECT_S          stCapRect;          /* the capture rect (corresponding to the size of the picture captured by a VI device).
                                                For primary channels, the stCapRect's u32Width and u32Height are static attributes. That is,
                                                the value of them can be changed only after primary and secondary channels are disabled.
                                                For secondary channels, stCapRect is an invalid attribute */
    SIZE_S          stDestSize;         /* Target picture size.
                                                For primary channels, stDestSize must be equal to stCapRect's u32Width and u32Height,
                                                because primary channel doesn't have scale capability. Additionally, it is a static
                                                attribute, That is, the value of stDestSize can be changed only after primary and
                                                secondary channels are disabled.
                                                For secondary channels, stDestSize is a dynamic attribute */

    VI_CAPSEL_E     enCapSel;           /* Frame/field select. It is used only in interlaced mode.
                                                For primary channels, enCapSel is a static attribute */

    PIXEL_FORMAT_E  enPixFormat;        /* Pixel storage format. Only the formats semi-planar420 and semi-planar422 are supported */
    HI_BOOL         bMirror;            /* Whether to mirror */
    HI_BOOL         bFlip;              /* Whether to flip */
    HI_BOOL         bChromaResample;    /* Whether to perform chrominance resampling. It is valid only for primary channels */
    HI_S32          s32SrcFrameRate;    /* Source frame rate. The value -1 indicates that the frame rate is not controlled */
    HI_S32          s32FrameRate;       /* Target frame rate. The value -1 indicates that the frame rate is not controlled */
} VI_CHN_ATTR_S;


typedef struct hiVI_CHN_STAT_S
{
    HI_BOOL bEnable;                    /* Whether this channel is enabled */
    HI_U32 u32IntCnt;                   /* The video frame interrupt count */
    HI_U32 u32FrmRate;                  /* current frame rate */
    HI_U32 u32LostInt;                  /* The interrupt is received but nobody care */
    HI_U32 u32VbFail;                   /* Video buffer malloc failure */
    HI_U32 u32PicWidth;                 /* curren pic width */
    HI_U32 u32PicHeight;                /* current pic height */
    HI_U32 u32AutoDisInt;               /* auto disable interrupt count, when VIU detected too many interrupts */
} VI_CHN_STAT_S;

typedef enum hi_VI_USERPIC_MODE_E
{
    VI_USERPIC_MODE_PIC = 0,            /* YUV picture */
    VI_USERPIC_MODE_BGC,                /* Background picture only with a color */
    VI_USERPIC_MODE_BUTT,
} VI_USERPIC_MODE_E;

typedef struct hiVI_USERPIC_BGC_S
{
    HI_U32          u32BgColor;
} VI_USERPIC_BGC_S;

typedef struct hiVI_USERPIC_ATTR_S
{
    HI_BOOL                 bPub;          /* Whether the user picture information is shared by all VI devices and channels*/
    VI_USERPIC_MODE_E       enUsrPicMode;  /* User picture mode */
    union
    {
        VIDEO_FRAME_INFO_S  stUsrPicFrm;   /* Information about a YUV picture */
        VI_USERPIC_BGC_S    stUsrPicBg;    /* Information about a background picture only with a color */
    }unUsrPic;
} VI_USERPIC_ATTR_S;

typedef struct hiVI_USR_GET_FRM_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S *pstVFrame;
    HI_U32              u32MilliSec;
} VI_USR_GET_FRM_TIMEOUT_S;


typedef enum hiVI_FLASH_MODE_E
{
    VI_FLASH_ONCE = 0,		            /* Flash one time */
    VI_FLASH_FREQ = 1,                  /* Flash frequently */
    VI_FLASH_MODE_BUTT
}VI_FLASH_MODE_E;

typedef struct hiVI_FlASH_CONFIG_S
{
    VI_FLASH_MODE_E enFlashMode;	    /* Flash one time, flash frequently*/

    HI_U32 u32StartTime;	            /* Flash start time£¬unit: sensor pix clk.*/
    HI_U32 u32Duration;                 /* Flash high duration, unit: sensor pix clk.*/
    HI_U32 u32CapFrmIndex;              /* Set which vframe will be bFlashed after flashing, default is 0. */
    HI_U32 u32Interval;                 /* Flash frequently interval, unit: frame*/
}VI_FLASH_CONFIG_S;

typedef struct hiVI_EXT_CHN_ATTR_S
{
    VI_CHN    s32BindChn;		        /* The channel num which extend channel will bind to*/
    SIZE_S    stDestSize;		        /* Target size*/

    HI_S32    s32SrcFrameRate;          /* Source frame rate. The value -1 indicates that the frame rate is not controlled */
    HI_S32    s32FrameRate;             /* Target frame rate. The value -1 indicates that the frame rate is not controlled */
    PIXEL_FORMAT_E  enPixFormat;        /* Pixel storage format. Only the formats semi-planar420 and semi-planar422 are supported */
}VI_EXT_CHN_ATTR_S;


typedef struct hiVI_LDC_ATTR_S
{
    HI_BOOL bEnable;                    /* Whether LDC is enbale */
    LDC_ATTR_S stAttr;                  /* LDC Attribute */
}VI_LDC_ATTR_S;

typedef struct hiVI_CHN_LUM_S
{
    HI_U32 u32Lum;                      /* Luma sum of current frame */
    HI_U64 u64Pts;                      /* PTS of current frame */
} VI_CHN_LUM_S;

typedef enum hiVI_CSC_TYPE_E
{
    VI_CSC_TYPE_601 = 0,                /* CSC Type: 601 */
    VI_CSC_TYPE_709,                    /* CSC Type: 709 */
    VI_CSC_TYPE_BUTT,
} VI_CSC_TYPE_E;

typedef struct hiVI_CSC_ATTR_S
{
    VI_CSC_TYPE_E enViCscType;          /* 601 or 709 */
    HI_U32 u32LumaVal;                  /* Luminance: [0 ~ 100] */
    HI_U32 u32ContrVal;                 /* Contrast: [0 ~ 100] */
    HI_U32 u32HueVal;                   /* Hue: [0 ~ 100] */
    HI_U32 u32SatuVal;                  /* Satuature: [0 ~ 100] */
} VI_CSC_ATTR_S;

typedef enum hiVI_PORTMODE_E
{
    VI_PORTMODE_D1 = 0,                /* D1/960H mode */
    VI_PORTMODE_720P,                  /* 720P mode */
    VI_PORTMODE_BUTT,
} VI_PORTMODE_E;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef__HI_COMM_VIDEO_IN_H__ */


