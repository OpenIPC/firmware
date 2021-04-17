/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: mpi_ae.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/6/27

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/6/27
  Author		: Lycai
  Modification	: creat
******************************************************************************/

#ifndef __MPI_AE_H__
#define __MPI_AE_H__

#include "xm_comm_isp.h"
#include "xm_comm_3a.h"
#include "xm_ae_comm.h"



/* The interface of ae lib register to isp. */
XM_S32 XM_MPI_AE_Register(ISP_DEV IspDev, ALG_LIB_S *pstAeLib);
XM_S32 XM_MPI_AE_UnRegister(ISP_DEV IspDev, ALG_LIB_S *pstAeLib);

/* The callback function of sensor register to ae lib. */
XM_S32 XM_MPI_ISP_AELibRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib, ISP_AE_REGISTER_S *pstRegister);
XM_S32 XM_MPI_ISP_AELibUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib);

XM_S32 XM_MPI_AE_SensorRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId,
    													AE_SENSOR_REGISTER_S *pstRegister);
XM_S32 XM_MPI_AE_SensorUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);

XM_S32 XM_MPI_ISP_SetExposureAttr(ISP_DEV IspDev, const ISP_EXPOSURE_ATTR_S *pstExpAttr);
XM_S32 XM_MPI_ISP_GetExposureAttr(ISP_DEV IspDev, ISP_EXPOSURE_ATTR_S *pstExpAttr);

XM_S32 XM_MPI_ISP_SetAERouteAttr(ISP_DEV IspDev, const ISP_AE_ROUTE_S *pstAERouteAttr);
XM_S32 XM_MPI_ISP_GetAERouteAttr(ISP_DEV IspDev, ISP_AE_ROUTE_S *pstAERouteAttr);

XM_S32 XM_MPI_ISP_QueryInnerStateInfo(ISP_DEV IspDev, ISP_INNER_STATE_INFO_S *pstInnerStateInfo);

XM_S32 XM_MPI_ISP_SetAICalibrate(ISP_DEV IspDev, const ISP_AI_CALIBRATE_S *pstAICalibrate);
XM_S32 XM_MPI_ISP_GetAICalibrate(ISP_DEV IspDev, ISP_AI_CALIBRATE_S *pstAICalibrate);

XM_S32 XM_MPI_ISP_SetIrisAttr(ISP_DEV IspDev, const ISP_IRIS_ATTR_S *pstIrisAttr);
XM_S32 XM_MPI_ISP_GetIrisAttr(ISP_DEV IspDev, ISP_IRIS_ATTR_S *pstIrisAttr);


XM_S32 XM_MPI_ISP_SetExpStaInfo(ISP_DEV IspDev, ISP_EXP_STA_INFO_S *pstExpStatistic);
XM_S32 XM_MPI_ISP_GetExpStaInfo(ISP_DEV IspDev, ISP_EXP_STA_INFO_S *pstExpStatistic);

XM_S32 XM_MPI_ISP_SetSlowFrameRate(ISP_DEV IspDev, XM_U8 u8Value);
XM_S32 XM_MPI_ISP_GetSlowFrameRate(ISP_DEV IspDev, XM_U8 *pu8Value);


XM_S32 XM_MPI_ISP_GetStatisticsAE(ISP_DEV IspDev, ISP_AE_STAT_S *pstAeStat);
XM_S32 XM_MPI_ISP_GetStatisticsAEV2(ISP_DEV IspDev, ISP_AE_STAT_3_S *pstAeStatV2);

XM_S32 XM_MPI_AE_SetFrameRate(ISP_DEV IspDev, XM_U8 u8Fps);

//可调节光补光LED
XM_S32 XM_MPI_AE_SetVarLEDAttr(ISP_DEV IspDev,const ISP_AE_VARLED_REGISTER_S *pstVarLED);
XM_S32 XM_MPI_AE_GetVarLEDAttr(ISP_DEV IspDev, ISP_AE_VARLED_REGISTER_S *pstVarLED);

#endif

