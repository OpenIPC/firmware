/*****************************************************************************
*Copyright (C), 2016-2018, Hisilicon Tech. Co., Ltd.
*
* FileName: hi_api_tde.h
* Description:TDE2 API define
*
* History:
* Version   Date          Author        DefectNum       Description
*
*****************************************************************************/

#ifndef _HI_API_TDE2_H_
#define _HI_API_TDE2_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif  /* __cplusplus */

#include "hi_type.h"
#include "hi_tde_type.h"

#define HI_TDE_Open HI_TDE2_Open
#define HI_TDE_Close HI_TDE2_Close
#define HI_TDE_BeginJob HI_TDE2_BeginJob

/****************************************************************************/
/*                             TDE2 API define                               */
/****************************************************************************/

/******************************* API Declaration *****************************/
/** \addtogroup      TDE */

/**
\brief Starts a two-dimensional engine 2 (TDE2) device.
\attention \n
Before using the TDE, you must call this API to ensure that a TDE device is started.\n
A TDE device can be started repeatedly by using multiple processes.\n.


\param  N/A
\retval ::HI_SUCCESS Success.
\retval ::HI_ERR_TDE_DEV_OPEN_FAILED Starting a TDE device fails.
\see \n
N/A
*/
HI_S32      HI_TDE2_Open(HI_VOID);


/**
\brief Stops a TDE2 device.
\attention \n
HI_TDE2_Open must work with HI_TDE2_Close.\n
\param  N/A
\retval N/A
\see \n
N/A
*/
HI_VOID     HI_TDE2_Close(HI_VOID);


/**
\brief Creates a TDE task.
\attention N/A
\param  N/A
\retval If the return value is greater than 0, it indicates that the TDE task handle is valid.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_INVALID_HANDLE The handle of a TDE task is invalid.
\see \n
N/A
*/
TDE_HANDLE  HI_TDE2_BeginJob(HI_VOID);


/**
\brief Submits an existing TDE task.
\attention \n
Before calling this API, you must call HI_TDE2_Open to start a TDE device and call HI_TDE2_BeginJob to obtain a valid task handle.\n
If the calling mode is set to block mode, and the API is returned due to timeout or interruption, the operation continues till it is complete even though the API called by the TDE is returned in advance.\n
The synchronization mode is not supported currently.\n
After a task is submitted, its handle becomes invalid. If the task is submitted again, the error code HI_ERR_TDE_INVALID_HANDLE is returned. \n.

\param[in] s32Handle  Handle of a TDE task.
\param[in] bSync  Whether to submit a TDE task in synchronization mode.
\param[in] bBlock  Block flag.
\param[in] u32TimeOut  Timeout period 10 ms.
\retval ::HI_SUCCESS  A task is submitted successfully in non-block mode.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The input task handle is invalid.
\retval ::HI_ERR_TDE_JOB_TIMEOUT  A task is not completed due to timeout.
\see \n
N/A
*/
HI_S32      HI_TDE2_EndJob(TDE_HANDLE s32Handle, HI_BOOL bSync, HI_BOOL bBlock, HI_U32 u32TimeOut);


/**
\brief Cancels an operation added in a TDE task.
\attention \n
Before calling this API, you must call HI_TDE2_Open to start a TDE device and call HI_TDE2_BeginJob to obtain a valid task handle.\n
A submitted task cannot be cancelled.\n
No operations can be added to cancelled tasks, because the cancelled tasks are invalid.\n
This API is called when an error occurs during TDE operation configuration.\n
\param[in] s32Handle Handle of a TDE task.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_FAILURE  The specified task cannot be cancelled because it has been submitted.

\see \n
N/A
*/
HI_S32      HI_TDE2_CancelJob(TDE_HANDLE s32Handle);



/**
\brief Waits for the completion of a specified TDE task.
\attention \n
 As a block interface, this API is blocked until the specified task is complete.\n

\param[in] s32Handle Handle of a TDE task.
\retval 0  The specified TDE task is not completed.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_QUERY_TIMEOUT  The specified task is not completed due to timeout.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The operation is not supported.
\retval ::HI_ERR_TDE_INTERRUPT Blocked job was interrupted.
\see \n
N/A
*/
HI_S32      HI_TDE2_WaitForDone(TDE_HANDLE s32Handle);


