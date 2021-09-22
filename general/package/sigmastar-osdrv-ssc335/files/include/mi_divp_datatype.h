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
#ifndef _MI_DIVP_DATATYPE_H_
#define _MI_DIVP_DATATYPE_H_
#pragma pack(push)
#pragma pack(4)

#define MI_DIVP_ERR_INVALID_DEVID (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID))
#define MI_DIVP_ERR_INVALID_CHNID (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID))
#define MI_DIVP_ERR_INVALID_PARAM (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM))
#define MI_DIVP_ERR_NULL_PTR (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR))
#define MI_DIVP_ERR_FAILED (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED))
#define MI_DIVP_ERR_CHN_NOT_STARTED (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STARTED))
#define MI_DIVP_ERR_CHN_NOT_STOPED (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STOPED))
#define MI_DIVP_ERR_CHN_NOT_SUPPORT (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT))
#define MI_DIVP_ERR_NO_RESOUCE (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_UNEXIST))
#define MI_DIVP_ERR_CHN_BUSY (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY))
#define MI_DIVP_ERR_HAS_CREATED (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_EXIST))
#define MI_DIVP_ERR_NO_CONTENT (MI_DEF_ERR( E_MI_MODULE_ID_DIVP, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NO_CONTENT))

typedef enum
{
    E_MI_DIVP_DI_TYPE_OFF,//off
    E_MI_DIVP_DI_TYPE_2D,///2.5D DI
    E_MI_DIVP_DI_TYPE_3D,///3D DI
    E_MI_DIVP_DI_TYPE_NUM,
} MI_DIVP_DiType_e;

typedef enum
{
    E_MI_DIVP_TNR_LEVEL_OFF,
    E_MI_DIVP_TNR_LEVEL_LOW,
    E_MI_DIVP_TNR_LEVEL_MIDDLE,
    E_MI_DIVP_TNR_LEVEL_HIGH,
    E_MI_DIVP_TNR_LEVEL_NUM,
} MI_DIVP_TnrLevel_e;

typedef struct MI_DIVP_OutputPortAttr_s
{
    MI_U32 u32Width;//output width
    MI_U32 u32Height;//output height
    MI_SYS_PixelFormat_e ePixelFormat;//output pixel format
    MI_SYS_CompressMode_e eCompMode;//compress mode
}MI_DIVP_OutputPortAttr_t;

typedef struct MI_DIVP_ChnAttr_s
{
    MI_U32 u32MaxWidth;//support max input width
    MI_U32 u32MaxHeight;//support max input height
    MI_DIVP_TnrLevel_e eTnrLevel;//TNR level
    MI_DIVP_DiType_e eDiType;//DI type
    MI_SYS_Rotate_e eRotateType;//rotate angle
    MI_SYS_WindowRect_t stCropRect;//crop information
    MI_BOOL bHorMirror;//horizontal mirror
    MI_BOOL bVerMirror;//vertical mirror
}MI_DIVP_ChnAttr_t;

typedef struct MI_DIVP_DirectBuf_s
{
    MI_SYS_PixelFormat_e ePixelFormat; //YUV420SP or ARGB888 only
    MI_U32 u32Width;
    MI_U32 u32Height;
    MI_U32 u32Stride[3];
    MI_PHY phyAddr[3];
}MI_DIVP_DirectBuf_t;

typedef struct MI_DIVP_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_DIVP_InitParam_t;

#pragma pack(pop)
#endif
