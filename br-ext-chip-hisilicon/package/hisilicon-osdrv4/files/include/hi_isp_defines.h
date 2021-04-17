
/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_isp_defines.h

  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/11/07
  Description   :
  History       :
  1.Date        : 2013/11/07
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __HI_ISP_DEFINES_H__
#define __HI_ISP_DEFINES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* isp sys part */
#define ISP_STRIPING_MAX_NUM                      (2)
#define ISP_NORMAL_BLOCK_NUM                      (1)
#define ISP_DEFAULT_BLOCK_NUM                     (2)
#define ISP_SBS_BLOCK_NUM                         (2)

#define ISP_BE0_PIPE_ID                           (0)
#define ISP_BE1_PIPE_ID                           (8)
#define ISP_MAX_BE_NUM                            (1)
#define ISP_MAX_STITCH_NUM                        (1)

#define ISP_MAX_PHY_PIPE_NUM                      (2)
#define ISP_MAX_VIR_PIPE_NUM                      (0)
#define ISP_MAX_PIPE_NUM                          (ISP_MAX_PHY_PIPE_NUM + ISP_MAX_VIR_PIPE_NUM)

#define ISP_STRIPING_OVERLAP_DEFAULT              (256)

#define ISP_WDR_CHN_MAX                           (2)
#define ISP_SUPPORT_DE_MODULE                     (1)

#define SENSOR_RES_WIDTH_MAX                      3072
#define SENSOR_RES_HEIGHT_MAX                     3072
#define ISP_RES_WIDTH_MAX                         3072,3072 // need change when pipe num is change
#define ISP_RES_HEIGHT_MAX                        3072,3072 // need change when pipe num is change

extern HI_U16 g_au16ResWMax[ISP_MAX_PIPE_NUM];
extern HI_U16 g_au16ResHMax[ISP_MAX_PIPE_NUM];

/* isp alg part */
#define HI_ISP_CLUT_LUT_LENGTH                    (5508)
#define HI_ISP_CA_YRATIO_LUT_LENGTH               (128)


#define STATIC_DP_COUNT_NORMAL                    (2048)
#define STATIC_DP_COUNT_MAX                       (STATIC_DP_COUNT_NORMAL * ISP_STRIPING_MAX_NUM)
#define ISP_SUPPORT_OFFLINE_DPC_CALIBRATION       (1)

#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT0          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT1          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT2          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT3          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT4          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT5          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT6          (4096)
#define HI_ISP_SPECAWB_WB_CNVTBL_LUT              (16384)
#define HI_ISP_SPECAWB_KELDBB_LUT                 (16384)
#define HI_ISP_SPECAWB_BBL_LUT                    (240)
#define HI_ISP_SPECAWB_CAA_LUT1                   (16384)
#define HI_ISP_SPECAWB_CAA_LUT2                   (16384)
#define HI_ISP_SPECAWB_CAA_LUT3                   (16384)
#define ISP_SPECAWB_BUF_SIZE                      (110832)

#define AWB_ZONE_BIN                              (1)
#define AWB_LIB_NUM                               (ISP_MAX_PHY_PIPE_NUM)
#define AE_LIB_NUM                                (ISP_MAX_PHY_PIPE_NUM)


#define HI_ISP_SHARPEN_RGAIN                      (31)
#define HI_ISP_SHARPEN_RGAIN_MAX                  (31)
#define HI_ISP_SHARPEN_RGAIN_BIT                  (31)
#define HI_ISP_SHARPEN_GGAIN                      (32)
#define HI_ISP_SHARPEN_GGAIN_MAX                  (255)
#define HI_ISP_SHARPEN_GGAIN_BIT                  (255)
#define HI_ISP_SHARPEN_BGAIN                      (31)
#define HI_ISP_SHARPEN_BGAIN_MAX                  (31)
#define HI_ISP_SHARPEN_BGAIN_BIT                  (31)
#define HI_ISP_SHARPEN_LUMAWGT                    (31)
#define HI_ISP_SHARPEN_LUMAWGT_MAX                (31)
#define HI_ISP_SHARPEN_LUMAWGT_BIT                (31)
#define HI_ISP_SHARPEN_EdgeFiltMaxCap             (18)
#define HI_ISP_SHARPEN_EdgeFiltMaxCap_MAX         (47)
#define HI_ISP_SHARPEN_EdgeFiltMaxCap_BIT         (63)

#define GAMMA_REG_NODE_NUM                        (257)
#define GAMMA_OUTSEG_NUM                          (8)

#define HI_ISP_DRC_STRENGTH_MAX                   (1023)
#define HI_ISP_DRC_SPA_FLT_COEF_MAX               (5)

#define HI_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MAX   (7)
#define HI_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MIN   (1)
#define HI_ISP_DEMOSAIC_NONDIR_MFDETALEHC_STR_MAX (127)

