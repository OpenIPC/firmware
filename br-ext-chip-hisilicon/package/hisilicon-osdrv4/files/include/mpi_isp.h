/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_isp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/12/20
  Description   :
  History       :
  1.Date        : 2010/12/20
    Author      :
    Modification: Created file

******************************************************************************/

#ifndef __MPI_ISP_H__
#define __MPI_ISP_H__

#include "hi_comm_isp.h"
#include "hi_comm_sns.h"
#include "hi_comm_3a.h"
#include "hi_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* Firmware Main Operation */
HI_S32 HI_MPI_ISP_Init(VI_PIPE ViPipe);
HI_S32 HI_MPI_ISP_MemInit(VI_PIPE ViPipe);
HI_S32 HI_MPI_ISP_Run(VI_PIPE ViPipe);
HI_S32 HI_MPI_ISP_RunOnce(VI_PIPE ViPipe);
HI_S32 HI_MPI_ISP_Exit(VI_PIPE ViPipe);

HI_S32 HI_MPI_ISP_SensorRegCallBack(VI_PIPE ViPipe, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, ISP_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_SensorUnRegCallBack(VI_PIPE ViPipe, SENSOR_ID SensorId);

/* if have registered multy libs, set bind attr to appoint the active lib. */
HI_S32 HI_MPI_ISP_SetBindAttr(VI_PIPE ViPipe, const ISP_BIND_ATTR_S *pstBindAttr);
HI_S32 HI_MPI_ISP_GetBindAttr(VI_PIPE ViPipe, ISP_BIND_ATTR_S *pstBindAttr);
HI_S32 HI_MPI_ISP_AELibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib,
                                   ISP_AE_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AWBLibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib,
                                    ISP_AWB_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AELibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);
HI_S32 HI_MPI_ISP_AWBLibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

HI_S32 HI_MPI_ISP_GetLightboxGain(VI_PIPE ViPipe, ISP_AWB_Calibration_Gain_S *pstAWBCalibrationGain);
HI_S32 HI_MPI_ISP_SetPubAttr(VI_PIPE ViPipe, const ISP_PUB_ATTR_S *pstPubAttr);
HI_S32 HI_MPI_ISP_GetPubAttr(VI_PIPE ViPipe, ISP_PUB_ATTR_S *pstPubAttr);

HI_S32 HI_MPI_ISP_SetPipeDifferAttr(VI_PIPE ViPipe, const ISP_PIPE_DIFF_ATTR_S *pstPipeDiffer);
HI_S32 HI_MPI_ISP_GetPipeDifferAttr(VI_PIPE ViPipe, ISP_PIPE_DIFF_ATTR_S *pstPipeDiffer);

HI_S32 HI_MPI_ISP_GetVDTimeOut(VI_PIPE ViPipe, ISP_VD_TYPE_E enIspVDType, HI_U32 u32MilliSec);
HI_S32 HI_MPI_ISP_QueryInnerStateInfo(VI_PIPE ViPipe, ISP_INNER_STATE_INFO_S *pstInnerStateInfo);

HI_S32 HI_MPI_ISP_GetAEStatistics(VI_PIPE ViPipe, ISP_AE_STATISTICS_S *pstAeStat);
HI_S32 HI_MPI_ISP_GetAEStitchStatistics(VI_PIPE ViPipe, ISP_AE_STITCH_STATISTICS_S *pstStitchStat);
HI_S32 HI_MPI_ISP_GetMGStatistics(VI_PIPE ViPipe, ISP_MG_STATISTICS_S *pstMgStat);
HI_S32 HI_MPI_ISP_GetWBStatistics(VI_PIPE ViPipe, ISP_WB_STATISTICS_S *pstWBStat);
HI_S32 HI_MPI_ISP_GetWBStitchStatistics(VI_PIPE ViPipe, ISP_WB_STITCH_STATISTICS_S *pstStitchWBStat);
HI_S32 HI_MPI_ISP_GetFocusStatistics(VI_PIPE ViPipe, ISP_AF_STATISTICS_S *pstAfStat);

