/******************************************************************************
*
*  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.
*
*******************************************************************************
*  File Name     : hi_comm_vda.h
*  Version       : Initial Draft
*  Author        : j00169368,l64467
*  Created       : 2011/1/7
*  Last Modified :
*  Description   : include VDA struct , Marco and Error information
*  Function List :
*
*
*  History:
* 
*       1.  Date         : 2011/1/7
*           Author       : j00169368,l64467
*           Modification : Created file
*
******************************************************************************/

#ifndef __HI_COMM_VDA_H__
#define __HI_COMM_VDA_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_comm_video.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/*Motion Region Data*/
typedef struct hiVDA_OBJ_S
{
    HI_U16 u16Left;
	HI_U16 u16Top;
	HI_U16 u16Right;
	HI_U16 u16Bottom;
}VDA_OBJ_S;

/*reference pictrue mode*/
typedef enum hiVDA_REF_MODE_E
{
	VDA_REF_MODE_DYNAMIC = 0,   /*reference pictrue dynamic*/
	VDA_REF_MODE_STATIC,	    /*reference pictrue static*/
    VDA_REF_MODE_USER,          /*reference pictrue user*/
	VDA_REF_MODE_BUTT           /*reserve*/
}VDA_REF_MODE_E;

/*VDA algorithm*/
typedef enum hiVDA_ALG_E
{
	VDA_ALG_BG = 0,		/*base on background pictrue*/
	VDA_ALG_REF,	    /*base on reference pictrue*/
	VDA_ALG_BUTT
}VDA_ALG_E;

/*MB size*/
typedef enum hiVDA_MB_SIZE_E
{
	VDA_MB_8PIXEL,      /* 8*8 */          
	VDA_MB_16PIXEL,     /* 16*16 */
	VDA_MB_BUTT	
}VDA_MB_SIZE_E;

/*SAD bits*/
typedef enum hiVDA_MB_SADBITS_E
{
    VDA_MB_SAD_8BIT = 0,  /*SAD precision 8bits*/
    VDA_MB_SAD_16BIT,     /*SAD precision 16bits*/
    VDA_MB_SAD_BUTT       /*reserve*/
} VDA_MB_SADBITS_E;


/*OD region attribute*/
typedef struct hiVDA_OD_RGN_ATTR_S
{
    /*static attribute*/
    RECT_S stRect;            /*region rect
                               *X:[0,VDA_MAX_WIDTH),align:16
                               *Y:[0,VDA_MAX_HEIGHT)
                               *W:[16,VDA_MAX_WIDTH],align:16
                               *H:[16,VDA_MAX_HEIGHT],align:16
                               *X+W <= chunnel wight
                               *Y+H <= chunnel height
                               */

    /*dynamic attribute*/
    HI_U32 u32SadTh;          /*SAD threshold,range:[0,4080]*/
    HI_U32 u32AreaTh;         /*alarm area threshold,range:[0,100]*/
	HI_U32 u32OccCntTh;	      /*alarm frame count threshold,range:[1,256]*/
    HI_U32 u32UnOccCntTh;	  /*The max uncover count,range:[0,256]*/
}VDA_OD_RGN_ATTR_S;

/*MD attribute*/
typedef struct hiVDA_MD_ATTR_S
{
    /*static attribute*/
	VDA_ALG_E		 enVdaAlg;	    /*arithmetic*/
    VDA_MB_SIZE_E    enMbSize;      /*MB size*/
    VDA_MB_SADBITS_E enMbSadBits;   /*MB SAD size*/
    VDA_REF_MODE_E   enRefMode;     /*reference picture mode*/
    HI_U32           u32MdBufNum;   /*Result buffer number,range:[1,16]*/ 
    
    /*dynamic attribute*/
    HI_U32           u32VdaIntvl;   /*VDA interval,range:[0,256]*/
    
    HI_U32           u32BgUpSrcWgt;  /*the source picture weight,
                                      *back ground updata totle weight 256,
                                      *range:[1,255],recommendatory value:128
                                      */
                                      
    HI_U32			 u32SadTh;      /*SAD threshold,range:[0,4080],recommendatory value:100*/
    HI_U32           u32ObjNumMax;  /*max OBJ number,range:[1,128]*/
}VDA_MD_ATTR_S;


#define VDA_OD_RGN_NUM_MAX 4

/*OD attribute*/
typedef struct hiVDA_OD_ATTR_S
{
    HI_U32           u32RgnNum;     /*region number*/
    VDA_OD_RGN_ATTR_S astOdRgnAttr[VDA_OD_RGN_NUM_MAX]; /*region attribute*/
    
    /*static attribute*/
	VDA_ALG_E		 enVdaAlg;		/*arithmetic*/
    VDA_MB_SIZE_E    enMbSize;      /*MB size*/
    VDA_MB_SADBITS_E enMbSadBits;   /*MB SAD size*/
    VDA_REF_MODE_E   enRefMode;     /*reference picture mode*/
        
    /*dynamic attribute*/
    HI_U32           u32VdaIntvl;   /*VDA interval,[0,256]*/
    HI_U32           u32BgUpSrcWgt;  /*back ground updata totle weight 256,the source picture weight,[1,255]*/

    
}VDA_OD_ATTR_S;


