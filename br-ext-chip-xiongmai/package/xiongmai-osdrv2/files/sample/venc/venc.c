/******************************************************************************
  Some simple xm510 video encode functions.

*****************************************************************************
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#include "venc.h"
#include "Camera.h"
const int capsize2width[CAPTURE_SIZE_EXT_V2_NR] =
	{704, 704, 352, 352, 176, 640, 320, 1024, 160,240,926,1280,1280,1600,1920,1920,1872,2048,2592,960,2560,3072,3264,4000,4096};
const int capsize2height[2][CAPTURE_SIZE_EXT_V2_NR] =
{
	{576, 288, 576, 288, 144, 480, 240, 768, 120,192,576,720,960,1200,1080,1200,1408,1536,1944,1080,1440,2048,2448,3000,2160},
	{480, 240, 480, 240, 120, 480, 240, 768, 120,192,576,720,960,1200,1080,1200,1408,1536,1944,1080,1440,2048,2448,3000,2160}
};



XM_S32 SampleSysInit(unsigned int RsltType)
{
    static int sys_inited = 0;

    if(sys_inited == 0)
    {
        XM_MPI_SYS_Init();
        //system("isp &");
        CAMERACFG_TO_ISP stCfg;
        memset(&stCfg,0,sizeof(CAMERACFG_TO_ISP));
        stCfg.u32StdType = 0;
		stCfg.u32RsltType = RsltType;
        stCfg.u32Infrared = (1<<31) | 0;
        camera_isp_task(&stCfg);    
        sys_inited = 1;
    }

	return XM_SUCCESS;
}

/******************************************************************************
* funciton : create Vi Chn
******************************************************************************/
XM_S32 ViChnConfig(VI_CHN vichn, VIDEO_NORM_E enNorm, XM_U32 enSize)
{
    SAMPLE_PRT("ViChn = %d,enNorm = %d,enSize = %d\n",vichn,enNorm,enSize);
	XM_S32 s32Ret;
	VI_CHN_ATTR_S ViChnAttr;

    memset(&ViChnAttr, 0, sizeof(VI_CHN_ATTR_S));
    ViChnAttr.stCapRect.s32X = 0;
    ViChnAttr.stCapRect.s32Y = 0;
    ViChnAttr.stCapRect.u32Width = (enSize == CAPTURE_SIZE_4M)?2296:capsize2width[enSize];
    ViChnAttr.stCapRect.u32Height = capsize2height[enNorm][enSize];
    ViChnAttr.stDestSize.u32Width = capsize2width[enSize];
    ViChnAttr.stDestSize.u32Height = capsize2height[enNorm][enSize];			
    s32Ret = XM_MPI_VI_SetChnAttr(vichn, &ViChnAttr);	
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("set VI Chn attribute failed 0x%x!\n",s32Ret);
        return XM_FAILURE;
    }
    s32Ret = XM_MPI_VI_EnableChn(vichn);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("set VI Chn enable failed 0x%x!\n",s32Ret);
        return XM_FAILURE;	
    }
	
	return XM_SUCCESS;
}
XM_S32 ViChnConfigEx(VI_CHN vichn, VIDEO_NORM_E enNorm, XM_U32 enSize)
{
    SAMPLE_PRT("ViChn = %d,enNorm = %d,enSize = %d\n",vichn,enNorm,enSize);
	XM_S32 s32Ret;
	VI_EXT_CHN_ATTR_S stExtAttr;

    memset(&stExtAttr, 0, sizeof(VI_EXT_CHN_ATTR_S));
    stExtAttr.stDestSize.u32Width = capsize2width[enSize];
    stExtAttr.stDestSize.u32Height = capsize2height[enNorm][enSize];		
    s32Ret = XM_MPI_VI_SetExtChnAttr(vichn, &stExtAttr);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("set VI Chn attribute failed 0x%x!\n",s32Ret);
        return XM_FAILURE;
    }
    s32Ret = XM_MPI_VI_EnableChn(vichn);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("set VI Chn enable failed 0x%x!\n",s32Ret);
        return XM_FAILURE;	
    }
    return XM_SUCCESS;
}

/******************************************************************************
* funciton : save stream
******************************************************************************/
XM_S32 VencSaveStream(FILE *fp, VENC_STREAM_S *pstStream)
{
    XM_S32 i;
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        fwrite(pstStream->pstPack[i].pu8Addr,pstStream->pstPack[i].u32Len, 1, fp);
        fflush(fp);
    }
    return XM_SUCCESS;
}


XM_S32 COMM_SYS_GetPicSize(VIDEO_NORM_E enNorm, XM_U32 enPicSize, SIZE_S *pstSize)
{
	SAMPLE_PRT("enNorm = %d,enPicSize = %d\n",enNorm,enPicSize);
    if(enPicSize>CAPTURE_SIZE_EXT_V2_NR || enPicSize<CAPTURE_SIZE_D1)
    {
		return XM_FAILURE;
	}
    pstSize->u32Height = capsize2height[enNorm][enPicSize];
    pstSize->u32Width = capsize2width[enPicSize];
    return XM_SUCCESS;
}

