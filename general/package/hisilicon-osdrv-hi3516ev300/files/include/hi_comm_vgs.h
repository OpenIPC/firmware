/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Common struct definition for VGS
 * Author: Hisilicon multimedia software group
 * Create: 2013-07-24
 */

#ifndef __HI_COMM_VGS_H__
#define __HI_COMM_VGS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* failure caused by malloc buffer */
#define HI_ERR_VGS_NOBUF         HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VGS_BUF_EMPTY     HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_VGS_NULL_PTR      HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VGS_ILLEGAL_PARAM HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VGS_BUF_FULL      HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_VGS_SYS_NOTREADY  HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VGS_NOT_SUPPORT   HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VGS_NOT_PERMITTED HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define VGS_PRIVATE_DATA_LEN 6

typedef HI_S32 VGS_HANDLE;

typedef enum hiVGS_COLOR_REVERT_MODE_E {
    VGS_COLOR_REVERT_NONE = 0, /* Not revert */
    VGS_COLOR_REVERT_RGB,      /* Revert RGB */
    VGS_COLOR_REVERT_ALPHA,    /* Revert alpha */
    VGS_COLOR_REVERT_BOTH,     /* Revert RGB and alpha */
    VGS_COLOR_REVERT_BUTT
} VGS_COLOR_REVERT_MODE_E;

typedef struct hiVGS_OSD_REVERT_S {
    RECT_S stSrcRect;                          /* OSD color revert area */
    VGS_COLOR_REVERT_MODE_E enColorRevertMode; /* OSD color revert mode */
} VGS_OSD_REVERT_S;

typedef struct hiVGS_TASK_ATTR_S {
    VIDEO_FRAME_INFO_S stImgIn;  /* Input picture */
    VIDEO_FRAME_INFO_S stImgOut; /* Output picture */
    HI_U64 au64PrivateData[4];   /* RW; Private data of task */
    HI_U32 reserved;             /* RW; Debug information,state of current picture */
} VGS_TASK_ATTR_S;

typedef struct hiVGS_DRAW_LINE_S {
    POINT_S stStartPoint; /* Line start point */
    POINT_S stEndPoint;   /* Line end point */
    HI_U32 u32Thick;      /* RW; Width of line */
    HI_U32 u32Color;      /* RW; Range: [0,0xFFFFFF]; Color of line */
} VGS_DRAW_LINE_S;

typedef enum hiVGS_COVER_TYPE_E {
    COVER_RECT = 0,    /* Retangle cover */
    COVER_QUAD_RANGLE, /* Quadrangle cover */
    COVER_BUTT
} VGS_COVER_TYPE_E;

typedef struct hiVGS_QUADRANGLE_COVER_S {
    HI_BOOL bSolid;     /* Solid or hollow cover */
    HI_U32 u32Thick;    /* RW; Range: [2,8]; Thick of the hollow quadrangle */
    POINT_S stPoint[4]; /* Four points of the quadrangle */
} VGS_QUADRANGLE_COVER_S;

typedef struct hiVGS_ADD_COVER_S {
    VGS_COVER_TYPE_E enCoverType; /* Cover type */
    union {
        RECT_S stDstRect;                    /* The rectangle area */
        VGS_QUADRANGLE_COVER_S stQuadRangle; /* The quadrangle area */
    };

    HI_U32 u32Color; /* RW; Range: [0,0xFFFFFF]; Color of cover */
} VGS_ADD_COVER_S;

typedef struct hiVGS_ADD_OSD_S {
    RECT_S stRect;                /* Osd area */
    HI_U32 u32BgColor;            /* RW; Background color of osd, depends on pixel format of osd,
                                        ARGB8888:[0,0xFFFFFFFF], ARGB4444:[0,0xFFFF], ARGB1555:[0,0x1FFF] */
    PIXEL_FORMAT_E enPixelFmt;    /* Pixel format of osd */
    HI_U64 u64PhyAddr;            /* RW; Physical address of osd */
    HI_U32 u32Stride;             /* RW; Range: [32,8192]; Stride of osd */
    HI_U32 u32BgAlpha;            /* RW; Range: [0,255]; Background alpha of osd */
    HI_U32 u32FgAlpha;            /* RW; Range: [0,255]; Foreground alpha of osd */
    HI_BOOL bOsdRevert;           /* RW; Enable osd color revert */
    VGS_OSD_REVERT_S stOsdRevert; /* Osd color revert information */
    HI_U16 u16ColorLUT[2];
} VGS_ADD_OSD_S;

typedef enum hiVGS_SCLCOEF_MODE_E {
    VGS_SCLCOEF_NORMAL = 0, /* normal scale coefficient */
    VGS_SCLCOEF_TAP2 = 1,   /* scale coefficient of 2 tap */
    VGS_SCLCOEF_TAP4 = 2,   /* scale coefficient of 4 tap */
    VGS_SCLCOEF_TAP6 = 3,   /* scale coefficient of 6 tap */
    VGS_SCLCOEF_TAP8 = 4,   /* scale coefficient of 8 tap */
    VGS_SCLCOEF_BUTT
} VGS_SCLCOEF_MODE_E;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_VGS_H__ */
