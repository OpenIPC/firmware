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
#ifndef _MI_VPE_DATATYPE_H_
#define _MI_VPE_DATATYPE_H_
#include "mi_sys_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MI_VPE_OK (0)
#define MI_ERR_VPE_INVALID_CHNID (0xA0078001) //The VPE channel ID is invalid.
#define MI_ERR_VPE_INVALID_PORTID (0xA0078002) //The VPE outport ID is invalid.
#define MI_ERR_VPE_ILLEGAL_PARAM (0xA0078003) //The VPE parameter is invalid.
#define MI_ERR_VPE_EXIST (0xA0078004) //A VPE channel is created.
#define MI_ERR_VPE_UNEXIST (0xA0078005) //No VPE channel is created.
#define MI_ERR_VPE_NULL_PTR (0xA0078006) //The pointer of the input parameter is null.
#define MI_ERR_VPE_NOT_SUPPORT (0xA0078008) //The operation is not supported.
#define MI_ERR_VPE_NOT_PERM (0xA0078009) //The operation is forbidden.
#define MI_ERR_VPE_NOMEM (0xA007800C) //The memory fails to be allocated.
#define MI_ERR_VPE_NOBUF (0xA007800D) //The buffer pool fails to be allocated.
#define MI_ERR_VPE_BUF_EMPTY (0xA007800E) //The picture queue is empty.
#define MI_ERR_VPE_NOTREADY (0xA0078010) //The VPE is not initialized.
#define MI_ERR_VPE_BUSY (0xA0078012) //The VPE is busy.


typedef MI_U32 MI_VPE_CHANNEL;
typedef MI_U32 MI_VPE_PORT;

typedef enum
{
    E_MI_VPE_RUN_INVALID              = 0x00,
    E_MI_VPE_RUN_DVR_MODE             = 0x01,
    E_MI_VPE_RUN_CAM_TOP_MODE         = 0x02,
    E_MI_VPE_RUN_CAM_BOTTOM_MODE      = 0x04,
    E_MI_VPE_RUN_CAM_MODE             = E_MI_VPE_RUN_CAM_TOP_MODE|E_MI_VPE_RUN_CAM_BOTTOM_MODE,
    E_MI_VPE_RUN_REALTIME_TOP_MODE    = 0x08,
    E_MI_VPE_RUN_REALTIME_BOTTOM_MODE = 0x10,
    E_MI_VPE_RUN_REALTIME_MODE        = E_MI_VPE_RUN_REALTIME_TOP_MODE | E_MI_VPE_RUN_REALTIME_BOTTOM_MODE,
    E_MI_VPE_RUNNING_MODE_MAX,
} MI_VPE_RunningMode_e;

typedef enum
{
    E_MI_VPE_HDR_TYPE_OFF,
    E_MI_VPE_HDR_TYPE_VC,                 //virtual channel mode HDR,vc0->long, vc1->short
    E_MI_VPE_HDR_TYPE_DOL,
    E_MI_VPE_HDR_TYPE_EMBEDDED,  //compressed HDR mode
    E_MI_VPE_HDR_TYPE_LI,                //Line interlace HDR
    E_MI_VPE_HDR_TYPE_MAX
} MI_VPE_HDRType_e;

typedef enum
{
    E_MI_VPE_SENSOR_INVALID = 0,
    E_MI_VPE_SENSOR0,
    E_MI_VPE_SENSOR1,
    E_MI_VPE_SENSOR2,
    E_MI_VPE_SENSOR3,
    E_MI_VPE_SENSOR_MAX
}MI_VPE_SensorChannel_e;

typedef enum
{
    E_MI_VPE_3DNR_LEVEL_OFF,
    E_MI_VPE_3DNR_LEVEL1,
    E_MI_VPE_3DNR_LEVEL2,
    E_MI_VPE_3DNR_LEVEL3,
    E_MI_VPE_3DNR_LEVEL4,
    E_MI_VPE_3DNR_LEVEL5,
    E_MI_VPE_3DNR_LEVEL6,
    E_MI_VPE_3DNR_LEVEL7,
    E_MI_VPE_3DNR_LEVEL_NUM
}MI_VPE_3DNR_Level_e;

typedef enum
{
    E_MI_VPE_ZOOM_LDC_NULL,
    E_MI_VPE_ZOOM_LDC_PORT0 = 0X01,
    E_MI_VPE_ZOOM_LDC_PORT1 = 0X02,
    E_MI_VPE_ZOOM_LDC_PORT2 = 0X04,
    E_MI_VPE_ZOOM_LDC_MAX = E_MI_VPE_ZOOM_LDC_PORT0|
                            E_MI_VPE_ZOOM_LDC_PORT1|E_MI_VPE_ZOOM_LDC_PORT2,
}MI_VPE_ChnPortMode_e;