/******************************************************************************
* funciton : Start venc stream mode (h264, jpeg)
******************************************************************************/
XM_S32 COMM_VENC_Start(XM_U32 dwType, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, XM_U32 enSize, int enRcMode)
{
	SAMPLE_PRT("VencChn = %d,enType = %d,enNorm = %d,enSize = %d,enRcMode = %d\n",dwType,enType,enNorm,enSize,enRcMode);
    XM_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_ATTR_H264_S stH264Attr;
    VENC_ATTR_H264_CBR_S    stH264Cbr;
    VENC_ATTR_H264_VBR_S    stH264Vbr;
    VENC_ATTR_H264_FIXQP_S  stH264FixQp;
    VENC_ATTR_H265_S stH265Attr;
    VENC_ATTR_H265_CBR_S    stH265Cbr;
    VENC_ATTR_H265_VBR_S    stH265Vbr;
    VENC_ATTR_H265_FIXQP_S  stH265FixQp;
    VENC_ATTR_JPEG_S stJpegAttr;
    SIZE_S stPicSize;

	s32Ret = COMM_SYS_GetPicSize(enNorm, enSize, &stPicSize);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get picture size failed!\n");
        return XM_FAILURE;
    }
    stVencChnAttr.stVeAttr.enType = enType;
    switch(enType)
    {
        case PT_H264:
        {
            stH264Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH264Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH264Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH264Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH264Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height * 3/8;/*stream buffer size*/
            stH264Attr.u32Profile  = 1;/*0: baseline; 1:MP; 2:HP   ? */
            stH264Attr.bByFrame = XM_TRUE;/*get stream mode is slice mode or frame mode?*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrH264e, &stH264Attr,sizeof(VENC_ATTR_H264_S));
            if(SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop            = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH264Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrmRate     = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;/* input (vi) frame rate */
                stH264Cbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;/* target frame rate */
                switch (enSize)
                {
                  case CAPTURE_SIZE_QCIF:
                	   stH264Cbr.u32BitRate = 256; /* average bit rate */
                	   break;
                  case CAPTURE_SIZE_QVGA:    /* 320 * 240 */
                  case CAPTURE_SIZE_CIF:	

                	   stH264Cbr.u32BitRate = 512;
                       break;

                  case CAPTURE_SIZE_D1:
                  case CAPTURE_SIZE_VGA:	   /* 640 * 480 */
                	   stH264Cbr.u32BitRate = 768;
                       break;
                  case CAPTURE_SIZE_720P:   /* 1280 * 720 */
					   stH264Cbr.u32BitRate = 2188;
                	   break;
		          case CAPTURE_SIZE_1_3M:   /* 1280 * 960 */	
					   stH264Cbr.u32BitRate = 2188;
                	   break;
						 
                  case CAPTURE_SIZE_1080P:  /* 1920 * 1080 */
                  	   stH264Cbr.u32BitRate = 1024*2;
                	   break;
				  case CAPTURE_SIZE_4M:  /* 2560 * 1440 */
                  	   stH264Cbr.u32BitRate = 4096;
                	   break;
                  default :
                       stH264Cbr.u32BitRate = 1024;
                       break;
                }
                stH264Cbr.u32FluctuateLevel = 0; /* average bit rate */
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode) 
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH264FixQp.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH264FixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH264FixQp.u32IQp = 20;
                stH264FixQp.u32PQp = 23;
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264FixQp, &stH264FixQp,sizeof(VENC_ATTR_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode) 
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH264Vbr.u32StatTime = 1;
                stH264Vbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH264Vbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH264Vbr.u32MinQp = 24;
                stH264Vbr.u32MaxQp = 32;
                switch (enSize)
                {
                  case CAPTURE_SIZE_QCIF:
                	   stH264Vbr.u32MaxBitRate= 256*3; /* average bit rate */
                	   break;
                  case CAPTURE_SIZE_QVGA:    /* 320 * 240 */
                  case PIC_CIF:
                	   stH264Vbr.u32MaxBitRate = 512*3;
                       break;
                  case CAPTURE_SIZE_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   stH264Vbr.u32MaxBitRate = 1024*2*3;
                       break;
                  case CAPTURE_SIZE_720P:   /* 1280 * 720 */
                	   stH264Vbr.u32MaxBitRate = 1024*3*3;
                	   break;
		    	  case CAPTURE_SIZE_1_3M:   /* 1280 * 960 */	//cly 0524
                	   stH264Vbr.u32MaxBitRate = 1024*4*3;
                	   break;			   
                  case CAPTURE_SIZE_1080P:  /* 1920 * 1080 */
                  	   stH264Vbr.u32MaxBitRate = 1024*6*3;
                	   break;
				  case CAPTURE_SIZE_4M:  /* 2560 * 1440 */
                  	   stH264Cbr.u32BitRate = 4096;
                	   break;
                  default :
                       stH264Vbr.u32MaxBitRate = 1024*4*3;
                       break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
            }
            else
            {
                return XM_FAILURE;
            }
        }
        break;
        case PT_H265:
        {
            stH265Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH265Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH265Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH265Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH265Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height * 3/8;/*stream buffer size*/
            stH265Attr.u32Profile  = 1;/*0: baseline; 1:MP; 2:HP   ? */
            stH265Attr.bByFrame = XM_TRUE;/*get stream mode is slice mode or frame mode?*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrH265e, &stH265Attr,sizeof(VENC_ATTR_H265_S));
            if(SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH265Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrmRate     = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;/* input (vi) frame rate */
                stH265Cbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;/* target frame rate */
                switch (enSize)
                {
                  case CAPTURE_SIZE_QCIF:
                	   stH265Cbr.u32BitRate = 256; /* average bit rate */
                	   break;
                  case CAPTURE_SIZE_QVGA:    /* 320 * 240 */
                  case CAPTURE_SIZE_CIF:	

                	   stH265Cbr.u32BitRate = 512;
                       break;

                  case CAPTURE_SIZE_D1:
                  case CAPTURE_SIZE_VGA:	   /* 640 * 480 */
                	   stH265Cbr.u32BitRate = 768;
                       break;
                  case CAPTURE_SIZE_720P:   /* 1280 * 720 */
					   stH265Cbr.u32BitRate = 2188;
                	   break;
		          case CAPTURE_SIZE_1_3M:   /* 1280 * 960 */	
					   stH265Cbr.u32BitRate = 2188;
                	   break;
						 
                  case CAPTURE_SIZE_1080P:  /* 1920 * 1080 */
                  	   stH265Cbr.u32BitRate = 1024*2;
                	   break;
				  case CAPTURE_SIZE_4M:  /* 2560 * 1440 */
                  	   stH264Cbr.u32BitRate = 4096;
                	   break;
                  default :
                       
                       break;
                }
                stH265Cbr.u32FluctuateLevel = 0; /* average bit rate */
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265Cbr, &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode) 
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH265FixQp.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH265FixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH265FixQp.u32IQp = 20;
                stH265FixQp.u32PQp = 23;
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265FixQp, &stH265FixQp,sizeof(VENC_ATTR_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode) 
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH265Vbr.u32StatTime = 1;
                stH265Vbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH265Vbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
                stH265Vbr.u32MinQp = 24;
                stH265Vbr.u32MaxQp = 32;
                switch (enSize)
                {
                  case CAPTURE_SIZE_QCIF:
                	   stH265Vbr.u32MaxBitRate= 256*3; /* average bit rate */
                	   break;
                  case CAPTURE_SIZE_QVGA:    /* 320 * 240 */
                  case PIC_CIF:
                	   stH265Vbr.u32MaxBitRate = 512*3;
                       break;
                  case CAPTURE_SIZE_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   stH265Vbr.u32MaxBitRate = 1024*2*3;
                       break;
                  case CAPTURE_SIZE_720P:   /* 1280 * 720 */
                	   stH265Vbr.u32MaxBitRate = 1024*3*3;
                	   break;
		    	  case CAPTURE_SIZE_1_3M:   /* 1280 * 960 */	//cly 0524
                	   stH265Vbr.u32MaxBitRate = 1024*4*3;
                	   break;			   
                  case CAPTURE_SIZE_1080P:  /* 1920 * 1080 */
                  	   stH265Vbr.u32MaxBitRate = 1024*6*3;
                	   break;
				  case CAPTURE_SIZE_4M:  /* 2560 * 1440 */
                  	   stH264Cbr.u32BitRate = 4096;
                	   break;
                  default :
                       stH265Vbr.u32MaxBitRate = 1024*4*3;
                       break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265Vbr, &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
            }
            else
            {
                return XM_FAILURE;
            }
        }
        break; 
        case PT_JPEG:
        	SAMPLE_PRT("set JpegAttr.\n");
            stJpegAttr.u32PicWidth  = stPicSize.u32Width;
            stJpegAttr.u32PicHeight = stPicSize.u32Height;
            stJpegAttr.u32MaxPicWidth= stPicSize.u32Width;
            stJpegAttr.u32MaxPicHeight= stPicSize.u32Height;
            stJpegAttr.u32BufSize = stPicSize.u32Width * stPicSize.u32Height * 3 / 4;
            stJpegAttr.bByFrame = XM_TRUE;/*get stream mode is field mode  or frame mode*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrJpeg, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        default:
            return XM_ERR_VENC_NOT_SUPPORT;
    }
    s32Ret = XM_MPI_VENC_CreateChn(dwType, &stVencChnAttr);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("XM_MPI_VENC_CreateChn [%d] faild with %#x!",dwType, s32Ret);
        return s32Ret;
    }
    return XM_SUCCESS;

}

/******************************************************************
*  funciton : get venc stream
*******************************************************************/
XM_VOID* VencGetStreamProc(XM_VOID *p)
{
	XM_S32 s32Ret;
	int i;
	VENC_STREAM_S vencStream;
	VENC_STREAM_S *pVencStream = &vencStream;
	XM_S32 s32ChnTotal;
	FILE *pFile[VENC_MAX_CHN_NUM];
	SAMPLE_VENC_GETSTREAM_PARA_S  *pstPara;
	XM_CHAR aszFileName[VENC_MAX_CHN_NUM][64];

	pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
	s32ChnTotal = pstPara->s32Cnt;
	if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }

    pVencStream->pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S));

	for (i = 0; i < s32ChnTotal; i++)
    {
        sprintf(aszFileName[i], "stream_chn%d",i);
        pFile[i] = fopen(aszFileName[i], "wb");
        if (!pFile[i])
        {
            SAMPLE_PRT("open file[%s] failed!\n", aszFileName[i]);
            return NULL;
        }
    }

	while(pstPara->bThreadStart)
	{
		XM_MPI_VENC_SelectData();
		for( i = 0 ; i< s32ChnTotal; i++ )
		{
			s32Ret = XM_MPI_VENC_GetStream(i, pVencStream, XM_FALSE);
            if (XM_SUCCESS != s32Ret)
            {
				break;
            }
			s32Ret = VencSaveStream(pFile[i], pVencStream);
            if (XM_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("save stream failed!\n");
            }
			s32Ret = XM_MPI_VENC_ReleaseStream(i, pVencStream);
            if (XM_SUCCESS != s32Ret)
            {
            	SAMPLE_PRT("XM_MPI_VENC_ReleaseStream [%d] failed!\n", i);
                break;
            }
		}
		usleep(10);
	}

    for (i = 0; i < s32ChnTotal; i++)
    {
        fclose(pFile[i]);
    }
	
	return 0;
}

