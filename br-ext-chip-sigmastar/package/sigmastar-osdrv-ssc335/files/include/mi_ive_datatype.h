/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef _MI_IVE_DATATYPE_H_
#define _MI_IVE_DATATYPE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include "mi_common_datatype.h"

#define MI_IVE_HANDLE_MAX                   (128)
#define MI_IVE_HIST_NUM                     (256)
#define MI_IVE_MAP_NUM                      (256)
#define MI_IVE_MAX_REGION_NUM               (255)
#define MI_IVE_ST_MAX_CORNER_NUM            (200)
#define MI_IVE_MASK_SIZE_5X5                (25)
#define MI_IVE_CANNY_STACK_RESERVED_SIZE    (12)

/************************************************IVE error code ***********************************/
#define MI_IVE_ERR_INVALID_DEVID    MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID)
#define MI_IVE_ERR_INVALID_CHNID    MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID)
#define MI_IVE_ERR_ILLEGAL_PARAM    MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM)
#define MI_IVE_ERR_EXIST            MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_EXIST)
#define MI_IVE_ERR_UNEXIST          MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_UNEXIST)
#define MI_IVE_ERR_NULL_PTR         MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR)
#define MI_IVE_ERR_NOT_CONFIG       MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG)
#define MI_IVE_ERR_NOT_SUPPORT      MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT)
#define MI_IVE_ERR_NOT_PERM         MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM)
#define MI_IVE_ERR_NOMEM            MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM)
#define MI_IVE_ERR_NOBUF            MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOBUF)
#define MI_IVE_ERR_BUF_EMPTY        MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY)
#define MI_IVE_ERR_BUF_FULL         MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL)
#define MI_IVE_ERR_SYS_NOTREADY     MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY)
#define MI_IVE_ERR_BADADDR          MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BADADDR)
#define MI_IVE_ERR_BUSY             MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY)
#define MI_IVE_ERR_CHN_NOT_STARTED  MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STARTED)
#define MI_IVE_ERR_CHN_NOT_STOPED   MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STOPED)
#define MI_IVE_ERR_NOT_INIT         MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_INIT)
#define MI_IVE_ERR_SYS_TIMEOUT      MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_TIMEOUT)
#define MI_IVE_ERR_FAILED           MI_DEF_ERR(E_MI_MODULE_ID_IVE, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED)

/*-----------------------------------------------*
 * The fixed-point data type, will be used to    *
 * represent float data in hardware calculations.*
 *-----------------------------------------------*/

/*--u8bit----------------------------------------*/
typedef unsigned char           MI_U0Q8;
typedef unsigned char           MI_U1Q7;
typedef unsigned char           MI_U5Q3;

/*--u16bit---------------------------------------*/
typedef unsigned short          MI_U0Q16;
typedef unsigned short          MI_U4Q12;
typedef unsigned short          MI_U6Q10;
typedef unsigned short          MI_U8Q8;
typedef unsigned short          MI_U12Q4;
typedef unsigned short          MI_U14Q2;

/*--s16bit---------------------------------------*/
typedef short                   MI_S9Q7;
typedef short                   MI_S14Q2;
typedef short                   MI_S1Q15;

/*--u32bit---------------------------------------*/
typedef unsigned int            MI_U22Q10;
typedef unsigned int            MI_U25Q7;

/*--s32bit---------------------------------------*/
typedef int                     MI_S25Q7;
typedef int                     MI_S16Q16;

/*
* Type of the MI_IVE_Image_t data.
* Aded by tanbing 2013-7-22
*/
typedef enum
{
    E_MI_IVE_IMAGE_TYPE_U8C1           =  0x0,
    E_MI_IVE_IMAGE_TYPE_S8C1           =  0x1,
    E_MI_IVE_IMAGE_TYPE_YUV420SP       =  0x2,      /*YUV420 SemiPlanar*/
    E_MI_IVE_IMAGE_TYPE_YUV422SP       =  0x3,      /*YUV422 SemiPlanar*/
    E_MI_IVE_IMAGE_TYPE_YUV420P        =  0x4,      /*YUV420 Planar */
    E_MI_IVE_IMAGE_TYPE_YUV422P        =  0x5,       /*YUV422 planar */
    E_MI_IVE_IMAGE_TYPE_S8C2_PACKAGE   =  0x6,
    E_MI_IVE_IMAGE_TYPE_S8C2_PLANAR    =  0x7,
    E_MI_IVE_IMAGE_TYPE_S16C1          =  0x8,
    E_MI_IVE_IMAGE_TYPE_U16C1          =  0x9,
    E_MI_IVE_IMAGE_TYPE_U8C3_PACKAGE   =  0xA,
    E_MI_IVE_IMAGE_TYPE_U8C3_PLANAR    =  0xB,
    E_MI_IVE_IMAGE_TYPE_S32C1          =  0xC,
    E_MI_IVE_IMAGE_TYPE_U32C1          =  0xD,
    E_MI_IVE_IMAGE_TYPE_S64C1          =  0xE,
    E_MI_IVE_IMAGE_TYPE_U64C1          =  0xF,

    E_MI_IVE_IMAGE_TYPE_MAX

}MI_IVE_ImageType_e;