typedef struct MI_VPE_IspApiHeader_s
{
    MI_U32 u32HeadSize;    //Size of MIIspApiHeader_t
    MI_U32 u32DataLen;     //Data length;
    MI_U32 u32CtrlID;      //Function ID
    MI_U32 u32Channel;     //Isp channel number
    MI_S32 s32Ret;         //Isp api retuen value
} MI_VPE_IspApiHeader_t;

typedef struct MI_VPE_IspInitPara_s
{
    MI_U32 u32Revision;
    MI_U32 u32Size;
    MI_U8  u8Data[64];
}MI_VPE_IspInitPara_t;

typedef struct MI_VPE_ChannelAttr_s
{
    MI_U16 u16MaxW;
    MI_U16 u16MaxH;
    MI_SYS_PixelFormat_e ePixFmt;
    MI_VPE_HDRType_e eHDRType;
    MI_VPE_SensorChannel_e eSensorBindId;

    MI_BOOL bNrEn;
    MI_BOOL bEdgeEn;
    MI_BOOL bEsEn;
    MI_BOOL bContrastEn;
    MI_BOOL bUvInvert;
    MI_BOOL bRotation;
    MI_VPE_RunningMode_e eRunningMode;
    MI_VPE_IspInitPara_t tIspInitPara;
    MI_BOOL  bEnLdc; // true port3 for ldc or for Isp(skip pass2)
    MI_U32   u32ChnPortMode;
}MI_VPE_ChannelAttr_t;

typedef struct MI_VPE_PqParam_s
{
    MI_U8 u8NrcSfStr; //0 ~ 255;
    MI_U8 u8NrcTfStr; //0 ~ 255
    MI_U8 u8NrySfStr; //0 ~ 255
    MI_U8 u8NryTfStr; //0 ~ 255
    MI_U8 u8NryBlendMotionTh; //0 ~ 15
    MI_U8 u8NryBlendStillTh; //0 ~ 15
    MI_U8 u8NryBlendMotionWei; //0 ~ 31
    MI_U8 u8NryBlendOtherWei; //0 ~ 31
    MI_U8 u8NryBlendStillWei; //0 ~ 31
    MI_U8 u8EdgeGain[6];//0~255
    MI_U8 u8Contrast;//0~255
} MI_VPE_PqParam_t;

typedef struct MI_VPE_ChannelPara_s
{
   MI_VPE_PqParam_t      stPqParam; // only dvr use
   MI_VPE_HDRType_e      eHDRType;
   MI_VPE_3DNR_Level_e   e3DNRLevel;
   MI_BOOL               bMirror;
   MI_BOOL               bFlip;
   MI_BOOL               bWdrEn;    //Wdr on/off not use
   MI_BOOL               bEnLdc;
} MI_VPE_ChannelPara_t;

typedef struct MI_VPE_RegionInfo_s
{
    MI_SYS_WindowRect_t *pstWinRect;            // region attribute
    MI_U32 u32RegionNum;                    // count of the region
} MI_VPE_RegionInfo_t;

/*Define attributes of Vpe port's work mode*/
typedef struct MI_VPE_PortMode_s
{
    MI_U16 u16Width;                         // Width of target image
    MI_U16 u16Height;                        // Height of target image
    MI_BOOL bMirror;
    MI_BOOL bFlip;
    MI_SYS_PixelFormat_e  ePixelFormat;            // Pixel format of target image
    MI_SYS_CompressMode_e eCompressMode;   // Compression mode of the output
}MI_VPE_PortMode_t;

typedef struct MI_VPE_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_VPE_InitParam_t;

typedef MI_S32 (*MI_VPE_CALLBK_FUNC)(MI_U64 u64Data);

typedef enum
{
    E_MI_VPE_CALLBACK_ISR,
    E_MI_VPE_CALLBACK_MAX,
} MI_VPE_CallBackMode_e;

typedef enum
{
    E_MI_VPE_IRQ_ISPVSYNC,
    E_MI_VPE_IRQ_ISPFRAMEDONE,
    E_MI_VPE_IRQ_MAX,
} MI_VPE_IrqType_e;

typedef struct MI_VPE_CallBackParam_s
{
    MI_VPE_CallBackMode_e eCallBackMode;
    MI_VPE_IrqType_e  eIrqType;
    MI_VPE_CALLBK_FUNC  pfnCallBackFunc;
    MI_U64    u64Data;
} MI_VPE_CallBackParam_t;


#ifdef __cplusplus
}
#endif
#endif///_MI_VPE_DATATYPE_H_
