/******************************************************************************
 * Hisilicon MPP sample common header — minimal subset for hi3520dv200.
 *
 * Trimmed port of the upstream vendor `sample_comm.h` (Hi3531 SDK sample
 * tree, mirrored at github.com/Mihawk086/HI3520DVR/include/sample_comm.h).
 * Keeps only the constants and prototypes used by the trimmed
 * sample_comm_sys.c and sample_comm_vo.c.
 *
 * Copyright (C) 2010-2011, Hisilicon Tech. Co., Ltd. (vendor original)
 ******************************************************************************/
#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vo.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLE_PRT(fmt, ...) \
    printf("[%s]-%d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define SAMPLE_SYS_ALIGN_WIDTH  16

HI_S32  SAMPLE_COMM_SYS_Init(VB_CONF_S *pstVbConf);
HI_VOID SAMPLE_COMM_SYS_Exit(void);

HI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
HI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
HI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);

#ifdef __cplusplus
}
#endif

#endif /* __SAMPLE_COMM_H__ */
