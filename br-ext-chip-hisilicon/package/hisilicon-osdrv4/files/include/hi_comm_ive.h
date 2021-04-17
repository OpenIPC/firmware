/******************************************************************************

  Copyright (C), 2001-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_ive.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (IVE) group
  Created       : 2011/05/16
  Description   :
  1.Date        : 2011/05/16
    Modification: Created file

  2.Date        : 2013/07/01~2014/08/08
    Modification: Add MPI function
******************************************************************************/
#ifndef _HI_COMM_IVE_H_
#define _HI_COMM_IVE_H_

#include "hi_type.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* -----------------------------------------------*
 * The fixed-point data type, will be used to    *
 * represent float data in hardware calculations.*
 * ----------------------------------------------- */
/* --u8bit---------------------------------------- */
typedef unsigned char HI_U0Q8;
typedef unsigned char HI_U1Q7;
typedef unsigned char HI_U5Q3;
typedef unsigned char HI_U3Q5;

/* --u16bit--------------------------------------- */
typedef unsigned short HI_U0Q16;
typedef unsigned short HI_U4Q12;
typedef unsigned short HI_U6Q10;
typedef unsigned short HI_U8Q8;
typedef unsigned short HI_U9Q7;
typedef unsigned short HI_U12Q4;
typedef unsigned short HI_U14Q2;
typedef unsigned short HI_U5Q11;
typedef unsigned short HI_U1Q15;
typedef unsigned short HI_U2Q14;
typedef HI_U6Q10 HI_UFP16;
/* --s16bit--------------------------------------- */
typedef short HI_S9Q7;
typedef short HI_S14Q2;
typedef short HI_S1Q15;

/* --u32bit--------------------------------------- */
typedef unsigned int HI_U22Q10;
typedef unsigned int HI_U25Q7;
typedef unsigned int HI_U21Q11;
typedef unsigned int HI_U14Q18;
typedef unsigned int HI_U8Q24;
typedef unsigned int HI_U4Q28;

/* --s32bit--------------------------------------- */
typedef int HI_S25Q7;
typedef int HI_S16Q16;
typedef int HI_S14Q18;
typedef int HI_S20Q12;

typedef int HI_S24Q8;

/* -----------------------------------------------*
 * The fixed-point data type combine with flag_bits.*
 * ----------------------------------------------- */
/* 8bits unsigned integer,4bits decimal fraction,4bits flag_bits */
typedef unsigned short HI_U8Q4F4;

/* -----------------------------------------------*
 * The fixed-point data type, will be used to    *
 * represent float data in hardware calculations. linux code style.*
 * ----------------------------------------------- */
/* --u8bit---------------------------------------- */
typedef HI_U0Q8 hi_u0q8;
typedef HI_U1Q7 hi_u1q7;
typedef HI_U5Q3 hi_u5q3;
typedef HI_U3Q5 hi_u3q5;

/* --u16bit--------------------------------------- */
typedef HI_U0Q16 hi_u0q16;
typedef HI_U4Q12 hi_u4q12;
typedef HI_U6Q10 hi_u6q10;
typedef HI_U8Q8  hi_u8q8;
typedef HI_U9Q7  hi_u9q7;
typedef HI_U12Q4 hi_u12q4;
typedef HI_U14Q2 hi_u14q2;
typedef HI_U5Q11 hi_u5q11;
typedef HI_U1Q15 hi_u1q15;
typedef HI_U2Q14 hi_u2q14;
typedef HI_UFP16 hi_ufp16;
/* 8bits unsigned integer,4bits decimal fraction,4bits flag_bits */
typedef HI_U8Q4F4 hi_u8q4f4;

/* --s16bit--------------------------------------- */
typedef HI_S9Q7  hi_s9q7;
typedef HI_S14Q2 hi_s14q2;
typedef HI_S1Q15 hi_s1q15;

/* --u32bit--------------------------------------- */
typedef HI_U22Q10 hi_u22q10;
typedef HI_U25Q7  hi_u25q7;
typedef HI_U21Q11 hi_u21q11;
typedef HI_U14Q18 hi_u14q18;
typedef HI_U8Q24  hi_u8q24;
typedef HI_U4Q28  hi_u4q28;