/*work mode*/
typedef enum hiVDA_WORK_MODE_E
{
	VDA_WORK_MODE_MD = 0, /*motion detection*/
    VDA_WORK_MODE_OD,     /*Occlusion detection*/
    VDA_WORK_MODE_BUTT    
}VDA_WORK_MODE_E;


/*work mode attribute*/
typedef union hiVDA_WORK_MODE_ATTR_U
{
    VDA_MD_ATTR_S stMdAttr;  /*MD attribute*/
    VDA_OD_ATTR_S stOdAttr;  /*OD attribute*/
}VDA_WORK_MODE_ATTR_U;


/*VDA CHN attribute*/
typedef struct hiVDA_CHN_ATTR_S
{
    VDA_WORK_MODE_E enWorkMode;  /*work mode*/
    VDA_WORK_MODE_ATTR_U unAttr; /*work mode attribute*/
    HI_U32 u32Width;             /*the width of CHNL,[16,VDA_MAX_WIDTH]*/
    HI_U32 u32Height;            /*the height of CHNL,[16,VDA_MAX_HEIGHT]*/
}VDA_CHN_ATTR_S;


/*MB SAD data*/
typedef struct hiVDA_MB_SAD_DATA_S
{
    HI_VOID	   *pAddr;             /*address*/
    HI_U32		u32Stride;         /*stride*/
    VDA_MB_SADBITS_E enMbSadBits;  /*the MB SAD size*/
}VDA_MB_SAD_DATA_S;

/*OBJ data*/
typedef struct hiVDA_OBJ_DATA_S
{
    HI_U32 u32ObjNum;              /*OBJ number*/
    VDA_OBJ_S	*pstAddr;          /*OBJ data address*/
    
    HI_U32 u32IndexOfMaxObj;       /*index of max OBJ*/
    HI_U32 u32SizeOfMaxObj;        /*size of max OBJ*/
	HI_U32 u32SizeOfTotalObj;      /*total size of all OBJ*/
}VDA_OBJ_DATA_S;


/*MD data*/
typedef struct hiVDA_MD_DATA_S
{
    HI_BOOL           bMbSadValid;     /*SAD data is valid?*/
    VDA_MB_SAD_DATA_S stMbSadData;    /*MB SAD data*/
    
    HI_BOOL           bObjValid;       /*OBJ data is valid?*/
    VDA_OBJ_DATA_S    stObjData;      /*OBJ data*/

    HI_BOOL           bPelsNumValid;   /*alarm pixel number data is valid?*/
	HI_U32 			  u32AlarmPixCnt;/*motion pix of picture*/
}VDA_MD_DATA_S;


/*OD data*/
typedef struct hiVDA_OD_DATA_S
{
    HI_U32  u32RgnNum;                      /*region number*/
    HI_BOOL abRgnAlarm[VDA_OD_RGN_NUM_MAX]; /*HI_TRUE:alarm*/
}VDA_OD_DATA_S;



typedef union hiVDA_DATA_U
{
    VDA_MD_DATA_S stMdData; /*MD data*/
    VDA_OD_DATA_S stOdData; /*OD data*/
}VDA_DATA_U;


/*VDA data*/
typedef struct hiVDA_DATA_S
{
    VDA_WORK_MODE_E enWorkMode;  /*work mode*/
    VDA_DATA_U      unData;      /*VDA data*/
    VDA_MB_SIZE_E   enMbSize;    /*MB size*/
    HI_U32          u32MbWidth;  /*VDA channle width in MB*/
    HI_U32          u32MbHeight; /*VDA channle height in MB*/
    HI_U64          u64Pts;      /*time*/    
}VDA_DATA_S;


/*chnnel state*/
typedef struct hiVDA_CHN_STAT_S
{
    HI_BOOL bStartRecvPic; /*start receive picture*/
	HI_U32 u32LeftPic;     /*left picture*/
	HI_U32 u32LeftRst;     /*left data*/   
}VDA_CHN_STAT_S;

/* invlalid device ID */
#define HI_ERR_VDA_INVALID_DEVID     HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HI_ERR_VDA_INVALID_CHNID     HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_VDA_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_VDA_EXIST             HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*UN exist*/
#define HI_ERR_VDA_UNEXIST           HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_VDA_NULL_PTR          HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_VDA_NOT_CONFIG        HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_VDA_NOT_SUPPORT      HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_VDA_NOT_PERM          HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_VDA_NOMEM             HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_VDA_NOBUF             HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_VDA_BUF_EMPTY         HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_VDA_BUF_FULL          HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define HI_ERR_VDA_BADADDR           HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define HI_ERR_VDA_BUSY              HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define HI_ERR_VDA_NOTREADY          HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_COMM_VDA_H__ */
