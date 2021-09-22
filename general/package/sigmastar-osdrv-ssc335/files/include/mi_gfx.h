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
#ifndef _MI_GFX_H_
#define _MI_GFX_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"
#include "mi_gfx_datatype.h"

#define GFX_MAJOR_VERSION 2
#define GFX_SUB_VERSION 5
#define MACRO_TO_STR(macro) #macro
#define GFX_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_gfx_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_gfx_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_gfx_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_GFX_API_VERSION GFX_VERSION_STR(GFX_MAJOR_VERSION,GFX_SUB_VERSION)

/*-------------------------------------------------------------------------------------------
 * Global Functions
-------------------------------------------------------------------------------------------*/

MI_S32 MI_GFX_Open(void);
MI_S32 MI_GFX_Close(void);
MI_S32 MI_GFX_WaitAllDone(MI_BOOL bWaitAllDone, MI_U16 u16TargetFence);

//-------------------------------------------------------------------------------------------------
/// Fill Rect
/// @param  pstDst          \b IN: Target Surface info
/// @param  pstDstRect          \b IN: Target Rect info
/// @param  u32ColorVal          \b IN: Color to fill
///     For all RGB color, the color set as the ARGB8888 format.\n
///     Each color component need to shift to high bit.\n
///     Use ARGB1555 as the example, the source color key as the following:\n
///     ARGB1555  --> ARRRRRGGGGGBBBBB                   (every character represents one bit)\n
///     For I8 format, the index set to b component\n
/// @param  pu16Fence          \b OUT: wait fence
/// @return MI_SUCCESS         - Success
/// @return OTHER            - Failure
//------------------------------------------------------------------------------------------------
MI_S32 MI_GFX_QuickFill(MI_GFX_Surface_t *pstDst, MI_GFX_Rect_t *pstDstRect,
    MI_U32 u32ColorVal, MI_U16 *pu16Fence);
MI_S32 MI_GFX_GetAlphaThresholdValue(MI_U8 *pu8ThresholdValue);
MI_S32 MI_GFX_SetAlphaThresholdValue(MI_U8 u8ThresholdValue);
MI_S32 MI_GFX_BitBlit(MI_GFX_Surface_t *pstSrc, MI_GFX_Rect_t *pstSrcRect,
    MI_GFX_Surface_t *pstDst,  MI_GFX_Rect_t *pstDstRect, MI_GFX_Opt_t *pstOpt, MI_U16 *pu16Fence);
//-------------------------------------------------------------------------------------------------
/// Set Palette for Index Color format(I2/I4/I8)
/// @param  eColorFmt          \b IN: Index Color format(I2/I4/I8)
/// @param  pstPalette          \b IN: RGB color data array for corresponding Index Color
/// @return MI_SUCCESS         - Success
/// @return OTHER            - Failure,refer to error code
//------------------------------------------------------------------------------------------------
MI_S32 MI_GFX_SetPalette(MI_GFX_ColorFmt_e eColorFmt, MI_GFX_Palette_t* pstPalette);

MI_S32 MI_GFX_InitDev(MI_GFX_InitParam_t *pstInitParam);
MI_S32 MI_GFX_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif //_MI_GFX_H_
