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
#ifndef _MI_SYS_DATATYPE_H_
#define _MI_SYS_DATATYPE_H_

#include "mi_common.h"
#define MI_SYS_MAX_INPUT_PORT_CNT   (16)
#define MI_SYS_MAX_OUTPUT_PORT_CNT  (5)
#define MI_SYS_MAX_DEV_CHN_CNT      (48)
#define MI_SYS_INVLAID_SEQUENCE_NUM ((MI_U32)-1)


//ensure that sizeof(MI_VB_PoolListConf_t)  is less that 4096 !!!
#define MI_MAX_MMA_HEAP_LENGTH      (32)
#define MI_VB_POOL_LIST_MAX_CNT     (8)

#define MI_SYS_INVALID_PTS (0xffffffffffffffffULL)

#define MI_VB_BLK_HANDLE_INVALID   (-1)
#define MI_VB_POOL_HANDLE_INVALID  (-1)

/* UserHandler ExFlags start. */
#define MI_SYS_MAP_VA               0x80000000
#define MI_SYS_MAP_CPU_READ         0X20000000
#define MI_SYS_MAP_CPU_WRITE        0X10000000

#define MI_SYS_HANDLE_ADD_TO_LIST   0X08000000
#define MI_SYS_HANDLE_DEL_FROM_LIST 0X04000000
/* UserHandler ExFlags end. */


#define MI_SYS_INVLAID_SEQUENCE_NUM ((MI_U32)-1)

#define MI_SYS_SUCCESS MI_SUCCESS// do not use MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, MI_SUCCESS)   !!!!

/* SYS Module ErrorCode */
#define MI_ERR_SYS_INVALID_DEVID MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID)
#define MI_ERR_SYS_INVALID_CHNID MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID)
#define MI_ERR_SYS_ILLEGAL_PARAM MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM)
#define MI_ERR_SYS_EXIST MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_EXIST)
#define MI_ERR_SYS_UNEXIST MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_UNEXIST)
#define MI_ERR_SYS_NULL_PTR MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR)
#define MI_ERR_SYS_NOT_CONFIG MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG)
#define MI_ERR_SYS_NOT_SUPPORT MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT)
#define MI_ERR_SYS_NOT_PERM MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM)
#define MI_ERR_SYS_NOMEM MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM)
#define MI_ERR_SYS_NOBUF MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOBUF)
#define MI_ERR_SYS_BUF_EMPTY MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY)
#define MI_ERR_SYS_BUF_FULL MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL)
#define MI_ERR_SYS_SYS_NOTREADY MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY)
#define MI_ERR_SYS_BADADDR MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BADADDR)
#define MI_ERR_SYS_BUSY MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY)
#define MI_ERR_SYS_CHN_NOT_STARTED MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STARTED)
#define MI_ERR_SYS_CHN_NOT_STOPED MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STOPED)
#define MI_ERR_SYS_NOT_INIT MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_INIT)
#define MI_ERR_SYS_INITED MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INITED)
#define MI_ERR_SYS_NOT_ENABLE MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_ENABLE)
#define MI_ERR_SYS_NOT_DISABLE MI_DEF_ERR(E_MI_MODULE_ID_SYS,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_DISABLE)
#define MI_ERR_SYS_TIMEOUT MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_TIMEOUT)
#define MI_ERR_SYS_DEV_NOT_STARTED MI_DEF_ERR(E_MI_MODULE_ID_SYS,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_DEV_NOT_STARTED)
#define MI_ERR_SYS_DEV_NOT_STOPED MI_DEF_ERR(E_MI_MODULE_ID_SYS,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_DEV_NOT_STOPED)
#define MI_ERR_SYS_CHN_NO_CONTENT MI_DEF_ERR(E_MI_MODULE_ID_SYS,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NO_CONTENT)
#define MI_ERR_SYS_NOVASAPCE MI_DEF_ERR(E_MI_MODULE_ID_SYS,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOVASPACE)
#define MI_ERR_SYS_NOITEM MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOITEM)
#define MI_ERR_SYS_FAILED MI_DEF_ERR(E_MI_MODULE_ID_SYS, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED)

typedef MI_S32 MI_VB_POOL_HANDLE;
typedef MI_S32 MI_VB_BLK_HANDLE;
typedef MI_S32 MI_SYS_BUF_HANDLE;

/*
****************************************************TODO Refine*****************************************
*/

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                                     1
/// definition for FALSE
#define FALSE                                    0
#endif

typedef  struct MI_VB_BufBlkInfo_s
{
    MI_VB_POOL_HANDLE poolHandle;
    MI_U32 u32OffsetInVBPool;
    MI_U32 u32BlkSize;
    MI_PHY phySicalAddr;
    void  *pVirtualAddress;
} MI_VB_BufBlkInfo_t;

typedef struct MI_VB_PoolConf_s
{
    MI_U32 u32BlkSize;
    MI_U32 u32BlkCnt;
    MI_U8  u8MMAHeapName[MI_MAX_MMA_HEAP_LENGTH];
}MI_VB_PoolConf_t;

typedef struct MI_VB_PoolListConf_s
{
    MI_U32 u32PoolListCnt;
    MI_VB_PoolConf_t stPoolConf[MI_VB_POOL_LIST_MAX_CNT];
} MI_VB_PoolListConf_t;

typedef enum
{
    E_MI_SYS_DATA_PRECISION_8BPP,
    E_MI_SYS_DATA_PRECISION_10BPP,
    E_MI_SYS_DATA_PRECISION_12BPP,
    E_MI_SYS_DATA_PRECISION_14BPP,
	E_MI_SYS_DATA_PRECISION_16BPP,
	E_MI_SYS_DATA_PRECISION_MAX,
} MI_SYS_DataPrecision_e;

typedef enum
{
    E_MI_SYS_PIXEL_BAYERID_RG,
    E_MI_SYS_PIXEL_BAYERID_GR,
    E_MI_SYS_PIXEL_BAYERID_BG,
    E_MI_SYS_PIXEL_BAYERID_GB,
    E_MI_SYS_PIXEL_RGBIR_R0,
    E_MI_SYS_PIXEL_RGBIR_G0,
    E_MI_SYS_PIXEL_RGBIR_B0,
    E_MI_SYS_PIXEL_RGBIR_G1,
    E_MI_SYS_PIXEL_RGBIR_G2,
    E_MI_SYS_PIXEL_RGBIR_I0,
    E_MI_SYS_PIXEL_RGBIR_G3,
    E_MI_SYS_PIXEL_RGBIR_I1,
    E_MI_SYS_PIXEL_BAYERID_MAX,
}MI_SYS_BayerId_e;

typedef enum
{
    E_MI_SYS_PIXEL_FRAME_YUV422_YUYV = 0,
    E_MI_SYS_PIXEL_FRAME_ARGB8888,
    E_MI_SYS_PIXEL_FRAME_ABGR8888,
    E_MI_SYS_PIXEL_FRAME_BGRA8888,

    E_MI_SYS_PIXEL_FRAME_RGB565,
    E_MI_SYS_PIXEL_FRAME_ARGB1555,
    E_MI_SYS_PIXEL_FRAME_ARGB4444,
    E_MI_SYS_PIXEL_FRAME_I2,
    E_MI_SYS_PIXEL_FRAME_I4,
    E_MI_SYS_PIXEL_FRAME_I8,

    E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_422,
    E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420,
    E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420_NV21,
    E_MI_SYS_PIXEL_FRAME_YUV_MST_420,
    E_MI_SYS_PIXEL_FRAME_YUV422_UYVY,
    E_MI_SYS_PIXEL_FRAME_YUV422_YVYU,
    E_MI_SYS_PIXEL_FRAME_YUV422_VYUY,

    //vdec sigmastar private video format
    E_MI_SYS_PIXEL_FRAME_YC420_MSTTILE1_H264,
    E_MI_SYS_PIXEL_FRAME_YC420_MSTTILE2_H265,
    E_MI_SYS_PIXEL_FRAME_YC420_MSTTILE3_H265,

    E_MI_SYS_PIXEL_FRAME_RGB_BAYER_BASE,
    E_MI_SYS_PIXEL_FRAME_RGB_BAYER_NUM = E_MI_SYS_PIXEL_FRAME_RGB_BAYER_BASE + E_MI_SYS_DATA_PRECISION_MAX*E_MI_SYS_PIXEL_BAYERID_MAX-1,

    E_MI_SYS_PIXEL_FRAME_RGB888,
    E_MI_SYS_PIXEL_FRAME_BGR888,

    E_MI_SYS_PIXEL_FRAME_FORMAT_MAX,
} MI_SYS_PixelFormat_e;

