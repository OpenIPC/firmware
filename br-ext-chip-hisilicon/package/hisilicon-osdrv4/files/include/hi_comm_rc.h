/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2017. All rights reserved.
 * Description:
 * Author: Hisilicon multimedia software group
 * Create: 2016/11/15
 */
#ifndef __HI_COMM_RC_H__
#define __HI_COMM_RC_H__

#include "hi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef HI_U32 HI_FR32;

/* rc mode */
typedef enum hiVENC_RC_MODE_E {
    VENC_RC_MODE_H264CBR = 1,
    VENC_RC_MODE_H264VBR,
    VENC_RC_MODE_H264AVBR, /* Not support for Hi3556AV100/Hi3556V200/Hi3559V200 */
    VENC_RC_MODE_H264QVBR,
    VENC_RC_MODE_H264CVBR,
    VENC_RC_MODE_H264FIXQP,
    VENC_RC_MODE_H264QPMAP, /* Not support for Hi3556AV100/Hi3556V200/Hi3559V200 */

    VENC_RC_MODE_MJPEGCBR,
    VENC_RC_MODE_MJPEGVBR,
    VENC_RC_MODE_MJPEGFIXQP,

    VENC_RC_MODE_H265CBR,
    VENC_RC_MODE_H265VBR,
    VENC_RC_MODE_H265AVBR, /* Not support for Hi3556AV100/Hi3556V200/Hi3559V200 */
    VENC_RC_MODE_H265QVBR,
    VENC_RC_MODE_H265CVBR,
    VENC_RC_MODE_H265FIXQP,
    VENC_RC_MODE_H265QPMAP, /* Not support for Hi3556AV100/Hi3556V200/Hi3559V200 */

    VENC_RC_MODE_BUTT,
} VENC_RC_MODE_E;

/* qpmap mode */
typedef enum hiVENC_RC_QPMAP_MODE_E {
    VENC_RC_QPMAP_MODE_MEANQP = 0,
    VENC_RC_QPMAP_MODE_MINQP,
    VENC_RC_QPMAP_MODE_MAXQP,

    VENC_RC_QPMAP_MODE_BUTT,
} VENC_RC_QPMAP_MODE_E;

/* the attribute of h264e fixqp */
typedef struct hiVENC_H264_FIXQP_S {
    HI_U32      u32Gop;                    /* RW; Range:[1, 65536]; the interval of ISLICE. */
    HI_U32      u32SrcFrameRate;           /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;         /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32IQp;                    /* RW; Range:[0, 51]; qp of the i frame */
    HI_U32      u32PQp;                    /* RW; Range:[0, 51]; qp of the p frame */
    HI_U32      u32BQp;                    /* RW; Range:[0, 51]; qp of the b frame */
} VENC_H264_FIXQP_S;

/* the attribute of h264e cbr */
typedef struct hiVENC_H264_CBR_S {
    HI_U32      u32Gop;                    /* RW; Range:[1, 65536]; the interval of I Frame. */
    HI_U32      u32StatTime;               /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;           /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;         /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32BitRate;                /* RW; Range:[2, 614400]; average bitrate */
} VENC_H264_CBR_S;

/* the attribute of h264e vbr */
typedef struct hiVENC_H264_VBR_S {
    HI_U32      u32Gop;                     /* RW; Range:[1, 65536]; the interval of ISLICE. */
    HI_U32      u32StatTime;                /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;            /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;          /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32MaxBitRate;              /* RW; Range:[2, 614400];the max bitrate */
} VENC_H264_VBR_S;

/* the attribute of h264e cvbr */
typedef struct hiVENC_H264_CVBR_S {
    HI_U32      u32Gop;                     /* RW; Range:[1, 65536]; the interval of ISLICE. */
    HI_U32      u32StatTime;                /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;            /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;          /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32MaxBitRate;              /* RW; Range:[2, 614400];the max bitrate */

    HI_U32      u32ShortTermStatTime;       /* RW; Range:[1, 120]; the long-term rate statistic time, the unit is second (s)*/
    HI_U32      u32LongTermStatTime;        /* RW; Range:[1, 1440]; the long-term rate statistic time, the unit is u32LongTermStatTimeUnit*/
    HI_U32      u32LongTermMaxBitrate ;     /* RW; Range:[2, 614400];the long-term target max bitrate, can not be larger than u32MaxBitRate,the unit is kbps */
    HI_U32      u32LongTermMinBitrate ;     /* RW; Range:[0, 614400];the long-term target min bitrate,  can not be larger than u32LongTermMaxBitrate,the unit is kbps */
} VENC_H264_CVBR_S;

