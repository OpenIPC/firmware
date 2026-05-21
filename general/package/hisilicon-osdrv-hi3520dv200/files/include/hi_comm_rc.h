/******************************************************************************

  Copyright (C), 2001-2012, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_rc.h
  Version       : 
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2006/11/24
  Last Modified :
  Description   : common struct definition for Rate control
  Function List :
  History       :
******************************************************************************/
#ifndef __HI_COMM_RC_H__
#define __HI_COMM_RC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef HI_U32 HI_FR32;

typedef enum hiVENC_RC_MODE_E
{
    VENC_RC_MODE_H264CBR = 1,    
    VENC_RC_MODE_H264VBR,        
    VENC_RC_MODE_H264ABR,        
    VENC_RC_MODE_H264FIXQP,      

    VENC_RC_MODE_MJPEGCBR,    
    VENC_RC_MODE_MJPEGVBR,        
    VENC_RC_MODE_MJPEGABR,        
    VENC_RC_MODE_MJPEGFIXQP,  

    VENC_RC_MODE_MPEG4CBR,    
    VENC_RC_MODE_MPEG4VBR,        
    VENC_RC_MODE_MPEG4ABR,        
    VENC_RC_MODE_MPEG4FIXQP,  

    VENC_RC_MODE_H264CBRv2,
    VENC_RC_MODE_H264VBRv2,
    VENC_RC_MODE_MJPEGCBRv2,    
    VENC_RC_MODE_MJPEGVBRv2,     
    VENC_RC_MODE_BUTT,

}VENC_RC_MODE_E;


typedef struct hiVENC_ATTR_H264_FIXQP_S
{
    HI_U32      u32Gop;                 /*the interval of ISLICE. */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */     
    HI_U32      u32IQp;                 /* qp of the i frame */
    HI_U32      u32PQp;                 /* qp of the p frame */
} VENC_ATTR_H264_FIXQP_S;

typedef struct hiVENC_ATTR_H264_CBR_S
{
    HI_U32      u32Gop;                 /*the interval of ISLICE. */
    HI_U32      u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */ 
    HI_U32      u32BitRate;             /* average bitrate */
    HI_U32      u32FluctuateLevel;      /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
} VENC_ATTR_H264_CBR_S;                                         
                                         
typedef struct hiVENC_ATTR_H264_VBR_S
{
    HI_U32      u32Gop;                 /*the interval of ISLICE. */
    HI_U32      u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */     
    HI_U32      u32MaxBitRate;          /* the max bitrate */                      
    HI_U32      u32MaxQp;               /* the max qp */
    HI_U32      u32MinQp;               /* the min qp */
}VENC_ATTR_H264_VBR_S;

typedef struct hiVENC_ATTR_H264_ABR_S
{
    HI_U32      u32Gop;                 /*the interval of ISLICE. */
    HI_U32      u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */    
    HI_U32      u32AvgBitRate;          /* average bitrate */        
    HI_U32      u32MaxBitRate;          /* the max bitrate */
}VENC_ATTR_H264_ABR_S;

typedef struct hiVENC_ATTR_MPEG4_FIXQP_S
{
    HI_U32      u32Gop;                 /*the interval of ISLICE. */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */     
    HI_U32      u32IQp;                 /* qp of the i frame */
    HI_U32      u32PQp;                 /* qp of the p frame */
}VENC_ATTR_MPEG4_FIXQP_S;

typedef struct hiVENC_ATTR_MPEG4_CBR_S
{
    HI_U32      u32Gop;                 /*the interval of ISLICE. */
    HI_U32      u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */ 
    HI_U32      u32BitRate;             /* average bitrate */
    HI_U32      u32FluctuateLevel;      /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
}VENC_ATTR_MPEG4_CBR_S;

typedef struct hiVENC_ATTR_MPEG4_VBR_S
{
    HI_U32      u32Gop;                 /* the interval of ISLICE. */
    HI_U32      u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */     
    HI_U32      u32MaxBitRate;          /* the max bitrate */                      
    HI_U32      u32MaxQp;               /* the max qp */
    HI_U32      u32MinQp;               /* the min qp */
}VENC_ATTR_MPEG4_VBR_S;
 
typedef struct hiVENC_ATTR_MJPEG_FIXQP_S
{
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate;      /* the target frame rate of the venc chnnel */
    HI_U32      u32Qfactor;             /* image quality :[1,99]*/
}VENC_ATTR_MJPEG_FIXQP_S;

typedef struct hiVENC_ATTR_MJPEG_CBR_S
{
    HI_U32      u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate ;     /* the target frame rate of the venc chnnel */
    HI_U32      u32BitRate;             /* average bitrate */
    HI_U32      u32FluctuateLevel;      /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
} VENC_ATTR_MJPEG_CBR_S;