/*
* Definition of the MI_IVE_Image_t.
* Added by Tan Bing, 2013-7-22.
*/
typedef struct MI_IVE_Image_s
{
    MI_IVE_ImageType_e  eType;

    MI_PHY  aphyPhyAddr[3];
    MI_U8  *apu8VirAddr[3];

    MI_U16  azu16Stride[3];
    MI_U16  u16Width;
    MI_U16  u16Height;

    MI_U16  u16Reserved;        /*Can be used such as elemSize*/
}MI_IVE_Image_t;

typedef MI_IVE_Image_t MI_IVE_SrcImage_t;
typedef MI_IVE_Image_t MI_IVE_DstImage_t;

/*
* Definition of the MI_IVE_MemInfo_t.This struct special purpose for input or ouput, such as Hist, CCL, ShiTomasi.
* Added by Chen Quanfu, 2013-7-23.
*/
typedef struct MI_IVE_MemInfo_s
{
    MI_PHY  phyPhyAddr;
    MI_U8  *pu8VirAddr;
    MI_U32  u32Size;
}MI_IVE_MemInfo_t;
typedef MI_IVE_MemInfo_t MI_IVE_SrcMemInfo_t;
typedef MI_IVE_MemInfo_t MI_IVE_DstMemInfo_t;

/*
*Data struct ,created by Chen Quanfu 2013-07-19
*/
typedef struct MI_IVE_Data_s
{
    MI_PHY  phyPhyAddr;  /*Physical address of the data*/
    MI_U8  *pu8VirAddr;

    MI_U16  u16Stride;   /*2D data stride by byte*/
    MI_U16  u16Width;    /*2D data width by byte*/
    MI_U16  u16Height;   /*2D data height*/

    MI_U16 u16Reserved;
}MI_IVE_Data_s;
typedef MI_IVE_Data_s MI_IVE_SrcData_t;
typedef MI_IVE_Data_s MI_IVE_DstData_t;

/*
* Definition of the union of MI_IVE_Length8bit_u.
* Added by Tan Bing, 2013-7-22.
*/
typedef union
{
    MI_S8 s8Val;
    MI_U8 u8Val;
}MI_IVE_Length8bit_u;

/*
* Definition of u16 point
*/
typedef struct MI_IVE_PointU16_s
{
    MI_U16 u16X;
    MI_U16 u16Y;
}MI_IVE_PointU16_t;

/*
*Float point represented by Fixed-point SQ25.7
*/
typedef struct MI_IVE_PointS25Q7_s
{
    MI_S25Q7     s25q7X;                /*X coordinate*/
    MI_S25Q7     s25q7Y;                /*Y coordinate*/
}MI_IVE_PointS25Q7_t;

/*
* Definition of rect
*/
typedef struct MI_IVE_Rect_s
{
    MI_U16 u16X;
    MI_U16 u16Y;
    MI_U16 u16Width;
    MI_U16 u16Height;
}MI_IVE_Rect_t;


/*
*Filter control parameters
*/
typedef struct MI_IVE_FilterCtrl_s
{
    MI_S8 as8Mask[MI_IVE_MASK_SIZE_5X5];        /*Template parameter filter coefficient*/
    MI_U8 u8Norm;             /*Normalization parameter, by right shift*/
}MI_IVE_FilterCtrl_t;

