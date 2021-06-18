/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_SENSOR_DATATYPE_H_
#define _MI_SENSOR_DATATYPE_H_

#include "mi_sys_datatype.h"
#include "mi_vif_datatype.h"
#pragma pack(push)
#pragma pack(4)

#define MI_ERR_SNR_INVALID_DEVID         MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID)
#define MI_ERR_SNR_INVALID_CHNID         MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID)
#define MI_ERR_SNR_INVALID_PARA          MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM)
#define MI_ERR_SNR_INVALID_NULL_PTR      MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR)
#define MI_ERR_SNR_FAILED_NOTCONFIG      MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG)
#define MI_ERR_SNR_NOT_SUPPORT           MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT)
#define MI_ERR_SNR_NOT_PERM              MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM)
#define MI_ERR_SNR_NOMEM                 MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM)
#define MI_ERR_SNR_BUF_EMPTY             MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY)
#define MI_ERR_SNR_BUF_FULL              MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL)
#define MI_ERR_SNR_SYS_NOTREADY          MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY)
#define MI_ERR_SNR_BUSY                  MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY)
#define MI_ERR_SNR_FAIL                  MI_DEF_ERR(E_MI_MODULE_ID_SNR, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED)

#define MI_SNR_MAX_PADNUM 4
#define MI_SNR_MAX_PLANENUM 3

typedef struct MI_SNR_Res_s
{
	MI_SYS_WindowRect_t  stCropRect;
	MI_SYS_WindowSize_t  stOutputSize;  /**< Sensor actual output size */

	MI_U32 u32MaxFps;    /**< Max fps in this resolution */
	MI_U32 u32MinFps;    /**< Min fps in this resolution*/
	MI_S8 strResDesc[32];	// Need to put “HDR” here if the resolution is for HDR
} __attribute__((packed, aligned(4))) MI_SNR_Res_t;


typedef enum
{
    E_MI_SNR_HDR_HW_MODE_NONE = 0,
    E_MI_SNR_HDR_HW_MODE_SONY_DOL = 1,
    E_MI_SNR_HDR_HW_MODE_DCG = 2,
    E_MI_SNR_HDR_HW_MODE_EMBEDDED_RAW8 = 3,
    E_MI_SNR_HDR_HW_MODE_EMBEDDED_RAW10 = 4,
    E_MI_SNR_HDR_HW_MODE_EMBEDDED_RAW12 = 5,
    E_MI_SNR_HDR_HW_MODE_EMBEDDED_RAW16 = 6, //Only for OV2718?
} MI_SNR_HDRHWMode_e;

typedef enum
{
    E_MI_SNR_PAD_ID_0 = 0,
    E_MI_SNR_PAD_ID_1 = 1,
    E_MI_SNR_PAD_ID_2 = 2,
    E_MI_SNR_PAD_ID_3 = 3,
    E_MI_SNR_PAD_ID_MAX = 3,
    E_MI_SNR_PAD_ID_NA = 0xFF,
} MI_SNR_PAD_ID_e;

typedef enum
{
    E_MI_SNR_HDR_SOURCE_VC0,
    E_MI_SNR_HDR_SOURCE_VC1,
    E_MI_SNR_HDR_SOURCE_VC2,
    E_MI_SNR_HDR_SOURCE_VC3,
    E_MI_SNR_HDR_SOURCE_MAX
} MI_SNR_HDRSrc_e;

typedef enum
{
    E_MI_SNR_CUSTDATA_TO_DRIVER,
    E_MI_SNR_CUSTDATA_TO_USER,
    E_MI_SNR_CUSTDATA_MAX = E_MI_SNR_CUSTDATA_TO_USER,
} MI_SNR_CUST_DIR_e;

typedef struct MI_SNR_AttrParallel_s
{
    MI_VIF_SyncAttr_t stSyncAttr;
} MI_SNR_AttrParallel_t;
// Should be the same structure definition with the one used in VIF

typedef  struct MI_SNR_MipiAttr_s
{
	MI_U32  u32LaneNum;
	MI_U32  u32DataFormat;	    //0: YUV 422 format. 1: RGB pattern.
    MI_VIF_DataYuvSeq_e    eDataYUVOrder;
	MI_U32	u32HsyncMode;
	MI_U32  u32Sampling_delay;
	/** < MIPI start sampling delay */ /*bit 0~7: clk_skip_ns. bit 8~15: data_skip_ns*/
	MI_SNR_HDRHWMode_e  eHdrHWmode;
    MI_U32  u32Hdr_Virchn_num;        //??
	MI_U32  u32Long_packet_type[2];    //??
}MI_SNR_MipiAttr_t;

typedef struct  MI_SNR_AttrBt656_s
{
	MI_U32  u32Multiplex_num;
    MI_VIF_SyncAttr_t stSyncAttr;
	MI_VIF_ClkEdge_e 	eClkEdge;
	MI_VIF_BitOrder_e   eBitSwap;
} MI_SNR_AttrBt656_t;

typedef union {
	MI_SNR_AttrParallel_t  stParallelAttr;
	MI_SNR_MipiAttr_t 	stMipiAttr;
	MI_SNR_AttrBt656_t	stBt656Attr;
} MI_SNR_IntfAttr_u;

typedef struct MI_SNR_PADInfo_s
{
    MI_U32              u32PlaneCount;	//It is different expo number for HDR. It is mux number for BT656. //??
    MI_VIF_IntfMode_e    eIntfMode;
    MI_VIF_HDRType_e     eHDRMode;
    MI_SNR_IntfAttr_u    unIntfAttr;
    MI_BOOL              bEarlyInit;
} MI_SNR_PADInfo_t;

typedef struct MI_SNR_PlaneInfo_s
{
    MI_U32                  u32PlaneID;// For HDR long/short exposure or BT656 channel 0~3
    MI_S8                   s8SensorName[32];
    MI_SYS_WindowRect_t     stCapRect;
    MI_SYS_BayerId_e        eBayerId;
    MI_SYS_DataPrecision_e  ePixPrecision;
    MI_SNR_HDRSrc_e         eHdrSrc;
    MI_U32                  u32ShutterUs;
    MI_U32                  u32SensorGainX1024;
    MI_U32                  u32CompGain;
    MI_SYS_PixelFormat_e    ePixel;
} MI_SNR_PlaneInfo_t;

typedef enum
{
    E_MI_SNR_ANADEC_SRC_NO_READY = 0,
    E_MI_SNR_ANADEC_SRC_DISCNT,
    E_MI_SNR_ANADEC_SRC_PAL,
    E_MI_SNR_ANADEC_SRC_NTSC,
    E_MI_SNR_ANADEC_SRC_HD_25P,
    E_MI_SNR_ANADEC_SRC_HD_30P,
    E_MI_SNR_ANADEC_SRC_HD_50P,
    E_MI_SNR_ANADEC_SRC_HD_60P,
    E_MI_SNR_ANADEC_SRC_FHD_25P,
    E_MI_SNR_ANADEC_SRC_FHD_30P,
    E_MI_SNR_ANADEC_SRC_NUM
} MI_SNR_Anadec_SrcType_e;

typedef struct MI_SNR_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_SNR_InitParam_t;

#pragma pack(pop)

#endif