int VencSetTitleV2(int streamType, CAPTURE_TITLE_PARAM_V2 *pParam)
{
    XM_S32 s32Ret = XM_FAILURE;
    RGN_HANDLE RgnHandle = 0;
    RGN_ATTR_S stRgnAttr;
    BITMAP_S stBitmap;
    int magn = 2;   //鏀惧ぇ鍊嶆暟

    static unsigned int rgnFlag[2] = {0};

    RgnHandle = pParam->index;
    if (pParam->enable == 0 || pParam->pixel == NULL)
    {
        if (rgnFlag[streamType] & (1 << RgnHandle))
        {
            s32Ret = XM_MPI_RGN_Disable(streamType, RgnHandle, OVERLAY_RGN);
            if (XM_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("XM_MPI_RGN_Disable [%d] failed with %#x\n", RgnHandle, s32Ret);
            }
        }
        return XM_SUCCESS;
    }

    if (rgnFlag[streamType] & 1 << RgnHandle)
    {
        stRgnAttr.enType = OVERLAY_RGN;
        stRgnAttr.u32Handle = RgnHandle;
        s32Ret = XM_MPI_RGN_GetAttr(streamType, &stRgnAttr);
        if (XM_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("XM_MPI_RGN_GetAttr (%d) failed with %#x!\n", RgnHandle, s32Ret);
            return XM_FAILURE;
        }

        if (stRgnAttr.unAttr.stOverlay.stRect.u32Height != pParam->height || stRgnAttr.unAttr.stOverlay.stRect.u32Width != pParam->width)
        {
            s32Ret = XM_MPI_RGN_Destroy(streamType, &stRgnAttr);
            if (XM_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("XM_MPI_RGN_Destroy [%d] failed with %#x\n", RgnHandle, s32Ret);
            }
            rgnFlag[streamType] &= ~(1 << RgnHandle);
        }
    }

    stRgnAttr.enType = OVERLAY_RGN;
    stRgnAttr.u32Handle = RgnHandle;
    stRgnAttr.unAttr.stOverlay.u32ColorMap = 0x8F0;
    stRgnAttr.unAttr.stOverlay.u32Format = 1;
    stRgnAttr.unAttr.stOverlay.u32Effect = (pParam->bg_color >> 30) | ((magn-1) << 16) | ((magn-1) << 12);
    stRgnAttr.unAttr.stOverlay.stRect.s32X = pParam->x;
    stRgnAttr.unAttr.stOverlay.stRect.s32Y = pParam->y;
    stRgnAttr.unAttr.stOverlay.stRect.u32Width = pParam->width / 2 * 2;
    stRgnAttr.unAttr.stOverlay.stRect.u32Height = pParam->height / 2 * 2;

    if (rgnFlag[streamType] & (1 << RgnHandle))
    {
        s32Ret = XM_MPI_RGN_SetAttr(streamType, &stRgnAttr);
        if (XM_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("XM_MPI_RGN_SetAttr (%d) failed with %#x!\n", RgnHandle, s32Ret);
            return XM_FAILURE;
        }
    }
    else
    {
        SAMPLE_PRT("chn = %d\n", streamType);
        s32Ret = XM_MPI_RGN_Create(streamType, &stRgnAttr);
        if (XM_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("XM_MPI_RGN_Create (%d) failed with %#x!\n", RgnHandle, s32Ret);
            return XM_FAILURE;
        }
        rgnFlag[streamType] |= 1 << RgnHandle;
    }

    stBitmap.u32Handle = RgnHandle;
    stBitmap.u32Width = pParam->width;
    stBitmap.u32Height = pParam->height;
    stBitmap.u32Format = 1;
    stBitmap.pData = (XM_VOID *)pParam->pixel;

    s32Ret = XM_MPI_RGN_SetBitMap(streamType, &stBitmap);
    if (s32Ret != XM_SUCCESS)
    {
        SAMPLE_PRT("XM_MPI_RGN_SetBitMaperror:%x\n", s32Ret);
        return XM_FAILURE;
    }
    XM_MPI_RGN_Enable(streamType, RgnHandle, OVERLAY_RGN);

    return XM_SUCCESS;
}