/*
* CSC working mode.
*/
typedef enum
{
    E_MI_IVE_CSC_MODE_PIC_BT601_YUV2RGB   =  0x0,   /*CSC: YUV2RGB, picture transfer mode, RGB value range [0, 255]*/
    E_MI_IVE_CSC_MODE_PIC_BT601_YUV2BGR   =  0x1,   /*CSC: YUV2BGR, picture transfer mode, RGB value range [0, 255]*/
    E_MI_IVE_CSC_MODE_PIC_BT601_RGB2YUV   =  0x2,   /*CSC: RGB2YUV, picture transfer mode, YUV:value range [0, 255]*/
    E_MI_IVE_CSC_MODE_PIC_BT601_BGR2YUV   =  0x3,   /*CSC: BGR2YUV, picture transfer mode, YUV:value range [0, 255]*/
    E_MI_IVE_CSC_MODE_MAX
}MI_IVE_CscMode_e;

/*
*CSC control parameters
*/
typedef struct MI_IVE_CscCtrl_s
{
    MI_IVE_CscMode_e    eMode; /*Working mode*/
}MI_IVE_CscCtrl_t;

/*
*Filter+CSC control parameters
*/
typedef struct MI_IVE_FilterAndCscCtrl_s
{
    MI_IVE_CscMode_e    eMode;                         /*CSC working mode*/
    MI_S8               as8Mask[MI_IVE_MASK_SIZE_5X5];  /*Template parameter filter coefficient*/
    MI_U8               u8Norm;                         /*Normalization parameter, by right shift*/
}MI_IVE_FilterAndCscCtrl_t;

/*
*SOBEL output ctrl
*/
typedef enum
{
    E_MI_IVE_SOBEL_OUT_CTRL_BOTH =  0x0,    /*Output horizontal and vertical*/
    E_MI_IVE_SOBEL_OUT_CTRL_HOR  =  0x1,    /*Output horizontal*/
    E_MI_IVE_SOBEL_OUT_CTRL_VER  =  0x2,    /*Output vertical*/
    E_MI_IVE_SOBEL_OUT_CTRL_MAX
}MI_IVE_SobelOutCtrl_e;

/*
*SOBEL control parameter
*/
typedef struct MI_IVE_SobelCtrl_s
{
    MI_IVE_SobelOutCtrl_e eOutCtrl;         /*Output format*/
    MI_S8 as8Mask[MI_IVE_MASK_SIZE_5X5];    /*Template parameter*/
}MI_IVE_SobelCtrl_t;

/*
*Type of the magnitude and angle output results
*/
typedef enum
{
    E_MI_IVE_MAG_AND_ANG_OUT_CTRL_MAG          =  0x0,      /*Only the magnitude is output.*/
    E_MI_IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG  =  0x1,      /*The magnitude and angle are output.*/
    E_MI_IVE_MAG_AND_ANG_OUT_CTRL_MAX
}MI_IVE_MagAndAngOutCtrl_e;

/*
*Magnitude and angle control parameter
*/
typedef struct MI_IVE_MagAndAngCtrl_s
{
    MI_IVE_MagAndAngOutCtrl_e eOutCtrl;
    MI_U16 u16Thr;
    MI_S8  as8Mask[MI_IVE_MASK_SIZE_5X5];   /*Template parameter.*/
}MI_IVE_MagAndAngCtrl_t;

/*
*Dilate control parameters
*/
typedef struct MI_IVE_DilateCtrl_s
{
    MI_U8 au8Mask[MI_IVE_MASK_SIZE_5X5];    /*The template parameter value must be 0 or 255.*/
}MI_IVE_DilateCtrl_t;

/*
*Erode control parameter
*/
typedef struct MI_IVE_ErodeCtrl_s
{
    MI_U8 au8Mask[MI_IVE_MASK_SIZE_5X5];    /*The template parameter value must be 0 or 255.*/
}MI_IVE_ErodeCtrl_t;

/*
* Type of the Thresh mode.
*/
typedef enum
{
    E_MI_IVE_THRESH_MODE_BINARY       =  0x0,  /*srcVal <= lowThr, dstVal = minVal; srcVal > lowThr, dstVal = maxVal.*/
    E_MI_IVE_THRESH_MODE_TRUNC        =  0x1,  /*srcVal <= lowThr, dstVal = srcVal; srcVal > lowThr, dstVal = maxVal.*/
    E_MI_IVE_THRESH_MODE_TO_MINVAL    =  0x2,  /*srcVal <= lowThr, dstVal = minVal; srcVal > lowThr, dstVal = srcVal.*/

    E_MI_IVE_THRESH_MODE_MIN_MID_MAX  =  0x3,  /*srcVal <= lowThr, dstVal = minVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = maxVal.*/
    E_MI_IVE_THRESH_MODE_ORI_MID_MAX  =  0x4,  /*srcVal <= lowThr, dstVal = srcVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = maxVal.*/
    E_MI_IVE_THRESH_MODE_MIN_MID_ORI  =  0x5,  /*srcVal <= lowThr, dstVal = minVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = srcVal.*/
    E_MI_IVE_THRESH_MODE_MIN_ORI_MAX  =  0x6,  /*srcVal <= lowThr, dstVal = minVal;  lowThr < srcVal <= highThr, dstVal = srcVal; srcVal > highThr, dstVal = maxVal.*/
    E_MI_IVE_THRESH_MODE_ORI_MID_ORI  =  0x7,  /*srcVal <= lowThr, dstVal = srcVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = srcVal.*/

    E_MI_IVE_THRESH_MODE_MAX
}MI_IVE_ThreshMode_e;

/*
* Thresh control parameters.
*/
typedef struct MI_IVE_ThreshCtrl_s
{
    MI_IVE_ThreshMode_e eMode;
    MI_U8 u8LowThr;             /*user-defined threshold,  0<=u8LowThr<=255 */
    MI_U8 u8HighThr;            /*user-defined threshold, if eMode<MI_IVE_THRESH_MODE_MIN_MID_MAX, u8HighThr is not used, else 0<=u8LowThr<=u8HighThr<=255;*/
    MI_U8 u8MinVal;             /*Minimum value when tri-level thresholding*/
    MI_U8 u8MidVal;             /*Middle value when tri-level thresholding, if eMode<2, u32MidVal is not used; */
    MI_U8 u8MaxVal;             /*Maxmum value when tri-level thresholding*/
}MI_IVE_ThreshCtrl_t;

/*
*Type of the Sub output results
*/
typedef enum
{
    E_MI_IVE_SUB_MODE_ABS    =  0x0,     /*Absolute value of the difference*/
    E_MI_IVE_SUB_MODE_SMIFT  =  0x1,    /*The output result is obtained by shifting the result one digit right to reserve the signed bit.*/
    E_MI_IVE_SUB_MODE_MAX
}MI_IVE_SubMode_e;

/*
*Sub control parameters
*/
typedef struct MI_IVE_SubCtrl_s
{
    MI_IVE_SubMode_e eMode;
}MI_IVE_SubCtrl_t;

/*
*Type of the Integ output results, created by Tan Bing, 2013-03-18
* Modify by Chen Quanfu 2013-07-15
* Spec: add MI_IVE_INTEG_OUT_CTRL_SQSUM output format
*/
typedef enum
{
    E_MI_IVE_INTEG_OUT_CTRL_COMBINE  =  0x0,
    E_MI_IVE_INTEG_OUT_CTRL_SUM      =  0x1,
    E_MI_IVE_INTEG_OUT_CTRL_SQSUM    =  0x2,
    E_MI_IVE_INTEG_OUT_CTRL_MAX
}MI_IVE_IntegOutCtrl_e;

/*
* Integ control parameters,Create by Chen Quanfu 2013-07-15
*/
typedef struct MI_IVE_IntegCtrl_s
{
    MI_IVE_IntegOutCtrl_e eOutCtrl;
}MI_IVE_IntegCtrl_t;

/*
*Type of the Thresh_S16 mode
*/
typedef enum
{
    E_MI_IVE_THRESH_S16_MODE_S16_TO_S8_MIN_MID_MAX  =  0x0,
    E_MI_IVE_THRESH_S16_MODE_S16_TO_S8_MIN_ORI_MAX  =  0x1,
    E_MI_IVE_THRESH_S16_MODE_S16_TO_U8_MIN_MID_MAX  =  0x2,
    E_MI_IVE_THRESH_S16_MODE_S16_TO_U8_MIN_ORI_MAX  =  0x3,

    E_MI_IVE_THRESH_S16_MODE_MAX
}MI_IVE_ThreshS16Mode_e;

/*
*Thresh_S16 control parameters
*/
typedef struct MI_IVE_ThreshS16Ctrl_s
{
    MI_IVE_ThreshS16Mode_e eMode;
    MI_S16 s16LowThr;               /*User-defined threshold*/
    MI_S16 s16HighThr;              /*User-defined threshold*/
    MI_IVE_Length8bit_u un8MinVal;  /*Minimum value when tri-level thresholding*/
    MI_IVE_Length8bit_u un8MidVal;  /*Middle value when tri-level thresholding*/
    MI_IVE_Length8bit_u un8MaxVal;  /*Maxmum value when tri-level thresholding*/
}MI_IVE_ThreshS16Ctrl_t;

