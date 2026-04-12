/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_isp_debug.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/08/30
  Description   : 
  History       :
  1.Date        : 2010/08/30
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_ISP_DEBUG_H__
#define __HI_ISP_DEBUG_H__

#include "hi_debug.h"

#if 0
#define PRINT_DEBUG_INFO
#endif

#if 0
#define PRINT_INFO_2FILE
#endif

#define ISP_TRACE(level, fmt...)\
    do{ \
            HI_TRACE(level,HI_ID_ISP,"[Func]:%s [Line]:%d [Info]:",__FUNCTION__, __LINE__);\
            HI_TRACE(level,HI_ID_ISP,##fmt);\
    }while(0)

/* To avoid divide-0 exception in code. */
#define DIV_0_TO_1(a)   ( (0 == (a)) ? 1 : (a) )

#endif     /* __HI_ISP_DEBUG_H__ */            