/* --s32bit--------------------------------------- */
typedef HI_S25Q7  hi_s25q7;
typedef HI_S16Q16 hi_s16q16;
typedef HI_S14Q18 hi_s14q18;
typedef HI_S20Q12 hi_s20q12;
typedef HI_S24Q8  hi_s24q8;


/* Type of the IVE_IMAGE_S data.Aded by tanbing 2013-7-22 */
typedef enum hiIVE_IMAGE_TYPE_E {
    IVE_IMAGE_TYPE_U8C1 = 0x0,
    IVE_IMAGE_TYPE_S8C1 = 0x1,

    IVE_IMAGE_TYPE_YUV420SP = 0x2, /* YUV420 SemiPlanar */
    IVE_IMAGE_TYPE_YUV422SP = 0x3, /* YUV422 SemiPlanar */
    IVE_IMAGE_TYPE_YUV420P = 0x4,  /* YUV420 Planar */
    IVE_IMAGE_TYPE_YUV422P = 0x5,  /* YUV422 planar */

    IVE_IMAGE_TYPE_S8C2_PACKAGE = 0x6,
    IVE_IMAGE_TYPE_S8C2_PLANAR = 0x7,

    IVE_IMAGE_TYPE_S16C1 = 0x8,
    IVE_IMAGE_TYPE_U16C1 = 0x9,

    IVE_IMAGE_TYPE_U8C3_PACKAGE = 0xa,
    IVE_IMAGE_TYPE_U8C3_PLANAR = 0xb,

    IVE_IMAGE_TYPE_S32C1 = 0xc,
    IVE_IMAGE_TYPE_U32C1 = 0xd,

    IVE_IMAGE_TYPE_S64C1 = 0xe,
    IVE_IMAGE_TYPE_U64C1 = 0xf,

    IVE_IMAGE_TYPE_BUTT

} IVE_IMAGE_TYPE_E;

/* Definition of the IVE_IMAGE_S. Added by Tan Bing, 2013-7-22. */
typedef struct hiIVE_IMAGE_S {
    HI_U64 au64PhyAddr[3];   /* RW;The physical address of the image */
    HI_U64 au64VirAddr[3];   /* RW;The virtual address of the image */
    HI_U32 au32Stride[3];    /* RW;The stride of the image */
    HI_U32 u32Width;         /* RW;The width of the image */
    HI_U32 u32Height;        /* RW;The height of the image */
    IVE_IMAGE_TYPE_E enType; /* RW;The type of the image */
} IVE_IMAGE_S;

typedef IVE_IMAGE_S IVE_SRC_IMAGE_S;
typedef IVE_IMAGE_S IVE_DST_IMAGE_S;

/*
* Definition of the IVE_MEM_INFO_S.This struct special purpose for input or ouput, such as Hist, CCL, ShiTomasi.
* Added by Chen Quanfu, 2013-7-23.
*/
typedef struct hiIVE_MEM_INFO_S {
    HI_U64 u64PhyAddr; /* RW;The physical address of the memory */
    HI_U64 u64VirAddr; /* RW;The virtual address of the memory */
    HI_U32 u32Size;    /* RW;The size of memory */
} IVE_MEM_INFO_S;
typedef IVE_MEM_INFO_S IVE_SRC_MEM_INFO_S;
typedef IVE_MEM_INFO_S IVE_DST_MEM_INFO_S;

/* Data struct ,created by Chen Quanfu 2013-07-19 */
typedef struct hiIVE_DATA_S {
    HI_U64 u64PhyAddr; /* RW;The physical address of the data */
    HI_U64 u64VirAddr; /* RW;The virtaul address of the data */

    HI_U32 u32Stride; /* RW;The stride of 2D data by byte */
    HI_U32 u32Width;  /* RW;The width of 2D data by byte */
    HI_U32 u32Height; /* RW;The height of 2D data by byte */

    HI_U32 u32Reserved;
} IVE_DATA_S;
typedef IVE_DATA_S IVE_SRC_DATA_S;
typedef IVE_DATA_S IVE_DST_DATA_S;

