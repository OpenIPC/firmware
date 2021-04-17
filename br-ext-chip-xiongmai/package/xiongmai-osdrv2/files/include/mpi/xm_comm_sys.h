/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : xm_comm_sys.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/

#ifndef __XM_COMM_SYS_H__
#define __XM_COMM_SYS_H__

#include "xm_type.h"
#include "xm_errno.h"
#include "xm_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define XM_TRACE_SYS(level, fmt...) XM_TRACE(level, XM_ID_SYS,##fmt)
typedef struct xmMPP_SYS_CONF_S
{
    /* stride of picture buffer must be aligned with this value.
     * you can choose a value from 1 to 1024, and it must be multiple of 16.
     */
    XM_U32 u32AlignWidth;  

}MPP_SYS_CONF_S;


#define XM_ERR_SYS_NULL_PTR         XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define XM_ERR_SYS_NOTREADY         XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define XM_ERR_SYS_NOT_PERM         XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define XM_ERR_SYS_NOMEM            XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define XM_ERR_SYS_ILLEGAL_PARAM    XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define XM_ERR_SYS_BUSY             XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define XM_ERR_SYS_NOT_SUPPORT      XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define XM_ERR_SYS_UNEXIST      XM_DEF_ERR(XM_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __XM_COMM_SYS_H__ */