HI_S32 HI_MPI_ISP_SetStatisticsConfig(VI_PIPE ViPipe, const ISP_STATISTICS_CFG_S *pstStatCfg);
HI_S32 HI_MPI_ISP_GetStatisticsConfig(VI_PIPE ViPipe, ISP_STATISTICS_CFG_S *pstStatCfg);

HI_S32 HI_MPI_ISP_GetISPRegAttr(VI_PIPE ViPipe, ISP_REG_ATTR_S *pstIspRegAttr);

HI_S32 HI_MPI_ISP_SetFMWState(VI_PIPE ViPipe, const ISP_FMW_STATE_E enState);
HI_S32 HI_MPI_ISP_GetFMWState(VI_PIPE ViPipe, ISP_FMW_STATE_E *penState);

HI_S32 HI_MPI_ISP_SetDebug(VI_PIPE ViPipe, const ISP_DEBUG_INFO_S *pstIspDebug);
HI_S32 HI_MPI_ISP_GetDebug(VI_PIPE ViPipe, ISP_DEBUG_INFO_S *pstIspDebug);

HI_S32 HI_MPI_ISP_SetModParam(const ISP_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_ISP_GetModParam(ISP_MOD_PARAM_S *pstModParam);

HI_S32 HI_MPI_ISP_SetCtrlParam(VI_PIPE ViPipe, const ISP_CTRL_PARAM_S *pstIspCtrlParam);
HI_S32 HI_MPI_ISP_GetCtrlParam(VI_PIPE ViPipe, ISP_CTRL_PARAM_S *pstIspCtrlParam);

HI_S32 HI_MPI_ISP_SetFSWDRAttr(VI_PIPE ViPipe, const ISP_WDR_FS_ATTR_S *pstFSWDRAttr);
HI_S32 HI_MPI_ISP_GetFSWDRAttr(VI_PIPE ViPipe, ISP_WDR_FS_ATTR_S *pstFSWDRAttr);

HI_S32 HI_MPI_ISP_SetDRCAttr(VI_PIPE ViPipe, const ISP_DRC_ATTR_S *pstDRC);
HI_S32 HI_MPI_ISP_GetDRCAttr(VI_PIPE ViPipe, ISP_DRC_ATTR_S *pstDRC);

HI_S32 HI_MPI_ISP_SetDehazeAttr(VI_PIPE ViPipe, const ISP_DEHAZE_ATTR_S *pstDehazeAttr);
HI_S32 HI_MPI_ISP_GetDehazeAttr(VI_PIPE ViPipe, ISP_DEHAZE_ATTR_S *pstDehazeAttr);

HI_S32 HI_MPI_ISP_SetLDCIAttr(VI_PIPE ViPipe, const ISP_LDCI_ATTR_S *pstLDCIAttr);
HI_S32 HI_MPI_ISP_GetLDCIAttr(VI_PIPE ViPipe, ISP_LDCI_ATTR_S *pstLDCIAttr);

HI_S32 HI_MPI_ISP_SetSnsSlaveAttr(SLAVE_DEV SlaveDev, const ISP_SLAVE_SNS_SYNC_S *pstSnsSync);
HI_S32 HI_MPI_ISP_GetSnsSlaveAttr(SLAVE_DEV SlaveDev, ISP_SLAVE_SNS_SYNC_S *pstSnsSync);

HI_S32 HI_MPI_ISP_SetModuleControl(VI_PIPE ViPipe, const ISP_MODULE_CTRL_U *punModCtrl);
HI_S32 HI_MPI_ISP_GetModuleControl(VI_PIPE ViPipe, ISP_MODULE_CTRL_U *punModCtrl);

HI_S32 HI_MPI_ISP_SetDPCalibrate(VI_PIPE ViPipe, const ISP_DP_STATIC_CALIBRATE_S *pstDPCalibrate);
HI_S32 HI_MPI_ISP_GetDPCalibrate(VI_PIPE ViPipe, ISP_DP_STATIC_CALIBRATE_S *pstDPCalibrate);

HI_S32 HI_MPI_ISP_SetDPStaticAttr(VI_PIPE ViPipe, const ISP_DP_STATIC_ATTR_S *pstDPStaticAttr);
HI_S32 HI_MPI_ISP_GetDPStaticAttr(VI_PIPE ViPipe,  ISP_DP_STATIC_ATTR_S *pstDPStaticAttr);

HI_S32 HI_MPI_ISP_SetDPDynamicAttr(VI_PIPE ViPipe, const ISP_DP_DYNAMIC_ATTR_S *pstDPDynamicAttr);
HI_S32 HI_MPI_ISP_GetDPDynamicAttr(VI_PIPE ViPipe, ISP_DP_DYNAMIC_ATTR_S *pstDPDynamicAttr);

HI_S32 HI_MPI_ISP_SetDISAttr(VI_PIPE ViPipe, const ISP_DIS_ATTR_S *pstDISAttr);
HI_S32 HI_MPI_ISP_GetDISAttr(VI_PIPE ViPipe, ISP_DIS_ATTR_S *pstDISAttr);

HI_S32 HI_MPI_ISP_SetMeshShadingAttr(VI_PIPE ViPipe, const ISP_SHADING_ATTR_S *pstShadingAttr);
HI_S32 HI_MPI_ISP_GetMeshShadingAttr(VI_PIPE ViPipe, ISP_SHADING_ATTR_S *pstShadingAttr);

HI_S32 HI_MPI_ISP_SetMeshShadingGainLutAttr(VI_PIPE ViPipe, const ISP_SHADING_LUT_ATTR_S *pstShadingGainLutAttr);
HI_S32 HI_MPI_ISP_GetMeshShadingGainLutAttr(VI_PIPE ViPipe, ISP_SHADING_LUT_ATTR_S *pstShadingGainLutAttr);

HI_S32 HI_MPI_ISP_SetRadialShadingAttr(VI_PIPE ViPipe, const ISP_RADIAL_SHADING_ATTR_S *pstRaShadingAttr);
HI_S32 HI_MPI_ISP_GetRadialShadingAttr(VI_PIPE ViPipe, ISP_RADIAL_SHADING_ATTR_S *pstRaShadingAttr);

HI_S32 HI_MPI_ISP_SetRadialShadingLUT(VI_PIPE ViPipe, const ISP_RADIAL_SHADING_LUT_ATTR_S *pstRaShadingLutAttr);
HI_S32 HI_MPI_ISP_GetRadialShadingLUT(VI_PIPE ViPipe, ISP_RADIAL_SHADING_LUT_ATTR_S *pstRaShadingLutAttr);

HI_S32 HI_MPI_ISP_MeshShadingCalibration(VI_PIPE ViPipe, HI_U16 *pu16SrcRaw, ISP_MLSC_CALIBRATION_CFG_S *pstMLSCCaliCfg, ISP_MESH_SHADING_TABLE_S *pstMLSCTable);

HI_S32 HI_MPI_ISP_SetNRAttr(VI_PIPE ViPipe, const ISP_NR_ATTR_S *pstNRAttr);
HI_S32 HI_MPI_ISP_GetNRAttr(VI_PIPE ViPipe, ISP_NR_ATTR_S *pstNRAttr);

HI_S32 HI_MPI_ISP_SetDEAttr(VI_PIPE ViPipe, const ISP_DE_ATTR_S *pstDEAttr);
HI_S32 HI_MPI_ISP_GetDEAttr(VI_PIPE ViPipe, ISP_DE_ATTR_S *pstDEAttr);

HI_S32 HI_MPI_ISP_SetColorToneAttr(VI_PIPE ViPipe, const ISP_COLOR_TONE_ATTR_S *pstCTAttr);
HI_S32 HI_MPI_ISP_GetColorToneAttr(VI_PIPE ViPipe, ISP_COLOR_TONE_ATTR_S *pstCTAttr);

HI_S32 HI_MPI_ISP_SetGammaAttr(VI_PIPE ViPipe, const ISP_GAMMA_ATTR_S *pstGammaAttr);
HI_S32 HI_MPI_ISP_GetGammaAttr(VI_PIPE ViPipe, ISP_GAMMA_ATTR_S *pstGammaAttr);

HI_S32 HI_MPI_ISP_SetPreGammaAttr(VI_PIPE ViPipe, const ISP_PREGAMMA_ATTR_S *pstPreGammaAttr);
HI_S32 HI_MPI_ISP_GetPreGammaAttr(VI_PIPE ViPipe, ISP_PREGAMMA_ATTR_S *pstPreGammaAttr);

HI_S32 HI_MPI_ISP_SetPreLogLUTAttr(VI_PIPE ViPipe, const ISP_PRELOGLUT_ATTR_S *pstPreLogLUTAttr);
HI_S32 HI_MPI_ISP_GetPreLogLUTAttr(VI_PIPE ViPipe, ISP_PRELOGLUT_ATTR_S *pstPreLogLUTAttr);

HI_S32 HI_MPI_ISP_SetLogLUTAttr(VI_PIPE ViPipe, const ISP_LOGLUT_ATTR_S *pstLogLUTAttr);
HI_S32 HI_MPI_ISP_GetLogLUTAttr(VI_PIPE ViPipe, ISP_LOGLUT_ATTR_S *pstLogLUTAttr);

HI_S32 HI_MPI_ISP_SetLocalCacAttr(VI_PIPE ViPipe, const ISP_LOCAL_CAC_ATTR_S *pstLocalCacAttr);
HI_S32 HI_MPI_ISP_GetLocalCacAttr(VI_PIPE ViPipe, ISP_LOCAL_CAC_ATTR_S *pstLocalCacAttr);

HI_S32 HI_MPI_ISP_SetGlobalCacAttr(VI_PIPE ViPipe, const ISP_GLOBAL_CAC_ATTR_S *pstGlobalCacAttr);
HI_S32 HI_MPI_ISP_GetGlobalCacAttr(VI_PIPE ViPipe, ISP_GLOBAL_CAC_ATTR_S *pstGlobalCacAttr);

HI_S32 HI_MPI_ISP_SetRcAttr(VI_PIPE ViPipe, const ISP_RC_ATTR_S *pstRcAttr);
HI_S32 HI_MPI_ISP_GetRcAttr(VI_PIPE ViPipe, ISP_RC_ATTR_S *pstRcAttr);

HI_S32 HI_MPI_ISP_SetIspSharpenAttr(VI_PIPE ViPipe, const ISP_SHARPEN_ATTR_S *pstIspShpAttr);
HI_S32 HI_MPI_ISP_GetIspSharpenAttr(VI_PIPE ViPipe, ISP_SHARPEN_ATTR_S *pstIspShpAttr);

HI_S32 HI_MPI_ISP_SetIspEdgeMarkAttr(VI_PIPE ViPipe, const ISP_EDGEMARK_ATTR_S *pstIspEdgeMarkAttr);
HI_S32 HI_MPI_ISP_GetIspEdgeMarkAttr(VI_PIPE ViPipe, ISP_EDGEMARK_ATTR_S *pstIspEdgeMarkAttr);

HI_S32 HI_MPI_ISP_SetIspHlcAttr(VI_PIPE ViPipe, const ISP_HLC_ATTR_S *pstIspHlcAttr);
HI_S32 HI_MPI_ISP_GetIspHlcAttr(VI_PIPE ViPipe, ISP_HLC_ATTR_S *pstIspHlcAttr);

HI_S32 HI_MPI_ISP_SetCrosstalkAttr(VI_PIPE ViPipe, const ISP_CR_ATTR_S *pstCRAttr);
HI_S32 HI_MPI_ISP_GetCrosstalkAttr(VI_PIPE ViPipe, ISP_CR_ATTR_S *pstCRAttr);

HI_S32 HI_MPI_ISP_SetCAAttr(VI_PIPE ViPipe, const ISP_CA_ATTR_S *pstCAAttr);
HI_S32 HI_MPI_ISP_GetCAAttr(VI_PIPE ViPipe, ISP_CA_ATTR_S *pstCAAttr);

HI_S32 HI_MPI_ISP_SetAntiFalseColorAttr(VI_PIPE ViPipe, const ISP_ANTIFALSECOLOR_ATTR_S *pstAntiFalseColor);
HI_S32 HI_MPI_ISP_GetAntiFalseColorAttr(VI_PIPE ViPipe, ISP_ANTIFALSECOLOR_ATTR_S *pstAntiFalseColor);

HI_S32 HI_MPI_ISP_SetDemosaicAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);
HI_S32 HI_MPI_ISP_GetDemosaicAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);