/******************************************************************************
* funciton : 720P Set Title
******************************************************************************/
XM_S32 VencSetTitle(CAPTURE_TITLE_PARAM *pParam)
{
	XM_S32 s32Ret;
	
	RGN_ATTR_S stRgnAttr;
	BITMAP_S stBitmap;
	VI_CHN ViChn = 0;
	RGN_HANDLE RgnHandle = 0;

	stRgnAttr.enType = OVERLAY_RGN;
	stRgnAttr.u32Handle = RgnHandle;
	stRgnAttr.unAttr.stOverlay.u32ColorMap = 0xF0;
	stRgnAttr.unAttr.stOverlay.u32Format = 0;
	stRgnAttr.unAttr.stOverlay.u32Effect = (pParam->bg_color >> 30);
	stRgnAttr.unAttr.stOverlay.stRect.s32X = pParam->x;
	stRgnAttr.unAttr.stOverlay.stRect.s32Y = pParam->y;
	stRgnAttr.unAttr.stOverlay.stRect.u32Width = pParam->width/2*2;
	stRgnAttr.unAttr.stOverlay.stRect.u32Height = pParam->height/2*2;
	
	s32Ret = XM_MPI_RGN_Create(ViChn, &stRgnAttr);
	if(XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("XM_MPI_RGN_Create failed!\n");
		return XM_FAILURE;
	}

	stBitmap.u32Handle     = RgnHandle;
	stBitmap.u32Width	   = pParam->width;
	stBitmap.u32Height	   = pParam->height;
    stBitmap.u32Format	   = 0; //鍗曡壊鍓嶆櫙
	stBitmap.pData = (XM_VOID *)pParam->pixel;
		
	s32Ret = XM_MPI_RGN_SetBitMap(ViChn,&stBitmap);
	if(s32Ret != XM_SUCCESS)
	{
		SAMPLE_PRT("XM_MPI_RGN_SetBitMaperror:%#x\n", s32Ret);
		return XM_FAILURE;
	}
	
	s32Ret = XM_MPI_RGN_Enable(ViChn,RgnHandle,OVERLAY_RGN);
	if(s32Ret != XM_SUCCESS)
	{
		SAMPLE_PRT("XM_MPI_RGN_Enable failed!\n");
		return XM_FAILURE;
	}
	
	return XM_SUCCESS;
}


