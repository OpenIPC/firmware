/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_comm_vda.h
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


#ifndef __XM_COMM_VDA_H__
#define __XM_COMM_VDA_H__

#include "xm_type.h"
#include "xm_debug.h"
#include "xm_comm_video.h"
#include "xm_common.h"
#include "xm_errno.h"
#include "xm_defines.h"


#if 0

/*Motion Region Data*/
typedef struct xmVDA_OBJ_S
{
    XM_U16 u16Left;
	XM_U16 u16Top;
	XM_U16 u16Right;
	XM_U16 u16Bottom;
}VDA_OBJ_S;



/*reference pictrue mode*/
typedef enum xmVDA_REF_MODE_E
{
	VDA_REF_MODE_DYNAMIC = 0,   /*reference pictrue dynamic*/
	VDA_REF_MODE_STATIC,	    /*reference pictrue static*/
	VDA_REF_MODE_USER,          /*reference pictrue user*/
	VDA_REF_MODE_BUTT           /*reserve*/
}VDA_REF_MODE_E;


/*VDA algorithm*/
typedef enum xmVDA_ALG_E
{
	VDA_ALG_REF,	    /*base on reference pictrue*/
	VDA_ALG_BUTT
}VDA_ALG_E;

/*MB size*/
typedef enum xmVDA_MB_SIZE_E
{
	VDA_MB_7PIXEL,      /* 7*7 */
	VDA_MB_BUTT	
}VDA_MB_SIZE_E;
#endif

/*SAD bits*/
typedef enum xmVDA_MB_SADBITS_E
{
    VDA_MB_SAD_8BIT = 0,  /*SAD precision 8bits*/
    VDA_MB_SAD_16BIT,     /*SAD precision 16bits*/
    VDA_MB_SAD_BUTT       /*reserve*/
} VDA_MB_SADBITS_E;



/*MD attribute*/
typedef struct xmVDA_MD_ATTR_S
{
    /*static attribute*/
	//VDA_ALG_E		 enVdaAlg;	    /*arithmetic*/
	//VDA_MB_SIZE_E    enMbSize;      /*MB size*/
	VDA_MB_SADBITS_E enMbSadBits;   /*MB SAD size*/
	//VDA_REF_MODE_E   enRefMode;     /*reference picture mode*/
	//XM_U32           u32MdBufNum;   /*Result buffer number,range:[1,16]*/ 
    
	/*dynamic attribute*/
	//XM_U32           u32VdaIntvl;   /*VDA interval,range:[0,256]*/

	XM_U32           u32BgUpSrcWgt; // 更新权重
                                      
	XM_U32	u32SadTh;      	// 报警阈值(0~255)<<8
	XM_U32	u32ObjNumMax;  // 最大运动区域的最大个数
}VDA_MD_ATTR_S;


#define VDA_OD_RGN_NUM_MAX 	1

/*OD attribute*/
typedef struct xmVDA_OD_ATTR_S
{
	XM_U32 u32SadTh;          /*SAD threshold,range:[0,4080]*/
	VDA_MB_SADBITS_E enMbSadBits;   /*MB SAD size*/
}VDA_OD_ATTR_S;


/*work mode*/
typedef enum xmVDA_WORK_MODE_E
{
	VDA_WORK_MODE_MD = 0, 	/*motion detection*/
	VDA_WORK_MODE_OD,     		/*Occlusion detection*/
	VDA_WORK_MODE_BUTT    
}VDA_WORK_MODE_E;


/*work mode attribute*/
typedef union xmVDA_WORK_MODE_ATTR_U
{
    VDA_MD_ATTR_S stMdAttr;  /*MD attribute*/
    VDA_OD_ATTR_S stOdAttr;  /*OD attribute*/
}VDA_WORK_MODE_ATTR_U;


/*VDA CHN attribute*/
typedef struct xmVDA_CHN_ATTR_S
{
	VDA_WORK_MODE_E enWorkMode;  /*work mode*/
	VDA_WORK_MODE_ATTR_U unAttr; /*work mode attribute*/
	XM_U32 u32Width; 	/*the width of CHNL,[16,VDA_MAX_WIDTH]*/
	XM_U32 u32Height;	/*the height of CHNL,[16,VDA_MAX_HEIGHT]*/
	XM_U16 u16VdaIntvl;	/*VDA interval,[0,65535]*/ // 视频间隔
}VDA_CHN_ATTR_S;


// 宏块部分信息
typedef struct xmVDA_SAD_DATA_S
{
	XM_BOOL           bSadValid;     /*SAD data is valid?*/
	XM_VOID	   	*pAddr;             		// 宏块统计信息
	XM_U32		u32Stride;        	// 宏块数据以字节为单位的内存行宽度
	XM_U32		u32TotalCnt;		// 宏块总个数
}VDA_SAD_DATA_S;



/*MD data*/
typedef struct xmVDA_MD_DATA_S
{
	// 运动部分信息
	XM_BOOL           bMdValid;       	// MD结果是否有效
	XM_VOID	   	*pAddr;             	// 结果地址
	XM_U32		u32Stride;        	// 结果数据以字节为单位的内存行宽度
	XM_U32		u32TotalCnt;		// 宏块总个数
	XM_U32		u32AlarmCnt;	// 报警个数
}VDA_MD_DATA_S;


/*OD data*/
typedef struct xmVDA_OD_DATA_S
{
    XM_BOOL bOdAlram; 				/*XM_TRUE:alarm*/
}VDA_OD_DATA_S;



typedef union xmVDA_DATA_U
{
	VDA_SAD_DATA_S stSadData;
	VDA_MD_DATA_S stMdData; /*MD data*/
	VDA_OD_DATA_S stOdData; /*OD data*/
}VDA_DATA_U;


/*VDA data*/
typedef struct xmVDA_DATA_S
{
    VDA_WORK_MODE_E enWorkMode;  /*work mode*/
    VDA_DATA_U      unData;      /*VDA data*/

    XM_U32          u32MbWidth;  /*VDA channle width in MB*/
    XM_U32          u32MbHeight; /*VDA channle height in MB*/
    XM_U64          u64Pts;      /*time*/    
}VDA_DATA_S;


/*chnnel state*/
typedef struct xmVDA_CHN_STAT_S
{
	XM_BOOL bStartRecvPic; 	/*start receive picture*/
	XM_U32 u32BufPcNow;     	// 现在指向的buffer位置
	XM_U32 u32StatusFlag;     	// 现在状态(0:NotOpen 1:InitOk 2:Get1Buffer 3:GetAllBuffer) 
}VDA_CHN_STAT_S;

/* invlalid device ID */
#define XM_ERR_VDA_INVALID_DEVID     XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define XM_ERR_VDA_INVALID_CHNID     XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define XM_ERR_VDA_ILLEGAL_PARAM     XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define XM_ERR_VDA_EXIST             XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*UN exist*/
#define XM_ERR_VDA_UNEXIST           XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define XM_ERR_VDA_NULL_PTR          XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define XM_ERR_VDA_NOT_CONFIG        XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define XM_ERR_VDA_NOT_SUPPORT      XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define XM_ERR_VDA_NOT_PERM          XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define XM_ERR_VDA_NOMEM             XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define XM_ERR_VDA_NOBUF             XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define XM_ERR_VDA_BUF_EMPTY         XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define XM_ERR_VDA_BUF_FULL          XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define XM_ERR_VDA_BADADDR           XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define XM_ERR_VDA_BUSY              XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define XM_ERR_VDA_NOTREADY          XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)



#endif /* End of #ifndef __XM_COMM_VDA_H__ */