/* the attribute of h264e avbr */
typedef struct hiVENC_H264_AVBR_S {
    HI_U32      u32Gop;                  /* RW; Range:[1, 65536]; the interval of ISLICE. */
    HI_U32      u32StatTime;             /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;         /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;       /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32MaxBitRate;           /* RW; Range:[2, 614400];the max bitrate */
} VENC_H264_AVBR_S;

/* the attribute of h264e qpmap */
typedef struct hiVENC_H264_QPMAP_S {
    HI_U32      u32Gop;                   /* RW; Range:[1, 65536]; the interval of ISLICE. */
    HI_U32      u32StatTime;              /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;          /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;        /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
} VENC_H264_QPMAP_S;

typedef struct hiVENC_H264_QVBR_S {
    HI_U32      u32Gop;                   /* RW; Range:[1, 65536];the interval of ISLICE. */
    HI_U32      u32StatTime;              /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;          /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;        /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32TargetBitRate;         /* RW; Range:[2, 614400]; the target bitrate */
}VENC_H264_QVBR_S;

/* the attribute of h265e qpmap */
typedef struct hiVENC_H265_QPMAP_S {
    HI_U32      u32Gop;                   /* RW; Range:[1, 65536]; the interval of ISLICE. */
    HI_U32      u32StatTime;              /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;          /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;        /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    VENC_RC_QPMAP_MODE_E enQpMapMode;     /* RW;  the QpMap Mode.*/
} VENC_H265_QPMAP_S;

typedef struct hiVENC_H264_CBR_S   VENC_H265_CBR_S;
typedef struct hiVENC_H264_VBR_S   VENC_H265_VBR_S;
typedef struct hiVENC_H264_AVBR_S  VENC_H265_AVBR_S;
typedef struct hiVENC_H264_FIXQP_S VENC_H265_FIXQP_S;
typedef struct hiVENC_H264_QVBR_S  VENC_H265_QVBR_S;
typedef struct hiVENC_H264_CVBR_S  VENC_H265_CVBR_S;

/* the attribute of mjpege fixqp */
typedef struct hiVENC_MJPEG_FIXQP_S {
    HI_U32      u32SrcFrameRate;          /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate;         /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32Qfactor;               /* RW; Range:[1,99];image quality. */
} VENC_MJPEG_FIXQP_S;

/* the attribute of mjpege cbr */
typedef struct hiVENC_MJPEG_CBR_S {
    HI_U32      u32StatTime;              /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;          /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate ;        /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32BitRate;               /* RW; Range:[2, 614400]; average bitrate */
} VENC_MJPEG_CBR_S;

/* the attribute of mjpege vbr */
typedef struct hiVENC_MJPEG_VBR_S {
    HI_U32      u32StatTime;              /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    HI_U32      u32SrcFrameRate;          /* RW; Range:[1, 240]; the input frame rate of the venc chnnel */
    HI_FR32     fr32DstFrameRate;         /* RW; Range:[0.015625, 240]; the target frame rate of the venc chnnel,can not be larger than u32SrcFrameRate */
    HI_U32      u32MaxBitRate;            /* RW; Range:[2, 614400];the max bitrate */
}VENC_MJPEG_VBR_S;

/* the attribute of rc */
typedef struct hiVENC_RC_ATTR_S {
    VENC_RC_MODE_E enRcMode;              /* RW; the type of rc*/
    union {
        VENC_H264_CBR_S    stH264Cbr;
        VENC_H264_VBR_S    stH264Vbr;
        VENC_H264_AVBR_S   stH264AVbr;
        VENC_H264_QVBR_S   stH264QVbr;
        VENC_H264_CVBR_S   stH264CVbr;
        VENC_H264_FIXQP_S  stH264FixQp;
        VENC_H264_QPMAP_S  stH264QpMap;

        VENC_MJPEG_CBR_S   stMjpegCbr;
        VENC_MJPEG_VBR_S   stMjpegVbr;
        VENC_MJPEG_FIXQP_S stMjpegFixQp;

        VENC_H265_CBR_S    stH265Cbr;
        VENC_H265_VBR_S    stH265Vbr;
        VENC_H265_AVBR_S   stH265AVbr;
        VENC_H265_QVBR_S   stH265QVbr;
        VENC_H265_CVBR_S   stH265CVbr;
        VENC_H265_FIXQP_S  stH265FixQp;
        VENC_H265_QPMAP_S  stH265QpMap;
    };
} VENC_RC_ATTR_S;

/* the super frame mode */
typedef enum hiRC_SUPERFRM_MODE_E {
    SUPERFRM_NONE=0,                    /* sdk don't care super frame */
    SUPERFRM_DISCARD,                   /* the super frame is discarded */
    SUPERFRM_REENCODE,                  /* the super frame is re-encode */
    SUPERFRM_BUTT
} VENC_SUPERFRM_MODE_E;

/* The param of H264e cbr */
typedef struct hiVENC_PARAM_H264_CBR_S {
    HI_U32  u32MinIprop;                /* RW; Range:[1, 100]; the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; Range:[1, 100]; the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51];the max QP value */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min QP value,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; max qp for i frame */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; min qp for i frame,can not be larger than u32MaxIQp */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; Range:max number of re-encode times.*/
    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/
} VENC_PARAM_H264_CBR_S;

/* The param of H264e vbr */
typedef struct hiVENC_PARAM_H264_VBR_S {
    HI_S32  s32ChangePos;               /* RW; Range:[50, 100]; Indicates the ratio of the current bit rate to the maximum
                                            bit rate when the QP value starts to be adjusted */
    HI_U32  u32MinIprop;                /* RW; Range:[1, 100] ; the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; Range:[1, 100] ; the max ratio of i frame and p frame,can not be smaller than u32MinIprop*/
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; max number of re-encode times */
    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; the min I qp ,can not be larger than u32MaxIQp*/
} VENC_PARAM_H264_VBR_S;

/* The param of H264e avbr */
typedef struct hiVENC_PARAM_H264_AVBR_S {
    HI_S32  s32ChangePos;               /* RW; Range:[50, 100]; Indicates the ratio of the current bit rate to the maximum
                                            bit rate when the QP value starts to be adjusted */
    HI_U32  u32MinIprop;                /* RW; Range:[1, 100] ; the min ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_U32  u32MaxIprop;                /* RW; Range:[1, 100] ; the max ratio of i frame and p frame */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; max number of re-encode times */
    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/

    HI_S32  s32MinStillPercent;         /* RW; Range:[5, 100]; the min percent of target bitrate for still scene */
    HI_U32  u32MaxStillQP;              /* RW; Range:[0, 51]; the max QP value of I frame for still scene,
                                               can not be smaller than u32MinIQp and can not be larger than su32MaxIQp*/
    HI_U32  u32MinStillPSNR;            /* RW; reserved,Invalid member currently */

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */
    HI_U32  u32MinQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMinQp and MinQp, FrameLevelMinQp = MinQp(or MinIQp) + MinQpDelta  */
    HI_U32  u32MotionSensitivity;       /* RW; Range:[0, 100]; Motion Sensitivity */
} VENC_PARAM_H264_AVBR_S;

typedef struct hiVENC_PARAM_H264_QVBR_S {
    HI_U32  u32MinIprop;               /* RW; Range:[1, 100]; the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;               /* RW; Range:[1, 100] ;the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;       /* RW; Range:[0, 3];max number of re-encode times [0, 3]*/
    HI_BOOL bQpMapEn;                  /* RW; Range:[0, 1]; enable qpmap.*/

    HI_U32  u32MaxQp;                  /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                  /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp*/
    HI_U32  u32MaxIQp;                 /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                 /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */

    HI_S32  s32BitPercentUL;           /* RW; Range:[30, 180]; Indicate the ratio of bitrate  upper limit*/
    HI_S32  s32BitPercentLL;           /* RW; Range:[30, 180]; Indicate the ratio of bitrate  lower limit,can not be larger than s32BitPercentUL*/
    HI_S32  s32PsnrFluctuateUL;        /* RW; Range:[18, 40]; Reduce the target bitrate when the value of psnr approch the upper limit*/
    HI_S32  s32PsnrFluctuateLL;        /* RW; Range:[18, 40]; Increase the target bitrate when the value of psnr approch the lower limit,
                                                              can not be larger than s32PsnrFluctuateUL*/
} VENC_PARAM_H264_QVBR_S;

/* The param of H264e cvbr */
typedef struct hiVENC_PARAM_H264_CVBR_S {
    HI_U32  u32MinIprop;                /* RW; Range:[1, 100] ; the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; Range:[1, 100] ; the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; max number of re-encode times */
    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */

    HI_U32  u32MinQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMinQp and MinQp, FrameLevelMinQp = MinQp(or MinIQp) + MinQpDelta  */
    HI_U32  u32MaxQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMaxQp and MaxQp, FrameLevelMaxQp = MaxQp(or MaxIQp) - MaxQpDelta  */

    HI_U32  u32ExtraBitPercent;         /* RW; Range:[0, 1000];the extra bits that can be allocated when the actual bitrate goes above the target bitrate*/
    HI_U32  u32LongTermStatTimeUnit;    /* RW; Range:[1, 1800]; the time unit of LongTermStatTime,, the unit is senconds(s)*/
} VENC_PARAM_H264_CVBR_S;

/* The param of mjpege cbr */
typedef struct hiVENC_PARAM_MJPEG_CBR_S {
    HI_U32  u32MaxQfactor;               /* RW; Range:[1, 99]; the max Qfactor value*/
    HI_U32  u32MinQfactor;               /* RW; Range:[1, 99]; the min Qfactor value ,can not be larger than u32MaxQfactor */
} VENC_PARAM_MJPEG_CBR_S;

/* The param of mjpege vbr */
typedef struct hiVENC_PARAM_MJPEG_VBR_S {
    HI_S32 s32ChangePos;                 /* RW; Range:[50, 100]; Indicates the ratio of the current bit rate to the maximum
                                                          bit rate when the Qfactor value starts to be adjusted */
    HI_U32 u32MaxQfactor;                /* RW; Range:[1, 99]; max image quailty allowed */
    HI_U32 u32MinQfactor;                /* RW; Range:[1, 99]; min image quality allowed ,can not be larger than u32MaxQfactor*/
} VENC_PARAM_MJPEG_VBR_S;

/* The param of h265e cbr */
typedef struct hiVENC_PARAM_H265_CBR_S {
    HI_U32  u32MinIprop;                 /* RW; Range:[1, 100]; the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                 /* RW; Range:[1, 100];the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_U32  u32MaxQp;                    /* RW; Range:[0, 51];the max QP value */
    HI_U32  u32MinQp;                    /* RW; Range:[0, 51];the min QP value ,can not be larger than u32MaxQp*/
    HI_U32  u32MaxIQp;                   /* RW; Range:[0, 51];max qp for i frame */
    HI_U32  u32MinIQp;                   /* RW; Range:[0, 51];min qp for i frame,can not be larger than u32MaxIQp */
    HI_S32  s32MaxReEncodeTimes;         /* RW; Range:[0, 3]; Range:max number of re-encode times.*/
    HI_BOOL bQpMapEn;                    /* RW; Range:[0, 1]; enable qpmap.*/
    VENC_RC_QPMAP_MODE_E enQpMapMode;    /* RW; Qpmap Mode*/
} VENC_PARAM_H265_CBR_S;

/* The param of h265e vbr */
typedef struct hiVENC_PARAM_H265_VBR_S {
    HI_S32  s32ChangePos;               /* RW; Range:[50, 100];Indicates the ratio of the current bit rate to the maximum                                             bit rate when the QP value starts to be adjusted */
    HI_U32  u32MinIprop;                /* RW; [1, 100]the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; [1, 100]the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; Range:max number of re-encode times.*/

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min P B qp ,can not be larger than u32MaxQp*/
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */

    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/
    VENC_RC_QPMAP_MODE_E enQpMapMode;   /* RW; Qpmap Mode*/
} VENC_PARAM_H265_VBR_S;

/* The param of h265e vbr */
typedef struct hiVENC_PARAM_H265_AVBR_S {
    HI_S32  s32ChangePos;               /* RW; Range:[50, 100];Indicates the ratio of the current bit rate to the maximum
                                            bit rate when the QP value starts to be adjusted */
    HI_U32  u32MinIprop;                /* RW; [1, 100]the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; [1, 100]the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; Range:max number of re-encode times.*/

    HI_S32  s32MinStillPercent;         /* RW; Range:[5, 100]; the min percent of target bitrate for still scene */
    HI_U32  u32MaxStillQP;              /* RW; Range:[0, 51]; the max QP value of I frame for still scene,
                                               can not be smaller than u32MinIQp and can not be larger than su32MaxIQp*/
    HI_U32  u32MinStillPSNR;            /* RW; reserved */

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51];the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51];the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51];the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51];the min I qp,can not be larger than u32MaxIQp */

    HI_U32  u32MinQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMinQp and MinQp, FrameLevelMinQp = MinQp(or MinIQp) + MinQpDelta  */
    HI_U32  u32MotionSensitivity;       /* RW; Range:[0, 100]; Motion Sensitivity */

    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/
    VENC_RC_QPMAP_MODE_E enQpMapMode;   /* RW; Qpmap Mode*/
} VENC_PARAM_H265_AVBR_S;

typedef struct hiVENC_PARAM_H265_QVBR_S {
    HI_U32  u32MinIprop;                /* RW; [1, 100];the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; [1, 100];the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;        /* RW; [0, 3]; max number of re-encode times [0, 3]*/

    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/
    VENC_RC_QPMAP_MODE_E enQpMapMode;   /* RW; Qpmap Mode*/

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */

    HI_S32  s32BitPercentUL;            /* RW; Range:[30, 180]; Indicate the ratio of bitrate  upper limit*/
    HI_S32  s32BitPercentLL;            /* RW; Range:[30, 180]; Indicate the ratio of bitrate  lower limit,can not be larger than s32BitPercentUL*/
    HI_S32  s32PsnrFluctuateUL;         /* RW; Range:[18, 40];  Reduce the target bitrate when the value of psnr approch the upper limit*/
    HI_S32  s32PsnrFluctuateLL;         /* RW; Range:[18, 40];  Increase the target bitrate when the value of psnr approch the lower limit,
                                                              can not be larger than s32PsnrFluctuateUL*/
} VENC_PARAM_H265_QVBR_S;

typedef struct hiVENC_PARAM_H265_CVBR_S {
    HI_U32  u32MinIprop;                /* RW; Range:[1, 100] ; the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; Range:[1, 100] ; the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; max number of re-encode times */
    HI_BOOL bQpMapEn;
    VENC_RC_QPMAP_MODE_E enQpMapMode;   /* RW; Qpmap Mode*/

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */

    HI_U32  u32MinQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMinQp and MinQp, FrameLevelMinQp = MinQp(or MinIQp) + MinQpDelta  */
    HI_U32  u32MaxQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMaxQp and MaxQp, FrameLevelMaxQp = MaxQp(or MaxIQp) - MaxQpDelta  */

    HI_U32  u32ExtraBitPercent;         /* RW; Range:[0, 1000];the extra ratio of bitrate that can be allocated when the actual bitrate goes above the long-term target bitrate*/
    HI_U32  u32LongTermStatTimeUnit;    /* RW; Range:[1, 1800]; the time unit of LongTermStatTime, the unit is senconds(s)*/
} VENC_PARAM_H265_CVBR_S;

typedef struct hiVENC_SCENE_CHANGE_DETECT_S {
    HI_BOOL bDetectSceneChange;         /* RW; Range:[0, 1]; enable detect scene change.*/
    HI_BOOL bAdaptiveInsertIDRFrame;    /* RW; Range:[0, 1]; enable a daptive insertIDR frame.*/
} VENC_SCENE_CHANGE_DETECT_S;

