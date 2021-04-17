/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : xm_comm_aenc.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/


#ifndef  __XM_COMM_AENC_H__
#define  __XM_COMM_AENC_H__

#include "xm_type.h"
#include "xm_common.h"
#include "xm_comm_aio.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct xmAENC_ATTR_G711_S
{
    XM_U32 resv;            /*reserve item*/
}AENC_ATTR_G711_S;

typedef struct xmAENC_ATTR_G726_S
{
    G726_BPS_E enG726bps; 
}AENC_ATTR_G726_S;

typedef struct xmAENC_ATTR_ADPCM_S
{
    ADPCM_TYPE_E enADPCMType; 
}AENC_ATTR_ADPCM_S;

typedef struct xmAENC_ATTR_LPCM_S
{
    XM_U32 resv;            /*reserve item*/
}AENC_ATTR_LPCM_S;

typedef struct xmAENC_ENCODER_S
{
    PAYLOAD_TYPE_E  enType;
    XM_U32          u32MaxFrmLen;
    XM_CHAR         aszName[16];    /* encoder type,be used to print proc information */
    XM_S32          (*pfnOpenEncoder)(XM_VOID *pEncoderAttr, XM_VOID **ppEncoder); /* pEncoder is the handle to control the encoder */
    XM_S32          (*pfnEncodeFrm)(XM_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
                        XM_U8 *pu8Outbuf,XM_U32 *pu32OutLen);
    XM_S32          (*pfnCloseEncoder)(XM_VOID *pEncoder);
} AENC_ENCODER_S;

typedef struct xmAENC_CHN_ATTR_S 
{ 
    PAYLOAD_TYPE_E      enType;         /*payload type ()*/ 
    XM_U32              u32BufSize;      /*buf size [2~MAX_AUDIO_FRAME_NUM]*/
    XM_VOID             *pValue;  /*point to attribute of definite audio encoder*/       
}AENC_CHN_ATTR_S;

typedef enum xmEN_AENC_ERR_CODE_E
{
    ADEC_ERR_ENCODER_ERR     = 64 ,

} EN_AENC_ERR_CODE_E;


#define XM_TRACE_AENC(level, fmt...)  XM_TRACE(level, XM_ID_AENC,##fmt)

/* invlalid device ID */
#define XM_ERR_AENC_INVALID_DEVID     XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define XM_ERR_AENC_INVALID_CHNID     XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define XM_ERR_AENC_ILLEGAL_PARAM     XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define XM_ERR_AENC_EXIST             XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel unexists */
#define XM_ERR_AENC_UNEXIST           XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define XM_ERR_AENC_NULL_PTR          XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define XM_ERR_AENC_NOT_CONFIG        XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define XM_ERR_AENC_NOT_SUPPORT       XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define XM_ERR_AENC_NOT_PERM          XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define XM_ERR_AENC_NOMEM             XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define XM_ERR_AENC_NOBUF             XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define XM_ERR_AENC_BUF_EMPTY         XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define XM_ERR_AENC_BUF_FULL          XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define XM_ERR_AENC_SYS_NOTREADY      XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* encoder internal err */ 
#define XM_ERR_AENC_ENCODER_ERR       XM_DEF_ERR(XM_ID_AENC, EN_ERR_LEVEL_ERROR, ADEC_ERR_ENCODER_ERR)


#ifdef __cplusplus
#if __cplusplus
}
#endif 
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __XM_COMM_AENC_H__*/

