/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vpss.h
  Version       : Initial Draft
  Author        : 
  Created       : 
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
  1.Date        : 20110616
    Author      : l00183122
    Modification: Create
******************************************************************************/
#ifndef __MPI_VPSS_H__
#define __MPI_VPSS_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


HI_S32 HI_MPI_VPSS_CreateGrp(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_StartGrp(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_StopGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_ResetGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN s32VpssChnl);
HI_S32 HI_MPI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN s32VpssChnl);

HI_S32 HI_MPI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);

HI_S32 HI_MPI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VPSS_SetGrpParam(VPSS_GRP VpssGrp, VPSS_GRP_PARAM_S *pstVpssParam);
HI_S32 HI_MPI_VPSS_GetGrpParam(VPSS_GRP VpssGrp, VPSS_GRP_PARAM_S *pstVpssParam);

HI_S32 HI_MPI_VPSS_SetCropCfg(VPSS_GRP VpssGrp,  VPSS_CROP_INFO_S *pstCropInfo);
HI_S32 HI_MPI_VPSS_GetCropCfg(VPSS_GRP VpssGrp,  VPSS_CROP_INFO_S *pstCropInfo);

HI_S32 HI_MPI_VPSS_SetChnMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_MODE_S *pstVpssMode);
HI_S32 HI_MPI_VPSS_GetChnMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_MODE_S *pstVpssMode);

HI_S32 HI_MPI_VPSS_SetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32Depth);
HI_S32 HI_MPI_VPSS_GetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 *pu32Depth);

HI_S32 HI_MPI_VPSS_UserSendFrame(VPSS_GRP VpssGrp,  VIDEO_FRAME_INFO_S *pstVideoFrame);
HI_S32 HI_MPI_VPSS_UserSendFrameTimeout(VPSS_GRP VpssGrp,  VIDEO_FRAME_INFO_S *pstVideoFrame, HI_U32 u32MilliSec);

HI_S32 HI_MPI_VPSS_UserGetFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,  VIDEO_FRAME_INFO_S *pstVideoFrame);
HI_S32 HI_MPI_VPSS_UserReleaseFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,  VIDEO_FRAME_INFO_S *pstVideoFrame);

HI_S32 HI_MPI_VPSS_UserGetGrpFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_U32 u32FrameIndex);
HI_S32 HI_MPI_VPSS_UserReleaseGrpFrame(VPSS_GRP VpssGrp,  VIDEO_FRAME_INFO_S *pstVideoFrame);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*set&get vpss group prescale information.*/
HI_S32 HI_MPI_VPSS_SetPreScale(VPSS_GRP VpssGrp,VPSS_PRESCALE_INFO_S *pstPreScaleInfo);
HI_S32 HI_MPI_VPSS_GetPreScale(VPSS_GRP VpssGrp,VPSS_PRESCALE_INFO_S *pstPreScaleInfo);

/*set&get vpss group picture size filter information*/
HI_S32 HI_MPI_VPSS_SetGrpSizer(VPSS_GRP VpssGrp, VPSS_SIZER_INFO_S *pstVpssSizerInfo);
HI_S32 HI_MPI_VPSS_GetGrpSizer(VPSS_GRP VpssGrp, VPSS_SIZER_INFO_S *pstVpssSizerInfo);

HI_S32 HI_MPI_VPSS_SetDelay(VPSS_GRP VpssGrp, HI_U32 u32Delay);
HI_S32 HI_MPI_VPSS_GetDelay(VPSS_GRP VpssGrp, HI_U32 *pu32Delay);

/*set&get vpss group source picture framerate ctrl information*/
HI_S32 HI_MPI_VPSS_SetGrpFrameRate(VPSS_GRP VpssGrp, VPSS_FRAME_RATE_S *pstVpssFrameRate);
HI_S32 HI_MPI_VPSS_GetGrpFrameRate(VPSS_GRP VpssGrp, VPSS_FRAME_RATE_S *pstVpssFrameRate);

/*set&get vpss channel Nr(Noise Reduction) parameters.*/
HI_S32 HI_MPI_VPSS_SetChnNrParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_NR_PARAM_S *pstChnNrParam);
HI_S32 HI_MPI_VPSS_GetChnNrParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_NR_PARAM_S *pstChnNrParam);

/*set&get vpss channel sharpen parameters.*/
HI_S32 HI_MPI_VPSS_SetChnSpParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_SP_PARAM_S *pstChnSpParam);
HI_S32 HI_MPI_VPSS_GetChnSpParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_SP_PARAM_S *pstChnSpParam);

/*set&get vpss channel field information.*/
HI_S32 HI_MPI_VPSS_SetChnField(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CAPSEL_E enCapSel);
HI_S32 HI_MPI_VPSS_GetChnField(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CAPSEL_E *enCapSel);

/*set&get vpss group field information.*/
HI_S32 HI_MPI_VPSS_SetGrpField(VPSS_GRP VpssGrp, VPSS_CAPSEL_E enCapSel);
HI_S32 HI_MPI_VPSS_GetGrpField(VPSS_GRP VpssGrp, VPSS_CAPSEL_E *penCapSel);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*set&get vpss extend channel attributes*/
HI_S32 HI_MPI_VPSS_SetExtChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_EXT_CHN_ATTR_S *pstExtChnAttr);
HI_S32 HI_MPI_VPSS_GetExtChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_EXT_CHN_ATTR_S *pstExtChnAttr);

/*set&get vpss group picture rotate information*/
//HI_S32 HI_MPI_VPSS_SetRotate(VPSS_GRP VpssGrp, ROTATE_E enRotate);
//HI_S32 HI_MPI_VPSS_GetRotate(VPSS_GRP VpssGrp, ROTATE_E *penRotate);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */

