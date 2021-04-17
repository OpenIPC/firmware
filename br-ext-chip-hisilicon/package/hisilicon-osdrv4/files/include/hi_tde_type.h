/******************************************************************************

  Copyright (C), 2016-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
 File Name     : tde_type.h
Version       : Initial Draft
Author        :
Created       : 2016/5/21
Last Modified :
Description   : TDE public type
Function List :
History       :
1.Date        : 2016/5/21
Author      :
Modification: Created file

 ******************************************************************************/
#ifndef __TDE_TYPE_H__
#define __TDE_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "hi_tde_errcode.h"

/****************************************************************************/
/*                             TDE2 types define                             */
/****************************************************************************/

/*************************** Structure Definition ****************************/
/** \addtogroup      TDE */


/**Definition of the TDE handle*/
typedef HI_S32 TDE_HANDLE;


typedef HI_VOID (* TDE_FUNC_CB) (HI_U32 enCallModId, HI_U32 u32CallDevId, HI_U32 u32CallChnId,
                            HI_VOID* pJobData);


typedef HI_VOID (* TDE_TRIG_SEL)(HI_U32);

/**RGB and packet YUV formats*/
typedef enum hiTDE2_COLOR_FMT_E
{
    TDE2_COLOR_FMT_RGB444 = 0,          /**<For the RGB444 format, red occupies four bits, green occupies four bits, and blue occupies four bits. Other formats may be deduced by analogy.*/
    TDE2_COLOR_FMT_BGR444,              /**<BGR444 format*/
    TDE2_COLOR_FMT_RGB555,              /**<RGB555 format*/
    TDE2_COLOR_FMT_BGR555,              /**<BGR555 format*/
    TDE2_COLOR_FMT_RGB565,              /**<RGB565 format*/
    TDE2_COLOR_FMT_BGR565,              /**<BGR565 format*/
    TDE2_COLOR_FMT_RGB888,              /**<RGB888 format*/
    TDE2_COLOR_FMT_BGR888,              /**<BGR888 format*/
    TDE2_COLOR_FMT_ARGB4444,          	/**<ARGB4444 format*/
    TDE2_COLOR_FMT_ABGR4444,           	/**<ABGR4444 format*/
    TDE2_COLOR_FMT_RGBA4444,            /**<RGBA4444 format*/
    TDE2_COLOR_FMT_BGRA4444,            /**<BGRA4444 format*/
    TDE2_COLOR_FMT_ARGB1555,            /**<ARGB1555 format*/
    TDE2_COLOR_FMT_ABGR1555,            /**<ABGR1555 format*/
    TDE2_COLOR_FMT_RGBA1555,            /**<RGBA1555 format*/
    TDE2_COLOR_FMT_BGRA1555,            /**<BGRA1555 format*/
    TDE2_COLOR_FMT_ARGB8565,            /**<ARGB8565 format*/
    TDE2_COLOR_FMT_ABGR8565,            /**<ABGR8565 format*/
    TDE2_COLOR_FMT_RGBA8565,            /**<RGBA8565 format*/
    TDE2_COLOR_FMT_BGRA8565,            /**<BGRA8565 format*/
    TDE2_COLOR_FMT_ARGB8888,            /**<ARGB8888 format*/
    TDE2_COLOR_FMT_ABGR8888,            /**<ABGR8888 format*/
    TDE2_COLOR_FMT_RGBA8888,            /**<RGBA8888 format*/
    TDE2_COLOR_FMT_BGRA8888,            /**<BGRA8888 format*/
    TDE2_COLOR_FMT_RABG8888,            /**<RABG8888 format*/
    TDE2_COLOR_FMT_CLUT1,               /**<1-bit palette format without alpha component. Each pixel occupies one bit.*/
    TDE2_COLOR_FMT_CLUT2,               /**<2-bit palette format without alpha component. Each pixel occupies two bits.*/
    TDE2_COLOR_FMT_CLUT4,               /**<4-bit palette format without alpha component. Each pixel occupies four bits.*/
    TDE2_COLOR_FMT_CLUT8,               /**<8-bit palette format without alpha component. Each pixel occupies eight bits.*/
    TDE2_COLOR_FMT_ACLUT44,             /**<1-bit palette format with alpha component. Each pixel occupies one bit.*/
    TDE2_COLOR_FMT_ACLUT88,             /**<1-bit palette format with alpha component. Each pixel occupies one bit.*/
    TDE2_COLOR_FMT_A1,                  /**<Alpha format. Each pixel occupies one bit.*/
    TDE2_COLOR_FMT_A8,                  /**<Alpha format. Each pixel occupies eight bits.*/
    TDE2_COLOR_FMT_YCbCr888,            /**<YUV packet format without alpha component*/
    TDE2_COLOR_FMT_AYCbCr8888,          /**<YUV packet format with alpha component*/
    TDE2_COLOR_FMT_YCbCr422,            /**<YUV packet422 format*/
    TDE2_COLOR_FMT_PKGVYUY,             /**<YUV packet422 format, VYUY format*/
    TDE2_COLOR_FMT_byte,                /**<Only for fast copy*/
	TDE2_COLOR_FMT_halfword,            /**<Only for fast copy*/
    TDE2_COLOR_FMT_JPG_YCbCr400MBP,     /**<Semi-planar YUV400 format, for JPG decoding*/
    TDE2_COLOR_FMT_JPG_YCbCr422MBHP,    /**<Semi-planar YUV422 format, horizontal sampling, for JPG decoding*/
    TDE2_COLOR_FMT_JPG_YCbCr422MBVP,    /**<Semi-planar YUV422 format, vertical sampling, for JPG decoding*/
    TDE2_COLOR_FMT_MP1_YCbCr420MBP,     /**<Semi-planar YUV420 format*/
    TDE2_COLOR_FMT_MP2_YCbCr420MBP,     /**<Semi-planar YUV420 format*/
    TDE2_COLOR_FMT_MP2_YCbCr420MBI,     /**<Semi-planar YUV400 format*/
    TDE2_COLOR_FMT_JPG_YCbCr420MBP,     /**<Semi-planar YUV400 format, for JPG decoding*/
    TDE2_COLOR_FMT_JPG_YCbCr444MBP,     /**<Semi-planar YUV444 format*/
    TDE2_COLOR_FMT_BUTT                 /**<End of enumeration*/
} TDE2_COLOR_FMT_E;

/**Definition of the semi-planar YUV format*/
typedef enum hiTDE2_MB_COLORFMT_E
{
    TDE2_MB_COLOR_FMT_JPG_YCbCr400MBP = 0,/**<Semi-planar YUV400 format, for JPG decoding*/
    TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP,   /**<Semi-planar YUV422 format, horizontal sampling, for JPG decoding*/
    TDE2_MB_COLOR_FMT_JPG_YCbCr422MBVP,   /**<Semi-planar YUV422 format, vertical sampling, for JPG decoding*/
    TDE2_MB_COLOR_FMT_MP1_YCbCr420MBP,    /**<Semi-planar YUV420 format*/
    TDE2_MB_COLOR_FMT_MP2_YCbCr420MBP,    /**<Semi-planar YUV420 format*/
    TDE2_MB_COLOR_FMT_MP2_YCbCr420MBI,    /**<Semi-planar YUV400 format*/
    TDE2_MB_COLOR_FMT_JPG_YCbCr420MBP,    /**<Semi-planar YUV400 format, for JPG pictures*/
    TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP,    /**<Semi-planar YUV444 format, for JPG pictures*/
    TDE2_MB_COLOR_FMT_BUTT
} TDE2_MB_COLOR_FMT_E;

/**Structure of the bitmap information set by customers*/
typedef struct hiTDE2_SURFACE_S
{
    HI_U64 PhyAddr;              /**<Header address of a bitmap or the Y component*/

    TDE2_COLOR_FMT_E enColorFmt;    /**<Color format*/

    HI_U32 u32Height;               /**<Bitmap height*/

    HI_U32 u32Width;                /**<Bitmap width*/

    HI_U32 u32Stride;               /**<Stride of a bitmap or the Y component*/

    HI_BOOL bYCbCrClut;             /**<Whether the CLUT is in the YCbCr space.*/

    HI_BOOL bAlphaMax255;           /**<The maximum alpha value of a bitmap is 255 or 128.*/

    HI_BOOL bAlphaExt1555;          /*<Whether to enable the alpha extension of an ARGB1555 bitmap.*/
    HI_U8 u8Alpha0;                 /**<Values of alpha0 and alpha1, used as the ARGB1555 format*/
    HI_U8 u8Alpha1;                 /**<Values of alpha0 and alpha1, used as the ARGB1555 format*/
    HI_U64 CbCrPhyAddr;          /**<Address of the CbCr component, pilot*/
    HI_U32 u32CbCrStride;           /**<Stride of the CbCr component, pilot*/
    HI_U64 ClutPhyAddr;          /**<Address of the color look-up table (CLUT), for color extension or color correction*/
} TDE2_SURFACE_S;

/**Definition of the semi-planar YUV data*/
typedef struct hiTDE2_MB_S
{
    TDE2_MB_COLOR_FMT_E enMbFmt;        /**<YUV format*/
    HI_U64              YPhyAddr;    /**<Physical address of the Y component*/
    HI_U32              u32YWidth;      /**<Width of the Y component*/
    HI_U32              u32YHeight;     /**<Height of the Y component*/
    HI_U32              u32YStride;     /**< Stride of the Y component, indicating bytes in each row*/
    HI_U64              CbCrPhyAddr; /**<Width of the UV component*/
    HI_U32              u32CbCrStride;  /**<Stride of the UV component, indicating the bytes in each row*/
} TDE2_MB_S;

/**Definition of the TDE rectangle*/
typedef struct hiTDE2_RECT_S
{
    HI_S32 s32Xpos;     /**<Horizontal coordinate*/
    HI_S32 s32Ypos;     /**<Vertical coordinate*/
    HI_U32 u32Width;    /**<Width*/
    HI_U32 u32Height;   /**<Height*/
} TDE2_RECT_S;

/**Logical operation type*/
typedef enum hiTDE2_ALUCMD_E
{
    TDE2_ALUCMD_NONE = 0x0,         /**<No alpha and raster of operation (ROP) blending*/
    TDE2_ALUCMD_BLEND = 0x1,        /**<Alpha blending*/
    TDE2_ALUCMD_ROP = 0x2,          /**<ROP blending*/
    TDE2_ALUCMD_COLORIZE = 0x4,     /**<Colorize operation*/
    TDE2_ALUCMD_BUTT = 0x8          /**<End of enumeration*/
} TDE2_ALUCMD_E;

/**Definition of ROP codes*/
typedef enum hiTDE2_ROP_CODE_E
{
    TDE2_ROP_BLACK = 0,     /**<Blackness*/
    TDE2_ROP_NOTMERGEPEN,   /**<~(S2 | S1)*/
    TDE2_ROP_MASKNOTPEN,    /**<~S2&S1*/
    TDE2_ROP_NOTCOPYPEN,    /**< ~S2*/
    TDE2_ROP_MASKPENNOT,    /**< S2&~S1 */
    TDE2_ROP_NOT,           /**< ~S1 */
    TDE2_ROP_XORPEN,        /**< S2^S1 */
    TDE2_ROP_NOTMASKPEN,    /**< ~(S2 & S1) */
    TDE2_ROP_MASKPEN,       /**< S2&S1 */
    TDE2_ROP_NOTXORPEN,     /**< ~(S2^S1) */
    TDE2_ROP_NOP,           /**< S1 */
    TDE2_ROP_MERGENOTPEN,   /**< ~S2|S1 */
    TDE2_ROP_COPYPEN,       /**< S2 */
    TDE2_ROP_MERGEPENNOT,   /**< S2|~S1 */
    TDE2_ROP_MERGEPEN,      /**< S2|S1 */
    TDE2_ROP_WHITE,         /**< Whiteness */
    TDE2_ROP_BUTT
} TDE2_ROP_CODE_E;

/**Definition of the blit mirror*/
typedef enum hiTDE2_MIRROR_E
{
    TDE2_MIRROR_NONE = 0,       /**<No mirror*/
    TDE2_MIRROR_HORIZONTAL,     /**<Horizontal mirror*/
    TDE2_MIRROR_VERTICAL,       /**<Vertical mirror*/
    TDE2_MIRROR_BOTH,           /**<Horizontal and vertical mirror*/
    TDE2_MIRROR_BUTT
} TDE2_MIRROR_E;

/**Clip operation type*/
typedef enum hiTDE2_CLIPMODE_E
{
    TDE2_CLIPMODE_NONE = 0, /**<No clip*/
    TDE2_CLIPMODE_INSIDE,   /**<Clip the data within the rectangle to output and discard others*/
    TDE2_CLIPMODE_OUTSIDE,  /**<Clip the data outside the rectangle to output and discard others*/
    TDE2_CLIPMODE_BUTT
} TDE2_CLIPMODE_E;

/**Scaling mode for the macroblock*/
typedef enum hiTDE2_MBRESIZE_E
{
    TDE2_MBRESIZE_NONE = 0,         /**<No scaling*/
    TDE2_MBRESIZE_QUALITY_LOW,      /**<Low-quality scaling*/
    TDE2_MBRESIZE_QUALITY_MIDDLE,   /**<Medium-quality scaling*/
    TDE2_MBRESIZE_QUALITY_HIGH,     /**<High-quality scaling*/
    TDE2_MBRESIZE_BUTT
} TDE2_MBRESIZE_E;

/**Definition of fill colors*/
typedef struct hiTDE2_FILLCOLOR_S
{
    TDE2_COLOR_FMT_E enColorFmt;    /**<TDE pixel format*/
    HI_U32           u32FillColor;  /**<Fill colors that vary according to pixel formats*/
} TDE2_FILLCOLOR_S;

/**Definition of colorkey modes*/
typedef enum hiTDE2_COLORKEY_MODE_E
{
    TDE2_COLORKEY_MODE_NONE = 0,     /**<No colorkey*/
    TDE2_COLORKEY_MODE_FOREGROUND,   /**<When performing the colorkey operation on the foreground bitmap, you need to perform this operation before the CLUT for color extension and perform this operation after the CLUT for color correction.*/
    TDE2_COLORKEY_MODE_BACKGROUND,   /**<Perform the colorkey operation on the background bitmap*/
    TDE2_COLORKEY_MODE_BUTT
} TDE2_COLORKEY_MODE_E;

/**Definition of colorkey range*/
typedef struct hiTDE2_COLORKEY_COMP_S
{
    HI_U8 u8CompMin;           /**<Minimum value of a component*/
    HI_U8 u8CompMax;           /**<Maximum value of a component*/
    HI_U8 bCompOut;            /**<The colorkey of a component is within or beyond the range.*/
    HI_U8 bCompIgnore;         /**<Whether to ignore a component.*/
    HI_U8 u8CompMask;          /**<Component mask*/
    HI_U8 u8Reserved;
    HI_U8 u8Reserved1;
    HI_U8 u8Reserved2;
} TDE2_COLORKEY_COMP_S;

/**Definition of colorkey values*/
typedef union hiTDE2_COLORKEY_U
{
    struct
    {
        TDE2_COLORKEY_COMP_S stAlpha;   /**<Alpha component*/
        TDE2_COLORKEY_COMP_S stRed;     /**<Red component*/
        TDE2_COLORKEY_COMP_S stGreen;   /**<Green component*/
        TDE2_COLORKEY_COMP_S stBlue;    /**<Blue component*/
    } struCkARGB;
    struct
    {
        TDE2_COLORKEY_COMP_S stAlpha;   /**<Alpha component*/
        TDE2_COLORKEY_COMP_S stY;       /**<Y component*/
        TDE2_COLORKEY_COMP_S stCb;      /**<Cb component*/
        TDE2_COLORKEY_COMP_S stCr;      /**<Cr component*/
    } struCkYCbCr;
    struct
    {
        TDE2_COLORKEY_COMP_S stAlpha;   /**<Alpha component*/
        TDE2_COLORKEY_COMP_S stClut;    /**<Palette component*/
    } struCkClut;
} TDE2_COLORKEY_U;

/*Definition of alpha output sources*/
typedef enum hiTDE2_OUTALPHA_FROM_E
{
    TDE2_OUTALPHA_FROM_NORM = 0,    /**<Output from the result of alpha blending or anti-flicker*/
    TDE2_OUTALPHA_FROM_BACKGROUND,  /**<Output from the background bitmap*/
    TDE2_OUTALPHA_FROM_FOREGROUND,  /**<Output from the foreground bitmap*/
    TDE2_OUTALPHA_FROM_GLOBALALPHA, /**<Output from the global alpha*/
    TDE2_OUTALPHA_FROM_BUTT
} TDE2_OUTALPHA_FROM_E;

/**Definition of filtering*/
typedef enum hiTDE2_FILTER_MODE_E
{
    TDE2_FILTER_MODE_COLOR = 0, /**<Filter the color*/
    TDE2_FILTER_MODE_ALPHA,     /**<Filter the alpha channel*/
    TDE2_FILTER_MODE_BOTH,      /**<Filter the color and alpha channel*/
    TDE2_FILTER_MODE_NONE,      /**<No filter */
    TDE2_FILTER_MODE_BUTT
} TDE2_FILTER_MODE_E;

/**Configuration of the anti-flicker channel*/
typedef enum hiTDE2_DEFLICKER_MODE_E
{
    TDE2_DEFLICKER_MODE_NONE = 0,   /*<No anti-flicker*/
    TDE2_DEFLICKER_MODE_RGB,        /**<Perform anti-flicker on the RGB component*/
    TDE2_DEFLICKER_MODE_BOTH,       /**<Perform anti-flicker on the alpha component*/
    TDE2_DEFLICKER_MODE_BUTT
} TDE2_DEFLICKER_MODE_E;

/* blend mode */
typedef enum hiTDE2_BLEND_MODE_E
{
    TDE2_BLEND_ZERO = 0x0,
    TDE2_BLEND_ONE,
    TDE2_BLEND_SRC2COLOR,
    TDE2_BLEND_INVSRC2COLOR,
    TDE2_BLEND_SRC2ALPHA,
    TDE2_BLEND_INVSRC2ALPHA,
    TDE2_BLEND_SRC1COLOR,
    TDE2_BLEND_INVSRC1COLOR,
    TDE2_BLEND_SRC1ALPHA,
    TDE2_BLEND_INVSRC1ALPHA,
    TDE2_BLEND_SRC2ALPHASAT,
    TDE2_BLEND_BUTT
} TDE2_BLEND_MODE_E;

/**Alpha blending command. You can set parameters or select Porter or Duff.*/
/* pixel = (source * fs + destination * fd),
   sa = source alpha,
   da = destination alpha */
typedef enum hiTDE2_BLENDCMD_E
{
    TDE2_BLENDCMD_NONE = 0x0,     /**< fs: sa      fd: 1.0-sa */
    TDE2_BLENDCMD_CLEAR,    /**< fs: 0.0     fd: 0.0 */
    TDE2_BLENDCMD_SRC,      /**< fs: 1.0     fd: 0.0 */
    TDE2_BLENDCMD_SRCOVER,  /**< fs: 1.0     fd: 1.0-sa */
    TDE2_BLENDCMD_DSTOVER,  /**< fs: 1.0-da  fd: 1.0 */
    TDE2_BLENDCMD_SRCIN,    /**< fs: da      fd: 0.0 */
    TDE2_BLENDCMD_DSTIN,    /**< fs: 0.0     fd: sa */
    TDE2_BLENDCMD_SRCOUT,   /**< fs: 1.0-da  fd: 0.0 */
    TDE2_BLENDCMD_DSTOUT,   /**< fs: 0.0     fd: 1.0-sa */
    TDE2_BLENDCMD_SRCATOP,  /**< fs: da      fd: 1.0-sa */
    TDE2_BLENDCMD_DSTATOP,  /**< fs: 1.0-da  fd: sa */
    TDE2_BLENDCMD_ADD,      /**< fs: 1.0     fd: 1.0 */
    TDE2_BLENDCMD_XOR,      /**< fs: 1.0-da  fd: 1.0-sa */
    TDE2_BLENDCMD_DST,      /**< fs: 0.0     fd: 1.0 */
    TDE2_BLENDCMD_CONFIG,   /**<You can set the parameteres.*/
    TDE2_BLENDCMD_BUTT
} TDE2_BLENDCMD_E;

/**Options for the alpha blending operation*/
typedef struct hiTDE2_BLEND_OPT_S
{
    HI_BOOL  bGlobalAlphaEnable;        /**<Global alpha enable*/
    HI_BOOL  bPixelAlphaEnable;         /**<Pixel alpha enable*/
    HI_BOOL bSrc1AlphaPremulti;         /**<Src1 alpha premultiply enable*/
    HI_BOOL bSrc2AlphaPremulti;         /**<Src2 alpha premultiply enable*/
    TDE2_BLENDCMD_E eBlendCmd;          /**<Alpha blending command*/
    TDE2_BLEND_MODE_E eSrc1BlendMode;   /**<Src1 blending mode select. It is valid when eBlendCmd is set to TDE2_BLENDCMD_CONFIG.*/
    TDE2_BLEND_MODE_E eSrc2BlendMode;   /**<Src2 blending mode select. It is valid when eBlendCmd is set to TDE2_BLENDCMD_CONFIG.*/
} TDE2_BLEND_OPT_S;

/**CSC parameter option*/
typedef struct hiTDE2_CSC_OPT_S
{
    HI_BOOL bICSCUserEnable;        /**User-defined ICSC parameter enable*/
    HI_BOOL bICSCParamReload;   /**User-defined ICSC parameter reload enable*/
    HI_BOOL bOCSCUserEnable;        /**User-defined OCSC parameter enable*/
    HI_BOOL bOCSCParamReload;   /**User-defined OCSC parameter reload enable*/
    HI_U64 ICSCParamAddr;        /**ICSC parameter address. The address must be 128-bit aligned.*/
    HI_U64 OCSCParamAddr;    /**OCSC parameter address. The address must be 128-bit aligned.*/
} TDE2_CSC_OPT_S;

/**Definition of blit operation options*/
typedef struct hiTDE2_OPT_S
{
    TDE2_ALUCMD_E enAluCmd;                 /**<Logical operation type*/

    TDE2_ROP_CODE_E enRopCode_Color;        /**<ROP type of the color space*/

    TDE2_ROP_CODE_E enRopCode_Alpha;        /**<ROP type of the alpha component*/

    TDE2_COLORKEY_MODE_E enColorKeyMode;    /**<Colorkey mode*/

    TDE2_COLORKEY_U unColorKeyValue;        /**<Colorkey value*/

    TDE2_CLIPMODE_E enClipMode;             /**<Perform the clip operation within or beyond the area*/

    TDE2_RECT_S stClipRect;                 /**<Definition of the clipping area*/

    TDE2_DEFLICKER_MODE_E enDeflickerMode;  /**<Anti-flicker mode*/

    HI_BOOL bResize;                        /**<Whether to scale*/

    TDE2_FILTER_MODE_E enFilterMode;        /**<Filtering mode during scaling*/

    TDE2_MIRROR_E enMirror;                 /**<Mirror type*/

    HI_BOOL bClutReload;                    /**<Whether to reload the CLUT*/

    HI_U8   u8GlobalAlpha;                  /**<Global alpha value*/

    TDE2_OUTALPHA_FROM_E enOutAlphaFrom;    /**<Source of the output alpha*/

    HI_U32 u32Colorize;                     /**<Colorize value*/

    TDE2_BLEND_OPT_S stBlendOpt;

    TDE2_CSC_OPT_S stCscOpt;
    HI_BOOL bCompress;
    HI_BOOL bDecompress;
} TDE2_OPT_S;

/**Definition of macroblock operation options*/
typedef struct hiTDE2_MBOPT_S
{
    TDE2_CLIPMODE_E enClipMode;     /**<Clip mode*/

    TDE2_RECT_S stClipRect;         /**<Definition of the clipping area*/

    HI_BOOL bDeflicker;             /**<Whether to perform anti-flicker*/

    TDE2_MBRESIZE_E enResize;       /**<Scaling information*/

    HI_BOOL bSetOutAlpha;           /**<If the alpha value is not set, the maximum alpha value is output by default.*/

    HI_U8   u8OutAlpha;             /**<Global alpha for operation*/
} TDE2_MBOPT_S;

/**Definition of the pattern filling operation*/
typedef struct hiTDE2_PATTERN_FILL_OPT_S
{
    TDE2_ALUCMD_E enAluCmd;                 /**<Logical operation type*/

    TDE2_ROP_CODE_E enRopCode_Color;        /**<ROP type of the color space*/

    TDE2_ROP_CODE_E enRopCode_Alpha;        /**<ROP type of the alpha component*/

    TDE2_COLORKEY_MODE_E enColorKeyMode;    /**<Colorkey mode*/

    TDE2_COLORKEY_U unColorKeyValue;        /**<Colorkey value*/

    TDE2_CLIPMODE_E enClipMode;             /**<Clip mode*/

    TDE2_RECT_S stClipRect;                 /**<Clipping area*/

    HI_BOOL bClutReload;                    /**<Whether to reload the CLUT*/

    HI_U8 u8GlobalAlpha;                    /**<Global alpha*/

    TDE2_OUTALPHA_FROM_E enOutAlphaFrom;    /**<Source of the output alpha*/

    HI_U32 u32Colorize;                     /**<Colorize value*/

    TDE2_BLEND_OPT_S stBlendOpt;           /**<Options of the blending operation*/

    TDE2_CSC_OPT_S stCscOpt;        /**<CSC parameter option*/

} TDE2_PATTERN_FILL_OPT_S;

/**Definition of rotation directions*/
typedef enum hiTDE_ROTATE_ANGLE_E
{
    TDE_ROTATE_CLOCKWISE_90 = 0,    /**<Rotate 90 degree clockwise*/
    TDE_ROTATE_CLOCKWISE_180,       /**<Rotate 180 degree clockwise*/
    TDE_ROTATE_CLOCKWISE_270,       /**<Rotate 270 degree clockwise*/
    TDE_ROTATE_BUTT
} TDE_ROTATE_ANGLE_E;

/**Definition of anti-flicker levels*/
typedef enum hiTDE_DEFLICKER_LEVEL_E
{
    TDE_DEFLICKER_AUTO = 0, /**<Adaptation. The anti-flicker coefficient is selected by the TDE.*/
    TDE_DEFLICKER_LOW,      /**<Low-level anti-flicker*/
    TDE_DEFLICKER_MIDDLE,   /**<Medium-level anti-flicker*/
    TDE_DEFLICKER_HIGH,     /**High-level anti-flicker*/
    TDE_DEFLICKER_BUTT
} TDE_DEFLICKER_LEVEL_E;

/* composed surface info */
typedef struct hiTDE_COMPOSOR_S
{
    TDE2_SURFACE_S stSrcSurface;
    TDE2_RECT_S stInRect;
    TDE2_RECT_S stOutRect;
    TDE2_OPT_S stOpt;
} TDE_COMPOSOR_S;



/* composed surface list */
typedef struct hiTDE_SURFACE_LIST_S
{
    HI_U32 u32SurfaceNum;
    TDE2_SURFACE_S *pDstSurface;
    TDE_COMPOSOR_S *pstComposor;
} TDE_SURFACE_LIST_S;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __TDE_TYPE_H__ */


