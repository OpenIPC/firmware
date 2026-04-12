/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_common.h
Version       : Initial Draft
Author        : Hi3511 MPP Team
Created       : 2006/11/09
Last Modified :
Description   : The common defination
Function List :
History       :
 1.Date        : 2009/03/03
   Author      : z44949
   Modification: Created file
2.Date        :   2009/07/01
  Author      :   z44949
  Modification:   Move MPP_VER_PRIX to hi_defines.h
3.Date        :   2009/08/13
  Author      :   y45339
  Modification:   add some proc define

4.Date        :   2010/11/03
  Author      :   z44949
  Modification:   Remove some unnecessary typedef
   
******************************************************************************/
#ifndef __HI_COMMON_H__
#define __HI_COMMON_H__

#include "hi_type.h"
#include "hi_math.h"
#include "hi_defines.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef VER_X
    #define VER_X 1
#endif

#ifndef VER_Y
    #define VER_Y 0
#endif

#ifndef VER_Z
    #define VER_Z 0
#endif

#ifndef VER_P
    #define VER_P 0
#endif

#ifdef HI_DEBUG
    #define VER_D " "
#else
    #define VER_D " Release"
#endif

#define __MK_VERSION(x,y,z,p) #x"."#y"."#z"."#p
#define MK_VERSION(x,y,z,p) __MK_VERSION(x,y,z,p)
#define MPP_VERSION  CHIP_NAME MPP_VER_PRIX MK_VERSION(VER_X,VER_Y,VER_Z,VER_P) VER_D
#define COM_VERSION  MPP_VER_PRIX MK_VERSION(VER_X,VER_Y,VER_Z,VER_P) VER_D

#define VERSION_NAME_MAXLEN 64
typedef struct hiMPP_VERSION_S
{
	HI_CHAR aVersion[VERSION_NAME_MAXLEN];
}MPP_VERSION_S;

typedef struct hiPOINT_S
{
    HI_S32 s32X;
    HI_S32 s32Y;
}POINT_S;

typedef struct hiSIZE_S
{
    HI_U32 u32Width;
    HI_U32 u32Height;
} SIZE_S;

typedef struct hiRECT_S
{
    HI_S32 s32X;
    HI_S32 s32Y;
    HI_U32 u32Width;
    HI_U32 u32Height;
}RECT_S;

typedef enum hiROTATE_E
{
    ROTATE_NONE = 0,
    ROTATE_90   = 1,
    ROTATE_180  = 2,
    ROTATE_270  = 3,
    ROTATE_BUTT
} ROTATE_E;

typedef HI_S32 AI_CHN;
typedef HI_S32 AO_CHN;
typedef HI_S32 AENC_CHN;
typedef HI_S32 ADEC_CHN;
typedef HI_S32 AUDIO_DEV;
typedef HI_S32 AVENC_CHN;
typedef HI_S32 VI_DEV;
typedef HI_S32 VI_WAY;
typedef HI_S32 VI_CHN;
typedef HI_S32 VO_DEV;
typedef HI_S32 VO_LAYER;
typedef HI_S32 VO_CHN;
typedef HI_S32 VENC_CHN;
typedef HI_S32 VDEC_CHN;
typedef HI_S32 VENC_GRP;
typedef HI_S32 VO_GRP;
typedef HI_S32 VDA_CHN;
typedef HI_S32 IVE_HANDLE;
typedef HI_S32 ISP_DEV;
typedef HI_S32 SENSOR_ID;

#define HI_INVALID_CHN (-1)
#define HI_INVALID_WAY (-1)
#define HI_INVALID_LAYER (-1)
#define HI_INVALID_DEV (-1)
#define HI_INVALID_HANDLE (-1)

#define HI_INVALID_VALUE (-1)
#define HI_INVALID_TYPE (-1)

