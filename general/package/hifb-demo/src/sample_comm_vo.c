/******************************************************************************
 * Hisilicon MPP — SAMPLE_COMM_VO_StartDev / StopDev / StartLayer / StopLayer.
 *
 * Trimmed port of the upstream vendor sample `sample_comm_vo.c` to the subset
 * usable on the hi3520dv200 V2.0.5.1 SDK (Apr 2015). Only the helpers actually
 * required to bring up a VO device + video layer for hifb userspace are
 * included.
 *
 * Upstream provenance:
 *   https://github.com/Mihawk086/HI3520DVR @ hisi/common/sample_comm_vo.c
 *
 * Differences from upstream:
 *   - HDMI helper functions (HdmiConvertSync, HdmiCallbackStart, HdmiStart,
 *     HdmiStop, HdmiHotPlugEvent, HdmiUnPlugEvent, HdmiCallbackEvent) dropped:
 *     hi3520dv200 libmpi.so ships no HI_MPI_HDMI_* symbols. HDMI is brought
 *     up implicitly by VO_SetPubAttr(enIntfType=VO_INTF_HDMI) on this SDK.
 *   - Writeback (Wbc) helpers dropped: VO_WBC / VO_WBC_SOURCE_S types absent.
 *   - Snap (jpeg snapshot) helpers dropped: relies on bSupportDCF and a newer
 *     HI_MPI_VO_GetScreenFrame signature.
 *   - GetWH / StartChn / StopChn / VPSS bind helpers dropped (not needed for
 *     fb-only demo).
 *
 * Copyright (C) 2010-2011, Hisilicon Tech. Co., Ltd. (vendor original)
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_comm_vo.h"
#include "mpi_vo.h"
#include "sample_comm.h"

HI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_SetPubAttr(VoDev, pstPubAttr);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_Enable(VoDev);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_Disable(VoDev);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_SetVideoLayerAttr(VoLayer, pstLayerAttr);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}
