/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef _MI_IVE_H_
#define _MI_IVE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "mi_ive_datatype.h"

/*****************************************************************************
*   Prototype    : MI_IVE_Create
*   Description  : Create IVE handle
*   Parameters   : MI_IVE_HANDLE            hHandle         Handle ID of a task
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Create(MI_IVE_HANDLE hHandle);

/*****************************************************************************
*   Prototype    : MI_IVE_Destroy
*   Description  : Destroy IVE handle
*   Parameters   : MI_IVE_HANDLE            hHandle         Handle ID of a task
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Destroy(MI_IVE_HANDLE hHandle);

/*****************************************************************************
*   Prototype    : MI_IVE_Filter
*   Description  : 5x5 template filter.
*   Parameters   : MI_IVE_HANDLE            hHandle         Handle ID of a task
*                  MI_IVE_SrcImage_t       *pstSrc             Input source data.
*                                                               The U8C1,SP420 and SP422 input formats are supported.
*                  MI_IVE_DstImage_t       *pstDst             Output result, of same type with the input.
*                  MI_IVE_FilterCtrl_t     *pstFltCtrl         Control parameters of filter
*                  MI_BOOL                  bInstant            Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Filter(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_FilterCtrl_t *pstFltCtrl,MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Csc
*   Description  : YUV2RGB\YUV2HSV\YUV2LAB\RGB2YUV color space conversion are supported.
*   Parameters   : MI_IVE_HANDLE         hHandle       Handle ID of a task
*                  MI_IVE_SrcImage_t    *pstSrc           Input source data:
*                                                          1. SP420\SP422 type for YUV2RGB\YUV2HSV\YUV2LAB;
*                                                          2. U8C3_PACKAGE\U8C3_PLANAR type for RGB2YUV;
*                  MI_IVE_DstImage_t    *pstDst           Output result:
*                                                          1. U8C3_PACKAGE\U8C3_PLANAR typed for YUV2RGB\YUV2HSV\YUV2LAB;
*                                                          2. SP420\SP422 type for RGB2YUV;
*                  MI_IVE_CscCtrl_t     *pstCscCtrl       Control parameters for CSC
*                  MI_BOOL               bInstant         Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Csc(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_CscCtrl_t *pstCscCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_FilterAndCsc
*   Description  : Only support YUV2RGB color space conversion.
*   Parameters   : MI_IVE_HANDLE                 hHandle        Handle ID of a task.
*                  MI_IVE_SrcImage_t            *pstSrc            Input source data.Only SP420\SP422 type are supported.
*                  MI_IVE_DstImage_t            *pstDst            Output result.Only U8C3_PACKAGE\U8C3_PLANAR are supported.
*                  MI_IVE_FilterAndCscCtrl_t  *pstFltCscCtrl     Control parameters.
*                  MI_BOOL                       bInstant          Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_FilterAndCsc(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
        MI_IVE_DstImage_t *pstDst, MI_IVE_FilterAndCscCtrl_t *pstFltCscCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Sobel
*   Description  : SOBEL is used to extract the gradient information.
*   Parameters   : MI_IVE_HANDLE           hHandle      Handle ID of a task
*                  MI_IVE_SrcImage_t      *pstSrc          Input source data. Only the U8C1 input image is supported.
*                  MI_IVE_DstImage_t      *pstDstH         The (horizontal) result of input image filtered by the input mask;
*                  MI_IVE_DstImage_t      *pstDstV         The (vertical) result  of input image filtered by the transposed mask;
*                  MI_IVE_SobelCtrl_t     *pstSobelCtrl    Control parameters
*                  MI_BOOL                 bInstant        Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Sobel(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
        MI_IVE_DstImage_t *pstDstH, MI_IVE_DstImage_t *pstDstV,
        MI_IVE_SobelCtrl_t *pstSobelCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_MagAndAng
*   Description  : MagAndAng is used to extract the edge information.
*   Parameters   : MI_IVE_HANDLE              hHandle         Handle ID of a task
*                  MI_IVE_SRC_INFO_S          *pstSrc             Input source data. Only the U8C1 input format is supported.
*                  MI_IVE_MemInfo_t          *pstDstMag          Output magnitude.
*                  MI_IVE_MemInfo_t          *pstDstAng          Output angle.
*                                                              If the output mode is set to magnitude only,
*                                                              this item can be set to null.
*                  MI_IVE_MagAndAngCtrl_t  *pstMagAndAngCtrl   Control parameters
*                  MI_BOOL                    bInstant           Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_MagAndAng(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
        MI_IVE_DstImage_t *pstDstMag, MI_IVE_DstImage_t *pstDstAng,
        MI_IVE_MagAndAngCtrl_t *pstMagAndAngCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Dilate
*   Description  : 5x5 template dilate. Only the U8C1 binary image input is supported.Or else the result is not expected.
*   Parameters   : MI_IVE_HANDLE             hHandle          Handle ID of a task
*                  MI_IVE_SrcImage_t        *pstSrc              Input binary image, which consists of 0 or 255;
*                  MI_IVE_DstImage_t        *pstDst              Output result.
*                  MI_IVE_DilateCtrl_t      *pstDilateCtrl       Control parameters.
*                  MI_BOOL                   bInstant            Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*                  The input value, output value, and mask value must be 0 or 255.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Dilate(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_DilateCtrl_t *pstDilateCtrl,MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Erode
*   Parameters   : 5x5 template erode. Only the U8C1 binary image input is supported.Or else the result is not correct.
*   Input        : MI_IVE_HANDLE               hHandle       Handle ID of a task
*                  MI_IVE_SrcImage_t          *pstSrc           Input binary image, which consists of 0 or 255;
*                  MI_IVE_DstImage_t          *pstDst           Output result.
*                  MI_IVE_ErodeCtrl_t         *pstErodeCtrl     Control parameters
*                  MI_BOOL                     bInstant         Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*                  The input value, output value, and mask value must be 0 or 255.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Erode(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_ErodeCtrl_t *pstErodeCtrl,MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Thresh
*   Description  : Thresh operation to the input image.
*   Parameters   : MI_IVE_HANDLE           hHandle       Handle ID of a task
*                  MI_IVE_SrcImage_t      *pstSrc           Input source data. Only the U8C1 input format is supported.
*                  MI_IVE_DstImage_t      *pstDst           Output result
*                  MI_IVE_ThreshCtrl_t    *pstThrCtrl    Control parameters
*                  MI_BOOL                 bInstant         Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Thresh(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_ThreshCtrl_t *pstThrCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_And
*   Description  : Binary images' And operation.
*   Parameters   : MI_IVE_HANDLE          hHandle       Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1          The input source1. Only U8C1 input format is supported.
*                  MI_IVE_SrcImage_t     *pstSrc2          The input source2.Only U8C1 input format is supported.
*                  MI_IVE_DstImage_t     *pstDst           Output result of " src1 & src2 ".
*                  MI_BOOL              bInstant         Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_And(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1,
    MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstDst, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Sub
*   Description  : Two gray images' Sub operation.
*   Parameters   : MI_IVE_HANDLE          hHandle   Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1      Minuend of the input source.Only the U8C1 input format is supported.
*                  MI_IVE_SrcImage_t     *pstSrc2      Subtrahend of the input source.Only the U8C1 input format is supported.
*                  MI_IVE_DstImage_t     *pstDst       Output result of src1 minus src2
*                  MI_IVE_SubCtrl_t      *pstSubCtrl   Control parameter
*                  MI_BOOL              bInstant     Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Sub(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1,
    MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstDst, MI_IVE_SubCtrl_t *pstSubCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Or
*   Description  : Two binary images' Or operation.
*   Parameters   : MI_IVE_HANDLE          hHandle    Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1       Input source1. Only the U8C1 input format is supported.
*                  MI_IVE_SrcImage_t     *pstSrc2       Input source2. Only the U8C1 input format is supported.
*                  MI_IVE_DstImage_t     *pstDst        Output result src1 or src2
*                  MI_BOOL              bInstant        Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Or(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1,
    MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstDst, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_INTEG
*   Description  : Calculate the input gray image's integral image.
*   Parameters   : MI_IVE_HANDLE           hHandle        Handle ID of a task
*                  MI_IVE_SrcImage_t      *pstSrc            Input source data.Only the U8C1 input format is supported.
*                  MI_IVE_DstImage_t      *pstDst            Output result.Can be U32C1 or U64C1, relied on the control parameter.
*                  MI_IVE_IntegCtrl_t     *pstIntegCtrl      Integ Control
*                  MI_BOOL                  bInstant          Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*                  The pixel can be 32bit or 64 bit relied on the control parameter.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Integ(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_IntegCtrl_t *pstIntegCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Hist
*   Description  : Calculate the input gray image's histogram.
*   Parameters   : MI_IVE_HANDLE          hHandle      Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc          Input source data. Only the U8C1 input format is supported.
*                  MI_IVE_DstMemInfo_t  *pstDst          Output result.
*                  MI_BOOL              bInstant        Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Hist(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstMemInfo_t *pstDst, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_ThreshS16
*   Description  : S16 image's THRESH operation.
*   Parameters   : MI_IVE_HANDLE              hHandle             Handle ID of a task
*                  MI_IVE_SrcImage_t         *pstSrc                 Input source data.Only the S16 input format is supported.
*                  MI_IVE_DstImage_t         *pstDst                 Output result.
*                  MI_IVE_ThreshS16Ctrl_t   *pstThrS16Ctrl          Control parameters
*                  MI_BOOL                  bInstant               Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data must be 2-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_ThreshS16(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_ThreshS16Ctrl_t *pstThrS16Ctrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_ThreshU16
*   Description  : U16 image's THRESH operation.
*   Parameters   : MI_IVE_HANDLE                hHandle        Handle ID of a task
*                  MI_IVE_SrcImage_t           *pstSrc            Input source data. Only the U16 input format is supported.
*                  MI_IVE_DstImage_t           *pstDst            Output result
*                  MI_IVE_ThreshU16Ctrl_t     *pstThrU16Ctrl     Control parameters
*                  MI_BOOL                    bInstant          Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data must be 2-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_ThreshU16(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_ThreshU16Ctrl_t *pstThrU16Ctrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_16BitTo8Bit
*   Description  : Scale the input 16bit data to the output 8bit data.
*   Parameters   : MI_IVE_HANDLE                hHandle              Handle ID of a task
*                  MI_IVE_SrcImage_t           *pstSrc                  Input source data.Only U16C1\S16C1 input is supported.
*                  MI_IVE_DstImage_t           *pstDst                  Output result
*                  MI_IVE_16BITTO8BIT_CTRL_S    *pst16BitTo8BitCtrl      control parameter
*                  MI_BOOL                    bInstant                Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data must be 2-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_16BitTo8Bit(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_16bitTo8BitCtrl_t *pst16BitTo8BitCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_OrdStatFilter
*   Description  : Order Statistic Filter. It can be used as median\max\min value filter.
*   Parameters   : MI_IVE_HANDLE                   hHandle          Handle ID of a task
*                  MI_IVE_SrcImage_t              *pstSrc              Input source data. Only U8C1 input is supported
*                  MI_IVE_DstImage_t              *pstDst              Output result
*                  MI_IVE_OrdStatFilter_t   *pstOrdStatFltCtrl   Control parameter
*                  MI_BOOL                         bInstant            Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_OrdStatFilter(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_OrdStatFilter_t *pstOrdStatFltCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Map
*   Description  : Map a image to another through a lookup table.
*   Parameters   : MI_IVE_HANDLE           hHandle  Handle ID of a task
*                  MI_IVE_SrcImage_t      *pstSrc          Input source. Only the U8C1 input format is supported.
*                  MI_IVE_SrcMemInfo_t   *pstMap          Input lookup table. Must be an U8 array of size 256.
*                  MI_IVE_DstImage_t      *pstDst          Output result.
*                  MI_BOOL                  bInstant        Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Map(MI_IVE_HANDLE hHandle,MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_SrcMemInfo_t *pstMap, MI_IVE_DstImage_t *pstDst,MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_EqualizeHist
*   Description  : Enhance the input image's contrast through histogram equalization.
*   Parameters   : MI_IVE_HANDLE                 hHandle              Handle ID of a task
*                  MI_IVE_SrcImage_t            *pstSrc                  Input source.Only U8C1 input format is supported.
*                  MI_IVE_DstImage_t            *pstDst                  Output result.
*                  MI_IVE_EQUALIZEHIST_CTRL_S    *pstEqualizeHistCtrl     EqualizeHist control parameter.
*                  MI_BOOL                     bInstant                Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of map data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_EqualizeHist(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MI_IVE_EqualizeHistCtrl_t *pstEqualizeHistCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Add
*   Description  : Two gray images' Add operation.
*   Parameters   : MI_IVE_HANDLE          hHandle      Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1         Augend of the input source.Only the U8C1 input format is supported.
*                  MI_IVE_SrcImage_t     *pstSrc2         Addend of the input source.Only the U8C1 input format is supported.
*                  MI_IVE_DstImage_t     *pstDst          Output result of src1 plus src2
*                  MI_IVE_AddCtrl_t      *pstAddCtrl      Control parameter
*                  MI_BOOL                bInstant        Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Add(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1,
    MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstDst, MI_IVE_AddCtrl_t *pstAddCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Xor
*   Description  : Two binary images' Xor operation.
*   Parameters   : MI_IVE_HANDLE           hHandle    Handle ID of a task
*                  MI_IVE_SrcImage_t      *pstSrc1       The input source1.Only the U8C1 input format is supported.
*                  MI_IVE_SrcImage_t      *pstSrc2       The input source2.
*                  MI_IVE_DstImage_t      *pstDst        Output result
*                  MI_BOOL                  bInstant      Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Xor(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1,
    MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstDst, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Ncc
*   Description  : Calculate two gray images' NCC (Normalized Cross Correlation).
*   Parameters   : MI_IVE_HANDLE            hHandle     Handle ID of a task
*                  MI_IVE_SrcImage_t       *pstSrc1        Input source1. Only the U8C1 input format is supported.
*                  MI_IVE_SrcImage_t       *pstSrc2        Input source2. Must be of the same type¡¢size of source1.
*                  MI_IVE_DstMemInfo_t    *pstDst         Output result
*                  MI_BOOL                  bInstant       Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Ncc(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1,
    MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstMemInfo_t *pstDst, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Ccl
*   Description  : Connected Component Labeling. Only 8-Connected method is supported.
*   Parameters   : MI_IVE_HANDLE         hHandle      Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrcDst       Input source
*                  MI_IVE_DstMemInfo_t   *pstBlob         Output result of detected region;
*                  MI_IVE_CclCtrl_t      *pstCclCtrl      CCL control parameter
*                  MI_BOOL               bInstant        Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 720x640 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Ccl(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrcDst,
    MI_IVE_DstMemInfo_t *pstBlob, MI_IVE_CclCtrl_t *pstCclCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Gmm
*   Description  : Separate foreground and background using GMM(Gaussian Mixture Model) method;
*                  Gray or RGB GMM are supported.
*   Parameters   : MI_IVE_HANDLE          hHandle  Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc       Input source. Only support U8C1 or U8C3_PACKAGE input.
*                  MI_IVE_DstImage_t     *pstFg        Output foreground (Binary) image.
*                  MI_IVE_DstImage_t     *pstBg        Output background image. Of the sampe type of pstSrc.
*                  MI_IVE_MemInfo_t      *pstModel     Model data.
*                  MI_IVE_GmmCtrl_t      *pstGmmCtrl   Control parameter.
*                  MI_BOOL                bInstant     Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 720x576 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Gmm(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc, MI_IVE_DstImage_t *pstFg,
    MI_IVE_DstImage_t *pstBg, MI_IVE_MemInfo_t *pstModel, MI_IVE_GmmCtrl_t *pstGmmCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_CannyHysEdge
*   Description  : The first part of canny Edge detection. Including step: gradient calculation,
*                  magnitude and angle calculation, hysteresis threshold, NMS(Non-Maximum Suppression)
*   Parameters   : MI_IVE_HANDLE           hHandle        Handle ID of a task
*                  MI_IVE_SrcImage_t      *pstSrc            Input source. Only the U8C1 input format is supported
*                  MI_IVE_DstImage_t      *pstEdge           Output result.
*                  MI_IVE_DstMemInfo_t   *pstStack          OutPut stack for CannyEdge
*                  MI_IVE_CannyHysEdgeCtrl_t     *pstCannyHysEdgeCtrl      Control parameter.
*                  MI_BOOL                  bInstant          Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned. .
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_CannyHysEdge(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc, MI_IVE_DstImage_t *pstEdge,
       MI_IVE_DstMemInfo_t *pstStack, MI_IVE_CannyHysEdgeCtrl_t *pstCannyHysEdgeCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_CannyEdge
*   Description  : The second part of canny Edge detection: trace strong edge by weak edge.
*   Parameters   : MI_IVE_SrcImage_t      *pstEdge        Input and Output source. Only the U8C1 format is supported
*                  MI_IVE_MemInfo_t   *pstStack       stack for CannyEdge
*                  MI_BOOL             bInstant       Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_CannyEdge(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstEdge, MI_IVE_MemInfo_t *pstStack, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Lbp
*   Description  : LBP calculation using the original method and a extensional method.
*   Parameters   : MI_IVE_HANDLE         hHandle     Handle ID of a task
*                  MI_IVE_SrcImage_t    *pstSrc         Input source.Only the U8C1 inpu format is supported.
*                  MI_IVE_DstImage_t    *pstDst         Output result
*                  MI_IVE_LbpCtrrl_t     *pstLbpCtrl     Control parameter
*                  MI_BOOL             bInstant       Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Lbp(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2,
    MI_IVE_DstImage_t *pstDst, MI_IVE_LbpCtrrl_t *pstLbpCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_NormGrad
*   Description  : Gradient calculation and the output is normalized to S8.
*   Parameters   : MI_IVE_HANDLE             hHandle        Handle ID of a task
*                  MI_IVE_SrcImage_t        *pstSrc            Input source data
*                  MI_IVE_DstImage_t        *pstDstH           The (horizontal) result of input image filtered by the input mask;
*                  MI_IVE_DstImage_t        *pstDstV           The (vertical) result  of input image filtered by the transposed mask;
*                  MI_IVE_DstImage_t        *pstDstHV          Output the horizontal and vertical component in single image in package format.
*                  MI_IVE_NormGradCtrl_t   *pstNormGradCtrl   Control parameter
*                  MI_BOOL                 bInstant          Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_NormGrad(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDstH, MI_IVE_DstImage_t *pstDstV, MI_IVE_DstImage_t *pstDstHV,
    MI_IVE_NormGradCtrl_t *pstNormGradCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_LkOpticalFlow
*   Description  : Calculate LK-Optical Flow in single-layer of the pyramid.
*   Parameters   : MI_IVE_HANDLE                  hHandle          Handle ID of a task
*                  MI_IVE_SrcImage_t             *pstSrcPre           Pre-frame input source.Must be U8C1 image
*                  MI_IVE_SrcImage_t             *pstSrcCur         Cur-frame input source.Same size¡¢type with pstPreSrc.
*                  MI_IVE_SrcMemInfo_t          *pstPoint           Intresting points coordinates in the cur-layer
*                                                                   for LKOpticalFlow tracking.
*                  MI_IVE_MemInfo_t              *pstMv               Accumlative movements of the interesting points in pre-layers
*                                                                   or init 0s for the first-layer as input.                                                                init 0s .
*                                                                   Movements of the interesting points being tracked in cur-layer
*                                                                   as output.
*                  MI_IVE_LKOPTICALFLOW_CTRL_S    *pstLkOptiFlowCtrl   Control parameters.
*                  MI_BOOL                      bInstant            Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 720x576 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
****************************************************************************/
MI_S32 MI_IVE_LkOpticalFlow(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrcPre, MI_IVE_SrcImage_t *pstSrcCur,
    MI_IVE_SrcMemInfo_t *pstPoint, MI_IVE_MemInfo_t *pstMv, MI_IVE_LkOpticalFlowCtrl_t *pstLkOptiFlowCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Sad
*   Description  : Sum of absolute differences.
*   Parameters   : MI_IVE_HANDLE          hHandle   Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1      The input source1.Only the U8C1 input format is supported.
*                  MI_IVE_SrcImage_t     *pstSrc2      The input source2.Only the U8C1 input format is supported.
*                  MI_IVE_DstImage_t     *pstSad       Output result of sad value.Only the U8C1/U16C1 format is supported.
*                  MI_IVE_DstImage_t     *pstThr       Output result of thresh.Only the U8C1 format is supported.
*                  MI_IVE_SadCtrl_t        *pstSadCtrl   Control parameter
*                  MI_BOOL             bInstant      Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*                    Not support in hi3516a
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Sad(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1,
    MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstSad, MI_IVE_DstImage_t *pstThr,
    MI_IVE_SadCtrl_t *pstSadCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Resize
*   Description  : Resize image
*   Parameters   : MI_IVE_HANDLE          hHandle   Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc                 Input source.
*                  MI_IVE_DstImage_t     *pstDst                Output result
*                  MVE_IVE_ResizeCtrl_t  *pstResizeCtrl      Control parameters.
*                  MI_BOOL             bInstant      Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec     : The size of the input data ranges from 32x12 pixels to 1920x1080 pixels.
*                  The input and output height must be 2-pixel-aligned with mode YUV420SP.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Resize(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst, MVE_IVE_ResizeCtrl_t *pstResizeCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Bernsen
*   Description  :
*   Parameters : MI_IVE_HANDLE                hHandle                 Handle ID of a task
*                        MI_IVE_SrcImage_t          *pstSrc                  Input source.
*                        MI_IVE_DstImage_t          *pstDst                  Output result
*                        MVE_IVE_BernsenCtrl_t    *pstBernsenCtrl      Control parameters.
*                        MI_BOOL                          bInstant                 Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec       :     The physical addresses of the input data and output data must be 16-byte-aligned.
*                        The stride must be 16-pixel-aligned.
*                        The type of source  and destination must be U8C1.
*                        The input size and output size must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Bernsen(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstImage_t *pstDst,MVE_IVE_BernsenCtrl_t *pstBernsenCtrl,MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_AdpThresh
*   Description  :
*   Parameters : MI_IVE_HANDLE                hHandle                 Handle ID of a task
*                        MI_IVE_SrcImage_t          *pstSrc                  Input source.
*                        MI_IVE_SrcImage_t          *pstInteg               Input integral image of source.
*                        MI_IVE_DstImage_t          *pstDst                  Output result
*                        MVE_IVE_AdpThreshCtrl_t    *pstAdpThrCtrl      Control parameters.
*                        MI_BOOL                          bInstant                 Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec       :     The physical addresses of the input data and output data must be 16-byte-aligned.
*                        The type of source  and destination must be U8C1.
*                        The stride must be 16-pixel-aligned.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_AdpThresh(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_SrcImage_t *pstInteg, MI_IVE_DstImage_t *pstDst, MVE_IVE_AdpThreshCtrl_t *pstAdpThrCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_LineFilterHor
*   Description  :
*   Parameters : MI_IVE_HANDLE                      hHandle                         Handle ID of a task
*                        MI_IVE_SrcImage_t                *pstSrcDst                     Input and output source.
*                        MVE_IVE_LineFilterHorCtrl_t    *pstLineFilterHorCtrl       Control parameters.
*                        MI_BOOL                                bInstant                         Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         :   The type of input source must be U8C1.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_LineFilterHor(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrcDst,
    MVE_IVE_LineFilterHorCtrl_t *pstLineFilterHorCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_LineFilterVer
*   Description  :
*   Parameters : MI_IVE_HANDLE                      hHandle                         Handle ID of a task
*                        MI_IVE_SrcImage_t                *pstSrcDst                     Input and output source.
*                        MVE_IVE_LineFilterVerCtrl_t    *pstLineFilterVerCtrl        Control parameters.
*                        MI_BOOL                                bInstant                         Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         :  The type of input source must be U8C1.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_LineFilterVer(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrcDst,
    MVE_IVE_LineFilterVerCtrl_t *pstLineFilterVerCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_NoiseRemoveHor
*   Description  :
*   Parameters : MI_IVE_HANDLE                              hHandle                                  Handle ID of a task
*                        MI_IVE_SrcImage_t                        *pstSrcDst                              Input and output source.
*                        MVE_IVE_NoiseRemoveHorCtrl_t    *pstNoiseRemoveHorCtrl        Control parameters.
*                        MI_BOOL                                        bInstant                                  Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         :   The type of input source must be U8C1.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_NoiseRemoveHor(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrcDst,
    MVE_IVE_NoiseRemoveHorCtrl_t *pstNoiseRemoveHorCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_NoiseRemoveVer
*   Description  :
*   Parameters : MI_IVE_HANDLE                              hHandle                                  Handle ID of a task
*                        MI_IVE_SrcImage_t                        *pstSrcDst                              Input and output source.
*                        MVE_IVE_NoiseRemoveVerCtrl_t     *pstNoiseRemoveVerCtrl         Control parameters.
*                        MI_BOOL                                        bInstant                                  Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         :   The type of input source must be U8C1.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_NoiseRemoveVer(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrcDst,
    MVE_IVE_NoiseRemoveVerCtrl_t *pstNoiseRemoveVerCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Acc
*   Description  :
*   Parameters : MI_IVE_HANDLE                              hHandle                                Handle ID of a task
*                        MI_IVE_SrcImage_t                        *pstSrc0                              First input source.
*                        MI_IVE_SrcImage_t                        *pstSrc1                              Second input source.
*                        MVE_IVE_AccCtrl_t                         *pstAccCtrl                          Control parameters.
*                        MI_BOOL                                        bInstant                               Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec       :     The physical addresses of the input data and output data must be 16-byte-aligned.
*                        The stride must be 16-pixel-aligned.
*                        The size of two input source must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Acc(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc0,
    MI_IVE_SrcImage_t *pstSrc1, MI_IVE_DstImage_t *pstDst, MVE_IVE_AccCtrl_t *pstAccCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_MVE_BAT
*   Description  :
*   Parameters : MI_IVE_HANDLE                              hHandle                                Handle ID of a task
*                        MI_IVE_SrcImage_t                        *pstSrc                                 Input source.
*                        MI_IVE_DstMemInfo_t                    *pstDstH                              First output.
*                        MI_IVE_DstMemInfo_t                    *pstDstV                              Second output.
*                        MVE_IVE_BatCtrl_t                         *pstCtrl                                Control parameters.
*                        MI_BOOL                                        bInstant                               Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec       :     The physical addresses of the input data and output data must be 16-byte-aligned.
*                       The stride must be 16-pixel-aligned.
*                       The type of input source must be U8C1.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_BAT(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc,
    MI_IVE_DstMemInfo_t *pstDstH, MI_IVE_DstMemInfo_t *pstDstV, MVE_IVE_BatCtrl_t *pstCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Matrix_Transform
*   Description  : Matrix transform operation.
*   Parameters   : MI_IVE_HANDLE          hHandle   Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1      Input source 1.
*                  MI_IVE_SrcImage_t     *pstSrc2      Input source 2.
*                  MI_IVE_SrcImage_t     *pstSrc3      Input source 3.
*                  MI_IVE_DstImage_t     *pstDst1       Output result 1
*                  MI_IVE_DstImage_t     *pstDst2       Output result 2
*                  MI_IVE_DstImage_t     *pstDst3       Output result 3
*                  MI_IVE_MatrTranfCtrl_t      *pstMatrTranfCtrl   Control parameter
*                  MI_BOOL              bInstant     Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Matrix_Transform(MI_IVE_HANDLE hHandle,
    MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2, MI_IVE_SrcImage_t *pstSrc3,
    MI_IVE_DstImage_t *pstDst1, MI_IVE_DstImage_t *pstDst2, MI_IVE_DstImage_t *pstDst3,
    MI_IVE_MatrTranfCtrl_t *pstMatrTranfCtrl, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Image_Dot
*   Description  : Matrix transform operation.
*   Parameters   : MI_IVE_HANDLE          hHandle   Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1      Input source 1.
*                  MI_IVE_SrcImage_t     *pstSrc2      Input source 2.
*                  MI_IVE_DstImage_t     *pstDst       Output result.
*                  MI_BOOL              bInstant     Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Image_Dot(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2,
    MI_IVE_DstImage_t *pstDst, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_AlphaBlending
*   Description  : Matrix transform operation.
*   Parameters   : MI_IVE_HANDLE          hHandle      Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1      Input source 1.
*                  MI_IVE_SrcImage_t     *pstSrc2      Input source 2.
*                  MI_IVE_SrcImage_t     *pstAlpha     Input alpha table.
*                  MI_IVE_DstImage_t     *pstDst1      Output result
*                  MI_BOOL               bInstant      Reserved
*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_AlphaBlending(MI_IVE_HANDLE hHandle,
     MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2, MI_IVE_SrcImage_t *pstAlpha,
     MI_IVE_DstImage_t *pstDst, MI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : MI_IVE_Shift_Detector
*   Description  : Matrix transform operation.
*   Parameters   : MI_IVE_HANDLE          hHandle   Handle ID of a task
*                  MI_IVE_SrcImage_t     *pstSrc1      Input source 1.
*                  MI_IVE_SrcImage_t     *pstSrc2      Input source 2.
*                  MI_IVE_DstImage_t     *pstDstX      Output result 1.
*                  MI_IVE_DstImage_t     *pstDstY      Output result 2.
*                  MI_IVE_SHIFT_DETECT_CTRL_t    *pstCtrl    Control parameters.
*                  MI_BOOL              bInstant     Reserved

*   Return Value : MI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of input sources must be the same.
*   History:
*****************************************************************************/
MI_S32 MI_IVE_Shift_Detector(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstDstX, MI_IVE_DstImage_t *pstDstY, MI_IVE_SHIFT_DETECT_CTRL_t *pstCtrl, MI_BOOL bInstant);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_MI_IVE_H_*/