HI_S32 HI_MPI_ISP_SetBlackLevelAttr(VI_PIPE ViPipe, const ISP_BLACK_LEVEL_S *pstBlackLevel);
HI_S32 HI_MPI_ISP_GetBlackLevelAttr(VI_PIPE ViPipe, ISP_BLACK_LEVEL_S *pstBlackLevel);

HI_S32 HI_MPI_ISP_SetClutAttr(VI_PIPE ViPipe, const ISP_CLUT_ATTR_S *pstClutAttr);
HI_S32 HI_MPI_ISP_GetClutAttr(VI_PIPE ViPipe, ISP_CLUT_ATTR_S *pstClutAttr);

HI_S32 HI_MPI_ISP_SetClutCoeff(VI_PIPE ViPipe, const ISP_CLUT_LUT_S *pstClutLUT);
HI_S32 HI_MPI_ISP_GetClutCoeff(VI_PIPE ViPipe, ISP_CLUT_LUT_S *pstClutLUT);

HI_S32 HI_MPI_ISP_SetDCFInfo(VI_PIPE ViPipe, const ISP_DCF_INFO_S *pstIspDCF);
HI_S32 HI_MPI_ISP_GetDCFInfo(VI_PIPE ViPipe, ISP_DCF_INFO_S *pstIspDCF);

