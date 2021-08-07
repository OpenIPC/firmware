/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ae.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2014/04/01
  Description   :
  History       :
  1.Date        : 2014/04/01
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __MPI_AE_H__
#define __MPI_AE_H__

#include "hi_comm_isp.h"
#include "hi_comm_3a.h"
#include "hi_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


/* The interface of ae lib register to isp. */
HI_S32 HI_MPI_AE_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);
HI_S32 HI_MPI_AE_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);

/* The callback function of sensor register to ae lib. */
HI_S32 HI_MPI_AE_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
                                   AE_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_AE_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);

HI_S32 HI_MPI_ISP_SetExposureAttr(VI_PIPE ViPipe, const ISP_EXPOSURE_ATTR_S *pstExpAttr);
HI_S32 HI_MPI_ISP_GetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExpAttr);

HI_S32 HI_MPI_ISP_SetWDRExposureAttr(VI_PIPE ViPipe, const ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);
HI_S32 HI_MPI_ISP_GetWDRExposureAttr(VI_PIPE ViPipe, ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);

HI_S32 HI_MPI_ISP_SetHDRExposureAttr(VI_PIPE ViPipe, const ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr);
HI_S32 HI_MPI_ISP_GetHDRExposureAttr(VI_PIPE ViPipe, ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr);

HI_S32 HI_MPI_ISP_SetAERouteAttr(VI_PIPE ViPipe, const ISP_AE_ROUTE_S *pstAERouteAttr);
HI_S32 HI_MPI_ISP_GetAERouteAttr(VI_PIPE ViPipe, ISP_AE_ROUTE_S *pstAERouteAttr);

HI_S32 HI_MPI_ISP_SetAERouteSFAttr(VI_PIPE ViPipe, const ISP_AE_ROUTE_S *pstAERouteSFAttr);
HI_S32 HI_MPI_ISP_GetAERouteSFAttr(VI_PIPE ViPipe, ISP_AE_ROUTE_S *pstAERouteSFAttr);

HI_S32 HI_MPI_ISP_QueryExposureInfo(VI_PIPE ViPipe, ISP_EXP_INFO_S *pstExpInfo);

HI_S32 HI_MPI_ISP_SetIrisAttr(VI_PIPE ViPipe, const ISP_IRIS_ATTR_S *pstIrisAttr);
HI_S32 HI_MPI_ISP_GetIrisAttr(VI_PIPE ViPipe, ISP_IRIS_ATTR_S *pstIrisAttr);

HI_S32 HI_MPI_ISP_SetDcirisAttr(VI_PIPE ViPipe, const ISP_DCIRIS_ATTR_S *pstDcirisAttr);
HI_S32 HI_MPI_ISP_GetDcirisAttr(VI_PIPE ViPipe, ISP_DCIRIS_ATTR_S *pstDcirisAttr);

HI_S32 HI_MPI_ISP_SetPirisAttr(VI_PIPE ViPipe, const ISP_PIRIS_ATTR_S *pstPirisAttr);
HI_S32 HI_MPI_ISP_GetPirisAttr(VI_PIPE ViPipe, ISP_PIRIS_ATTR_S *pstPirisAttr);

HI_S32 HI_MPI_ISP_SetAERouteAttrEx(VI_PIPE ViPipe, const ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);
HI_S32 HI_MPI_ISP_GetAERouteAttrEx(VI_PIPE ViPipe, ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);

HI_S32 HI_MPI_ISP_SetAERouteSFAttrEx(VI_PIPE ViPipe, const ISP_AE_ROUTE_EX_S *pstAERouteSFAttrEx);
HI_S32 HI_MPI_ISP_GetAERouteSFAttrEx(VI_PIPE ViPipe, ISP_AE_ROUTE_EX_S *pstAERouteSFAttrEx);

HI_S32 HI_MPI_ISP_SetSmartExposureAttr(VI_PIPE ViPipe, const ISP_SMART_EXPOSURE_ATTR_S *pstSmartExpAttr);
HI_S32 HI_MPI_ISP_GetSmartExposureAttr(VI_PIPE ViPipe, ISP_SMART_EXPOSURE_ATTR_S *pstSmartExpAttr);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
