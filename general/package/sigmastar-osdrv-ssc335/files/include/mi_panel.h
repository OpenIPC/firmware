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
#ifndef _MI_PANEL_H_
#define _MI_PANEL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"
#include "mi_panel_datatype.h"

#define PANEL_MAJOR_VERSION 2
#define PANEL_SUB_VERSION 12
#define MACRO_TO_STR(macro) #macro
#define PANEL_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_panel_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_panel_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_panel_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_PANEL_API_VERSION PANEL_VERSION_STR(PANEL_MAJOR_VERSION,PANEL_SUB_VERSION)

MI_S32 MI_PANEL_Init(MI_PANEL_IntfType_e eIntfType);
MI_S32 MI_PANEL_DeInit(void);
MI_S32 MI_PANEL_GetTotalNum(MI_U32 *pu32TotalNum);
MI_S32 MI_PANEL_SetPowerOn(MI_PANEL_IntfType_e eIntfType, MI_PANEL_PowerConfig_t *pstPowerCfg);
MI_S32 MI_PANEL_GetPowerOn(MI_PANEL_IntfType_e eIntfType, MI_PANEL_PowerConfig_t *pstPowerCfg);
MI_S32 MI_PANEL_SetBackLight(MI_PANEL_IntfType_e eIntfType, MI_PANEL_BackLightConfig_t *pstBackLightCfg);
MI_S32 MI_PANEL_GetBackLight(MI_PANEL_IntfType_e eIntfType, MI_PANEL_BackLightConfig_t *pstBackLightCfg);
MI_S32 MI_PANEL_SetBackLightLevel(MI_PANEL_IntfType_e eIntfType, MI_PANEL_BackLightConfig_t *pstBackLightCfg);
MI_S32 MI_PANEL_GetBackLightLevel(MI_PANEL_IntfType_e eIntfType, MI_PANEL_BackLightConfig_t *pstBackLightCfg);
MI_S32 MI_PANEL_SetSscConfig(MI_PANEL_IntfType_e eIntfType, MI_PANEL_SscConfig_t *pstSscCfg);
MI_S32 MI_PANEL_SetMipiDsiConfig(MI_PANEL_IntfType_e eIntfType, MI_PANEL_MipiDsiConfig_t *pstMipiDsiCfg);
MI_S32 MI_PANEL_SetTimingConfig(MI_PANEL_IntfType_e eIntfType, MI_PANEL_TimingConfig_t *pstTimingCfg);
MI_S32 MI_PANEL_SetDrvCurrentConfig(MI_PANEL_IntfType_e eIntfType, MI_PANEL_DrvCurrentConfig_t *pstDrvCurrentCfg);
MI_S32 MI_PANEL_SetOutputPattern(MI_PANEL_IntfType_e eIntfType, MI_PANEL_TestPatternConfig_t * pstTestPatternCfg);
MI_S32 MI_PANEL_SetPanelParam(MI_PANEL_IntfType_e eIntfType, MI_PANEL_ParamConfig_t *pstParamCfg);
MI_S32 MI_PANEL_GetPanelParam(MI_PANEL_IntfType_e eIntfType, MI_PANEL_ParamConfig_t *pstParamCfg);
MI_S32 MI_PANEL_InitDev(MI_PANEL_InitParam_t *pstInitParam);
MI_S32 MI_PANEL_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif ///_MI_PANEL_H_