/* Definition of the union of IVE_8BIT_U.Added by Tan Bing, 2013-7-22. */
typedef union hiIVE_8BIT_U {
    HI_S8 s8Val;
    HI_U8 u8Val;
} IVE_8BIT_U;

/* Definition of u16 point */
typedef struct hiIVE_POINT_U16_S {
    HI_U16 u16X; /* RW;The X coordinate of the point */
    HI_U16 u16Y; /* RW;The Y coordinate of the point */
} IVE_POINT_U16_S;

/*
* Definition of s16 point
*/
typedef struct hiIVE_POINT_S16_S {
    HI_U16 s16X; /* RW;The X coordinate of the point */
    HI_U16 s16Y; /* RW;The Y coordinate of the point */
} IVE_POINT_S16_S;

/*
* Float point represented by Fixed-point SQ25.7
*/
typedef struct hiIVE_POINT_S25Q7_S {
    HI_S25Q7 s25q7X; /* RW;The X coordinate of the point */
    HI_S25Q7 s25q7Y; /* RW;The Y coordinate of the point */
} IVE_POINT_S25Q7_S;

/*
* Point u14q2
*/
typedef struct hiIVE_POINT_U14Q2_S {
    HI_U14Q2 u14q2X;
    HI_U14Q2 u14q2Y;
} IVE_POINT_U14Q2_S;

/* Definition of rectangle */
typedef struct hiIVE_RECT_U16_S {
    HI_U16 u16X;      /* RW;The location of X axis of the rectangle */
    HI_U16 u16Y;      /* RW;The location of Y axis of the rectangle */
    HI_U16 u16Width;  /* RW;The width of the rectangle */
    HI_U16 u16Height; /* RW;The height of the rectangle */
} IVE_RECT_U16_S;

typedef struct hiIVE_RECT_U32_S {
    HI_U32 u32X;      /* RW;The location of X axis of the rectangle */
    HI_U32 u32Y;      /* RW;The location of Y axis of the rectangle */
    HI_U32 u32Width;  /* RW;The width of the rectangle */
    HI_U32 u32Height; /* RW;The height of the rectangle */
} IVE_RECT_U32_S;

typedef struct hiIVE_RECT_S24Q8_S {
    HI_S24Q8 s24q8X;
    HI_S24Q8 s24q8Y;
    HI_U32 u32Width;
    HI_U32 u32Height;
} IVE_RECT_S24Q8_S;

typedef struct hiIVE_LOOK_UP_TABLE_S {
    IVE_MEM_INFO_S stTable;
    HI_U16 u16ElemNum; /* RW;LUT's elements number */

    HI_U8 u8TabInPreci;
    HI_U8 u8TabOutNorm;

    HI_S32 s32TabInLower; /* RW;LUT's original input lower limit */
    HI_S32 s32TabInUpper; /* RW;LUT's original input upper limit */
} IVE_LOOK_UP_TABLE_S;

/* Blob type */
typedef enum hiIVE_BLOB_TYPE_E {
    IVE_BLOB_TYPE_S32 = 0x0,

    IVE_BLOB_TYPE_U8 = 0x1,

    /* channel = 3 */
    IVE_BLOB_TYPE_YVU420SP = 0x2,
    /* channel = 3 */
    IVE_BLOB_TYPE_YVU422SP = 0x3,

    IVE_BLOB_TYPE_VEC_S32 = 0x4,

    IVE_BLOB_TYPE_SEQ_S32 = 0x5,

    IVE_BLOB_TYPE_U16 = 0x6,

    IVE_BLOB_TYPE_S8 = 0x7,

    IVE_BLOB_TYPE_F32 = 0x8,

    IVE_BLOB_TYPE_BUTT
} IVE_BLOB_TYPE_E;

