/******************************************************************************
 * Hisilicon MPP — SAMPLE_COMM_SYS_Init / SAMPLE_COMM_SYS_Exit bootstrap.
 *
 * Trimmed port of the upstream vendor sample `sample_comm_sys.c` to the subset
 * usable on the hi3520dv200 V2.0.5.1 SDK (Apr 2015). Only the helpers actually
 * required to bring up the MPP system enough for hifb userspace are included.
 *
 * Upstream provenance:
 *   https://github.com/Mihawk086/HI3520DVR @ hisi/common/sample_comm_sys.c
 *   (originally from Hisilicon Hi3531 SDK sample tree; the same boilerplate
 *   ships across the Hi35xx MPP family.)
 *
 * Differences from upstream:
 *   - HI_MPI_VB_ExitModCommPool loop dropped — symbol does not exist in
 *     hi3520dv200 libmpi.so (added in a later SDK revision).
 *   - All other SAMPLE_COMM_SYS_* helpers (GetPicSize, MemConfig,
 *     Payload2FilePostfix, CalcPicVbBlkSize) dropped — they pull in
 *     COMPRESS_MODE_E / PIXEL_FORMAT_E enums that the V2.0.5.1 headers
 *     don't define.
 *
 * Copyright (C) 2010-2011, Hisilicon Tech. Co., Ltd. (vendor original)
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "sample_comm.h"

HI_S32 SAMPLE_COMM_SYS_Init(VB_CONF_S *pstVbConf)
{
    MPP_SYS_CONF_S stSysConf = {0};
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 i;

    HI_MPI_SYS_Exit();

    for (i = 0; i < VB_MAX_POOLS; i++) {
        HI_MPI_VB_DestroyPool(i);
    }
    HI_MPI_VB_Exit();

    if (NULL == pstVbConf) {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_SetConf(pstVbConf);
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("HI_MPI_VB_SetConf failed!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_Init();
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("HI_MPI_VB_Init failed!\n");
        return HI_FAILURE;
    }

    stSysConf.u32AlignWidth = SAMPLE_SYS_ALIGN_WIDTH;
    s32Ret = HI_MPI_SYS_SetConf(&stSysConf);
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("HI_MPI_SYS_SetConf failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("HI_MPI_SYS_Init failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_VOID SAMPLE_COMM_SYS_Exit(void)
{
    HI_S32 i;

    HI_MPI_SYS_Exit();

    for (i = 0; i < VB_MAX_POOLS; i++) {
        HI_MPI_VB_DestroyPool(i);
    }
    HI_MPI_VB_Exit();
}
