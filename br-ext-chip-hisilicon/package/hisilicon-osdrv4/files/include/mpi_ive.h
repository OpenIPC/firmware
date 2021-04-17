/******************************************************************************

  Copyright (C), 2001-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_ive.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (IVE) group
  Created       : 2011/05/16
  Description   :
  1.Date        : 2011/05/16
    Modification: Created file

  2.Date        : 2013/07/01~2014/08/08
    Modification: Add MPI function
******************************************************************************/
#ifndef _HI_MPI_IVE_H_
#define _HI_MPI_IVE_H_

#include "hi_ive.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_DMA
*   Description  : Direct memory access (DMA):
*                  1.Direct memory copy;
* 				     2. Copy with interval bytes;
*                  3. Memset using 3 bytes;
* 				     4. Memset using 8 bytes;
*   Parameters   : IVE_HANDLE          *pIveHandle        Returned handle ID of a task.
*                  IVE_DATA_S          *pstSrc            Input source data.The input data is treated as U8C1 data.
* 				     IVE_DATA_S          *pstDst            Output result data.
*                  IVE_DMA_CTRL_S      *pstDmaCtrl        DMA control parameter.
*                  HI_BOOL              bInstant          Flag indicating whether to generate an interrupt.
*                                                         If the output result blocks the next operation,
*                                                         set bInstant to HI_TRUE.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 32x1 pixels to 1920x1080 pixels.
*                  The stride must be 16-byte-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
* 		2.  Data		 : 2013-07-19
* 			Author		 :
* 			Modification : Modify parameters
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_DMA(IVE_HANDLE *pIveHandle, IVE_DATA_S *pstSrc, IVE_DST_DATA_S *pstDst, 
    IVE_DMA_CTRL_S *pstDmaCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Filter
*   Description  : 5x5 template filter.
*   Parameters   : IVE_HANDLE            *pIveHandle         Returned handle ID of a task
*                  IVE_SRC_IMAGE_S       *pstSrc             Input source data.
*                                                            The U8C1,SP420 and SP422 input formats are supported.
*                  IVE_DST_IMAGE_S       *pstDst             Output result, of same type with the input.
*                  IVE_FILTER_CTRL_S     *pstFltCtrl         Control parameters of filter
*                  HI_BOOL               bInstant            For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-07-23
*           Author       :
*           Modification : Modified function parameters

*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Filter(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_FILTER_CTRL_S *pstFltCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CSC
*   Description  : YUV2RGB\YUV2HSV\YUV2LAB\RGB2YUV color space conversion are supported.
*   Parameters   : IVE_HANDLE         *pIveHandle       Returned handle ID of a task
*                  IVE_SRC_IMAGE_S    *pstSrc           Input source data:
*                                                       1. SP420\SP422 type for YUV2RGB\YUV2HSV\YUV2LAB;
*                                                       2. U8C3_PACKAGE\U8C3_PLANAR type for RGB2YUV;
*                  IVE_DST_IMAGE_S    *pstDst           Output result:
*                                                       1. U8C3_PACKAGE\U8C3_PLANAR typed for YUV2RGB\YUV2HSV\YUV2LAB;
*                                                       2. SP420\SP422 type for RGB2YUV;
*                  IVE_CSC_CTRL_S     *pstCscCtrl       Control parameters for CSC
*                  HI_BOOL             bInstant         For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-08-09
*           Author       :
*           Modification : Modified function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CSC(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_CSC_CTRL_S *pstCscCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_FILTER_AND_CSC
*   Description  : Only support YUV2RGB color space conversion.
*   Parameters   : IVE_HANDLE                 *pIveHandle        Returned handle ID of a task.
*                  IVE_SRC_IMAGE_S            *pstSrc            Input source data.Only SP420\SP422 type are supported.
*                  IVE_DST_IMAGE_S            *pstDst            Output result.Only U8C3_PACKAGE\U8C3_PLANAR are supported.
*                  IVE_FILTER_AND_CSC_CTRL_S  *pstFltCscCtrl     Control parameters.
*                  HI_BOOL                     bInstant          For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-08-09
*           Author       :
*           Modification : Modified function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_FilterAndCSC(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_FILTER_AND_CSC_CTRL_S *pstFltCscCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Sobel
*   Description  : SOBEL is used to extract the gradient information.
*   Parameters   : IVE_HANDLE           *pIveHandle      Returned handle ID of a task
*                  IVE_SRC_IMAGE_S      *pstSrc          Input source data. Only the U8C1 input image is supported.
*                  IVE_DST_IMAGE_S      *pstDstH         The (horizontal) result of input image filtered by the input mask;
*                  IVE_DST_IMAGE_S      *pstDstV         The (vertical) result  of input image filtered by the transposed mask;
*                  IVE_SOBEL_CTRL_S     *pstSobelCtrl    Control parameters
*                  HI_BOOL               bInstant        For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-07-23
*           Author       :
*           Modification : Modified function parameters
*
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Sobel(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstH, 
    IVE_DST_IMAGE_S *pstDstV, IVE_SOBEL_CTRL_S *pstSobelCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_MagAndAng
*   Description  : MagAndAng is used to extract the edge information.
*   Parameters   : IVE_HANDLE              *pIveHandle         Returned handle ID of a task
*                  IVE_SRC_INFO_S          *pstSrc             Input source data. Only the U8C1 input format is supported.
*                  IVE_MEM_INFO_S          *pstDstMag          Output magnitude.
*                  IVE_MEM_INFO_S          *pstDstAng          Output angle.
*                                                              If the output mode is set to magnitude only,
*                                                              this item can be set to null.
*                  IVE_MAG_AND_ANG_CTRL_S  *pstMagAndAngCtrl   Control parameters
*                  HI_BOOL                  bInstant           For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
* 		2.  Date         : 2013-07-17
* 			Author       :
* 			Modification : Modified function and control parameter name
* 		3.  Date         : 2013-07-23
* 			Author       :
* 			Modification : Modified function parameters
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_MagAndAng(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstMag, 
    IVE_DST_IMAGE_S *pstDstAng, IVE_MAG_AND_ANG_CTRL_S *pstMagAndAngCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Dilate
*   Description  : 5x5 template dilate. Only the U8C1 binary image input is supported.Or else the result is not expected.
*   Parameters   : IVE_HANDLE             *pIveHandle          Returned handle ID of a task
*                  IVE_SRC_IMAGE_S        *pstSrc              Input binary image, which consists of 0 or 255;
*                  IVE_DST_IMAGE_S        *pstDst              Output result.
*                  IVE_DILATE_CTRL_S      *pstDilateCtrl       Control parameters.
*                  HI_BOOL                 bInstant            For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*                  The input value, output value, and mask value must be 0 or 255.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-07-23
*           Author       :
*           Modification : Modified parameters
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Dilate(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_DILATE_CTRL_S *pstDilateCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Erode
*   Parameters   : 5x5 template erode. Only the U8C1 binary image input is supported.Or else the result is not correct.
*   Input        : IVE_HANDLE               *pIveHandle       Returned handle ID of a task
*                  IVE_SRC_IMAGE_S          *pstSrc           Input binary image, which consists of 0 or 255;
*                  IVE_DST_IMAGE_S          *pstDst           Output result.
*                  IVE_ERODE_CTRL_S         *pstErodeCtrl     Control parameters
*                  HI_BOOL                   bInstant         For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*                  The input value, output value, and mask value must be 0 or 255.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-07-23
*           Author       :
*           Modification : Modified parameters
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Erode(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_ERODE_CTRL_S *pstErodeCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Thresh
*   Description  : Thresh operation to the input image.
*   Parameters   : IVE_HANDLE           *pIveHandle       Returned handle ID of a task
*                  IVE_SRC_IMAGE_S      *pstSrc           Input source data. Only the U8C1 input format is supported.
*                  IVE_DST_IMAGE_S      *pstDst           Output result
*                  IVE_THRESH_CTRL_S    *pstThrCtrl    Control parameters
*                  HI_BOOL               bInstant         For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
* 		2.   Date         : 2013-07-23
*           Author		:
* 			 Modification  : Modification
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Thresh(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_THRESH_CTRL_S *pstThrCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_And
*   Description  : Binary images' And operation.
*   Parameters   : IVE_HANDLE          *pIveHandle       Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc1          The input source1. Only U8C1 input format is supported.
*                  IVE_SRC_IMAGE_S     *pstSrc2          The input source2.Only U8C1 input format is supported.
*                  IVE_DST_IMAGE_S     *pstDst           Output result of " src1 & src2 ".
*                  HI_BOOL              bInstant         For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_And(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, 
    IVE_DST_IMAGE_S *pstDst, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Sub
*   Description  : Two gray images' Sub operation.
*   Parameters   : IVE_HANDLE          *pIveHandle   Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc1      Minuend of the input source.Only the U8C1 input format is supported.
*                  IVE_SRC_IMAGE_S     *pstSrc2      Subtrahend of the input source.Only the U8C1 input format is supported.
*                  IVE_DST_IMAGE_S     *pstDst       Output result of src1 minus src2
*                  IVE_SUB_CTRL_S      *pstSubCtrl   Control parameter
*                  HI_BOOL              bInstant     For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-08-09
*           Author       :
*           Modification : Modified function parameter
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Sub(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, 
    IVE_DST_IMAGE_S *pstDst, IVE_SUB_CTRL_S *pstSubCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Or
*   Description  : Two binary images' Or operation.
*   Parameters   : IVE_HANDLE          *pIveHandle    Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc1       Input source1. Only the U8C1 input format is supported.
*                  IVE_SRC_IMAGE_S     *pstSrc2       Input source2. Only the U8C1 input format is supported.
*                  IVE_DST_IMAGE_S     *pstDst        Output result src1 or src2
*                  HI_BOOL              bInstant        For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-08-09
*           Author       :
*           Modification : Modified function parameter
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Or(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, 
    IVE_DST_IMAGE_S *pstDst, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_INTEG
*   Description  : Calculate the input gray image's integral image.
*   Parameters   : IVE_HANDLE           *pIveHandle        Returned handle ID of a task
*                  IVE_SRC_IMAGE_S      *pstSrc            Input source data.Only the U8C1 input format is supported.
*                  IVE_DST_IMAGE_S      *pstDst            Output result.Can be U32C1 or U64C1, relied on the control parameter.
* 				   IVE_INTEG_CTRL_S     *pstIntegCtrl      Integ Control
*                  HI_BOOL               bInstant          For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*                  The pixel can be 32bit or 64 bit relied on the control parameter.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*       2.	 Date		   : 2013-03-18
* 			 Author		   :
* 			 Modification : Modified function
* 		3.  Date		   : 2013-07-15
* 		    Author		   :
* 			Modification : Modified function
* 			Spec		  : Modify IVE_INTEG_OUT_FMT_E to  IVE_INTEG_CTRL_S
*       4.	Date		  : 2013-07-23
* 			Author		  :
* 			Modification : Modified parameters
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Integ(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_INTEG_CTRL_S *pstIntegCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Hist
*   Description  : Calculate the input gray image's histogram.
*   Parameters   : IVE_HANDLE          *pIveHandle      Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc          Input source data. Only the U8C1 input format is supported.
*                  IVE_DST_MEM_INFO_S  *pstDst          Output result.
*                  HI_BOOL              bInstant        For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Hist(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_MEM_INFO_S *pstDst, 
    HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Thresh_S16
*   Description	 : S16 image's THRESH operation.
*   Parameters   : IVE_HANDLE              *pIveHandle             Returned handle ID of a task
*                  IVE_SRC_IMAGE_S         *pstSrc                 Input source data.Only the S16 input format is supported.
*                  IVE_DST_IMAGE_S         *pstDst                 Output result.
*                  IVE_THRESH_S16_CTRL_S   *pstThrS16Ctrl          Control parameters
*                  HI_BOOL                  bInstant               For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data must be 2-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-05-16
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Thresh_S16(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_THRESH_S16_CTRL_S *pstThrS16Ctrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Thresh_U16
*   Description	 : U16 image's THRESH operation.
*   Parameters   : IVE_HANDLE                *pIveHandle        Returned handle ID of a task
*                  IVE_SRC_IMAGE_S           *pstSrc            Input source data. Only the U16 input format is supported.
*                  IVE_DST_IMAGE_S           *pstDst            Output result
*                  IVE_THRESH_U16_CTRL_S     *pstThrU16Ctrl     Control parameters
*                  HI_BOOL                    bInstant          For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data must be 2-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-05-16
*           Author       :
*           Modification : Created function
*       2.  Date         : 2013-08-07
*           Author       :
*           Modification : Implement function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Thresh_U16(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_THRESH_U16_CTRL_S *pstThrU16Ctrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_16BitTo8Bit
*   Description  : Scale the input 16bit data to the output 8bit data.
*   Parameters   : IVE_HANDLE                *pIveHandle              Returned handle ID of a task
*                  IVE_SRC_IMAGE_S           *pstSrc                  Input source data.Only U16C1\S16C1 input is supported.
*                  IVE_DST_IMAGE_S           *pstDst                  Output result
* 				   IVE_16BITTO8BIT_CTRL_S    *pst16BitTo8BitCtrl      control parameter
*                  HI_BOOL                    bInstant                For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data must be 2-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-12
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_16BitTo8Bit(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_16BIT_TO_8BIT_CTRL_S *pst16BitTo8BitCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_OrdStatFilter
*   Description  : Order Statistic Filter. It can be used as median\max\min value filter.
*   Parameters   : IVE_HANDLE                   *pIveHandle          Returned handle ID of a task
*                  IVE_SRC_IMAGE_S              *pstSrc              Input source data. Only U8C1 input is supported
*                  IVE_DST_IMAGE_S              *pstDst              Output result
* 				   IVE_ORD_STAT_FILTER_CTRL_S   *pstOrdStatFltCtrl   Control parameter
*                  HI_BOOL                       bInstant            For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-12
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_OrdStatFilter(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_ORD_STAT_FILTER_CTRL_S *pstOrdStatFltCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Map
*   Description  : Map a image to another through a lookup table.
*   Parameters   : IVE_HANDLE           *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_IMAGE_S      *pstSrc          Input source. Only the U8C1 input format is supported.
*                  IVE_SRC_MEM_INFO_S   *pstMap          Input lookup table. Must be an U8 array of size 256.
*                  IVE_DST_IMAGE_S      *pstDst          Output result.
* 				   IVE_MAP_CTRL_S       *pstMapCtrl      Map control parameter.
*                  HI_BOOL               bInstant        For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-17
*           Author       :
*           Modification : Created function
*       2.  Date         : 2015-03-02
*           Author       :
*           Modification : Add s16/u16 map
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Map(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_MEM_INFO_S *pstMap, 
    IVE_DST_IMAGE_S *pstDst, IVE_MAP_CTRL_S *pstMapCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_EqualizeHist
*   Description  : Enhance the input image's contrast through histogram equalization.
*   Parameters   : IVE_HANDLE                 *pIveHandle              Returned handle ID of a task
*                  IVE_SRC_IMAGE_S            *pstSrc                  Input source.Only U8C1 input format is supported.
*                  IVE_DST_IMAGE_S            *pstDst                  Output result.
* 				   IVE_EQUALIZEHIST_CTRL_S    *pstEqualizeHistCtrl     EqualizeHist control parameter.
*                  HI_BOOL                     bInstant                For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of map data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-07-17
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_EqualizeHist(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_EQUALIZE_HIST_CTRL_S *pstEqualizeHistCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Add
*   Description  : Two gray images' Add operation.
*   Parameters   : IVE_HANDLE          *pIveHandle      Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc1         Augend of the input source.Only the U8C1 input format is supported.
*                  IVE_SRC_IMAGE_S     *pstSrc2         Addend of the input source.Only the U8C1 input format is supported.
*                  IVE_DST_IMAGE_S     *pstDst          Output result of src1 plus src2
* 				     IVE_ADD_CTRL_S      *pstAddCtrl      Control parameter
*                  HI_BOOL              bInstant        For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*
*       1.  Date         : 2013-07-17
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Add(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, 
    IVE_DST_IMAGE_S *pstDst, IVE_ADD_CTRL_S *pstAddCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Xor
*   Description  : Two binary images' Xor operation.
*   Parameters   : IVE_HANDLE           *pIveHandle    Returned handle ID of a task
*                  IVE_SRC_IMAGE_S      *pstSrc1       The input source1.Only the U8C1 input format is supported.
*                  IVE_SRC_IMAGE_S      *pstSrc2       The input source2.
*                  IVE_DST_IMAGE_S      *pstDst        Output result
*                  HI_BOOL               bInstant      For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*
*       1.  Date         : 2013-07-17
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Xor(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, 
    IVE_DST_IMAGE_S *pstDst, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_NCC
*   Description  : Calculate two gray images' NCC (Normalized Cross Correlation).
*   Parameters   : IVE_HANDLE            *pIveHandle     Returned handle ID of a task
*                  IVE_SRC_IMAGE_S       *pstSrc1        Input source1. Only the U8C1 input format is supported.
*                  IVE_SRC_IMAGE_S       *pstSrc2        Input source2. Must be of the same type, size of source1.
*                  IVE_DST_MEM_INFO_S    *pstDst         Output result
*                  HI_BOOL                bInstant       For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-16
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_NCC(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, 
    IVE_DST_MEM_INFO_S *pstDst, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CCL
*   Description  : Connected Component Labeling. Only 8-Connected method is supported.
*   Parameters   : IVE_HANDLE         *pIveHandle      Returned handle ID of a task
*                  IVE_IMAGE_S        *pstSrcDst       Input source
*                  IVE_MEM_INFO_S     *pstBlob         Output result of detected region;
* 				   IVE_CCL_CTRL_S     *pstCclCtrl      CCL control parameter
*                  HI_BOOL             bInstant        For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 720x640 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-1
*           Author       :
*           Modification : Created function
*****************************************************************************/
HI_S32 HI_MPI_IVE_CCL(IVE_HANDLE *pIveHandle, IVE_IMAGE_S *pstSrcDst, IVE_DST_MEM_INFO_S *pstBlob, 
    IVE_CCL_CTRL_S *pstCclCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_GMM
*   Description  : Separate foreground and background using GMM(Gaussian Mixture Model) method;
*                  Gray or RGB GMM are supported.
*   Parameters   : IVE_HANDLE          *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc       Input source. Only support U8C1 or U8C3_PACKAGE input.
* 				   IVE_DST_IMAGE_S     *pstFg        Output foreground (Binary) image.
* 				   IVE_DST_IMAGE_S     *pstBg        Output background image. Of the sampe type of pstSrc.
* 				   IVE_MEM_INFO_S      *pstModel     Model data.
* 				   IVE_GMM_CTRL_S      *pstGmmCtrl   Control parameter.
*                  HI_BOOL              bInstant     For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 720x576 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-07
*           Author       :
*           Modification : Created function
*****************************************************************************/
HI_S32 HI_MPI_IVE_GMM(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstFg,
    IVE_DST_IMAGE_S *pstBg, IVE_MEM_INFO_S *pstModel, IVE_GMM_CTRL_S *pstGmmCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_GMM2
*   Description  : Separate foreground and background using GMM(Gaussian Mixture Model) method;
*                  Gray or RGB GMM are supported.
*   Parameters   : IVE_HANDLE          *pIveHandle			Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc				Only U8C1 or U8C3_PACKAGE input are supported.
* 				   IVE_SRC_IMAGE_S	   *pstFactor			U16C1 input, low-8bits is sensitivity factor, and high-8bits is life update factor.
* 				   IVE_DST_IMAGE_S     *pstFg				Output foreground (Binary) image.
* 				   IVE_DST_IMAGE_S     *pstBg				Output background image. With same type of pstSrc.
* 				   IVE_DST_IMAGE_S     *pstMatchModelInfo	Output U8C1 match model info image. Low-1bit is match flag,and high-7bits is max freq index.
* 				   IVE_MEM_INFO_S      *pstModel			Model data.
* 				   IVE_GMM2_CTRL_S     *pstGmm2Ctrl			Control parameter.
*                  HI_BOOL              bInstant			For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1280x720 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2015-04-16
*           Author       :
*           Modification : Created function
*****************************************************************************/
HI_S32 HI_MPI_IVE_GMM2(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_IMAGE_S *pstFactor,
    IVE_DST_IMAGE_S *pstFg, IVE_DST_IMAGE_S *pstBg, IVE_DST_IMAGE_S *pstMatchModelInfo, IVE_MEM_INFO_S *pstModel, 
    IVE_GMM2_CTRL_S *pstGmm2Ctrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CannyHysEdge
*   Description  : The first part of canny Edge detection. Including step: gradient calculation,
*                  magnitude and angle calculation, hysteresis threshold, NMS(Non-Maximum Suppression)
*   Parameters   : IVE_HANDLE           *pIveHandle        Returned handle ID of a task
*                  IVE_SRC_IMAGE_S      *pstSrc            Input source. Only the U8C1 input format is supported
*                  IVE_DST_IMAGE_S      *pstEdge           Output result.
*                  IVE_DST_MEM_INFO_S   *pstStack          OutPut stack for CannyEdge
* 				   IVE_CANNY_HYS_EDGE_CTRL_S     *pstCannyHysEdgeCtrl      Control parameter.
*                  HI_BOOL               bInstant        For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned. .
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-12
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CannyHysEdge(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstEdge,
    IVE_DST_MEM_INFO_S *pstStack, IVE_CANNY_HYS_EDGE_CTRL_S *pstCannyHysEdgeCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CannyEdge
*   Description  : The second part of canny Edge detection: trace strong edge by weak edge.
*   Parameters   : IVE_IMAGE_S      *pstEdge        Input and Output source. Only the U8C1 format is supported
*                  IVE_MEM_INFO_S   *pstStack       stack for CannyEdge
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-12
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CannyEdge(IVE_IMAGE_S *pstEdge, IVE_MEM_INFO_S *pstStack);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_LBP
*   Description  : LBP calculation using the original method and a extensional method.
*   Parameters   : IVE_HANDLE         *pIveHandle     Returned handle ID of a task
*                  IVE_SRC_IMAGE_S    *pstSrc         Input source.Only the U8C1 inpu format is supported.
*                  IVE_DST_IMAGE_S    *pstDst         Output result
* 				   IVE_LBP_CTRL_S     *pstLbpCtrl     Control parameter
*                  HI_BOOL             bInstant       For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-09-22
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_LBP(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, 
    IVE_LBP_CTRL_S *pstLbpCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_NormGrad
*   Description  : Gradient calculation and the output is normalized to S8.
*   Parameters   : IVE_HANDLE             *pIveHandle        Returned handle ID of a task
*                  IVE_SRC_IMAGE_S        *pstSrc            Input source data
*                  IVE_DST_IMAGE_S        *pstDstH           The (horizontal) result of input image filtered by the input mask;
*                  IVE_DST_IMAGE_S        *pstDstV           The (vertical) result  of input image filtered by the transposed mask;
* 				   IVE_DST_IMAGE_S        *pstDstHV          Output the horizontal and vertical component in single image in package format.
* 				   IVE_NORM_GRAD_CTRL_S   *pstNormGradCtrl   Control parameter
*                  HI_BOOL                 bInstant          For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-08-12
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_NormGrad(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstH, 
    IVE_DST_IMAGE_S *pstDstV, IVE_DST_IMAGE_S *pstDstHV, IVE_NORM_GRAD_CTRL_S *pstNormGradCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_LKOpticalFlowPyr
*   Description  : Calculate LK Optical Flow using multi-layer of the pyramid-images.
*   Parameters   : IVE_HANDLE						*pIveHandle          Returned handle ID of a task
*                  IVE_SRC_IMAGE_S					astSrcPrevPyr        Prev-frame's pyramid. Must be U8C1 images.
* 				   IVE_SRC_IMAGE_S					astSrcNextPyr		 Next-frame's pyramid. Same size and type with astSrcPrePyr.
*                  IVE_SRC_MEM_INFO_S				*pstPrevPts		     Intresting points on astSrcPrePyr[0].
*                  IVE_MEM_INFO_S					*pstNextPts          Output points. When bUseInitFlow is true, must have the same
* 																		 size of pstPrevPts as input.
*                  IVE_LK_OPTICAL_FLOW_PYR_CTRL_S	*pstLkOptiFlowCtrl   Control parameters.
*                  HI_BOOL							bInstant             For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1280x720 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2015-03-20
*           Author       :
*           Modification : Created function
****************************************************************************/
HI_S32 HI_MPI_IVE_LKOpticalFlowPyr(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S astSrcPrevPyr[],
    IVE_SRC_IMAGE_S astSrcNextPyr[], IVE_SRC_MEM_INFO_S *pstPrevPts, IVE_MEM_INFO_S *pstNextPts, 
    IVE_DST_MEM_INFO_S *pstStatus, IVE_DST_MEM_INFO_S *pstErr, IVE_LK_OPTICAL_FLOW_PYR_CTRL_S *pstLkOptiFlowPyrCtrl, 
    HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_STCandiCorner
*   Description  : The first part of corners detection using Shi-Tomasi-like method: calculate candidate corners.
*   Parameters   : IVE_HANDLE              *pIveHandle         Returned handle ID of a task
*                  IVE_SRC_IMAGE_S         *pstSrc             Input source data
*                  IVE_DST_IMAGE_S         *pstCandiCorner     Output result of eig
* 				   IVE_ST_CANDI_CORNER_CTRL_S    *pstStCandiCornerCtrl   Control parameter
*                  HI_BOOL                  bInstant           For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 720x576 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-09-16
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_STCandiCorner(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstCandiCorner,
    IVE_ST_CANDI_CORNER_CTRL_S *pstStCandiCornerCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_STCorner
*   Description  : The second part of corners detection using Shi-Tomasi-like method: select corners by certain rules.
*                  IVE_SRC_IMAGE_S         *pstCandiCorner     Input source data
*                  IVE_DST_MEM_INFO_S *pstCorner    Output result of Corner
* 				   IVE_ST_CORNER_CTRL_S    *pstStCornerCtrl   Control parameter
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 720x576 pixels.
*                  The physical addresses of the input data and output data must be 16-byte-aligned.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2013-09-16
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_STCorner(IVE_SRC_IMAGE_S *pstCandiCorner, IVE_DST_MEM_INFO_S *pstCorner,
    IVE_ST_CORNER_CTRL_S *pstStCornerCtrl);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_GradFg
*   Description  :
*   Parameters   : IVE_HANDLE            *pIveHandle       Returned handle ID of a task
*                  IVE_SRC_IMAGE_S       *pstBgDiffFg      Background subtraction foreground image
*                  IVE_SRC_IMAGE_S       *pstCurGrad       Current gradient image, both horizontally and vertically
*                                                          graded in accordance with [xyxyxy ...] format
*                  IVE_SRC_IMAGE_S       *pstBgGrad        Background gradient image
*                  IVE_DST_IMAGE_S       *pstGradFg        Gradient foreground image
*                  IVE_GRAD_FG_CTRL_S    *pstGradFgCtrl    Gradient calculation parameters
*                  HI_BOOL                bInstant         For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2013-10-29
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_GradFg(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstBgDiffFg, IVE_SRC_IMAGE_S *pstCurGrad,
    IVE_SRC_IMAGE_S *pstBgGrad, IVE_DST_IMAGE_S *pstGradFg, IVE_GRAD_FG_CTRL_S *pstGradFgCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_MatchBgModel
*   Description  :
*   Parameters   : IVE_HANDLE                  *pIveHandle            Returned handle ID of a task
*                  IVE_SRC_IMAGE_S             *pstCurImg             Current grayscale image
*                  IVE_DATA_S                  *pstBgModel            Background model data
*                  IVE_IMAGE_S                 *pstFgFlag             Foreground status image
*                  IVE_DST_IMAGE_S             *pstBgDiffFg           Foreground image obtained by background matching,
*                                                                     the background pixel value is 0, the foreground pixel
*                                                                     value is the gray difference value
*                  IVE_DST_IMAGE_S             *pstFrmDiffFg          Foreground image obtained by interframe difference,
*                                                                     the background pixel value is 0, the foreground pixel
*                                                                     value is the gray difference value
*                  IVE_DST_MEM_INFO_S          *pstStatData           result status data
*                  IVE_MATCH_BG_MODEL_CTRL_S   *pstMatchBgModelCtrl   Background matching parameters
*                  HI_BOOL                      bInstant              For details, see HI_MPI_IVE_DMA.

*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2013-10-29
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_MatchBgModel(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstCurImg, IVE_DATA_S *pstBgModel,
    IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstBgDiffFg, IVE_DST_IMAGE_S *pstFrmDiffFg, 
    IVE_DST_MEM_INFO_S *pstStatData, IVE_MATCH_BG_MODEL_CTRL_S *pstMatchBgModelCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_UpdateBgModel
*   Description  :
*   Parameters   : IVE_HANDLE                  *pIveHandle              Returned handle ID of a task
*                  IVE_DATA_S                  *pstBgModel              Background model data
*                  IVE_IMAGE_S                 *pstFgFlag               Foreground status image
*                  IVE_DST_IMAGE_S             *pstBgImg                Background grayscale image
*                  IVE_DST_IMAGE_S             *pstChgStaImg            Change state life image, for still detection
*                  IVE_DST_IMAGE_S             *pstChgStaFg             Change state grayscale image, for still detection
*                  IVE_DST_IMAGE_S             *pstChgStaLife           Change state foreground image, for still detection
*                  IVE_DST_MEM_INFO_S          *pstStatData             result status data
*                  IVE_UPDATE_BG_MODEL_CTRL_S  *pstUpdateBgModelCtrl    Background update parameters
*                  HI_BOOL                      bInstant                For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2013-10-29
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_UpdateBgModel(IVE_HANDLE *pIveHandle, IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag,
    IVE_DST_IMAGE_S *pstBgImg, IVE_DST_IMAGE_S *pstChgStaImg, IVE_DST_IMAGE_S *pstChgStaFg,
    IVE_DST_IMAGE_S *pstChgStaLife, IVE_DST_MEM_INFO_S *pstStatData, IVE_UPDATE_BG_MODEL_CTRL_S *pstUpdateBgModelCtrl, 
    HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_ANN_MLP_LoadModel
*   Description  : Load ANN_MLP model data from ".bin" file.
*   Parameters   : HI_CHAR			        *pchFileName        ANN_MLP model file name, must be ".bin" file.
*                  IVE_ANN_MLP_MODEL_S      *pstAnnMlpModel	    ANN_MLP model data.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014-05-13
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_ANN_MLP_LoadModel(const HI_CHAR *pchFileName, IVE_ANN_MLP_MODEL_S *pstAnnMlpModel);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_ANN_MLP_UnloadModel
*   Description  : Unload ANN_MLP model data.
*   Parameters   : IVE_ANN_MLP_MODEL_S      *pstAnnMlpModel	    ANN_MLP model data.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014-05-13
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_VOID HI_MPI_IVE_ANN_MLP_UnloadModel(IVE_ANN_MLP_MODEL_S *pstAnnMlpModel);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_ANN_MLP_Predict
*   Description  :
*   Parameters   : IVE_HANDLE			   *pIveHandle          Returned handle ID of a task
* 				   IVE_SRC_DATA_S	       *pstSrc			    Input samples
*                  IVE_SRC_MEM_INFO_S      *pstActivFuncTable   Look-up talbe for active function
*                  IVE_ANN_MLP_MODEL_S     *pstAnnMlpModel	    ANN_MLP model
*                  IVE_DST_DATA_S          *pstDst              Output layers of every intput sample
*                  HI_BOOL				    bInstant            For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2013-11-28
*           Author       :
*           Modification : Created function
* 		2.  Date		 : 2015-05-21
* 			Author		 :
* 			Modification : support 1024d feature vector and array of vectors input
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_ANN_MLP_Predict(IVE_HANDLE *pIveHandle, IVE_SRC_DATA_S *pstSrc,
    IVE_LOOK_UP_TABLE_S *pstActivFuncTab, IVE_ANN_MLP_MODEL_S *pstAnnMlpModel, IVE_DST_DATA_S *pstDst, 
    HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_SVM_LoadModel
*   Description  : Load SVM model data from ".bin" file.
*   Parameters   : HI_CHAR			    *pchFileName        SVM model file name, must be ".bin" file.
*                  IVE_SVM_MODEL_S      *pstSvmModel	    SVM model data.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014-05-13
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_SVM_LoadModel(const HI_CHAR *pchFileName, IVE_SVM_MODEL_S *pstSvmModel);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_SVM_UnloadModel
*   Description  : Unload SVM model data.
*   Parameters   : IVE_SVM_MODEL_S      *pstSvmModel	    SVM model data.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014-05-13
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_VOID HI_MPI_IVE_SVM_UnloadModel(IVE_SVM_MODEL_S *pstSvmModel);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_SVM_Predict
*   Description  :
*   Parameters   : IVE_HANDLE			   *pIveHandle          Returned handle ID of a task
* 				   IVE_SRC_DATA_S	       *pstSrc			    Input sample
*                  IVE_SRC_MEM_INFO_S      *pstKernelTable      Look-up talbe for active function
*                  IVE_SVM_MODEL_S         *pstSvmModel         SVM model
*                  IVE_SRC_DATA_S          *pstDstVote          Output Votes' array of each class
*                  HI_BOOL				    bInstant            For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2013-11-28
*           Author       :
*           Modification : Created function
* 		2.  Date		 : 2015-04-21
* 			Author		 :
* 			Modification : support 1024d feature vector and array of vectors input
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_SVM_Predict(IVE_HANDLE *pIveHandle, IVE_SRC_DATA_S *pstSrc, IVE_LOOK_UP_TABLE_S *pstKernelTab, 
    IVE_SVM_MODEL_S *pstSvmModel, IVE_DST_DATA_S *pstDstVote, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_SAD
*   Description  : Sum of absolute differences.
*   Parameters   : IVE_HANDLE          *pIveHandle   Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     *pstSrc1      The input source1.Only the U8C1 input format is supported.
*                  IVE_SRC_IMAGE_S     *pstSrc2      The input source2.Only the U8C1 input format is supported.
*                  IVE_DST_IMAGE_S     *pstSad       Output result of sad value.Only the U8C1/U16C1 format is supported.
*                  IVE_DST_IMAGE_S     *pstThr       Output result of thresh.Only the U8C1 format is supported.
*                  IVE_SAD_CTRL_S	   *pstSadCtrl   Control parameter
*                  HI_BOOL             bInstant     For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   History:
*
*       1.  Date         : 2014-08-28
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_SAD(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, 
    IVE_DST_IMAGE_S *pstSad, IVE_DST_IMAGE_S *pstThr, IVE_SAD_CTRL_S *pstSadCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Resize
*   Description  : Resize.
*   Parameters   : IVE_HANDLE          *pIveHandle		Returned handle ID of a task
*                  IVE_SRC_IMAGE_S     astSrc[]			The input source.Only the U8C1/U8C3_PLANAR input format is supported.
*                  IVE_DST_IMAGE_S     astDst[]			Output result.Only the U8C1/U8C3_PLANAR format is supported.
*                  IVE_RESIZE_CTRL_S   *pstResizeCtrl   Control parameter
*                  HI_BOOL             bInstant			For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 32x16 pixels to 1920x1080 pixels.
*                  The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2015-03-26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Resize(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S astSrc[], IVE_DST_IMAGE_S astDst[], 
    IVE_RESIZE_CTRL_S *pstResizeCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CNN_LoadModel
*   Description  : Load CNN model data from ".bin" file.
*   Parameters   : HI_CHAR			*pchFileName     CNN model file name, must be ".bin" file.
* 		  		     IVE_CNN_MODEL_S	*pstCnnModel	   CNN model data.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2015-04-19
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CNN_LoadModel(const HI_CHAR *pchFileName, IVE_CNN_MODEL_S *pstCnnModel);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CNN_UnloadModel
*   Description  : Unload CNN model data and release memory.
*   Parameters   : IVE_CNN_MODEL_S *pstCnnModel)	 CNN model data.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2015-4-19
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_VOID HI_MPI_IVE_CNN_UnloadModel(IVE_CNN_MODEL_S *pstCnnModel);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CNN_Predict
*   Description  : Perform CNN prediction on input sample(s), and output responses for corresponding sample(s)
*   Parameters   : IVE_HANDLE			*pIveHandle        	Returned handle ID of a task
* 				    IVE_SRC_IMAGE_S	 	astSrc[]          	Input images array. Only the U8C1 and U8C3_PLANAR input are supported
* 				    IVE_CNN_MODEL_S		*pstCnnModel       	CNN model data
* 				    IVE_CNN_CTRL_S   		*pstCnnCtrl        	CNN control parameter
* 				    IVE_DST_DATA_S   		*pstDst            	Output vectors of CNN_Predict
* 				    HI_BOOL           	bInstant          	For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success; Error codes: Failure.
*   Spec         : The stride must be 16-pixel-aligned.
* 				   The types, widths, heights and strides of input images must be the same.
*   History:
*
*       1.  Date         : 2015-4-19
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CNN_Predict(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S astSrc[], IVE_CNN_MODEL_S *pstCnnModel, 
    IVE_DST_DATA_S *pstDst, IVE_CNN_CTRL_S *pstCnnCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CNN_GetResult
*   Description  : Calculate classification and confidence with CNN output by softmax regression.
*   Parameters   : IVE_SRC_DATA_S			*pstSrc						The result of CNN_Predict output.
* 		            IVE_DST_MEM_INFO_S		*pstDst						The prediction about classification label index and confidence.
* 				     IVE_CNN_MODEL_S       	*pstpstCnnModel         	CNN model data
* 		            IVE_CNN_CTRL_S        	*pstCnnCtrl				CNN control parameter
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2015-4-19
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CNN_GetResult(IVE_SRC_DATA_S *pstSrc, IVE_DST_MEM_INFO_S *pstDst,
    IVE_CNN_MODEL_S *pstCnnModel, IVE_CNN_CTRL_S *pstCnnCtrl);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_PerspTrans
*   Description  : Perspective transform
*   Parameters  : IVE_HANDLE		         *pIveHandle        Returned handle ID of a task.
* 		          IVE_SRC_IMAGE_S            *pstSrc			The input source.Only the U8C1/YUVSP420 input format is supported..
* 				  IVE_RECT_U32_S             astRoi[]           Roi array.
* 				  IVE_SRC_MEM_INFO_S         astPointPair[]     Point pair array.
* 				  IVE_DST_IMAGE_S            astDst[]           Output result.Only the U8C1/YUVSP420/U8C3_PACKAGE format is supported.
* 		          IVE_PERSP_TRANS_CTRL_S     *pstPerspTransCtrl PerspTrans control parameter.
* 				  HI_BOOL                    bInstant           For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The stride must be 16-pixel-aligned.
*   History:
*
*       1.  Date         : 2018-9-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_PerspTrans(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_RECT_U32_S astRoi[],
    IVE_SRC_MEM_INFO_S astPointPair[], IVE_DST_IMAGE_S astDst[], IVE_PERSP_TRANS_CTRL_S *pstPerspTransCtrl,
    HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_GetMemSize
*   Description  : KCF get object memory size
*   Parameters  :  HI_U32 u32MaxObjNum    The maximum numbers of tracking object.
* 		          HI_U32 *pu32Size       memmory size.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_GetMemSize(HI_U32 u32MaxObjNum, HI_U32 *pu32Size);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_CreateObjList
*   Description  : KCF creste object list
*   Parameters  :  IVE_MEM_INFO_S *pstMem  The memory of object for object list.
* 		          HI_U32 u32MaxObjNum The maximum numbers of tracking object (list node).
*                  IVE_KCF_OBJ_LIST_S *pstObjList  The object list.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_CreateObjList(IVE_MEM_INFO_S *pstMem, HI_U32 u32MaxObjNum, IVE_KCF_OBJ_LIST_S *pstObjList);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_DestroyObjList
*   Description  : KCF destroy object list
*   Parameters  :  IVE_KCF_OBJ_LIST_S *pstObjList  The object list.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_DestroyObjList(IVE_KCF_OBJ_LIST_S *pstObjList);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_CreateGaussPeak
*   Description  : KCF calculate gauss peak
*   Parameters  :  HI_U3Q5 u3q5Padding The multiple of roi rectangle's width and height.
*                  IVE_DST_MEM_INFO_S *pstGaussPeak The memory of gauss peak.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_CreateGaussPeak(HI_U3Q5 u3q5Padding, IVE_DST_MEM_INFO_S *pstGaussPeak);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_CreateCosWin
*   Description  : KCF calculate cos window x and y.
*   Parameters  :  IVE_DST_MEM_INFO_S *pstCosWinX The memory of cos window x.
*                  IVE_DST_MEM_INFO_S *pstCosWinY The memory of cos window y.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_CreateCosWin(IVE_DST_MEM_INFO_S *pstCosWinX, IVE_DST_MEM_INFO_S *pstCosWinY);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_GetTrainObj
*   Description  : KCF get train object.
*   Parameters  :  HI_U3Q5 u3q5Padding The multiple of roi rectangle's width and height.
*                  IVE_ROI_INFO_S astRoiInfo[] The array of roi information.
*                  HI_U32 u32ObjNum The numbers of RoiInfo and object.
*                  IVE_MEM_INFO_S *pstCosWinX The memory of cos window x.
*                  IVE_MEM_INFO_S *pstCosWinY The memory of cos window y.
*                  IVE_MEM_INFO_S *pstGaussPeak The memory of gauss peak.
*                  IVE_KCF_OBJ_LIST_S *pstObjList The object list.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_GetTrainObj(HI_U3Q5 u3q5Padding, IVE_ROI_INFO_S astRoiInfo[], HI_U32 u32ObjNum,
    IVE_MEM_INFO_S *pstCosWinX, IVE_MEM_INFO_S *pstCosWinY, IVE_MEM_INFO_S *pstGaussPeak,
    IVE_KCF_OBJ_LIST_S *pstObjList);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_Process
*   Description  : KCF process.
*   Parameters   : IVE_HANDLE         *pIveHandle     Returned handle ID of a task
*                  IVE_SRC_IMAGE_S    *pstSrc         Input source.Only the YUV420SP inpu format is supported.
*                  IVE_KCF_OBJ_LIST_S *pstObjList     The object list.
* 				 IVE_KCF_PRO_CTRL_S *pstKcfProCtrl  Control parameter.
*                  HI_BOOL             bInstant       For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_Process(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_KCF_OBJ_LIST_S *pstObjList, 
    IVE_KCF_PRO_CTRL_S *pstKcfProCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_GetObjBbox
*   Description  : KCF get track object bbox information.
*   Parameters  :  IVE_KCF_OBJ_LIST_S *pstObjList The object list.
*                  IVE_KCF_BBOX_S astBbox   The output bbox of object.
*                  HI_U32 *pu32BboxObjNum   The valid numbers of output bbox.
*                  IVE_KCF_BBOX_CTRL_S *pstKcfBboxCtrl Control parameter.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_GetObjBbox(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_BBOX_S astBbox[],
    HI_U32 *pu32BboxObjNum, IVE_KCF_BBOX_CTRL_S *pstKcfBboxCtrl);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_JudgeObjBboxTrackState
*   Description  : KCF judge object tracking state.
*   Parameters  :  IVE_KCF_OBJ_LIST_S *pstObjList The object list.
*                  IVE_KCF_BBOX_S astBbox   The bbox of object.
*                  HI_BOOL *pbTrackOk  The tracking state of object.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_JudgeObjBboxTrackState(IVE_ROI_INFO_S *pstRoiInfo, IVE_KCF_BBOX_S *pstBbox, 
    HI_BOOL *pbTrackOk);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_KCF_ObjUpdate
*   Description  : KCF update object.
*   Parameters  :  IVE_KCF_OBJ_LIST_S *pstObjList The object list.
*                  IVE_KCF_BBOX_S astBbox   The bbox information of update object.
*                  HI_U32 u32BboxObjNum The update numbers of bbox.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-10-15
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_KCF_ObjUpdate(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_BBOX_S astBbox[],
    HI_U32 u32BboxObjNum);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Hog
*   Description  : get Hog feature.
*   Parameters  :  IVE_HANDLE         *pIveHandle     Returned handle ID of a task.
*                  IVE_SRC_IMAGE_S    *pstSrc         Input source.Only the YUV420SP inpu format is supported.
*                  IVE_RECT_U32_S      astRoi[]       The array of roi.
*                  IVE_DST_BLOB_S      astDst[]       The array of Hog feature.
*                  IVE_HOG_CTRL_S     *pstHogCtrl     Control parameter.
*                  HI_BOOL             bInstant       For details, see HI_MPI_IVE_DMA.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-11-12
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Hog(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc, IVE_RECT_U32_S astRoi[],
    IVE_DST_BLOB_S astDst[], IVE_HOG_CTRL_S *pstHogCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Query
*   Description  : This API is used to query the status of a called function by using the returned IveHandle of the function.
                   In block mode, the system waits until the function that is being queried is called.
                   In non-block mode, the current status is queried and no action is taken.
*   Parameters   : IVE_HANDLE     IveHandle     IveHandle of a called function. It is entered by users.
*                  HI_BOOL       *pbFinish      Returned status
*                  HI_BOOL        bBlock        Flag indicating the block mode or non-block mode
*                  HI_BOOL  *pbFinish
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2011-05-16
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_Query(IVE_HANDLE IveHandle, HI_BOOL *pbFinish, HI_BOOL bBlock);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*__MPI_IVE_H__*/
