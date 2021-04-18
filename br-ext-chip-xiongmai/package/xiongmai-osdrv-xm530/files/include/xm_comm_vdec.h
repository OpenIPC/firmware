#ifndef __XM_COMM_VENC_H__
#define __XM_COMM_VENC_H__

#include "xm_type.h"
#include "xm_common.h"
#include "xm_errno.h"
#include "xm_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/* invlalid channel ID */
#define XM_ERR_VDEC_INVALID_CHNID     XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define XM_ERR_VDEC_ILLEGAL_PARAM     XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define XM_ERR_VDEC_EXIST             XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define XM_ERR_VDEC_UNEXIST           XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define XM_ERR_VDEC_NULL_PTR          XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define XM_ERR_VDEC_NOT_CONFIG        XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define XM_ERR_VDEC_NOT_SUPPORT       XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define XM_ERR_VDEC_NOT_PERM          XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/*encode one frame not finish,maybe bus error,timeout,sys reset and so on*/
#define XM_ERR_VDEC_NOT_FINISH        XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_FINISH)
/* insufficient resource*/
#define XM_ERR_VDEC_INSUFF_RES        XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INSUFF_RES)
/* failure caused by malloc memory */
#define XM_ERR_VDEC_NOMEM             XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define XM_ERR_VDEC_NOBUF             XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define XM_ERR_VDEC_BUF_EMPTY         XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define XM_ERR_VDEC_BUF_FULL          XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define XM_ERR_VDEC_SYS_NOTREADY      XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*failure caused by hardware issue eg: hardware not init or config*/
#define XM_ERR_VDEC_HW_FAILED         XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_HW_FAILED)
/* system is busy*/
#define XM_ERR_VDEC_BUSY              XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* undefined error */
#define XM_ERR_VDEC_UND_ERROR         XM_DEF_ERR(XM_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_UND)


typedef enum xmVIDEO_MODE_E
{
	VIDEO_MODE_STREAM = 0,
	VIDEO_MODE_FRAME,
	VIDEO_MODE_BUTT
}VIDEO_MODE_E;

typedef struct xmVDEC_STREAM_S
{
	XM_U8 *pu8Addr;
	XM_U32 u32Len;
	XM_U64 u64PTS;
}VDEC_STREAM_S;

typedef struct xmVDEC_ATTR_JPEG_S
{
	VIDEO_MODE_E enMode;
}VDEC_ATTR_JPEG_S;

typedef struct xmVDEC_ATTR_VIDEO_S
{
	XM_U32 u32RefFrame;
	VIDEO_MODE_E enMode;
	XM_S32 s32SupportBFrame;
}VDEC_ATTR_VIDEO_S;

typedef struct xmVDEC_CHN_ATTR_S
{
	PAYLOAD_TYPE_E enType;
	XM_U32 u32BufSize;
	XM_U32 u32Priority;
	XM_U32 u32PicWidth;
	XM_U32 u32PicHeight;
	union
	{
		VDEC_ATTR_JPEG_S stVdecJpegAttr;
		VDEC_ATTR_VIDEO_S stVdecVideoAtrr;
	};
}VDEC_CHN_ATTR_S;

typedef struct xmVDEC_USERDATA_S
{
	XM_U8 *pu8Addr;
	XM_U32 u32PhyAddr;
	XM_U32 u32Len;
	XM_BOOL bValid;
}VDEC_USERDATA_S;


typedef struct xmVDEC_CHN_STAT_S
{
	XM_U32 u32LeftStreamBytes;/*待解码byte数*/
	XM_U32 u32LeftStreamFrames;/*待解码的frame数*/
	XM_U32 u32LeftPics;/*待获取的pic数*/
	XM_BOOL bStartRecvStream;/*是否允许接受码流*/
	XM_U32 u32RecvStreamFrames;/*已接收的frame数*/
	XM_U32 u32DecodeStreamFrames;/*已解码的frame数*/
}VDEC_CHN_STAT_S;

typedef union
{
	struct
	{
		XM_S32 IsAdvProfile;
		XM_S32 CodecVersion;
	}Vc1Ext;
	struct 
	{
		XM_S32 bReversed;
	}Vp6Ext;
}VDEC_STD_EXTENSION_U;

typedef struct xmVDEC_CHN_PARAM_S
{
	VDEC_STD_EXTENSION_U StdExt;
	XM_S32 s32ChanErrThr;
	XM_S32 s32ChanStrmOfThr;
	XM_S32 s32DecMode;
	XM_S32 s32DecOrderOutput;
	XM_S32 s32DnrTfEnable;
	XM_S32 s32DnrDispOutEnable;
	XM_U32 u32MaxFramesInDec;
}VDEC_CHN_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif

