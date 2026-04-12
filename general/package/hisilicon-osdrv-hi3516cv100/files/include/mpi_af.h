/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_af.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/19
  Description   : 
  History       :
  1.Date        : 2012/12/19
    Author      : n00168968
    Modification: Created file

******************************************************************************/
#ifndef __MPI_AF_H__
#define __MPI_AF_H__

#include "hi_comm_isp.h"
#include "hi_comm_3a.h"
#include "hi_af_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* The interface of af lib register to isp. */
HI_S32 HI_MPI_AF_Register(ALG_LIB_S *pstAfLib);
HI_S32 HI_MPI_AF_UnRegister(ALG_LIB_S *pstAfLib);

#if 0
/* The callback function of sensor register to af lib. */
HI_S32 hi_af_sensor_register_cb(ALG_LIB_S *pstAfLib, SENSOR_ID SensorId,
    CMOS_ISP_AF_DEFAULT_S *pstSnsDft, SENSOR_AF_EXP_FUNC_S *pstSnsExp);
#endif

/* The new awb lib is compatible with the old mpi interface. */
HI_S32 HI_MPI_ISP_SetFocusType(ISP_OP_TYPE_E enFocusType);    //not support yet
HI_S32 HI_MPI_ISP_GetFocusType(ISP_OP_TYPE_E *penFocusType);  //not support yet

HI_S32 HI_MPI_ISP_SetAFAttr(const ISP_AF_ATTR_S *pstAFAttr);    //not support yet
HI_S32 HI_MPI_ISP_GetAFAttr(ISP_AF_ATTR_S *pstAFAttr);          //not support yet

HI_S32 HI_MPI_ISP_SetMFAttr(const ISP_MF_ATTR_S *pstMFAttr);    //not support yet
HI_S32 HI_MPI_ISP_GetMFAttr(ISP_MF_ATTR_S *pstMFAttr);         //not support yet

HI_S32 HI_MPI_ISP_ManualFocusMove(HI_S32 s32MoveSteps);    //not support yet

HI_S32 HI_MPI_ISP_SetFocusStaInfo(const ISP_FOCUS_STA_INFO_S *pstFocusStatistic);
HI_S32 HI_MPI_ISP_GetFocusStaInfo(ISP_FOCUS_STA_INFO_S *pstFocusStatistic);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

