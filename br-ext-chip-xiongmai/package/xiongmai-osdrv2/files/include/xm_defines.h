/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_defines.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/6/27

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/6/27
  Author		: Lycai
  Modification	: creat
******************************************************************************/

#ifndef __XM_DEFINES_H__
#define __XM_DEFINES_H__


#define BITMSK(bit)				(XM_S32)(1 << (bit))

#define LINE_LEN_BIT            5
#define LINE_LEN                (1<<LINE_LEN_BIT)
#define LINE_BASE_MASK          (~(LINE_LEN-1))
/* For Sys */
#define DEFAULT_ALIGN    16
#define MAX_MMZ_NAME_LEN 16

#define MAX_NODE_NUM        16

/* For VDA */
#define VDA_MAX_NODE_NUM        32
#define VDA_MAX_INTERNAL        256
#define VDA_CHN_NUM_MAX         32
#define VDA_MAX_WIDTH           960
#define VDA_MAX_HEIGHT          576

/* For H264 H265 JPEG */
#define VENC_MAX_CHN_NUM   4

/* For Region */
#define OVERLAY_MAX_NUM        8
#define COVER_MAX_NUM          32
#define RGN_MAX_CHN_NUM        3

/* number of channle and device on video input unit of chip
 * Note! VIU_MAX_CHN_NUM is NOT equal to VIU_MAX_DEV_NUM
 * multiplied by VIU_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VIU_MAX_CHN_NUM              3




#define AIO_MAX_CHN_NUM         1
#define AENC_MAX_CHN_NUM        1
#define ADEC_MAX_CHN_NUM        1

#define AI_DEV_MAX_NUM          1
#define AO_DEV_MIN_NUM          0
#define AO_DEV_MAX_NUM          1

#endif /* __XM_DEFINES_H__ */