#define RGB_BAYER_PIXEL(BitMode, PixelID) (E_MI_SYS_PIXEL_FRAME_RGB_BAYER_BASE+ BitMode*E_MI_SYS_PIXEL_BAYERID_MAX+ PixelID)

typedef enum
{
    E_MI_SYS_COMPRESS_MODE_NONE,//no compress
    E_MI_SYS_COMPRESS_MODE_SEG,//compress unit is 256 bytes as a segment
    E_MI_SYS_COMPRESS_MODE_LINE,//compress unit is the whole line
    E_MI_SYS_COMPRESS_MODE_FRAME,//compress unit is the whole frame
    E_MI_SYS_COMPRESS_MODE_BUTT, //number
}MI_SYS_CompressMode_e;

typedef enum
{
    E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE = 0x0,  // progessive.
    E_MI_SYS_FRAME_SCAN_MODE_INTERLACE   = 0x1,  // interlace.
    E_MI_SYS_FRAME_SCAN_MODE_MAX,
} MI_SYS_FrameScanMode_e;

typedef enum
{
    E_MI_SYS_FRAME_TILE_MODE_NONE = 0,
    E_MI_SYS_FRAME_TILE_MODE_16x16,      // tile mode 16x16
    E_MI_SYS_FRAME_TILE_MODE_16x32,      // tile mode 16x32
    E_MI_SYS_FRAME_TILE_MODE_32x16,      // tile mode 32x16
    E_MI_SYS_FRAME_TILE_MODE_32x32,      // tile mode 32x32
    E_MI_SYS_FRAME_TILE_MODE_MAX
} MI_SYS_FrameTileMode_e;

typedef enum
{
    E_MI_SYS_FIELDTYPE_NONE,        //< no field.
    E_MI_SYS_FIELDTYPE_TOP,           //< Top field only.
    E_MI_SYS_FIELDTYPE_BOTTOM,    //< Bottom field only.
    E_MI_SYS_FIELDTYPE_BOTH,        //< Both fields.
    E_MI_SYS_FIELDTYPE_NUM
} MI_SYS_FieldType_e;

typedef enum
{
    E_MI_SYS_BUFDATA_RAW = 0,
    E_MI_SYS_BUFDATA_FRAME,
    E_MI_SYS_BUFDATA_META,
} MI_SYS_BufDataType_e;

typedef enum
{
    E_MI_SYS_ROTATE_NONE, //Rotate 0 degrees
    E_MI_SYS_ROTATE_90, //Rotate 90 degrees
    E_MI_SYS_ROTATE_180, //Rotate 180 degrees
    E_MI_SYS_ROTATE_270, //Rotate 270 degrees
    E_MI_SYS_ROTATE_NUM,
}MI_SYS_Rotate_e;

typedef enum
{
   E_MI_SYS_BIND_TYPE_FRAME_BASE = 0x00000001,
   E_MI_SYS_BIND_TYPE_SW_LOW_LATENCY = 0x00000002,
   E_MI_SYS_BIND_TYPE_REALTIME = 0x00000004,
   E_MI_SYS_BIND_TYPE_HW_AUTOSYNC = 0x00000008,
   E_MI_SYS_BIND_TYPE_HW_RING = 0x00000010
}MI_SYS_BindType_e;

typedef enum
{
   E_MI_SYS_VPE_TO_VENC_PRIVATE_RING_POOL = 0,
   E_MI_SYS_PER_CHN_PRIVATE_POOL=1,
   E_MI_SYS_PER_DEV_PRIVATE_POOL=2,
   E_MI_SYS_PER_CHN_PORT_OUTPUT_POOL=3,
}MI_SYS_InsidePrivatePoolType_e;

typedef enum
{
    E_MI_SYS_FRAME_ISP_INFO_TYPE_NONE,
    E_MI_SYS_FRAME_ISP_INFO_TYPE_GLOBAL_GRADIENT
}MI_SYS_FrameIspInfoType_e;


