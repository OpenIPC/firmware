/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

/*
*   mi_iqserver.h
*
*   Created on: July 10, 2018
*       Author: Shan Li
*/


#ifndef _MI_IQSERVER_H
#define _MI_IQSERVER_H

#define IRQSERVER_MAJOR_VERSION 2
#define IRQSERVER_SUB_VERSION 3
#define MACRO_TO_STR(macro) #macro
#define IRQSERVER_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_irqserver_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_irqserver_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_irqserver_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_IRQSERVER_API_VERSION IRQSERVER_VERSION_STR(IRQSERVER_MAJOR_VERSION,IRQSERVER_SUB_VERSION)

#ifdef __cplusplus
extern "C"
{
#endif
#include "mi_iqserver_datatype.h"

/*
*   Open IQServer
*   Param:
*       width: sensor width
*       height: sensor height
*/
MI_S32 MI_IQSERVER_Open(MI_U16 width, MI_U16 height, MI_S32 vpeChn);

MI_S32 MI_IQSERVER_SetDataPath(char* path);

MI_S32 MI_IQSERVER_SetCustFunc(MI_S32(* func)(MI_U16 data_type, MI_U32 length, MI_U8 * data));

/*
*   Close IQServer
*/
MI_S32 MI_IQSERVER_Close();
#endif

#ifdef __cplusplus
}   //end of extern C
#endif
