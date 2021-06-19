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

#ifndef _MI_MIPITX_DATATYPE_H_
#define _MI_MIPITX_DATATYPE_H_

#include "mi_sys_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MI_ERR_MIPITX_CHN_HAVECREATE MI_DEF_ERR(E_MI_MODULE_ID_MIPITX, E_MI_ERR_LEVEL_WARNING, E_MI_ERR_INITED)
#define MI_ERR_MIPITX_CHNID_INVALID MI_DEF_ERR(E_MI_MODULE_ID_MIPITX, E_MI_ERR_LEVEL_WARNING, E_MI_ERR_INVALID_CHNID)
#define MI_ERR_MIPITX_CHN_NOTSTOP MI_DEF_ERR(E_MI_MODULE_ID_MIPITX, E_MI_ERR_LEVEL_WARNING, E_MI_ERR_CHN_NOT_STOPED)
#define MI_ERR_MIPITX_ILLEGAL_PARAM MI_DEF_ERR(E_MI_MODULE_ID_MIPITX, E_MI_ERR_LEVEL_WARNING, E_MI_ERR_ILLEGAL_PARAM)


#define MI_ERR_MIPITX_RUNFAIL  MI_DEF_ERR(E_MI_MODULE_ID_MIPITX, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_EXIST)
#define MI_ERR_MIPITX_NULL_PTR      MI_DEF_ERR(E_MI_MODULE_ID_MIPITX, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR)

typedef struct MI_MipiTx_TimingConfig_s
{
    MI_U8 u8Lpx;
    MI_U8 u8ClkHsPrpr;
    MI_U8 u8ClkZero;
    MI_U8 u8ClkHsPre;
    MI_U8 u8ClkHsPost;
    MI_U8 u8ClkTrail;
    MI_U8 u8HsPrpr;
    MI_U8 u8HsZero;
    MI_U8 u8HsTrail;
} MI_MipiTx_TimingConfig_t;

typedef enum
{
    E_MI_MIPITX_LANE_NUM_NONE = 0,
    E_MI_MIPITX_LANE_NUM_1    = 1,
    E_MI_MIPITX_LANE_NUM_2    = 2,
    E_MI_MIPITX_LANE_NUM_3    = 3,
    E_MI_MIPITX_LANE_NUM_4    = 4,
} MI_MipiTx_LaneNum_e;

typedef enum
{
    E_MI_MIPITX_CH_SWAP_0,
    E_MI_MIPITX_CH_SWAP_1,
    E_MI_MIPITX_CH_SWAP_2,
    E_MI_MIPITX_CH_SWAP_3,
    E_MI_MIPITX_CH_SWAP_4,
} MI_MipiTx_ChannelSwapType_e;

typedef struct
{
    MI_U32                          u32Width;
    MI_U32                          u32Height;
    MI_SYS_PixelFormat_e            ePixFormat; // YUV422 YVYU, RAW8
    MI_MipiTx_LaneNum_e             eLaneNum;

    MI_U8                          u8DCLKDelay; ///<  DCLK Delay
    MI_U32                         u32Dclk;     ///<  DCLK ( Htt * Vtt * Fps)

    MI_MipiTx_ChannelSwapType_e    *peChSwapType;  ///< all lane order swap
}MI_MipiTx_ChannelAttr_t;

typedef struct MI_MipiTx_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_MipiTx_InitParam_t;

#ifdef __cplusplus
}
#endif
#endif