/*
*Type of the Thresh_U16 mode
*/
typedef enum
{
    E_MI_IVE_THRESH_U16_MODE_U16_TO_U8_MIN_MID_MAX  =  0x0,
    E_MI_IVE_THRESH_U16_MODE_U16_TO_U8_MIN_ORI_MAX  =  0x1,

    E_MI_IVE_THRESH_U16_MODE_MAX
}MI_IVE_ThreshU16Mode_e;

/*
*Thresh_U16 control parameters
*/
typedef struct MI_IVE_ThreshU16Ctrl_s
{
    MI_IVE_ThreshU16Mode_e eMode;
    MI_U16 u16LowThr;
    MI_U16 u16HighThr;
    MI_U8  u8MinVal;
    MI_U8  u8MidVal;
    MI_U8  u8MaxVal;
}MI_IVE_ThreshU16Ctrl_t;

/*
*Mode of 16BitTo8Bit
*/
typedef enum
{
    E_MI_IVE_16BIT_TO_8BIT_MODE_S16_TO_S8       =  0x0,
    E_MI_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS   =  0x1,
    E_MI_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS  =  0x2,
    E_MI_IVE_16BIT_TO_8BIT_MODE_U16_TO_U8       =  0x3,

    E_MI_IVE_16BIT_TO_8BIT_MODE_MAX
}MI_IVE_16BitTo8BitMode_e;

/*
*16BitTo8Bit control parameters
*/
typedef struct MI_IVE_16bitTo8BitCtrl_s
{
    MI_IVE_16BitTo8BitMode_e eMode;
    MI_U16 u16Denominator;
    MI_U8  u8Numerator;
    MI_S8  s8Bias;
}MI_IVE_16bitTo8BitCtrl_t;

/*
*Type of the OrdStaFilter
*/
typedef enum
{
    E_MI_IVE_ORD_STAT_FILTER_MODE_MEDIAN  =  0x0,
    E_MI_IVE_ORD_STAT_FILTER_MODE_MAX     =  0x1,
    E_MI_IVE_ORD_STAT_FILTER_MODE_MIN     =  0x2,

    E_MI_IVE_ORD_STAT_FILTER_MODE_ENUM_MAX
}MI_IVE_OrdStatFilterMode_e;

/*
*OrdStaFilter control parameters
*/
typedef struct MI_IVE_OrdStatFilter_s
{
    MI_IVE_OrdStatFilterMode_e eMode;

}MI_IVE_OrdStatFilter_t;

/*
* Map LUT memory struct
*/
typedef struct MI_IVE_MapLutMem_s
{
    MI_U8  au8Map[MI_IVE_MAP_NUM];
}MI_IVE_MapLutMem_t;

/*
* Equalizehist control member struct
*/
typedef struct MI_IVE_EqualizeHistCtrlMem_s
{
    MI_U32 au32Hist[MI_IVE_HIST_NUM];
    MI_U8  au8Map[MI_IVE_MAP_NUM];
}MI_IVE_EqualizeHistCtrlMem_t;

/*
*Equalizehist control parameters,created by Chen Quanfu 2013-07-17
*/
typedef struct MI_IVE_EqualizeHistCtrl_s
{
    MI_IVE_MemInfo_t stMem;
}MI_IVE_EqualizeHistCtrl_t;

/*
*Mode of Add
*/
typedef enum
{
    E_MI_IVE_ADD_MODE_ROUNDING       =  0x0,
    E_MI_IVE_ADD_MOD_CLIPPING        =  0x1,

    E_MI_IVE_ADD_MODE_MAX
}MI_IVE_AddMode_e;

/*
*Add control parameters,created by Chen Quanfu 2013-07-17
*/
typedef struct MI_IVE_AddCtrl_s
{
    MI_IVE_AddMode_e eMode;
    MI_U0Q16 u0q16X;         /*x of "xA+yB"*/
    MI_U0Q16 u0q16Y;         /*y of "xA+yB"*/
}MI_IVE_AddCtrl_t;

/*
*NCC dst memory struct
*/
typedef struct MI_IVE_NccDstMem_s
{
    MI_U64 u64Numerator;
    MI_U64 u64QuadSum1;
    MI_U64 u64QuadSum2;
    MI_U8  au8Reserved[8];
}MI_IVE_NccDstMem_t;

