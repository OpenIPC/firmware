/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : hi_jpeg_api.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : The user will use this api to realize some function
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/

#ifndef __HI_JPEG_API_H__
#define __HI_JPEG_API_H__


/*********************************add include here******************************/

#include "jpeglib.h"
#include "hi_type.h"

/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" 
{
#endif
#endif /* __cplusplus */


     /***************************** Macro Definition ******************************/
	 /** \addtogroup	  JPEG */
	 /** @{ */	/** <!--[JPEG]*/

	 /** Maximum number of data components */
	 /** CNcomment:最大数据分量数 CNend */
     #define MAX_PIXEL_COMPONENT_NUM            3
	 /** Maximum column number of hor coef */
	 /** CNcomment:最大水平列系数 CNend */
     #define MAX_HORCOEF_COL                     8
	 /** Maximum row number of hor coef */
	 /** CNcomment:最大水平行系数 CNend */
     #define MAX_HORCOEF_ROW                     4
	 /** Maximum column number of ver coef */
	 /** CNcomment:最大垂直列系数 CNend */
	 #define MAX_VERCOEF_COL                     4
	 /** Maximum row number of ver coef */
	 /** CNcomment:最大垂直行系数 CNend */
     #define MAX_VERCOEF_ROW                     4
	 /** Maximum column number of csc coef */
	 /** CNcomment:最大csc列系数 CNend */
	 #define MAX_CSCCOEF_COL                     3
	 /** Maximum row number of csc coef */
	 /** CNcomment:最大csc行系数 CNend */
     #define MAX_CSCCOEF_ROW                     3

     /** @} */  /** <!-- ==== Macro Definition end ==== */

	 /*************************** Enum Definition ****************************/
    /****************************************************************************/
	/*							   jpeg enum    					            */
	/****************************************************************************/
	
	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/

	/** enum of the input jpeg color space */
    /** CNcomment:输入的jpeg图片格式 CNend */
	typedef enum hiHI_JPEG_FMT_E
	{
		JPEG_FMT_YUV400	    = 0,   /**< yuv400     */
		JPEG_FMT_YUV420	    = 1,   /**< yuv420     */
		JPEG_FMT_YUV422_21    = 2,   /**< yuv422_21  */
		JPEG_FMT_YUV422_12    = 3,   /**< yuv422_12  */
		JPEG_FMT_YUV444	    = 4,   /**< yuv444     */
		JPEG_FMT_YCCK   	    = 5,   /**< YCCK       */
		JPEG_FMT_CMYK   	    = 6,   /**< CMYK       */
		JPEG_FMT_BUTT	
	}HI_JPEG_FMT_E;

	/** enum of mem check type */
    /** CNcomment:内存剩余判断类型 CNend */
	typedef enum hiHI_JPEG_MEMCHECK_E
	{
		JPEG_MEMCHECK_MEMSIZE    = 0,   /**< 内存大小   */
		JPEG_MEMCHECK_RESOLUTION = 1,   /**< 分辨率     */
		JPEG_MEMCHECK_BUTT	
	}HI_JPEG_MEMCHECK_E;

    /** @} */  /** <!-- ==== enum Definition end ==== */
	
	/*************************** Structure Definition ****************************/
    /****************************************************************************/
	/*							   jpeg api structure    					    */
	/****************************************************************************/
	
	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/

	/** Structure of Rectangle */
    /** CNcomment:矩形 CNend */
	typedef struct hiJPEG_RECT_S
	{
		HI_S32 x, y;
		HI_S32 w, h;
	}HI_JPEG_RECT_S;

	/** Structure of the output yuvsp/rgb/yuv package message, only support 1/2/4/8/ scale */
	/** CNcomment:设置输出yuvsp/rgb/yuv pacekage 的相关信息，只支持1/2/4/8缩放 CNend */
	typedef struct hiJPEG_SURFACE_S
	{
		 HI_CHAR* pOutPhy[MAX_PIXEL_COMPONENT_NUM];        /**< output physic address,0 is luminance or rgb buf, 1 is chroma buf    *//**<CNcomment:输出物理地址，0分量输出y或者rgb地址，1分量为色度地址 CNend     */
		 HI_CHAR* pOutVir[MAX_PIXEL_COMPONENT_NUM];        /**< output virtual address,0 is luminance or rgb buf, 1 is chroma buf   *//**<CNcomment:输出虚拟地址，0分量输出y或者rgb地址，1分量为色度地址 CNend     */
		 HI_U32   u32OutStride[MAX_PIXEL_COMPONENT_NUM];   /**< output stride                     *//**<CNcomment:输出行间距 CNend      */
         HI_BOOL  bUserPhyMem;                             /**< if use user physic memeory         *//**<CNcomment:是否使用用户的物理内存 CNend  */
	
	}HI_JPEG_SURFACE_S;
	
	/** Structure of the output message */
	/** CNcomment:设置输出相关信息 CNend */
	typedef struct hiJPEG_SURFACE_DESCRIPTION_S
	{
		 HI_JPEG_SURFACE_S	 stOutSurface; /**< output  surface   *//**<CNcomment:输出信息 CNend       */
		 HI_JPEG_RECT_S      stCropRect;   /**< output crop size  *//**<CNcomment:输出裁剪区域 CNend   */
		 HI_BOOL bCrop;                    /**<  if crop           *//**<CNcomment:是否裁剪 CNend      */
	}HI_JPEG_SURFACE_DESCRIPTION_S;


	/** Structure of output argb sampler and filte message */
	/** CNcomment:设置输出argb采样和滤波相关信息 CNend */
    typedef struct hiJPEG_DEC_COEF_S
    {
        HI_BOOL bEnHorMedian;                                  /**< if set hor median filter           *//**<CNcomment:是否水平中值滤波 CNend        */
		HI_BOOL bEnVerMedian;                                  /**< if set ver median filter           *//**<CNcomment:是否垂直中值滤波 CNend        */
        HI_BOOL bSetHorSampleCoef;                             /**< if set hor sample coef            *//**<CNcomment:是否设置水平采样参数 CNend    */
		HI_BOOL bSetVerSampleCoef;                             /**< if set ver sample coef            *//**<CNcomment:是否设置垂直采样参数 CNend    */
		HI_BOOL bSetCSCCoef;                                   /**< if set csc coef                    *//**<CNcomment:是否设置CSC参数 CNend         */
		HI_S16  s16HorCoef[MAX_HORCOEF_ROW][MAX_HORCOEF_COL];  /**< set chrome hor sampler coefficient  *//**<CNcomment:色度上采样水平滤波系数 CNend */
		HI_S16  s16VerCoef[MAX_VERCOEF_ROW][MAX_VERCOEF_COL];  /**< set chrome ver sampler coefficient  *//**<CNcomment:色度上采样垂直滤波系数 CNend */
		HI_S16  s16CSCCoef[MAX_CSCCOEF_ROW][MAX_CSCCOEF_COL];  /**< set csc coefficient                 *//**<CNcomment:颜色空间转换系数 CNend       */
    }HI_JPEG_DEC_COEF_S;


	/** Structure of the input and output jpeg width and height,input jpeg format */
	/** CNcomment:jpeg文件的原始大小以及解码输出大小，原始图片格式 CNend */
	typedef struct hiJPEG_INFO_S
	{
	     HI_U32  u32Width[MAX_PIXEL_COMPONENT_NUM];     /**< 0 input and output rgb width and lu width,1 is ch width    *//**<CNcomment:0输入和解码输出rgb图片宽度或者解码yuvsp亮度宽度，1为色度宽度 CNend */
		 HI_U32  u32Height[MAX_PIXEL_COMPONENT_NUM];    /**< 0 input and output rgb height and lu width,1 is ch height  *//**<CNcomment:0输入和解码输出rgb图片高度或者解码yuvsp亮度高度，1为色度高度 CNend */
		 HI_U32  u32OutStride[MAX_PIXEL_COMPONENT_NUM]; /**< 0 output rgb stride and lu stride,1 is ch stride           *//**<CNcomment:0解码输出rgb或yubsp的亮度行间距，1为色度行间距离 CNend             */
		 HI_U32  u32OutSize[MAX_PIXEL_COMPONENT_NUM];   /**< 0 output rgb size and lu size,1 is ch size		         *//**<CNcomment:0解码输出rgb或yubsp的大小，1为色度大小 CNend            			  */
		 HI_JPEG_FMT_E enFmt;  /**< the input format  *//**<CNcomment:输入的图片格式 CNend      */
		 HI_BOOL bOutInfo;     /**< false is get input information and the other get output information   *//**<CNcomment:FALSE获取输入信息，TRUE获取输出信息 CNend */
	}HI_JPEG_INFO_S;


	/** Structure of the leave memsize that jpeg decode can used */
	/** CNcomment:jpeg解码可以使用的内存大小信息 CNend */
	typedef struct hiJPEG_MEMSIZE_INFO_S
	{
	     HI_U32  u32MemSize;      /**< the memsize  *//**<CNcomment:剩余内存大小 CNend */
		 HI_U32  u32Width;		   /**< limit width  *//**<CNcomment:限制宽 CNend */
		 HI_U32  u32Height;		   /**< limit height *//**<CNcomment:限制高 CNend */
		 HI_JPEG_MEMCHECK_E eCheckType; /**< limit type *//**<CNcomment:通过哪种类型来限制 CNend */
	}HI_JPEG_MEMSIZE_INFO_S;
	
	/** @} */  /** <!-- ==== Structure Definition End ==== */
	
    /********************** Global Variable declaration **************************/
 
    /******************************* API declaration *****************************/


	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/
	

	/** 
	\brief Sets dec output message. CNcomment:设置解码输出的信息上下文 CNend
	\attention \n
	HI_JPEG_SetOutDesc should have called create jpeg decoder.set the output address \n
	and output stride,set whether crop, set crop rect \n
	CNcomment:必须在创建解码器之后，启动解码之前调用该接口，主要设置解码输出地址和输出 \n
	          行间距，设置是否裁剪以及对应的裁剪区域 CNend\n

	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	*pstSurfaceDesc. CNcomment:解码输出描述信息 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetOutDesc
	*/
	HI_S32	HI_JPEG_SetOutDesc(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_SURFACE_DESCRIPTION_S *pstSurfaceDesc);

	/** 
	\brief Get Jpeg information. CNcomment:获取jpeg图片信息 CNend
	\attention \n
	if you want to get input format and input width and input height,you should set bOutInfo false.\n
    others you can get the information as follows: output rgb widht/height/stride/size or output \n
    yuvsp lu width/height/stride/size and ch width/height/stride/size.\n
	you call this function should after read header and set the ouput parameter.\n
	CNcomment:当bOutInfo设置成FALSE的时候，可以获取到图片输入的宽度和高度以及像素格式，当设置成TRUE的 \n
	          时候则可以获取到如下信息，要是解码RGB则获取到宽度/高度/行间距/大小,要是解码输出yuvsp，\n
	          则可以获取的亮度和色度的宽度/高度/行间距/大小的信息。 CNend\n

	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[out]	pJpegInfo.  CNcomment:解码jpeg的相关信息  CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetJpegInfo
	*/
	HI_S32  HI_JPEG_GetJpegInfo(j_decompress_ptr cinfo, HI_JPEG_INFO_S *pJpegInfo);


	/** 
	\brief set jpeg dec inflexion. CNcomment:在硬件解码支持的情况下，设置软解和硬解的拐点 CNend
	\attention \n
	HI_JPEG_SetInflexion should have called jpeg_create_decompress.if no call this \n
	function,use the default flexion \n
	CNcomment:必须在创建完解码器之后调用该函数，要是没有设置拐点，使用默认的拐点大小 CNend\n

	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	u32flexionSize. CNcomment:要设置的解码拐点大小 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetInflexion
	*/
	HI_S32 HI_JPEG_SetInflexion(const struct jpeg_decompress_struct *cinfo, const HI_U32 u32flexionSize);
	

	/** 
	\brief get jpeg dec inflexion. CNcomment:获取软件和硬件解码的拐点 CNend
	\attention \n
	HI_JPEG_GetInflexion should have called jpeg_create_decompress.\n
	CNcomment:在调用HI_JPEG_GetInflexion之前必须已经创建好了解码器 CNend\n

	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[out]	pu32flexionSize. CNcomment:解码拐点大小 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetInflexion
	*/
	HI_S32 HI_JPEG_GetInflexion(const struct jpeg_decompress_struct *cinfo, HI_U32 *pu32flexionSize);
	

	/** 
	\brief set jpeg dec coef when output argb. CNcomment:在解码输出ARGB的情况下设置相关系数 CNend
	\attention \n
	HI_JPEG_SetDecCoef should have called jpeg_create_decompress.set whether horizontal \n
	and vertical fliter,whether set horizontal and ver sample, whether set csc coefficient, \n
	and set there coefficient.if no call this function, use the default parameter. \n
	CNcomment:必须在创建完解码器之后调用该函数，主要设置是否垂直和水平滤波，是否设置垂直和水平 \n
	          采样系数，是否设置CSS系数，并设置相对应的系数，要是没有调用该函数，使用默认值 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	*pstDecCoef. CNcomment:解码系数 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetDecCoef
	*/
	HI_S32 HI_JPEG_SetDecCoef(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_DEC_COEF_S *pstDecCoef);


	/** 
	\brief get jpeg dec coef when output argb. CNcomment:在解码输出ARGB的情况下获取设置的相关系数 CNend
	\attention \n
	HI_JPEG_GetDecCoef should have called HI_JPEG_SetDecCoef.\n
	CNcomment:在调用HI_JPEG_GetDecCoef之前必须已经HI_JPEG_SetDecCoef CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[out]	pstOutDecCoef. CNcomment:输出解码系数 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetDecCoef
	*/
	HI_S32 HI_JPEG_GetDecCoef(const struct jpeg_decompress_struct *cinfo, HI_JPEG_DEC_COEF_S *pstOutDecCoef);


	/** 
	\brief set alpha value. CNcomment:设置alpha的值 CNend
	\attention \n
	HI_JPEG_SetAlpha should have called jpeg_create_decompress.when decode output \n
	argb8888 and argb8888,we can call this function,if no call it,use the default value. \n
	CNcomment:必须在创建完解码器之后调用该函数，当解码输出为ARGB8888和ABGR8888的时候可以 \n
	调用该函数，要是没有调用该函数，就使用默认的值 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	s32Alpha. CNcomment:设置alpha值 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetAlpha
	*/
	HI_S32 HI_JPEG_SetAlpha(const struct jpeg_decompress_struct *cinfo, const HI_U32 u32Alpha);


	/** 
	\brief set stream from flag of use phy mem  or virtual mem. CNcomment:设置码流连续还是虚拟内存信息 CNend
	\attention \n
	if want to use this function,should call between create decompress and 
	jpeg_stdio_src or jpeg_mem_src.if not call this we should check\n
	CNcomment:如果要调用，必须在创建完解码器关联码流之前调用，如果没有调用该接口也有可能是连续的内存 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	pStreamPhyAddr. CNcomment:码流物理地址 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetStreamPhyMem
	*/
	 HI_S32 HI_JPEG_SetStreamPhyMem(const struct jpeg_decompress_struct *cinfo, HI_CHAR* pStreamPhyAddr);

	/** 
	\brief set stream mem information. CNcomment:设置码流内存信息 CNend
	\attention \n
	if want to use this function,should call before call
	jpeg_stdio_src or jpeg_mem_src.if not call this we should check\n
	CNcomment:如果要调用，必须放在jpeg_mem_src之前 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	pPhyStremBuf.         CNcomment:码流buffer起始物理地址 CNend
	\param[in]	u32StremBufLen.       CNcomment:码流buffer大小         CNend
	\param[in]	pVirSaveReturnBuf.    CNcomment:码流buffer回绕起始地址 CNend
	\param[in]	s32StrmReturnSize.    CNcomment:回绕码流大小           CNend
	\param[in]	bStreamBufNeedReturn. CNcomment:码流buffer是否回绕     CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetBufInfo
	*/
	HI_S32 HI_JPEG_SetBufInfo(const struct jpeg_decompress_struct *cinfo,HI_CHAR* pPhyStremBuf,HI_U32 u32StremBufLen, HI_CHAR* pVirSaveReturnBuf,HI_S32 s32StrmReturnSize,HI_BOOL bStreamBufNeedReturn);

	/** 
	\brief set if dec output yuv420sp. CNcomment:设置是否统一输出yuv420sp标识 CNend
	\attention \n
	HI_JPEG_SetYCbCr420spFlag should have called jpeg_create_decompress.\n
	CNcomment:在调用HI_JPEG_SetYCbCr420spFlag之前必须已经创建好了解码器 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	bOutYCbCr420sp. CNcomment:是否统一解码输出yuv420sp格式 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetYCbCr420spFlag
	*/
	HI_S32 HI_JPEG_SetYCbCr420spFlag(const struct jpeg_decompress_struct *cinfo,const HI_BOOL bOutYCbCr420sp);


	/** 
	\brief set if output lu pixle sum value. CNcomment:设置是否统计亮度值标识 CNend
	\attention \n
	HI_JPEG_SetLuPixSumFlag should have called jpeg_create_decompress.\n
	CNcomment:在调用HI_JPEG_SetLuPixSumFlag之前必须已经创建好了解码器 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[in]	bLuPixSum. CNcomment:设置是否统计亮度值标识 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetLuPixSumFlag
	*/
	HI_S32 HI_JPEG_SetLuPixSumFlag(const struct jpeg_decompress_struct *cinfo,const HI_BOOL bLuPixSum);

	/** 
	\brief get lu pixle sum value. CNcomment:获取亮度值 CNend
	\attention \n
	If you want to get the luminance value, you can call this function, \n
	but you should call it after jpeg_start_decompress and have call HI_JPEG_SetLuPixSumFlag.\n
	CNcomment:要是想得到亮度值，可以调用该函数，但必须在jpeg_start_decompress之后调用而且解码 \n
	          之前要调用HI_JPEG_SetLuPixSumFlag CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[out]	u64LuPixSum. CNcomment:输出亮度值 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetLuPixSum
	*/
	HI_S32 HI_JPEG_GetLuPixSum(const struct jpeg_decompress_struct *cinfo,HI_U64 *u64LuPixSum);


	/** 
	\brief get jpeg dec time. CNcomment:获取jpeg解码时间 CNend
	\attention \n
	If you want to know how much the decode cost time ,you can call HI_JPEG_GetDecTime, \n
	but should have called it after jpeg_finish_decompress.\n
	CNcomment:要是想看解码花费了多少时间可以调用该函数，但必须在解码完成之后调用 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[out]	pu32DecTime. CNcomment:输出整个解码时间 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetDecTime
	*/
	HI_S32 HI_JPEG_GetDecTime(const struct jpeg_decompress_struct *cinfo, HI_U32 *pu32DecTime);


	
	/** 
	\brief set the leave memory size. CNcomment:设置剩余内存大小可以供解码使用 CNend
	\attention \n
	should have called it after  create jpeg decoder.\n
	CNcomment:必须在创建解码器之后调用 CNend\n
	
	\param[in]	cinfo. CNcomment:解码对象 CNend
	\param[int] sMemSizeInfo. CNcomment:内存信息 CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetLeaveMemSize
	*/
	HI_S32 HI_JPEG_SetLeaveMemSize(const struct jpeg_decompress_struct *cinfo, HI_JPEG_MEMSIZE_INFO_S sMemSizeInfo);

	/** @} */  /** <!-- ==== API Declaration End ==== */

    /****************************************************************************/



#ifdef __cplusplus
    
#if __cplusplus
   
}
#endif
#endif /* __cplusplus */

#endif /* __HI_JPEG_API_H__*/