/* The param of rc */
typedef struct hiVENC_RC_PARAM_S {
    HI_U32 u32ThrdI[RC_TEXTURE_THR_SIZE];   /* RW; Range:[0, 255]; Mad threshold for controlling the macroblock-level bit rate of I frames */
    HI_U32 u32ThrdP[RC_TEXTURE_THR_SIZE];   /* RW; Range:[0, 255]; Mad threshold for controlling the macroblock-level bit rate of P frames */
    HI_U32 u32ThrdB[RC_TEXTURE_THR_SIZE];   /* RW; Range:[0, 255]; Mad threshold for controlling the macroblock-level bit rate of B frames */
    HI_U32 u32DirectionThrd;                /* RW; Range:[0, 16]; The direction for controlling the macroblock-level bit rate*/
    HI_U32 u32RowQpDelta;                   /* RW; Range:[0, 10];the start QP value of each macroblock row relative to the start QP value */
    HI_S32 s32FirstFrameStartQp;            /* RW; Range:[-1, 51];Start QP value of the first frame*/
    VENC_SCENE_CHANGE_DETECT_S stSceneChangeDetect;/* RW;*/
    union {
        VENC_PARAM_H264_CBR_S     stParamH264Cbr;
        VENC_PARAM_H264_VBR_S     stParamH264Vbr;
        VENC_PARAM_H264_AVBR_S    stParamH264AVbr;
        VENC_PARAM_H264_QVBR_S    stParamH264QVbr;
        VENC_PARAM_H264_CVBR_S    stParamH264CVbr;
        VENC_PARAM_H265_CBR_S     stParamH265Cbr;
        VENC_PARAM_H265_VBR_S     stParamH265Vbr;
        VENC_PARAM_H265_AVBR_S    stParamH265AVbr;
        VENC_PARAM_H265_QVBR_S    stParamH265QVbr;
        VENC_PARAM_H265_CVBR_S    stParamH265CVbr;
        VENC_PARAM_MJPEG_CBR_S    stParamMjpegCbr;
        VENC_PARAM_MJPEG_VBR_S    stParamMjpegVbr;
    };
} VENC_RC_PARAM_S;

/* the frame lost mode */
typedef enum hiVENC_FRAMELOST_MODE_E {
    FRMLOST_NORMAL=0,                               /*normal mode*/
    FRMLOST_PSKIP,                                  /*pskip*/
    FRMLOST_BUTT,
} VENC_FRAMELOST_MODE_E;

/* The param of the frame lost mode */
typedef struct hiVENC_FRAMELOST_S {
    HI_BOOL                bFrmLostOpen;          /* RW; Range:[0,1];Indicates whether to discard frames to ensure stable bit rate when the instant bit rate is exceeded */
    HI_U32                 u32FrmLostBpsThr;      /* RW; the instant bit rate threshold */
    VENC_FRAMELOST_MODE_E  enFrmLostMode;         /* RW; frame lost strategy*/
    HI_U32                 u32EncFrmGaps;         /* RW; Range:[0,65535]; the gap of frame lost*/
} VENC_FRAMELOST_S;

/* the rc priority */
typedef enum hiVENC_RC_PRIORITY_E {
    VENC_RC_PRIORITY_BITRATE_FIRST = 1,           /* bitrate first */
    VENC_RC_PRIORITY_FRAMEBITS_FIRST,             /* framebits first*/
    VENC_RC_PRIORITY_BUTT,
} VENC_RC_PRIORITY_E;

/* the config of the superframe */
typedef struct hiVENC_SUPERFRAME_CFG_S {
    VENC_SUPERFRM_MODE_E  enSuperFrmMode;          /* RW; Indicates the mode of processing the super frame*/
    HI_U32                u32SuperIFrmBitsThr;     /* RW; Range:[0, 4294967295];Indicate the threshold of the super I frame for enabling the super frame processing mode */
    HI_U32                u32SuperPFrmBitsThr;     /* RW; Range:[0, 4294967295];Indicate the threshold of the super P frame for enabling the super frame processing mode */
    HI_U32                u32SuperBFrmBitsThr;     /* RW; Range:[0, 4294967295];Indicate the threshold of the super B frame for enabling the super frame processing mode */
    VENC_RC_PRIORITY_E    enRcPriority;            /* RW; Rc Priority*/
} VENC_SUPERFRAME_CFG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_RC_H__ */
