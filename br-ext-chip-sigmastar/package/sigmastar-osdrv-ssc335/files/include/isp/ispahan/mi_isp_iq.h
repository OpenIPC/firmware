/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

/*
*   mi_isp.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _MI_ISP_IQ_H_
#define _MI_ISP_IQ_H_

#include "mi_isp_3a_datatype.h"
#include "mi_isp_iq_datatype.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if 1
    /************************************* IQ  API START *************************************/
    MI_S32 MI_ISP_IQ_GetVersionInfo(MI_U32 Channel, MI_ISP_IQ_VERSION_INFO_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetVersionInfo(MI_U32 Channel, MI_ISP_IQ_VERSION_INFO_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetParaInitStatus(MI_U32 Channel, MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetColorToGray(MI_U32 Channel, MI_ISP_IQ_COLORTOGRAY_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetColorToGray(MI_U32 Channel, MI_ISP_IQ_COLORTOGRAY_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetContrast(MI_U32 Channel, MI_ISP_IQ_CONTRAST_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetContrast(MI_U32 Channel, MI_ISP_IQ_CONTRAST_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetBrightness(MI_U32 Channel, MI_ISP_IQ_BRIGHTNESS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetBrightness(MI_U32 Channel, MI_ISP_IQ_BRIGHTNESS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetLightness(MI_U32 Channel, MI_ISP_IQ_LIGHTNESS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetLightness(MI_U32 Channel, MI_ISP_IQ_LIGHTNESS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetRGBGamma(MI_U32 Channel, MI_ISP_IQ_RGBGAMMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetRGBGamma(MI_U32 Channel, MI_ISP_IQ_RGBGAMMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetYUVGamma(MI_U32 Channel, MI_ISP_IQ_YUVGAMMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetYUVGamma(MI_U32 Channel, MI_ISP_IQ_YUVGAMMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetSaturation(MI_U32 Channel, MI_ISP_IQ_SATURATION_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetSaturation(MI_U32 Channel, MI_ISP_IQ_SATURATION_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetDefog(MI_U32 Channel, MI_ISP_IQ_DEFOG_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetDefog(MI_U32 Channel, MI_ISP_IQ_DEFOG_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetRGBMatrix(MI_U32 Channel, MI_ISP_IQ_RGBMATRIX_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetRGBMatrix(MI_U32 Channel, MI_ISP_IQ_RGBMATRIX_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetFalseColor(MI_U32 Channel, MI_ISP_IQ_FALSECOLOR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetFalseColor(MI_U32 Channel, MI_ISP_IQ_FALSECOLOR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetNR3D(MI_U32 Channel, MI_ISP_IQ_NR3D_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetNR3D(MI_U32 Channel, MI_ISP_IQ_NR3D_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetNR3D_P1(MI_U32 Channel, MI_ISP_IQ_NR3D_P1_TYPE_t *data); //[not support chip] macaron, pudding
    MI_S32 MI_ISP_IQ_GetNR3D_P1(MI_U32 Channel, MI_ISP_IQ_NR3D_P1_TYPE_t *data); //[not support chip] macaron, pudding
    MI_S32 MI_ISP_IQ_SetNRDeSpike(MI_U32 Channel, MI_ISP_IQ_NRDESPIKE_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetNRDeSpike(MI_U32 Channel, MI_ISP_IQ_NRDESPIKE_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetNRLuma(MI_U32 Channel, MI_ISP_IQ_NRLUMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetNRLuma(MI_U32 Channel, MI_ISP_IQ_NRLUMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetNRChroma(MI_U32 Channel, MI_ISP_IQ_NRChroma_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetNRChroma(MI_U32 Channel, MI_ISP_IQ_NRChroma_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetSharpness(MI_U32 Channel, MI_ISP_IQ_SHARPNESS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetSharpness(MI_U32 Channel, MI_ISP_IQ_SHARPNESS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetCrossTalk(MI_U32 Channel, MI_ISP_IQ_CROSSTALK_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetCrossTalk(MI_U32 Channel, MI_ISP_IQ_CROSSTALK_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetOBC(MI_U32 Channel, MI_ISP_IQ_OBC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetOBC(MI_U32 Channel, MI_ISP_IQ_OBC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetOBC_P1(MI_U32 Channel, MI_ISP_IQ_OBC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetOBC_P1(MI_U32 Channel, MI_ISP_IQ_OBC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetWDR(MI_U32 Channel, MI_ISP_IQ_WDR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetWDR(MI_U32 Channel, MI_ISP_IQ_WDR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetWDRCurveAdv(MI_U32 Channel, MI_ISP_IQ_WDRCurveAdv_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetWDRCurveAdv(MI_U32 Channel, MI_ISP_IQ_WDRCurveAdv_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetWDRCurveFull(MI_U32 Channel, MI_ISP_IQ_WDRCurveFull_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetWDRCurveFull(MI_U32 Channel, MI_ISP_IQ_WDRCurveFull_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetDynamicDP(MI_U32 Channel, MI_ISP_IQ_DYNAMIC_DP_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetDynamicDP(MI_U32 Channel, MI_ISP_IQ_DYNAMIC_DP_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetDynamicDP_CLUSTER(MI_U32 Channel, MI_ISP_IQ_DYNAMIC_DP_CLUSTER_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetDynamicDP_CLUSTER(MI_U32 Channel, MI_ISP_IQ_DYNAMIC_DP_CLUSTER_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetHSV(MI_U32 Channel, MI_ISP_IQ_HSV_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetHSV(MI_U32 Channel, MI_ISP_IQ_HSV_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetRGBIR(MI_U32 Channel, MI_ISP_IQ_RGBIR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetRGBIR(MI_U32 Channel, MI_ISP_IQ_RGBIR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetFPN(MI_U32 Channel, MI_ISP_IQ_FPN_TYPE_t *data); //[not support chip] macaron, pudding
    MI_S32 MI_ISP_IQ_GetFPN(MI_U32 Channel, MI_ISP_IQ_FPN_TYPE_t *data); //[not support chip] macaron, pudding
    MI_S32 MI_ISP_IQ_SetPFC(MI_U32 Channel, MI_ISP_IQ_PFC_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_GetPFC(MI_U32 Channel, MI_ISP_IQ_PFC_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_SetDEMOSAIC(MI_U32 Channel, MI_ISP_IQ_DEMOSAIC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetDEMOSAIC(MI_U32 Channel, MI_ISP_IQ_DEMOSAIC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetR2Y(MI_U32 Channel, MI_ISP_IQ_R2Y_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetR2Y(MI_U32 Channel, MI_ISP_IQ_R2Y_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetCOLORTRANS(MI_U32 Channel, MI_ISP_IQ_COLORTRANS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetCOLORTRANS(MI_U32 Channel, MI_ISP_IQ_COLORTRANS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetHDR(MI_U32 Channel, MI_ISP_IQ_HDR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetHDR(MI_U32 Channel, MI_ISP_IQ_HDR_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetLSC(MI_U32 Channel, MI_ISP_IQ_LSC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetLSC(MI_U32 Channel, MI_ISP_IQ_LSC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetLSC_CTRL(MI_U32 Channel, MI_ISP_IQ_LSC_CTRL_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetLSC_CTRL(MI_U32 Channel, MI_ISP_IQ_LSC_CTRL_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetALSC(MI_U32 Channel, MI_ISP_IQ_ALSC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetALSC(MI_U32 Channel, MI_ISP_IQ_ALSC_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetALSC_CTRL(MI_U32 Channel, MI_ISP_IQ_ALSC_CTRL_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetALSC_CTRL(MI_U32 Channel, MI_ISP_IQ_ALSC_CTRL_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetNRLuma_Adv(MI_U32 Channel, MI_ISP_IQ_NRLuma_Adv_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_GetNRLuma_Adv(MI_U32 Channel, MI_ISP_IQ_NRLuma_Adv_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_SetNRChroma_Adv(MI_U32 Channel, MI_ISP_IQ_NRChroma_Adv_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_GetNRChroma_Adv(MI_U32 Channel, MI_ISP_IQ_NRChroma_Adv_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_SetNRChroma_Pre(MI_U32 Channel, MI_ISP_IQ_NRChroma_Pre_TYPE_t *data); //[not support chip] macaron, pudding, ispahan
    MI_S32 MI_ISP_IQ_GetNRChroma_Pre(MI_U32 Channel, MI_ISP_IQ_NRChroma_Pre_TYPE_t *data); //[not support chip] macaron, pudding, ispahan
    MI_S32 MI_ISP_IQ_SetPFC_EX(MI_U32 Channel, MI_ISP_IQ_PFC_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_GetPFC_EX(MI_U32 Channel, MI_ISP_IQ_PFC_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_SetHDR_EX(MI_U32 Channel, MI_ISP_IQ_HDR_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_GetHDR_EX(MI_U32 Channel, MI_ISP_IQ_HDR_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_SetSHP_EX(MI_U32 Channel, MI_ISP_IQ_SHARPNESS_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_GetSHP_EX(MI_U32 Channel, MI_ISP_IQ_SHARPNESS_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_SetNR3D_EX(MI_U32 Channel, MI_ISP_IQ_NR3D_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_GetNR3D_EX(MI_U32 Channel, MI_ISP_IQ_NR3D_EX_TYPE_t *data); //[not support chip] macaron
    MI_S32 MI_ISP_IQ_SetDUMMY(MI_U32 Channel, MI_ISP_IQ_DUMMY_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetDUMMY(MI_U32 Channel, MI_ISP_IQ_DUMMY_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetDUMMY_EX(MI_U32 Channel, MI_ISP_IQ_DUMMY_EX_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetDUMMY_EX(MI_U32 Channel, MI_ISP_IQ_DUMMY_EX_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetIQMode(MI_U32 Channel, MI_ISP_IQ_PARAM_MODE_e *data);
    MI_S32 MI_ISP_IQ_GetIQMode(MI_U32 Channel, MI_ISP_IQ_PARAM_MODE_e *data);
    MI_S32 MI_ISP_IQ_GetIQind(MI_U32 Channel, MI_ISP_IQ_INDEX_e *data);
    MI_S32 MI_ISP_IQ_SetAPIBypassMode(MI_U32 Channel, MI_ISP_API_BYPASS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetAPIBypassMode(MI_U32 Channel, MI_ISP_API_BYPASS_TYPE_t *data);
    MI_S32 MI_ISP_IQ_QueryCCMInfo(MI_U32 Channel, MI_ISP_IQ_CCM_INFO_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetAdaptiveGamma(MI_U32 Channel, MI_ISP_IQ_ADAPTIVE_GAMMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetAdaptiveGamma(MI_U32 Channel, MI_ISP_IQ_ADAPTIVE_GAMMA_TYPE_t *data);
    MI_S32 MI_ISP_IQ_SetTEMP(MI_U32 Channel, MI_ISP_IQ_TEMP_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetTEMP(MI_U32 Channel, MI_ISP_IQ_TEMP_TYPE_t *data);
    MI_S32 MI_ISP_IQ_GetTEMP_INFO(MI_U32 Channel, MI_ISP_IQ_TEMP_INFO_TYPE_t *data);
    /************************************* IQ  API END   *************************************/

    /************************************* AE  API START *************************************/
    MI_S32 MI_ISP_AE_GetHistoWghtY(MI_U32 Channel, MI_ISP_AE_HIST_WEIGHT_Y_TYPE_t *data);
    MI_S32 MI_ISP_AE_QueryExposureInfo(MI_U32 Channel, MI_ISP_AE_EXPO_INFO_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetEVComp(MI_U32 Channel, MI_ISP_AE_EV_COMP_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetEVComp(MI_U32 Channel, MI_ISP_AE_EV_COMP_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetExpoMode(MI_U32 Channel, MI_ISP_AE_MODE_TYPE_e *data);
    MI_S32 MI_ISP_AE_GetExpoMode(MI_U32 Channel, MI_ISP_AE_MODE_TYPE_e *data);
    MI_S32 MI_ISP_AE_SetManualExpo(MI_U32 Channel, MI_ISP_AE_EXPO_VALUE_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetManualExpo(MI_U32 Channel, MI_ISP_AE_EXPO_VALUE_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetManualShortExpo(MI_U32 Channel, MI_ISP_AE_EXPO_VALUE_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetManualShortExpo(MI_U32 Channel, MI_ISP_AE_EXPO_VALUE_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetState(MI_U32 Channel, MI_ISP_SM_STATE_TYPE_e *data);  // Pause, Resume
    MI_S32 MI_ISP_AE_GetState(MI_U32 Channel, MI_ISP_SM_STATE_TYPE_e *data);
    MI_S32 MI_ISP_AE_SetTarget(MI_U32 Channel, MI_ISP_AE_INTP_LUT_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetTarget(MI_U32 Channel, MI_ISP_AE_INTP_LUT_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetConverge(MI_U32 Channel, MI_ISP_AE_CONV_CONDITON_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetConverge(MI_U32 Channel, MI_ISP_AE_CONV_CONDITON_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetExposureLimit(MI_U32 Channel, MI_ISP_AE_EXPO_LIMIT_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetExposureLimit(MI_U32 Channel, MI_ISP_AE_EXPO_LIMIT_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetPlainLongExpoTable(MI_U32 Channel, MI_ISP_AE_EXPO_TABLE_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetPlainLongExpoTable(MI_U32 Channel, MI_ISP_AE_EXPO_TABLE_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetPlainShortExpoTable(MI_U32 Channel, MI_ISP_AE_EXPO_TABLE_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetPlainShortExpoTable(MI_U32 Channel, MI_ISP_AE_EXPO_TABLE_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetWinWgtType(MI_U32 Channel, MI_ISP_AE_WIN_WEIGHT_MODE_TYPE_e *data);
    MI_S32 MI_ISP_AE_GetWinWgtType(MI_U32 Channel, MI_ISP_AE_WIN_WEIGHT_MODE_TYPE_e *data);
    MI_S32 MI_ISP_AE_SetWinWgt(MI_U32 Channel, MI_ISP_AE_WIN_WEIGHT_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetWinWgt(MI_U32 Channel, MI_ISP_AE_WIN_WEIGHT_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetFlicker(MI_U32 Channel, MI_ISP_AE_FLICKER_TYPE_e *data);
    MI_S32 MI_ISP_AE_GetFlicker(MI_U32 Channel, MI_ISP_AE_FLICKER_TYPE_e *data);
    MI_S32 MI_ISP_AE_SetFlickerEX(MI_U32 Channel, MI_ISP_AE_FLICKER_EX_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetFlickerEX(MI_U32 Channel, MI_ISP_AE_FLICKER_EX_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetStrategy(MI_U32 Channel, MI_ISP_AE_STRATEGY_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetStrategy(MI_U32 Channel, MI_ISP_AE_STRATEGY_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetStrategyEX(MI_U32 Channel, MI_ISP_AE_STRATEGY_EX_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetStrategyEX(MI_U32 Channel, MI_ISP_AE_STRATEGY_EX_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetRGBIRAE(MI_U32 Channel, MI_ISP_AE_RGBIR_AE_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetRGBIRAE(MI_U32 Channel, MI_ISP_AE_RGBIR_AE_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetHDR(MI_U32 Channel, MI_ISP_AE_HDR_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetHDR(MI_U32 Channel, MI_ISP_AE_HDR_TYPE_t *data);
    MI_S32 MI_ISP_AE_SetStabilizer(MI_U32 Channel, MI_ISP_AE_STABILIZER_TYPE_t *data);
    MI_S32 MI_ISP_AE_GetStabilizer(MI_U32 Channel, MI_ISP_AE_STABILIZER_TYPE_t *data);
    /************************************* AE  API END   *************************************/

    /************************************* AWB API START *************************************/
    MI_S32 MI_ISP_AWB_QueryInfo(MI_U32 Channel, MI_ISP_AWB_QUERY_INFO_TYPE_t *data);
    MI_S32 MI_ISP_AWB_SetAttr(MI_U32 Channel, MI_ISP_AWB_ATTR_TYPE_t *data);
    MI_S32 MI_ISP_AWB_GetAttr(MI_U32 Channel, MI_ISP_AWB_ATTR_TYPE_t *data);
    MI_S32 MI_ISP_AWB_SetAttrEx(MI_U32 Channel, MI_ISP_AWB_ATTR_EX_TYPE_t *data);
    MI_S32 MI_ISP_AWB_GetAttrEx(MI_U32 Channel, MI_ISP_AWB_ATTR_EX_TYPE_t *data);
    MI_S32 MI_ISP_AWB_SetMultiLSAttr(MI_U32 Channel, MI_ISP_AWB_MULTILS_LS_TYPE_t *data);
    MI_S32 MI_ISP_AWB_GetMultiLSAttr(MI_U32 Channel, MI_ISP_AWB_MULTILS_LS_TYPE_t *data);
    MI_S32 MI_ISP_AWB_SetCTWeight(MI_U32 Channel, MI_ISP_AWB_CT_WEIGHT_TYPE_t *data); //Not works now, please use MI_ISP_AWB_SetAttr to set LvWeight indstead.
    MI_S32 MI_ISP_AWB_GetCTWeight(MI_U32 Channel, MI_ISP_AWB_CT_WEIGHT_TYPE_t *data); //Not works now, please use MI_ISP_AWB_GetAttr to set LvWeight indstead.

    MI_S32 MI_ISP_AWB_SetCTCaliAttr(MI_U32 Channel, MI_ISP_AWB_CT_CALI_TYPE_t *data);
    MI_S32 MI_ISP_AWB_GetCTCaliAttr(MI_U32 Channel, MI_ISP_AWB_CT_CALI_TYPE_t *data);
    MI_S32 MI_ISP_AWB_GetCurCTCaliAttr(MI_U32 Channel, MI_ISP_AWB_CUR_CT_CALI_TYPE_t *data);
    MI_S32 MI_ISP_AWB_GetCTStats(MI_U32 Channel, MI_ISP_AWB_CT_STATISTICS_TYPE_t *data);
    MI_S32 MI_ISP_AWB_GetHWStats(MI_U32 Channel, MI_ISP_AWB_HW_STATISTICS_TYPE_t *data);
    MI_S32 MI_ISP_AWB_SetCTMwbAttr(MI_U32 Channel, MI_ISP_AWB_CTMWB_PARAM_t *data);
    MI_S32 MI_ISP_AWB_GetCTMwbAttr(MI_U32 Channel, MI_ISP_AWB_CTMWB_PARAM_t *data);

    /************************************* AWB API END   *************************************/

    /************************************* AF  API START *************************************/
    /************************************* AF  API END   *************************************/

    MI_S32 MI_ISP_ALGO_SET_ALL(int Channel, MI_U16 ApiId, MI_U32 ApiLen, MI_U8* pApiBuf);
    MI_S32 MI_ISP_ALGO_GET_ALL(int Channel, MI_U16 ApiId, MI_U32* ApiLen, MI_U8* pApiBuf);

#endif

#if 1

typedef struct MI_ISP_EXCUTE_CMD_ARRAY_s
{
    MI_U32 u32CmdType;
    MI_U16 u16APIID;
    MI_S32 (*callback)(MI_U32 Channel, MI_U8 *param_ary[], MI_U8 param_num);
} MI_ISP_EXCUTE_CMD_ARRAY_t;

typedef enum __attribute__ ((aligned (1)))
{
    MI_ISP_CAMERA_CMD_SET = 0,
    MI_ISP_CAMERA_CMD_GET,
    MI_ISP_CAMERA_CMD_SET_MODE ,
    MI_ISP_CAMERA_CMD_GET_MODE,
    MI_ISP_CAMERA_CMD_GET_PIC,
    MI_ISP_CAMERA_CMD_SET_API,
    MI_ISP_CAMERA_CMD_GET_API,
    MI_ISP_CAMERA_CMD_UPLOAD_FILE,    /* client upload file to server */
    MI_ISP_CAMERA_CMD_DOWNLOAD_FILE,  /* client download file from server*/
} MI_ISP_CAMERA_EXT_CMD_TYPE_e;

typedef struct MI_ISP_CMD_HEADER_s
{
    MI_ISP_CAMERA_EXT_CMD_TYPE_e CmdType;
    MI_S32 CmdLen;
} MI_ISP_CMD_HEADER_t;

typedef struct MI_ISP_API_PARAM_s
{
    MI_U32 u32MagicKey;
    MI_ISP_CMD_HEADER_t sCmdheader;
    MI_U16 u16APIID;
    MI_U16 u16ParamNum;
} MI_ISP_API_PARAM_t;

typedef struct MI_ISP_API_BIN_FILE_s
{
    MI_U32 u32FileID;
    MI_U32 u32ISPVer;
    MI_U32 u32DataLen;
    MI_U32 u32Checksum;
    MI_U32 u32MagicKey;
    MI_U32 u32Reserved[3];
} MI_ISP_API_BIN_FILE_t;

typedef struct MI_ISP_API_CALI_FILE_s
{
    MI_U32 u32CaliVer;
    MI_U32 u32DataSize;
    MI_U32 u32Checksum;
    MI_U32 u32Reserved[2];
} MI_ISP_API_CALI_FILE_t;

    MI_S32 MI_ISP_ALGO_API_DisableAllApi(MI_U32 Channel);
    MI_S32 MI_ISP_ALGO_API_CmdLoadBinFile(MI_U32 Channel, MI_U8 *bindata_buf, MI_U32 user_key);
    MI_S32 MI_ISP_ALGO_API_CmdLoadCaliData(MI_U32 Channel, MI_U8 *bindata_buf, MI_U32 BIN_BUF_MAX_LEN, MI_ISP_CALI_ITEM_e eCaliItem);

#endif

#ifdef __cplusplus
}   //end of extern C
#endif

#endif  //_MI_ISP_IQ_H_
