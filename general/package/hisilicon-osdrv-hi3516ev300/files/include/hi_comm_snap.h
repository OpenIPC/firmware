/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: hi_comm_snap.h
 * Author:
 * Create: 2017-06-19
 */

#ifndef __HI_COMM_SNAP_H__
#define __HI_COMM_SNAP_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum hiSNAP_TYPE_E {
    SNAP_TYPE_NORMAL, /* ZSL and none-ZSL type */
    SNAP_TYPE_PRO, /* Professional type support HDR, AEB, LongExposure */
    SNAP_TYPE_BUTT
} SNAP_TYPE_E;

typedef struct hiSNAP_PRO_MANUAL_PARAM_S {
    HI_U32 au32ManExpTime[PRO_MAX_FRAME_NUM]; /* RW;  Range: [0x0, 0xFFFFFFFF];sensor exposure time (unit: us),
                                                                    it's related to the specific sensor */
    HI_U32 au32ManSysgain[PRO_MAX_FRAME_NUM]; /* RW;  Range: [0x400, 0xFFFFFFFF];system gain (unit: times,
                                                                    10bit precision), it's related to the specific
                                                                    sensor and ISP Dgain range */
} SNAP_PRO_MANUAL_PARAM_S;

typedef struct hiSNAP_PRO_AUTO_PARAM_S {
    HI_U16 au16ProExpStep[PRO_MAX_FRAME_NUM]; /* RW; Range:[0x0, 0xFFFF];Professional exposure step
                                                                    (unit: times, 8bit precision) */
} SNAP_PRO_AUTO_PARAM_S;

typedef struct hiSNAP_PRO_PARAM_S {
    OPERATION_MODE_E enOperationMode;
    SNAP_PRO_AUTO_PARAM_S stAutoParam;
    SNAP_PRO_MANUAL_PARAM_S stManualParam;
} SNAP_PRO_PARAM_S;

typedef struct hiSNAP_NORMAL_ATTR_S {
    HI_U32  u32FrameCnt;         /* RW;Set capture frame counts */
    HI_U32  u32RepeatSendTimes;  /* [0, 2]; When FE-BE is offline, the first raw frame that is triggered should be sent
                                                    repeatedly to the BE. */

    /* ZSL struct */
    HI_BOOL bZSL;           /* RW;statical attrs */
    HI_U32  u32FrameDepth;  /* buffer depth: [1,8] */
    HI_U32  u32RollbackMs;  /* RW;Rollback time, unit(ms), invalid when bZSL is HI_FALSE  */
    HI_U32  u32Interval;    /* RW;For continuous capture, select frame every u32Intercal frames */
} SNAP_NORMAL_ATTR_S;

typedef struct hiSNAP_PRO_ATTR_S {
    HI_U32  u32FrameCnt;          /* [1, PRO_MAX_FRAME_NUM] */
    HI_U32  u32RepeatSendTimes;   /* [0, 2];When FE-BE is offline, the first raw frame that is triggered should be sent
                                                    repeatedly to the BE. */
    SNAP_PRO_PARAM_S stProParam;
} SNAP_PRO_ATTR_S;

typedef struct hiSNAP_USER_ATTR_S {
    HI_U32 u32FrameDepth;
} SNAP_USER_ATTR_S;

typedef struct hiSNAP_ATTR_S {
    SNAP_TYPE_E enSnapType;
    HI_BOOL bLoadCCM;    /* HI_TRUE:use CCM of SnapIspInfo, HI_FALSE: Algorithm calculate */
    union {
        SNAP_NORMAL_ATTR_S     stNormalAttr;
        SNAP_PRO_ATTR_S        stProAttr;
    };
} SNAP_ATTR_S;

typedef struct hiISP_PRO_BNR_PARAM_S {
    HI_BOOL bEnable;
    HI_U32 u32ParamNum;
    ISP_NR_AUTO_ATTR_S *pastNrAttr;
} ISP_PRO_BNR_PARAM_S;

typedef struct hiISP_PRO_SHARPEN_PARAM_S {
    HI_BOOL bEnable;
    HI_U32 u32ParamNum;
    ISP_SHARPEN_AUTO_ATTR_S *pastShpAttr;
} ISP_PRO_SHARPEN_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