/****************************** Blob struct ******************************
In Caffe, the blob contain shape info as the following order:
Image\FeatureMap:               N       C       H       W
FC(normal vector):              N       C
RNN\LSTM(Recurrent) vector:     T       N       D

The relationship of the following blob struct with Caffe blob is as follows:
Image\FeatureMap:               Num    Chn    Height   With
FC(VEC_S32):                    Num    Width
RNN\LSTM(SEQ_S32) vector:       Step   Num     Dim
The stride, which measuring unit is byte, is always algined by the width or
dim direction.
**************************************************************************/
typedef struct hiIVE_BLOB_S {
    IVE_BLOB_TYPE_E enType; /* Blob type */
    HI_U32 u32Stride;       /* Stride, a line bytes num */

    HI_U64 u64VirAddr; /* virtual addr */
    HI_U64 u64PhyAddr; /* physical addr */

    HI_U32 u32Num; /* N: frame num or sequence num,correspond to caffe blob's n */
    union {
        struct {
            HI_U32 u32Width;  /* W: frame width, correspond to caffe blob's w */
            HI_U32 u32Height; /* H: frame height, correspond to caffe blob's h */
            HI_U32 u32Chn;    /* C: frame channel,correspond to caffe blob's c */
        } stWhc;
        struct {
            HI_U32 u32Dim;         /* D: vecotr dimension */
            HI_U64 u64VirAddrStep; /* T: virtual adress of   time steps array in each sequence */
        } stSeq;
    } unShape;

} IVE_BLOB_S;

typedef IVE_BLOB_S IVE_SRC_BLOB_S;
typedef IVE_BLOB_S IVE_DST_BLOB_S;

typedef enum hiEN_IVE_ERR_CODE_E {
    ERR_IVE_SYS_TIMEOUT = 0x40,   /* IVE process timeout */
    ERR_IVE_QUERY_TIMEOUT = 0x41, /* IVE query timeout */
    ERR_IVE_OPEN_FILE = 0x42,     /* IVE open file error */
    ERR_IVE_READ_FILE = 0x43,     /* IVE read file error */
    ERR_IVE_WRITE_FILE = 0x44,    /* IVE write file error */
    ERR_IVE_BUS_ERR = 0x45,

    ERR_IVE_BUTT
} EN_IVE_ERR_CODE_E;

typedef enum hiEN_FD_ERR_CODE_E {
    ERR_FD_SYS_TIMEOUT = 0x40,   /* FD process timeout */
    ERR_FD_CFG = 0x41,           /* FD configuration error */
    ERR_FD_FACE_NUM_OVER = 0x42, /* FD candidate face number over */
    ERR_FD_OPEN_FILE = 0x43,     /* FD open file error */
    ERR_FD_READ_FILE = 0x44,     /* FD read file error */
    ERR_FD_WRITE_FILE = 0x45,    /* FD write file error */

    ERR_FD_BUTT
} EN_FD_ERR_CODE_E;

/************************************************IVE error code ***********************************/
/* Invalid device ID */
#define HI_ERR_IVE_INVALID_DEVID HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* Invalid channel ID */
#define HI_ERR_IVE_INVALID_CHNID HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define HI_ERR_IVE_ILLEGAL_PARAM HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define HI_ERR_IVE_EXIST HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* The UN exists. */
#define HI_ERR_IVE_UNEXIST HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* A null point is used. */
#define HI_ERR_IVE_NULL_PTR HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define HI_ERR_IVE_NOT_CONFIG HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* The operation is not supported currently. */
#define HI_ERR_IVE_NOT_SURPPORT HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define HI_ERR_IVE_NOT_PERM HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define HI_ERR_IVE_NOMEM HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* A failure caused by the malloc buffer occurs. */
#define HI_ERR_IVE_NOBUF HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* The buffer is empty. */
#define HI_ERR_IVE_BUF_EMPTY HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define HI_ERR_IVE_BUF_FULL HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* The system is not ready because it may be not initialized or loaded.
 * The error code is returned when a device file fails to be opened. */