typedef enum hiMOD_ID_E
{
    HI_ID_CMPI    = 0,
    HI_ID_VB      = 1,
    HI_ID_SYS     = 2,
    HI_ID_VALG    = 3,

#if 1
    /* 
        VDEC内部会创建VB Pool, VDEC销毁的时候销毁VB Pool,
        但是如果其他模块使用了这个图像, 销毁的时候VB--会报错;
        所以VDEC模块要等其他模块销毁之后再销毁      f65132注

        ctrl+c退出时如果先销毁CHNL再退出VDEC,则VDEC退出时会unregister CHNL，
        因此时CHNL已被销毁，所以unregister不成功，打印出错信息。
        为解决这个问题，需要把CHNL和VDEC的id顺序互换
    */
    HI_ID_CHNL    = 4,   
    HI_ID_VDEC    = 5,
    HI_ID_GROUP   = 6,
    HI_ID_VENC    = 7,
    HI_ID_VPSS    = 8,
    HI_ID_VDA     = 9,
    
    HI_ID_H264E   = 10,
    HI_ID_JPEGE   = 11,
    HI_ID_MPEG4E  = 12,
#else

    HI_ID_CHNL    = 4,
    HI_ID_GROUP   = 5,
    HI_ID_VENC    = 6,
    HI_ID_VPSS    = 7,
    HI_ID_VDA     = 8,
    
    HI_ID_H264E   = 9,
    HI_ID_JPEGE   = 10,
    HI_ID_MPEG4E  = 11,

    HI_ID_VDEC    = 12,
#endif    
    HI_ID_H264D   = 13,
    HI_ID_JPEGD   = 14,
    HI_ID_VOU     = 15,

    HI_ID_VIU     = 16,
    HI_ID_DSU     = 17,
    HI_ID_RGN	  = 18,
    HI_ID_RC      = 19,

    HI_ID_SIO     = 20,
    HI_ID_AI      = 21,
    HI_ID_AO      = 22,
    HI_ID_AENC    = 23,
    HI_ID_ADEC    = 24,

    HI_ID_AVENC   = 25,

    HI_ID_PCIV    = 26,
    HI_ID_PCIVFMW = 27,

    HI_ID_ISP	  = 28,

    HI_ID_IVE	  = 29,
    /* there is a hole */    

    HI_ID_DCCM    = 31,
    HI_ID_DCCS    = 32,

    HI_ID_PROC    = 33,
    HI_ID_LOG     = 34,
    HI_ID_MST_LOG = 35,
    HI_ID_VD      = 36,

    HI_ID_VCMP    = 38,
    HI_ID_FB      = 39,

    
    HI_ID_HDMI    = 40,
    HI_ID_VOIE    = 41,
    HI_ID_TDE     = 42,

    HI_ID_USR     = 43,

    HI_ID_VEDU   = 44,
    
    HI_ID_BUTT,
} MOD_ID_E;

typedef struct hiMPP_CHN_S
{
    MOD_ID_E    enModId;
    HI_S32      s32DevId;
    HI_S32      s32ChnId;
} MPP_CHN_S;

#define MPP_MOD_VIU       "vi"
#define MPP_MOD_VOU       "vo"
#define MPP_MOD_HDMI      "hdmi"
#define MPP_MOD_DSU       "dsu"

#define MPP_MOD_CHNL      "chnl"
#define MPP_MOD_VENC      "venc"
#define MPP_MOD_GRP       "grp"
#define MPP_MOD_VDA       "vda"
#define MPP_MOD_VPSS      "vpss"
#define MPP_MOD_RGN       "rgn"
#define MPP_MOD_IVE       "ive"

#define MPP_MOD_H264E     "h264e"
#define MPP_MOD_JPEGE     "jpege"
#define MPP_MOD_MPEG4E    "mpeg4e"

#define MPP_MOD_VDEC      "vdec"
#define MPP_MOD_H264D     "h264d"
#define MPP_MOD_JPEGD     "jpegd"

