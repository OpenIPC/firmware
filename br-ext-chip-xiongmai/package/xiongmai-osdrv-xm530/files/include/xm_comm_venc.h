/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : xm_comm_venc.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/

#ifndef __XM_COMM_VENC_H__
#define __XM_COMM_VENC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "xm_type.h"
#include "xm_common.h"
#include "xm_errno.h"
#include "xm_comm_video.h"
#include "xm_comm_rc.h"


#define XM_TRACE_VENC(level, fmt...) XM_TRACE(level, XM_ID_VENC,##fmt)

/* invlalid channel ID */
#define XM_ERR_VENC_INVALID_CHNID     XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define XM_ERR_VENC_ILLEGAL_PARAM     XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define XM_ERR_VENC_EXIST             XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define XM_ERR_VENC_UNEXIST           XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define XM_ERR_VENC_NULL_PTR          XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define XM_ERR_VENC_NOT_CONFIG        XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define XM_ERR_VENC_NOT_SUPPORT       XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define XM_ERR_VENC_NOT_PERM          XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* encode one frame not finish,maybe bus error,timeout,sys reset and so on */
#define XM_ERR_VENC_NOT_FINISH        XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_FINISH)
/* insufficient resource*/
#define XM_ERR_VENC_INSUFF_RES        XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INSUFF_RES)
/* failure caused by malloc memory */
#define XM_ERR_VENC_NOMEM             XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define XM_ERR_VENC_NOBUF             XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define XM_ERR_VENC_BUF_EMPTY         XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define XM_ERR_VENC_BUF_FULL          XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define XM_ERR_VENC_SYS_NOTREADY      XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* encoder hardware init or config failed */
#define XM_ERR_VENC_HW_FAILED         XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_HW_FAILED)
/* system is busy*/
#define XM_ERR_VENC_BUSY              XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* undefined error */
#define XM_ERR_VENC_UND_ERROR         XM_DEF_ERR(XM_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_UND)

/*the coding frame type for H265*/
typedef enum xmH265E_Coding_Type_E
{
	H265ENC_INFO_FRAME 	= 0,
	H265ENC_INTRA_FRAME 	= 1,
	H265ENC_PERDICTED_FRAME = 2,
	H265_NOTCODED_FRAME
}H265E_Coding_Type_E;

/*the nalu type of H264E*/
typedef enum xmH264E_NALU_TYPE_E
{
     H264E_NALU_PSLICE = 1,                         /*PSLICE types*/
     H264E_NALU_ISLICE = 5,                         /*ISLICE types*/
     H264E_NALU_SEI    = 6,                         /*SEI types*/
     H264E_NALU_SPS    = 7,                         /*SPS types*/
     H264E_NALU_PPS    = 8,                         /*PPS types*/
     H264E_NALU_VIRP   = 24,						/*VISLICE types*/
     H264E_NALU_BUTT        
} H264E_NALU_TYPE_E;

/*the nalu type of H265E*/
typedef enum xmH265E_NALU_TYPE_E
{
	 H265E_NALU_PSLICE = 1,                         /*P SLICE types*/
	 H265E_NALU_ISLICE = 19,                         /*I SLICE types*/
     H265E_NALU_VPS    = 32,                         /*VPS types*/
     H265E_NALU_SPS    = 33,                         /*SPS types*/
     H265E_NALU_PPS    = 34,                         /*PPS types*/
     H265E_NALU_SEI    = 39,                         /*SEI types*/
     
     H265E_NALU_BUTT        
} H265E_NALU_TYPE_E;

/*the reference type of H264E slice*/
typedef enum xmH264E_REFSLICE_TYPE_E
{
     H264E_REFSLICE_FOR_1X = 1,                     /*Reference slice for H264E_REF_MODE_1X*/
     H264E_REFSLICE_FOR_2X = 2,                     /*Reference slice for H264E_REF_MODE_2X*/
     H264E_REFSLICE_FOR_4X = 5,                     /*Reference slice for H264E_REF_MODE_4X*/
     H264E_REFSLICE_FOR_BUTT                        /* slice not for reference*/          
} H264E_REFSLICE_TYPE_E;

