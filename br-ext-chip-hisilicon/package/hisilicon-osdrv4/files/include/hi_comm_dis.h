/******************************************************************************

  Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_dis.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2016/08/23
  Description   : hi_comm_dis.h header file
  History       :
******************************************************************************/
#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#ifndef __HI_COMM_DIS_H__
#define __HI_COMM_DIS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* failure caused by malloc buffer */
#define HI_ERR_DIS_NOBUF         HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_DIS_BUF_EMPTY     HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_DIS_NULL_PTR      HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_DIS_ILLEGAL_PARAM HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_DIS_BUF_FULL      HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_DIS_SYS_NOTREADY  HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_DIS_NOT_SUPPORT   HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_DIS_NOT_PERMITTED HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_DIS_BUSY          HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_DIS_INVALID_CHNID HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_DIS_CHN_UNEXIST   HI_DEF_ERR(HI_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)

/* Different mode of DIS */
typedef enum hiDIS_MODE_E {
    DIS_MODE_4_DOF_GME = 0, /* Only use with GME in 4 dof  */
    DIS_MODE_6_DOF_GME, /* Only use with GME in 6 dof  */
    DIS_MODE_GYRO, /* Only use with gryo in 6 dof  */
    DIS_MODE_DOF_BUTT,
} DIS_MODE_E;

/* The motion level of camera */
typedef enum hiDIS_MOTION_LEVEL_E {
    DIS_MOTION_LEVEL_LOW = 0, /* Low motion level */
    DIS_MOTION_LEVEL_NORMAL, /* Normal motion level */
    DIS_MOTION_LEVEL_HIGH, /* High motion level */
    DIS_MOTION_LEVEL_BUTT
} DIS_MOTION_LEVEL_E;

/* Different product type used DIS */
typedef enum hiDIS_PDT_TYPE_E {
    DIS_PDT_TYPE_IPC = 0, /* IPC product type */
    DIS_PDT_TYPE_DV, /* DV product type */
    DIS_PDT_TYPE_DRONE, /* DRONE product type */
    DIS_PDT_TYPE_BUTT
} DIS_PDT_TYPE_E;

/* The Attribute of DIS */
typedef struct hiDIS_ATTR_S {
    HI_BOOL bEnable; /* RW; DIS enable */
    HI_BOOL bGdcBypass; /* RW; gdc correction process , DIS = GME&GDC correction */
    HI_U32 u32MovingSubjectLevel; /* RW; Range:[0,6]; Moving Subject level */
    HI_S32 s32RollingShutterCoef; /* RW; Range:[0,1000]; Rolling shutter coefficients */
    HI_S32 s32Timelag; /* RW; Range:[-2000000,2000000]; Timestamp delay between Gyro and Frame PTS */
    HI_U32 u32ViewAngle; /* Reserved */
    HI_U32 u32HorizontalLimit; /* RW; Range:[0,1000]; Parameter to limit horizontal drift by large foreground */
    HI_U32 u32VerticalLimit; /* RW; Range:[0,1000]; Parameter to limit vertical drift by large foreground */
    HI_BOOL bStillCrop; /* RW; The stabilization will be not working ,but the output image still be cropped */
} DIS_ATTR_S;

/* The Config of DIS */
typedef struct hiDIS_CONFIG_S {
    DIS_MODE_E enMode; /* RW; DIS Mode */
    DIS_MOTION_LEVEL_E enMotionLevel; /* RW; DIS Motion level of the camera */
    DIS_PDT_TYPE_E enPdtType; /* RW; DIS product type */
    HI_U32 u32BufNum; /* RW; Range:[5,10]; Buf num for DIS */
    HI_U32 u32CropRatio; /* RW; Range:[50,98]; Crop ratio of output image */
    HI_U32 u32FrameRate; /* RW; Range: Hi3559AV100 = (0, 120] | Hi3519AV100 = (0, 120] | Hi3516CV500 = (0, 60] |
                            Hi3516DV300 = (0, 60] | Hi3559V200 = (0, 60] | Hi3556V200 = (0, 60] . */
    HI_U32 u32GyroOutputRange; /* RW; Range:[0,200000]; The range of Gyro output in degree */
    HI_U32 u32GyroDataBitWidth; /* RW; Range:[0,32]; The bits used for gyro angular velocity output */
    HI_BOOL bCameraSteady; /* RW; The camera is steady or not */
    HI_BOOL bScale; /* RW; Scale output image or not */
} DIS_CONFIG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