typedef struct MI_SYS_ChnPort_s
{
    MI_ModuleId_e eModId;
    MI_U32  u32DevId;
    MI_U32 u32ChnId;
    MI_U32 u32PortId;

} MI_SYS_ChnPort_t;

typedef struct MI_SYS_WindowRect_s
{
    MI_U16 u16X;
    MI_U16 u16Y;
    MI_U16 u16Width;
    MI_U16 u16Height;
}MI_SYS_WindowRect_t;

typedef struct MI_SYS_WindowSize_s
{
    MI_U16 u16Width;
    MI_U16 u16Height;
}MI_SYS_WindowSize_t;

typedef struct MI_SYS_RawData_s
{
    void*   pVirAddr;
    MI_PHY  phyAddr;//notice that this is miu bus addr,not cpu bus addr.
    MI_U32  u32BufSize;

    MI_U32  u32ContentSize;
    MI_BOOL bEndOfFrame;
    MI_U64  u64SeqNum;
} MI_SYS_RawData_t;

typedef struct MI_SYS_MetaData_s
{
    void*  pVirAddr;
    MI_PHY phyAddr;//notice that this is miu bus addr,not cpu bus addr.

    MI_U32 u32Size;
    MI_U32 u32ExtraData;    /*driver special flag*/
    MI_ModuleId_e eDataFromModule;
} MI_SYS_MetaData_t;

typedef enum
{
  REALTIME_FRAME_DATA,
  RINGBUF_FRAME_DATA,
  NORMAL_FRAME_DATA,
}MI_SYS_FrameData_PhySignalType;

#define MI_SYS_REALTIME_MAGIC_PADDR ((MI_PHY)0x46414B45) //"FAKE"
#define MI_SYS_REALTIME_MAGIC_VADDR ((void*)0x46414B45) //"FAKE"
#define MI_SYS_REALTIME_MAGIC_PITCH ((MI_U32)0x46414B45) //"FAKE"

typedef struct MI_SYS_FrameIspInfo_s
{
    MI_SYS_FrameIspInfoType_e eType;
    union
    {
        MI_U32 u32GlobalGradient;
    }uIspInfo;
}MI_SYS_FrameIspInfo_t;

//N.B. in MI_SYS_FrameData_t should never support u32Size,
//for other values are enough,and not support u32Size is general standard method.
typedef  struct  MI_SYS_FrameData_s
{
    MI_SYS_FrameTileMode_e eTileMode;
    MI_SYS_PixelFormat_e ePixelFormat;
    MI_SYS_CompressMode_e eCompressMode;
    MI_SYS_FrameScanMode_e eFrameScanMode;
    MI_SYS_FieldType_e eFieldType;
    MI_SYS_FrameData_PhySignalType ePhylayoutType;

    MI_U16 u16Width;
    MI_U16 u16Height;
//in case ePhylayoutType equal to REALTIME_FRAME_DATA, pVirAddr would be MI_SYS_REALTIME_MAGIC_PADDR and phyAddr would be MI_SYS_REALTIME_MAGIC_VADDR

    void* pVirAddr[3];
    MI_PHY phyAddr[3];//notice that this is miu bus addr,not cpu bus addr.
    MI_U32 u32Stride[3];
    MI_U32 u32BufSize;//total size that allocated for this buffer,include consider alignment.

    MI_U16 u16RingBufStartLine;//Valid in case RINGBUF_FRAME_DATA,  u16RingBufStartLine must be LGE than 0 and less than u16Height
    MI_U16 u16RingBufRealTotalHeight;///Valid in case RINGBUF_FRAME_DATA,  u16RingBufStartLine must be LGE than u16Height

    MI_SYS_FrameIspInfo_t stFrameIspInfo;//isp info of each frame
    MI_SYS_WindowRect_t stContentCropWindow;
} MI_SYS_FrameData_t;

typedef  struct  MI_SYS_BufInfo_s
{
    MI_U64 u64Pts;
    MI_U64 u64SidebandMsg;
    MI_SYS_BufDataType_e eBufType;
    MI_BOOL bEndOfStream;
    MI_BOOL bUsrBuf;
    MI_U32 u32SequenceNumber;
    MI_BOOL bDrop;
    union
    {
        MI_SYS_FrameData_t stFrameData;
        MI_SYS_RawData_t stRawData;
        MI_SYS_MetaData_t stMetaData;
    };
} MI_SYS_BufInfo_t;