/*the pack type of JPEGE*/
typedef enum xmJPEGE_PACK_TYPE_E
{
     JPEGE_PACK_ECS = 5,                            /*ECS types*/
     JPEGE_PACK_APP = 6,                            /*APP types*/
     JPEGE_PACK_VDO = 7,                            /*VDO types*/
     JPEGE_PACK_PIC = 8,                            /*PIC types*/
     JPEGE_PACK_BUTT
} JPEGE_PACK_TYPE_E;

/*the pack type of MPEG4*/
typedef enum xmMPEG4E_PACK_TYPE_E
{
     MPEG4E_PACK_VOP_P = 1,                         /*P VOP packet types*/
     MPEG4E_PACK_VOP_I = 5,                         /*I VOP packet types*/
     MPEG4E_PACK_VOS   = 6,                         /*VOS types*/
     MPEG4E_PACK_VO    = 7,                         /*VO types*/
     MPEG4E_PACK_VOL   = 8,                         /*VOL types*/
     MPEG4E_PACK_GVOP  = 9                          /*GROUP of vop types */
} MPEG4E_PACK_TYPE_E;

/*the data type of VENC*/
typedef union xmVENC_DATA_TYPE_U
{
    H264E_NALU_TYPE_E    enH264EType;               /*H264E NALU types*/
    JPEGE_PACK_TYPE_E    enJPEGEType;               /*JPEGE pack types*/
    MPEG4E_PACK_TYPE_E   enMPEG4EType;              /*MPEG4E pack types*/
    H265E_NALU_TYPE_E    enH265EType;               /*H264E NALU types*/
}VENC_DATA_TYPE_U;

typedef struct xmVENC_PACK_INFO_S
{
	VENC_DATA_TYPE_U  u32PackType;
    XM_U32 u32PackOffset;                          
    XM_U32 u32PackLength;                        
}VENC_PACK_INFO_S;


typedef struct xmVENC_PACK_S
{
    XM_U32   u32PhyAddr;                         /*the physics address of stream*/
    XM_U8   *pu8Addr;                            /*the virtual address of stream*/
    XM_U32   u32Len;                             /*the length of stream*/
    
    XM_U64   u64PTS;                                /*PTS*/
    XM_BOOL  bFrameEnd;                             /*frame end？*/
    
    VENC_DATA_TYPE_U  DataType;                     /*the type of stream*/
  }VENC_PACK_S;

typedef enum xmH264E_REF_TYPE_E
{
    BASE_IDRSLICE = 0,                              //the Idr frame at Base layer
    BASE_PSLICE_REFBYBASE,                          //the P frame at Base layer, referenced by other frames at Base layer
    BASE_PSLICE_REFBYENHANCE,                       //the P frame at Base layer, referenced by other frames at Enhance layer
    ENHANCE_PSLICE_REFBYENHANCE,                    //the P frame at Enhance layer, referenced by other frames at Enhance layer
    ENHANCE_PSLICE_NOTFORREF,                       //the P frame at Enhance layer ,not referenced
    ENHANCE_PSLICE_BUTT
} H264E_REF_TYPE_E;

typedef enum xmH264E_REF_TYPE_E H265E_REF_TYPE_E;