/**
\brief Waits for the completion of all TDE tasks.
\attention \n
As a block interface, this API is blocked until all TDE tasks are complete.\n

\param  N/A
\retval 0  The specified TDE task is not completed.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The operation is not supported.
\retval ::HI_ERR_TDE_INTERRUPT Blocked job was interrupted.
N/A
*/
HI_S32 HI_TDE2_WaitAllDone(HI_VOID);


/**
\brief Resets all statuses of the TDE.
\attention \n
This API is used to reset software and hardware if a timeout error occurs due to the inconsistency of the software and hardware for standby wakeup.\n
\param  N/A
\retval 0  TDE reset successfully.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\see \n
N/A
*/
HI_S32 HI_TDE2_Reset(HI_VOID);


/**
\brief Adds the fast copy operation to a TDE task.
\attention \n
HI_TDE2_QuickCopy adopts the direct memory access (DMA) transfer technology; therefore, HI_TDE2_QuickCopy is superior to HI_TDE2_Bitblit in transfer.
Because the format conversion is not supported during fast copying, the formats of the source bitmap and the target bitmap must be the same.\n
In addition, scaling is not supported during fast copying. If the operating area size of the source bitmap is different from that of the target bitmap, the copy and transfer operations are performed based on the minimum width and height.\n
The specified operating area and the specified bitmap must have a public area; otherwise, an error is returned. This requirement is applicable to other operations.\n
If the pixel format of a bitmap is greater than or equal to a byte, the base address and stride of the bitmap format must be aligned based on the pixel format. If the pixel format of a bitmap is smaller than a byte, the base address and stride of the bitmap must be aligned based on byte. This requirement is applicable to other operations.\n
If the pixel format of a bitmap is smaller than a byte, the horizontal start point and width of the bitmap must be aligned based on pixel.\n
The horizontal start point and width of the YCbCr422 bitmap must be even numbers. This requirement is applicable to other operations.\n

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstSrc  Source bitmap.
\param[in] pstSrcRect  Operating area of the source bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_QuickCopy(TDE_HANDLE s32Handle, const TDE2_SURFACE_S *pstSrc, const TDE2_RECT_S  *pstSrcRect,
                              const TDE2_SURFACE_S *pstDst, const TDE2_RECT_S *pstDstRect);


/**
\brief Rapidly fills in the target bitmap with a fixed value.
\attention \n
The data to be filled in must be specified based on the format of the target bitmap.\n

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\param[in] u32FillData  Data to be filled in.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_QuickFill(TDE_HANDLE s32Handle, const TDE2_SURFACE_S *pstDst, const TDE2_RECT_S *pstDstRect,
                              HI_U32 u32FillData);


/**
\brief Scales the source bitmap based on a specified size of the target bitmap. The source bitmap and target bitmap can be the same.
\attention \n
The maximum minification is smaller than 256, there is no limit on magnification,but the max size of the bitmap is 4095*4095\n
You can scale a bitmap that serves as both source bitmap and target bitmap. If the memories for storing the source bitmap and target bitmap overlap, the bitmaps cannot be scaled.\n

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstSrc  Source bitmap.
\param[in] pstSrcRect  Operating area of the source bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_ERR_TDE_NOT_ALIGNED  The start address or stride of the bitmap is not aligned based on pixel.
\retval ::HI_ERR_TDE_MINIFICATION  The start address or stride of the bitmap is not aligned based on pixel.
\retval ::HI_ERR_TDE_NOT_ALIGNED  The start address of the color look-up table (CLUT) is not 4-byte aligned.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The operation is not supported.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_QuickResize(TDE_HANDLE s32Handle, const TDE2_SURFACE_S *pstSrc, const TDE2_RECT_S  *pstSrcRect,
                                const TDE2_SURFACE_S *pstDst, const TDE2_RECT_S  *pstDstRect);


/**
\brief Adds the anti-flicker operation to a TDE task.
\attention \n
The anti-flicker operation supports vertical filtering only.\n
The anti-flicker operation can be performed on the source bitmap and target bitmap that are stored in the same memory.\n
If the sizes of the specified input area and output area are different, scaling is performed.\n

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstSrc  Source bitmap.
\param[in] pstSrcRect  Operating area of the source bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_ERR_TDE_NOT_ALIGNED  The start address or stride of the bitmap is not aligned based on pixel.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The operation is not supported.
\retval ::HI_ERR_TDE_MINIFICATION  The minification is too large.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_QuickDeflicker(TDE_HANDLE s32Handle, const TDE2_SURFACE_S *pstSrc, const TDE2_RECT_S  *pstSrcRect,
                                   const TDE2_SURFACE_S *pstDst, const TDE2_RECT_S  *pstDstRect);



/**
\brief Adds the transfer operation (with additional functions available for the raster or macroblock bitmap) to a TDE task.
\attention \n
Before calling this API, you must call HI_TDE2_Open to start a TDE device and call HI_TDE2_BeginJob to obtain a valid task handle.\n
If the sizes of the foreground bitmap and target bitmap are different, scaling is performed based on the preset scaling area. If no scaling area is set, the clip and transfer operations are performed based on the minimum value of the public area.\n
The value of the global alpha, Alpha0, or Alpha1 ranges from 0 to 255.\n
The background bitmap and the target bitmap can be the same.\n
If only the single-source transfer operation with additional functions,you can set the structure information and the pointer of the operating area of the background or the background bitmap to null,but the pointer to the operation option type can't be null,and it doesn't support ColorKey and Blend.\n
Scaling is not supported if the mirror function is enabled.\n
If only the single-source transfer operation (for example, performing the ROP and reverse operations on the source bitmap only) is required, you can set the structure information and the pointer of the operating area of the background or the background bitmap to null.\n
Scaling is not supported if the mirror function is enabled.\n
The clip operation can be performed only when the clipping area overlaps the operating area; otherwise, an error occurs.\n
When the color extension (from CLUT to ARGB/AYCbCr) is performed for the first time, the CLUT Reload flag function must be enabled.\n
During a simple transfer operation (the operation does not contain additional functions and the pixel format of source and target bitmaps are the same), you can set the pointer to the operation option type to null. Then fast transfer is implemented.\n
The pixel format of the target bitmap cannot be the macroblock format.\n

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstBackGround  Background bitmap.
\param[in] pstBackGroundRect  Operating area of the background bitmap.
\param[in] pstForeGround  Foreground bitmap.
\param[in] pstForeGroundRect  Operating area of the foreground bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\param[in] pstOpt  Structure for setting operation parameters.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_ERR_TDE_NOT_ALIGNED  The start address of the CLUT is not 4-byte aligned.
\retval ::HI_ERR_TDE_MINIFICATION  The minification is too large.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The bitmap does not support this operation.
\retval ::HI_ERR_TDE_CLIP_AREA  The operating area does not overlap the clipping area, and the displayed information is not updated.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_Bitblit(TDE_HANDLE s32Handle, const TDE2_SURFACE_S *pstBackGround, const TDE2_RECT_S  *pstBackGroundRect,
                            const TDE2_SURFACE_S *pstForeGround, const TDE2_RECT_S  *pstForeGroundRect, const TDE2_SURFACE_S *pstDst,
                            const TDE2_RECT_S  *pstDstRect, const TDE2_OPT_S *pstOpt);


/**
\brief Adds the fill and transfer operations (with additional functions available for the raster bitmap) to a TDE task. After addition, the functions of drawing a point, drawing a line, filling in a color block on the surface, and filling in a memory can be implemented.
\attention \n
Before calling this API, you must call HI_TDE2_Open to start a TDE device and call HI_TDE2_BeginJob to obtain a valid task handle.\n
When the foreground bitmap is null, the functions of this API are the same as those of HI_TDE2_QuickFil.\n
During solid drawing, the foreground bitmap supports the colorkey operation only.\n
To draw a rectangle, a vertical line, or a horizontal line by calling HI_TDE2_SolidDraw, you need to set the width and height of the filled rectangle. For example, drawing a vertical line is to draw a rectangle with the width of one pixel.

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstForeGround  Foreground bitmap.
\param[in] pstForeGroundRect  Operating area of the foreground bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\param[in] pstFillColor  Structure of fill colors.
\param[in] pstOpt  Structure of operation attributes.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_ERR_TDE_NOT_ALIGNED  The start address of the CLUT is not 4-byte aligned.
\retval ::HI_ERR_TDE_MINIFICATION  The minification is too large.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The bitmap does not support this operation.
\retval ::HI_ERR_TDE_CLIP_AREA  The operating area does not overlap the clipping area, and the displayed information is not updated.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_SolidDraw(TDE_HANDLE s32Handle, const TDE2_SURFACE_S *pstForeGround, const TDE2_RECT_S  *pstForeGroundRect,
                              const TDE2_SURFACE_S *pstDst,
                              const TDE2_RECT_S  *pstDstRect, const TDE2_FILLCOLOR_S *pstFillColor,
                              const TDE2_OPT_S *pstOpt);


/**
\brief Adds the transfer operation (with additional functions available for the macroblock bitmap) to a TDE task. That is, the luminance macroblock data and the chrominance macroblock data are combined into raster data. During the combination, the scaling, anti-flicker, and clip operations can be performed concurrently.
\attention \n
Before calling this API, you must call HI_TDE2_Open to start a TDE device and call HI_TDE2_BeginJob to obtain a valid task handle.\n
For the YCbCr422 macroblock, the horizontal coordination of the start point and the width of the operating area must be an even number.\n
The pixel format of the target bitmap cannot be the macroblock format.\n

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstMB  Surface of a macroblock.
\param[in] pstMbRect  Operating area of a macroblock.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\param[in] pstMbOpt  Operation attributes of a macroblock.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_ERR_TDE_MINIFICATION  The minification is too large because its maximum value is 255.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The bitmap does not support this operation.
\retval ::HI_ERR_TDE_CLIP_AREA  The operating area does not overlap the clipping area, and the displayed information is not updated.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_MbBlit(TDE_HANDLE s32Handle, const TDE2_MB_S *pstMB, const TDE2_RECT_S  *pstMbRect, const TDE2_SURFACE_S *pstDst, const TDE2_RECT_S  *pstDstRect,
                           const TDE2_MBOPT_S *pstMbOpt);


/**
\brief Adds the mask raster of operation (ROP) (available for the raster bitmap) to a TDE task. That is, the ROP operation is performed on the foreground bitmap and the background bitmap based on the mask bitmap.
\attention \n
Before calling this API, you must call HI_TDE2_Open to start a TDE device and call HI_TDE2_BeginJob to obtain a valid task handle.\n
The sizes of the operating areas of the mask bitmap, foreground bitmap, and background bitmap must be the same.\n
The mask bitmap must be in A1 format.\n
The target bitmap and foreground bitmap must be in the same color space.\n
\param[in] s32Handle  Handle of a TDE task.
\param[in] pstBackGround  Background bitmap.
\param[in] pstBackGroundRect  Operating area of the background bitmap.
\param[in] pstForeGround  Foreground bitmap.
\param[in] pstForeGroundRect  Operating area of the foreground bitmap.
\param[in] pstMask  Mask bitmap.
\param[in] pstMaskRect  Operating area of the mask bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\param[in] enRopCode_Color  ROP code of a color component.
\param[in] enRopCode_Alpha  ROP code of the alpha component.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_BitmapMaskRop(TDE_HANDLE s32Handle,
                                  const TDE2_SURFACE_S *pstBackGround, const TDE2_RECT_S  *pstBackGroundRect,
                                  const TDE2_SURFACE_S *pstForeGround, const TDE2_RECT_S  *pstForeGroundRect,
                                  const TDE2_SURFACE_S *pstMask, const TDE2_RECT_S  *pstMaskRect,
                                  const TDE2_SURFACE_S *pstDst, const TDE2_RECT_S  *pstDstRect,
                                  TDE2_ROP_CODE_E enRopCode_Color, TDE2_ROP_CODE_E enRopCode_Alpha);


/**
\brief Adds the mask blend operation (available for the raster bitmap) to a TDE task. That is, the foreground bitmap and the background bitmap are blending with the mask bitmap based on the mask bitmap.
\attention \n
Before calling this API, you must call HI_TDE2_Open to start a TDE device and call HI_TDE2_BeginJob to obtain a valid task handle.\n
The target bitmap and background bitmap must be in the same color space.\n
If the foreground bitmap contains the premultiplied data, the premultiplied mode must be selected during alpha blending; otherwise, the non-premultiplied mode is selected.
The parameter enBlendMode cannot be set to TDE2_ALUCMD_ROP.\n

\param[in] s32Handle   Handle of a TDE task.
\param[in] pstBackGround  Background bitmap.
\param[in] pstBackGroundRect  Operating area of the background bitmap.
\param[in] pstForeGround  Foreground bitmap.
\param[in] pstForeGroundRect  Operating area of the foreground bitmap.
\param[in] pstMask  Mask bitmap.
\param[in] pstMaskRect  Operating area of the mask bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating area of the target bitmap.
\param[in] u8Alpha  Global alpha value during alpha blending.
\param[in] enBlendMode  Alpha blending mode.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\retval ::HI_ERR_TDE_NO_MEM  Adding an operation fails due to insufficient memory.
\retval ::HI_FAILURE  A system error or an unknown error occurs.
\see \n
N/A
*/
HI_S32      HI_TDE2_BitmapMaskBlend(TDE_HANDLE s32Handle,
                                    const TDE2_SURFACE_S *pstBackGround, const TDE2_RECT_S  *pstBackGroundRect,
                                    const TDE2_SURFACE_S *pstForeGround, const TDE2_RECT_S  *pstForeGroundRect,
                                    const TDE2_SURFACE_S *pstMask, const TDE2_RECT_S  *pstMaskRect,
                                    const TDE2_SURFACE_S *pstDst, const TDE2_RECT_S  *pstDstRect,
                                    HI_U8 u8Alpha, TDE2_ALUCMD_E enBlendMode);