#define HI_ISP_LDCI_HEPOSWGT_MAX                  (255)
#define HI_ISP_LDCI_HENEGWGT_MAX                  (255)
#define HI_ISP_PREGAMMA_LUT_MAX                   (0xFFFFF)

#define EXPANDER_NODE_NUM                         (129)

#define HI_ISP_CR_SLOPE_MAX                       (16)
#define HI_ISP_CR_THR_MAX                         (65535)
#define HI_ISP_CR_NPOFFSET_MAX                    (65535)
#define HI_ISP_CR_NPOFFSET_MIN                    (8192)
#define HI_ISP_LSC_MESHSTR_MAX                    (1023)
#define HI_ISP_LSC_DEFAULT_MESH_STRENGTH          (256)
#define HI_ISP_LSC_DEFAULT_WEIGHT                 (256)
#define HI_ISP_LSC_GRID_COL                       (17)
#define HI_ISP_LSC_GRID_ROW                       (17)
#define HI_ISP_LSC_GRID_POINTS                    (HI_ISP_LSC_GRID_COL * HI_ISP_LSC_GRID_ROW)
#define HI_ISP_LSC_DEFAULT_MESH_SCALE             (4)    // 0.10bit
#define HI_ISP_LSC_XGRID_WIDTH_MAX(x)        (255)
#define HI_ISP_LSC_YGRID_WIDTH_MAX(x)        (255)

#define DEFOG_ZONE_ROW                            (24)
#define DEFOG_ZONE_COLUMN                         (16)
#define DEFOG_ZONE_NUM                            (384)
#define DEFOG_MAX_ZONE_NUM                        (384)

#define HI_ISP_CCM_RECOVER_EN_DEFAULT             (1)
#define HI_ISP_CCM_LUMA_COEFR_DEFAULT             (8)
#define HI_ISP_CCM_LUMA_COEFB_DEFAULT             (4)
#define HI_ISP_CCM_DARKPREV_DEFAULT               (14)
#define HI_ISP_CCM_LUMA_COEFR_UP_DEFAULT          (10)
#define HI_ISP_CCM_LUMA_COEFB_UP_DEFAULT          (10)
#define HI_ISP_CCM_SOFT_CLIP0_STEP_DEFAULT        (10)
#define HI_ISP_CCM_SOFT_CLIP1_STEP_DEFAULT        (14)
#define HI_ISP_CCM_PEAKSUPP_SAT_DEFAULT           (110)
#define HI_ISP_CCM_PEAKSUPP_MAX_DEFAULT           (255)
#define HI_ISP_CCM_DARK_RANGE_MAX                 (15)


#define HI_ISP_BAYERNR_LUT_EVEN_LENGTH            (17)
#define HI_ISP_BAYERNR_LUT_ODD_LENGTH             (16)
#define HI_ISP_BAYERNR_LMTLUT_EVEN_NUM            (65)
#define HI_ISP_BAYERNR_LMTLUT_ODD_NUM             (64)


#define  HI_ISP_BAYERNR_WINDOW_SIZE_SEL_DEFAULT (4)
#define  HI_ISP_BAYERNR_PATTERN_NOISE_REDUCTION_DEFAULT (1)
#define  HI_ISP_BAYERNR_GAIN_SAD_DEFAULT (0x40)
#define  HI_ISP_BAYERNR_IMPLS_NR_ENABLE1_DEFAULT (1 )
#define  HI_ISP_BAYERNR_IMPLS_NR_ENABLE2_DEFAULT (1 )
#define  HI_ISP_BAYERNR_IMPLS_NR_ENABLE3_DEFAULT (1 )
#define  HI_ISP_BAYERNR_IMPLS_NR_STRENGTH_DEFAULT (0x20)
#define  HI_ISP_BAYERNR_SYMMSAD_DEFAULT  (1)
#define  HI_ISP_BAYERNR_ENABLE_POST_PROC_DEFAULT (3)
#define  HI_ISP_BAYERNR_BILATERAL_ENABLE_DEFAULT (2)
#define  HI_ISP_BAYERNR_NLIMIT_GAIN_BLT_DEFAULT (200)

#define  AF_GAMMA_VALUE_MAX  (1)
#define  HI_EXT_SYSTEM_MANUAL_DEHAZE_HBLK_DEFAULT (24)
#define  HI_EXT_SYSTEM_MANUAL_DEHAZE_VBLK_DEFAULT (16)

#define HI_ISP_FE_AF_SUPPORT                      0
#define HI_ISP_BE_AF_SUPPORT                      1
#define HI_ISP_AF_ENABLE_MASK                     ((HI_ISP_BE_AF_SUPPORT << 1) + HI_ISP_FE_AF_SUPPORT)


// #define HI_ISP_CCM_RECOVER_EN_DEFAULT             (0x1)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_ISP_DEFINES_H__ */
