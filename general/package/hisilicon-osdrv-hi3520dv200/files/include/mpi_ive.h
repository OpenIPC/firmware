/******************************************************************************
 
  Copyright (C), 2001-2011, HiSilicon Technologies Co., Ltd.
 
 ******************************************************************************
  File Name     : mpi_ive.h
  Version       : Initial Draft
  Author        : HiSilicon multimedia software group
  Created       : 2011-04-11
  Description   : Intelligent video engine (IVE) function declarations
  History       : 
  1.Date        : 2010-04-11
    Author      : Gong Xiongliang (employee ID: 00181683)
    Modification: This file is created.
 ******************************************************************************/

#ifndef __MPI_IVE_H__
#define __MPI_IVE_H__

#include "hi_comm_ive.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/*****************************************************************************
*   Prototype    : HI_MPI_IVE_DMA
*   Description: Describes the information about direct memory access (DMA). Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_INFO_S *pstSrc  Input source data
*                  IVE_MEM_INFO_S *pstDst  Output result
*                  HI_BOOL bInstant        Flag indicating whether to generate an interrupt.
*                                          If the output result blocks the next operation,
*                                          set bInstant to HI_TRUE.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 32x1 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be aligned by byte. 
*                  The stride must be 8-byte-aligned.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_DMA(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_FILTER
*   Description: Describes the information about the 3x3 template filter.The single-component,SP420 and SP422 input formats are supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task           
*                  IVE_SRC_INFO_S *pstSrc  Input source data
*                  IVE_MEM_INFO_S *pstDst  Output result
*                  IVE_FILTER_CTRL_S *pstFilterCtrl  Control parameters of filter
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.            
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_FILTER(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, IVE_FILTER_CTRL_S *pstFilterCtrl,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CSC
*   Description: Describes the information about color space conversion (CSC). Only the conversion from YUV to RGB is supported. The SP420 and SP422 input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task         
*                  IVE_SRC_INFO_S *pstSrc  Input source data    
*                  IVE_MEM_INFO_S *pstDst  Output result    
*                  IVE_CSC_CTRL_S *pstCscCtrl  Control parameters for CSC
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CSC(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, IVE_CSC_CTRL_S *pstCscCtrl,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_FILTER_AND_CSC
*   Description: Describes the information about 3x3 template filter+CSC. The SP420 and SP422 input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task                   
*                  IVE_SRC_INFO_S *pstSrc  Input source data                     
*                  IVE_MEM_INFO_S *pstDst  Output result                   
*                  IVE_FILTER_AND_CSC_CTRL_S *pstFltCscCtrl  Control parameters
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.                  
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_FILTER_AND_CSC(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
        IVE_MEM_INFO_S *pstDst, IVE_FILTER_AND_CSC_CTRL_S *pstFltCscCtrl,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_SOBEL
*   Description: Describes the information about SOBEL. HI_MPI_IVE_SOBEL is used to extract the edge information. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task          
*                  IVE_SRC_INFO_S *pstSrc  Input source data        
*                  IVE_MEM_INFO_S *pstDstH Output horizontal result        
*                  IVE_MEM_INFO_S *pstDstV Output vertical result        
*                  IVE_SOBEL_CTRL_S *pstSobelCtrl  Control parameters
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_SOBEL(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
        IVE_MEM_INFO_S *pstDstH, IVE_MEM_INFO_S *pstDstV,
        IVE_SOBEL_CTRL_S *pstSobelCtrl,HI_BOOL bInstant);


/*****************************************************************************
*   Prototype    : HI_MPI_IVE_CANNY
*   Description: Describes the information about CANNY. HI_MPI_IVE_CANNY is used to extract the edge information and is more complicated than HI_MPI_IVE_SOBEL. HI_MPI_IVE_CANNY does not provide comprehensive functions. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task             
*                  IVE_SRC_INFO_S *pstSrc  Input source data             
*                  IVE_MEM_INFO_S *pstDstMag  Output magnitude         
*                  IVE_MEM_INFO_S *pstDstAng  Output angle
*                                             If the output mode is set to magnitude only, this item can be set to null.
*                  IVE_CANNY_CTRL_S *pstCannyCtrl  Control parameters
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_CANNY(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
        IVE_MEM_INFO_S *pstDstMag, IVE_MEM_INFO_S *pstDstAng,
        IVE_CANNY_CTRL_S *pstCannyCtrl,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_DILATE
*   Description: Describes the information about dilate. This API can be used to implement bold characters and fault offset. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task           
*                  IVE_SRC_INFO_S *pstSrc  Input source data             
*                  IVE_MEM_INFO_S *pstDst  Output result           
*                  IVE_DILATE_CTRL_S *pstDilateCtrl  Control parameters
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*                  The input value, output value, and mask value must be 0 or 255.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_DILATE(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, IVE_DILATE_CTRL_S *pstDilateCtrl,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_ERODE
*   Description: Describes the information about erode. This MPI can be used to remove tiny points. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task           
*                  IVE_SRC_INFO_S *pstSrc  Input source data             
*                  IVE_MEM_INFO_S *pstDst  Output result           
*                  IVE_ERODE_CTRL_S *pstErodeCtrl  Control parameters
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1024 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*                  The input value, output value, and mask value must be 0 or 255.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/

HI_S32 HI_MPI_IVE_ERODE(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, IVE_ERODE_CTRL_S *pstErodeCtrl,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_THRESH 
*   Description: Describes the information about thresh. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task           
*                  IVE_SRC_INFO_S *pstSrc  Input source data             
*                  IVE_MEM_INFO_S *pstDst  Output result           
*                  IVE_THRESH_CTRL_S *pstThreshCtrl  Control parameters
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be aligned by byte. 
*                  The stride must be 8-byte-aligned.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_THRESH(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, IVE_THRESH_CTRL_S *pstThreshCtrl,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_AND
*   Description: Describes the information about AND. The pixel points at mapping positions of two input sources are ANDed. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_INFO_S *pstSrc1  Augend of the input source
*                  IVE_SRC_INFO_S *pstSrc2  Addend of the input source
*                  IVE_MEM_INFO_S *pstDst   Output result of src1 plus src2
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be aligned by byte. 
*                  The stride must be 8-byte-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_AND(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc1, 
    IVE_SRC_INFO_S *pstSrc2,IVE_MEM_INFO_S *pstDst, HI_BOOL bInstant);


/*****************************************************************************
*   Prototype    : HI_MPI_IVE_SUB
*   Description: Describes the information about SUB. The pixel points of an input source are subtracted from the pixel points at the mapping positions of another input source. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_INFO_S *pstSrc1  Minuend of the input source
*                  IVE_SRC_INFO_S *pstSrc2  Subtrahend of the input source
*                  IVE_MEM_INFO_S *pstDst   Output result of src1 minus src2
*                  IVE_SUB_OUT_FMT_E enOutFmt Output result format
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be aligned by byte. 
*                  The stride must be 8-byte-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_SUB(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc1, 
    IVE_SRC_INFO_S *pstSrc2,IVE_MEM_INFO_S *pstDst, 
    IVE_SUB_OUT_FMT_E enOutFmt,HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_OR
*   Description: Describes the information about OR. The pixel points at mapping positions of two input sources are ORed. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_INFO_S *pstSrc1  Input source 1
*                  IVE_SRC_INFO_S *pstSrc2  Input source 2
*                  IVE_MEM_INFO_S *pstDst   Output result src1 or src2
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be aligned by byte. 
*                  The stride must be 8-byte-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_OR(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc1, 
    IVE_SRC_INFO_S *pstSrc2,IVE_MEM_INFO_S *pstDst, HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_INTEG
*   Description: Describes the information about integral statistics. This API is used to collect statistics on the single-component data in the form of integrogram. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_INFO_S *pstSrc  Input source data
*                  IVE_MEM_INFO_S *pstDst  Output result
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_INTEG(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, HI_BOOL bInstant);



/*****************************************************************************
*   Prototype    : HI_MPI_IVE_HIST
*   Description: Describes the information about histogram statistics. This API is used to collect statistics on the single-component data in the form of histogram. Only the single-component input format is supported.
*   Input        : IVE_HANDLE *pIveHandle  Returned handle ID of a task
*                  IVE_SRC_INFO_S *pstSrc  Input source data
*                  IVE_MEM_INFO_S *pstDst  Output result
*                  HI_BOOL bInstant        For details, see HI_MPI_IVE_DMA.
*   Output       : IVE_HANDLE *pIveHandle
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : The size of the input data ranges from 64x64 pixels to 1920x1080 pixels.
*                  The physical addresses of the input data and output data must be 8-byte-aligned. 
*                  The stride must be 8-byte-aligned.
*                  The types, widths, heights of two input sources must be the same.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_IVE_HIST(IVE_HANDLE *pIveHandle, IVE_SRC_INFO_S *pstSrc, 
    IVE_MEM_INFO_S *pstDst, HI_BOOL bInstant);


/*****************************************************************************
*   Prototype    : HI_MPI_IVE_Query
*   Description: This API is used to query the status of a called function by using the returned IveHandle of the function.
                   In block mode, the system waits until the function that is being queried is called.
                   In non-block mode, the current status is queried and no action is taken.
*   Input        : IVE_HANDLE IveHandle  IveHandle of a called function. It is entered by users.
*                  HI_BOOL *pbFinish     Returned status
*                  HI_BOOL bBlock        Flag indicating the block mode or non-block mode
*   Output       : HI_BOOL *pbFinish
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2011-05-16
*           Author       : Jiang Xiaohua (employee ID: 00169368)
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