/**
\brief  Adds the rotation operation to a TDE task.
\attention \n
The pixel formats of the source bitmap and target bitmap must be the same, and the format cannot be YUV, CLUT1, or CLUT4.\n
The parameters s32DstPointX and s32DstPointY are set to 0 by default. Because the two parameters are not used at present, they can be set to any values.\n

\param[in] s32Handle  Handle of a TDE task.
\param[in] pstSrc  Source bitmap.
\param[in] pstSrcRect  Operating area of the source bitmap.
\param[in] pstDst  Target bitmap.
\param[in] s32DstPointX  Horizontal coordinate of the start point of the operating area in the target bitmap.
\param[in] s32DstPointY  Vertical coordinate of the start point of the operating area in the target bitmap.
\param[in] enRotateAngle  Rotate angle.
\retval 0 success.
\retval ::HI_ERR_TDE_DEV_NOT_OPEN  Calling this API fails because no TDE device is started.
\retval ::HI_ERR_TDE_INVALID_HANDLE  The handle of a TDE task is invalid.
\retval ::HI_ERR_TDE_NULL_PTR  The parameter contains a null pointer.
\retval ::HI_ERR_TDE_UNSUPPORTED_OPERATION  The operation is not supported.
\retval ::HI_ERR_TDE_INVALID_PARA  The parameter is invalid.
\see \n
N/A
*/
HI_S32 HI_TDE2_Rotate(TDE_HANDLE s32Handle, const TDE2_SURFACE_S* pstSrc, const TDE2_RECT_S  *pstSrcRect, const TDE2_SURFACE_S* pstDst,
                                                const TDE2_RECT_S  *pstDstRect,TDE_ROTATE_ANGLE_E enRotateAngle);



