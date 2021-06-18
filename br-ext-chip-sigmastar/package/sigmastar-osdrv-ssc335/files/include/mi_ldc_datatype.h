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

#ifndef _MI_LDC_DATATYPE_H_
#define _MI_LDC_DATATYPE_H_
#include "mi_sys_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    E_MI_LDC_ERR_DEV_CREATED = MI_LDC_INITIAL_ERROR_CODE,           // dev has been created
    E_MI_LDC_ERR_DEV_NOT_CREATE,                                    // dev not be created
    E_MI_LDC_ERR_DEV_NOT_DESTROY,                                   // dev not be destroyed
    E_MI_LDC_ERR_CHN_CREATED,                                       // chn has been created
    E_MI_LDC_ERR_CHN_NOT_CREATE,                                    // chn not be created
    E_MI_LDC_ERR_CHN_NOT_STOP,                                      // chn is still working
    E_MI_LDC_ERR_CHN_NOT_DESTROY,                                   // chn not be destroyed
    E_MI_LDC_ERR_PORT_NOT_UNBIND,                                   // port not unbind
} MI_LDC_ErrCode_e;

typedef MI_U32 MI_LDC_DEV;
typedef MI_U32 MI_LDC_CHN;

typedef struct MI_LDC_OutputPortAttr_s
{
    MI_U16 u16Width;
    MI_U16 u16Height;
    MI_SYS_PixelFormat_e  ePixelFmt;
} MI_LDC_OutputPortAttr_t;

typedef struct MI_LDC_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_LDC_InitParam_t;


#define MI_LDC_OK                      MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_INFO, MI_SUCCESS)
#define MI_ERR_LDC_ILLEGAL_PARAM       MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM)
#define MI_ERR_LDC_NULL_PTR            MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR)
#define MI_ERR_LDC_BUSY                MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY)
#define MI_ERR_LDC_FAIL                MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED)
#define MI_ERR_LDC_INVALID_DEVID       MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID)
#define MI_ERR_LDC_NOT_SUPPORT         MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT)
#define MI_ERR_LDC_MOD_INITED          MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INITED)
#define MI_ERR_LDC_MOD_NOT_INIT        MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_INIT)
#define MI_ERR_LDC_DEV_CREATED         MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_DEV_CREATED)
#define MI_ERR_LDC_DEV_NOT_CREATE      MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_DEV_NOT_CREATE)
#define MI_ERR_LDC_DEV_NOT_DESTROY     MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_DEV_NOT_DESTROY)
#define MI_ERR_LDC_CHN_CREATED         MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_CHN_CREATED)
#define MI_ERR_LDC_CHN_NOT_CREATE      MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_CHN_NOT_CREATE)
#define MI_ERR_LDC_CHN_NOT_STOP        MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_CHN_NOT_STOP)
#define MI_ERR_LDC_CHN_NOT_DESTROY     MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_CHN_NOT_DESTROY)
#define MI_ERR_LDC_PORT_NOT_DISABLE    MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_DISABLE)
#define MI_ERR_LDC_PORT_NOT_UNBIND     MI_DEF_ERR(E_MI_MODULE_ID_LDC, E_MI_ERR_LEVEL_ERROR, E_MI_LDC_ERR_PORT_NOT_UNBIND)


#ifdef __cplusplus
}
#endif

#endif///_MI_VPE_DATATYPE_H_