/*
*Region struct
*/
typedef struct MI_IVE_Region_s
{
    MI_U32 u32Area;            /*Represented by the pixel number*/
    MI_U16 u16Left;            /*Circumscribed rectangle left border*/
    MI_U16 u16Right;           /*Circumscribed rectangle right border*/
    MI_U16 u16Top;             /*Circumscribed rectangle top border*/
    MI_U16 u16Bottom;          /*Circumscribed rectangle bottom border*/
}MI_IVE_Region_t;

/*
*CCBLOB struct
*/
typedef struct MI_IVE_CcBlob_s
{
    MI_U16 u16CurAreaThr;                               /*Threshold of the result regions' area*/
    MI_S8  s8LabelStatus;                               /*-1: Labeled failed ; 0: Labeled successfully*/
    MI_U8  u8RegionNum;                                 /*Number of valid region, non-continuous stored*/
    MI_IVE_Region_t astRegion[MI_IVE_MAX_REGION_NUM];   /*Valid regions with 'u32Area>0' and 'label = ArrayIndex+1'*/
}MI_IVE_CcBlob_t;

/*
*CCL control struct
*/
typedef struct MI_IVE_Ccl_CTRL_S
{
    MI_U16 u16InitAreaThr;    /*Init threshold of region area*/
    MI_U16 u16Step;           /*Increase area step for once*/
}MI_IVE_CclCtrl_t;

/*
*GMM control struct
*/
typedef struct MI_IVE_GmmCtrl_s
{
    MI_U22Q10    u22q10NoiseVar;        /*Initial noise Variance*/
    MI_U22Q10    u22q10MaxVar;          /*Max  Variance*/
    MI_U22Q10    u22q10MinVar;          /*Min  Variance*/
    MI_U0Q16     u0q16LearnRate;        /*Learning rate*/
    MI_U0Q16     u0q16BgRatio;          /*Background ratio*/
    MI_U8Q8      u8q8VarThr;            /*Variance Threshold*/
    MI_U0Q16     u0q16InitWeight;       /*Initial Weight*/
    MI_U8        u8ModelNum;            /*Model number: 3 or 5*/
}MI_IVE_GmmCtrl_t;

/*
*CannyHysEdge control struct
*/
typedef struct MI_IVE_CannyHysEdgeCtrl_s
{
    MI_IVE_MemInfo_t stMem;
    MI_U16 u16LowThr;
    MI_U16 u16HighThr;
    MI_S8 as8Mask[MI_IVE_MASK_SIZE_5X5];
} MI_IVE_CannyHysEdgeCtrl_t;

/*
*Canny stack size struct
*/
typedef struct MI_IVE_CannyStackSize_s
{
    MI_U32 u32StackSize;   /*Stack size for output*/
    MI_U8 u8Reserved[MI_IVE_CANNY_STACK_RESERVED_SIZE];  /*For 16 byte align*/
}MI_IVE_CannyStackSize_t;

/*
*LBP compare mode
*/
typedef enum
{
    E_MI_IVE_LBP_CMP_MODE_NORMAL    = 0x0,     /* P(x)-P(center)>= un8BitThr.s8Val, s(x)=1; else s(x)=0; */
    E_MI_IVE_LBP_CMP_MODE_ABS       = 0x1,     /* Abs(P(x)-P(center))>=un8BitThr.u8Val, s(x)=1; else s(x)=0; */
    E_MI_IVE_LBP_CMP_MODE_ABS_MUL   = 0x2,

    E_MI_IVE_LBP_CMP_MODE_MAX
}MI_IVE_LbpCmpMode_e;

/*
*LBP channel mode
*/
typedef enum
{
    E_MI_IVE_LBP_CHAL_MODE_U8C1    = 0x0,
    E_MI_IVE_LBP_CHAL_MODE_U8C2    = 0x1,

    E_MI_IVE_LBP_CHAL_MODE_MAX
}MI_IVE_LbpChalMode_e;

/*
*LBP control struct
*/
typedef struct MI_IVE_LbpCtrrl_s
{
    MI_IVE_LbpCmpMode_e eMode;
    MI_IVE_LbpChalMode_e chMode;
    MI_IVE_Length8bit_u un8BitThr;
}MI_IVE_LbpCtrrl_t;