typedef struct xmVENC_STREAM_INFO_H264_S
{    
    XM_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    XM_U32 u32PSkipMbNum;                           /* the skip macroblock num */
    XM_U32 u32IpcmMbNum;                            /* the ipcm macroblock num */
    XM_U32 u32Inter16x8MbNum;                       /* the inter16x8 macroblock num */
    XM_U32 u32Inter16x16MbNum;                      /* the inter16x16 macroblock num */
    XM_U32 u32Inter8x16MbNum;                       /* the inter8x16 macroblock num */
    XM_U32 u32Inter8x8MbNum;                        /* the inter8x8 macroblock num */
    XM_U32 u32Intra16MbNum;                         /* the intra16x16 macroblock num */
    XM_U32 u32Intra8MbNum;                          /* the intra8x8 macroblock num */
    XM_U32 u32Intra4MbNum;                          /* the inter4x4 macroblock num */

    H264E_REFSLICE_TYPE_E enRefSliceType;           /*the reference type of H264E slice*/
    H264E_REF_TYPE_E      enRefType;                /*Type of encoded frames in advanced frame skipping reference mode*/
    XM_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
}VENC_STREAM_INFO_H264_S;

typedef struct xmVENC_STREAM_INFO_H265_S
{    
    XM_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    XM_U32 u32Inter64x64CuNum;                            /* the  num */
    XM_U32 u32Inter32x32CuNum;
    XM_U32 u32Inter16x16CuNum;
    XM_U32 u32Inter8x8CuNum;
    XM_U32 u32Intra32x32CuNum;
    XM_U32 u32Intra16x16CuNum;
    XM_U32 u32Intra8x8CuNum;
    XM_U32 u32Intra4x4CuNum;

    H265E_REF_TYPE_E      enRefType;                /*Type of encoded frames in advanced frame skipping reference mode*/
    XM_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
}VENC_STREAM_INFO_H265_S;

typedef struct xmVENC_STREAM_INFO_JPEG_S
{
    XM_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    XM_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
    XM_U32 u32Qfactor;                              /* image quality */
}VENC_STREAM_INFO_JPEG_S;

typedef struct xmVENC_STREAM_INFO_MPEG4_S
{
    XM_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    XM_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
}VENC_STREAM_INFO_MPEG4_S;

typedef struct xmVENC_STREAM_S
{
    VENC_PACK_S *pstPack;                           /*stream pack attribute*/
    XM_U32      u32PackCount;                       /*the pack number of one frame stream*/
    XM_U32      u32Seq;                             /*the list number of stream*/
}VENC_STREAM_S;

typedef struct xmVENC_ATTR_H264_S
{
    XM_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    XM_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/

    XM_U32  u32BufSize;                             /*stream buffer size*/
    XM_U32  u32Profile;                             /*0: baseline; 1:MP; 2:HP; 3: SVC-T [0,3]; */          
    XM_BOOL bByFrame;                               /*get stream mode is slice mode or frame mode*/
	

    XM_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    XM_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/    

    XM_U32  u32BFrameNum;                           /* 0: not support B frame; >=1: number of B frames */
    XM_U32  u32RefNum;                              /* 0: default; number of refrence frame*/

}VENC_ATTR_H264_S;

typedef struct xmVENC_ATTR_H265_S
{
    XM_U32  u32MaxPicWidth;     /*maximum width of a picture to be encoded, in pixel*/
    XM_U32  u32MaxPicHeight;    /*maximum height of a picture to be encoded, in pixel*/
    
    XM_U32  u32BufSize;         /*stream buffer size*/
    XM_U32  u32Profile;         /*0: MP */          
    XM_BOOL bByFrame;           /*get stream mode is slice mode or frame mode*/

    
    XM_U32  u32PicWidth;        /*width of a picture to be encoded, in pixel*/
    XM_U32  u32PicHeight;       /*height of a picture to be encoded, in pixel*/
    
    XM_U32  u32BFrameNum;                           /* 0: not support B frame; >=1: number of B frames */
    XM_U32  u32RefNum;                              /* 0: default; number of refrence frame*/
}VENC_ATTR_H265_S;

typedef struct xmVENC_ATTR_MJPEG_S
{
    XM_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    XM_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    XM_U32  u32BufSize;                             /*stream buffer size*/
    XM_BOOL bByFrame;                               /*get stream mode is field mode  or frame mode*/

	
    XM_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    XM_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/ 
    
}VENC_ATTR_MJPEG_S;