typedef struct hiVENC_ATTR_MJPEG_VBR_S
{
    HI_U32      u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    HI_U32      u32ViFrmRate;           /* the input frame rate of the venc chnnel */
    HI_FR32     fr32TargetFrmRate;      /* the target frame rate of the venc chnnel */
    HI_U32      u32MaxBitRate;          /* max bitrate */
    HI_U32      u32MaxQfactor;          /* max image quailty allowed */
    HI_U32      u32MinQfactor;          /* min image quality allowed */
}VENC_ATTR_MJPEG_VBR_S;

typedef struct hiVENC_RC_ATTR_S
{
    VENC_RC_MODE_E enRcMode;            /*the type of rc*/
    union
    {
        VENC_ATTR_H264_CBR_S    stAttrH264Cbr;
        VENC_ATTR_H264_VBR_S    stAttrH264Vbr;
        VENC_ATTR_H264_FIXQP_S  stAttrH264FixQp;
        VENC_ATTR_H264_ABR_S    stAttrH264Abr;

        VENC_ATTR_MPEG4_CBR_S   stAttrMpeg4Cbr;
        VENC_ATTR_MPEG4_FIXQP_S stAttrMpeg4FixQp;
        VENC_ATTR_MPEG4_VBR_S   stAttrMpeg4Vbr;

        VENC_ATTR_MJPEG_CBR_S   stAttrMjpegeCbr;
        VENC_ATTR_MJPEG_FIXQP_S stAttrMjpegeFixQp;
        VENC_ATTR_MJPEG_VBR_S   stAttrMjpegeVbr;
    };   
    HI_VOID*       pRcAttr ;     /*the rc attribute which could be specified by user*/
 
}VENC_RC_ATTR_S;

typedef enum hiRC_SUPERFRM_MODE_EN
{
    SUPERFRM_NONE,                /* sdk don't care super frame */
    SUPERFRM_DISCARD,             /* the super frame is discarded */
    SUPERFRM_REENCODE,            /* the super frame is re-encode */
    SUPERFRM_BUTT
}VENC_SUPERFRM_MODE_EN;

typedef struct hiVENC_PARAM_H264_CBR_S
{
    HI_U32  u32MinIprop;                    /* the min ratio of i frame and p frame */           
    HI_U32  u32MaxIprop;                    /* the max ratio of i frame and p frame */
    HI_U32  u32MaxQp;                       /* the max QP value */
    HI_U32  u32MaxStartQp;                  /* the max QP value for rate control of frame level,should between [u32MinQp,u32MaxQp] */
    HI_U32  u32MinQp;                       /* the min QP value */
    HI_U32  u32MaxPPDeltaQp;                /* the max qp value difference between two successive P frame */
    HI_U32  u32MaxIPDeltaQp;                /* the max qp value difference between p frame and the next i frame */
    HI_BOOL bLostFrmOpen;                   /* Indicates whether to discard frames to ensure 
                                               stable bit rate when the instant bit rate is exceeded */
    HI_U32  u32LostFrmBpsThr;               /* the instant bit rate threshold */
    VENC_SUPERFRM_MODE_EN  enSuperFrmMode;  /* the super frame mode */
    HI_U32  u32SuperIFrmBitsThr;            /* Indicate the threshold of the super I frame
                                               for enabling the super frame processing mode */            
    HI_U32  u32SuperPFrmBitsThr;            /* Indicate the threshold of the super I frame
                                               for enabling the super frame processing mode */
    HI_S32  s32IPQPDelta;                   /* the qp difference between the i frame and the before gop avarage qp */
    HI_U32  u32RQRatio[8];                  /* the rate stabilization weight, 
                                               100-u32RQRatio[i] is the sequence quality stabilization weight */
    HI_U32  u32MinIQp;                      /* min qp for i frame */
}VENC_PARAM_H264_CBR_S;

typedef struct hiVENC_PARAM_H264_CBRV2_S
{
    HI_U32  u32MinIprop;                    /* the min ratio of i frame and p frame */           
    HI_U32  u32MaxIprop;                    /* the max ratio of i frame and p frame */
    HI_U32  u32MaxQp;                       /* the max QP value */
    HI_U32  u32MaxStartQp;                  /* the max QP value for rate control of frame level,should between [u32MinQp,u32MaxQp] */
    HI_U32  u32MinQp;                       /* the min QP value */
    HI_BOOL bLostFrmOpen;                   /* Indicates whether to discard frames to ensure 
                                               stable bit rate when the instant bit rate is exceeded */
    HI_U32  u32LostFrmBpsThr;               /* the instant bit rate threshold */
    VENC_SUPERFRM_MODE_EN  enSuperFrmMode;  /* the super frame mode */
    HI_U32  u32SuperIFrmBitsThr;            /* Indicate the threshold of the super I frame
                                               for enabling the super frame processing mode */            
    HI_U32  u32SuperPFrmBitsThr;            /* Indicate the threshold of the super I frame
                                               for enabling the super frame processing mode */
    HI_S32  s32IPQPDelta;                   /* the qp difference between the i frame and the before gop avarage qp; == Qp(P) - Qp(I) */
    HI_S32  s32QualityLevel;                /* quality of picture [1, 5] */
    HI_S32  s32MaxReEncodeTimes;            /* max number of re-encode times [0, 3]*/ 
    HI_U32  u32MinIQp;                      /* min qp for i frame */
}VENC_PARAM_H264_CBRV2_S;

