/******************************************************************************
 Copyright (C), 2015-2025 XM Tech. Co., Ltd.
******************************************************************************
File Name     : xm_type.h
Version       : Initial Draft
Author        : XM multimedia software group
Created       : 2015/9/17
Last Modified :
Description   : The common data type defination
  
******************************************************************************/
#ifndef __XM_TYPE_H__
#define __XM_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           XM_U8;
typedef unsigned short          XM_U16;
typedef unsigned int            XM_U32;

typedef signed char             XM_S8;
typedef short                   XM_S16;
typedef int                     XM_S32;

#ifndef _M_IX86
    typedef unsigned long long  XM_U64;
    typedef long long           XM_S64;
#else
    typedef __int64             XM_U64;
    typedef __int64             XM_S64;
#endif

typedef char                    XM_CHAR;
#define XM_VOID                 void

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
    XM_FALSE = 0,
    XM_TRUE  = 1,
} XM_BOOL;

#ifndef NULL
    #define NULL    0L
#endif

#define XM_NULL     0L
#define XM_SUCCESS  0
#define XM_FAILURE  (-1)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __XM_TYPE_H__ */