typedef struct xmVENC_ATTR_JPEG_S 
{
    XM_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    XM_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    XM_U32  u32BufSize;                             /*stream buffer size*/
    XM_BOOL bByFrame;                               /*get stream mode is field mode  or frame mode*/
	

    XM_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    XM_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
    XM_BOOL  bSupportDCF;                           /*support dcf*/
    
}VENC_ATTR_JPEG_S;

/* attribute of MPEG4*/
typedef struct  xmVENC_ATTR_MPEG4_S
{
    XM_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    XM_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/    

    XM_U32  u32BufSize;                             /*buffer size*/
    XM_BOOL bByFrame;                               /*get stream mode is pack or frame*/
	

    XM_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    XM_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
}VENC_ATTR_MPEG4_S;

typedef struct xmVENC_ATTR_S
{
    PAYLOAD_TYPE_E  enType;                         /*the type of payload*/
    union
    {
        VENC_ATTR_H264_S  stAttrH264e;              /*attributes of h264*/
        VENC_ATTR_MJPEG_S stAttrMjpeg;              /*attributes of mjpeg*/
        VENC_ATTR_JPEG_S  stAttrJpeg;               /*attributes of jpeg*/
        VENC_ATTR_MPEG4_S stAttrMpeg4;              /*attributes of mpeg4*/
        VENC_ATTR_H265_S  stAttrH265e;              /*attributes of h265*/
    };    
}VENC_ATTR_S;

typedef struct xmVENC_CHN_ATTR_S
{
    VENC_ATTR_S stVeAttr;                           /*the attribute of video encoder*/
    VENC_RC_ATTR_S stRcAttr;                        /*the attribute of rate  ctrl*/
}VENC_CHN_ATTR_S;
typedef struct xmJPEG_ENC_ATTR_S
{
	int lumWidthSrc;//16对齐
	int lumHeightSrc;//2对齐
	int horOffsetSrc;//2对齐
	int verOffsetSrc;//2对齐
	int dstWidth;//>=96  4对齐
	int dstHeight;//>=32  2对齐
}JPEG_ENC_ATTR_S;


/*vbr mode*/
typedef struct xmVENC_PARAM_REF_EX_S
{
	XM_U8	bVirtualEnable;							/*virtual I frame enable/disable*/
	XM_U32  u32VirtualIInterval;					/*virtual I frame interval*/
}VENC_PARAM_REF_EX_S;

typedef struct xmVENC_CHN_STAT_S
{
    XM_U32 u32LeftPics;                             /*left picture number */
    XM_U32 u32LeftStreamBytes;                      /*left stream bytes*/
    XM_U32 u32LeftStreamFrames;                     /*left stream frames*/
    XM_U32 u32CurPacks;                             /*pack number of current frame*/
    XM_U32 u32LeftRecvPics;                         /*Number of frames to be received. This member is valid after HI_MPI_VENC_StartRecvPicEx is called.*/
    XM_U32 u32LeftEncPics;                          /*Number of frames to be encoded. This member is valid after HI_MPI_VENC_StartRecvPicEx is called.*/
}VENC_CHN_STAT_S;



typedef struct xmVENC_PARAM_H264_SLICE_SPLIT_S
{
    XM_BOOL bSplitEnable;                           /*slice split enable, XM_TRUE:enable, HI_FALSE:diable, default value:HI_FALSE*/
    XM_U32  u32SplitMode;                           /*0:bit number, 1:mb line number, >=2:no meaning*/
    XM_U32  u32SliceSize;                           /*when the splitmode is 0, this value presents the bitsnum of one slice average, 
                                                      when the splitmode is 1, this value presents the mb num of one slice*/
} VENC_PARAM_H264_SLICE_SPLIT_S;