typedef struct hiVENC_PARAM_H264_VBR_S
{
    HI_S32 s32DeltaQP;                      /* Indicates the maximum change of QP values of frames 
                                               when the picture quality changes */
    HI_S32 s32ChangePos;                    /* Indicates the ratio of the current bit rate to the maximum 
                                                    bit rate when the QP value starts to be adjusted */
    HI_U32  u32MinIprop;                    /* the min ratio of i frame and p frame */           
    HI_U32  u32MaxIprop;                     /* the max ratio of i frame and p frame */

    HI_BOOL bLostFrmOpen;                   /* Indicates whether to discard frames to ensure 
                                               stable bit rate when the instant bit rate is exceeded */
    HI_U32  u32LostFrmBpsThr;               /* the instant bit rate threshold */
                                               
    VENC_SUPERFRM_MODE_EN  enSuperFrmMode;  /* Indicates the mode of processing the super frame */
    HI_U32  u32SuperIFrmBitsThr;            /* Indicate the threshold of the super I frame 
                                               for enabling the super frame processing mode */              
    HI_U32  u32SuperPFrmBitsThr;            /* Indicate the threshold of the super I frame 
                                               for enabling the super frame processing mode */      
    HI_U32  u32MinIQP;                      /* min qp for i frame */
}VENC_PARAM_H264_VBR_S;

typedef struct hiVENC_PARAM_H264_ABR_S
{
    HI_U32 u32MaxQp;
    HI_U32 u32MinQp;
    HI_S32 s32IPQPDelta;
    HI_U32 u32StartQp;
}VENC_PARAM_H264_ABR_S;

typedef struct hiVENC_PARAM_MJPEG_CBR_S
{
    HI_U32  u32MaxQfactor;                  /* the max Qfactor value*/
    HI_U32  u32MinQfactor;                  /* the min Qfactor value */

    HI_BOOL bLostFrmOpen;                   /* Indicates whether to discard frames to ensure 
                                               stable bit rate when the instant bit rate is exceeded  */
    HI_U32  u32LostFrmBpsThr;               /* the instant bit rate threshold */                 
    
    VENC_SUPERFRM_MODE_EN  enSuperFrmMode;  /* the super frame mode */
    HI_U32  u32SuperFrmBitsThr;             /* Indicate the threshold of the super frame 
                                               for enabling the super frame processing mode */            
    HI_U32  u32RQRatio[8];                  /* the rate stabilization weight, 
                                               100-u32RQRatio[i] is the sequence quality stabilization weight */ 

}VENC_PARAM_MJPEG_CBR_S;

typedef struct hiVENC_PARAM_MJPEG_VBR_S
{
    HI_S32 s32DeltaQfactor;                 /* Indicates the maximum change of Qfactor values of frames 
                                               when the picture quality changes */
    HI_S32 s32ChangePos;                    /* Indicates the ratio of the current bit rate to the maximum 
                                               bit rate when the Qfactor value starts to be adjusted */
    HI_BOOL bLostFrmOpen;                   /* Indicates whether to discard frames to ensure 
                                               stable bit rate when the instant bit rate is exceeded */
    HI_U32  u32LostFrmBpsThr;               /* the instant bit rate threshold */
                                               
    VENC_SUPERFRM_MODE_EN  enSuperFrmMode;  /* Indicates the mode of processing the super frame */
    
    HI_U32  u32SuperFrmBitsThr;             /* Indicate the threshold of the super frame 
                                               for enabling the super frame processing mode */              
}VENC_PARAM_MJPEG_VBR_S;