HI_S32 HI_MPI_ISP_SetCSCAttr(VI_PIPE ViPipe, const ISP_CSC_ATTR_S *pstCSCAttr);
HI_S32 HI_MPI_ISP_GetCSCAttr(VI_PIPE ViPipe, ISP_CSC_ATTR_S *pstCSCFAttr);

HI_S32 HI_MPI_ISP_SetRGBIRAttr(VI_PIPE ViPipe, const ISP_RGBIR_ATTR_S *pstRGBIRAttr);
HI_S32 HI_MPI_ISP_GetRGBIRAttr(VI_PIPE ViPipe, ISP_RGBIR_ATTR_S *pstRGBIRAttr);
HI_S32 HI_MPI_ISP_SetRegister(VI_PIPE ViPipe, HI_U32 u32Addr, HI_U32 u32Value);
HI_S32 HI_MPI_ISP_GetRegister(VI_PIPE ViPipe, HI_U32 u32Addr, HI_U32 *pu32Value);

/* block, return fpn frame when this is function is called. */
HI_S32 HI_MPI_ISP_FPNCalibrate(VI_PIPE ViPipe, ISP_FPN_CALIBRATE_ATTR_S *pstCalibrateAttr);
HI_S32 HI_MPI_ISP_SetFPNAttr(VI_PIPE ViPipe, const ISP_FPN_ATTR_S *pstFPNAttr);
HI_S32 HI_MPI_ISP_GetFPNAttr(VI_PIPE ViPipe, ISP_FPN_ATTR_S *pstFPNAttr);

HI_S32 HI_MPI_ISP_GetDngImageStaticInfo(VI_PIPE ViPipe, DNG_IMAGE_STATIC_INFO_S *pstDngImageStaticInfo);
HI_S32 HI_MPI_ISP_SetDngColorParam(VI_PIPE ViPipe, const ISP_DNG_COLORPARAM_S *pstDngColorParam);
HI_S32 HI_MPI_ISP_GetDngColorParam(VI_PIPE ViPipe, ISP_DNG_COLORPARAM_S *pstDngColorParam);
HI_S32 HI_MPI_ISP_IrAutoRunOnce(VI_PIPE ViPipe, ISP_IR_AUTO_ATTR_S *pstIrAttr);

HI_S32 HI_MPI_ISP_SetSmartInfo(VI_PIPE ViPipe, const ISP_SMART_INFO_S *pstSmartInfo);
HI_S32 HI_MPI_ISP_GetSmartInfo(VI_PIPE ViPipe, ISP_SMART_INFO_S *pstSmartInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __MPI_ISP_H__ */