/******************************************************************************
* function :  H.264@720p@25fps+H.264@D1@25fps
******************************************************************************/
XM_S32 SAMPLE_VENC_720P_CLASSIC(int seconds)
{
	XM_S32 s32Cnt = 0;
	XM_S32 s32Ret;
	VENC_CHN VencChn = 0;
    VI_CHN ViChn = 0;
    SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
    pthread_t gs_VencPid;
	
    s32Ret = SampleSysInit(0);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SampleSysInit failed!\n");
		return XM_FAILURE;
	}

    ViChn = 0;
    s32Ret = ViChnConfig(ViChn, VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_720P);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start ViChn %d failed!\n", ViChn);
        return XM_FAILURE;
    }
    ViChn = 1;
    s32Ret = ViChnConfigEx(ViChn, VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_D1);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start ViChn %d failed!\n", ViChn);
        return XM_FAILURE;
    }

    VencChn = 0;
    s32Ret = COMM_VENC_Start(VencChn, PT_H264,VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_720P,SAMPLE_RC_CBR);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VencChn %d failed!\n", VencChn);
        goto END_VI_STOP;
    }
	s32Cnt += 1;
	VencChn = 1;
	s32Ret = COMM_VENC_Start(VencChn, PT_H264,VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_D1,SAMPLE_RC_CBR);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VencChn %d failed!\n", VencChn);
        goto END_VI_STOP;
    }
	s32Cnt += 1;
	for(VencChn = 0; VencChn < s32Cnt; VencChn++)
	{
	 	s32Ret = XM_MPI_VENC_StartRecvPic(VencChn);
		if (XM_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("XM_MPI_VENC_StartRecvPic [%d] failed!\n", VencChn);
	        goto END_VENC_DESTORY;
	    }
	}

    gs_stPara.bThreadStart = XM_TRUE;
	gs_stPara.s32Cnt = s32Cnt;
	s32Ret = pthread_create(&gs_VencPid, 0, VencGetStreamProc, (XM_VOID*)&gs_stPara);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VENC_StartGetStream failed!\n");
		goto END_VENC_STOP_RECV_PIC;
	}
	sleep(seconds);

    gs_stPara.bThreadStart = XM_FALSE;
    pthread_join(gs_VencPid, 0);

