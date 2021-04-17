/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_isp_debug.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/6/27

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/6/27
  Author		: Lycai
  Modification	: creat
******************************************************************************/


#ifndef __XM_ISP_DEBUG_H__
#define __XM_ISP_DEBUG_H__

#include "xm_debug.h"

#if 0
#define PRINT_DEBUG_INFO
#endif

#if 0
#define PRINT_INFO_2FILE
#endif

#define ISP_TRACE(level, fmt...)\
    do{ \
            XM_TRACE(level,XM_ID_ISP,"[Func]:%s [Line]:%d [Info]:",__FUNCTION__, __LINE__);\
            XM_TRACE(level,XM_ID_ISP,##fmt);\
    }while(0)

/* To avoid divide-0 exception in code. */
#define DIV_0_TO_1(a)   ( (0 == (a)) ? 1 : (a) )

#endif     /* __XM_ISP_DEBUG_H__ */            