typedef struct xmVENC_PARAM_H264_INTER_PRED_S
{
    /* search window */
    XM_U32 u32HWSize;                              /* size of horizontal search window.
                                                      default value: differ with the picture size */
    XM_U32 u32VWSize;                              /* size of vertical search window.
                                                    default value: differ with the picture size */
                                    
    /* inter pred, one of the following 4 kinds of inter prediction mode must be enable */
    XM_BOOL bInter16x16PredEn;                     /*default: XM_TRUE, enable 16x16 prediction*/
    XM_BOOL bInter16x8PredEn;                      /*default: XM_TRUE*/
    XM_BOOL bInter8x16PredEn;                      /*default: XM_TRUE*/
    XM_BOOL bInter8x8PredEn;                       /*default: XM_TRUE*/
    XM_BOOL bExtedgeEn;                            /*default: XM_TRUE*/
} VENC_PARAM_H264_INTER_PRED_S;

typedef struct xmVENC_PARAM_H264_INTRA_PRED_S
{
    /* intra pred, one of following 2 kinds of intra pred mode must be enable */                            
    XM_BOOL bIntra16x16PredEn;                     /*default: XM_TRUE, enable 16x16 intra prediction*/           
    XM_BOOL bIntraNxNPredEn;                       /*default: XM_TRUE, enable 4x4 and/or 8x8 prediction
                                                     Notes: this value must work with IntraTransMode*/
    XM_U32 constrained_intra_pred_flag;            /*default: HI_FALSE, see the H.264 protocol*/
    XM_BOOL bIpcmEn;                               /*default: XM_TRUE, enable ipcm*/
}VENC_PARAM_H264_INTRA_PRED_S;

typedef struct xmVENC_PARAM_H264_TRANS_S
{
    XM_U32 u32IntraTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    XM_U32 u32InterTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    
    XM_BOOL bScalingListValid;                     /* default: HI_FALSE */
                                                   /* Notes: Enable IntraScalingList8X8 and InterScalingList8X8 while bScalingListValid is XM_TRUE */
    XM_U8  InterScalingList8X8[64];                
    XM_U8  IntraScalingList8X8[64];                
    
    XM_S32 chroma_qp_index_offset;                 /* [-12,12],default value: 0*/
}VENC_PARAM_H264_TRANS_S;

typedef struct xmVENC_PARAM_H264_ENTROPY_S
{
    XM_U32 u32EntropyEncModeI;                     /* 0:cavlc, 1:cabac */
    XM_U32 u32EntropyEncModeP;                     /* 0:cavlc, 1:cabac */
    XM_U32 cabac_stuff_en;                         /* default: 0*/
    XM_U32 Cabac_init_idc;                         /* 0~2 */
}VENC_PARAM_H264_ENTROPY_S;

typedef struct xmVENC_PARAM_H264_POC_S
{
    XM_U32 pic_order_cnt_type;                     /* default value: 2. {0,1,2} */

}VENC_PARAM_H264_POC_S;

typedef struct xmVENC_PARAM_H264_DBLK_S
{
    XM_U32 disable_deblocking_filter_idc;          /* default value: 0. {0,1,2} */
    XM_S32 slice_alpha_c0_offset_div2;             /* default value: 5. [-6,+6] */
    XM_S32 slice_beta_offset_div2;                 /* default value: 5. [-6,+6] */
}VENC_PARAM_H264_DBLK_S;



typedef struct xmVENC_PARAM_H264_VUI_TIME_INFO_S
{
    XM_U8  timing_info_present_flag;               /* default value: 0. If 1, timing info belows will be encoded into vui. {0,1} */
    XM_U8  fixed_frame_rate_flag;                  /* default value: n/a. {0,1} */
    XM_U32 num_units_in_tick;                      /* default value: n/a. > 0. */
    XM_U32 time_scale;                             /* default value: n/a. > 0. */

}VENC_PARAM_VUI_H264_TIME_INFO_S;

