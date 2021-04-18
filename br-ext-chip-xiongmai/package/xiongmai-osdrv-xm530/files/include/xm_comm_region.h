/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : xm_comm_region.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/


#ifndef __XM_COMM_REGION_H__
#define __XM_COMM_REGION_H__

#include "xm_common.h"
#include "xm_comm_video.h"
#include "xm_errno.h"
#include "xm_defines.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */





typedef XM_U32 RGN_HANDLE;

/* type of video regions */
typedef enum xmRGN_TYPE_E
{
    OVERLAY_RGN = 0,     /* video overlay region */
    COVER_RGN,
    COVEREX_RGN,
    OVERLAYEX_RGN,
    RGN_BUTT
} RGN_TYPE_E;

typedef struct xmOVERLAY_COMM_ATTR_S
{
	XM_U32 u32ColorMap;//3-0:背景调色盘索引 7-4:字符调色盘索引0,  bit 11-8:字符调色盘索引1,  
	XM_U32 u32Effect;      //BIt0-3 背景透明度,4-7淡入淡出,8-11闪烁,12-15水平放大,16-19垂直放大,20-23字符透明度
	XM_U32 u32Format;//字符色彩模式，1 代表单色彩，2 代表双色彩，4 代表15色彩
	RECT_S stRect;
}OVERLAY_ATTR_S;

typedef struct xmCOVER_COMM_ATTR_S
{
	XM_U32 u32Color;
	XM_U32 u32Effect;    //0边框  1实心
	RECT_S stRect;
}COVER_ATTR_S;

typedef union xmRGN_ATTR_U
{
	OVERLAY_ATTR_S stOverlay;
	COVER_ATTR_S stCover;
} RGN_ATTR_U;

typedef struct xmRGN_PALLET_S
{
	XM_U32 pallet[16];
}RGN_PALLET_S;

/* attribute of a region */
typedef struct xmRGN_ATTR_S
{
	XM_U32 u32Handle;
	RGN_TYPE_E enType;
	RGN_ATTR_U unAttr;
} RGN_ATTR_S;


#define RGN_MAX_BMP_UPD_NUM 16

typedef struct xmRGN_BMP_UPD_S
{
    POINT_S             stPoint;
    BITMAP_S            stBmp;
    XM_U32              u32Stride;
} RGN_BMP_UPD_S;

typedef struct xmRGN_BMP_UPD_CFG_S
{
    XM_U32              u32BmpCnt;
    RGN_BMP_UPD_S       astBmpUpd[RGN_MAX_BMP_UPD_NUM];
} RGN_BMP_UPD_CFG_S;


#define XM_TRACE_RGN(level, fmt...)  XM_TRACE(level, XM_ID_RGN,##fmt)
/* invlalid device ID */
#define XM_ERR_RGN_INVALID_DEVID     XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define XM_ERR_RGN_INVALID_CHNID     XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define XM_ERR_RGN_ILLEGAL_PARAM     XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define XM_ERR_RGN_EXIST             XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*UN exist*/
#define XM_ERR_RGN_UNEXIST           XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define XM_ERR_RGN_NULL_PTR          XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define XM_ERR_RGN_NOT_CONFIG        XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define XM_ERR_RGN_NOT_SUPPORT      XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define XM_ERR_RGN_NOT_PERM          XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define XM_ERR_RGN_NOMEM             XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define XM_ERR_RGN_NOBUF             XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define XM_ERR_RGN_BUF_EMPTY         XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define XM_ERR_RGN_BUF_FULL          XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define XM_ERR_RGN_BADADDR           XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define XM_ERR_RGN_BUSY              XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define XM_ERR_RGN_NOTREADY          XM_DEF_ERR(XM_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __XM_COMM_REGION_H__ */



