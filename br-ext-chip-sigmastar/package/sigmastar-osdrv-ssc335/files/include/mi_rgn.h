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
#ifndef _MI_RGN_H_
#define _MI_RGN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"
#include "mi_rgn_datatype.h"

#define RGN_MAJOR_VERSION 2
#define RGN_SUB_VERSION 7
#define MACRO_TO_STR(macro) #macro
#define RGN_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_rgn_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_rgn_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_rgn_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_RGN_API_VERSION RGN_VERSION_STR(RGN_MAJOR_VERSION,RGN_SUB_VERSION)

MI_S32 MI_RGN_Init(MI_RGN_PaletteTable_t *pstPaletteTable);
MI_S32 MI_RGN_DeInit(void);
MI_S32 MI_RGN_Create(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
MI_S32 MI_RGN_Destroy (MI_RGN_HANDLE hHandle);
MI_S32 MI_RGN_GetAttr(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
MI_S32 MI_RGN_SetBitMap(MI_RGN_HANDLE hHandle, MI_RGN_Bitmap_t *pstBitmap);
MI_S32 MI_RGN_AttachToChn(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t* pstChnPort, MI_RGN_ChnPortParam_t *pstChnAttr);
MI_S32 MI_RGN_DetachFromChn(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort);
MI_S32 MI_RGN_SetDisplayAttr(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
MI_S32 MI_RGN_GetDisplayAttr(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
MI_S32 MI_RGN_GetCanvasInfo(MI_RGN_HANDLE hHandle, MI_RGN_CanvasInfo_t* pstCanvasInfo);
MI_S32 MI_RGN_UpdateCanvas(MI_RGN_HANDLE hHandle);
MI_S32 MI_RGN_ScaleRect(MI_RGN_ChnPort_t *pstChnPort, MI_RGN_Size_t *pstCanvasSize, MI_RGN_Size_t *pstScreenSize);
MI_S32 MI_RGN_InitDev(MI_RGN_InitParam_t *pstInitParam);
MI_S32 MI_RGN_DeInitDev(void);


#ifdef __cplusplus
}
#endif

#endif //_MI_RGN_H_
