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
#ifndef _MI_COMMON_H_
#define _MI_COMMON_H_

#include "mi_common_datatype.h"

#define COMMON_MAJOR_VERSION 2
#define COMMON_SUB_VERSION 3
#define MACRO_TO_STR(macro) #macro
#define COMMON_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_common_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_common_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_common_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_COMMON_API_VERSION COMMON_VERSION_STR(COMMON_MAJOR_VERSION,COMMON_SUB_VERSION)

#endif///_MI_COMMON_H_
