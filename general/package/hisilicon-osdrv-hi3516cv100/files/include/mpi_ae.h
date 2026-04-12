/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ae.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/19
  Description   : 
  History       :
  1.Date        : 2012/12/19
    Author      : n00168968
    Modification: Created file

******************************************************************************/
#ifndef __MPI_AE_H__
#define __MPI_AE_H__

#include "hi_comm_isp.h"
#include "hi_comm_3a.h"
#include "hi_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* The interface of ae lib register to isp. */
HI_S32 HI_MPI_AE_Register(ALG_LIB_S *pstAeLib);
HI_S32 HI_MPI_AE_UnRegister(ALG_LIB_S *pstAeLib);

/* The callback function of sensor register to ae lib. */
HI_S32 HI_MPI_AE_SensorRegCallBack(ALG_LIB_S *pstAeLib, SENSOR_ID SensorId,
    AE_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_AE_SensorUnRegCallBack(ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);

/* The new ae lib is compatible with the old mpi interface. */
HI_S32 HI_MPI_ISP_SetExposureType(ISP_OP_TYPE_E enExpType);
HI_S32 HI_MPI_ISP_GetExposureType(ISP_OP_TYPE_E *penExpType);

HI_S32 HI_MPI_ISP_SetAEAttr(const ISP_AE_ATTR_S *pstAEAttr);
HI_S32 HI_MPI_ISP_GetAEAttr(ISP_AE_ATTR_S *pstAEAttr);

HI_S32 HI_MPI_ISP_SetAEAttrEx(const ISP_AE_ATTR_EX_S *pstAEAttrEx);
HI_S32 HI_MPI_ISP_GetAEAttrEx(ISP_AE_ATTR_EX_S *pstAEAttrEx);

HI_S32 HI_MPI_ISP_SetExpStaInfo(ISP_EXP_STA_INFO_S *pstExpStatistic);
HI_S32 HI_MPI_ISP_GetExpStaInfo(ISP_EXP_STA_INFO_S *pstExpStatistic);

HI_S32 HI_MPI_ISP_SetMEAttr(const ISP_ME_ATTR_S *pstMEAttr);
HI_S32 HI_MPI_ISP_GetMEAttr(ISP_ME_ATTR_S *pstMEAttr);

HI_S32 HI_MPI_ISP_SetMEAttrEx(const ISP_ME_ATTR_EX_S *pstMEAttrEx);
HI_S32 HI_MPI_ISP_GetMEAttrEx(ISP_ME_ATTR_EX_S *pstMEAttrEx);

HI_S32 HI_MPI_ISP_SetSlowFrameRate(HI_U8 u8Value);
HI_S32 HI_MPI_ISP_GetSlowFrameRate(HI_U8 *pu8Value);

HI_S32 HI_MPI_ISP_SetAntiFlickerAttr(const ISP_ANTIFLICKER_S *pstAntiflicker);
HI_S32 HI_MPI_ISP_GetAntiFlickerAttr(ISP_ANTIFLICKER_S *pstAntiflicker);

HI_S32 HI_MPI_ISP_SetSubFlickerAttr(const ISP_SUBFLICKER_S *pstSubflicker);
HI_S32 HI_MPI_ISP_GetSubFlickerAttr(ISP_SUBFLICKER_S *pstSubflicker);

HI_S32 HI_MPI_ISP_SetDcirisAttr(const ISP_DCIRIS_ATTR_S *pstDcirisAttr);
HI_S32 HI_MPI_ISP_GetDcirisAttr(ISP_DCIRIS_ATTR_S *pstDcirisAttr);

HI_S32 HI_MPI_ISP_QueryInnerStateInfo(ISP_INNER_STATE_INFO_S *pstInnerStateInfo);
HI_S32 HI_MPI_ISP_QueryInnerStateInfoEx(ISP_INNER_STATE_INFO_EX_S *pstInnerStateInfoEx);

HI_S32 HI_MPI_ISP_SetIrisType(ISP_OP_TYPE_E enIrisType);        //not support yet
HI_S32 HI_MPI_ISP_GetIrisType(ISP_OP_TYPE_E *penIrisType);      //not support yet

HI_S32 HI_MPI_ISP_SetAIAttr(const ISP_AI_ATTR_S *pstAIAttr);
HI_S32 HI_MPI_ISP_GetAIAttr(ISP_AI_ATTR_S *pstAIAttr);

HI_S32 HI_MPI_ISP_SetMIAttr(const ISP_MI_ATTR_S *pstMIAttr);    //not support yet
HI_S32 HI_MPI_ISP_GetMIAttr(ISP_MI_ATTR_S *pstMIAttr);          //not support yet

HI_S32 HI_MPI_ISP_SetAERouteAttr(const ISP_AE_ROUTE_S *pstAERouteAttr);
HI_S32 HI_MPI_ISP_GetAERouteAttr(ISP_AE_ROUTE_S *pstAERouteAttr);

HI_S32 HI_MPI_ISP_SetAEDelayAttr(const ISP_AE_DELAY_S *pstAEDelayAttr);
HI_S32 HI_MPI_ISP_GetAEDelayAttr(ISP_AE_DELAY_S *pstAEDelayAttr);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

