/******************************************************************************

  Copyright (C), 2001-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name       	: 	hi_comm_adec.h
  Version        		: 	Initial Draft
  Author         		: 	Hisilicon multimedia software group
  Created       		: 	2006/12/15
  Last Modified		    :
  Description  		    :
  Function List 		:
  History       		:
  1.Date        		: 	2006/12/15
    Author      		: 	z50825
    Modification   	    :	Created file
  2.Date                :   2007/5/10
    Author              :   z50825
    Modification        :   add err code
******************************************************************************/


#ifndef  __HI_COMM_ADEC_H__
#define  __HI_COMM_ADEC_H__


#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiADEC_ATTR_G711_S {
    HI_U32 resv;
} ADEC_ATTR_G711_S;

typedef struct hiADEC_ATTR_G726_S {
    G726_BPS_E enG726bps;
} ADEC_ATTR_G726_S;

typedef struct hiADEC_ATTR_ADPCM_S {
    ADPCM_TYPE_E enADPCMType;
} ADEC_ATTR_ADPCM_S;

typedef struct hiADEC_ATTR_LPCM_S {
    HI_U32 resv;
} ADEC_ATTR_LPCM_S;

typedef enum hiADEC_MODE_E {
    ADEC_MODE_PACK = 0, /* require input is valid dec pack(a
                           complete frame encode result),
                           e.g.the stream get from AENC is a
                           valid dec pack, the stream know actually
                           pack len from file is also a dec pack.
                           this mode is high-performative */
    ADEC_MODE_STREAM,   /* input is stream, low-performative,
                           if you couldn't find out whether a stream is
                           vaild dec pack,you could use
                           this mode */
    ADEC_MODE_BUTT
}ADEC_MODE_E;

typedef struct hiADEC_CH_ATTR_S {
    PAYLOAD_TYPE_E enType;
    HI_U32         u32BufSize;  /* buf size[2~MAX_AUDIO_FRAME_NUM] */
    ADEC_MODE_E    enMode;      /* decode mode */
    HI_VOID ATTRIBUTE      *pValue;
}ADEC_CHN_ATTR_S;

typedef struct hiADEC_CHN_STATE_S {
    HI_BOOL bEndOfStream;             /* EOS flag */
    HI_U32 u32BufferFrmNum;           /* total number of channel buffer */
    HI_U32 u32BufferFreeNum;          /* free number of channel buffer */
    HI_U32 u32BufferBusyNum;          /* busy number of channel buffer */
} ADEC_CHN_STATE_S;

typedef struct hiADEC_DECODER_S {
    PAYLOAD_TYPE_E  enType;
    HI_CHAR         aszName[17];
    HI_S32          (*pfnOpenDecoder)(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder); /* struct ppDecoder is packed by user,
    user malloc and free memory for this struct */
    HI_S32          (*pfnDecodeFrm)(HI_VOID *pDecoder, HI_U8 **pu8Inbuf, HI_S32 *ps32LeftByte,
                        HI_U16 *pu16Outbuf, HI_U32 *pu32OutLen, HI_U32 *pu32Chns);
    HI_S32          (*pfnGetFrmInfo)(HI_VOID *pDecoder, HI_VOID *pInfo);
    HI_S32          (*pfnCloseDecoder)(HI_VOID *pDecoder);
    HI_S32          (*pfnResetDecoder)(HI_VOID *pDecoder);
} ADEC_DECODER_S;

typedef enum hiEN_ADEC_ERR_CODE_E {
    ADEC_ERR_DECODER_ERR     = 64,
    ADEC_ERR_BUF_LACK,
} EN_ADEC_ERR_CODE_E;


/* invlalid device ID */
#define HI_ERR_ADEC_INVALID_DEVID     HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HI_ERR_ADEC_INVALID_CHNID     HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_ADEC_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_ADEC_EXIST             HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel unexists */
#define HI_ERR_ADEC_UNEXIST           HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_ADEC_NULL_PTR          HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_ADEC_NOT_CONFIG        HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_ADEC_NOT_SUPPORT       HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_ADEC_NOT_PERM          HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_ADEC_NOMEM             HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_ADEC_NOBUF             HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_ADEC_BUF_EMPTY         HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_ADEC_BUF_FULL          HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_ADEC_SYS_NOTREADY      HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* decoder internal err */
#define HI_ERR_ADEC_DECODER_ERR       HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */
#define HI_ERR_ADEC_BUF_LACK          HI_DEF_ERR(HI_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_BUF_LACK)




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __HI_COMM_ADEC_H__*/