/**
\brief Sets the anti-flicker level.
\attention \n
N/A
\param[in] enDeflickerLevel  Anti-flicker level.
\retval 0 success.
\retval ::

\see none
*/
HI_S32 HI_TDE2_SetDeflickerLevel(TDE_DEFLICKER_LEVEL_E enDeflickerLevel);


/**
\brief Obtains the anti-flicker level.
\attention \n
N/A
\param[in] pDeflickerLevel  Anti-flicker level.
\retval 0 success.
\retval ::

\see \n
N/A
*/
HI_S32 HI_TDE2_GetDeflickerLevel(TDE_DEFLICKER_LEVEL_E *pDeflickerLevel);


/**
\brief Sets the alpha judgment threshold.
\attention \n
N/A
\param[in] u8ThresholdValue  Judgment threshold.
\retval 0 success.
\retval ::

\see \n
N/A
*/
HI_S32 HI_TDE2_SetAlphaThresholdValue(HI_U8 u8ThresholdValue);


/**
\brief Obtains the alpha judgment threshold.
\attention \n
N/A
\param[in] pu8ThresholdValue  Judgment threshold.
\retval 0 success.
\retval ::

\see \n
N/A
*/
HI_S32 HI_TDE2_GetAlphaThresholdValue(HI_U8 *pu8ThresholdValue);


/**
\brief Enables or disables the alpha judgment.
\attention  N/A
\param[in] bEnAlphaThreshold  Whether to enable the alpha judgment.
\retval 0 success.
\retval ::
\see \n
N/A
*/
HI_S32 HI_TDE2_SetAlphaThresholdState(HI_BOOL bEnAlphaThreshold);


/**
\brief Obtains the enable status of the alpha judgment.
\attention N/A
\param[in]  p_bEnAlphaThreshold  Whether to enable the alpha judgment.
\retval 0 success.
\retval ::

\see \n
N/A
*/
HI_S32 HI_TDE2_GetAlphaThresholdState(HI_BOOL *p_bEnAlphaThreshold);


/**
\brief Fills in the pattern.
\attention N/A
\param[in] s32Handle  Handle of a TDE task.
\param[in] pstBackGround  Background bitmap.
\param[in] pstBackGroundRect  Operating rectangle of the background bitmap.
\param[in] pstForeGround  Foreground bitmap.
\param[in] pstForeGroundRect  Operating rectangle of the foreground bitmap.
\param[in] pstDst  Target bitmap.
\param[in] pstDstRect  Operating rectangle of the target bitmap.
\param[in] pstOpt  Options for pattern filling.
\retval 0 success.
\retval ::

\see \n
N/A
*/
HI_S32 HI_TDE2_PatternFill(TDE_HANDLE s32Handle, const TDE2_SURFACE_S *pstBackGround,
                           const TDE2_RECT_S *pstBackGroundRect, const TDE2_SURFACE_S *pstForeGround,
                           const TDE2_RECT_S *pstForeGroundRect, const TDE2_SURFACE_S *pstDst,
                           const TDE2_RECT_S *pstDstRect, const TDE2_PATTERN_FILL_OPT_S *pstOpt);

/**
\brief Enables or disables the regional anti-flicker function.
\attention N/A
\param[in] bRegionDeflicker  Enable/disable flag.
\retval 0 success.
\retval ::

\see \n
N/A
*/
HI_S32 HI_TDE2_EnableRegionDeflicker(HI_BOOL bRegionDeflicker);

/**
\brief: multi blend surface by surface list
\param[in]: composed surface list
\retval: 0 success
**/
HI_S32 HI_TDE2_MultiBlending(TDE_HANDLE s32Handle, const TDE_SURFACE_LIST_S *pstSurfaceList);

/** @} */  /** <!-- ==== API declaration end ==== */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif  /* __cplusplus */

#endif  /* _HI_API_TDE2_H_ */
