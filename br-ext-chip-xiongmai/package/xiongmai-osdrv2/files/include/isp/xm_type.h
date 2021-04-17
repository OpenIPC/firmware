/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_type.h
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
#ifndef __XM_TYPE_H__
#define __XM_TYPE_H__


/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           XM_U8;
typedef unsigned short          XM_U16;
typedef unsigned int            XM_U32;

typedef char             		XM_S8;
typedef short                   XM_S16;
typedef int                     XM_S32;

typedef unsigned long long  	XM_U64;
typedef long long           	XM_S64;

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


#endif /* __XM_TYPE_H__ */