/*
*Type of the GradientFilter output format
*/
typedef enum
{
    E_MI_IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER     =  0x0,
    E_MI_IVE_NORM_GRAD_OUT_CTRL_HOR             =  0x1,
    E_MI_IVE_NORM_GRAD_OUT_CTRL_VER             =  0x2,
    E_MI_IVE_NORM_GRAD_OUT_CTRL_COMBINE         =  0x3,

    E_MI_IVE_NORM_GRAD_OUT_CTRL_MAX
}MI_IVE_NormGradOutCtrl_e;

/*
*GradientFilter control parameters
*/
typedef struct MI_IVE_NormGradCtrl_s
{
    MI_IVE_NormGradOutCtrl_e eOutCtrl;
    MI_S8 as8Mask[MI_IVE_MASK_SIZE_5X5];
    MI_U8 u8Norm;
}MI_IVE_NormGradCtrl_t;

/*
* LKOpticalFlow  movement
*/
typedef struct MI_IVE_MvS9Q7_s
{
    MI_S32      s32Status;        /*Result of tracking: 0-success; -1-failure*/
    MI_S9Q7     s9q7Dx;           /*X-direction component of the movement*/
    MI_S9Q7     s9q7Dy;           /*Y-direction component of the movement*/
}MI_IVE_MvS9Q7_t;

typedef struct MI_IVE_LkOpticalFlowCtrl_s
{
    MI_U16      u16CornerNum;     /*Number of the feature points,<200*/
    MI_U0Q8     u0q8MinEigThr;    /*Minimum eigenvalue threshold*/
    MI_U8       u8IterCount;      /*Maximum iteration times*/
    MI_U0Q8     u0q8Epsilon;      /*Threshold of iteration for dx^2 + dy^2 < u0q8Epsilon */
}MI_IVE_LkOpticalFlowCtrl_t;

/*
* Sad mode
*/
typedef enum
{
    E_MI_IVE_SAD_MODE_MB_4X4    = 0x0, /*4x4*/
    E_MI_IVE_SAD_MODE_MB_8X8    = 0x1, /*8x8*/
    E_MI_IVE_SAD_MODE_MB_16X16  = 0x2, /*16x16*/

    E_MI_IVE_SAD_MODE_MAX
}MI_IVE_SadMode_e;
/*
*Sad output ctrl
*/
typedef enum
{
    E_MI_IVE_SAD_OUT_CTRL_16BIT_BOTH    = 0x0, /*Output 16 bit sad and thresh*/
    E_MI_IVE_SAD_OUT_CTRL_8BIT_BOTH     = 0x1, /*Output 8 bit sad and thresh*/
    E_MI_IVE_SAD_OUT_CTRL_16BIT_SAD     = 0x2, /*Output 16 bit sad*/
    E_MI_IVE_SAD_OUT_CTRL_8BIT_SAD      = 0x3, /*Output 8 bit sad*/
    E_MI_IVE_SAD_OUT_CTRL_THRESH        = 0x4, /*Output thresh,16 bits sad */

    E_MI_IVE_SAD_OUT_CTRL_MAX
}MI_IVE_SadOutCtrl_e;
/*
* Sad ctrl param
*/
typedef struct MI_IVE_SadCtrl_s
{
    MI_IVE_SadMode_e eMode;
    MI_IVE_SadOutCtrl_e eOutCtrl;
    MI_U16 u16Thr;              /*srcVal <= u16Thr, dstVal = minVal; srcVal > u16Thr, dstVal = maxVal.*/
    MI_U8 u8MinVal;             /*Min value*/
    MI_U8 u8MaxVal;             /*Max value*/
}MI_IVE_SadCtrl_t;

typedef enum
{
    E_MI_IVE_RESIZE_TYPE_U8C1             = 0x0,
    E_MI_IVE_RESIZE_TYPE_U8C3_PLANAR      = 0x1,
    E_MI_IVE_RESIZE_TYPE_U8C3_PACKAGE     = 0x2,
    E_MI_IVE_RESIZE_TYPE_YUV420SP         = 0x3,

    E_MI_IVE_RESIZE_TYPE_MAX
}MVE_IVE_ResizeMode_e;

typedef struct _MVE_IVE_ResizeCtrl_s
{
    MVE_IVE_ResizeMode_e eMode;    /*Input and output mode*/
} MVE_IVE_ResizeCtrl_t;

typedef enum
{
    E_MI_IVE_BERNSEN_MODE_NORMAL              = 0x00,
    E_MI_IVE_BERNSEN_MODE_THRESH              = 0x01,

    E_MI_IVE_BERNSEN_MODE_MAX
} MVE_IVE_BernsenMode_e;

