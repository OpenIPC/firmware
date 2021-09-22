/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/
#include "hi_common.h"

#ifndef __HI_VOU_EXP_H__
#define __HI_VOU_EXP_H__

typedef struct hiVOU_EXPORT_CALLBACK_S {
    void (*pfnVoNotify)(int module_id, int vodev);
} VOU_EXPORT_CALLBACK_S;

typedef HI_S32 FN_VO_RegisterExternCallback(VOU_EXPORT_CALLBACK_S *pstExpCallback);

typedef struct hiVOU_EXPORT_SYMBOL_S {
    FN_VO_RegisterExternCallback *pfnVoRegisterExpCallback;
} VOU_EXPORT_SYMBOL_S;

#endif /* __HI_VOU_EXP_H__ */