typedef struct xmVENC_PARAM_VUI_ASPECT_RATIO_S
{
    XM_U8  aspect_ratio_info_present_flag;        /* default value: 0. If 1, aspectratio info belows will be encoded into vui. {0,1} */
    XM_U8  aspect_ratio_idc;                      /* default value: n/a. [0,255],17~254 is reserved. */
	XM_U8  overscan_info_present_flag;			  /* default value: 0,just be 0.If 1, oversacan info belows will be encoded into vui. {0,1} */
	XM_U8  overscan_appropriate_flag;			  /* default value: n/a. */
	XM_U16 sar_width;                             /* default value: n/a. sar_width  and  sar_height  shall  be  relatively  prime. */
    XM_U16 sar_height ;                  		  /* default value: n/a.  */
}VENC_PARAM_VUI_ASPECT_RATIO_S;


typedef struct xmVENC_PARAM_VUI_VIDEO_SIGNAL_S
{
    XM_U8  video_signal_type_present_flag ;        /* default value: 0. If 1, video singnal info will be encoded into vui. {0,1} */
    XM_U8  video_format ;                          /* default value: n/a. >= 0. */
    XM_U8  video_full_range_flag;                  /* default value: n/a. {0,1}. */
    XM_U8  colour_description_present_flag ;       /* default value: n/a. {0,1} */
	XM_U8  colour_primaries ;					   /* default value: n/a. [0,255],0/9~255 is reserved */
	XM_U8  transfer_characteristics;			   /* default value: n/a. [0,255],0/13~255 is reserved */
	XM_U8  matrix_coefficients; 				   /* default value: n/a. [0,255],9~255 is reserved */		
}VENC_PARAM_VUI_VIDEO_SIGNAL_S;



typedef struct xmVENC_PARAM_H264_VUI_S
{
	VENC_PARAM_VUI_ASPECT_RATIO_S stVuiAspectRatio;
	VENC_PARAM_VUI_H264_TIME_INFO_S    stVuiTimeInfo;
	VENC_PARAM_VUI_VIDEO_SIGNAL_S stVuiVideoSignal;
}VENC_PARAM_H264_VUI_S;