#define MPP_MOD_AI        "ai"
#define MPP_MOD_AO        "ao"
#define MPP_MOD_AENC      "aenc"
#define MPP_MOD_ADEC      "adec"
#define MPP_MOD_SIO       "sio"

#define MPP_MOD_VB        "vb"
#define MPP_MOD_SYS       "sys"
#define MPP_MOD_CMPI      "cmpi"

#define MPP_MOD_PCIV      "pciv"
#define MPP_MOD_PCIVFMW   "pcivfmw"

#define MPP_MOD_PROC      "proc"
#define MPP_MOD_LOG       "logmpp"
#define MPP_MOD_MST_LOG   "mstlog"

#define MPP_MOD_DCCM      "dccm"
#define MPP_MOD_DCCS      "dccs"

#define MPP_MOD_VCMP      "vcmp"
#define MPP_MOD_FB        "fb"

#define MPP_MOD_RC        "rc"

#define MPP_MOD_VOIE      "voie"

#define MPP_MOD_TDE       "tde"
#define MPP_MOD_ISP       "isp"
#define MPP_MOD_ACODEC    "acodec"

/* We just coyp this value of payload type from RTP/RTSP definition */
typedef enum
{
    PT_PCMU          = 0,
    PT_1016          = 1,
    PT_G721          = 2,
    PT_GSM           = 3,
    PT_G723          = 4,
    PT_DVI4_8K       = 5,
    PT_DVI4_16K      = 6,
    PT_LPC           = 7,
    PT_PCMA          = 8,
    PT_G722          = 9,
    PT_S16BE_STEREO  = 10,
    PT_S16BE_MONO    = 11,
    PT_QCELP         = 12,
    PT_CN            = 13,
    PT_MPEGAUDIO     = 14,
    PT_G728          = 15,
    PT_DVI4_3        = 16,
    PT_DVI4_4        = 17,
    PT_G729          = 18,
    PT_G711A         = 19,
    PT_G711U         = 20,
    PT_G726          = 21,
    PT_G729A         = 22,
    PT_LPCM          = 23,
    PT_CelB          = 25,
    PT_JPEG          = 26,
    PT_CUSM          = 27,
    PT_NV            = 28,
    PT_PICW          = 29,
    PT_CPV           = 30,
    PT_H261          = 31,
    PT_MPEGVIDEO     = 32,
    PT_MPEG2TS       = 33,
    PT_H263          = 34,
    PT_SPEG          = 35,
    PT_MPEG2VIDEO    = 36,
    PT_AAC           = 37,
    PT_WMA9STD       = 38,
    PT_HEAAC         = 39,
    PT_PCM_VOICE     = 40,
    PT_PCM_AUDIO     = 41,
    PT_AACLC         = 42,
    PT_MP3           = 43,
    PT_ADPCMA        = 49,
    PT_AEC           = 50,
    PT_X_LD          = 95,
    PT_H264          = 96,
    PT_D_GSM_HR      = 200,
    PT_D_GSM_EFR     = 201,
    PT_D_L8          = 202,
    PT_D_RED         = 203,
    PT_D_VDVI        = 204,
    PT_D_BT656       = 220,
    PT_D_H263_1998   = 221,
    PT_D_MP1S        = 222,
    PT_D_MP2P        = 223,
    PT_D_BMPEG       = 224,
    PT_MP4VIDEO      = 230,
    PT_MP4AUDIO      = 237,
    PT_VC1           = 238,
    PT_JVC_ASF       = 255,
    PT_D_AVI         = 256,
    PT_DIVX3		= 257,
    PT_AVS		= 258,
    PT_REAL8		= 259,
    PT_REAL9		= 260,
    PT_VP6		= 261,
    PT_VP6F		= 262,
    PT_VP6A		= 263,
    PT_SORENSON	 =264,
    PT_MAX           = 265,
    /* add by hisilicon */
    PT_AMR           = 1001, 
    PT_MJPEG         = 1002,
    PT_AMRWB         = 1003,
    PT_BUTT
}PAYLOAD_TYPE_E;

