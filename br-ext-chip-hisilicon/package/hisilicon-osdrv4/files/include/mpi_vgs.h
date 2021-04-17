/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: mpi functions declaration
 * Author: Hisilicon multimedia software group
 * Create: 2016-07-24
 */

#ifndef __MPI_VGS_H__
#define __MPI_VGS_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vgs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_VGS_BeginJob(VGS_HANDLE *phHandle);

HI_S32 HI_MPI_VGS_EndJob(VGS_HANDLE hHandle);

HI_S32 HI_MPI_VGS_CancelJob(VGS_HANDLE hHandle);

HI_S32 HI_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask,
                               VGS_SCLCOEF_MODE_E enScaleCoefMode);

HI_S32 HI_MPI_VGS_AddDrawLineTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask,
                                  const VGS_DRAW_LINE_S *pstVgsDrawLine);

HI_S32 HI_MPI_VGS_AddCoverTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask,
                               const VGS_ADD_COVER_S *pstVgsAddCover);

HI_S32 HI_MPI_VGS_AddOsdTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask,
                             const VGS_ADD_OSD_S *pstVgsAddOsd);

HI_S32 HI_MPI_VGS_AddDrawLineTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask,
                                       const VGS_DRAW_LINE_S astVgsDrawLine[], HI_U32 u32ArraySize);

HI_S32 HI_MPI_VGS_AddCoverTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask,
                                    const VGS_ADD_COVER_S astVgsAddCover[], HI_U32 u32ArraySize);

HI_S32 HI_MPI_VGS_AddOsdTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask,
                                  const VGS_ADD_OSD_S astVgsAddOsd[], HI_U32 u32ArraySize);

HI_S32 HI_MPI_VGS_AddRotationTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, ROTATION_E enRotationAngle);

HI_S32 HI_MPI_VGS_AddLumaTaskArray(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, const RECT_S astVgsLumaRect[],
                                   HI_U32 u32ArraySize, HI_U64 au64LumaData[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* end of __MPI_VGS_H__ */