typedef struct xmVENC_PARAM_JPEG_S
{
    XM_U32 u32Qfactor;                             /*image quality :[1,99]*/

    XM_U8  u8YQt[64];                              /* y qt value */
    XM_U8  u8CbQt[64];                             /* cb qt value */
    XM_U8  u8CrQt[64];                             /* cr qt value */

    XM_U32 u32MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_JPEG_S;

typedef struct xmVENC_PARAM_MJPEG_S
{
    XM_U8 u8YQt[64];                               /* y qt value */
    XM_U8 u8CbQt[64];                              /* cb qt value */
    XM_U8 u8CrQt[64];                              /* cr qt value */

    XM_U32 u32MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_MJPEG_S;

typedef struct xmVENC_PARAM_MPEG4_S
{
    XM_BOOL bSplitEnable;                          /*slice split enable, XM_TRUE:enable, HI_FALSE:diable, default value:HI_FALSE*/
    XM_U32  u32SplitMode;                          /*0:bit number, 1:mb line number, >=2:no meaning*/
    XM_U32  u32PacketSize;                         /*when the splitmode is 0, this value presents the bitsnum of one packet average, 
                                                        when the splitmode is 1, this value presents the mb num of one packet*/

    /* search window */
    XM_U32 u32HWSize;                              /* size of horizontal search window.
                                                    * default value: differ with the picture size
                                                    * 0000: [-16, +15]
                                                    * 0001: [-32, +31]
                                                    */
    XM_U32 u32VWSize;                              /* size of vertical search window.
                                                    * default value: differ with the picture size
                                                    * 000: [-16, +15]
                                                    * >000: no meaning. 
                                                    */
} VENC_PARAM_MPEG4_S;


/* ROI struct */
typedef struct xmVENC_ROI_CFG_S
{
    XM_U32  u32Index;                              /* Index of an ROI. The system supports indexes ranging from 0 to 7 */
    XM_BOOL bEnable;                               /* Whether to enable this ROI */
    XM_BOOL bAbsQp;                                /* QP mode of an ROI.HI_FALSE: relative QP.HI_TURE: absolute QP.*/
    XM_S32  s32Qp;                                 /* QP value. */
    RECT_S  stRect;                                /* Region of an ROI*/
}VENC_ROI_CFG_S;


typedef struct xmVENC_ROIBG_FRAME_RATE_S
{
    XM_S32 s32SrcFrmRate;                          /* Input frame rate of   Roi backgroud*/
    XM_S32 s32DstFrmRate;                          /* Output frame rate of  Roi backgroud */
}VENC_ROIBG_FRAME_RATE_S;


typedef struct xmVENC_COLOR2GREY_S
{
    XM_BOOL bColor2Grey;                           /* Whether to enable Color2Grey.*/
}VENC_COLOR2GREY_S;

typedef struct xmVENC_CROP_CFG_S
{
    XM_BOOL bEnable;                               /* Crop region enable */
    RECT_S  stRect;                                /* Crop region, note: s32X must be multi of 16 */
}VENC_CROP_CFG_S;


typedef struct xmVENC_FRAME_RATE_S
{
    XM_S32 s32SrcFrmRate;                          /* Input frame rate of a  channel*/
    XM_S32 s32DstFrmRate;                          /* Output frame rate of a channel*/
} VENC_FRAME_RATE_S;

typedef struct xmVENC_PARAM_REF_S
{
    XM_U32       u32Base;                          /*Base layer period*/
    XM_U32       u32Enhance;                       /*Enhance layer period*/
    XM_BOOL      bEnablePred;                      /*Whether some frames at the base layer are referenced by other frames at the base layer. When bEnablePred is HI_FALSE, all frames at the base layer reference IDR frames.*/
} VENC_PARAM_REF_S;


typedef enum xmH264E_IDR_PIC_ID_MODE_E
{
    H264E_IDR_PIC_ID_MODE_AUTO = 0,                /*auto mode */
    H264E_IDR_PIC_ID_MODE_USR,                     /*user mode */
    H264E_IDR_PIC_ID_MODE_BUTT,
}H264E_IDR_PIC_ID_MODE_E;

typedef struct xmVENC_H264_IDRPICID_CFG_S
{
    H264E_IDR_PIC_ID_MODE_E enH264eIdrPicIdMode;   /*the mode of idr_pic_id that is set*/
    XM_U32 u32H264eIdrPicId;                       /*idr_pic_id value*/
} VENC_H264_IDRPICID_CFG_S;     

/* Jpeg snap mode */
typedef enum xmVENC_JPEG_SNAP_MODE_E
{
    JPEG_SNAP_ALL   = 0,                           /* Jpeg channel snap all the pictures when started. */
    JPEG_SNAP_FLASH = 1,                           /* Jpeg channel snap the flashed pictures when started. */
    JPEG_SNAP_BUTT,
    
}VENC_JPEG_SNAP_MODE_E;

typedef struct xmVENC_RECV_PIC_PARAM_S
{
    XM_S32 s32RecvPicNum;                          /*Number of frames received and encoded by the encoding channel*/
} VENC_RECV_PIC_PARAM_S;

typedef struct xmVENC_STREAM_BUF_INFO_S
{
    XM_U32   u32PhyAddr;
    XM_VOID *pUserAddr;
    XM_U32   u32BufSize;    
} VENC_STREAM_BUF_INFO_S;

typedef struct xmVENC_CHN_PARAM_S
{
	XM_U32 u32QuarterPixelMv;
	XM_U32 u32MinIQp;
}VENC_CHN_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __XM_COMM_VENC_H__ */
