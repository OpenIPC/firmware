
/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_pciv.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software pciv
  Created       : 2008/06/04
  Last Modified :
  Description   : common struct definition for PCIV
  Function List :
  History       :

******************************************************************************/

#ifndef __HI_COMM_PCIV_H__
#define __HI_COMM_PCIV_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"
#include "hi_comm_vdec.h"
#include "hi_comm_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef HI_S32 PCIV_CHN;

#define PCIV_MAX_BUF_NUM 16   /* pciv channel max buffer number */
#define PCIV_MAX_CHIPNUM 32   /* max pciv device number which can join in the system */

#define MAKE_DWORD(high,low) (((low)&0x0000ffff)|((high)<<16))
#define HIGH_WORD(x) (((x)&0xffff0000)>>16)
#define LOW_WORD(x) ((x)&0x0000ffff)

/* vi object struct */
typedef struct hiPCIV_VIDEVICE_S
{
    VI_DEV viDev;    /* vi device number */
    VI_CHN viChn;    /* vi channel number */
} PCIV_VIDEVICE_S;

/* vo object struct */
typedef struct hiPCIV_VODEVICE_S
{
    VO_DEV voDev;    /* vo device number */
    VO_CHN voChn;    /* vo channel number */
} PCIV_VODEVICE_S;

/* vdec object struct */
typedef struct hiPCIV_VDECDEVICE_S
{
    VDEC_CHN vdecChn;  /* vedc channel number */
} PCIV_VDECDEVICE_S;

/* vpss object struct */
typedef struct hiPCIV_VPSSDEVICE_S
{
    VPSS_GRP vpssGrp; /* vpss group number */
    VPSS_CHN vpssChn; /* vpss channel number */
} PCIV_VPSSDEVICE_S;

/* bind type for pciv */
typedef enum hiPCIV_BIND_TYPE_E
{
    PCIV_BIND_VI    = 0,
    PCIV_BIND_VO    = 1,
    PCIV_BIND_VDEC  = 2,
    PCIV_BIND_VPSS  = 3,
    PCIV_BIND_BUTT
} PCIV_BIND_TYPE_E;

/* bind object struct for pciv */
typedef struct hiPCI_BIND_OBJ_S
{
    PCIV_BIND_TYPE_E enType;  /* bind type for pciv */
    union
    {
        PCIV_VIDEVICE_S   viDevice;
        PCIV_VODEVICE_S   voDevice;
        PCIV_VDECDEVICE_S vdecDevice;
        PCIV_VPSSDEVICE_S vpssDevice;
    } unAttachObj;
} PCIV_BIND_OBJ_S;

/* remote pciv object */
typedef struct hiPCIV_REMOTE_OBJ_S
{
    HI_S32   s32ChipId;  /* remote pciv device Id number */
    PCIV_CHN pcivChn;    /* pciv channel number of remote pciv device */
} PCIV_REMOTE_OBJ_S;

/* attribution of target picture  */
typedef struct hiPCIV_PIC_ATTR_S
{
    HI_U32   u32Width;             /* pciture width of pciv channel */
    HI_U32   u32Height;            /* picture height of pciv channel */
    HI_U32   u32Stride[3];         /* pciture stride of pciv channel */
    VIDEO_FIELD_E   u32Field;      /* video frame field type of pciv channel */
    PIXEL_FORMAT_E  enPixelFormat; /* pixel format of pciture of pciv channel */
} PCIV_PIC_ATTR_S;


/* attribution of pciv chn */
typedef struct hiPCIV_ATTR_S
{
    PCIV_PIC_ATTR_S   stPicAttr;                    /* picture attibute */
    HI_S32            s32BufChip;                   /* The chip id which buffer is belong to */
    HI_U32            u32BlkSize;                   /* vb size of receiver for preview */
    HI_U32            u32Count;                     /* lenght of address list */
    HI_U32            u32PhyAddr[PCIV_MAX_BUF_NUM]; /* address list for picture move */
    PCIV_REMOTE_OBJ_S stRemoteObj;                  /* remote pciv object */
} PCIV_ATTR_S;

/* max count of video buffer block for pci window */
#define PCIV_MAX_VBCOUNT 8

/* mpp video buffer config for pci window */
typedef struct hiPCIV_WINVBCFG_S
{
    HI_U32 u32PoolCount;                     /* total number of video buffer pool  */
    HI_U32 u32BlkSize[PCIV_MAX_VBCOUNT];     /* size of video buffer pool */
    HI_U32 u32BlkCount[PCIV_MAX_VBCOUNT];    /* number of video buffer pool */
} PCIV_WINVBCFG_S;

typedef struct hiPCIV_BASEWINDOW_S
{
    HI_S32 s32ChipId;     /* pciv device number */
    HI_U32 u32NpWinBase;  /* non-prefetch window pcie base address */
    HI_U32 u32PfWinBase;  /* prefetch window pcie base address */
    HI_U32 u32CfgWinBase; /* config window pcie base address */
    HI_U32 u32PfAHBAddr;  /* prefetch window AHB base address */
} PCIV_BASEWINDOW_S;

