/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: mpi_awb.h
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

#ifndef __MPI_AWB_H__
#define __MPI_AWB_H__

#include "xm_comm_isp.h"
#include "xm_comm_3a.h"
#include "xm_awb_comm.h"


/* The interface of awb lib register to isp. */
XM_S32 XM_MPI_AWB_Register(ALG_LIB_S *pstAwbLib);
XM_S32 XM_MPI_AWB_UnRegister(ALG_LIB_S *pstAwbLib);

/* The callback function of sensor register to awb lib. */
XM_S32 XM_MPI_AWB_SensorRegCallBack(ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId,
    AWB_SENSOR_REGISTER_S *pstRegister);
XM_S32 XM_MPI_AWB_SensorUnRegCallBack(ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId);

XM_S32 XM_MPI_ISP_AWBLibRegCallBack(ALG_LIB_S *pstAWBLib,
        ISP_AWB_REGISTER_S *pstRegister);


XM_S32 XM_MPI_ISP_SetWBAttr(const ISP_WB_ATTR_S *pstWBAttr);
XM_S32 XM_MPI_ISP_GetWBAttr(ISP_WB_ATTR_S *pstWBAttr);

XM_S32 XM_MPI_ISP_SetCCMAttr(const ISP_COLORMATRIX_ATTR_S *pstCCMAttr);
XM_S32 XM_MPI_ISP_GetCCMAttr(ISP_COLORMATRIX_ATTR_S *pstCCMAttr);

XM_S32 XM_MPI_ISP_QueryWBInfo(ISP_WB_INFO_S *pstWBInfo);
XM_S32 XM_MPI_ISP_QueryWBInfo2(ISP_WB_INFO_S_V2 *pstWBInfo);
XM_S32 XM_MPI_AWB_GetStatistics(ISP_DEV IspDev, ISP_WB_BAYER_STATISTICS_S *pstStat);

XM_S32 XM_MPI_ISP_SetWBCalAttr(const ISP_AWB_CALIBRATION_V2_S *pstWBCalAttr);
XM_S32 XM_MPI_ISP_GetWBCalAttr(ISP_AWB_CALIBRATION_V2_S *pstWBCalAttr);


#endif

