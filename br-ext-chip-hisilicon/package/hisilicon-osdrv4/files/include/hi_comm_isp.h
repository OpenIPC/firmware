/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_isp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/12/20
  Description   :
  History       :
  1.Date        : 2010/12/20
    Author      :
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_ISP_H__
#define __HI_COMM_ISP_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common.h"
#include "hi_isp_debug.h"
#include "hi_comm_video.h"
#include "hi_isp_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/

#define RES_WIDTH_MAX(dev)              (g_au16ResWMax[dev])
#define RES_WIDTH_MIN                   (120)
#define RES_HEIGHT_MAX(dev)             (g_au16ResHMax[dev])
#define RES_HEIGHT_MIN                  (120)
#define FRAME_RATE_MAX                  (65535.0)

#define VREG_MAX_NUM                    (16)

#define AWB_ZONE_ORIG_ROW               (32)
#define AWB_ZONE_ORIG_COLUMN            (32)
#define AE_ZONE_ROW                     (15)
#define AE_ZONE_COLUMN                  (17)
#define MG_ZONE_ROW                     (15)
#define MG_ZONE_COLUMN                  (17)
#define AWB_ZONE_NUM                    (AWB_ZONE_ORIG_ROW * AWB_ZONE_ORIG_COLUMN * AWB_ZONE_BIN)
#define AWB_ZONE_STITCH_MAX             (AWB_ZONE_NUM * ISP_MAX_STITCH_NUM)
#define AF_ZONE_ROW                     (15)
#define AF_ZONE_COLUMN                  (17)
#define LIGHTSOURCE_NUM                 (4)
#define MAX_AWB_LIB_NUM                 (AWB_LIB_NUM)
#define MAX_AE_LIB_NUM                  (AE_LIB_NUM)

#define AE_MIN_WIDTH                    (256)
#define AE_MIN_HEIGHT                   (120)
#define AWB_MIN_WIDTH                   (60)
#define AWB_MIN_HEIGHT                  (14)

#define GAMMA_NODE_NUM                  (1025)
#define PREGAMMA_NODE_NUM               (257)
#define PREGAMMA_SEG_NUM                (8)

#define LCAC_STRENGTH_NUM               (16)

#define ISP_AUTO_ISO_STRENGTH_NUM       (16)
#define ISP_SHARPEN_LUMA_NUM            (32)
#define ISP_SHARPEN_GAIN_NUM            (32)

#define ISP_MAX_SNS_REGS                (32)

#define HI_ISP_RLSC_POINTS                (129)
#define HI_ISP_RLSC_DEFAULT_RADIAL_STR    (4096)
#define HI_ISP_RLSC_DEFAULT_SCALE         (3)
#define HI_ISP_RLSC_DEFAULT_MANUAL_WEIGHT (256)
#define HI_ISP_RLSC_DEFAULT_WBGAIN        (256)
#define HI_ISP_RLSC_DEFAULT_LIGHT         (0)
#define HI_ISP_RLSC_WEIGHT_Q_BITS         (8)

#define ISP_VREG_SIZE_BIN               (0x20000)
#define ALG_LIB_VREG_SIZE_BIN           (0x1000)
#define WDR_MAX_FRAME_NUM               (4)

#define BAYER_CALIBTAION_MAX_NUM        (50)
#define HI_ISP_BAYERNR_STRENGTH_DIVISOR (100)
#define HI_ISP_BAYERNR_CORINGLOW_STRENGTH_DIVISOR (10000)
#define AI_MAX_STEP_FNO_NUM             (1024)

#define ISP_BAS_TAPS_ROW_NUM            (17)
#define ISP_BAS_TAPS_COL_6              (6)
#define ISP_BAS_TAPS_COL_4              (4)
#define HI_ISP_BAYERNR_LUT_LENGTH       (33)
#define HI_ISP_BAYERNR_LMTLUTNUM        (129)
#define PRO_MAX_FRAME_NUM               (8)

#define HI_ISP_DEMOSAIC_LUT_LENGTH      (17)
#define ISP_BAYER_CHN_NUM               (4)
#define BAYER_CHN_R                     0
#define BAYER_CHN_GR                    1
#define BAYER_CHN_GB                    2
#define BAYER_CHN_B                     3
#define LDCI_LPF_LUT_SIZE               (9)
#define LDCI_HE_LUT_SIZE                (33)
#define LDCI_DE_USM_LUT_SIZE            (33)
#define LDCI_COLOR_GAIN_LUT_SIZE        (65)

#define NoiseSet_EleNum                 (7)
#define CCM_MATRIX_SIZE                 (9)
#define CCM_MATRIX_NUM                  (7)

#define HI_ISP_DRC_CUBIC_POINT_NUM      (5)
#define HI_ISP_DRC_CC_NODE_NUM          (33)
#define HI_ISP_DRC_TM_NODE_NUM          (200)
#define HI_ISP_DRC_TM_SEG_NUM           (8)
#define HI_ISP_DRC_EXP_COMP_SAMPLE_NUM  (8)
#define HI_ISP_DRC_SHP_LOG_CONFIG_NUM   (16)

#define LOG_LUT_SIZE                    (1025)
#define PRE_LOG_LUT_SIZE                (1025)

#define HI_ISP_DE_LUMA_GAIN_LUT_N       (17)
#define ISP_EXP_RATIO_STRENGTH_NUM      (16)

#define ISP_DO_NOT_NEED_SWITCH_IMAGEMODE (-2)

/*****************************************************************************
*******DEFAULT VALUE OF GLOBAL REGISTERS DEFINED HERE*************************
*****************************************************************************/

/* ----------------------- AE ------------------------------- */
#define HI_ISP_TOP_RGGB_START_R_GR_GB_B           (0)
#define HI_ISP_TOP_RGGB_START_GR_R_B_GB           (1)
#define HI_ISP_TOP_RGGB_START_GB_B_R_GR           (2)
#define HI_ISP_TOP_RGGB_START_B_GB_GR_R           (3)

#define HI_ISP_TOP_AE_SELECT_AFTER_DG             (0)
#define HI_ISP_TOP_AE_SELECT_AFTER_WB             (1)
#define HI_ISP_TOP_AE_SELECT_AFTER_DRC            (2)

#define HI_ISP_AE_FOUR_PLANE_MODE_DISABLE         (0)
#define HI_ISP_AE_FOUR_PLANE_MODE_ENABLE          (1)

/* ----------------------- AWB ------------------------------- */
#define HI_ISP_AWB_OFFSET_COMP_DEF                (0)
#define HI_ISP_CCM_COLORTONE_EN_DEFAULT           (1)
#define HI_ISP_CCM_COLORTONE_RGAIN_DEFAULT        (256)
#define HI_ISP_CCM_COLORTONE_BGAIN_DEFAULT        (256)
#define HI_ISP_CCM_COLORTONE_GGAIN_DEFAULT        (256)

#define HI_ISP_CCM_PROT_EN_DEFAULT                (0)
#define HI_ISP_CCM_CC_THD0_DEFAULT                (0)
#define HI_ISP_CCM_CC_THD1_DEFAULT                (0x1E)
#define HI_ISP_CCM_CC_PROT_RATIO_DEFAULT          (10)
#define HI_ISP_CCM_RR_THD0_DEFAULT                (0x1E)
#define HI_ISP_CCM_RR_THD1_DEFAULT                (0x64)
#define HI_ISP_CCM_GG_THD0_DEFAULT                (0x6)
#define HI_ISP_CCM_GG_THD1_DEFAULT                (0x3C)
#define HI_ISP_CCM_BB_THD0_DEFAULT                (0x1E)
#define HI_ISP_CCM_BB_THD1_DEFAULT                (0x64)
#define HI_ISP_CCM_MAX_RGB_DEFAULT                (0xC8)
#define HI_ISP_CCM_RGB_PROT_RATIO_DEFAULT         (10)
#define HI_ISP_CCM_RECOVER_EN_DEFAULT             (1)
#define HI_ISP_CCM_LUM_RATIO_DEFAULT              (256)
#define HI_ISP_CCM_HUE_RATIO_DEFAULT              (256)

#define CCM_CONVERT_PRE(value)  ((value & 0x8000) | (value << 2))
#define CCM_CONVERT(value)      ((value & 0x8000)?((~(value & 0x7FFF)) + 1):(value))
/********************************** DEFINED END **************************************/

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/
/*
ISP Error Code
0x40 : ISP_NOT_INIT
0x41 : ISP_MEM_NOT_INIT
0x42 : ISP_ATTR_NOT_CFG
0x43 : ISP_SNS_UNREGISTER
0x44 : ISP_INVALID_ADDR
0x45 : ISP_NOMEM
0x46 : ISP_NO_INT
*/
typedef enum hiISP_ERR_CODE_E {
    ERR_ISP_NOT_INIT                = 0x40, // ISP not init
    ERR_ISP_MEM_NOT_INIT            = 0x41, // ISP memory not init
    ERR_ISP_ATTR_NOT_CFG            = 0x42, // ISP attribute not cfg
    ERR_ISP_SNS_UNREGISTER          = 0x43, // ISP sensor unregister
    ERR_ISP_INVALID_ADDR            = 0x44, // ISP invalid address
    ERR_ISP_NOMEM                   = 0x45, // ISP nomem
    ERR_ISP_NO_INT                  = 0x46, // ISP
} ISP_ERR_CODE_E;

#define HI_ERR_ISP_NULL_PTR         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_ISP_ILLEGAL_PARAM    HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_ISP_NOT_SUPPORT      HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)

#define HI_ERR_ISP_NOT_INIT         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define HI_ERR_ISP_MEM_NOT_INIT     HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_MEM_NOT_INIT)
#define HI_ERR_ISP_ATTR_NOT_CFG     HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define HI_ERR_ISP_SNS_UNREGISTER   HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define HI_ERR_ISP_INVALID_ADDR     HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define HI_ERR_ISP_NOMEM            HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)
#define HI_ERR_ISP_NO_INT           HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NO_INT)