END_VENC_STOP_RECV_PIC:
	for(VencChn = 0; VencChn < s32Cnt; VencChn++)
	{
		s32Ret = XM_MPI_VENC_StopRecvPic(VencChn);
		if (s32Ret != XM_SUCCESS)
		{
			SAMPLE_PRT("XM_MPI_VENC_StopRecvPic [%d] err 0x%x",VencChn, s32Ret);
		}
	}
END_VENC_DESTORY:
	for(VencChn = 0; VencChn < s32Cnt; VencChn++)
	{
		s32Ret = XM_MPI_VENC_DestroyChn(VencChn);
		if (XM_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("XM_MPI_VENC_DestroyChn [%d] faild with %#x!",VencChn, s32Ret);
		}
	}
END_VI_STOP:
    for(ViChn = 0; ViChn < s32Cnt; ViChn++)
	{
		s32Ret = XM_MPI_VI_DisableChn(ViChn);
		if (XM_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("XM_MPI_VI_DisableChn [%d] faild with %#x!",ViChn, s32Ret);
		}
	}
	return s32Ret;  
}

/******************************************************************************
* function :  H.265@400W@25fps+H.264@D1@25fps
******************************************************************************/
XM_S32 SAMPLE_VENC_400W_CLASSIC(int seconds)
{
	XM_S32 s32Cnt = 0;
	XM_S32 s32Ret;
	VENC_CHN VencChn = 0;
    VI_CHN ViChn = 0;
    SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
    pthread_t gs_VencPid;
	
    s32Ret = SampleSysInit(4);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SampleSysInit failed!\n");
		return XM_FAILURE;
	}

    ViChn = 0;
    s32Ret = ViChnConfig(ViChn, VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_4M);//此处待修改2560*1440
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start ViChn %d failed!\n", ViChn);
        return XM_FAILURE;
    }
    ViChn = 1;
    s32Ret = ViChnConfigEx(ViChn, VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_D1);//此处待修改2560*1440
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start ViChn %d failed!\n", ViChn);
        return XM_FAILURE;
    }

    VencChn = 0;
    s32Ret = COMM_VENC_Start(VencChn, PT_H264,VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_4M,SAMPLE_RC_CBR);//此处待修改2560*1440
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VencChn %d failed!\n", VencChn);
        goto END_VI_STOP;
    }
	s32Cnt += 1;
	VencChn = 1;
	s32Ret = COMM_VENC_Start(VencChn, PT_H264,VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_D1,SAMPLE_RC_CBR);//此处待修改2560*1440
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VencChn %d failed!\n", VencChn);
        goto END_VI_STOP;
    }
	s32Cnt += 1;
	for(VencChn = 0; VencChn < s32Cnt; VencChn++)
	{
	 	s32Ret = XM_MPI_VENC_StartRecvPic(VencChn);
		if (XM_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("XM_MPI_VENC_StartRecvPic [%d] failed!\n", VencChn);
	        goto END_VENC_DESTORY;
	    }
	}

    gs_stPara.bThreadStart = XM_TRUE;
	gs_stPara.s32Cnt = s32Cnt;
	s32Ret = pthread_create(&gs_VencPid, 0, VencGetStreamProc, (XM_VOID*)&gs_stPara);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VENC_StartGetStream failed!\n");
		goto END_VENC_STOP_RECV_PIC;
	}
	sleep(seconds);

    gs_stPara.bThreadStart = XM_FALSE;
    pthread_join(gs_VencPid, 0);

END_VENC_STOP_RECV_PIC:
	for(VencChn = 0; VencChn < s32Cnt; VencChn++)
	{
		s32Ret = XM_MPI_VENC_StopRecvPic(VencChn);
		if (s32Ret != XM_SUCCESS)
		{
			SAMPLE_PRT("XM_MPI_VENC_StopRecvPic [%d] err 0x%x",VencChn, s32Ret);
		}
	}
END_VENC_DESTORY:
	for(VencChn = 0; VencChn < s32Cnt; VencChn++)
	{
		s32Ret = XM_MPI_VENC_DestroyChn(VencChn);
		if (XM_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("XM_MPI_VENC_DestroyChn [%d] faild with %#x!",VencChn, s32Ret);
		}
	}
END_VI_STOP:
    for(ViChn = 0; ViChn < s32Cnt; ViChn++)
	{
		s32Ret = XM_MPI_VI_DisableChn(ViChn);
		if (XM_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("XM_MPI_VI_DisableChn [%d] faild with %#x!",ViChn, s32Ret);
		}
	}
	return s32Ret;  
}

/******************************************************************************
* H.264@720p@25fps with Title
******************************************************************************/
XM_S32 SAMPLE_VENC_720P_SetTitle(int seconds)
{
	XM_S32 s32Ret;
	XM_S32 s32Cnt = 0;
	VENC_CHN VencChn = 0;
    VI_CHN ViChn = 0;
	SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
    pthread_t gs_VencPid;

    XM_U8 thePixel[] = {
		0x10,0x40,0x00,0x04,0x10,0xA0,0x47,0xFE,
        0x10,0x88,0x31,0x00,0xFD,0xFC,0x11,0x08,
        0x11,0x20,0x01,0xFC,0x13,0x28,0x01,0x08,
        0x25,0xFC,0xF1,0x08,0x21,0x20,0x11,0x08,
        0x51,0x28,0x11,0x08,0x51,0xFC,0x11,0x08,
        0x91,0x20,0x12,0x08,0x25,0x20,0x14,0x50,
        0x45,0x24,0x10,0x20,0x7D,0xFE,0x28,0x06,
        0x01,0x00,0x47,0xFC,0x01,0x00,0x00,0x00
	};
    XM_U8 secondPixel[] = {      //2bit妯″紡锛屾瘡涓儚绱犵偣鍗犵敤2bit绌洪棿
        0x00,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,
        0x0a,0x00,0x00,0x00,0x00,0x80,0x0a,0x5a,
        0x02,0xa8,0x00,0x00,0xaa,0x80,0xa5,0xa0,
        0x02,0x80,0x02,0x60,0x25,0x55,0x89,0x56,
        0x00,0x02,0x55,0x62,0x55,0x58,0x09,0x60,
        0x09,0x58,0x95,0xa9,0x69,0x56,0x00,0x09,
        0x56,0x89,0x5a,0x96,0x25,0x58,0x09,0x58,
        0x96,0x02,0x5a,0x58,0x00,0x25,0x68,0x09,
        0x60,0x25,0xa5,0x58,0x25,0x98,0x96,0x02,
        0x5a,0x58,0x00,0x95,0x80,0x09,0x60,0x25,
        0xa5,0x58,0x25,0x98,0x96,0xaa,0x5a,0x58,
        0x00,0x96,0x00,0x09,0x6a,0xa5,0xa5,0x96,
        0x25,0x96,0x95,0x55,0x5a,0x58,0x00,0x96,
        0x00,0x09,0x55,0x55,0xa5,0x96,0x26,0x96,
        0x95,0x55,0x5a,0x58,0x00,0x96,0x00,0x09,
        0x55,0x55,0xa6,0x96,0x96,0x96,0x96,0xaa,
        0x5a,0x58,0x00,0x96,0x00,0x09,0x6a,0xa5,
        0xa6,0x25,0x96,0x96,0x96,0x02,0x5a,0x58,
        0x00,0x96,0x00,0x09,0x60,0x25,0x96,0x25,
        0x96,0x96,0x96,0x02,0x5a,0x58,0x00,0x95,
        0x80,0x09,0x60,0x25,0x96,0x25,0x58,0x96,
        0x96,0x02,0x5a,0x58,0x00,0x25,0x6a,0x89,
        0x60,0x25,0x96,0x09,0x58,0x96,0x96,0x02,
        0x59,0x56,0x00,0x09,0x55,0x69,0x60,0x25,
        0x96,0x09,0x58,0x26,0x96,0x02,0x59,0x56,
        0x00,0x02,0x55,0x69,0x60,0x25,0x96,0x09,
        0x60,0x25,0xa8,0x00,0xa2,0xa8,0x00,0x00,
        0xaa,0x82,0x80,0x0a,0x28,0x02,0x80,0x0a
    };

	CAPTURE_TITLE_PARAM Param = 
	{
		.x = 10,
		.y = 10,
		.width = 32,
		.height = 16,
		.fg_color = 0x030303,					
		.bg_color = 0xFFFFFF,					
	 	.pixel = thePixel,
	};

    CAPTURE_TITLE_PARAM_V2 ParamV2 = 
	{
        .index = 1,
        .enable = 1,
		.x = 1000,
		.y = 100,
		.width = 56,
		.height = 16,
		.fg_color = 0x030303,					
		.bg_color = 0xFFFFFF,					
	 	.pixel = secondPixel,
	};

    s32Ret = SampleSysInit(0);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SampleSysInit failed!\n");
		return XM_FAILURE;
	}

    RGN_PALLET_S pallet = {{0x008080, 0x216EB9, 0x415BF1, 0x8235C1, 0xA21193, 0x813623, 0x19F16E, 0x5CB3B4, 
                                                        0x408080, 0x71488A, 0x4D9348, 0x9AA611, 0x179398, 0x4E93E7, 0x9BA5AF, 0xDB8080}};
    XM_MPI_RGN_SetPallet(0, &pallet);
    XM_MPI_RGN_SetPallet(1, &pallet);

    s32Ret = ViChnConfig(ViChn, VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_720P);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start ViChn %d failed!\n", ViChn);
        return XM_FAILURE;
    }

	s32Ret = VencSetTitle(&Param);
	if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Title failed!\n");
        goto END_VI_STOP;
    }
    s32Ret = VencSetTitleV2(ViChn, &ParamV2);
	if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set TitleV2 failed!\n");
        goto END_VI_STOP;
    }
	
    s32Ret = COMM_VENC_Start(VencChn, PT_H264,VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_720P,SAMPLE_RC_CBR);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VI_STOP;
    }
	s32Cnt += 1;
		
	s32Ret = XM_MPI_VENC_StartRecvPic(VencChn);
	if (XM_SUCCESS != s32Ret)
	{
	    SAMPLE_PRT("XM_MPI_VENC_StartRecvPic [%d] failed!\n", VencChn);
	    goto END_VENC_DESTORY;
	}

	gs_stPara.bThreadStart = XM_TRUE;
	gs_stPara.s32Cnt = s32Cnt;
	s32Ret = pthread_create(&gs_VencPid, 0, VencGetStreamProc, (XM_VOID*)&gs_stPara);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VENC_StartGetStream failed!\n");
		goto END_VENC_STOP_RECV_PIC;
	}

	sleep(seconds);

    gs_stPara.bThreadStart = XM_FALSE;
    pthread_join(gs_VencPid, 0);

END_VENC_STOP_RECV_PIC:
	s32Ret = XM_MPI_VENC_StopRecvPic(VencChn);
    if (XM_SUCCESS != s32Ret)
	{
	    SAMPLE_PRT("XM_MPI_VENC_StopRecvPic [%d] failed!\n", VencChn);
	}
END_VENC_DESTORY:
	s32Ret = XM_MPI_VENC_DestroyChn(VencChn);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("XM_MPI_VENC_DestroyChn [%d] faild with %#x!",VencChn, s32Ret);
    }
END_VI_STOP:
    s32Ret = XM_MPI_VI_DisableChn(ViChn);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("XM_MPI_VI_DisableChn [%d] faild with %#x!",ViChn, s32Ret);
    }
	return s32Ret; 
}

/******************************************************************************
* function : Capture JPEG
******************************************************************************/
XM_S32 SAMPLE_VENC_CaptureJPEG(int num)
{
	XM_S32 s32Ret, count;
	VENC_CHN VencChn = 0;
	VENC_CHN jpegChn = 2;
    VI_CHN ViChn = 0;
    FILE *fp;
	XM_CHAR jpegFileName[64];
    VENC_STREAM_S stStream;

	s32Ret = SampleSysInit(0);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SampleSysInit failed!\n");
		return XM_FAILURE;
	}

    s32Ret = ViChnConfig(ViChn, VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_720P);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start ViChn %d failed!\n", ViChn);
        return XM_FAILURE;
    }

	s32Ret = COMM_VENC_Start(VencChn, PT_H264,VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_720P,SAMPLE_RC_CBR);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        return XM_FAILURE;
    }

	s32Ret = COMM_VENC_Start(jpegChn, PT_JPEG,VIDEO_ENCODING_MODE_PAL,CAPTURE_SIZE_720P,0);
	if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc JPEG failed!\n");
        return XM_FAILURE;
    }
	
	stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S));
    for(count = 1; count <= num; count++)
    {
        sleep(1);

        s32Ret = XM_MPI_VENC_StartRecvPic(jpegChn);
        if (s32Ret != XM_SUCCESS)
        {
            SAMPLE_PRT("XM_MPI_VENC_StartRecvPic err !\n");
            break;
        }
                
        s32Ret = XM_MPI_VENC_GetStream(jpegChn, &stStream, XM_TRUE);
        if (XM_SUCCESS != s32Ret) 
        {
            SAMPLE_PRT("XM_MPI_VENC_GetStream:%#x\n",s32Ret);
            continue;
        }

        sprintf(jpegFileName, "%d.jpeg", count);
        fp = fopen(jpegFileName, "w+");
        s32Ret = VencSaveStream(fp, &stStream);
        fclose(fp);
                
        s32Ret = XM_MPI_VENC_StopRecvPic(jpegChn);
        if (XM_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("XM_VENC_StopCH jpegChn %d failed %#X\n", jpegChn, s32Ret);
            break;
        }
                
        s32Ret = XM_MPI_VENC_ReleaseStream(jpegChn, &stStream);
        if ( s32Ret != XM_SUCCESS )
        {
            SAMPLE_PRT("XM_MPI_VENC_ReleaseStream failed %#x\n", s32Ret);
        }
    }  

	s32Ret = XM_MPI_VENC_DestroyChn(jpegChn);
	if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("XM_MPI_VENC_DestroyChn jpeg faild!\n");
    }

	s32Ret = XM_MPI_VENC_DestroyChn(VencChn);
	if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("XM_MPI_VENC_DestroyChn [%d] faild!\n", VencChn);
    }

    s32Ret = XM_MPI_VI_DisableChn(ViChn);
    if (XM_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("XM_MPI_VI_DisableChn [%d] faild with %#x!",ViChn, s32Ret);
    }

	return XM_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
