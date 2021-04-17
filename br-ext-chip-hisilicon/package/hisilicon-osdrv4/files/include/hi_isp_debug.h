/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_isp_debug.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/08/30
  Description   :
  History       :
  1.Date        : 2010/08/30
    Author      :
    Modification: Created file

******************************************************************************/

#ifndef __HI_ISP_DEBUG_H__
#define __HI_ISP_DEBUG_H__

#include "hi_debug.h"
#define ISP_TRACE(level, fmt, ...)\
    do{ \
        HI_TRACE(level, HI_ID_ISP,"[Func]:%s [Line]:%d [Info]:"fmt,__FUNCTION__, __LINE__,##__VA_ARGS__);\
    }while(0)

/* To avoid divide-0 exception in code. */
#define DIV_0_TO_1(a)   ( ((a) == 0) ? 1 : (a) )
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))

#endif     /* __HI_ISP_DEBUG_H__ */
