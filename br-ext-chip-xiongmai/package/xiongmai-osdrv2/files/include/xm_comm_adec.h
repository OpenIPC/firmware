/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name       	: 	xm_comm_adec.h
  Version        		: 	Initial Draft
  Author         		: 	XM multimedia software group
  Created       		: 	2015/9/17
  Last Modified		    :
  Description  		    : 
******************************************************************************/


#ifndef  __XM_COMM_ADEC_H__
#define  __XM_COMM_ADEC_H__


#include "xm_type.h"
#include "xm_common.h"
#include "xm_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct xmADEC_ATTR_G711_S
{
    XM_U32 resv; 
}ADEC_ATTR_G711_S;

typedef struct xmADEC_ATTR_G726_S
{
    G726_BPS_E enG726bps; 
}ADEC_ATTR_G726_S;

typedef struct xmADEC_ATTR_ADPCM_S
{
    ADPCM_TYPE_E enADPCMType; 
}ADEC_ATTR_ADPCM_S;

typedef struct xmADEC_ATTR_LPCM_S
{
    XM_U32 resv;
}ADEC_ATTR_LPCM_S;

typedef enum xmADEC_MODE_E
{
    ADEC_MODE_PACK = 0,/*require input is valid dec pack(a
                        complete frame encode result),
                        e.g.the stream get from AENC is a 
                        valid dec pack, the stream know actually
                        pack len from file is also a dec pack.
                        this mode is high-performative*/
    ADEC_MODE_STREAM ,/*input is stream,low-performative,
                        if you couldn't find out whether a stream is
                        vaild dec pack,you could use 
                        this mode*/
    ADEC_MODE_BUTT
}ADEC_MODE_E;

typedef struct xmADEC_CH_ATTR_S 
{ 
    PAYLOAD_TYPE_E enType; 
    XM_U32         u32BufSize;  /*buf size[2~MAX_AUDIO_FRAME_NUM]*/
    ADEC_MODE_E    enMode;      /*decode mode*/
    XM_VOID       *pValue; 
}ADEC_CHN_ATTR_S;

typedef struct xmADEC_DECODER_S
{
    PAYLOAD_TYPE_E  enType;
    XM_CHAR         aszName[16];
    XM_S32          (*pfnOpenDecoder)(XM_VOID *pDecoderAttr, XM_VOID **ppDecoder); /*struct ppDecoder is packed by user,user malloc and free memory for this struct */
    XM_S32          (*pfnDecodeFrm)(XM_VOID *pDecoder, XM_U8 **pu8Inbuf,XM_S32 *ps32LeftByte,
                        XM_U16 *pu16Outbuf,XM_U32 *pu32OutLen,XM_U32 *pu32Chns);
    XM_S32          (*pfnGetFrmInfo)(XM_VOID *pDecoder, XM_VOID *pInfo);
    XM_S32          (*pfnCloseDecoder)(XM_VOID *pDecoder);
} ADEC_DECODER_S;

typedef enum xmEN_ADEC_ERR_CODE_E
{
    ADEC_ERR_DECODER_ERR     = 64,   
    ADEC_ERR_BUF_LACK,   
    ADEC_ERR_VOICE_DEC_TYPE,   
    ADEC_ERR_VOICE_DEC_FRAMESIZE,       
    ADEC_ERR_VOICE_DEC_FRAMETYPE,     
    ADEC_ERR_VOICE_INVALID_DEVICE,
    ADEC_ERR_VOICE_INVALID_INBUF,
    ADEC_ERR_VOICE_INVALID_OUTBUF,
    ADEC_ERR_VOICE_TRANS_DEVICE,
    ADEC_ERR_VOICE_TRANS_TYPE,

} EN_ADEC_ERR_CODE_E;


typedef enum xm_ADEC_OUTPUT_MODE_E
{
    ADEC_OUTPUT_MODE_BIND = 0,   
    ADEC_OUTPUT_MODE_MANUAL,   
    ADEC_OUTPUT_MODE_BUTTL
} ADEC_OUTPUT_MODE_E;


#define XM_TRACE_ADEC(level, fmt...)  XM_TRACE(level, XM_ID_ADEC,##fmt)

/* invlalid device ID */
#define XM_ERR_ADEC_INVALID_DEVID     XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define XM_ERR_ADEC_INVALID_CHNID     XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define XM_ERR_ADEC_ILLEGAL_PARAM     XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define XM_ERR_ADEC_EXIST             XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel unexists */
#define XM_ERR_ADEC_UNEXIST           XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define XM_ERR_ADEC_NULL_PTR          XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define XM_ERR_ADEC_NOT_CONFIG        XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define XM_ERR_ADEC_NOT_SUPPORT       XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define XM_ERR_ADEC_NOT_PERM          XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define XM_ERR_ADEC_NOMEM             XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define XM_ERR_ADEC_NOBUF             XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define XM_ERR_ADEC_BUF_EMPTY         XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define XM_ERR_ADEC_BUF_FULL          XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define XM_ERR_ADEC_SYS_NOTREADY      XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* decoder internal err */ 
#define XM_ERR_ADEC_DECODER_ERR       XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */ 
#define XM_ERR_ADEC_BUF_LACK          XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_BUF_LACK)



#define XM_ERR_ADEC_DEC_TYPE		  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_DEC_TYPE)
#define XM_ERR_ADEC_DEC_FRAMESIZE	  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_DEC_FRAMESIZE)
#define XM_ERR_ADEC_DEC_FRAMETYPE	  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_DEC_FRAMETYPE)
#define XM_ERR_ADEC_INVALID_DEVICE	  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_INVALID_DEVICE)
#define XM_ERR_ADEC_INVALID_INBUF	  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_INVALID_INBUF)
#define XM_ERR_ADEC_INVALID_OUTBUF	  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_INVALID_OUTBUF)
#define XM_ERR_ADEC_TRANS_DEVICE	  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_TRANS_DEVICE)
#define XM_ERR_ADEC_TRANS_TYPE		  XM_DEF_ERR(XM_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_VOICE_TRANS_TYPE)




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __XM_COMM_ADEC_H__*/