/*
Defines the structure of ISP module parameters.
*/
typedef struct hiISP_MOD_PARAM_S {
    HI_U32      u32IntBotHalf;  /* RW;Range:[0,1]; Format:32.0; Indicate ISP interrupt bottom half,No distinction vipipe */
    HI_U32      u32QuickStart;  /* RW;Range:[0,1]; Format:32.0; Indicate ISP Quick Start No distinction vipipe. Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_BOOL     bLongFrmIntEn; /*when wdr mode enable/disable long frame pipe interrupt.Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_MOD_PARAM_S;

/*
Defines the structure of ISP control parameters.
*/
typedef struct hiISP_CTRL_PARAM_S {
    HI_U32      u32ProcParam;    /* RW;Format:32.0; Indicate the update frequency of ISP_PROC information,No distinction vipipe */
    HI_U32      u32StatIntvl;    /* RW;Range:(0,0xffffffff];Format:32.0; Indicate the time interval of ISP statistic information */
    HI_U32      u32UpdatePos;    /* RW;Range:[0,1]; Format:32.0; Indicate the location of the configuration register of ISP interrupt */
    HI_U32      u32IntTimeOut;   /* RW;Format:32.0; Indicate the time(unit:ms) of interrupt timeout */
    HI_U32      u32PwmNumber;    /* R;Format:32.0; Indicate PWM number */
    HI_U32      u32PortIntDelay; /* RW;Format:32.0; Port intertupt delay value, unit:clk */
    HI_BOOL     bLdciTprFltEn;  /* R;Indicate Enable LDCI temporal filter. Not support changed after isp init */
} ISP_CTRL_PARAM_S;

/*
Defines the working mode of ISP
0 = automatic mode
1 = manual mode
*/
typedef enum hiISP_OP_TYPE_E {
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_BUTT
} ISP_OP_TYPE_E;


/*
Defines the prior frame of ISP
0 = short frame
1 = middle frame
2 = second middle frame
3 = long frame
*/
typedef enum hiISP_PRIOR_FRAME_E {
    LONG_FRAME      = 0,
    SHORT_FRAME       = 1,
    PRIOR_FRAME_BUTT
} ISP_PRIOR_FRAME_E;

/*
Defines the ISP correction or detection status
0 = initial status, no calibration
1 = The static defect pixel calibration ends normally
2 = The static defect pixel calibration ends due to timeout.
*/
typedef enum hiISP_STATE_E {
    ISP_STATE_INIT     = 0,
    ISP_STATE_SUCCESS  = 1,
    ISP_STATE_TIMEOUT  = 2,
    ISP_STATE_BUTT
} ISP_STATUS_E;

typedef struct hiISP_PIPE_DIFF_ATTR_S {
    HI_S32 as32Offset[ISP_BAYER_CHN_NUM];   /* RW;Range:[-4095,4095];Format:12.0; Black level differences between the offset value */
    HI_U32 au32Gain[ISP_BAYER_CHN_NUM];     /* RW;Range:[0x80,0x400];Format:4.8; The gain difference ratio */
    HI_U16 au16ColorMatrix[CCM_MATRIX_SIZE]; /* RW;Range:[0x0,0xFFFF];Format:8.8; Color correction matrix variance ratio */
} ISP_PIPE_DIFF_ATTR_S;

/* Defines the format of the input Bayer image */
typedef enum hiISP_BAYER_FORMAT_E {
    BAYER_RGGB    = 0,
    BAYER_GRBG    = 1,
    BAYER_GBRG    = 2,
    BAYER_BGGR    = 3,
    BAYER_BUTT
} ISP_BAYER_FORMAT_E;

/* Defines the bitwidth of the input Bayer image, used for lsc online calibration */
typedef enum hiISP_BAYER_RAWBIT_E {
    BAYER_RAWBIT_8BIT    = 8,
    BAYER_RAWBIT_10BIT   = 10,
    BAYER_RAWBIT_12BIT   = 12,
    BAYER_RAWBIT_14BIT   = 14,
    BAYER_RAWBIT_16BIT   = 16,
    BAYER_RAWBIT_BUTT

} ISP_BAYER_RAWBIT_E;



/* ISP public attribute, contains the public image attribute */
typedef struct hiISP_PUB_ATTR_S {
    RECT_S          stWndRect;      /* RW; Start position of the cropping window, image width, and image height */
    SIZE_S          stSnsSize;      /* RW; Width and height of the image output from the sensor */
    HI_FLOAT        f32FrameRate;   /* RW; Range: [0, 0xFFFF]; For frame rate */
    ISP_BAYER_FORMAT_E  enBayer;    /* RW; Range:[0,3] ;Format:2.0;the format of the input Bayer image */
    WDR_MODE_E      enWDRMode;      /* RW; WDR mode select */
    HI_U8           u8SnsMode;      /* RW; Range: [0, 0xFF]; Format: 8.0; For special sensor mode switches */
} ISP_PUB_ATTR_S;

/* Slave mode sensor sync signal generate module */
typedef struct hiISP_SLAVE_SNS_SYNC_S {
    union {
        struct {
            HI_U32  bit16Rsv          :  16;
            HI_U32  bitHInv           :  1;
            HI_U32  bitVInv           :  1;
            HI_U32  bit12Rsv          :  12;
            HI_U32  bitHEnable        :  1;
            HI_U32  bitVEnable        :  1;
        } stBits;
        HI_U32 u32Bytes;
    } unCfg;

    HI_U32  u32VsTime;
    HI_U32  u32HsTime;
    HI_U32  u32VsCyc;
    HI_U32  u32HsCyc;
    HI_U32  u32SlaveModeTime;

} ISP_SLAVE_SNS_SYNC_S;

/*
Defines the ISP stitch attribute.
*/
/*
Defines the ISP firmware status
0 = Running status
1 = Frozen status
*/
typedef enum hiISP_FMW_STATE_E {
    ISP_FMW_STATE_RUN = 0,
    ISP_FMW_STATE_FREEZE,
    ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

/* Defines the WDR mode of the ISP */
typedef struct hiISP_WDR_MODE_S {
    WDR_MODE_E  enWDRMode;
} ISP_WDR_MODE_S;

typedef union hiISP_MODULE_CTRL_U {
    HI_U64  u64Key;
    struct {
        HI_U64  bitBypassISPDGain      : 1 ;   /* RW;[0] */
        HI_U64  bitBypassAntiFC        : 1 ;   /* RW;[1] */
        HI_U64  bitBypassCrosstalkR    : 1 ;   /* RW;[2] */
        HI_U64  bitBypassDPC           : 1 ;   /* RW;[3] */
        HI_U64  bitBypassNR            : 1 ;   /* RW;[4] */
        HI_U64  bitBypassDehaze        : 1 ;   /* RW;[5] */
        HI_U64  bitBypassWBGain        : 1 ;   /* RW;[6] */
        HI_U64  bitBypassMeshShading   : 1 ;   /* RW;[7] */
        HI_U64  bitBypassDRC           : 1 ;   /* RW;[8] */
        HI_U64  bitBypassDemosaic      : 1 ;   /* RW;[9] */
        HI_U64  bitBypassColorMatrix   : 1 ;   /* RW;[10] */
        HI_U64  bitBypassGamma         : 1 ;   /* RW;[11] */
        HI_U64  bitBypassFSWDR         : 1 ;   /* RW;[12] */
        HI_U64  bitBypassCA            : 1 ;   /* RW;[13]; Not support for Hi3559V200/Hi3556V200 */
        HI_U64  bitBypassCsConv        : 1 ;   /* RW;[14] */
        HI_U64  bitBypassRadialCrop    : 1 ;   /* RW;[15]; Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
        HI_U64  bitBypassSharpen       : 1 ;   /* RW;[16] */
        HI_U64  bitBypassLCAC          : 1 ;   /* RW;[17] */
        HI_U64  bitBypassGCAC          : 1 ;   /* RW;[18]; Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bit2ChnSelect          : 2 ;   /* RW;[19:20] */
        HI_U64  bitBypassLdci          : 1 ;   /* RW;[21] */
        HI_U64  bitBypassPreGamma      : 1 ;   /* RW;[22]; Not support for Hi3559V200/Hi3556V200 */
        HI_U64  bitBypassRadialShading : 1 ;   /* RW;[23]; Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bitBypassAEStatFE      : 1 ;   /* RW;[24] */
        HI_U64  bitBypassAEStatBE      : 1 ;   /* RW;[25] */
        HI_U64  bitBypassMGStat        : 1 ;   /* RW;[26] */
        HI_U64  bitBypassDE            : 1 ;   /* RW;[27] ; Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
        HI_U64  bitBypassAFStatFE      : 1 ;   /* RW;[28] ; Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bitBypassAFStatBE      : 1 ;   /* RW;[29] ; Not support for Hi3559V200/Hi3556V200 */
        HI_U64  bitBypassAWBStat       : 1 ;   /* RW;[30] ; */
        HI_U64  bitBypassCLUT          : 1 ;   /* RW;[31] ; Not support for  Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bitBypassHLC           : 1 ;   /* RW;[32] ; Not support for Hi3559AV100/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bitBypassEdgeMark      : 1 ;   /* RW;[33] ; Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bitBypassRGBIR         : 1 ;   /* RW;[34] ; Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bitRsv29               : 29 ;  /* H  ; [35:63] */
    };
} ISP_MODULE_CTRL_U;
/*
Defines the ISP FSWDR attributes in combination mode.
 0 = Short exposure data is used when combining
 1 = Long exposure data is used when combining
 */
typedef enum hiISP_COMBINE_MODE_E {
    FS_WDR_COMBINE_SHORT_FIRST  = 0,
    FS_WDR_COMBINE_LONG_FIRST   = 1,
    FS_WDR_COMBINE_BUTT
} ISP_COMBINE_MODE_E;

/*
Defines the ISP WDR merge mode.
0 = WDR mode is used
1 = Fusion mode is used
*/
typedef enum hiISP_WDR_MERGE_MODE_E {
    MERGE_WDR_MODE      = 0,
    MERGE_FUSION_MODE   = 1,
    MERGE_BUTT
} ISP_WDR_MERGE_MODE_E;

/*
 0 = The function of bnr is off
 1 = The function of bnr is on
*/
typedef enum hiISP_BNR_MODE_E {
    BNR_OFF_MODE         = 0,
    BNR_ON_MODE          = 1,
    BNR_BUTT
} ISP_BNR_MODE_E;

typedef enum hiISP_WDR_WBGAIN_POSITION_E {
    WDR_WBGAIN_IN_DG1         = 0,
    WDR_WBGAIN_IN_WB          = 1,
    WDR_WBGAIN_POSITION_BUTT
} ISP_WDR_WBGAIN_POSITION_E;

typedef struct hiISP_FSWDR_MANUAL_ATTR_S {
    HI_U8 u8MdThrLowGain;
    HI_U8 u8MdThrHigGain;
} ISP_FSWDR_MANUAL_ATTR_S;

typedef struct hiISP_FSWDR_AUTO_ATTR_S {
    HI_U8 au8MdThrLowGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8MdThrHigGain[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_FSWDR_AUTO_ATTR_S;

typedef struct hiISP_MDT_ATTR_S {
    HI_BOOL bShortExpoChk;     /* RW;Range:[0x0,0x1];Format:1.0; */
    HI_U16  u16ShortCheckThd;   /* RW;Range:[0x0,0xFFF];Format:12.0 */
    HI_BOOL bMDRefFlicker;
    HI_U8   u8MdtStillThd;     /* RW;Range:[0x0,0xFE];Format:8.0 */
    HI_U8   u8MdtFullThd;      /* RW;Range:[0x0,0xFE];Format:8.0,Only used for Hi3559AV100 */
    HI_U8   u8MdtLongBlend;    /* RW;Range:[0x0,0xFE] */
    ISP_OP_TYPE_E enOpType;
    ISP_FSWDR_MANUAL_ATTR_S stManual;
    ISP_FSWDR_AUTO_ATTR_S   stAuto;
} ISP_FSWDR_MDT_ATTR_S;


typedef struct hiISP_WDR_COMBINE_ATTR_S {
    HI_BOOL bMotionComp;              /* RW;Range:[0x0,0x1];Format:1.0; HI_TRUE: enable motion compensation; HI_FALSE: disable motion compensation */
    HI_U16  u16ShortThr;              /* RW;Range:[0x0,0xFFF];Format:12.0; Data above this threshold will be taken from short exposure only. */
    HI_U16  u16LongThr;               /* RW;Range:[0x0,0xFFF];Format:12.0; limited range :[0x0,u16ShortThr],Data below this threshold will be taken from long exposure only. */
    HI_BOOL bForceLong;               /* RW;Range:[0x0,0x1];Format:1.0; HI_TRUE: enable Force Long; HI_FALSE: disable Force Long,Not support for Hi3559AV100 */
    HI_U16  u16ForceLongLowThr;       /* RW;Range:[0x0,0xFFF];Format:12.0; Data above this threshold will Force to choose long frame only,Not support for Hi3559AV100 */
    HI_U16  u16ForceLongHigThr;       /* RW;Range:[0x0,0xFFF];Format:12.0; Data below this threshold will Force to choose long frame only,Not support for Hi3559AV100 */
    ISP_FSWDR_MDT_ATTR_S stWDRMdt;
} ISP_WDR_COMBINE_ATTR_S;

typedef struct hiISP_FUSION_ATTR_S {
    HI_U16  au16FusionThr[4];      /* RW;Range:[0x0,0x3FFF];Format:14.0;The threshold of the 4 frame */
} ISP_FUSION_ATTR_S;

typedef struct hiISP_WDR_BNR_ATTR_S {
    ISP_BNR_MODE_E enBnrMode;         /* RW;Range:[0x0,0x1];Format:1.0; HI_TRUE:enable the bnr of wdr;HI_FALSE:disable the bnr of wdr */
    HI_BOOL        bShortFrameNR;     /* RW;Range:[0x0,0x1];Format:1.0; HI_TRUE:enable the bnr of short frame;HI_FALSE:disable the bnr of short frame,Only used for Hi3519AV100 */
    HI_U8          u8ShortFrameNRStr; /* RW;Range:[0x0,0x3F];Format:8.0;the Level 2 short frame sigma weight of G channel,Only used for Hi3519AV100                             */
    HI_U8          u8FusionBnrStr;    /* RW;Range:[0x0,0x3F];Format:3.3,Only used for Hi3519AV100   */
    HI_U8          au8NoiseFloor[NoiseSet_EleNum];  /* RW;Range:[0x0,0xFF];Format:8.0,Only used for Hi3559AV100/Hi3519AV100   */
    HI_U8          u8FullMdtSigGWgt;  /* RW;Range:[0x0,0x1F];Format:8.0,Only used for Hi3519AV100   */
    HI_U8          u8FullMdtSigRBWgt; /* RW;Range:[0x0,0x1F];Format:8.0,Only used for Hi3519AV100  */
    HI_U8          u8FullMdtSigWgt;  /* RW;Range:[0x0,0x1F];Format:8.0,Only used for Hi3559AV100 */
    HI_U8          au8GsigmaGain[3];  /* RW;Range:[0x0,0xFF];Format:8.0,Only used for Hi3559AV100/Hi3519AV100  */
    HI_U8          au8RBsigmaGain[3]; /* RW;Range:[0x0,0xFF];Format:8.0,Only used for Hi3559AV100/Hi3519AV100  */
} ISP_WDR_BNR_ATTR_S;

typedef struct hiISP_WDR_FS_ATTR_S {
    ISP_WDR_MERGE_MODE_E enWDRMergeMode;
    ISP_WDR_COMBINE_ATTR_S stWDRCombine;
    ISP_WDR_BNR_ATTR_S stBnr;         /* Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_FUSION_ATTR_S stFusion;
    ISP_WDR_WBGAIN_POSITION_E enWDRWbgainPosition; /* RW; Range: [0x0, 0x1]; WDR Gain with or without WB Gain. Only used for Hi3559AV100 */
} ISP_WDR_FS_ATTR_S;

typedef struct hiISP_DRC_CUBIC_POINT_ATTR_S {
    HI_U16 u16X;        /* RW; Range:[0,1000];  Format:10.0; x position of the characteristic point of the cubic curve; Only used for Hi3559AV100 */
    HI_U16 u16Y;        /* RW; Range:[0,1000];  Format:10.0; y position of the characteristic point of the cubic curve; Only used for Hi3559AV100 */
    HI_U16 u16Slope;    /* RW; Range:[0,10000]; Format:14.0; slope of the characteristic point of the cubic curve; Only used for Hi3559AV100 */
} ISP_DRC_CUBIC_POINT_ATTR_S;

typedef struct hiISP_DRC_ASYMMETRY_CURVE_ATTR_S {
    HI_U8   u8Asymmetry;              /* RW; Range:[0x1, 0x1E]; Format:5.0; The parameter0 of DRC asymmetry tone mapping curve */
    HI_U8   u8SecondPole;             /* RW; Range:[0x96,0xD2]; Format:8.0; The parameter1 of DRC asymmetry tone mapping curve */
    HI_U8   u8Stretch;                /* RW; Range:[0x1E,0x3C]; Format:6.0; The parameter2 of DRC asymmetry tone mapping curve */
    HI_U8   u8Compress;               /* RW; Range:[0x64,0xC8]; Format:8.0; The parameter3 of DRC asymmetry tone mapping curve */
} ISP_DRC_ASYMMETRY_CURVE_ATTR_S;

typedef struct hiISP_DRC_MANUAL_ATTR_S {
    HI_U16  u16Strength;          /* RW; Range:Hi3559AV100 = [0x0, 0xFF] | Hi3519AV100 = [0x0, 0x3FF] | Hi3516CV500 = [0x0, 0x3FF] | Hi3516DV300 = [0x0, 0x3FF] | Hi3516AV300 = [0x0, 0x3FF] |
                                     Hi3559V200 = [0x0, 0x3FF] | Hi3556V200 = [0x0, 0x3FF] | Hi3516EV200 = [0x0, 0x3FF] | Hi3516EV300 = [0x0, 0x3FF] | Hi3518EV300 = [0x0, 0x3FF] | Hi3516DV200 = [0x0, 0x3FF];
                                     Strength of dynamic range compression. Higher values lead to higher differential gain between shadows and highlights. */
} ISP_DRC_MANUAL_ATTR_S;

typedef struct hiISP_DRC_AUTO_ATTR_S {
    HI_U16  u16Strength;   /* RW; Range:Hi3559AV100 = [0x0, 0xFF] | Hi3519AV100 = [0x0, 0x3FF] | Hi3516CV500 = [0x0, 0x3FF]| Hi3516DV300 = [0x0, 0x3FF] |Hi3516AV300 = [0x0, 0x3FF] |
                                                 Hi3559V200 = [0x0, 0x3FF]| Hi3556V200 = [0x0, 0x3FF] | Hi3516EV200 = [0x0, 0x3FF] | Hi3516EV300 = [0x0, 0x3FF]|Hi3518EV300 = [0x0, 0x3FF]|Hi3516DV200 = [0x0, 0x3FF];
                                                 It is the base strength. The strength used in ISP is generated by firmware.
                                                 n linear mode, strength = f1(u16Strength, histogram)
                                                 In sensor WDR mode: strength = f2(u16Strength, histogram)
                                                 In 2to1 WDR mode: strength = f3(ExpRatio) */

    HI_U16 u16StrengthMax; /* RW; Range: Hi3559AV100 = [0x0, 0xFF] | Hi3519AV100 = [0x0, 0x3FF] | Hi3516CV500 = [0x0, 0x3FF] | Hi3516DV300 = [0x0, 0x3FF] |
                                                 Hi3516AV300 = [0x0, 0x3FF] |Hi3559V200 = [0x0, 0x3FF] | Hi3556V200 = [0x0, 0x3FF] | Hi3516EV200 = [0x0, 0x3FF]|
                                                 Hi3516EV300 = [0x0, 0x3FF] | Hi3518EV300 = [0x0, 0x3FF]| Hi3516DV200 = [0x0, 0x3FF]; Maximum DRC strength in Auto mode */
    HI_U16 u16StrengthMin; /* RW; Range: Hi3559AV100 = [0x0, 0xFF] | Hi3519AV100 = [0x0, 0x3FF] | Hi3516CV500 = [0x0, 0x3FF] | Hi3516DV300 = [0x0, 0x3FF] |
                                                 Hi3516AV300 = [0x0, 0x3FF] | Hi3559V200 = [0x0, 0x3FF] | Hi3556V200 = [0x0, 0x3FF]| Hi3516EV200 = [0x0, 0x3FF] |
                                                 Hi3516EV300 = [0x0, 0x3FF]|Hi3518EV300 = [0x0, 0x3FF]|Hi3516DV200 = [0x0, 0x3FF]; Minimum DRC strength in Auto mode */

} ISP_DRC_AUTO_ATTR_S;


/* DRC curve type: 0 = Asymmetry curve, 1 = Cubic curve, 2 = User-defined curve */
typedef enum hiISP_DRC_CURVE_SELECT_E {
    DRC_CURVE_ASYMMETRY = 0x0,
    DRC_CURVE_CUBIC,      /* Only used for Hi3559AV100 */
    DRC_CURVE_USER,
    DRC_CURVE_BUTT
} ISP_DRC_CURVE_SELECT_E;

typedef struct hiISP_DRC_ATTR_S {
    HI_BOOL bEnable;
    ISP_DRC_CURVE_SELECT_E enCurveSelect;  /* RW; Range:[0x0, 0x2]; Select tone mapping curve type */

    HI_U8  u8PDStrength;           /* RW; Range:[0x0, 0x80]; Format:8.0; Controls the purple detection strength,
                                      Only used for Hi3559AV100/Hi3519AV100/Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    HI_U8  u8LocalMixingBrightMax; /* RW; Range:[0x0, 0x80]; Format:8.0; Maximum enhancement strength for positive detail */
    HI_U8  u8LocalMixingBrightMin; /* RW; Range:[0x0, 0x40]; Format:8.0; Minimum enhancement strength for positive detail */
    HI_U8  u8LocalMixingBrightThr; /* RW; Range:[0x0, 0xFF]; Format:8.0; Luma threshold for enhancement strength adaptation of positive detail */
    HI_S8  s8LocalMixingBrightSlo; /* RW; Range:[-7, 7]; Format:s4.0; Enhancement strength adaptation slope for positive detail */
    HI_U8  u8LocalMixingDarkMax;   /* RW; Range:[0x0, 0x80]; Format:8.0; Maximum enhancement strength for negative detail */
    HI_U8  u8LocalMixingDarkMin;   /* RW; Range:[0x0, 0x40]; Format:8.0; Minimum enhancement strength for negative detail */
    HI_U8  u8LocalMixingDarkThr;   /* RW; Range:[0x0, 0xFF]; Format:8.0; Luma threshold for enhancement strength adaptation of negative detail */
    HI_S8  s8LocalMixingDarkSlo;   /* RW; Range:[-7, 7]; Format:s4.0; Enhancement strength adaptation slope for negative detail */

    HI_U8  u8DetailBrightStr;      /* RW; Range:[0x0, 0x80]; Format:8.0; Controls the gain of the non-linear positive detail enhancement; Only used for Hi3559AV100 */
    HI_U8  u8DetailDarkStr;        /* RW; Range:[0x0, 0x80]; Format:8.0; Controls the gain of the non-linear negative detail enhancement; Only used for Hi3559AV100 */
    HI_U8  u8DetailBrightStep;     /* RW; Range:[0x0, 0x80]; Format:8.0; Controls the step of the non-linear positive detail enhancement; Only used for Hi3559AV100 */
    HI_U8  u8DetailDarkStep;       /* RW; Range:[0x0, 0x80]; Format:8.0; Controls the step of the non-linear negative detail enhancement; Only used for Hi3559AV100 */

    HI_U8  u8BrightGainLmt;        /* RW; Range:[0x0, 0xF];  Format:4.0; Bright area gain high limit */
    HI_U8  u8BrightGainLmtStep;    /* RW; Range:[0x0, 0xF];  Format:4.0; Bright area gain high limit step */
    HI_U8  u8DarkGainLmtY;         /* RW; Range:[0x0, 0x85]; Format:7.0; Dark area luma gain limit */
    HI_U8  u8DarkGainLmtC;         /* RW; Range:[0x0, 0x85]; Format:7.0; Dark area chroma gain limit */
    HI_U16 au16ColorCorrectionLut[HI_ISP_DRC_CC_NODE_NUM]; /* RW; Range:[0x0, 0x400]; Format:4.12; LUT of color correction coefficients */
    HI_U16 au16ToneMappingValue[HI_ISP_DRC_TM_NODE_NUM];   /* RW; Range:[0x0, 0xffff]; Format:16.0; LUT of user-defined curve */

    HI_U8  u8FltScaleCoarse;     /* RW; Range:[0x0, 0xF]; Format:4.0; Spatial filter scale coarse control; Only used for Hi3559AV100 and Hi3519AV100 */
    HI_U8  u8FltScaleFine;       /* RW; Range:[0x0, 0xF]; Format:4.0; Spatial filter scale fine control; Only used for Hi3559AV100 and Hi3519AV100  */
    HI_U8  u8ContrastControl;    /* RW; Range:[0x0, 0xF]; Format:4.0; Contrast control */
    HI_S8  s8DetailAdjustFactor; /* RW; Range:[-15, 15];  Format:4.0; Detail adjustment factor */

    HI_U8  u8SpatialFltCoef;     /* RW; Range: Hi3559AV100 = [0x0, 0xA] | Hi3519AV100 = [0x0, 0x5] | Hi3516CV500 = [0x0, 0x5] | Hi3516DV300 = [0x0, 0x5] |
                                                                      Hi3516AV300 = [0x0, 0x5] |Hi3559V200 = [0x0, 0x5] | Hi3556V200 = [0x0, 0x5] |
                                                                      Hi3516EV200 = [0x0, 0x5] | Hi3516EV300 = [0x0, 0x5] |Hi3518EV300 = [0x0, 0x5]|Hi3516DV200 = [0x0, 0x5];
                                                                      Spatial filter coefficients */
    HI_U8  u8RangeFltCoef;       /* RW; Range:[0x0, 0xA]; Format:4.0; Range filter coefficients */
    HI_U8  u8RangeAdaMax;        /* RW; Range:[0x0, 0x8]; Format:4.0; Maximum range filter coefficient adaptation range */

    HI_U8  u8GradRevMax;         /* RW; Range:[0x0, 0x40]; Format:7.0; Maximum gradient reversal reduction strength */
    HI_U8  u8GradRevThr;         /* RW; Range:[0x0, 0x80]; Format:8.0; Gradient reversal reduction threshold */

    HI_U8  u8DpDetectRangeRatio; /* RW; Range:[0x0, 0x1F]; Format:5.0; DRC defect pixel detection control parameter; Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8  u8DpDetectThrSlo;     /* RW; Range:[0x0, 0x1F]; Format:5.0; DRC defect pixel detection control parameter; Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16 u16DpDetectThrMin;    /* RW; Range:[0x0, 0xFFFF]; Format:16.0; DRC defect pixel detection control parameter; Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */

    ISP_OP_TYPE_E enOpType;
    ISP_DRC_MANUAL_ATTR_S stManual;
    ISP_DRC_AUTO_ATTR_S   stAuto;
    ISP_DRC_CUBIC_POINT_ATTR_S astCubicPoint[HI_ISP_DRC_CUBIC_POINT_NUM]; /* Only used for Hi3559AV100 */
    ISP_DRC_ASYMMETRY_CURVE_ATTR_S stAsymmetryCurve;

} ISP_DRC_ATTR_S;

typedef struct hiISP_LDCI_GAUSS_COEF_ATTR_S {
    HI_U8   u8Wgt;      /* RW;Range:Hi3559AV100 = [0x0, 0x80] | Hi3519AV100 = [0x0, 0x80]| Hi3516CV500 = [0x0, 0xFF] | Hi3516DV300 = [0x0, 0xFF]|
                                                           Hi3516AV300 = [0x0, 0xFF]|Hi3559V200 = [0x0, 0xFF] | Hi3556V200 = [0x0, 0xFF] |
                                                           Hi3516EV200 = [0x0, 0xFF] | Hi3516EV300 = [0x0, 0xFF] | Hi3518EV300 = [0x0, 0xFF]| Hi3516DV200 = [0x0, 0xFF];
                                                           Format:1.7;Weight of Gaussian distribution */
    HI_U8   u8Sigma;    /* RW;Range: [0x1, 0xFF];Format:0.8;Sigma of Gaussian distribution */
    HI_U8   u8Mean;     /* RW;Range: [0x0, 0xFF];Format:0.8;Mean of Gaussian distribution */
} ISP_LDCI_GAUSS_COEF_ATTR_S;

typedef struct hiISP_LDCI_HE_WGT_ATTR_S {
    ISP_LDCI_GAUSS_COEF_ATTR_S  stHePosWgt;
    ISP_LDCI_GAUSS_COEF_ATTR_S  stHeNegWgt;
} ISP_LDCI_HE_WGT_ATTR_S;

typedef struct hiISP_LDCI_MANUAL_ATTR_S {
    ISP_LDCI_HE_WGT_ATTR_S  stHeWgt;
    HI_U16                  u16BlcCtrl; /* RW;Range: [0x0, 0x1FF];Format:9.0;Restrain dark region */
} ISP_LDCI_MANUAL_ATTR_S;

typedef struct hiISP_LDCI_AUTO_ATTR_S {
    ISP_LDCI_HE_WGT_ATTR_S  astHeWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16                  au16BlcCtrl[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range: [0x0, 0x1FF];Format:9.0;auto mode, Restrain dark region */
} ISP_LDCI_AUTO_ATTR_S;

typedef struct hiISP_LDCI_ATTR_S {
    HI_BOOL                 bEnable;                /* RW;Range:[0x0,0x1];Format:1.0; */
    HI_U8                   u8GaussLPFSigma;        /* RW;Range: [0x1, 0xFF];Format:0.8;Coefficient of Gaussian low-pass filter */
    ISP_OP_TYPE_E           enOpType;
    ISP_LDCI_MANUAL_ATTR_S  stManual;
    ISP_LDCI_AUTO_ATTR_S    stAuto;
    HI_U16                  u16TprIncrCoef;         /* RW;Range: [0x0, 0x100];Format:0.9;Increase Coefficient of temporal filter */
    HI_U16                  u16TprDecrCoef;         /* RW;Range: [0x0, 0x100];Format:0.9;Decrease Coefficient of temporal filter */
} ISP_LDCI_ATTR_S;

/*
defines CA type
0 = enable Ca module
1 = enable Cp module
 */
typedef enum hiISP_CA_TYPE_E {
    ISP_CA_ENABLE = 0x0,
    ISP_CP_ENABLE,
    ISP_CA_BUTT
} ISP_CA_TYPE_E;

typedef struct hiISP_CA_LUT_S {
    HI_U32  au32YRatioLut[HI_ISP_CA_YRATIO_LUT_LENGTH];  /* RW;Range:[0,2047];Format:1.11;Not support for Hi3559V200/Hi3556V200 */
    HI_S32  as32ISORatio[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW;Range:[0,2047];Format:1.10;Not support for Hi3559V200/Hi3556V200 */
} ISP_CA_LUT_S;

typedef struct hiISP_CP_LUT_S {
    HI_U8   au8CPLutY[HI_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0;Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   au8CPLutU[HI_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0;Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   au8CPLutV[HI_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0;Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_CP_LUT_S;

/* Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_CA_ATTR_S {
    HI_BOOL bEnable;            /* RW;Range:[0x0,0x1];Format:1.0;Not support for Hi3559V200/Hi3556V200 */
    ISP_CA_TYPE_E eCaCpEn;      /* Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_CA_LUT_S stCA;          /* Not support for Hi3559V200/Hi3556V200 */
    ISP_CP_LUT_S stCP;          /* Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_CA_ATTR_S;

// CSC

typedef struct hiCSC_MATRX_S {
    HI_S16 as16CSCIdc[3];  /* RW; Range:[-1024, 1023]; Format: 11.0; Input dc component for csc matrix */
    HI_S16 as16CSCOdc[3];  /* RW; Range:[-1024, 1023]; Format: 11.0; Output dc component for csc matrix */
    HI_S16 as16CSCCoef[9]; /* RW; Range:[-4096, 4095]; Format: 5.10; 3x3 coefficients for csc matrix */
} CSC_MATRX_S;

typedef struct hiISP_CSC_ATTR_S {
    HI_BOOL bEnable;            /* RW; Range:[0, 1];Format:1.0; Enable/Disable CSC Function */

    COLOR_GAMUT_E enColorGamut; /* RW; Range: [0, 4]; Color gamut type; COLOR_GAMUT_BT2020 only used for Hi3559AV100 */
    HI_U8   u8Hue;              /* RW; Range:[0, 100];Format:8.0; Csc Hue Value */
    HI_U8   u8Luma;             /* RW; Range:[0, 100];Format:8.0; Csc Luma Value */
    HI_U8   u8Contr;            /* RW; Range:[0, 100];Format:8.0; Csc Contrast Value */
    HI_U8   u8Satu;             /* RW; Range:[0, 100];Format:8.0; Csc Saturation Value */
    HI_BOOL bLimitedRangeEn;    /* RW; Range: [0x0, 0x1]; Enable/Disable: Enable Limited range output mode(default full range output) */
    HI_BOOL bExtCscEn;          /* RW; Range: [0x0, 0x1]; Enable/Disable: Enable extended luma range */
    HI_BOOL bCtModeEn;          /* RW; Range: [0x0, 0x1]; Enable/Disable: Enable ct mode */
    CSC_MATRX_S stCscMagtrx;    /* RW; Color Space Conversion matrix */

} ISP_CSC_ATTR_S;

/********************** CLUT **************************************/

typedef struct hiISP_CLUT_ATTR_S {
    HI_BOOL         bEnable         ; /* RW; Range:[0, 1];Format:1.0; Enable/Disable CLUT Function,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U32          u32GainR        ; /* RW; Range:[0,4095];Format:12.0,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U32          u32GainG        ; /* RW; Range:[0,4095];Format:12.0,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U32          u32GainB        ; /* RW; Range:[0,4095];Format:12.0,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_CLUT_ATTR_S;

/* CLUT LUT */

typedef struct hiISP_CLUT_LUT_S {
    HI_U32 au32lut[HI_ISP_CLUT_LUT_LENGTH];     /* RW; Range: Hi3559AV100 = [0, 4294967295] | Hi3519AV100 = [0x0, 1073741823]| Hi3516CV500 = [0x0, 1073741823]|
                                                   Hi3516DV300 = [0x0, 1073741823] | Hi3516AV300 = [0x0, 1073741823] | Hi3559V200 = [0x0, 1073741823]| Hi3556V200 = [0x0, 1073741823];
                                                   Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_CLUT_LUT_S;

/*****************************************************************/


/*
Defines the type of static defect pixel calibration
0 = bright defect pixel calibration
1 = dark defect pixel calibration
 */
typedef enum hiISP_STATIC_DP_TYPE_E {
    ISP_STATIC_DP_BRIGHT = 0x0,
    ISP_STATIC_DP_DARK,
    ISP_STATIC_DP_BUTT
} ISP_STATIC_DP_TYPE_E;


typedef struct hiISP_DP_STATIC_CALIBRATE_S {
    HI_BOOL bEnableDetect;                  /* RW; Range: [0, 1];Format 1.0;Set 'HI_TRUE'to start static defect-pixel calibration, and firmware will set 'HI_FALSE' when finished,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_STATIC_DP_TYPE_E enStaticDPType;    /* RW; Range: [0, 1];Format 1.0;Select static bright/dark defect-pixel calibration,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8StartThresh;                  /* RW; Range: [1, 255]; Format 8.0;Start threshold for static defect-pixel calibraiton,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  u16CountMax;                    /* RW; Range:Hi3559AV100 = [0, 8192] | Hi3519AV100 = [0, 8192]| Hi3516CV500 = [0, 6144] | Hi3516DV300 = [0, 6144] |
                                               Hi3516AV300 = [0, 6144] |Hi3559V200 = [0, 6144] | Hi3556V200 = [0, 6144];Format 14.0;
                                               limited Range: [0, STATIC_DP_COUNT_NORMAL*BlkNum],Limit of max number of static defect-pixel calibraiton.
                                               Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  u16CountMin;                    /* RW; Range:Hi3559AV100 = [0, 8192] | Hi3519AV100 = [0, 8192]| Hi3516CV500 = [0, 6144] | Hi3516DV300 = [0, 6144]|
                                               Hi3516AV300 = [0, 6144]| Hi3559V200 = [0, 6144] | Hi3556V200 = [0, 6144];Format 14.0;
                                               limited Range: [0, u16CountMax],Limit of min number of static defect-pixel calibraiton.
                                               Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  u16TimeLimit;                   /* RW; Range: [0x0, 1600];Format 11.0;Time limit for static defect-pixel calibraiton, in frame number,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */

    HI_U32      au32Table[STATIC_DP_COUNT_MAX];  /* R;  [0,0x1FFF1FFF];Format 29.0;Static defect-pixel calibraiton table,0~12 bits represents the X coordinate of the defect pixel, 16~28 bits represent the Y coordinate of the defect pixel.
                                                  Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8       u8FinishThresh;   /* R; Range: [0, 255];Format 8.0; Finish threshold for static defect-pixel calibraiton,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16      u16Count;         /* R; Range:Hi3559AV100 = [0, 8192] | Hi3519AV100 = [0, 8192] Hi3516CV500 = [0, 6144]| Hi3516DV300 = [0, 6144]|
                                     Hi3516AV300 = [0, 6144]| Hi3559V200 = [0, 6144] | Hi3556V200 = [0, 6144];Format 14.0; Finish number for static defect-pixel calibraiton
                                     Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_STATUS_E enStatus;        /* R; Range: [0, 2];Format 2.0;Status of static defect-pixel calibraiton,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_DP_STATIC_CALIBRATE_S;

typedef struct hiISP_DP_STATIC_ATTR_S {
    HI_BOOL bEnable;                /* RW; Range: [0, 1];Format 1.0;Enable/disable the static defect-pixel module,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  u16BrightCount;         /* RW; Range:Hi3559AV100 = [0, 8192] | Hi3519AV100 = [0, 8192]| Hi3516CV500 = [0, 6144]| Hi3516DV300 = [0, 6144]|
                                       Hi3516AV300 = [0, 6144]| Hi3559V200 = [0, 6144] | Hi3556V200 = [0, 6144];Format 14.0;limited Range: [0, STATIC_DP_COUNT_NORMAL*BlkNum],
                                       When used as input(W), indicate the number of static bright defect pixels;As output(R),indicate the number of static bright and dark defect pixels.
                                       Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  u16DarkCount;           /* RW; Range:Hi3559AV100 = [0, 8192] | Hi3519AV100 = [0, 8192]| Hi3516CV500 = [0, 6144]| Hi3516DV300 = [0, 6144]|
                                       Hi3516AV300 = [0, 6144]|Hi3559V200 = [0, 6144] | Hi3556V200 = [0, 6144];Format 14.0;limited Range: [0, STATIC_DP_COUNT_NORMAL*BlkNum],
                                       When used as input(W), indicate the number of static dark defect pixels; As output(R), invalid value 0.
                                       Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U32  au32BrightTable[STATIC_DP_COUNT_MAX];   /* RW; Range: [0x0, 0x1FFF1FFF];Format 29.0;0~12 bits represents the X coordinate of the defect pixel, 16~28 bits represent the Y coordinate of the defect pixel
                                                       Notice : When used as input(W), indicate static bright defect pixels table;  As output(R), indicate static bright and dark defect pixels table.
                                                       Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */

    HI_U32  au32DarkTable[STATIC_DP_COUNT_MAX];     /* RW; Range: [0x0, 0x1FFF1FFF];Format 29.0;0~12 bits represents the X coordinate of the defect pixel, 16~28 bits represent the Y coordinate of the defect pixel
                                                       Notice : When used as input(W), indicate static dark defect pixels table;  As output(R), invalid value.
                                                       Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_BOOL bShow;                  /* RW; Range: [0, 1];Format 1.0;RW;highlight static defect pixel,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_DP_STATIC_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_MANUAL_ATTR_S {
    HI_U16  u16Strength;          /* RW; Range: [0, 255];Format:8.0;Dynamic DPC strength. */
    HI_U16  u16BlendRatio;        /* RW; Range: [0, 128];Format:9.0;Blending ratio required for DPC */
} ISP_DP_DYNAMIC_MANUAL_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_AUTO_ATTR_S {
    HI_U16  au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 255];Format:8.0;Dynamic DPC strength. */
    HI_U16  au16BlendRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 128];Format:9.0;Blending ratio required for DPC */
} ISP_DP_DYNAMIC_AUTO_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_ATTR_S {
    HI_BOOL bEnable;            /* RW; Range: [0, 1];Format 1.0;Enable/disable the dynamic defect-pixel module */
    HI_BOOL bSupTwinkleEn;      /* RW; Range: [0, 1];Format 1.0;Enable/disable the twinkle suppression  module */
    HI_S8   s8SoftThr;          /* RW; Range: [0, 127];Format s8.0;twinkle suppression threshold */
    HI_U8   u8SoftSlope;        /* RW; Range: [0, 255];Format 8.0;Correction controlling parameter of the pixels whose deviation is smaller than s8SoftThr. */
    ISP_OP_TYPE_E enOpType;     /* RW; Range: [0, 1];Format 1.0;Working mode of dynamic DPC */
    ISP_DP_DYNAMIC_MANUAL_ATTR_S stManual;
    ISP_DP_DYNAMIC_AUTO_ATTR_S   stAuto;
} ISP_DP_DYNAMIC_ATTR_S;

typedef struct hiISP_DIS_ATTR_S {
    HI_BOOL bEnable; /* RW; Range: [0, 1];Format 1.0;Enable/disable dis module,Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
} ISP_DIS_ATTR_S;


typedef struct hiISP_SHADING_ATTR_S {
    HI_BOOL bEnable;            /* RW; Range:[0, 1];Format:1.0; HI_TRUE: enable lsc; HI_FALSE: disable lsc */
    HI_U16  u16MeshStr;         /* RW; Range:Hi3559AV100=[0, 65535]| Hi3519AV100=[0, 65535]|Hi3516CV500=[0, 65535]|Hi3516DV300=[0, 65535]|Hi3516AV300=[0, 65535]|
                                  Hi3559V200=[0, 65535]|Hi3556V200=[0, 65535] | Hi3516EV200 = [0x0, 1023] | Hi3516EV300 = [0x0,1023] | Hi3518EV300 = [0x0,1023] | Hi3516DV200 = [0x0,1023];
                                  The strength of the mesh shading correction */
    HI_U16  u16BlendRatio;      /* RW; Range:[0, 256];Format:9.0; the blendratio of the two mesh gain lookup-table */
} ISP_SHADING_ATTR_S;

typedef struct hiISP_SHADING_LUT_S {
    HI_U16  au16RGain[HI_ISP_LSC_GRID_POINTS];  /* RW; Range:[0,1023];Member used to store the calibration data of the R channel required for LSC. */
    HI_U16  au16GrGain[HI_ISP_LSC_GRID_POINTS]; /* RW; Range:[0,1023];Member used to store the calibration data of the Gr channel required for LSC. */
    HI_U16  au16GbGain[HI_ISP_LSC_GRID_POINTS]; /* RW; Range:[0,1023];Member used to store the calibration data of the Gb channel required for LSC. */
    HI_U16  au16BGain[HI_ISP_LSC_GRID_POINTS];  /* RW; Range:[0,1023];Member used to store the calibration data of the B channel required for LSC. */
} ISP_SHADING_GAIN_LUT_S;

typedef struct hiISP_BNR_LSC_GAIN_LUT_S {
    HI_U16  au16RGain[HI_ISP_RLSC_POINTS];  /* RW; Range:[0,65535];Member used to store the calibration data of the R channel required for BNR_LSC.Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  au16GrGain[HI_ISP_RLSC_POINTS]; /* RW; Range:[0,65535];Member used to store the calibration data of the Gr channel required for BNR_LSC.Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  au16GbGain[HI_ISP_RLSC_POINTS]; /* RW; Range:[0,65535];Member used to store the calibration data of the Gb channel required for BNR_LSC.Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  au16BGain[HI_ISP_RLSC_POINTS];  /* RW; Range:[0,65535];Member used to store the calibration data of the B channel required for BNR_LSC.Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_BNR_LSC_GAIN_LUT_S;

typedef struct hiISP_SHADING_LUT_ATTR_S {
    HI_U8   u8MeshScale;        /* RW; Range:[0, 7];Format:3.0;Three bit value that selects the scale and precision for the 10 bit gain coefficients stored in mesh tables */
    HI_U16  au16XGridWidth[(HI_ISP_LSC_GRID_COL - 1) / 2]; /* RW; Range:Hi3559AV100=[4, 1988]| Hi3519AV100=[4, 1988]|Hi3516CV500=[4, 1092]|Hi3516DV300=[4, 1092]|
                                                             Hi3516AV300=[4, 1092]|Hi3559V200=[4, 1092]|Hi3556V200=[4, 1092] | Hi3516EV200 = [4,255]|Hi3516EV300 = [4,255]|Hi3518EV300 = [4,255]|Hi3516DV200 = [4,255];
                                                             limited Range:[4, u32Width/4 - 60], Member used to store the width data of each GRID partition */
    HI_U16  au16YGridWidth[(HI_ISP_LSC_GRID_ROW - 1) / 2]; /* RW; Range:Hi3559AV100=[4, 1988]| Hi3519AV100=[4, 1988]|Hi3516CV500=[4, 1092]|Hi3516DV300=[4, 1092]|
                                                             Hi3516AV300=[4, 1092]|Hi3559V200=[4, 1092]|Hi3556V200=[4, 1092]|Hi3516EV200 = [4,255]|Hi3516EV300 =[4,255]| Hi3518EV300 = [0,255 ]| Hi3516DV200 = [0,255 ];
                                                             limited Range:[4, u32Height/4 - 60]; Member used to store the height data of each GRID partition */
    ISP_SHADING_GAIN_LUT_S astLscGainLut[2];
    ISP_BNR_LSC_GAIN_LUT_S stBNRLscGainLut; /* Not support for Hi3559AV100/Hi3519AV100 */
} ISP_SHADING_LUT_ATTR_S;

typedef struct hiISP_MLSC_CALIBRATION_CFG_S {
    ISP_BAYER_FORMAT_E enBayer;   /* RW; Range: [0, 3];Format ENUM;Shows bayer pattern */
    ISP_BAYER_RAWBIT_E enRawBit;  /* RW; Range: {8,10,12,14,16};Format ENUM;Shows input raw bitwidth */

    HI_U16  u16ImgHeight; /* RW; Range: [0, 65535];Format 16.0;Input raw image height */
    HI_U16  u16ImgWidth;  /* RW; Range: [0, 65535];Format 16.0;Input raw image width */

    HI_U16  u16DstImgHeight; /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, u16ImgHeight],Image height that crop from input raw image, set to ImgHeight if don't need to crop */
    HI_U16  u16DstImgWidth;  /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, u16ImgWidth],Image width that crop from input raw image, set to ImgWidth if don't need to crop */
    HI_U16  u16OffsetX;      /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, u16ImgWidth - u16DstImgWidth],Horizontal offset that crop from input raw image, set to 0 if don't need to crop */
    HI_U16  u16OffsetY;      /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, u16ImgHeight - u16DstImgHeight],Vertical offset that crop from input raw image, set to 0 if don't need to crop */

    HI_U32  u32MeshScale; /* RW; Range: [0, 7];Format 3.0; Shows Mesh Scale value */

    HI_U16  u16BLCOffsetR; /* RW; Range: [0, 4095];Format 12.0;BLC value for R channel */
    HI_U16  u16BLCOffsetGr; /* RW; Range: [0, 4095];Format 12.0;BLC value for Gr channel */
    HI_U16  u16BLCOffsetGb; /* RW; Range: [0, 4095];Format 12.0;BLC value for Gb channel */
    HI_U16  u16BLCOffsetB; /* RW; Range: [0, 4095];Format 12.0;BLC value for B channel */
} ISP_MLSC_CALIBRATION_CFG_S;

typedef struct hiISP_MESH_SHADING_TABLE_S {
    HI_U8   u8MeshScale;       /* RW; Range:[0, 7];Format:3.0;Three bit value that selects the scale and precision for the 10 bit gain coefficients stored in mesh tables */
    HI_U16  au16XGridWidth[(HI_ISP_LSC_GRID_COL - 1) / 2]; /* RW; Range:Hi3559AV100=[4, 16323]| Hi3519AV100=[4, 16323]|Hi3516CV500=[4, 16323]|Hi3516DV300=[4, 16323]|Hi3516AV300=[4, 16323] |
                                                            Hi3559V200=[4, 16323]|Hi3556V200=[4, 16323]|Hi3516EV200 = [4,255]|Hi3516EV300 =[4,255]| Hi3518EV300 = [0,255 ]| Hi3516DV200 = [0,255 ];
                                                            limited Range:[4, u16ImgWidth /4 - 60],Member used to store the width data of each GRID partition */
    HI_U16  au16YGridWidth[(HI_ISP_LSC_GRID_ROW - 1) / 2]; /* RW; Hi3559AV100=[4, 16323]| Hi3519AV100=[4, 16323]|Hi3516CV500=[4, 16323]|Hi3516DV300=[4, 16323]|Hi3516AV300=[4, 16323]|
                                                            Hi3559V200=[4, 16323] |Hi3556V200=[4, 16323]|Hi3516EV200 = [4,255]|Hi3516EV300 =[4,255]| Hi3518EV300 = [0,255 ]|Hi3516DV200 = [0,255 ];
                                                            limited Range:[4, u16ImgHeight /4 - 60],Member used to store the height data of each GRID partition */
    ISP_SHADING_GAIN_LUT_S stLscGainLut;
    ISP_BNR_LSC_GAIN_LUT_S stBNRLscGainLut; /* Not support for Hi3559AV100/Hi3519AV100/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_MESH_SHADING_TABLE_S;

/* Only used for Hi3559AV100/Hi3519AV100 */
typedef struct hiISP_RADIAL_SHADING_ATTR_S {
    HI_BOOL bEnable;          /* RW; Range:[0, 1];Format:1.0; HI_TRUE: enable rlsc; HI_FALSE: disable rlsc,Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16RadialStr;     /* RW; Range:[0, 65535];Format:4.12; the strength of the mesh shading correction,Only used for Hi3559AV100/Hi3519AV100 */
} ISP_RADIAL_SHADING_ATTR_S;

/* Only used for Hi3559AV100/Hi3519AV100 */
typedef struct hiISP_RADIAL_SHADING_LUT_S {
    HI_U16  au16RGain[HI_ISP_RLSC_POINTS];  /* RW; Range:[0,65535];Member used to store the calibration data of the R channel required for RLSC.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  au16GrGain[HI_ISP_RLSC_POINTS]; /* RW; Range:[0,65535];Member used to store the calibration data of the Gr channel required for RLSC.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  au16GbGain[HI_ISP_RLSC_POINTS]; /* RW; Range:[0,65535];Member used to store the calibration data of the Gb channel required for RLSC.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  au16BGain[HI_ISP_RLSC_POINTS];  /* RW; Range:[0,65535];Member used to store the calibration data of the B channel required for RLSC.Only used for Hi3559AV100/Hi3519AV100 */
} ISP_RADIAL_SHADING_GAIN_LUT_S;

/* Only used for Hi3559AV100/Hi3519AV100 */
typedef struct hiISP_RADIAL_SHADING_LUT_ATTR_S {
    OPERATION_MODE_E enLightMode;   /* RW; Range:[0, 1];Format:1.0; HI_TRUE: manual mode; HI_FALSE: auto mode.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16BlendRatio;          /* RW; Range:[0, 256];Format:8.0; Used in manual mode only, indicates the light blending strength for the first light info.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U8   u8LightType1;           /* RW; Range:[0, 2];Format:2.0; Used in manual mode only, indicates the first light source selected.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U8   u8LightType2;           /* RW; Range:[0, 2];Format:2.0; Used in manual mode only, indicates the second light source selected.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U8   u8RadialScale;          /* RW; Range:[0, 13];Format:4.0;Four bit value that selects the scale and precision for the 10 bit gain coefficients.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterRX;     /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Width], shows x value of the position of optical center of red channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterRY;     /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Height], shows y value of the position of optical center of red channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterGrX;    /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Width], shows x value of the position of optical center of gr channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterGrY;    /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Height], shows y value of the position of optical center of gr channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterGbX;    /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Width], shows x value of the position of optical center of gb channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterGbY;    /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Height], shows y value of the position of optical center of gb channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterBX;     /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Width], shows x value of the position of optical center of blue channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16CenterBY;     /* RW; Range:[0, 65535];Format:16.0;Limited Range:[0, Height], shows y value of the position of optical center of blue channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16OffCenterR;   /* RW; Range:[0, 65535];Format:16.0;related to the 1/R^2 value of red channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16OffCenterGr;  /* RW; Range:[0, 65535];Format:16.0;related to the 1/R^2 value of gr channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16OffCenterGb;  /* RW; Range:[0, 65535];Format:16.0;related to the 1/R^2 value of gb channel.Only used for Hi3559AV100/Hi3519AV100 */
    HI_U16  u16OffCenterB;   /* RW; Range:[0, 65535];Format:16.0;related to the 1/R^2 value of blue channel.Only used for Hi3559AV100/Hi3519AV100 */
    ISP_RADIAL_SHADING_GAIN_LUT_S astRLscGainLut[3]; /* Only used for Hi3559AV100/Hi3519AV100 */
} ISP_RADIAL_SHADING_LUT_ATTR_S;

typedef struct hiISP_NR_MANUAL_ATTR_S {
    HI_U8   au8ChromaStr[ISP_BAYER_CHN_NUM];   /* RW;Range:[0x0,0x3];Format:2.0;Strength of Chrmoa noise reduction for R/Gr/Gb/B channel,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8FineStr;                         /* RW;Range:[0x0,0x80];Format:8.0;Strength of Luma noise reduction */
    HI_U16  u16CoringWgt;                      /* RW;Range:[0x0,0xc80];Format:12.0;Strength of reserving the random noise */
    HI_U16  au16CoarseStr[ISP_BAYER_CHN_NUM];  /* RW;Range:[0x0,0x360];Format:10.0; Coarse Strength of noise reduction */
} ISP_NR_MANUAL_ATTR_S;

typedef struct hiISP_NR_AUTO_ATTR_S {
    HI_U8   au8ChromaStr[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW;Range:[0x0,0x3];Format:2.0; Strength of chrmoa noise reduction for R/Gr/Gb/B channel,Not support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   au8FineStr[ISP_AUTO_ISO_STRENGTH_NUM];                       /* RW;Range:[0x0,0x80];Format:8.0; Strength of luma noise reduction */
    HI_U16  au16CoringWgt[ISP_AUTO_ISO_STRENGTH_NUM];                    /* RW;Range:[0x0,0xc80];Format:12.0; Strength of reserving the random noise */
    HI_U16  au16CoarseStr[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range:[0x0,0x360];Format:10.0; Coarse Strength of noise reduction */
} ISP_NR_AUTO_ATTR_S;

typedef struct hiISP_NR_WDR_ATTR_S {
    HI_U8    au8WDRFrameStr[WDR_MAX_FRAME_NUM];       /* RW;Range:[0x0,0x50];Format:7.0; Strength of each frame in wdr mode */
    HI_U8    au8FusionFrameStr[WDR_MAX_FRAME_NUM];    /* RW;Range:[0x0,0x50];Format:7.0; Strength of each frame in wdr mode. Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_NR_WDR_ATTR_S;

typedef struct hiISP_NR_ATTR_S {
    HI_BOOL  bEnable;                                     /* RW;Range:[0x0,0x1];Format:1.0; Nr Enable */
    HI_BOOL  bLowPowerEnable;                             /* RW;Range:[0x0,0x1];Format:1.0; Nr Low Power Enable. Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_BOOL  bNrLscEnable;                                /* RW;Range:[0x0,0x1];Format:1.0; HI_TRUE: Noise reduction refers to lens shading; HI_FALSE: Noise reduction not refers to lens shading; */
    HI_U8    u8NrLscRatio;                                /* RW;Range:[0x0,0xff];Format:8.0; Ratio of referring to lens shading. Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8    u8BnrLscMaxGain;                             /* RW;Range:[0x0,0xbf];Format:2.6; Max gain for referring to lens shading.Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16   u16BnrLscCmpStrength;                        /* RW;Range:[0x0,0x100];Format:1.8; Compare strength for referring to lens shading.Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16   au16CoringRatio[HI_ISP_BAYERNR_LUT_LENGTH];  /* RW;Range:[0x0,0x3ff];Format:12.0; Strength of reserving the random noise according to luma */

    ISP_OP_TYPE_E enOpType;
    ISP_NR_AUTO_ATTR_S stAuto;
    ISP_NR_MANUAL_ATTR_S stManual;
    ISP_NR_WDR_ATTR_S  stWdr;
} ISP_NR_ATTR_S;

/* Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef struct hiISP_DE_MANUAL_ATTR_S {
    HI_U16       u16GlobalGain;         /* RW;Range:[0x0,0x100];Format:1.8; Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    HI_U16       u16GainLF;             /* RW;Range:[0x0,0x20];Format:2.4; Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    HI_U16       u16GainHF;             /* RW;Range:[0x0,0x20];Format:2.4;Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
} ISP_DE_MANUAL_ATTR_S;

/* Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
typedef struct hiISP_DE_AUTO_ATTR_S {
    HI_U16       au16GlobalGain[ISP_AUTO_ISO_STRENGTH_NUM];         /* RW;Range:[0x0,0x100];Format:1.8; Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    HI_U16       au16GainLF[ISP_AUTO_ISO_STRENGTH_NUM];             /* RW;Range:[0x0,0x20];Format:2.4; Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    HI_U16       au16GainHF[ISP_AUTO_ISO_STRENGTH_NUM];             /* RW;Range:[0x0,0x20];Format:2.4; Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
} ISP_DE_AUTO_ATTR_S;


/* Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
typedef struct hiISP_DE_ATTR_S {
    HI_BOOL       bEnable;                                    /* RW;Range:[0x0,0x1];Format:1.0; De Enable,Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    HI_U16        au16LumaGainLut[HI_ISP_DE_LUMA_GAIN_LUT_N]; /* RW;Range:[0x0,0x100];Format:1.8; Only used for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */

    ISP_OP_TYPE_E        enOpType;
    ISP_DE_AUTO_ATTR_S   stAuto;
    ISP_DE_MANUAL_ATTR_S stManual;
} ISP_DE_ATTR_S;

#define ISP_CVTMAT_NUM                            12
#define ISP_EXP_CTRL_NUM                          2

/* Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef enum hiISP_IRBAYER_FORMAT_E {
    IRBAYER_GRGBI = 0,
    IRBAYER_RGBGI = 1,
    IRBAYER_GBGRI = 2,
    IRBAYER_BGRGI = 3,
    IRBAYER_IGRGB = 4,
    IRBAYER_IRGBG = 5,
    IRBAYER_IBGRG = 6,
    IRBAYER_IGBGR = 7,
    IRBAYER_BUTT

} ISP_IRBAYER_FORMAT_E;

/* Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef enum hiISP_IR_CVTMAT_MODE_E {
    ISP_IR_CVTMAT_MODE_NORMAL = 0,
    ISP_IR_CVTMAT_MODE_MONO,
    ISP_IR_CVTMAT_MODE_USER,
    ISP_IR_CVTMAT_MODE_BUTT

} ISP_IR_CVTMAT_MODE_E;

/* Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef struct hiISP_RGBIR_ATTR_S {
    HI_BOOL              bEnable    ;      /* RW;Range:[0x0,0x1];Format:1.0; Enable/Disable RGBIR module,Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_IRBAYER_FORMAT_E enInPattern;      /* RW;Range:[0x0,0x7];Format:3.0; IR pattern of the input signal,Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_BAYER_FORMAT_E   enOutPattern;     /* RW;Range:[0x0,0x3];Format:2.0; Bayer pattern of the output signal,Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */

    HI_U16               au16ExpCtrl[ISP_EXP_CTRL_NUM];    /* RW;Range:[0, 2047];Format:11.0; Over expose control parameter,Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16               au16Gain[ISP_EXP_CTRL_NUM];       /* RW;Range:[0, 511];Format:9.0; Over expose control parameter,Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */

    ISP_IR_CVTMAT_MODE_E enIRCvtMatMode;
    HI_S16               as16CvtMatrix[ISP_CVTMAT_NUM]; /* RW;Range:[-32768, 32767];Format:s15.0; RGBIR to Bayer image Convert matrix coefficients(need calibration),Only support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_RGBIR_ATTR_S;

/*
Defines the type of the ISP gamma curve
0 = Default curve
1 = Default BT.709 curve (Gamma Curve)
2 = Default SMPTE.2084 curve (PQ Curve), Only used for Hi3559AV100
3 = User defined Gamma curve, LUT must be correct
*/
typedef enum hiISP_GAMMA_CURVE_TYPE_E {
    ISP_GAMMA_CURVE_DEFAULT = 0x0,
    ISP_GAMMA_CURVE_SRGB,
    ISP_GAMMA_CURVE_HDR,    /* Only used for Hi3559AV100 */
    ISP_GAMMA_CURVE_USER_DEFINE,
    ISP_GAMMA_CURVE_BUTT
} ISP_GAMMA_CURVE_TYPE_E;
typedef struct hiISP_GAMMA_ATTR_S {
    HI_BOOL   bEnable;                     /* RW; Range:[0, 1]; Format:1.0;Enable/Disable Gamma Function */
    HI_U16    u16Table[GAMMA_NODE_NUM];    /* RW; Range:[0, 4095]; Format:12.0;Gamma LUT nodes value */

    ISP_GAMMA_CURVE_TYPE_E enCurveType;    /* RW; Range:[0, 3]; Format:2.0;Gamma curve type */
} ISP_GAMMA_ATTR_S;

/* Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_PREGAMMA_ATTR_S {
    HI_BOOL   bEnable;                         /* RW; Range:[0, 1]; Format:1.0;Enable/Disable PreGamma Function.Not support for Hi3559V200/Hi3556V200 */
    HI_U32    au32Table[PREGAMMA_NODE_NUM];    /* RW; Range:Hi3559AV100 = [0, 0x100000] | Hi3519AV100 = [0, 0x100000] | Hi3516CV500 = [0, 0xFFFFF] |Hi3516DV300 = [0, 0xFFFFF] |
                                                 Hi3516AV300 = [0, 0xFFFFF] |Hi3516EV200 = [0x0, 0xFFFFF]|Hi3516EV300 = [0x0, 0xFFFFF]|Hi3518EV300 = [0x0,0xFFFFF]|Hi3516DV200 = [0x0,0xFFFFF];
                                                 Format:21.0;PreGamma LUT nodes value.Not support for Hi3559V200/Hi3556V200 */
} ISP_PREGAMMA_ATTR_S;

typedef struct hiISP_PRELOGLUT_ATTR_S {
    HI_BOOL                 bEnable;    /* RW; Range:[0, 1]; Format:1.0;Enable/Disable PreLogLUT Function.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
} ISP_PRELOGLUT_ATTR_S;

typedef struct hiISP_LOGLUT_ATTR_S {
    HI_BOOL                 bEnable;    /* RW; Range:[0, 1]; Format:1.0;Enable/Disable LogLUT Function.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
} ISP_LOGLUT_ATTR_S;

#define     ISP_SHARPEN_FREQ_CORING_LENGTH      (8)
#define     SHRP_GAIN_LUT_SIZE                  (64)

typedef struct hiISP_SHARPEN_MANUAL_ATTR_S {
    HI_U8  au8LumaWgt[ISP_SHARPEN_LUMA_NUM];     /* RW; Range: Hi3559AV100 =[0,127]|Hi3519AV100=[0,127]|Hi3516CV500 = [0, 127]|Hi3516DV300 =[0, 127]|
                                                    Hi3516AV300 =[0, 127]|Hi3559V200 = [0, 127]| Hi3556V200 =[0, 127]| Hi3516EV200 = [0x0, 31]|Hi3516EV300 = [0x0, 31]|Hi3518EV300 = [0x0, 31]|Hi3516DV200 = [0x0, 31];
                                                    Format:0.7;Adjust the sharpen strength according to luma. Sharpen strength will be weaker when it decrease. */
    HI_U16 au16TextureStr[ISP_SHARPEN_GAIN_NUM]; /* RW; Range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement */
    HI_U16 au16EdgeStr[ISP_SHARPEN_GAIN_NUM];    /* RW; Range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement */
    HI_U16 u16TextureFreq;         /* RW; Range: [0, 4095];Format:6.6; Texture frequency adjustment. Texture and detail will be finer when it increase */
    HI_U16 u16EdgeFreq;            /* RW; Range: [0, 4095];Format:6.6; Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    HI_U8  u8OverShoot;            /* RW; Range: [0, 127]; Format:7.0;u8OvershootAmt */
    HI_U8  u8UnderShoot;           /* RW; Range: [0, 127]; Format:7.0;u8UndershootAmt */
    HI_U8  u8ShootSupStr;          /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase */
    HI_U8  u8ShootSupAdj;          /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot suppression adjusting, adjust the edge shoot suppression strength */
    HI_U8  u8DetailCtrl;           /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge. */
    HI_U8  u8DetailCtrlThr;        /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl, it is used to distinguish detail and edge. */
    HI_U8  u8EdgeFiltStr;          /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering. */
    HI_U8  u8EdgeFiltMaxCap;       /* RW; Range: [0, 47]; Format:6.0;The max capacity of edge filtering.*/
    HI_U8  u8RGain;                /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Red Area */
    HI_U8  u8GGain;                /* RW; Range: [0, 255]; Format:8.0; Sharpen Gain for Green Area */
    HI_U8  u8BGain;                /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Blue Area */
    HI_U8  u8SkinGain;             /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area */
    HI_U16  u16MaxSharpGain;       /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */
    HI_U8  u8WeakDetailGain;       /* RW; Range: [0, 127];  Only support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200; sharpen Gain for weakdetail */
} ISP_SHARPEN_MANUAL_ATTR_S;


typedef struct hiISP_SHARPEN_AUTO_ATTR_S {
    HI_U8  au8LumaWgt[ISP_SHARPEN_LUMA_NUM][ISP_AUTO_ISO_STRENGTH_NUM];      /* RW; Range:Hi3559AV100 =[0,127]|Hi3519AV100=[0,127]|Hi3516CV500 = [0, 127]|
                                                                                Hi3516DV300 =[0, 127]|  Hi3516AV300 =[0, 127]|Hi3559V200 = [0, 127] |Hi3556V200 = [0, 127]| Hi3556V200 =[0, 127]| Hi3516EV200 = [0x0, 31]|Hi3516EV300 = [0x0, 31]|Hi3518EV300 = [0x0, 31]|Hi3516DV200 = [0x0, 31]
                                                                                Format:0.7; Adjust the sharpen strength according to luma. Sharpen strength will be weaker when it decrease. */
    HI_U16 au16TextureStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement */
    HI_U16 au16EdgeStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement */
    HI_U16 au16TextureFreq[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range: [0, 4095]; Format:6.6;Texture frequency adjustment. Texture and detail will be finer when it increase */
    HI_U16 au16EdgeFreq[ISP_AUTO_ISO_STRENGTH_NUM];      /* RW; Range: [0, 4095]; Format:6.6;Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    HI_U8  au8OverShoot[ISP_AUTO_ISO_STRENGTH_NUM];      /* RW; Range: [0, 127];  Format:7.0;u8OvershootAmt */
    HI_U8  au8UnderShoot[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range: [0, 127];  Format:7.0;u8UndershootAmt */
    HI_U8  au8ShootSupStr[ISP_AUTO_ISO_STRENGTH_NUM];    /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase */
    HI_U8  au8ShootSupAdj[ISP_AUTO_ISO_STRENGTH_NUM];    /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot suppression adjusting, adjust the edge shoot suppression strength */
    HI_U8  au8DetailCtrl[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge. */
    HI_U8  au8DetailCtrlThr[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl, it is used to distinguish detail and edge. */
    HI_U8  au8EdgeFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];    /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering. */
    HI_U8  au8EdgeFiltMaxCap[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 47]; Format:6.0;The max capacity of edge filtering.*/
    HI_U8  au8RGain[ISP_AUTO_ISO_STRENGTH_NUM];                 /* RW; Range: [0, 31];   Format:5.0; Sharpen Gain for Red Area */
    HI_U8  au8GGain[ISP_AUTO_ISO_STRENGTH_NUM];                 /* RW; Range: [0, 255]; Format:8.0; Sharpen Gain for Green Area */
    HI_U8  au8BGain[ISP_AUTO_ISO_STRENGTH_NUM];                 /* RW; Range: [0, 31];   Format:5.0; Sharpen Gain for Blue Area */
    HI_U8  au8SkinGain[ISP_AUTO_ISO_STRENGTH_NUM];       /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area */
    HI_U16  au16MaxSharpGain[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */
    HI_U8  au8WeakDetailGain[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 127]; Format:7.0; Only support for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200; sharpen Gain for weakdetail */
} ISP_SHARPEN_AUTO_ATTR_S;

typedef struct hiISP_SHARPEN_ATTR_S {
    HI_BOOL bEnable;    /* RW; Range:[0, 1]; Format:1.0;Enable/Disable sharpen module */
    HI_U8 u8SkinUmin;   /* RW; Range: [0, 255];  Format:8.0; U min value of the range of skin area */
    HI_U8 u8SkinVmin;   /* RW; Range: [0, 255];  Format:8.0; V min value of the range of skin area */
    HI_U8 u8SkinUmax;   /* RW; Range: [0, 255];  Format:8.0; U max value of the range of skin area */
    HI_U8 u8SkinVmax;   /* RW; Range: [0, 255];  Format:8.0; V max value of the range of skin area */
    ISP_OP_TYPE_E enOpType;
    ISP_SHARPEN_MANUAL_ATTR_S stManual;
    ISP_SHARPEN_AUTO_ATTR_S   stAuto;
} ISP_SHARPEN_ATTR_S;

/* Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef struct hiISP_EDGEMARK_ATTR_S {
    HI_BOOL bEnable;              /* RW; Range:[0, 1]; Format:1.0;Enable/Disable Edge Mark. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8Threshold;          /* RW; Range: [0, 255];  Format:8.0;Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U32  u32Color;             /* RW; Range: [0, 0xFFFFFF];  Format:32.0;Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_EDGEMARK_ATTR_S;

// High Light Constraint
/* Not support for Hi3559AV100/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef struct hiISP_HLC_ATTR_S {
    HI_BOOL bEnable;              /* RW; Range:[0, 1];  Format:1.0;Enable/Disable HLC module,Only used for Hi3519AV100/Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8LumaThr;            /* RW; Range:[0, 255];Format:8.0;Only used for Hi3519AV100/Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8LumaTarget;         /* RW; Range:[0, 255];Format:8.0;Only used for Hi3519AV100/Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_HLC_ATTR_S;

/* Crosstalk Removal */
typedef struct hiISP_CR_ATTR_S {
    HI_BOOL  bEnable;       /* RW; Range: [0, 1];Format 1.0;Enable/disable the crosstalk removal module */
    HI_U8    u8Slope;       /* RW; Range: Hi3559AV100 = [0, 14] | Hi3519AV100 = [0, 14]|Hi3516CV500 = [0, 14]|Hi3516DV300 =[0, 14] |Hi3516AV300 =[0, 14] |
                                                   Hi3559V200 =[0, 14] |Hi3556V200 =[0, 14] |Hi3516EV200 = [0,16]|Hi3516EV300 = [0,16]|Hi3518EV300 = [0,16]|Hi3516DV200 = [0,16];
                                                   Crosstalk slope value. */
    HI_U8    u8SensiSlope;  /* RW; Range: Hi3559AV100 = [0, 14] | Hi3519AV100 = [0, 14]|Hi3516CV500 = [0, 14]|Hi3516DV300 =[0, 14]||Hi3516AV300 =[0, 14] |
                                                   Hi3559V200 =[0, 14] |Hi3556V200 =[0, 14] |Hi3516EV200 = [0,16]|Hi3516EV300 = [0,16]|Hi3518EV300 = [0,16]|Hi3516DV200 = [0,16];
                                                   Crosstalk sensitivity. */
    HI_U16   u16SensiThr;   /* RW; Range: Hi3559AV100 = [0, 16383] | Hi3519AV100 = [0, 16383]|Hi3516CV500 = [0, 16383]|Hi3516DV300 =[0, 16383]|Hi3516AV300 =[0, 16383]|
                                                   Hi3559V200 =[0, 16383]|Hi3556V200 =[0, 16383]|Hi3516EV200 = [0,65535]|Hi3516EV300 = [0,65535]|Hi3518EV300 = [0,65535]|Hi3516DV200 = [0,65535];
                                                   Crosstalk sensitivity threshold. */
    HI_U16   au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range: [0,256];Crosstalk strength value. */
    HI_U16   au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: Hi3559AV100 = [0, 16383] | Hi3519AV100 = [0, 16383]|Hi3516CV500 = [0, 16383]|Hi3516DV300 =[0, 16383] |
                                                                                                  Hi3516AV300 =[0, 16383]|Hi3559V200 =[0, 16383] |Hi3556V200 =[0, 16383] |Hi3516EV200 = [0,65535]|
                                                                                                  Hi3516EV300 = [0,65535]|Hi3518EV300 = [0,65535]|Hi3516DV200 = [0,65535]; Crosstalk threshold. */
    HI_U16   au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range: Hi3559AV100 = [512, 16383] | Hi3519AV100 = [512, 16383]|Hi3516CV500 = [512, 16383]|
                                                                                                  Hi3516DV300 =[512, 16383] |Hi3516AV300 =[512, 16383] |Hi3559V200 =[512, 16383] |Hi3556V200 =[512, 16383] |
                                                                                                  Hi3516EV200 = [8192,65535]|Hi3516EV300 = [8192,65535]|Hi3518EV300 = [8192,65535]|Hi3516DV200 = [8192,65535];  Set Noise profile value. */
} ISP_CR_ATTR_S;

typedef struct hiISP_ANTIFALSECOLOR_MANUAL_ATTR_S {
    HI_U8    u8AntiFalseColorThreshold;     /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    HI_U8    u8AntiFalseColorStrength;      /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ISP_ANTIFALSECOLOR_MANUAL_ATTR_S;

typedef struct hiISP_ANTIFALSECOLOR_AUTO_ATTR_S {
    HI_U8   au8AntiFalseColorThreshold[ISP_AUTO_ISO_STRENGTH_NUM];    /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    HI_U8   au8AntiFalseColorStrength[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ISP_ANTIFALSECOLOR_AUTO_ATTR_S;

typedef struct hiISP_ANTIFALSECOLOR_ATTR_S {
    HI_BOOL  bEnable;                               /* RW;Range:[0x0,0x1];Format:1.0; AntiFalseColor Enable */
    ISP_OP_TYPE_E enOpType;
    ISP_ANTIFALSECOLOR_AUTO_ATTR_S stAuto;
    ISP_ANTIFALSECOLOR_MANUAL_ATTR_S stManual;
} ISP_ANTIFALSECOLOR_ATTR_S;

typedef struct hiISP_DEMOSAIC_MANUAL_ATTR_S {
    HI_U8   u8NonDirStr;            /* RW; Range:[0x0, 0xFF]; Format:4.4; Non-direction strength */
    HI_U8   u8NonDirMFDetailEhcStr; /* RW; Range:Hi3559AV100 = [0x0, 0x10] |Hi3519AV100 = [0x0, 0x7f] |Hi3516CV500= [0x0, 0x7f]|Hi3516DV300= [0x0, 0x7f]|
                                                               Hi3516AV300= [0x0, 0x7f]|Hi3559V200= [0x0, 0x7f]|Hi3556V200= [0x0, 0x7f]|
                                                               Hi3516EV200 = [0x0, 0x7f]|Hi3516EV300 = [0x0, 0x7f]|Hi3518EV300 = [0x0, 0x7f]|Hi3516DV200 = [0x0, 0x7f];
                                                               Format:3.4; Non-direction medium frequent detail enhance  strength */
    HI_U8   u8NonDirHFDetailEhcStr; /* RW; Range:[0x0,0x10];  Format:2.2; Non-direction high frequent detail enhance strength */
    HI_U8   u8DetailSmoothRange;    /* RW; Range:Hi3559AV100 = [0x1, 0x8] |Hi3519AV100 = [0x1, 0x7] |Hi3516CV500 = [0x1, 0x7]|Hi3516DV300 = [0x1, 0x7]|
                                                                Hi3516AV300 = [0x1, 0x7]|Hi3559V200 = [0x1, 0x7]|Hi3556V200 = [0x1, 0x7] |
                                                                Hi3516EV200 = [0x1, 0x7]|Hi3516EV300 = [0x1, 0x7]|Hi3518EV300 = [0x1, 0x7]|Hi3516DV200 = [0x1, 0x7];
                                                                Format:4.0; Detail smooth range */
    HI_U16  u16DetailSmoothStr;     /* RW;Range:[0x0,0x100]; Format:9.0;   Strength of detail smooth, Only used for Hi3559AV100 */
	HI_U8   u8ColorNoiseThdF;       /* RW;Range:[0x0,0xFF]; Format:8.0;   Frequency Threshold used for ColorNoise */
    HI_U8   u8ColorNoiseStrF;       /* RW;Range:[0x0,0x8]; Format:4.0;   Frequency Strength used for ColorNoise */
    HI_U8   u8ColorNoiseThdY;       /* RW;Range:[0x0,0xF];Format:4.0; Range of color denoise luma*/
    HI_U8   u8ColorNoiseStrY;       /* RW;Range:[0x0,0x3F];Format:6.0; Strength of color denoise luma*/
} ISP_DEMOSAIC_MANUAL_ATTR_S;

typedef struct hiISP_DEMOSAIC_AUTO_ATTR_S {
    HI_U8   au8NonDirStr[ISP_AUTO_ISO_STRENGTH_NUM];            /* RW; Range:[0x0, 0xFF]; Format:4.4; Non-direction strength */
    HI_U8   au8NonDirMFDetailEhcStr[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:Hi3559AV100 = [0x0, 0x10] |Hi3519AV100 = [0x0, 0x7f] |Hi3516CV500= [0x0, 0x7f]|
                                                                  Hi3516DV300= [0x0, 0x7f]|Hi3516AV300= [0x0, 0x7f]|Hi3559V200 = [0x0, 0x7f] |Hi3556V200= [0x0, 0x7f] |
                                                                  Hi3516EV200= [0x0, 0x7f]|Hi3516EV300 = [0x0, 0x7f] |Hi3518EV300= [0x0, 0x7f]|Hi3516DV200 = [0x0, 0x7f];
                                                                  Format:3.4; Non-direction medium frequent detail enhance  strength */
    HI_U8   au8NonDirHFDetailEhcStr[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0,0x10];  Format:2.2; Non-direction high frequent detail enhance  strength */
    HI_U8   au8DetailSmoothRange[ISP_AUTO_ISO_STRENGTH_NUM];    /* RW; Range:Hi3559AV100 = [0x1, 0x8] |Hi3519AV100 = [0x1, 0x7] |Hi3516CV500 = [0x1, 0x7]|
                                                                  Hi3516DV300 = [0x1, 0x7]|Hi3516AV300 = [0x1, 0x7]|Hi3559V200 = [0x1, 0x7] |Hi3556V200 = [0x1, 0x7]|
                                                                  Hi3516EV200= [0x1, 0x7]|Hi3516EV300 = [0x, 0x7] |Hi3518EV300= [0x1, 0x7]|Hi3516DV200= [0x1, 0x7] ;
                                                                  Format:4.0; Detail smooth range */
    HI_U16  au16DetailSmoothStr[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW;Range:[0x0,0x100]; Format:9.0;   Strength of detail smooth, Only used for Hi3559AV100 */
	HI_U8   au8ColorNoiseThdF[ISP_AUTO_ISO_STRENGTH_NUM];       /* RW;Range:[0x0,0xFF]; Format:8.0;   Frequency Threshold used for ColorNoise */
    HI_U8   au8ColorNoiseStrF[ISP_AUTO_ISO_STRENGTH_NUM];       /* RW;Range:[0x0,0x8]; Format:4.0;   Frequency Strength used for ColorNoise */
    HI_U8   au8ColorNoiseThdY[ISP_AUTO_ISO_STRENGTH_NUM];       /* RW;Range:[0x0,0xF];Format:4.0; Range of color denoise luma*/
    HI_U8   au8ColorNoiseStrY[ISP_AUTO_ISO_STRENGTH_NUM];       /* RW;Range:[0x0,0x3F];Format:6.0; Strength of color denoise luma*/
} ISP_DEMOSAIC_AUTO_ATTR_S;

typedef struct hiISP_DEMOSAIC_ATTR_S {
    HI_BOOL bEnable;        /* RW; Range:[0, 1]; Format:1.0;Enable/Disable demosaic module */
    ISP_OP_TYPE_E enOpType;
    ISP_DEMOSAIC_MANUAL_ATTR_S stManual;
    ISP_DEMOSAIC_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_ATTR_S;

/* Defines the attributes of the ISP black level */
typedef struct hiISP_BLACK_LEVEL_S {
    ISP_OP_TYPE_E enOpType;
    HI_U16 au16BlackLevel[ISP_BAYER_CHN_NUM]; /* RW; Range: [0x0, 0xFFF];Format:12.0;Black level values that correspond to the black levels of the R,Gr, Gb, and B components respectively. */
} ISP_BLACK_LEVEL_S;

/*
Defines the FPN removal type
0 = Frame mode correction
1 = Line mode correction
*/
typedef enum hiISP_FPN_TYPE_E {
    ISP_FPN_TYPE_FRAME = 0,
    ISP_FPN_TYPE_LINE = 1,
    ISP_FPN_TYPE_BUTT
} ISP_FPN_TYPE_E;

/* Defines the information about calibrated black frames. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_FPN_FRAME_INFO_S {
    HI_U32              u32Iso;             /* RW;Range:[0x64,0xFFFFFFFF];Format:32.0;FPN CALIBRATE ISO */
    HI_U32              u32Offset[VI_MAX_SPLIT_NODE_NUM];          /* RW;Range:[0,0xFFF];Format:12.0;FPN frame u32Offset (agv pixel value) */
    HI_U32              u32FrmSize;         /* RW;FPN frame size (exactly frm size or compress len) */
    VIDEO_FRAME_INFO_S  stFpnFrame;         /* FPN frame info, 8bpp,10bpp,12bpp,16bpp. Compression or not */
} ISP_FPN_FRAME_INFO_S;

/* Defines the calibration attribute of the FPN removal module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_FPN_CALIBRATE_ATTR_S {
    HI_U32                          u32Threshold;        /* RW;Range:[1,0xFFF];Format:12.0;pix value > threshold means defective pixel */
    HI_U32                          u32FrameNum;         /* RW;Range:[1, 16];Format:5.0;Number of frames to be calibrated. The value range is {1, 2, 4, 8, 16},that is, the integer exponentiation of 2. */
    ISP_FPN_TYPE_E                  enFpnType;           /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S            stFpnCaliFrame;
} ISP_FPN_CALIBRATE_ATTR_S;
/* Defines the manual correction attribute of the FPN removal module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_FPN_MANUAL_ATTR_S {
    HI_U32               u32Strength;         /* RW;Range:[0,1023];Format:10.0;Manual correction strength */
} ISP_FPN_MANUAL_ATTR_S;

/* Defines the automatic correction attribute of the FPN removal module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_FPN_AUTO_ATTR_S {
    HI_U32               u32Strength;          /* RW;Range:[0,1023];Format:10.0;Auto correction strength */
} ISP_FPN_AUTO_ATTR_S;

/* Defines the correction attribute of the FPN removal module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_FPN_ATTR_S {
    HI_BOOL               bEnable;            /* RW;Range:[0,1];Format:1.0; */
    ISP_OP_TYPE_E         enOpType;           /* manual or auto mode */
    ISP_FPN_TYPE_E        enFpnType;          /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S  stFpnFrmInfo;       /* input in correction mode. */
    ISP_FPN_MANUAL_ATTR_S stManual;
    ISP_FPN_AUTO_ATTR_S   stAuto;
} ISP_FPN_ATTR_S;

/* Defines the manual dehze attribute */
typedef struct hiISP_DEHAZE_MANUAL_ATTR_S {
    HI_U8              u8strength;  /* RW;Range:[0,0xFF];Format:8.0;Manual dehze strength */
} ISP_DEHAZE_MANUAL_ATTR_S;

/* Defines the automatic dehze attribute */
typedef struct hiISP_DEHAZE_AUTO_ATTR_S {
    HI_U8              u8strength; /* RW;Range:[0,0xFF];Format:8.0;Weighted coefficient for automatic dehaze strength. */
} ISP_DEHAZE_AUTO_ATTR_S;

/* Defines the ISP dehaze attribute */
typedef struct hiISP_DEHAZE_ATTR_S {
    HI_BOOL  bEnable;           /* RW;Range:[0,1];Format:1.0; */
    HI_BOOL  bUserLutEnable;    /* RW;Range:[0,1];0:Auto Lut 1:User Lut */
    HI_U8    au8DehazeLut[256];
    ISP_OP_TYPE_E      enOpType;
    ISP_DEHAZE_MANUAL_ATTR_S stManual;
    ISP_DEHAZE_AUTO_ATTR_S   stAuto;
    HI_U16       u16TmprfltIncrCoef; /* RW, Range: [0x0, 0x80].filter increase coeffcient. */
    HI_U16       u16TmprfltDecrCoef; /* RW, Range: [0x0, 0x80].filter decrease coeffcient. */

} ISP_DEHAZE_ATTR_S;

/* Defines purple fringing correction manual attribute */
typedef struct hiISP_DEPURPLESTR_MANUAL_ATTR_S {
    HI_U8 u8DePurpleCrStr;               /* RW;Range: [0,8];Format:4.0;Correction strength of the R channel */
    HI_U8 u8DePurpleCbStr;               /* RW;Range: [0,8];Format:4.0;Correction strength of the B channel */
} ISP_DEPURPLESTR_MANUAL_ATTR_S;

/* Defines purple fringing correction automatic attribute */
typedef struct hiISP_DEPURPLESTR_AUTO_ATTR_S {
    HI_U8 au8DePurpleCrStr[ISP_EXP_RATIO_STRENGTH_NUM];    /* RW;Range: [0,8];Format:4.0;Correction strength of the R channel */
    HI_U8 au8DePurpleCbStr[ISP_EXP_RATIO_STRENGTH_NUM];    /* RW;Range: [0,8];Format:4.0;Correction strength of the B channel */
} ISP_DEPURPLESTR_AUTO_ATTR_S;

/* Purple fringing detection and correction attribute */
typedef struct hiISP_LOCAL_CAC_ATTR_S {
    HI_BOOL bEnable;                        /* RW;Range: [0,1];Format:1.0; HI_TRUE: enable local cac; HI_FALSE: disable local cac */
    HI_U16  u16PurpleDetRange;              /* RW;Range: [0,410];Format:9.0;Purple fringing detection scope */
    HI_U16  u16VarThr;                      /* RW;Range: [0,4095];Format:12.0;Edge detection threshold */
    ISP_OP_TYPE_E      enOpType;            /* RW;Range: [0,1];Format:1.0;Purple fringing correction working mode */
    ISP_DEPURPLESTR_MANUAL_ATTR_S stManual;
    ISP_DEPURPLESTR_AUTO_ATTR_S   stAuto;
} ISP_LOCAL_CAC_ATTR_S;

/* Defines the lateral chromatic aberration correction attribute, Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef struct hiISP_GLOBAL_CAC_ATTR_S {
    HI_BOOL bEnable;            /* RW; Range: [0, 1];Format: 1.0;  enable/disable global cac */
    HI_U16  u16VerCoordinate;   /* RW; Range: [0, 8191];Format: 13.0; limited Range: [0, ImageHeight - 1],Vertical coordinate of the optical center.  Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  u16HorCoordinate;   /* RW; Range: [0, 8191];Format: 13.0; limited range : [0, ImageWidth - 1],Horizontal coordinate of the optical center. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_S16  s16ParamRedA;       /* RW; Range: [-256, 255];Format: 9.0; Coefficient a of the radius polynomial corresponding to channel R. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_S16  s16ParamRedB;       /* RW; Range: [-256, 255];Format: 9.0; Coefficient b of the radius polynomial corresponding to channel R. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_S16  s16ParamRedC;       /* RW; Range: [-256, 255];Format: 9.0; Coefficient c of the radius polynomial corresponding to channel R. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_S16  s16ParamBlueA;      /* RW; Range: [-256, 255];Format: 9.0; Coefficient a of the radius polynomial corresponding to channel B. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_S16  s16ParamBlueB;      /* RW; Range: [-256, 255];Format: 9.0; Coefficient b of the radius polynomial corresponding to channel B. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_S16  s16ParamBlueC;      /* RW; Range: [-256, 255];Format: 9.0; Coefficient c of the radius polynomial corresponding to channel B. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8VerNormShift;     /* RW; Range: [0, 7];Format: 3.0; Normalized shift parameter in the vertical direction. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8VerNormFactor;    /* RW; Range: [0, 31];Format: 5.0;Normalized coefficient in the vertical direction. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8HorNormShift;     /* RW; Range: [0, 7];Format: 3.0; Normalized shift parameter in the horizontal direction. Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8   u8HorNormFactor;    /* RW; Range: [0, 31];Format: 5.0;Normalized coefficient in the horizontal direction.Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16  u16CorVarThr;       /* RW; Range: [0, 4095];Format: 12.0;Variance threshold of lateral chromatic aberration correction.Not support for Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_GLOBAL_CAC_ATTR_S;

/* Defines the Radial Crop attribute */
typedef struct hiISP_RC_ATTR_S {
    HI_BOOL bEnable;       /* RW;Range:[0,1];Format:1.0; HI_TRUE: enable ridial crop; HI_FALSE: disable ridial crop.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    POINT_S stCenterCoor;  /* RW;the coordinate of central pixel.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
    HI_U32  u32Radius;     /* RW;Range:[0,11586];Format:14.0; when the distance to central pixel is greater than u32Radius,the pixel value becomes 0.
                                                 Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
} ISP_RC_ATTR_S;

typedef struct hiISP_INNER_STATE_INFO_S {

    HI_U16 au16TextureStr[ISP_SHARPEN_GAIN_NUM];     /* RW; range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement */
    HI_U16 au16EdgeStr[ISP_SHARPEN_GAIN_NUM];        /* RW; range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement */
    HI_U16 u16TextureFreq;         /* RW; range: [0, 4095];Format:6.6; Texture frequency adjustment. Texture and detail will be finer when it increase */
    HI_U16 u16EdgeFreq;            /* RW; range: [0, 4095];Format:6.6; Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    HI_U8  u8OverShoot;            /* RW; range: [0, 127]; Format:7.0;u8OvershootAmt */
    HI_U8  u8UnderShoot;           /* RW; range: [0, 127]; Format:7.0;u8UndershootAmt */
    HI_U8  u8ShootSupStr;          /* RW; range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase */

    HI_U8   u8NrLscRatio;                       /* RW;Range:[0x0,0xff];Format:8.0; Strength of reserving the random noise according to luma,Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 not support */
    HI_U16  au16CoarseStr[ISP_BAYER_CHN_NUM];   /* RW;Range:[0x0,0x3ff];Format:10.0; Coarse Strength of noise reduction */
    HI_U8   au8WDRFrameStr[WDR_MAX_FRAME_NUM];  /* RW;Range:[0x0,0x50];Format:7.0; Coarse strength of each frame in \nwdr mode */
    HI_U8   au8ChromaStr[ISP_BAYER_CHN_NUM];    /* RW;Range:[0x0,0x3];Format:2.0;Strength of Chrmoa noise reduction for R/Gr/Gb/B channel,Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200
                                                 Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 not support */
    HI_U8   u8FineStr;              /* RW;Range:[0x0,0x80];Format:8.0;Strength of Luma noise reduction */
    HI_U16  u16CoringWgt;           /* RW;Range:[0x0,0xC80];Format:12.0;Strength of reserving the random noise */

    HI_U16 u16DeHazeStrengthActual; /* RW;Range:[0,0xFF];Format:8.0;actual dehze strength */
    HI_U16 u16DrcStrengthActual;    /* RW;Range: Hi3559AV100 = [0x0, 0xFF] | Hi3519AV100 = [0x0, 0x3FF] | Hi3516CV500 = [0x0, 0x3FF]| Hi3516DV300 = [0x0, 0x3FF]|
                                                                Hi3516AV300 = [0x0, 0x3FF]|Hi3559V200 = [0x0, 0x3FF]| Hi3556V200 = [0x0, 0x3FF]|
                                                                Hi3516EV200 = [0x0, 0x3FF]|Hi3516EV300 = [0x0, 0x3FF]|Hi3518EV300 = [0x0, 0x3FF]|Hi3516DV200 = [0x0, 0x3FF] ;Strength of dynamic range compression.
                                                                Higher values lead to higher differential gain between shadows and highlights. */
    HI_U32 u32WDRExpRatioActual[3]; /* RW; Range:[0x40, 0x4000]; Format:26.6; 0x40 means 1 times.
                                                                 When enExpRatioType is OP_TYPE_AUTO, u32ExpRatio is invalid.
                                                                 When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatio is quotient of long exposure time / short exposure time. */
    HI_BOOL bWDRSwitchFinish;       /* RW; Range:[0, 1];Format:1.0;HI_TRUE: WDR switch is finished */
    HI_BOOL bResSwitchFinish;       /* RW; Range:[0, 1];Format:1.0;HI_TRUE: Resolution switch is finished */
    HI_U16 au16BLActual[ISP_BAYER_CHN_NUM]; /* RW; Range: [0x0, 0xFFF];Format:12.0;Actual Black level values that correspond to the black levels of the R,Gr, Gb, and B components respectively. */
} ISP_INNER_STATE_INFO_S;

/*
The position of AE histogram in ISP BE pipeline
0 = After DG;
1 = After static WB;
2 = After DRC;
*/
typedef enum hiISP_AE_SWITCH_E {
    ISP_AE_AFTER_DG = 0,
    ISP_AE_AFTER_WB,
    ISP_AE_AFTER_DRC,
    ISP_AE_SWITCH_BUTT
} ISP_AE_SWITCH_E;

/*
FourPlaneMode enable
0 : Disabled;
1 : Enabled
*/
typedef enum hiISP_AE_FOUR_PLANE_MODE_E {
    ISP_AE_FOUR_PLANE_MODE_DISABLE = 0,
    ISP_AE_FOUR_PLANE_MODE_ENABLE,
    ISP_AE_FOUR_PLANE_MODE_BUTT
} ISP_AE_FOUR_PLANE_MODE_E;

typedef enum hiISP_AE_HIST_SKIP_E {
    ISP_AE_HIST_SKIP_EVERY_PIXEL = 0,
    ISP_AE_HIST_SKIP_EVERY_2ND_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_3RD_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_4TH_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_5TH_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_8TH_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_9TH_PIXEL,
    ISP_AE_HIST_SKIP_BUTT
} ISP_AE_HIST_SKIP_E;

typedef enum hiISP_AE_HIST_OFFSET_X_E {
    ISP_AE_HIST_START_FORM_FIRST_COLUMN = 0,
    ISP_AE_HIST_START_FORM_SECOND_COLUMN,
    ISP_AE_HIST_OFFSET_X_BUTT
} ISP_AE_HIST_OFFSET_X_E;

typedef enum hiISP_AE_HIST_OFFSET_Y_E {
    ISP_AE_HIST_START_FORM_FIRST_ROW = 0,
    ISP_AE_HIST_START_FORM_SECOND_ROW,
    ISP_AE_HIST_OFFSET_Y_BUTT
} ISP_AE_HIST_OFFSET_Y_E;

/* Defines the mode configuration for the sampling points during global histogram statistics. */
typedef struct hiISP_AE_HIST_CONFIG_S {
    ISP_AE_HIST_SKIP_E enHistSkipX;    /* RW; Range:[0,6]; Format:4.0;
                                                                    Histogram decimation in horizontal direction: 0=every pixel; 1=every 2nd pixel; 2=every
                                                                    3rd pixel; 3=every 4th pixel; 4=every 5th pixel; 5=every 8th pixel ; 6+=every 9th pixel */
    ISP_AE_HIST_SKIP_E enHistSkipY;    /* RW; Range:[0,6]; Format:4.0;
                                                                    Histogram decimation in vertical direction: 0=every pixel; 1=every 2nd pixel; 2=every
                                                                    3rd pixel; 3=every 4th pixel; 4=every 5th pixel; 5=every 8th pixel ; 6+=every 9th pixel */
    ISP_AE_HIST_OFFSET_X_E enHistOffsetX;  /* RW; Range:[0,1]; Format:1.0; 0= start from the first column; 1=start from second column */
    ISP_AE_HIST_OFFSET_Y_E enHistOffsetY;  /* RW; Range:[0,1]; Format:1.0; 0= start from the first row; 1= start from second row */
} ISP_AE_HIST_CONFIG_S;

typedef enum hiISP_AE_STAT_MODE__E {
    ISP_AE_NORMAL = 0,
    ISP_AE_ROOT,
    ISP_AE_MODE_BUTT
} ISP_AE_STAT_MODE_E;

/* Crops the input image of the AE module */
typedef struct hiISP_AE_CROP_S {
    HI_BOOL bEnable; /* RW; Range: [0,1];  Format:1.0;AE crop enable. */
    HI_U16  u16X;    /* RW; Range: Hi3559AV100 = [0, 8192 - 256]|Hi3519AV100 = [0, 8192 - 256]|Hi3516CV500 = [0, 4608 - 256]|Hi3516DV300 = [0, 4608 - 256]|
                                       Hi3516AV300 = [0, 4608 - 256]|Hi3559V200   = [0, 4608 - 256]|Hi3556V200  = [0, 4608 - 256]|
                                       Hi3516EV200 = [0,3072 - 256]|Hi3516EV300 = [0,3072 - 256]|Hi3518EV300 = [0,3072 - 256]|Hi3516DV200 = [0,3072 - 256];
                                       Format:13.0;AE image crop start x, limited range:[0, ImageWidth - 256] */
    HI_U16  u16Y;   /* RW; Range: Hi3559AV100 = [0, 8192 - 120]|Hi3519AV100 = [0, 8192 - 120]|Hi3516CV500 = [0, 4608 - 120]|Hi3516DV300 = [0, 4608 - 120]|
                                      Hi3516AV300 = [0, 4608 - 120]| Hi3559V200   = [0, 4608 - 120]|Hi3556V200   = [0, 4608 - 120]|
                                      Hi3516EV200 = [0,3072 - 120]|Hi3516EV300 = [0,3072 - 120]|Hi3518EV300 = [0,3072 - 120]| Hi3516DV200 = [0,3072 - 120];
                                      Format:13.0;AEimage crop start y, limited range:[0, ImageHeight - 120]  */
    HI_U16  u16W;   /* RW; Range: Hi3559AV100 = [256, 8192]|Hi3519AV100 = [256, 8192]|Hi3516CV500 = [256, 4608]|Hi3516DV300 = [256, 4608] |
                                     Hi3516AV300 = [256, 4608] |Hi3559V200   = [256, 4608]| Hi3556V200  = [256, 4608]|
                                     Hi3516EV200 = [256, 3072]|Hi3516EV300 = [256, 3072]|Hi3518EV300 = [256, 3072]|Hi3516DV200 = [256, 3072];
                                     Format:14.0;AE image crop width,  limited range:[256, ImageWidth] */
    HI_U16  u16H;  /* RW; Range: Hi3559AV100 = [120, 8192]|Hi3519AV100 = [120, 8192]|Hi3516CV500 = [120, 4608]|Hi3516DV300 = [120, 4608] |
                                    Hi3516AV300 = [120, 4608] |Hi3559V200   = [120, 4608]| Hi3556V200  = [120, 4608]|
                                    Hi3516EV200 = [120, 3072]|Hi3516EV300 = [120, 3072]|Hi3518EV300 = [120, 3072]|Hi3516DV200 = [120, 3072];
                                    Format:14.0;AE image crop height  limited range:[120, ImageHeight] */
} ISP_AE_CROP_S;

/* config of statistics structs */
#define HIST_THRESH_NUM                           (4)
typedef struct hiISP_AE_STATISTICS_CFG_S {
    ISP_AE_SWITCH_E          enAESwitch;          /* RW; Range:[0,2]; Format:2.0;The position of AE histogram in ISP BE pipeline */
    ISP_AE_HIST_CONFIG_S     stHistConfig;
    ISP_AE_FOUR_PLANE_MODE_E enFourPlaneMode;     /* RW; Range:[0,1]; Format:2.0;Four Plane Mode Enable */
    ISP_AE_STAT_MODE_E       enHistMode;          /* RW; Range:[0,1]; Format:2.0;AE Hist Rooting Mode */
    ISP_AE_STAT_MODE_E       enAverMode;          /* RW; Range:[0,1]; Format:2.0;AE Aver Rooting Mode */
    ISP_AE_STAT_MODE_E       enMaxGainMode;       /* RW; Range:[0,1]; Format:2.0;Max Gain Rooting Mode */
    ISP_AE_CROP_S            stCrop;
    HI_U8 au8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; /* RW; Range:[0x0, 0xF]; Format:4.0; AE weighting table */
} ISP_AE_STATISTICS_CFG_S;

// Smart Info
#define SMART_CLASS_MAX                           (2)

/* Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef struct hiISP_SMART_ROI_S {
    HI_BOOL  bEnable;
    HI_BOOL  bAvailable;
    HI_U8    u8Luma ;
} ISP_SMART_ROI_S;

/* Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
typedef struct hiISP_SMART_INFO_S {
    ISP_SMART_ROI_S  stROI[SMART_CLASS_MAX];
} ISP_SMART_INFO_S;

/* fines whether the peak value of the zoned IIR statistics is calculated. Not support for Hi3559V200/Hi3556V200 */
typedef enum hiISP_AF_PEAK_MODE_E {
    ISP_AF_STA_NORM         = 0,    /* use every value of the block statistic */
    ISP_AF_STA_PEAK,                /* use the maximum value in one line of the block statistic */
    ISP_AF_STA_BUTT
} ISP_AF_PEAK_MODE_E;

/* Defines whether the zoned statistics are squared in square mode. Not support for Hi3559V200/Hi3556V200 */
typedef enum hiISP_AF_SQU_MODE_E {
    ISP_AF_STA_SUM_NORM     = 0,    /* statistic value accumlate */
    ISP_AF_STA_SUM_SQU,             /* statistic value square then accumlate */
    ISP_AF_STA_SUM_BUTT
} ISP_AF_SQU_MODE_E;
/* Crops the input image of the AF module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_CROP_S {
    HI_BOOL bEnable; /* RW; Range: [0,1];  Format:1.0; AF crop enable */
    HI_U16  u16X;    /* RW; Range: Hi3559AV100 = [0, 7936]|Hi3519AV100 = [0, 7936]|Hi3516CV500 = [0, 4352]|Hi3516DV300 = [0, 4352]|
                                       Hi3516AV300 = [0, 4352]|Hi3516EV200 = [0, 2816]|Hi3516EV300 = [0, 2816]|Hi3518EV300 = [0, 2816]|Hi3516DV200 = [0, 2816];
                                       Format:13.0;AF image crop start x, limited range:[0, ImageWidth-256] */
    HI_U16  u16Y;    /* RW; Range: Hi3559AV100 = [0, 8072]|Hi3519AV100 = [0, 8072]|Hi3516CV500 = [0, 4488]|Hi3516DV300 = [0, 4488]|
                                       Hi3516AV300 = [0, 4488]|Hi3516EV200 = [0, 2952]|Hi3516EV300 = [0,2952]|Hi3518EV300 = [0,2952]|Hi3516DV200 = [0,2952];
                                       Format:13.0;AF image crop start y, limited range:[0, ImageHeight-120] */
    HI_U16  u16W;    /* RW; Range: Hi3559AV100 = [256, 8192]|Hi3519AV100 = [256, 8192]|Hi3516CV500 = [256, 4608]|Hi3516DV300 = [256, 4608]|
                                       Hi3516AV300 = [256, 4608]|Hi3516EV200 = [256, 3072]|Hi3516EV300 = [256, 3072]|Hi3518EV300 = [256, 3072]|Hi3516DV200 = [256, 3072];
                                       Format:14.0;AF image crop width, limited range:[256, ImageWidth] */
    HI_U16  u16H;    /* RW; Range: Hi3559AV100 = [120, 8192]|Hi3519AV100 = [120, 8192]|Hi3516CV500 = [120, 4608]|Hi3516DV300 = [120, 4608]|
                                       Hi3516AV300 = [120, 4608]|Hi3516EV200 = [120,3072]|Hi3516EV300 = [120,3072]|Hi3518EV300 = [120, 3072]|Hi3516DV200 = [120, 3072];
                                       Format:14.0;AF image crop height, limited range:[120, ImageHeight] */
} ISP_AF_CROP_S;

/* Defines the position of AF module statistics. Not support for Hi3559V200/Hi3556V200 */
typedef enum hiISP_AF_STATISTICS_POS_E {
    ISP_AF_STATISTICS_AFTER_DGAIN    = 0, // The AF module is placed in the raw field for statistics,AF after DGain
    ISP_AF_STATISTICS_AFTER_DRC,         // The AF module is placed in the raw field for statistics,AF after DRC
    ISP_AF_STATISTICS_AFTER_CSC,         // The AF module is placed in the YUV field for statistics,AF after CSC
    ISP_AF_STATISTICS_BUTT
} ISP_AF_STATISTICS_POS_E;

/* Configures the Bayer field of the AF module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_RAW_CFG_S {
    HI_U8               GammaGainLimit; /* RW; Range: [0x0, 0x5]; Format:3.0,Not support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U8               GammaValue;     /* RW; Range: Hi3559AV100 = [0x0, 0x6] | Hi3519AV100 = [0x0, 0x6] | Hi3516CV500 = [0x0, 0x6] | Hi3516DV300 = [0x0, 0x6]|
                                                                       Hi3516AV300 = [0x0, 0x6]|Hi3516EV200 = [0x0, 0x1]|Hi3516EV300 = [0x0, 0x1]|Hi3518EV300 = [0x0, 0x1]|Hi3516DV200 = [0x0, 0x1]; */
    ISP_BAYER_FORMAT_E  enPattern;      /* RW; Range: [0x0, 0x3]; Format:2.0;raw domain pattern */
} ISP_AF_RAW_CFG_S;
/* Configures the pre-filter of the AF module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_PRE_FILTER_CFG_S {
    HI_BOOL             bEn;         /* RW; Range: [0,1]; Format:1.0; pre filter enable  .          */
    HI_U16              u16strength; /* RW; Range: [0x0, 0xFFFF]; Format:16.0;pre filter strength    */
} ISP_AF_PRE_FILTER_CFG_S;

/* Defines AF statistics configuration parameters. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_CFG_S {
    HI_BOOL                 bEnable;        /* RW; Range: [0,1];   AF enable. */
    HI_U16                  u16Hwnd;        /* RW; Range: [1, 17]; AF statistic window horizontal block.  */
    HI_U16                  u16Vwnd;        /* RW; Range: [1, 15]; AF statistic window veritical block.  */
    HI_U16                  u16Hsize;       /* RW; Range: Hi3559AV100 = [256, 8192]|Hi3519AV100 = [256, 8192]|Hi3516CV500 = [256, 4608]|Hi3516DV300 = [256, 4608]|
                                                                              Hi3516AV300 = [256, 4608]|Hi3516EV200 = [256,3072]|Hi3516EV300 = [256,3072]|Hi3518EV300 = [256,3072]|Hi3516DV200 = [256,3072];
                                                                              limited Range: [256, ImageWidth],AF image width. */
    HI_U16                  u16Vsize;       /* RW; Range: Hi3559AV100 = [120, 8192]|Hi3519AV100 = [120, 8192]|Hi3516CV500 = [120, 4608]|Hi3516DV300 = [120, 4608]|
                                                                              Hi3516AV300 = [120, 4608]|Hi3516EV200 = [120,3072]|Hi3516EV300 = [120,3072]|Hi3518EV300 = [120,3072]|Hi3516DV200 = [120,3072];
                                                                              limited Range: [120, ImageHeight],AF image height. */
    ISP_AF_PEAK_MODE_E      enPeakMode;     /* RW; Range: [0,1]; AF peak value statistic mode. */
    ISP_AF_SQU_MODE_E       enSquMode;      /* RW; Range: [0,1]; AF statistic square accumulate. */
    ISP_AF_CROP_S           stCrop;         /* RW; AF input image crop */
    ISP_AF_STATISTICS_POS_E enStatisticsPos; /* RW; Range: [0,2]; AF statistic position, it can be set to yuv or raw */
    ISP_AF_RAW_CFG_S        stRawCfg;       /* RW; When AF locate at RAW domain, these para should be cfg. */
    ISP_AF_PRE_FILTER_CFG_S stPreFltCfg;    /* RW; pre filter cfg */
    HI_U16                  u16HighLumaTh;  /* RW; Range: [0,0xFF]; high luma threshold. */
} ISP_AF_CFG_S;
/* Configures the AF level depend gain module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_LD_S {
    HI_BOOL     bLdEn;                      /* RW; Range: [0, 1]; FILTER level depend gain enable. */
    HI_U16      u16ThLow;                   /* RW; range: [0x0, 0xFF]; FILTER level depend th low */
    HI_U16      u16GainLow;                 /* RW; range: [0x0, 0xFF]; FILTER level depend gain low */
    HI_U16      u16SlpLow;                  /* RW; range: [0x0, 0xF];  FILTER level depend slope low */
    HI_U16      u16ThHigh;                  /* RW; range: [0x0, 0xFF]; FILTER level depend th high */
    HI_U16      u16GainHigh;                /* RW; range: [0x0, 0xFF]; FILTER level depend gain high */
    HI_U16      u16SlpHigh;                 /* RW; range: [0x0, 0xF];  FILTER level depend slope high */
} ISP_AF_LD_S;
/* Configures the AF coring module. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_CORING_S {
    HI_U16      u16Th;                      /* RW; Range: [0x0, 0x7FF];FILTER coring threshold. */
    HI_U16      u16Slp;                     /* RW; Range: [0x0, 0xF];  FILTER Coring Slope */
    HI_U16      u16Lmt;                     /* RW; Range: [0x0, 0x7FF];FILTER coring limit */
} ISP_AF_CORING_S ;


#define IIR_EN_NUM                                (3)
#define IIR_GAIN_NUM                              (7)
#define IIR_SHIFT_NUM                             (4)
/* Defines the IIR parameter configuration of horizontal filters for AF statistics. Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_H_PARAM_S {
    HI_BOOL         bNarrowBand;                /* RW; Range: [0, 1]; IIR narrow band enable. */
    HI_BOOL         abIIREn[IIR_EN_NUM];        /* RW; Range: [0, 1]; IIR enable. */
    HI_U8           u8IIRShift;                 /* RW; Range: [0, 63]; IIR Shift. Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_S16          as16IIRGain[IIR_GAIN_NUM];  /* RW; Range: [-511, 511]. IIR gain,gain0 range:[0,255]. */
    HI_U16          au16IIRShift[IIR_SHIFT_NUM]; /* RW; Range: [0x0, 0x7];  IIR shift. */
    ISP_AF_LD_S     stLd;                       /* RW; filter level depend. */
    ISP_AF_CORING_S stCoring;                   /* RW; filter coring. */
} ISP_AF_H_PARAM_S;

#define FIR_GAIN_NUM                              (5)

/* Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_V_PARAM_S {
    HI_S16          as16FIRH[FIR_GAIN_NUM]; /* RW; Range: [-31, 31];   FIR gain. */
    ISP_AF_LD_S     stLd;                   /* RW; filter level depend. */
    ISP_AF_CORING_S stCoring;               /* RW; filter coring.  */
} ISP_AF_V_PARAM_S;

#define ACC_SHIFT_H_NUM                           (2)
#define ACC_SHIFT_V_NUM                           (2)

/* Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_AF_FV_PARAM_S {
    HI_U16 u16AccShiftY;                    /* RW; Range: [0x0, 0xF]; luminance Y statistic shift. */
    HI_U16 au16AccShiftH[ACC_SHIFT_H_NUM];  /* RW; Range: [0x0, 0xF]; IIR statistic shift. */
    HI_U16 au16AccShiftV[ACC_SHIFT_V_NUM];  /* RW; Range: [0x0, 0xF]; FIR statistic shift. */
    HI_U16 u16HlCntShift;                   /* RW; Range: [0x0, 0xF]; High luminance counter shift */
} ISP_AF_FV_PARAM_S;

/* Not support for Hi3559V200/Hi3556V200 */
typedef struct hiISP_FOCUS_STATISTICS_CFG_S {
    ISP_AF_CFG_S        stConfig;
    ISP_AF_H_PARAM_S    stHParam_IIR0;
    ISP_AF_H_PARAM_S    stHParam_IIR1;
    ISP_AF_V_PARAM_S    stVParam_FIR0;
    ISP_AF_V_PARAM_S    stVParam_FIR1;
    ISP_AF_FV_PARAM_S   stFVParam;
} ISP_FOCUS_STATISTICS_CFG_S;

/*
   the main purpose of stat key was to access individual statistic info separately...
   ...for achieving performance optimization of CPU, because of we acquire stat...
   ... in ISP_DRV ISR for our firmware reference and USER can also use alternative MPI...
   ... to do this job, so bit1AeStat1~bit1AfStat for MPI behavior control, and bit16IsrAccess...
   ... for ISR access control, they were independent. but they have the same bit order, for example...
   ... bit1AeStat1 for MPI AeStat1 access key, and bit16 of u32Key for ISR AeStat1 access key
*/
typedef union hiISP_STATISTICS_CTRL_U {
    HI_U64  u64Key;
    struct {
        HI_U64  bit1FEAeGloStat     : 1 ;   /* [0] */
        HI_U64  bit1FEAeLocStat     : 1 ;   /* [1] */
        HI_U64  bit1FEAeStiGloStat  : 1 ;   /* [2] .Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bit1FEAeStiLocStat  : 1 ;   /* [3] .Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bit1BEAeGloStat     : 1 ;   /* [4] */
        HI_U64  bit1BEAeLocStat     : 1 ;   /* [5] */
        HI_U64  bit1BEAeStiGloStat  : 1 ;   /* [6] .Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bit1BEAeStiLocStat  : 1 ;   /* [7] .Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bit1AwbStat1        : 1 ;   /* [8] */
        HI_U64  bit1AwbStat2        : 1 ;   /* [9] */
        HI_U64  bit2Rsv0            : 2 ;   /* [10:11] */
        HI_U64  bit1FEAfStat        : 1 ;   /* [12] .Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
        HI_U64  bit1BEAfStat        : 1 ;   /* [13] .Not support for Hi3559V200/Hi3556V200 */
        HI_U64  bit2Rsv1            : 2 ;   /* [14:15] */
        HI_U64  bit1Dehaze          : 1 ;   /* [16] */
        HI_U64  bit1MgStat          : 1 ;   /* [17] */
        HI_U64  bit14Rsv            : 14;   /* [18:31] */
        HI_U64  bit32IsrAccess      : 32;   /* [32:63] */
    };
} ISP_STATISTICS_CTRL_U;


/* statistics structs */
#define HIST_NUM                                  (1024)
#define BAYER_PATTERN_NUM                         (4)
#define WDR_CHN_MAX                               (4)
#define ISP_CHN_MAX_NUM                           (4)

typedef struct hiISP_AE_GRID_INFO_S {
    HI_U16 au16GridYPos[AE_ZONE_ROW + 1];   /* R */
    HI_U16 au16GridXPos[AE_ZONE_COLUMN + 1]; /* R */
    HI_U8  u8Status;                        /* R;0:not update, 1: update,others:reserved */
} ISP_AE_GRID_INFO_S;

typedef struct hiISP_MG_GRID_INFO_S {
    HI_U16 au16GridYPos[MG_ZONE_ROW + 1];    /* R */
    HI_U16 au16GridXPos[MG_ZONE_COLUMN + 1]; /* R */
    HI_U8  u8Status;                         /* R;0:not update, 1: update,others:reserved */
} ISP_MG_GRID_INFO_S;

typedef struct hiISP_AWB_GRID_INFO_S {
    HI_U16 au16GridYPos[AWB_ZONE_ORIG_ROW  + 1];   /* R */
    HI_U16 au16GridXPos[AWB_ZONE_ORIG_COLUMN + 1]; /* R */
    HI_U8  u8Status;                               /* R;0:not update, 1: update,others:reserved */
} ISP_AWB_GRID_INFO_S;

typedef struct hiISP_FOCUS_GRID_INFO_S {
    HI_U16 au16GridYPos[AF_ZONE_ROW + 1];    /* R */
    HI_U16 au16GridXPos[AF_ZONE_COLUMN + 1]; /* R */
    HI_U8  u8Status;                         /* R;0:not update, 1: update,others:reserved */
} ISP_FOCUS_GRID_INFO_S;


typedef struct hiISP_AE_STATISTICS_S {
    HI_U32 au32FEHist1024Value[ISP_CHN_MAX_NUM][HIST_NUM];                                 /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0; Global 1024 bins histogram of FE */
    HI_U16 au16FEGlobalAvg[ISP_CHN_MAX_NUM][BAYER_PATTERN_NUM];                            /* R; Range: [0x0, 0xFFFF]; Format:16.0; Global average value of FE,Not support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16 au16FEZoneAvg[ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM]; /* R; Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of FE, Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200's Fe1 and Fe3,
                                                                                                                                                           Not support Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U32 au32BEHist1024Value[HIST_NUM];                                                  /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0; Global 1024 bins histogram of BE */
    HI_U16 au16BEGlobalAvg[BAYER_PATTERN_NUM];                                             /* R; Range: [0x0, 0xFFFF]; Format:16.0; Global average value of BE */
    HI_U16 au16BEZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM];                  /* R; Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of BE */
    ISP_AE_GRID_INFO_S stFEGridInfo;
    ISP_AE_GRID_INFO_S stBEGridInfo;
} ISP_AE_STATISTICS_S;

/* Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200 */
typedef struct hiISP_AE_STITCH_STATISTICS_S {
    HI_U32 au32FEHist1024Value[ISP_CHN_MAX_NUM][HIST_NUM];                                                  /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0; Global 1024 bins histogram of FE.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16 au16FEGlobalAvg[ISP_CHN_MAX_NUM][BAYER_PATTERN_NUM];                                             /* R; Range: [0x0, 0xFFFF]; Format:16.0; Global average value of FE.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16 au16FEZoneAvg[VI_MAX_PIPE_NUM][ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM]; /* R; Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of FE.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U32 au32BEHist1024Value[HIST_NUM];                                                                   /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0; Global 1024 bins histogram of BE.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16 au16BEGlobalAvg[BAYER_PATTERN_NUM];                                                              /* R; Range: [0x0, 0xFFFF]; Format:16.0; Global average value of BE.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    HI_U16 au16BEZoneAvg[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM];                  /* R; Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of BE.Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
} ISP_AE_STITCH_STATISTICS_S;

typedef struct hiISP_MG_STATISTICS_S {
    HI_U16 au16ZoneAvg[MG_ZONE_ROW][MG_ZONE_COLUMN][BAYER_PATTERN_NUM];         /* R; Range: [0x0, 0xFF]; Format:8.0; Zone average value */
    ISP_MG_GRID_INFO_S stGridInfo;
} ISP_MG_STATISTICS_S;


/************ AWB*************/

typedef enum hiISP_AWB_SWITCH_E {
    ISP_AWB_AFTER_DG                   = 0,
    ISP_AWB_AFTER_Expander,
    ISP_AWB_AFTER_DRC,
    ISP_AWB_SWITCH_BUTT
} ISP_AWB_SWITCH_E;

/* Crops the input image of the AWB module */
typedef struct hiISP_AWB_CROP_S {
    HI_BOOL bEnable;  /* RW; Range: [0,1];  Format:1.0;AWB crop enable */
    HI_U16  u16X;     /* RW; Range: Hi3559AV100 = [0, 8192 - 60]|Hi3519AV100 = [0, 8192 - 60]|Hi3516CV500 = [0, 4608 - 60]|Hi3516DV300 = [0, 4608 - 60]|
                                         Hi3516AV300 = [0, 4608 - 60]|Hi3559V200   = [0, 4608 - 60]|Hi3556V200   = [0, 4608 - 60]|
                                         Hi3516EV200 = [0,3072- 60]|Hi3516EV300 = [0,3072- 60]|Hi3518EV300 = [0,3072- 60]|Hi3516DV200 = [0,3072- 60];
                                         Format:13.0;AWB image crop start x, limited range:[0, ImageWidth - u16ZoneCol * 60] */
    HI_U16  u16Y;     /* RW; Range: Hi3559AV100 = [0, 8192 - 14]|Hi3519AV100 = [0, 8192 - 14]|Hi3516CV500 = [0, 4608 - 14]|Hi3516DV300 = [0, 4608 - 14]|
                                         Hi3516AV300 = [0, 4608 - 14]|Hi3559V200   = [0, 4608 - 14]|Hi3556V200   = [0, 4608 - 14]|
                                         Hi3516EV200 = [0,3072-14]|Hi3516EV300 = [0,3072-14]|Hi3518EV300 = [0,3072-14]|Hi3516DV200 = [0,3072-14];
                                         Format:13.0;AWB image crop start y,limited range:[0, ImageHeight - u16ZoneRow * 14] */
    HI_U16  u16W;     /* RW; Range: Hi3559AV100 = [60, 8192]|Hi3519AV100 = [60, 8192]|Hi3516CV500 = [60, 4608]|Hi3516DV300 = [60, 4608] |
                                         Hi3516AV300 = [60, 4608] |Hi3559V200   = [60, 4608]| Hi3556V200  = [60, 4608]|
                                         Hi3516EV200 = [60,3072]|Hi3516EV300 = [60,3072]|Hi3518EV300 = [60,3072]|Hi3516DV200 = [60,3072];
                                         Format:14.0;AWB image crop width, limited range:[u16ZoneCol * 60, ImageWidth] */
    HI_U16  u16H;     /* RW; Range: Hi3559AV100 = [14, 8192]|Hi3519AV100 = [14, 8192]|Hi3516CV500 = [14, 4608]|Hi3516DV300 = [14, 4608] |
                                         Hi3516AV300 = [14, 4608] |Hi3559V200 = [14, 4608]| Hi3556V200 = [14, 4608] |
                                         Hi3516EV200 = [14,3072]|Hi3516EV300 = [14,3072]|Hi3518EV300 = [14,3072]|Hi3516DV200 = [14,3072];
                                         Format:14.0;AWB image crop height, limited range:[u16ZoneRow * 14, ImageHeight] */
} ISP_AWB_CROP_S;

/* Defines the AWB statistics configuration */
typedef struct hiISP_WB_STATISTICS_CFG_S {
    ISP_AWB_SWITCH_E enAWBSwitch; /* RW; Range: [0x0, 0x2]; Position of AWB statistics in pipeline */
    HI_U16 u16ZoneRow;            /* RW; Range: [0x1, 0x20]; Vertical Blocks, limited range:[1, min(32, ImageHeight /AWB_MIN_HEIGHT)] */
    HI_U16 u16ZoneCol;            /* RW; Range: [0x1, 0x20]; Horizontal Blocks, limited range:[BlkNum, min(32, Width /AWB_MIN_WIDTH)] */
    HI_U16 u16ZoneBin;            /* RW; Range:Hi3559AV100 = [1, 4] | Hi3519AV100 = [1, 4] | Hi3516CV500 = [1, 1]|Hi3516DV300 = [1, 1]|
                                                         Hi3516AV300 = [1, 1]|Hi3559V200 = [1, 1]|Hi3556V200 = [1, 1]|
                                                         Hi3516EV200 = [1,1]|Hi3516EV300 = [1,1]|Hi3518EV300 = [1,1]|Hi3516DV200 = [1,1]; Brightness Bins */
    HI_U16 au16HistBinThresh[4]; /* RW; Range: [0x0, 0xFFFF]; Bin Threshold */
    HI_U16 u16WhiteLevel;        /* RW; Range: [0x0, 0xFFFF];Upper limit of valid data for white region,  for Bayer statistics, [0x0, 0x3FF] for RGB statistics */
    HI_U16 u16BlackLevel;        /* RW; Range: [0x0, 0xFFFF];limited range: [0x0, u16WhiteLevel],Lower limit of valid data for white region . for Bayer statistics, bitwidth is 12, for RGB statistics, bitwidth is 10 */
    HI_U16 u16CbMax;             /* RW; Range: [0x0, 0xFFF];Maximum value of B/G for white region */
    HI_U16 u16CbMin;             /* RW; Range: [0x0, 0xFFF];limited range: [0x0, u16CbMax]Minimum value of B/G for white region */
    HI_U16 u16CrMax;             /* RW; Range: [0x0, 0xFFF];Maximum value of R/G for white region */
    HI_U16 u16CrMin;             /* RW; Range: [0x0, 0xFFF];limited range: [0x0, u16CrMax],Minimum value of R/G for white region */
    ISP_AWB_CROP_S stCrop;
} ISP_WB_STATISTICS_CFG_S;

typedef struct hiISP_WB_STITCH_STATISTICS_S {
    HI_U16  u16ZoneRow;                                 /* R; Range: [0x0, 0x20]; effective horizontal block number for AWB statistic stitch window */
    HI_U16  u16ZoneCol;                                 /* R; Range: [0x0, 0x80]; effective vetical block number for AWB statistic stitch window */
    HI_U16  au16ZoneAvgR[AWB_ZONE_STITCH_MAX];          /* R; Range: [0x0, 0xFFFF];Zone Average R  for Stitich mode */
    HI_U16  au16ZoneAvgG[AWB_ZONE_STITCH_MAX];          /* R; Range: [0x0, 0xFFFF];Zone Average G  for Stitich mode */
    HI_U16  au16ZoneAvgB[AWB_ZONE_STITCH_MAX];          /* R; Range: [0x0, 0xFFFF];Zone Average B  for Stitich mode */
    HI_U16  au16ZoneCountAll[AWB_ZONE_STITCH_MAX];      /* R; Range: [0x0, 0xFFFF];normalized number of Gray points  for Stitich mode */
} ISP_WB_STITCH_STATISTICS_S;

typedef struct hiISP_WB_STATISTICS_S {
    HI_U16 u16GlobalR;          /* R; Range: [0x0, 0xFFFF];Global WB output Average R */
    HI_U16 u16GlobalG;          /* R; Range: [0x0, 0xFFFF];Global WB output Average G */
    HI_U16 u16GlobalB;          /* R; Range: [0x0, 0xFFFF];Global WB output Average B */
    HI_U16 u16CountAll;         /* R; Range: [0x0, 0xFFFF];normalized number of Gray points */

    HI_U16 au16ZoneAvgR[AWB_ZONE_NUM];            /* R; Range: [0x0, 0xFFFF];Zone Average R */
    HI_U16 au16ZoneAvgG[AWB_ZONE_NUM];            /* R; Range: [0x0, 0xFFFF];Zone Average G */
    HI_U16 au16ZoneAvgB[AWB_ZONE_NUM];            /* R; Range: [0x0, 0xFFFF];Zone Average B */
    HI_U16 au16ZoneCountAll[AWB_ZONE_NUM];        /* R; Range: [0x0, 0xFFFF];normalized number of Gray points */
    ISP_AWB_GRID_INFO_S stGridInfo;
} ISP_WB_STATISTICS_S;

typedef struct hiISP_FOCUS_ZONE_S {
    HI_U16  u16v1;             /* R; Range: [0x0, 0xFFFF];vertical fir block1 Statistics */
    HI_U16  u16h1;             /* R; Range: [0x0, 0xFFFF];horizontal iir block1 Statistics */
    HI_U16  u16v2;             /* R; Range: [0x0, 0xFFFF];vertical fir block2 Statistics */
    HI_U16  u16h2;             /* R; Range: [0x0, 0xFFFF];horizontal iir block2 Statistics */
    HI_U16  u16y;              /* R; Range: [0x0, 0xFFFF];Y Statistics */
    HI_U16  u16HlCnt;          /* R; Range: [0x0, 0xFFFF];HlCnt Statistics */
} ISP_FOCUS_ZONE_S;

typedef struct hiISP_FE_FOCUS_STATISTICS_S {
    ISP_FOCUS_ZONE_S stZoneMetrics[WDR_CHN_MAX][AF_ZONE_ROW][AF_ZONE_COLUMN]; /* R; The zoned measure of contrast */
} ISP_FE_FOCUS_STATISTICS_S;

typedef struct hiISP_BE_FOCUS_STATISTICS_S {
    ISP_FOCUS_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /* R; The zoned measure of contrast */
} ISP_BE_FOCUS_STATISTICS_S;

typedef struct hiISP_AF_STATISTICS_S {
    ISP_FE_FOCUS_STATISTICS_S      stFEAFStat; /* Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_BE_FOCUS_STATISTICS_S      stBEAFStat; /* Not support for Hi3559V200/Hi3556V200 */
    ISP_FOCUS_GRID_INFO_S          stFEAFGridInfo; /* Not support for Hi3516CV500/Hi3516DV300/Hi3516AV300/Hi3559V200/Hi3556V200/Hi3516EV200/Hi3516EV300/Hi3518EV300/Hi3516DV200 */
    ISP_FOCUS_GRID_INFO_S          stBEAFGridInfo; /* Not support for Hi3559V200/Hi3556V200 */
} ISP_AF_STATISTICS_S;


typedef struct hiISP_STATISTICS_CFG_S {
    ISP_STATISTICS_CTRL_U       unKey;
    ISP_AE_STATISTICS_CFG_S     stAECfg;
    ISP_WB_STATISTICS_CFG_S     stWBCfg;
    ISP_FOCUS_STATISTICS_CFG_S  stFocusCfg; /* Not support for Hi3559V200/Hi3556V200 */
} ISP_STATISTICS_CFG_S;

typedef struct hiISP_INIT_ATTR_S {
    HI_BOOL bIsIrMode;
    HI_U32 u32ExpTime;
    HI_U32 u32AGain;
    HI_U32 u32DGain;
    HI_U32 u32ISPDGain;
    HI_U32 u32Exposure;
    HI_U32 u32InitIso;
    HI_U32 u32LinesPer500ms;
    HI_U32 u32PirisFNO;
    HI_U16 u16WBRgain;
    HI_U16 u16WBGgain;
    HI_U16 u16WBBgain;
    HI_U16 u16SampleRgain;
    HI_U16 u16SampleBgain;
    HI_U16 au16CCM[CCM_MATRIX_SIZE];
} ISP_INIT_ATTR_S ;

/* ISP debug information */
typedef struct hiISP_DEBUG_INFO_S {
    HI_BOOL bDebugEn;       /* RW; 1:enable debug, 0:disable debug */
    HI_U64  u64PhyAddr;     /* RW; phy address of debug info */
    HI_U32  u32Depth;       /* RW; depth of debug info */
} ISP_DEBUG_INFO_S;

typedef struct hiISP_DBG_ATTR_S {
    HI_U32  u32Rsv;         /* H;need to add member */
} ISP_DBG_ATTR_S;

typedef struct hiISP_DBG_STATUS_S {
    HI_U32  u32FrmNumBgn;
    HI_U32  u32Rsv;         /* H;need to add member */
    HI_U32  u32FrmNumEnd;
} ISP_DBG_STATUS_S;

/*
0 = Communication between the sensor and the ISP over the I2C interface
1 = Communication between the sensor and the ISP over the SSP interface
*/
typedef enum hiISP_SNS_TYPE_E {
    ISP_SNS_I2C_TYPE = 0,
    ISP_SNS_SSP_TYPE,

    ISP_SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

/* sensor communication bus */
typedef union hiISP_SNS_COMMBUS_U {
    HI_S8   s8I2cDev;
    struct {
        HI_S8  bit4SspDev       : 4;
        HI_S8  bit4SspCs        : 4;
    } s8SspDev;
} ISP_SNS_COMMBUS_U;

typedef struct hiISP_I2C_DATA_S {
    HI_BOOL bUpdate;            /* RW; Range: [0x0, 0x1]; Format:1.0; HI_TRUE: The sensor registers are written,HI_FALSE: The sensor registers are not written */
    HI_U8   u8DelayFrmNum;      /* RW; Number of delayed frames for the sensor register */
    HI_U8   u8IntPos;           /* RW;Position where the configuration of the sensor register takes effect
                                 0x0:very short frame start interrupt, 0x1:very short frame end interrupt,
                                 0x10:short frame start interrupt, 0x11:short frame end interrupt,
                                 0x20:middle frame start interrupt, 0x21:middle frame end interrupt,
                                 0x30:long frame start interrupt, 0x31:long frame end interrupt */

    HI_U8   u8DevAddr;          /* RW;Sensor device address */
    HI_U32  u32RegAddr;         /* RW;Sensor register address */
    HI_U32  u32AddrByteNum;     /* RW;Bit width of the sensor register address */
    HI_U32  u32Data;            /* RW;Sensor register data */
    HI_U32  u32DataByteNum;     /* RW;Bit width of sensor register data */
} ISP_I2C_DATA_S;

typedef struct hiISP_SSP_DATA_S {
    HI_BOOL bUpdate;            /* RW; Range: [0x0, 0x1]; Format:1.0; HI_TRUE: The sensor registers are written,HI_FALSE: The sensor registers are not written */
    HI_U8   u8DelayFrmNum;      /* RW; Number of delayed frames for the sensor register */
    HI_U8   u8IntPos;           /* RW;Position where the configuration of the sensor register takes effect
                                 0x0:very short frame start interrupt, 0x1:very short frame end interrupt,
                                 0x10:short frame start interrupt, 0x11:short frame end interrupt,
                                 0x20:middle frame start interrupt, 0x21:middle frame end interrupt,
                                 0x30:long frame start interrupt, 0x31:long frame end interrupt */
    HI_U32  u32DevAddr;         /* RW;Sensor device address */
    HI_U32  u32DevAddrByteNum;  /* RW;Bit width of the sensor device address */
    HI_U32  u32RegAddr;         /* RW;Sensor register address */
    HI_U32  u32RegAddrByteNum;  /* RW;Bit width of the sensor register address */
    HI_U32  u32Data;            /* RW;Sensor register data */
    HI_U32  u32DataByteNum;     /* RW;Bit width of sensor register data */
} ISP_SSP_DATA_S;

typedef struct hiISP_SNS_REGS_INFO_S {
    ISP_SNS_TYPE_E enSnsType;
    HI_U32  u32RegNum;              /* RW;Number of registers required when exposure results are written to the sensor. The member value cannot be dynamically changed */
    HI_U8   u8Cfg2ValidDelayMax;    /* RW;Maximum number of delayed frames from the time when all sensor registers are configured to the
                                      time when configurations take effect, which is used to ensure the synchronization between sensor registers and ISP registers */
    ISP_SNS_COMMBUS_U  unComBus;
    union {
        ISP_I2C_DATA_S astI2cData[ISP_MAX_SNS_REGS];
        ISP_SSP_DATA_S astSspData[ISP_MAX_SNS_REGS];
    };

    struct {
        HI_BOOL bUpdate;
        HI_U8   u8DelayFrmNum;
        HI_U32  u32SlaveVsTime;      /* RW;time of vsync. Unit: inck clock cycle */
        HI_U32  u32SlaveBindDev;
    } stSlvSync;

    HI_BOOL bConfig;
} ISP_SNS_REGS_INFO_S;

typedef enum hiISP_VD_TYPE_E {
    ISP_VD_FE_START   = 0,
    ISP_VD_FE_END,
    ISP_VD_BE_END,

    ISP_VD_BUTT
} ISP_VD_TYPE_E;

/* Defines the attributes of the virtual addresses for the registers of ISP submodules */
typedef struct hiISP_REG_ATTR_S {
    HI_VOID *pIspExtRegAddr;        /* R;Start virtual address for the ISP external virtual registers */
    HI_U32 u32IspExtRegSize;        /* R;Size of the ISP external virtual registers */
    HI_VOID *pAeExtRegAddr;         /* R;Start virtual address for the AE library module */
    HI_U32 u32AeExtRegSize;         /* R;Size of the AE library module */
    HI_VOID *pAwbExtRegAddr;        /* R;Start virtual address for the AWB library module */
    HI_U32 u32AwbExtRegSize;        /* R;Size of the AWB library module */
} ISP_REG_ATTR_S;


/******************************************************/
/********************* AI structs ************************/
/*
Defines the ISP iris type
0 = DC iris
1 = P iris
*/
typedef enum hiISP_IRIS_TYPE_E {
    ISP_IRIS_DC_TYPE = 0,
    ISP_IRIS_P_TYPE,

    ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;

/*
Defines the F value of the ISP iris
*/
typedef enum hiISP_IRIS_F_NO_E {
    ISP_IRIS_F_NO_32_0 = 0,
    ISP_IRIS_F_NO_22_0,
    ISP_IRIS_F_NO_16_0,
    ISP_IRIS_F_NO_11_0,
    ISP_IRIS_F_NO_8_0,
    ISP_IRIS_F_NO_5_6,
    ISP_IRIS_F_NO_4_0,
    ISP_IRIS_F_NO_2_8,
    ISP_IRIS_F_NO_2_0,
    ISP_IRIS_F_NO_1_4,
    ISP_IRIS_F_NO_1_0,

    ISP_IRIS_F_NO_BUTT,
} ISP_IRIS_F_NO_E;

typedef struct hiISP_DCIRIS_ATTR_S {
    HI_S32 s32Kp;                    /* RW; Range:[0, 100000]; Format:32.0; the proportional gain of PID algorithm, default value is 7000 */
    HI_S32 s32Ki;                    /* RW; Range:[0, 1000]; Format:32.0; the integral gain of PID algorithm, default value is 100 */
    HI_S32 s32Kd;                    /* RW; Range:[0, 100000]; Format:32.0; the derivative gain of PID algorithm, default value is 3000 */
    HI_U32 u32MinPwmDuty;            /* RW; Range:[0, 1000]; Format:32.0; which is the min pwm duty for dciris control */
    HI_U32 u32MaxPwmDuty;            /* RW; Range:[0, 1000]; Format:32.0; which is the max pwm duty for dciris control */
    HI_U32 u32OpenPwmDuty;           /* RW; Range:[0, 1000]; Format:32.0; which is the open pwm duty for dciris control */
} ISP_DCIRIS_ATTR_S;

typedef struct hiISP_PIRIS_ATTR_S {
    HI_BOOL bStepFNOTableChange;    /* W; Range:[0, 1]; Format:1.0; Step-F number mapping table change or not */
    HI_BOOL bZeroIsMax;             /* RW; Range:[0, 1]; Format:1.0; Step 0 corresponds to max aperture or not, it's related to the specific iris */
    HI_U16 u16TotalStep;            /* RW; Range:[1, 1024]; Format:16.0; Total steps of  Piris's aperture, it's related to the specific iris */
    HI_U16 u16StepCount;            /* RW; Range:[1, 1024]; Format:16.0; Used steps of Piris's aperture. when Piris's aperture is too small, the F number precision is not enough,
                                          so not all the steps can be used. It's related to the specific iris */
    HI_U16 au16StepFNOTable[AI_MAX_STEP_FNO_NUM];   /* RW; Range:[1, 1024]; Format:16.0; Step-F number mapping table. F1.0 is expressed as 1024, F32 is expressed as 1, it's related to the specific iris */
    ISP_IRIS_F_NO_E  enMaxIrisFNOTarget;  /* RW; Range:[0, 10]; Format:4.0; Max F number of Piris's aperture, it's related to the specific iris */
    ISP_IRIS_F_NO_E  enMinIrisFNOTarget;  /* RW; Range:[0, 10]; Format:4.0; Min F number of Piris's aperture, it's related to the specific iris */

    HI_BOOL bFNOExValid;            /* RW; Range:[0, 1]; Format:1.0; use equivalent gain to present FNO or not */
    HI_U32 u32MaxIrisFNOTarget;    /* RW; Range:[1, 1024]; Format:16.0; Max equivalent gain of F number of Piris's aperture, only used when bFNOExValid is true, it's related to the specific iris */
    HI_U32 u32MinIrisFNOTarget;    /* RW; Range:[1, 1024]; Format:16.0; Min equivalent gain of F number of Piris's aperture, only used when bFNOExValid is true, it's related to the specific iris */
} ISP_PIRIS_ATTR_S;

/* Defines the MI attribute */
typedef struct hiISP_MI_ATTR_S {
    HI_U32  u32HoldValue;            /* RW; Range:[0, 1000]; Format:32.0; iris hold value for DC-iris */
    ISP_IRIS_F_NO_E  enIrisFNO;      /* RW; Range:[0, 10]; Format:4.0; the F number of P-iris */
} ISP_MI_ATTR_S;

/*
Defines the ISP iris status
0 = In this mode can set the MI holdvalue
1 = Open Iris to the max
2 = Close Iris to the min
*/
typedef enum hiISP_IRIS_STATUS_E {
    ISP_IRIS_KEEP  = 0,
    ISP_IRIS_OPEN  = 1,
    ISP_IRIS_CLOSE = 2,
    ISP_IRIS_BUTT
} ISP_IRIS_STATUS_E;

/* Defines the iris attribute */
typedef struct hiISP_IRIS_ATTR_S {
    HI_BOOL bEnable;                 /* RW;iris enable/disable */
    ISP_OP_TYPE_E   enOpType;        /* auto iris or manual iris */
    ISP_IRIS_TYPE_E enIrisType;      /* DC-iris or P-iris */
    ISP_IRIS_STATUS_E enIrisStatus;  /* RW; status of Iris */
    ISP_MI_ATTR_S   stMIAttr;
} ISP_IRIS_ATTR_S;


/******************************************************/
/********************* AE structs ************************/
/*
Defines the AE mode
0 = Automatic frame rate reduction mode (slow shutter mode)
1 = Constant frame rate mode
*/
typedef enum hiISP_AE_MODE_E {
    AE_MODE_SLOW_SHUTTER = 0,
    AE_MODE_FIX_FRAME_RATE  = 1,
    AE_MODE_BUTT
} ISP_AE_MODE_E;
/*
Defines the AE exposure policy mode
0 = Highlight first exposure mode
1 = Lowlight first exposure mode
*/
typedef enum hiISP_AE_STRATEGY_E {
    AE_EXP_HIGHLIGHT_PRIOR = 0,
    AE_EXP_LOWLIGHT_PRIOR  = 1,
    AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;
/* Defines the maximum exposure time or gain and minimum exposure time or gain */
typedef struct hiISP_AE_RANGE_S {
    HI_U32 u32Max;  /* RW;Range:[0,0xFFFFFFFF];Format:32.0;Maximum value */
    HI_U32 u32Min;  /* RW;Range:[0,0xFFFFFFFF];Format:32.0;limited Range:[0,u32Max],Minimum value */
} ISP_AE_RANGE_S;

/* Defines the ISP exposure delay attribute */
typedef struct hiISP_AE_DELAY_S {
    HI_U16 u16BlackDelayFrame;    /* RW; Range:[0, 65535]; Format:16.0; AE black delay frame count */
    HI_U16 u16WhiteDelayFrame;    /* RW; Range:[0, 65535]; Format:16.0; AE white delay frame count */
} ISP_AE_DELAY_S;

/*
Defines the anti-flicker mode.
0 = The epxosure time is fixed to be the multiplies of 1/(2*frequency) sec,it may lead to over exposure in the high-luminance environments.
1 = The anti flicker may be closed to avoid over exposure in the high-luminance environments.
*/
typedef enum hiISP_ANTIFLICKER_MODE_E {
    ISP_ANTIFLICKER_NORMAL_MODE = 0x0,
    ISP_ANTIFLICKER_AUTO_MODE   = 0x1,
    ISP_ANTIFLICKER_MODE_BUTT
} ISP_ANTIFLICKER_MODE_E;

/* Defines the anti-flicker attribute */
typedef struct hiISP_ANTIFLICKER_S {
    HI_BOOL bEnable;              /* RW; Range:[0, 1]; Format:1.0; */
    HI_U8   u8Frequency;          /* RW; Range:[0, 255]; Format:8.0; usually this value is 50 or 60  which is the frequency of the AC power supply */
    ISP_ANTIFLICKER_MODE_E  enMode;
} ISP_ANTIFLICKER_S;

typedef struct hiISP_SUBFLICKER_S {
    HI_BOOL bEnable; /* RW; Range:[0, 1]; Format:1.0; */

    HI_U8   u8LumaDiff;     /* RW; Range:[0, 100]; Format:8.0; if subflicker mode enable, current luma is less than AE compensation plus LumaDiff,
                                AE will keep min antiflicker shutter time(for example: 1/100s or 1/120s) to avoid flicker. while current luma is
                                larger than AE compensation plus the LumaDiff, AE will reduce shutter time to avoid over-exposure and introduce
                                flicker in the pircture */
} ISP_SUBFLICKER_S;

/*
Defines the ISP FSWDR operating mode
0 = Normal FSWDR mode
1 = Long frame mode, only effective in LINE_WDR, when running in this mode FSWDR module only output the long frame data
*/
typedef enum hiISP_FSWDR_MODE_E {
    ISP_FSWDR_NORMAL_MODE          = 0x0,
    ISP_FSWDR_LONG_FRAME_MODE      = 0x1,
    ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2, /* Auto long frame mode, only effective in LINE_WDR, When running in this mode, normal WDR and long frame mode would auto switch */
    ISP_FSWDR_MODE_BUTT
} ISP_FSWDR_MODE_E;


typedef struct hiISP_AE_ATTR_S {
    /* base parameter */
    ISP_AE_RANGE_S stExpTimeRange;   /* RW; Range:[0x0, 0xFFFFFFFF]; Format:32.0; sensor exposure time (unit: us ), it's related to the specific sensor */
    ISP_AE_RANGE_S stAGainRange;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10; sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    ISP_AE_RANGE_S stDGainRange;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;  sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    ISP_AE_RANGE_S stISPDGainRange;  /* RW; Range:[0x400, 0x40000]; Format:22.10;  ISP digital gain (unit: times, 10bit precision), it's related to the ISP digital gain range */
    ISP_AE_RANGE_S stSysGainRange;   /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;  system gain (unit: times, 10bit precision), it's related to the specific sensor and ISP Dgain range */
    HI_U32 u32GainThreshold;         /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;  Gain threshold for slow shutter mode (unit: times, 10bit precision) */

    HI_U8  u8Speed;                  /* RW; Range:[0x0, 0xFF]; Format:8.0; AE adjust step for dark scene to bright scene switch */
    HI_U16  u16BlackSpeedBias;       /* RW; Range:[0x0, 0xFFFF]; Format:16.0; AE adjust step bias for bright scene to dark scene switch */
    HI_U8  u8Tolerance;              /* RW; Range:[0x0, 0xFF]; Format:8.0; AE adjust tolerance */
    HI_U8  u8Compensation;           /* RW; Range:[0x0, 0xFF]; Format:8.0; AE compensation */
    HI_U16  u16EVBias;               /* RW; Range:[0x0, 0xFFFF]; Format:16.0; AE EV bias */
    ISP_AE_STRATEGY_E enAEStrategyMode;  /* RW; Range:[0, 1]; Format:1.0; Support Highlight prior or Lowlight prior */
    HI_U16  u16HistRatioSlope;       /* RW; Range:[0x0, 0xFFFF]; Format:16.0; AE hist ratio slope */
    HI_U8   u8MaxHistOffset;         /* RW; Range:[0x0, 0xFF]; Format:8.0; Max hist offset */

    ISP_AE_MODE_E  enAEMode;         /* RW; Range:[0, 1]; Format:1.0; AE mode(slow shutter/fix framerate)(onvif) */
    ISP_ANTIFLICKER_S stAntiflicker;
    ISP_SUBFLICKER_S stSubflicker;
    ISP_AE_DELAY_S stAEDelayAttr;

    HI_BOOL bManualExpValue;         /* RW; Range:[0, 1]; Format:1.0; manual exposure value or not */
    HI_U32 u32ExpValue;              /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0; manual exposure value */

    ISP_FSWDR_MODE_E enFSWDRMode;    /* RW; Range:[0, 2]; Format:2.0; FSWDR running mode */
    HI_BOOL bWDRQuick;               /* RW; Range:[0, 1]; Format:1.0; WDR use delay strategy or not; If is true, WDR AE adjust will be faster */

    HI_U16  u16ISOCalCoef;     /* RW; Range:[0x0, 0xFFFF]; Format:8.8; The coefficient between Standard ISO and origin ISO (unit: times, 8bit precision) */
} ISP_AE_ATTR_S;

typedef struct hiISP_ME_ATTR_S {
    ISP_OP_TYPE_E enExpTimeOpType;
    ISP_OP_TYPE_E enAGainOpType;
    ISP_OP_TYPE_E enDGainOpType;
    ISP_OP_TYPE_E enISPDGainOpType;

    HI_U32 u32ExpTime;        /* RW; Range:[0x0, 0xFFFFFFFF]; Format:32.0; sensor exposure time (unit: us ), it's related to the specific sensor */
    HI_U32 u32AGain;          /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10; sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    HI_U32 u32DGain;          /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10; sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    HI_U32 u32ISPDGain;       /* RW; Range:[0x400, 0x40000]; Format:22.10; ISP digital gain (unit: times, 10bit precision), it's related to the ISP digital gain range */
} ISP_ME_ATTR_S;

typedef struct hiISP_EXPOSURE_ATTR_S {
    HI_BOOL         bByPass;    /* RW; Range:[0, 1]; Format:1.0; */
    ISP_OP_TYPE_E   enOpType;
    HI_U8     u8AERunInterval;  /* RW; Range:[0x1, 0xFF]; Format:8.0; set the AE run interval */
    HI_BOOL   bHistStatAdjust;  /* RW; Range:[0, 1]; Format:1.0;  HI_TRUE: 256 bin histogram statistic config will adjust when large red or blue area detected.
                                                        HI_FALSE: 256 bin histogram statistic config will not change */
    HI_BOOL   bAERouteExValid;  /* RW; Range:[0, 1]; Format:1.0; use extend AE route or not */
    ISP_ME_ATTR_S   stManual;
    ISP_AE_ATTR_S   stAuto;
    ISP_PRIOR_FRAME_E enPriorFrame;  /* RW; Range:[0, 3]; Format:1.0; AE prior frame */
    HI_BOOL   bAEGainSepCfg;  /* RW; Range:[0, 1]; Format:1.0; long and short frame gain separately configure or not */
} ISP_EXPOSURE_ATTR_S;


#define ISP_AE_ROUTE_MAX_NODES                    (16)
typedef struct hiISP_AE_ROUTE_NODE_S {
    HI_U32  u32IntTime;          /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0; sensor exposure time (unit: us ), it's related to the specific sensor */
    HI_U32  u32SysGain;          /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10; system gain (unit: times, 10bit precision), it's related to the specific sensor and ISP Dgain range */
    ISP_IRIS_F_NO_E  enIrisFNO;  /* RW; Range:[0, 10]; Format:4.0; the F number of the iris's aperture, only support for Piris */
    HI_U32  u32IrisFNOLin;       /* RW; Range:[0x1, 0x400]; Format:32.0; the equivalent gain of F number of the iris's aperture, only support for Piris */
} ISP_AE_ROUTE_NODE_S;

typedef struct hiISP_AE_ROUTE_S {
    HI_U32 u32TotalNum;     /* RW; Range:[0, 0x10]; Format:8.0; total node number of AE route */
    ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

#define ISP_AE_ROUTE_EX_MAX_NODES                 (16)
typedef struct hiISP_AE_ROUTE_EX_NODE_S {
    HI_U32  u32IntTime;     /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0;  sensor exposure time (unit: us ), it's related to the specific sensor */
    HI_U32  u32Again;       /* RW; Range:[0x400, 0x3FFFFF]; Format:22.10;  sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    HI_U32  u32Dgain;       /* RW; Range:[0x400, 0x3FFFFF]; Format:22.10;  sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    HI_U32  u32IspDgain;    /* RW; Range:[0x400, 0x40000]; Format:22.10;  ISP digital gain (unit: times, 10bit precision) */
    ISP_IRIS_F_NO_E  enIrisFNO;  /* RW; Range:[0, 10]; Format:4.0; the F number of the iris's aperture, only support for Piris */
    HI_U32  u32IrisFNOLin;  /* RW; Range:[0x1, 0x400]; Format:32.0; the equivalent gain of F number of the iris's aperture, only support for Piris */
} ISP_AE_ROUTE_EX_NODE_S;

typedef struct hiISP_AE_ROUTE_EX_S {
    HI_U32 u32TotalNum;       /* RW; Range:[0, 0x10]; Format:8.0; total node number of extend AE route */
    ISP_AE_ROUTE_EX_NODE_S astRouteExNode[ISP_AE_ROUTE_EX_MAX_NODES];
} ISP_AE_ROUTE_EX_S;

typedef struct hiISP_EXP_INFO_S {
    HI_U32 u32ExpTime;                 /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    HI_U32 u32ShortExpTime;            /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    HI_U32 u32MedianExpTime;           /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    HI_U32 u32LongExpTime;             /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    HI_U32 u32AGain;                   /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    HI_U32 u32DGain;                   /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    HI_U32 u32AGainSF;                 /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    HI_U32 u32DGainSF;                 /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    HI_U32 u32ISPDGain;                /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    HI_U32 u32Exposure;                /* R; Range:[0x40, 0xFFFFFFFF]; Format:26.6; */
    HI_BOOL bExposureIsMAX;            /* R; Range:[0, 1]; Format:1.0; */
    HI_S16 s16HistError;               /* R; Range:[-32768, 32767]; Format:16.0; */
    HI_U32 au32AE_Hist1024Value[HIST_NUM]; /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; 1024 bins histogram for channel 1 */

    HI_U8  u8AveLum;                   /* R; Range:[0x0, 0xFF]; Format:8.0; */
    HI_U32 u32LinesPer500ms;           /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; exposure lines per 500ms */
    HI_U32 u32PirisFNO;                /* R; Range:[0x0, 0x400]; Format:32.0; */
    HI_U32 u32Fps;                     /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; actual fps */
    HI_U32 u32ISO;                     /* R; Range:[0x64, 0xFFFFFFFF]; Format:32.0; */
    HI_U32 u32ISOSF;                   /* R; Range:[0x64, 0xFFFFFFFF]; Format:32.0; */
    HI_U32 u32ISOCalibrate;            /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    HI_U32 u32RefExpRatio;             /* R; Range:[0x40, 0x4000]; Format:26.6; */
    HI_U32 u32FirstStableTime;         /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; AE first stable time for quick start */
    ISP_AE_ROUTE_S stAERoute;          /* R; Actual AE route */
    ISP_AE_ROUTE_EX_S stAERouteEx;     /* R; Actual AE route_ex */
    ISP_AE_ROUTE_S stAERouteSF;          /* R; Actual AE route_sf */
    ISP_AE_ROUTE_EX_S stAERouteSFEx;     /* R; Actual AE route_sf_ex */
} ISP_EXP_INFO_S;


#define EXP_RATIO_NUM                             (3)
typedef struct hiISP_WDR_EXPOSURE_ATTR_S {
    ISP_OP_TYPE_E enExpRatioType;   /* RW; Range:[0, 1]; Format:1.0; OP_TYPE_AUTO: The ExpRatio used in ISP is generated by firmware; OP_TYPE_MANUAL: The ExpRatio used in ISP is set by u32ExpRatio */
    HI_U32 au32ExpRatio[EXP_RATIO_NUM];             /* RW; Range:[0x40, 0xFFF]; Format:26.6; 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatio is invalid.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatio is quotient of long exposure time / short exposure time. */
    HI_U32 u32ExpRatioMax;          /* RW; Range:[0x40, 0x4000]; Format:26.6; 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatioMax is max(upper limit) of ExpRatio generated by firmware.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatioMax is invalid. */
    HI_U32 u32ExpRatioMin;          /* RW; Range:[0x40, 0x4000]; Format:26.6; limited Range:[0x40, u32ExpRatioMax],0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatioMin is min(lower limit) of ExpRatio generated by firmware.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatioMin is invalid. */
    HI_U16 u16Tolerance;            /* RW; Range:[0x0, 0xFF]; Format:8.0;et the dynamic range tolerance. Format: unsigned 6.2-bit fixed-point. 0x4 means 1dB. */
    HI_U16 u16Speed;                /* RW; Range:[0x0, 0xFF]; Format:8.0; exposure ratio adjust speed */
    HI_U16 u16RatioBias;            /* RW; Range:[0x0, 0xFFFF]; Format:16.0; exposure ratio bias */

} ISP_WDR_EXPOSURE_ATTR_S;

typedef struct hiISP_HDR_EXPOSURE_ATTR_S {
    ISP_OP_TYPE_E enExpHDRLvType;   /* RW; Range:[0, 1]; Format:1.0; OP_TYPE_AUTO: The ExpHDRLv used in ISP is generated by firmware; OP_TYPE_MANUAL: The ExpHDRLv used in ISP is set by u32ExpHDRLv */
    HI_U32 u32ExpHDRLv;             /* RW; Range:[0x40, 0x400]; Format:26.6; 0x40 means 1 times.
                                            When enExpHDRLvType is OP_TYPE_AUTO, u32ExpHDRLv is invalid.
                                            When enExpHDRLvType is OP_TYPE_MANUAL, u32ExpHDRLv is the quotient of exposure / linear exposure. */
    HI_U32 u32ExpHDRLvMax;          /* RW; Range:[0x40, 0x400]; Format:26.6; 0x40 means 1 times.
                                            When enExpHDRLvType is OP_TYPE_AUTO, u32ExpHDRLvMax is max(upper limit) of ExpHDRLv generated by firmware.
                                            When enExpHDRLvType is OP_TYPE_MANUAL, u32ExpHDRLvMax is invalid. */
    HI_U32 u32ExpHDRLvMin;          /* RW; Range:[0x40, 0x400]; Format:26.6; limited range:[0x40, u32ExpHDRLvMax],0x40 means 1 times.
                                            When enExpHDRLvType is OP_TYPE_AUTO, u32ExpHDRLvMin is min(lower limit) of ExpHDRLv generated by firmware.
                                            When enExpHDRLvType is OP_TYPE_MANUAL, u32ExpHDRLvMin is invalid. */
    HI_U32 u32ExpHDRLvWeight;       /* RW; Range:[0x0, 0x400]; Format:16.0; exposure HDR level weight. */

} ISP_HDR_EXPOSURE_ATTR_S;

typedef struct hiISP_SMART_EXPOSURE_ATTR_S {
    HI_BOOL bEnable;                /* RW; Range:[0, 1]; Format:1.0; smart ae enable or not */
    HI_BOOL bIRMode;                /* RW; Range:[0, 1]; Format:1.0; smart ae IR mode or not */
    ISP_OP_TYPE_E enSmartExpType;   /* RW; Range:[0, 1]; Format:1.0; OP_TYPE_AUTO: The ExpCoef used in ISP is generated by firmware; OP_TYPE_MANUAL: The ExpCoef used in ISP is set by u32ExpCoef */
    HI_U16  u16ExpCoef;             /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                            When enExpHDRLvType is OP_TYPE_AUTO, u32ExpCoef is invalid.
                                            When enExpHDRLvType is OP_TYPE_MANUAL, u32ExpCoef is the quotient of exposure. */
    HI_U8   u8LumaTarget;           /* RW; Range:[0x0, 0xFF]; Format:8.0; luma target of smart ae. */
    HI_U16  u16ExpCoefMax;          /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                            When enExpHDRLvType is OP_TYPE_AUTO, u32ExpCoefvMax is max(upper limit) of ExpCoef generated by firmware.
                                            When enExpHDRLvType is OP_TYPE_MANUAL, u32ExpCoefMax is invalid. */
    HI_U16  u16ExpCoefMin;          /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                            When enExpHDRLvType is OP_TYPE_AUTO, u32ExpCoefMax is min(lower limit) of ExpCoef generated by firmware.
                                            When enExpHDRLvType is OP_TYPE_MANUAL, u32ExpCoefMin is invalid. */
    HI_U8   u8SmartInterval;        /*RW; Range:[0x1, 0xFF]; Format:8.0; smart ae run interval*/
    HI_U8   u8SmartSpeed;           /*RW; Range:[0x0, 0xFF]; Format:8.0; smart ae adjust step*/
    HI_U16  u16SmartDelayNum;       /*RW; Range:[0x0, 0x400]; Format:16.0; smart ae adjust delay frame num */
} ISP_SMART_EXPOSURE_ATTR_S;


/********************* AWB structs ************************/
/*
Defines the AWB online calibration type
u16AvgRgain: the avg value of Rgain after online calibration
u16AvgBgain: the avg value of Bgain after online calibration
*/
typedef struct hiISP_AWB_Calibration_Gain_S {
    HI_U16 u16AvgRgain;
    HI_U16 u16AvgBgain;
} ISP_AWB_Calibration_Gain_S;
/*
Defines the AWB algorithm type
0 = Improved gray world algorithm.
1 = AWB algorithm that classifies the statistics and re-filters the white blocks
*/
typedef enum hiISP_AWB_ALG_TYPE_E {
    AWB_ALG_LOWCOST = 0,
    AWB_ALG_ADVANCE = 1,
    AWB_ALG_BUTT
} ISP_AWB_ALG_TYPE_E;

/*
Defines the AWB policy in the multi-illuminant scenario
0 = Automatic saturation adjustment in the multi-illuminant scenario
1 = Automatic ccm adjustment in the multi-illuminant scenario
*/
typedef enum hiISP_AWB_MULTI_LS_TYPE_E {
    AWB_MULTI_LS_SAT = 0,
    AWB_MULTI_LS_CCM = 1,
    AWB_MULTI_LS_BUTT
} ISP_AWB_MULTI_LS_TYPE_E;

typedef enum hiISP_AWB_INDOOR_OUTDOOR_STATUS_E {
    AWB_INDOOR_MODE = 0,
    AWB_OUTDOOR_MODE = 1,
    AWB_INDOOR_OUTDOOR_BUTT
} ISP_AWB_INDOOR_OUTDOOR_STATUS_E;

/* Defines the AWB gain range */
typedef struct hiISP_AWB_CT_LIMIT_ATTR_S {
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;

    HI_U16 u16HighRgLimit;     /* RW; Range:[0x0, 0xFFF]; Format:4.8; in Manual Mode, user define the Max Rgain of High Color Temperature */
    HI_U16 u16HighBgLimit;     /* RW; Range:[0x0, 0xFFF]; Format:4.8; in Manual Mode, user define the Min Bgain of High Color Temperature */
    HI_U16 u16LowRgLimit;      /* RW; Range:[0x0, 0xFFF]; Format:4.8; limited range:[0x0, u16HighRgLimit),in Manual Mode, user define the Min Rgain of Low Color Temperature */
    HI_U16 u16LowBgLimit;      /* RW; Range:[0, 0xFFF]; Format:4.8; limited Range:(u16HighBgLimit, 0xFFF],in Manual Mode, user define the Max Bgain of Low Color Temperature */
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct hiISP_AWB_IN_OUT_ATTR_S {
    HI_BOOL bEnable;                                    /* RW; Range:[0, 1]; Format:1.0; Outdoor/Indoor scenario determination enable */
    ISP_OP_TYPE_E   enOpType;
    ISP_AWB_INDOOR_OUTDOOR_STATUS_E  enOutdoorStatus;   /* RW;User should config indoor or outdoor status in Manual Mode */
    HI_U32 u32OutThresh;                                /* RW;Format:32.0;shutter time(in us) to judge indoor or outdoor */
    HI_U16 u16LowStart;                                 /* RW;Format:16.0;5000K is recommend */
    HI_U16 u16LowStop;                                  /* RW;Format:16.0;limited range:(0,u16LowStart),4500K is recommend, should be smaller than u8LowStart */
    HI_U16 u16HighStart;                                /* RW;Format:16.0;limited range:(u16LowStart,0xFFFF],6500K is recommend, shoule be larger than u8LowStart */
    HI_U16 u16HighStop;                                 /* RW;Format:16.0;limited range:(u16HighStart,0xFFFF],8000K is recommend, should be larger than u8HighStart */
    HI_BOOL bGreenEnhanceEn;                            /* RW; Range:[0, 1]; Format:1.0;If this is enabled, Green channel will be enhanced based on the ratio of green plant */
    HI_U8   u8OutShiftLimit;                            /* RW; Range:[0, 0xFF]; Format:8.0;Max white point zone distance to Planckian Curve */
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct hiISP_AWB_CBCR_TRACK_ATTR_S {
    HI_BOOL bEnable;                                /* RW; Range:[0, 1]; Format:1.0;If enabled, statistic parameter cr, cb will change according to iso */

    HI_U16  au16CrMax[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range:[0x0, 0xFFF]; au16CrMax[i] >= au16CrMin[i] */
    HI_U16  au16CrMin[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range:[0x0, 0xFFF] */
    HI_U16  au16CbMax[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range:[0x0, 0xFFF]; au16CbMax[i] >= au16CbMin[i] */
    HI_U16  au16CbMin[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range:[0x0, 0xFFF] */
} ISP_AWB_CBCR_TRACK_ATTR_S;

#define AWB_LUM_HIST_NUM                          (6)
/* Defines the parameters of the luminance histogram statistics for white balance */
typedef struct hiISP_AWB_LUM_HISTGRAM_ATTR_S {
    HI_BOOL bEnable;                          /* RW; Range:[0, 1]; Format:1.0; If enabled, zone weight to awb is combined with zone luma */
    ISP_OP_TYPE_E   enOpType;                 /* In auto mode, the weight distribution follows Gaussian distribution */
    HI_U8   au8HistThresh[AWB_LUM_HIST_NUM];  /* RW; Range:[0x0, 0xFF]; Format:8.0; In manual mode, user define luma thresh, thresh[0] = 0, thresh[5] = 0xFF,  thresh[i] >=  thresh[i-1] */
    HI_U16  au16HistWt[AWB_LUM_HIST_NUM];     /* RW; Range:[0x0, 0xFFFF]; Format:16.0; user can define luma weight in both manual and auto mode. */
} ISP_AWB_LUM_HISTGRAM_ATTR_S;

/* Defines the information about a separate illuminant */
typedef struct hiISP_AWB_LIGHTSOURCE_INFO_S {
    HI_U16 u16WhiteRgain;         /* RW;Range:[0x0, 0xFFF]; Format:4.8; G/R of White points at this light source */
    HI_U16 u16WhiteBgain;         /* RW;Range:[0x0, 0xFFF]; Format:4.8; G/B of White points at this light source */
    HI_U16 u16ExpQuant;           /* RW;shutter time * again * dgain >> 4, Not support Now */
    HI_U8  u8LightStatus;         /* RW;Range:[0,2]; Format:2.0; idle  1:add light source  2:delete sensitive color */
    HI_U8  u8Radius;              /* RW;Range:[0x0, 0xFF];  Format:8.0; Radius of light source, */
} ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S;

#define AWB_LS_NUM                                (4)
#define AWB_MULTI_CT_NUM                          (8)
/* extended AWB attributes */
typedef struct hiISP_AWB_ATTR_EX_S {
    HI_U8  u8Tolerance;                                 /* RW; Range:[0x0, 0xFF]; Format:8.0; AWB adjust tolerance */
    HI_U8  u8ZoneRadius;                                /* RW; Range:[0x0, 0xFF]; Format:8.0; radius of AWB blocks */
    HI_U16 u16CurveLLimit;                              /* RW; Range:[0x0, 0x100]; Format:9.0; Left limit of AWB Curve, recomend for indoor 0xE0, outdoor 0xE0 */
    HI_U16 u16CurveRLimit;                              /* RW; Range:[0x100, 0xFFF]; Format:12.0; Right Limit of AWB Curve,recomend for indoor 0x130, outdoor 0x120 */

    HI_BOOL  bExtraLightEn;                             /* RW; Range:[0, 1]; Format:1.0; Enable special light source function */
    ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S    stLightInfo[AWB_LS_NUM];
    ISP_AWB_IN_OUT_ATTR_S               stInOrOut;

    HI_BOOL bMultiLightSourceEn;                        /* RW; Range:[0, 1]; Format:1.0; If enabled, awb will do special process in multi light source enviroment */
    ISP_AWB_MULTI_LS_TYPE_E enMultiLSType;              /* Saturation or CCM Tunning */
    HI_U16  u16MultiLSScaler;                           /* RW; Range:[0x0, 0x100];  Format:12.0; In saturation type, it means the max saturation it can achieve, in ccm type, it means the strenght of multi process. */
    HI_U16  au16MultiCTBin[AWB_MULTI_CT_NUM];           /* RW; Range:[0, 0XFFFF]; Format:16.0;AWB Support divide the color temperature range by 8 bins */
    HI_U16  au16MultiCTWt[AWB_MULTI_CT_NUM];            /* RW; Range:[0x0, 0x400];Weight for different color temperature, same value of 8 means CT weight does't work, */

    HI_BOOL bFineTunEn;                                 /* RW; Range:[0x0, 0x1]; Format:1.0;If enabled, skin color scene will be optimized */
    HI_U8   u8FineTunStrength;                          /* RW; Range:[0x0, 0xFF]; Format:8.0;larger value means better performance of skin color scene, but will increase error probability in low color temperature scene */
} ISP_AWB_ATTR_EX_S;

#define AWB_CURVE_PARA_NUM                        (6)
typedef struct hiISP_AWB_ATTR_S {
    HI_BOOL bEnable;               /* RW; Range:[0x0, 0x1]; Format:1.0;If AWB is disabled, static wb gain will be used, otherwise auto wb gain will be used */

    HI_U16 u16RefColorTemp;        /* RW; Range:[0x0, 0xFFFF]; Format:16.0; Calibration Information */
    HI_U16 au16StaticWB[ISP_BAYER_CHN_NUM];        /* RW; Range:[0x0, 0xFFF]; Format:12.0;Calibration Information */
    HI_S32 as32CurvePara[AWB_CURVE_PARA_NUM];       /* RW; Format:32.0;Calibration Information,limited Range:as32CurvePara[3] != 0, as32CurvePara[4]==128 */

    ISP_AWB_ALG_TYPE_E          enAlgType;

    HI_U8  u8RGStrength;                            /* RW; Range: [0x0, 0xFF]; Format:8.0; AWB Strength of R Channel */
    HI_U8  u8BGStrength;                            /* RW; Range: [0x0, 0xFF]; Format:8.0; AWB Strength of B Channel */
    HI_U16 u16Speed;                                /* RW; Range: [0x0, 0xFFF]; Format:12.0; Convergence speed of AWB  */
    HI_U16 u16ZoneSel;                              /* RW; Range: [0, 255]; Format:8.0; A value of 0 or 0xFF means global AWB, A value between 0 and 0xFF means zoned AWB */
    HI_U16 u16HighColorTemp;                        /* RW; Range: [0, 65535]; Format:16.0; AWB max temperature, Recommended: [8500, 10000] */
    HI_U16 u16LowColorTemp;                         /* RW; Range: [0, 65535]; Format:16.0; Limited Range:[0, u16HighColorTemp), AWB min temperature, Recommended: [2000, 2500] */
    ISP_AWB_CT_LIMIT_ATTR_S   stCTLimit;
    HI_BOOL bShiftLimitEn;                          /* RW; Range: [0, 1]; Format:1.0; If enabled, when the statistic information is out of range, it should be project back */
    HI_U8  u8ShiftLimit;                            /* RW; Range: [0x0, 0xFF];  Format:8.0; planckian curve range, Recommended: [0x30, 0x50] */
    HI_BOOL bGainNormEn;                            /* RW; Range: [0, 1]; Format:1.0; if enabled, the min of RGB gain is fixed. */
    HI_BOOL bNaturalCastEn;                         /* RW, Range: [0, 1]; Format:1.0; if enabled, the color performance will be natural in lowlight and low color temperature */

    ISP_AWB_CBCR_TRACK_ATTR_S stCbCrTrack;
    ISP_AWB_LUM_HISTGRAM_ATTR_S stLumaHist;
    HI_BOOL bAWBZoneWtEn;                           /* RW, Range: [0, 1]; Format:1.0; if enabled, user can set weight for each zones */
    HI_U8   au8ZoneWt[AWB_ZONE_ORIG_ROW *AWB_ZONE_ORIG_COLUMN];   /* RW; Range: [0, 255]; Format:8.0;Zone Wt Table */
} ISP_AWB_ATTR_S;

/* Attr for SPC AWB */
#define SPECAWB_FACTTBL_ELMNUM                    (7)          /* Table number. */
#define SPECAWB_FACTTBL_SIZE                      (64)         /* Table size for weight for light source detection. */
#define SPECAWB_BBL_SIZE                          (40)         /* Table size for BlackBodyTable. */

typedef struct hiISP_SPECAWB_BBL_TBL_S {
    HI_S16 s16Kelvin;   /* RW;Range:[-32768, 32767];Kelvin */
    HI_S16 s16Wbr;      /* RW;Range:[-32768, 32767];WBR value */
    HI_S16 s16Wbb;      /* RW;Range:[-32768, 32767];WBB value */
} ISP_SPECAWB_BBL_TBL_S;

typedef struct hiISP_SPECAWB_KELVIN_DBB_MAP_S {
    HI_S16 s16Kelvin; /* RW;Range:[-32768, 32767];kevin */
    HI_S16 s16DBB;   /* RW;Range:[-32768, 32767];DeltaBlackBody */
} ISP_SPECAWB_KELVIN_DBB_MAP_S;

typedef struct  hiISP_SPECKCWB_S {
    HI_S32    s32RGain; /* RW;Range:[-2147483648, 2147483647]; RgainValue */
    HI_S32    s32BGain; /* RW;Range:[-2147483648, 2147483647]; BgainValue */
} ISP_SPECKCWB_S;

typedef struct  hiISP_SPECKCWBS16_S {
    HI_S16    s16RGain; /* RW;Range:[-32768, 32767]; RgainValue(16bit) */
    HI_S16    s16BGain; /* RW;Range:[-32768, 32767]; BgainValue(16bit) */
} ISP_SPECKCWBS16_S;

typedef struct  hiISP_SPECAWB_FACTTBL_ELEMENT_S {
    HI_S16    s16Bv;                             /* RW;Range:[-32768, 32767]; The center value of the Bv value targeted by this table. */
    HI_U8     u8FactTbl[SPECAWB_FACTTBL_SIZE][SPECAWB_FACTTBL_SIZE];     /* RW;Range:[0, 0xFF]; Weight for light source detection. */
} ISP_SPECAWB_FACTTBL_ELEMENT_S;

typedef struct  hiISP_SPECAWB_ATTR_S {
    ISP_SPECKCWB_S                       stWBCenter;                 /* RW;WB gain value at the center of the table: The table index is calculated as a relative value from this gain. */
    ISP_SPECKCWB_S                       stWBMin;                    /* RW;Minimum value of WB gain that the table covers. */
    ISP_SPECKCWB_S                       stWBMax;                    /* RW;Maximum value of WB gain that the table covers. */
    ISP_SPECKCWB_S                       stLogFact;                  /* RW;Range covered by the table: 1/exp(stLogFact) to exp(stLogFact). For example, if it is 1.0, WBDelta will be 1/e to e. However, e = 2.71828 ... (base of natural logarithm) */
    HI_S32                               s32LimitFactMin;            /* RW;Range:[0, s32LimitFactMax];AWB statics threshold value : minimum threshold value of ratio to pixel maximum value. */
    HI_S32                               s32LimitFactMax;            /* RW;Range:[s32LimitFactMin, 0xFFFF];AWB statics threshold value : maximum threshold value of ratio to pixel maximum value. */
    HI_S32                               s32LimitFactLimit;          /* RW;Range:[0x600, 0x4000];AWB statics limit value : ratio to (pixel maximum value * s32LimitFactMax) */
    HI_S32                               s32FactOffset;              /* RW;Range:[0, 0x800];The offset value of the weight when calculating the AWB statics. */
    HI_S32                               s32BFact;                   /* RW;Range:[0x399, 0x500];The value of specular weight when calculating the AWB statics. */
    HI_S32                               s32ACntMin;                 /* RW;Range:[0, 0x20];Threshold for block adoption.  If the integrated value of the AWB statics is less than m_ACntMin, the block is ignored. */
    ISP_SPECKCWB_S                       stWBNeutral;                /* RW;WB gain returned when AWB calculation can not be performed. */
    HI_U8                                bLastIsFlash;               /* RW;Range:[1]: The last table is used for flash. 0: Used for unknown Bv value. */
    HI_S16                               s16BvFlashELimit;           /* RW;Range:[-32768, 32767];Threshold of Bv value before strobe lighting when using table for strobe. If bLastIsFlash == TRUE and the Bv value before strobe flash is less than this value, use the table for strobe. */
    HI_S32                               s32APercent;                /* RW;Range:[0, 100];A value percent of the WB statics */
    ISP_SPECKCWBS16_S                    stWBCnvTbl[SPECAWB_FACTTBL_SIZE][SPECAWB_FACTTBL_SIZE];     /* RW; WB gain conversion table (incomplete chromatic adaptation table). */
    HI_U8                                u8ElementNum;                                               /* RW; Range:[7];Number of connected ISP_SPECAWB_FACTTBL_ELEMENT. */
    ISP_SPECAWB_FACTTBL_ELEMENT_S        stFactElement[SPECAWB_FACTTBL_ELMNUM];                      /* RW; Weight Table */
    ISP_SPECAWB_KELVIN_DBB_MAP_S         stKelvinDBBTbl[SPECAWB_FACTTBL_SIZE][SPECAWB_FACTTBL_SIZE]; /* RW;KelvinDbb table. */
    ISP_SPECAWB_BBL_TBL_S                stBlackBodyTbl[SPECAWB_BBL_SIZE];                           /* RW;BlackBody table. */
    HI_U16                               u16Fno;                                                     /* RW; Range:[10, 100];F number of the len,F1.4=14,F2.8=28,F36 =360... */
} ISP_SPECAWB_ATTR_S;

typedef struct  hiISP_SPECAWB_CONTROL_ATTR_S {
    HI_S16 s16BlendHighBvThresh;          /* RW; Range: [-32768, 32767]; High Bv threshold for inner blending function */
    HI_U16 u16BlendHighBvWt;              /* RW; Range: [0, 2048]; High Bv  weight for inner blending function */
    HI_S16 s16BlendLowBvThresh;           /* RW; Range: [-32768, s16BlendHighBvThresh); Low Bv threshold for inner blending function */
    HI_U16 u16BlendLowBvWt;               /* RW; Range: [0, 2048]; Low Bv  weight for inner blending function */
} ISP_SPECAWB_CONTROL_ATTR_S;

#define SPECAWB_MAX_CAA_NUM                       3
#define SPECAWB_KEVIN_CONVER_MAX_NUM              8

typedef struct  hiISP_SPECAWB_CAA_CONVERSION_S {
    HI_S32 s32SrcKelvin;    /* RW;Range:[-2147483648, 2147483647]; Source kelvin */
    HI_S32 s32DstKelvin;    /* RW;Range:[-2147483648, 2147483647]; Destination kelvin */
} ISP_SPECAWB_CAA_CONVERSION_S;
typedef struct  hiISP_SPECAWB_CAA_TBL_S {
    HI_U8   u8Enable;       /* RW;Range:[0,1]; lut enable */
    HI_S16  s16Bv;          /* RW;Range:[-32768, 32767]; Bv value */
    ISP_SPECAWB_CAA_CONVERSION_S stKelvinCon[SPECAWB_KEVIN_CONVER_MAX_NUM];
    ISP_SPECKCWBS16_S stWBCnvTbl[SPECAWB_FACTTBL_SIZE][SPECAWB_FACTTBL_SIZE];
} ISP_SPECAWB_CAA_TBL_S;

typedef struct  hiISP_SPECAWB_CAA_CONTROl_S {
    ISP_SPECAWB_CAA_TBL_S stControl[SPECAWB_MAX_CAA_NUM];
} ISP_SPECAWB_CAA_CONTROl_S;
typedef struct hiISP_MWB_ATTR_S {
    HI_U16 u16Rgain;            /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for R  color channel  */
    HI_U16 u16Grgain;           /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for Gr color channel */
    HI_U16 u16Gbgain;           /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for Gb color channel */
    HI_U16 u16Bgain;            /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for B  color channel */
} ISP_MWB_ATTR_S;

typedef enum hiISP_AWB_ALG_E {
    ALG_AWB         = 0,
    ALG_AWB_SPEC    = 1,
    ALG_BUTT
} ISP_AWB_ALG_E;
typedef struct hiISP_WB_ATTR_S {
    HI_BOOL bByPass;              /* RW; Range: [0, 1];  Format:1.0; If enabled, awb will be bypassed */
    HI_U8   u8AWBRunInterval;         /* RW; Range: [0x1, 0xFF];  Format:8.0; set the AWB run interval */
    ISP_OP_TYPE_E   enOpType;
    ISP_MWB_ATTR_S  stManual;
    ISP_AWB_ATTR_S  stAuto;
    ISP_AWB_ALG_E enAlgType;
} ISP_WB_ATTR_S;

typedef struct hiISP_COLORMATRIX_MANUAL_S {
    HI_BOOL bSatEn;                           /* RW; Range: [0, 1]; Format:1.0; If bSatEn=1, the active CCM = SatMatrix * ManualMatrix, else tha active CCM =  ManualMatrix */
    HI_U16 au16CCM[CCM_MATRIX_SIZE];          /* RW; Range: [0x0, 0xFFFF]; Format:8.8; Manul CCM matrix, */
} ISP_COLORMATRIX_MANUAL_S;

typedef struct hiISP_COLORMATRIX_PARAM_S {
    HI_U16 u16ColorTemp;                   /* RW; Range: [500, 30000]; Format:16.0; the current color temperature */
    HI_U16 au16CCM[CCM_MATRIX_SIZE];       /* RW; Range: [0x0, 0xFFFF]; Format:8.8; CCM matrixes for different color temperature */
} ISP_COLORMATRIX_PARAM_S;

typedef struct hiISP_COLORMATRIX_AUTO_S {
    HI_BOOL bISOActEn;                     /* RW; Range: [0, 1]; Format:1.0; if enabled, CCM will bypass in low light */
    HI_BOOL bTempActEn;                    /* RW; Range: [0, 1]; Format:1.0; if enabled, CCM will bypass when color temperature is larger than 8000K or less than 2500K */
    HI_U16  u16CCMTabNum;                  /* RW; Range: [0x3, 0x7]; Format:16.0; The number of CCM matrixes */
    ISP_COLORMATRIX_PARAM_S astCCMTab[CCM_MATRIX_NUM];
} ISP_COLORMATRIX_AUTO_S;

typedef struct hiISP_COLORMATRIX_ATTR_S {
    ISP_OP_TYPE_E enOpType;
    ISP_COLORMATRIX_MANUAL_S stManual;
    ISP_COLORMATRIX_AUTO_S stAuto;
} ISP_COLORMATRIX_ATTR_S;

typedef struct hiISP_SATURATION_MANUAL_S {
    HI_U8   u8Saturation;        /* RW; Range: [0, 0xFF]; Format:8.0; set the manual saturation of CCM */
} ISP_SATURATION_MANUAL_S;

typedef struct hiISP_SATURATION_AUTO_S {
    HI_U8   au8Sat[ISP_AUTO_ISO_STRENGTH_NUM];           /* RW; Range: [0, 0xFF]; should be decreased with increasing ISO */
} ISP_SATURATION_AUTO_S;

typedef struct hiISP_SATURATION_ATTR_S {
    ISP_OP_TYPE_E enOpType;
    ISP_SATURATION_MANUAL_S stManual;
    ISP_SATURATION_AUTO_S stAuto;
} ISP_SATURATION_ATTR_S;

typedef struct hiISP_COLOR_TONE_ATTR_S {
    HI_U16 u16RedCastGain;        /* RW; Range: [0x100, 0x180]; Format:4.8; R channel gain after CCM */
    HI_U16 u16GreenCastGain;      /* RW; Range: [0x100, 0x180]; Format:4.8; G channel gain after CCM */
    HI_U16 u16BlueCastGain;       /* RW; Range: [0x100, 0x180]; Format:4.8; B channel gain after CCM */
} ISP_COLOR_TONE_ATTR_S;

typedef struct hiISP_WB_INFO_S {
    HI_U16 u16Rgain;                /* R; Range: [0x0, 0xFFF]; Format:8.8;AWB result of R color channel */
    HI_U16 u16Grgain;               /* R; Range: [0x0, 0xFFF]; Format:8.8; AWB result of Gr color channel */
    HI_U16 u16Gbgain;               /* R; Range: [0x0, 0xFFF]; Format:8.8; AWB result of Gb color channel */
    HI_U16 u16Bgain;                /* R; Range: [0x0, 0xFFF]; Format:8.8; AWB result of B color channel */
    HI_U16 u16Saturation;           /* R; Range: [0x0, 0xFF];Format:8.0;Current saturation */
    HI_U16 u16ColorTemp;            /* R; Range: [0x0, 0xFFFF];Format:16.0;Detect color temperature, maybe out of color cemeprature range */
    HI_U16 au16CCM[CCM_MATRIX_SIZE];              /* R; Range: [0x0, 0xFFFF];Format:16.0;Current color correction matrix */

    HI_U16 u16LS0CT;                                 /* R; Range: [0x0, 0xFFFF];Format:16.0;color tempearture of primary light source */
    HI_U16 u16LS1CT;                                 /* R; Range: [0x0, 0xFFFF];Format:16.0;color tempearture of secondary light source */
    HI_U16 u16LS0Area;                               /* R; Range: [0x0, 0xFF];Format:8.0;area of primary light source */
    HI_U16 u16LS1Area;                               /* R; Range: [0x0, 0xFF];Format:8.0;area of secondary light source */
    HI_U8  u8MultiDegree;                            /* R; Range: [0x0, 0xFF];0 means uniform light source, larger value means multi light source */
    HI_U16 u16ActiveShift;                           /* R; Range;[0x0,0xFF] */
    HI_U32 u32FirstStableTime;                       /* R, Range: [0x0, 0xFFFFFFFF];Format:32.0;AWB first stable frame number */
    ISP_AWB_INDOOR_OUTDOOR_STATUS_E enInOutStatus;   /* R; indoor or outdoor status */
    HI_S16  s16Bv;                                   /* R; Range;[-32768, 32767]; Bv value */
} ISP_WB_INFO_S;



/********************* AF structs ************************/

typedef struct hiISP_AF_ATTR_S {
    HI_S32 s32DistanceMax; /* RW;the focuse range */
    HI_S32 s32DistanceMin;
    HI_U8 u8Weight[AF_ZONE_ROW][AF_ZONE_COLUMN]; /* RW;weighting table */
} ISP_AF_ATTR_S;
typedef struct hiISP_MF_ATTR_S {
    HI_S32 s32DefaultSpeed;     /* RW;1,default speed(unit:m/s).(onvif) */
} ISP_MF_ATTR_S;
typedef struct hiISP_FOCUS_ATTR_S {
    ISP_OP_TYPE_E  enOpType;
    ISP_MF_ATTR_S  stManual;
    ISP_AF_ATTR_S  stAuto;
} ISP_FOCUS_ATTR_S;

/*
DNG cfalayout type
1 = Rectangular (or square) layout
2 = Staggered layout A: even columns are offset down by 1/2 row
3 = Staggered layout B: even columns are offset up by 1/2 row
4 = Staggered layout C: even rows are offset right by 1/2 column
5 = Staggered layout D: even rows are offset left by 1/2 column
6 = Staggered layout E: even rows are offset up by 1/2 row, even columns are offset left by 1/2 column
7 = Staggered layout F: even rows are offset up by 1/2 row, even columns are offset right by 1/2 column
8 = Staggered layout G: even rows are offset down by 1/2 row, even columns are offset left by 1/2 column
9 = Staggered layout H: even rows are offset down by 1/2 row, even columns are offset right by 1/2 column
*/
typedef enum hiDNG_CFALAYOUT_TYPE_E {
    CFALAYOUT_TYPE_RECTANGULAR = 1,
    CFALAYOUT_TYPE_A,        /* a,b,c... not support */
    CFALAYOUT_TYPE_B,
    CFALAYOUT_TYPE_C,
    CFALAYOUT_TYPE_D,
    CFALAYOUT_TYPE_E,
    CFALAYOUT_TYPE_F,
    CFALAYOUT_TYPE_G,
    CFALAYOUT_TYPE_H,
    CFALAYOUT_TYPE_BUTT
} DNG_CFALAYOUT_TYPE_E;

typedef struct hiDNG_SRATIONAL_S {
    HI_S32 s32Numerator;   /* represents the numerator of a fraction, */
    HI_S32 s32Denominator; /* the denominator. */
} DNG_SRATIONAL_S;

typedef struct hiDNG_BLCREPEATDIM_S {
    HI_U16 u16BlcRepeatRows;
    HI_U16 u16BlcRepeatCols;
} DNG_BLCREPEATDIM_S;

typedef struct hiDNG_DEFAULTSCALE_S {
    DNG_RATIONAL_S stDefaultScaleH;
    DNG_RATIONAL_S stDefaultScaleV;
} DNG_DEFAULTSCALE_S;

typedef struct hiDNG_REPEATPATTERNDIM_S {
    HI_U16 u16RepeatPatternDimRows;
    HI_U16 u16RepeatPatternDimCols;
} DNG_REPEATPATTERNDIM_S;

/*
Defines the structure of dng raw format.
*/
typedef struct hiDNG_RAW_FORMAT_S {
    HI_U8 u8BitsPerSample;                        /* RO;Format:8.0; Indicate the bit numbers of raw data */
    HI_U8 au8CfaPlaneColor[CFACOLORPLANE];        /* RO;Format:8.0; Indicate the planer numbers of raw data; 0:red 1:green 2: blue */
    DNG_CFALAYOUT_TYPE_E enCfaLayout;             /* RO;Range:[1,9]; Describes the spatial layout of the CFA */
    DNG_BLCREPEATDIM_S stBlcRepeatDim;            /* Specifies repeat pattern size for the BlackLevel */
    HI_U32 u32WhiteLevel;                         /* RO;Format:32.0; Indicate the WhiteLevel of the raw data */
    DNG_DEFAULTSCALE_S stDefaultScale;            /* Specifies the default scale factors for each direction to convert the image to square pixels */
    DNG_REPEATPATTERNDIM_S stCfaRepeatPatternDim; /* Specifies the pixel number of repeat color planer in each direction */
    HI_U8 au8CfaPattern[ISP_BAYER_CHN];           /* RO;Format:8.0; Indicate the bayer start order; 0:red 1:green 2: blue */
} DNG_RAW_FORMAT_S;

/*
Defines the structure of dng image static infomation. read only
*/
typedef struct hiDNG_IMAGE_STATIC_INFO_S {
    DNG_RAW_FORMAT_S stDngRawFormat;
    DNG_SRATIONAL_S astColorMatrix1[CCM_MATRIX_SIZE];       /* defines a transformation matrix that converts XYZ values to reference camera native color space values, under the first calibration illuminant. */
    DNG_SRATIONAL_S astColorMatrix2[CCM_MATRIX_SIZE];       /* defines a transformation matrix that converts XYZ values to reference camera native color space values, under the second calibration illuminant. */
    DNG_SRATIONAL_S astCameraCalibration1[CCM_MATRIX_SIZE]; /* defines a calibration matrix that transforms reference camera native space values to individual camera native space values under the first calibration illuminant */
    DNG_SRATIONAL_S astCameraCalibration2[CCM_MATRIX_SIZE]; /* defines a calibration matrix that transforms reference camera native space values to individual camera native space values under the second calibration illuminant */
    DNG_SRATIONAL_S astForwadMatrix1[CCM_MATRIX_SIZE];      /* defines a matrix that maps white balanced camera colors to XYZ D50 colors */
    DNG_SRATIONAL_S astForwadMatrix2[CCM_MATRIX_SIZE];      /* defines a matrix that maps white balanced camera colors to XYZ D50 colors */

    HI_U8  u8CalibrationIlluminant1;                       /* RO;Format:8.0;Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                 fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                 C, '20' D55, '21' D65, '22' D75, '255' other */
    HI_U8  u8CalibrationIlluminant2;                       /* RO;Format:8.0;Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                 fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                 C, '20' D55, '21' D65, '22' D75, '255' other */
} DNG_IMAGE_STATIC_INFO_S;

/*
Defines the structure of DNG WB gain used for calculate DNG colormatrix.
*/
typedef struct hiISP_DNG_WBGAIN_S {
    HI_U16 u16Rgain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for R  color channel */
    HI_U16 u16Ggain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for G  color channel */
    HI_U16 u16Bgain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for B  color channel */
} ISP_DNG_WBGAIN_S;

/*
Defines the structure of DNG color parameters.
*/
typedef struct hiISP_DNG_COLORPARAM_S {
    ISP_DNG_WBGAIN_S stWbGain1; /* the calibration White balance gain of colorcheker in low colortemper light source */
    ISP_DNG_WBGAIN_S stWbGain2; /* the calibration White balance gain of colorcheker in high colortemper light source */
} ISP_DNG_COLORPARAM_S;


typedef enum hiISP_IR_STATUS_E {
    ISP_IR_STATUS_NORMAL = 0,
    ISP_IR_STATUS_IR     = 1,
    ISP_IR_BUTT
} ISP_IR_STATUS_E;

typedef enum hiISP_IR_SWITCH_STATUS_E {
    ISP_IR_SWITCH_NONE      = 0,
    ISP_IR_SWITCH_TO_NORMAL = 1,
    ISP_IR_SWITCH_TO_IR     = 2,
    ISP_IR_SWITCH_BUTT
} ISP_IR_SWITCH_STATUS_E;


typedef struct hiISP_IR_AUTO_ATTR_S {
    HI_BOOL bEnable;            /* RW, HI_TRUE: enable IR_auto function;  HI_TRUE: disable IR_auto function. */
    HI_U32  u32Normal2IrIsoThr; /* RW, Range: [0, 0xFFFFFFFF]. ISO threshold of switching from normal to IR mode. */
    HI_U32  u32Ir2NormalIsoThr; /* RW, Range: [0, 0xFFFFFFFF]. ISO threshold of switching from IR to normal mode. */
    HI_U32  u32RGMax;           /* RW, Range: [0x0, 0xFFF].    Maximum value of R/G in IR scene, 4.8-bit fix-point. */
    HI_U32  u32RGMin;           /* RW, Range: [0x0, u32RGMax]. Minimum value of R/G in IR scene, 4.8-bit fix-point. */
    HI_U32  u32BGMax;           /* RW, Range: [0x0, 0xFFF].    Maximum value of B/G in IR scene, 4.8-bit fix-point. */
    HI_U32  u32BGMin;           /* RW, Range: [0x0, u32BGMax]. Minimum value of B/G in IR scene, 4.8-bit fix-point. */

    ISP_IR_STATUS_E enIrStatus; /* RW. Current IR status. */

    ISP_IR_SWITCH_STATUS_E enIrSwitch; /* RO, IR switch status. */
} ISP_IR_AUTO_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_ISP_H__ */
