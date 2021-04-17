/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_awb.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/19
  Description   :
  History       :
  1.Date        : 2012/12/19
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __MPI_AWB_H__
#define __MPI_AWB_H__

#include "hi_comm_isp.h"
#include "hi_comm_3a.h"
#include "hi_awb_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* The interface of awb lib register to isp. */
HI_S32 HI_MPI_AWB_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);
HI_S32 HI_MPI_AWB_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

/* The callback function of sensor register to awb lib. */
HI_S32 HI_MPI_AWB_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
                                    AWB_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_AWB_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId);

HI_S32 HI_MPI_ISP_SetWBAttr(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWBAttr);
HI_S32 HI_MPI_ISP_GetWBAttr(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWBAttr);

HI_S32 HI_MPI_ISP_SetAWBAttrEx(VI_PIPE ViPipe, ISP_AWB_ATTR_EX_S *pstAWBAttrEx);
HI_S32 HI_MPI_ISP_GetAWBAttrEx(VI_PIPE ViPipe, ISP_AWB_ATTR_EX_S *pstAWBAttrEx);

HI_S32 HI_MPI_ISP_SetCCMAttr(VI_PIPE ViPipe, const ISP_COLORMATRIX_ATTR_S *pstCCMAttr);
HI_S32 HI_MPI_ISP_GetCCMAttr(VI_PIPE ViPipe, ISP_COLORMATRIX_ATTR_S *pstCCMAttr);

HI_S32 HI_MPI_ISP_SetSaturationAttr(VI_PIPE ViPipe, const ISP_SATURATION_ATTR_S *pstSatAttr);
HI_S32 HI_MPI_ISP_GetSaturationAttr(VI_PIPE ViPipe, ISP_SATURATION_ATTR_S *pstSatAttr);

HI_S32 HI_MPI_ISP_QueryWBInfo(VI_PIPE ViPipe, ISP_WB_INFO_S *pstWBInfo);
HI_S32 HI_MPI_ISP_CalGainByTemp(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWBAttr, HI_U16 u16ColorTemp, HI_S16 s16Shift, HI_U16 *pu16AWBGain);
HI_S32 HI_MPI_ISP_SetSpecAwbAttr(VI_PIPE ViPipe, const ISP_SPECAWB_ATTR_S *pstSpecAwbAttr);
HI_S32 HI_MPI_ISP_SetCaaControlAttr(VI_PIPE ViPipe, const ISP_SPECAWB_CAA_CONTROl_S *pstSpecAwbCaaAttr);
HI_S32 HI_MPI_ISP_GetCaaControlAttr(VI_PIPE ViPipe, ISP_SPECAWB_CAA_CONTROl_S *pstSpecAwbCaaAttr);
HI_S32 HI_MPI_ISP_GetSpecAwbAttr(VI_PIPE ViPipe, ISP_SPECAWB_ATTR_S *pstSpecAwbAttr);
HI_S32 HI_MPI_ISP_SetSpecAwbControlAttr(VI_PIPE ViPipe, ISP_SPECAWB_CONTROL_ATTR_S *pstSpecAwbControlAttr);
HI_S32 HI_MPI_ISP_GetSpecAwbControlAttr(VI_PIPE ViPipe, ISP_SPECAWB_CONTROL_ATTR_S *pstSpecAwbControlAttr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