#define PCIV_MAX_DMABLK 128
typedef struct hiPCIV_DMA_BLOCK_S
{
    HI_U32  u32SrcAddr;  /* source address of dma task */
    HI_U32  u32DstAddr;  /* destination address of dma task */
    HI_U32  u32BlkSize;  /* data block size of dma task */
} PCIV_DMA_BLOCK_S;

typedef struct hiPCIV_DMA_TASK_S
{
    HI_U32  u32Count;         /* total dma task number */
    HI_BOOL bRead;            /* dam task is  read or write data */
    PCIV_DMA_BLOCK_S *pBlock;
} PCIV_DMA_TASK_S;


/*
 * Message port used in pciv_drvadp, you can't use it in your application.
 */
#define PCIV_MSGPORT_KERNEL     80  


/*
 * You should not use follow mocros, they will be deleted !
 */
#define PCIV_MSGPORT_MAXPORT    100
#define PCIV_MSGPORT_TIME       79  
#define PCIV_MSGPORT_USERCMD    81
#define PCIV_MSGPORT_USERNOTIFY2HOST  82
#define PCIV_MSGPORT_USERNOTIFY2SLAVE 83
#define PCIV_MSGPORT_USERNOTIFY2HOST_VDEC  84
#define PCIV_MSGPORT_USERNOTIFY2SLAVE_VDEC 85
#define PCIV_MAXVO_BIND 4
/*----------------------------------------*/

typedef enum hiPCIV_FILTER_TYPE_E
{
    PCIV_FILTER_TYPE_NORM,
    PCIV_FILTER_TYPE_EX,
    PCIV_FILTER_TYPE_EX2,
    
    PCIV_FILTER_TYPE_BUTT
} PCIV_FILTER_TYPE_E;

typedef enum hiPCIV_PIC_FIELD_E
{
    PCIV_FIELD_TOP,
    PCIV_FIELD_BOTTOM,
    PCIV_FIELD_BOTH,
    
    PCIV_FIELD_BUTT
} PCIV_PIC_FIELD_E;

/* 
 * configuration of pre-process before sending source picture whth PCI 
 * item in this structrue have default value when sys init 
 */
typedef struct hiPCIV_PREPROC_CFG_S
{    
    PCIV_PIC_FIELD_E        enFieldSel;     /* pictrue field select */
    PCIV_FILTER_TYPE_E      enFilterType;   /* filter group type */
    DSU_HSCALE_FILTER_E     enHFilter;      /* horizontal filter */
    DSU_VSCALE_FILTER_E     enVFilterL;     /* vertical filter of luminance */
    DSU_VSCALE_FILTER_E     enVFilterC;     /* vertical filter of chroma */
} PCIV_PREPROC_CFG_S;


#if 0
/* Synchronize the time and PTS. The host is base. */
typedef struct hiPCIV_TIME_SYNC_S
{
    struct timeval stSysTime;  /* The current system time */
    HI_U64         u64PtsBase; /* The media PTS */
    HI_U32         u32ReqTagId;
} PCIV_TIME_SYNC_S;
#endif

/* invlalid channel ID */
#define HI_ERR_PCIV_INVALID_CHNID       HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_PCIV_ILLEGAL_PARAM       HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_PCIV_EXIST               HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define HI_ERR_PCIV_UNEXIST             HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_PCIV_NULL_PTR            HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_PCIV_NOT_CONFIG          HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_PCIV_NOT_SUPPORT         HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_PCIV_NOT_PERM            HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_PCIV_NOMEM               HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_PCIV_NOBUF               HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_PCIV_BUF_EMPTY           HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_PCIV_BUF_FULL            HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define HI_ERR_PCIV_SYS_NOTREADY        HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* One DMA task is working, wait a minute */
#define HI_ERR_PCIV_BUSY                HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_PCIV_TIMEOUT             HI_DEF_ERR(HI_ID_PCIV, EN_ERR_LEVEL_ERROR, EN_ERR_BUTT+1)

#define PCIV_TRACE(level, fmt...)\
do{ \
    HI_TRACE(level, HI_ID_PCIV,"[Func]:%s [Line]:%d [Info]:", __FUNCTION__, __LINE__);\
    HI_TRACE(level,HI_ID_PCIV,##fmt);\
}while(0)

#define PCIV_CHECK_CHNID(ChnID)\
do{\
    if(((ChnID) < 0) || ((ChnID) >= PCIV_MAX_CHN_NUM))\
    {\
        PCIV_TRACE(HI_DBG_ERR, "invalid chn id:%d \n", ChnID);\
        return HI_ERR_PCIV_INVALID_CHNID;\
    }\
}while(0)

#define PCIV_CHECK_PTR(ptr)\
do{\
    if(NULL == (ptr))\
    {\
        return HI_ERR_PCIV_NULL_PTR;\
    }\
}while(0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_PCIV_H__ */