typedef struct MVE_IVE_BernsenCtrl_s
{
    MVE_IVE_BernsenMode_e enMode;
    MI_U8 u8WinSize;    /*3 or 5*/
    MI_U8 u8Thr;
} MVE_IVE_BernsenCtrl_t;

typedef struct MVE_IVE_AdpThreshCtrl_s
{
    MI_U8 u8RateThr;
    MI_U8 u8HalfMaskx;
    MI_U8 u8HalfMasky;
    MI_S8 s8Offset;
    MI_U8 u8ValueThr;
} MVE_IVE_AdpThreshCtrl_t;

typedef struct MVE_IVE_LineFilterHorCtrl_s
{
    MI_U8 u8GapMinLen;
    MI_U8 u8DensityThr;
    MI_U8 u8HorThr;
} MVE_IVE_LineFilterHorCtrl_t;

typedef struct MVE_IVE_LineFilterVerCtrl_s
{
    MI_U8 u8VerThr;
} MVE_IVE_LineFilterVerCtrl_t;

typedef struct MVE_IVE_NoiseRemoveHorCtrl_s
{
    MI_U8 u8HorThr;
    MI_U8 u8HorThrMax;
} MVE_IVE_NoiseRemoveHorCtrl_t;

typedef struct MVE_IVE_NoiseRemoveVerCtrl_s
{
    MI_U8 u8VerThr;
    MI_U8 u8VerThrMax;
} MVE_IVE_NoiseRemoveVerCtrl_t;

typedef enum
{
    E_MI_IVE_ACC_MODE_INCREASE             = 0x0,
    E_MI_IVE_ACC_MODE_DECREASE             = 0x1,
    E_MI_IVE_ACC_MODE_INCREASE_MAP_255TO1  = 0x2,

    E_MI_IVE_ACC_MODE_MAX
} MVE_IVE_AccMode_e;

typedef struct MVE_IVE_AccCtrl_s
{
    MVE_IVE_AccMode_e enMode;
} MVE_IVE_AccCtrl_t;

typedef enum
{
    E_MI_IVE_BAT_OUT_CTRL_BOTH   = 0x0,  /*Output horizontal and vertical*/
    E_MI_IVE_BAT_OUT_CTRL_HOR    = 0x1,  /*Output horizontal*/
    E_MI_IVE_BAT_OUT_CTRL_VER    = 0x2,  /*Output vertical*/

    E_MI_IVE_BAT_OUT_CTRL_MAX
} MVE_IVE_BatMode_e;

typedef struct MVE_IVE_BatCtrl_s
{
    MVE_IVE_BatMode_e enMode;
    MI_U16 u16HorTimes;
    MI_U16 u16VerTimes;
} MVE_IVE_BatCtrl_t;

typedef enum
{
    E_MI_IVE_MATRIX_TRANSFORM_TYPE_C1     = 0x0,
    E_MI_IVE_MATRIX_TRANSFORM_TYPE_C2     = 0x1,
    E_MI_IVE_MATRIX_TRANSFORM_TYPE_C3     = 0x2,

    E_MI_IVE_MATRIX_TRANSFORM_TYPE_MAX
}MVE_IVE_MatrTranfMode_e;

typedef struct MI_IVE_MatrTranfCtrl_S
{
    MVE_IVE_MatrTranfMode_e enMode;    /*Input channel mode*/
    MI_S32 s32MatrixArray[9];  //Official
} MI_IVE_MatrTranfCtrl_t;

typedef enum
{
    E_MI_IVE_SHIFT_DETECT_MODE_SINGLE    = 0x00,
    E_MI_IVE_SHIFT_DETECT_MODE_MULTI     = 0x01,

    E_MI_IVE_SHIFT_DETECT_MODE_MAX
} MVE_IVE_SHIFT_DETECT_MODE_e;

typedef struct MVE_IVE_SHIFT_DETECT_CTRL_S
{
    MVE_IVE_SHIFT_DETECT_MODE_e enMode;
    MI_U8 pyramid_level;
    MI_U8 search_range;
    MI_U16 u16Left;
    MI_U16 u16Top;
    MI_U16 u16Width;
    MI_U16 u16Height;
} MI_IVE_SHIFT_DETECT_CTRL_t;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_MI_IVE_DATATYPE_H_*/