typedef struct MI_SYS_FrameBufExtraConfig_s
{
  //Buf alignment requirement in horizontal
  MI_U16 u16BufHAlignment;
  //Buf alignment requirement in vertical
  MI_U16 u16BufVAlignment;
  //Buf alignment requirement in chroma
  MI_U16 u16BufChromaAlignment;
  //Clear padding flag
  MI_BOOL bClearPadding;
}MI_SYS_FrameBufExtraConfig_t;

typedef struct MI_SYS_BufFrameConfig_s
{
    MI_U16 u16Width;
    MI_U16 u16Height;
    MI_SYS_FrameScanMode_e eFrameScanMode;//
    MI_SYS_PixelFormat_e eFormat;
    MI_SYS_FrameBufExtraConfig_t stFrameBufExtraConf;//set by MI_SYS internal
    //MI_U32 u32Size;//this value will be calculated through others values in this struct
}MI_SYS_BufFrameConfig_t;

typedef struct MI_SYS_BufRawConfig_s
{
    MI_U32 u32Size;
}MI_SYS_BufRawConfig_t;

typedef struct MI_SYS_MetaDataConfig_s
{
    MI_U32 u32Size;
}MI_SYS_MetaDataConfig_t;

typedef struct MI_SYS_BufConf_s
{
    MI_SYS_BufDataType_e eBufType;
    MI_U32 u32Flags;   //0 or MI_SYS_MAP_VA
    MI_U64 u64TargetPts;
    union
    {
        MI_SYS_BufFrameConfig_t stFrameCfg;
        MI_SYS_BufRawConfig_t stRawCfg;
        MI_SYS_MetaDataConfig_t stMetaCfg;
    };
}MI_SYS_BufConf_t;

typedef struct MI_SYS_Version_s
{
    MI_U8 u8Version[128];
}MI_SYS_Version_t;


typedef struct MI_PerChnPrivHeapConf_s
{
    MI_ModuleId_e eModule;
    MI_U32 u32Devid;
    MI_U32 u32Channel;
    MI_U8 u8MMAHeapName[MI_MAX_MMA_HEAP_LENGTH];
    MI_U32 u32PrivateHeapSize;
}MI_SYS_PerChnPrivHeapConf_t;

typedef struct MI_PerDevPrivHeapConf_s
{
    MI_ModuleId_e eModule;
    MI_U32 u32Devid;
    MI_U32 u32Reserve;
    MI_U8 u8MMAHeapName[MI_MAX_MMA_HEAP_LENGTH];
    MI_U32 u32PrivateHeapSize;
}MI_SYS_PerDevPrivHeapConf_t;

typedef struct MI_SYS_PerVpe2VencRingPoolConf_s
{
    MI_U32 u32VencInputRingPoolStaticSize;
    MI_U8 u8MMAHeapName[MI_MAX_MMA_HEAP_LENGTH];
}MI_SYS_PerVpe2VencRingPoolConf_t;

typedef struct MI_SYS_PerChnPortOutputPool_s
{
    MI_ModuleId_e eModule;
    MI_U32 u32Devid;
    MI_U32 u32Channel;
    MI_U32 u32Port;
    MI_U8 u8MMAHeapName[MI_MAX_MMA_HEAP_LENGTH];
    MI_U32 u32PrivateHeapSize;
}MI_SYS_PerChnPortOutputPool_t;

typedef struct MI_SYS_GlobalPrivPoolConfig_s
{
    MI_SYS_InsidePrivatePoolType_e eConfigType;
    MI_BOOL bCreate;
    union
    {
        MI_SYS_PerChnPrivHeapConf_t stPreChnPrivPoolConfig;
        MI_SYS_PerDevPrivHeapConf_t stPreDevPrivPoolConfig;
        MI_SYS_PerVpe2VencRingPoolConf_t stPreVpe2VencRingPrivPoolConfig;
        MI_SYS_PerChnPortOutputPool_t stPreChnPortOutputPrivPool;
    }uConfig;
}MI_SYS_GlobalPrivPoolConfig_t;

typedef struct MI_SYS_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_SYS_InitParam_t;

#endif ///_MI_SYS_DATATYPE_H_