typedef enum hiVOU_WHO_SENDPIC_E
{
    VOU_WHO_SENDPIC_VIU     = 0,
    VOU_WHO_SENDPIC_VDEC    = 1,
    VOU_WHO_SENDPIC_PCIV    = 2,
    VOU_WHO_SENDPIC_VPP     = 3,
    VOU_WHO_SENDPIC_USR     = 4,
    VOU_WHO_SENDPIC_BUTT
} VOU_WHO_SENDPIC_E;

#if 1
//滤波系数的定义可能与芯片有关，具体实现时再考虑放在哪里
/* horizontal scale filter coefficient of dsu
** which affect image quality of encoding and preview.

** normally the filter can be set be DSU_HSCALE_FILTER_DEFAULT
** which means sdk will choose filter automatically.Otherwise,
** you can choose other filter

** Notes:65M means 6.5
*/
typedef enum hiDSU_HSCALE_FILTER_E
{
    DSU_HSCALE_FILTER_DEFAULT = 0,
    DSU_HSCALE_FILTER_C_65M,	
    DSU_HSCALE_FILTER_CG_56M,
    DSU_HSCALE_FILTER_LC_45M,
    DSU_HSCALE_FILTER_CG_3M,
    DSU_HSCALE_FILTER_CG_2M,
    DSU_HSCALE_FILTER_CG_1M,
    DSU_HSCALE_FILTER_BUTT
}DSU_HSCALE_FILTER_E;


/* vertical scale filter coefficient of dsu
** which affect image quality of encoding and preview.

** normally the filter can be set be DSU_VSCALE_FILTER_DEFAULT
** which means sdk will choose filter automatically.Otherwise,
** you can choose other filter

** Notes:38M means 3.8
*/
typedef enum hiDSU_VSCALE_FILTER_E
{
    DSU_VSCALE_FILTER_DEFAULT = 0,
    DSU_VSCALE_FILTER_S_6M,    
    DSU_VSCALE_FILTER_S_5M,    
    DSU_VSCALE_FILTER_S_4M,  
    DSU_VSCALE_FILTER_S_38M,     
    DSU_VSCALE_FILTER_S_37M,     
    DSU_VSCALE_FILTER_S_36M,     
    DSU_VSCALE_FILTER_S_25M,     
    DSU_VSCALE_FILTER_S_2M, 
    DSU_VSCALE_FILTER_S_15M,    
    DSU_VSCALE_FILTER_S_12M,    
    DSU_VSCALE_FILTER_S_11M,    
    DSU_VSCALE_FILTER_S_1M, 
    DSU_VSCALE_FILTER_BUTT
}DSU_VSCALE_FILTER_E;

/*DSU filter param type*/
typedef enum hiDSU_FILTER_PARAM_TYPE
{
    FILTER_PARAM_TYPE_NORM = 0,   
    FILTER_PARAM_TYPE_EX,		 
    FILTER_PARAM_TYPE_EX2,        
    FILTER_PARAM_TYPE_USER1,      
    FILTER_PARAM_TYPE_USER2,      
    FILTER_PARAM_TYPE_BUTT
}DSU_FILTER_PARAM_TYPE;

#define DSU_HFILTER_PARAM_NUM   792 
#define DSU_VFILTER_PARAM_NUM   480 
typedef struct hiDSU_FILTER_PARAM_S 
{ 
     DSU_FILTER_PARAM_TYPE enFiltType; 
     HI_U8 au8HParamTable[DSU_HFILTER_PARAM_NUM]; 
     HI_U8 au8VParamTable[DSU_VFILTER_PARAM_NUM]; 
}DSU_FILTER_PARAM_S; 

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* _HI_COMMON_H_ */