#define HI_ERR_IVE_NOTREADY HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* The source address or target address is incorrect during the operations such as calling 
copy_from_user or copy_to_user. */
#define HI_ERR_IVE_BADADDR HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* The resource is busy during the operations such as destroying a VENC channel 
without deregistering it. */
#define HI_ERR_IVE_BUSY HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* IVE process timeout: 0xA01D8040 */
#define HI_ERR_IVE_SYS_TIMEOUT HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_SYS_TIMEOUT)
/* IVE query timeout: 0xA01D8041 */
#define HI_ERR_IVE_QUERY_TIMEOUT HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_QUERY_TIMEOUT)
/* IVE open file error: 0xA01D8042 */
#define HI_ERR_IVE_OPEN_FILE HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_OPEN_FILE)
/* IVE read file error: 0xA01D8043 */
#define HI_ERR_IVE_READ_FILE HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_READ_FILE)
/* IVE read file error: 0xA01D8044 */
#define HI_ERR_IVE_WRITE_FILE HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_WRITE_FILE)
/* IVE Bus error: 0xA01D8045 */
#define HI_ERR_IVE_BUS_ERR HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_BUS_ERR)

/************************************************FD error code ***********************************/
/* Invalid device ID */
#define HI_ERR_FD_INVALID_DEVID HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* Invalid channel ID */
#define HI_ERR_FD_INVALID_CHNID HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define HI_ERR_FD_ILLEGAL_PARAM HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define HI_ERR_FD_EXIST HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* The UN exists. */
#define HI_ERR_FD_UNEXIST HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* A null point is used. */
#define HI_ERR_FD_NULL_PTR HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define HI_ERR_FD_NOT_CONFIG HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* The operation is not supported currently. */
#define HI_ERR_FD_NOT_SURPPORT HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define HI_ERR_FD_NOT_PERM HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define HI_ERR_FD_NOMEM HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* A failure caused by the malloc buffer occurs. */
#define HI_ERR_FD_NOBUF HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* The buffer is empty. */
#define HI_ERR_FD_BUF_EMPTY HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define HI_ERR_FD_BUF_FULL HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* The system is not ready because it may be not initialized or loaded.
 * The error code is returned when a device file fails to be opened. */
#define HI_ERR_FD_NOTREADY HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* The source address or target address is incorrect during the operations such as calling 
copy_from_user or copy_to_user. */
#define HI_ERR_FD_BADADDR HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* The resource is busy during the operations such as destroying a VENC channel 
without deregistering it. */
#define HI_ERR_FD_BUSY HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* FD process timeout:         0xA02F8040 */
#define HI_ERR_FD_SYS_TIMEOUT HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_SYS_TIMEOUT)
/* FD configuration error:     0xA02F8041 */
#define HI_ERR_FD_CFG HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_CFG)
/* FD candidate face number over: 0xA02F8042 */
#define HI_ERR_FD_FACE_NUM_OVER HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_FACE_NUM_OVER)
/* FD open file error: 0xA02F8043 */
#define HI_ERR_FD_OPEN_FILE HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_OPEN_FILE)
/* FD read file error: 0xA02F8044 */
#define HI_ERR_FD_READ_FILE HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_READ_FILE)
/* FD read file error: 0xA02F8045 */
#define HI_ERR_FD_WRITE_FILE HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_WRITE_FILE)

/************************************************ODT error code ***********************************/
/* ODT Invalid channel ID: 0xA0308002 */
#define HI_ERR_ODT_INVALID_CHNID HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* ODT exist: 0xA0308004 */
#define HI_ERR_ODT_EXIST HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* ODT unexist: 0xA0308005 */
#define HI_ERR_ODT_UNEXIST HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* ODT The operation, changing static attributes for example, is not permitted: 0xA0308009 */
#define HI_ERR_ODT_NOT_PERM HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* ODT the system is not ready because it may be not initialized: 0xA0308010 */
#define HI_ERR_ODT_NOTREADY HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* ODT busy: 0xA0308012 */
#define HI_ERR_ODT_BUSY HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __HI_COMM_IVE_H__ */

