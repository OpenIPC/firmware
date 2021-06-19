/*
* mi_sed.h- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: XXXX <XXXX@sigmastar.com.cn>
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/
#ifndef _MI_SED_H_
#define _MI_SED_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "mi_sed_datatype.h"

#define SED_MAJOR_VERSION 2
#define SED_SUB_VERSION 1
#define MACRO_TO_STR(macro) #macro
#define SED_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_sed_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_sed_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_sed_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_SED_API_VERSION SED_VERSION_STR(SED_MAJOR_VERSION, SED_SUB_VERSION)

MI_S32 MI_SED_CreateChn(MI_SED_CHN SedChn, MI_SED_DetectorAttr_t* pstAttr);
MI_S32 MI_SED_DestroyChn(MI_SED_CHN SedChn);
MI_S32 MI_SED_StartDetector(MI_SED_CHN SedChn);
MI_S32 MI_SED_StopDetector(MI_SED_CHN SedChn);
MI_S32 MI_SED_AttachToVencChn(MI_SED_CHN SedChn, MI_SED_TARGET_CHN TargetChn);
MI_S32 MI_SED_DetachFromVencChn(MI_SED_CHN SedChn, MI_SED_TARGET_CHN TargetChn);
MI_S32 MI_SED_GetRect(MI_SED_CHN SedChn, MI_SED_RectInfo_t *pstRectInfo);
MI_S32 MI_SED_SetDbgLevel(MI_DBG_LEVEL_e eLevel);

#ifdef __cplusplus
}
#endif

#endif