typedef struct hiVENC_PARAM_MPEG4_CBR_S
{
    HI_U32  u32MinIprop;                    /* the min ratio of i frame and p frame*/           
    HI_U32  u32MaxIprop;                    /* the max ratio of i frame and p frame */
     
    HI_U32  u32MaxQp;                       /* the max QP value*/
    HI_U32  u32MaxStartQp;                  /* the max QP value for rate control of frame level,should between [u32MinQp,u32MaxQp]*/
    HI_U32  u32MinQp;                       /* the min QP value */
    HI_U32  u32MaxPPDeltaQp;                /* the max qp value difference between two successive P frame */
    HI_U32  u32MaxIPDeltaQp;                /* the max qp value difference between p frame and the next i frame */
 
    HI_BOOL bLostFrmOpen;                   /* Indicates whether to discard frames to ensure 
                                               stable bit rate when the instant bit rate is exceeded  */
    HI_U32  u32LostFrmBpsThr;               /* the instant bit rate threshold */                 
    
    VENC_SUPERFRM_MODE_EN  enSuperFrmMode;  /* the super frame mode */
    HI_U32  u32SuperIFrmBitsThr;            /* Indicate the threshold of the super I frame 
                                               for enabling the super frame processing mode */            
    HI_U32  u32SuperPFrmBitsThr;            /* Indicate the threshold of the super I frame 
                                               for enabling the super frame processing mode */              
                                          
    HI_S32 s32IPQPDelta;                    /* the qp difference between the i frame and the before gop avarage qp*/

    HI_U32  u32RQRatio[8];                  /* the rate stabilization weight, 
                                               100-u32RQRatio[i] is the sequence quality stabilization weight */
}VENC_PARAM_MPEG4_CBR_S;

typedef struct hiVENC_PARAM_MPEG4_VBR_S
{
    HI_S32 s32DeltaQP;                      /* Indicates the maximum change of QP values of frames 
                                               when the picture quality changes */
    HI_S32 s32ChangePos;                    /* Indicates the ratio of the current bit rate to the maximum 
                                               bit rate when the QP value starts to be adjusted */

    HI_U32  u32MinIprop;                    /* the min ratio of i frame and p frame */           
    HI_U32  u32MaxIprop;                     /* the max ratio of i frame and p frame */

    HI_BOOL bLostFrmOpen;                   /* Indicates whether to discard frames to ensure 
                                               stable bit rate when the instant bit rate is exceeded */
    HI_U32  u32LostFrmBpsThr;               /* the instant bit rate threshold */
                                               
    VENC_SUPERFRM_MODE_EN  enSuperFrmMode;  /* Indicates the mode of processing the super frame */
    HI_U32  u32SuperIFrmBitsThr;            /* Indicate the threshold of the super I frame 
                                               for enabling the super frame processing mode */              
    HI_U32  u32SuperPFrmBitsThr;            /* Indicate the threshold of the super I frame 
                                               for enabling the super frame processing mode */      
}VENC_PARAM_MPEG4_VBR_S;

typedef struct hiVENC_RC_PARAM_S
{     
    HI_U32 u32ThrdI[12];                     /* just useful for h264 and mpeg4 for now */
    HI_U32 u32ThrdP[12];
    HI_U32 u32QpDelta;
    union
    {
        VENC_PARAM_H264_CBR_S     stParamH264Cbr;
        VENC_PARAM_H264_VBR_S     stParamH264VBR;
        VENC_PARAM_H264_ABR_S     stParamH264ABR;
        VENC_PARAM_MJPEG_CBR_S    stParamMjpegCbr;
        VENC_PARAM_MJPEG_VBR_S    stParamMjpegVbr;
        VENC_PARAM_MPEG4_CBR_S    stParamMpeg4Cbr;
        VENC_PARAM_MPEG4_VBR_S    stParamMpeg4Vbr;
        VENC_PARAM_H264_CBRV2_S   stParamH264CbrV2;
    };

    HI_VOID* pRcParam;                      /*RC parameter which could be specified by usrer*/
}VENC_RC_PARAM_S;

typedef enum hiVENC_RC_PRIORITY_E
{
    VENC_RC_PRIORITY_BITRATE_FIRST = 1,    
    VENC_RC_PRIORITY_FRAMEBITS_FIRST,

    VENC_RC_PRIORITY_BUTT,
} VENC_RC_PRIORITY_E;

typedef enum hiVENC_LOSTFRM_MODE_E
{   
    LOSTFRM_NORMAL,                                /*normal mode*/         
    LOSTFRM_PSKIP,                                  /*pskip*/ 
    LOSTFRM_BUTT,
} VENC_LOSTFRM_MODE_E;

typedef struct hiVENC_PARAM_LOSTFRM_S
{ 
    HI_BOOL bLostFrmOpen;               /* Indicates whether to discard frames to ensure 
                                           stable bit rate when the instant bit rate is exceeded */
    HI_U32  u32LostFrmBpsThr;           /* the instant bit rate threshold */
    VENC_LOSTFRM_MODE_E enLostFrmMode;  /* lost frame strategy */
    HI_U32  u32LostFrmGaps;             /* the gap of lost frame */
} VENC_PARAM_LOSTFRM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_RC_H__ */