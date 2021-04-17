/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_debug.h
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
#ifndef __XM_DEBUG_H__
#define __XM_DEBUG_H__

#include "xm_type.h"
#include "xm_common.h"


#define _EX__FILE_LINE(fxx,lxx) "[File]:"fxx"\n[Line]:"#lxx"\n[Info]:"
#define EX__FILE_LINE(fxx,lxx) _EX__FILE_LINE(fxx,lxx)
#define __FILE_LINE__ EX__FILE_LINE(__FILE__, __LINE__)

#define XM_DBG_EMERG      0   /* system is unusable                   */
#define XM_DBG_ALERT      1   /* action must be taken immediately     */
#define XM_DBG_CRIT       2   /* critical conditions                  */
#define XM_DBG_ERR        3   /* error conditions                     */
#define XM_DBG_WARN       4   /* warning conditions                   */
#define XM_DBG_NOTICE     5   /* normal but significant condition     */
#define XM_DBG_INFO       6   /* informational                        */
#define XM_DBG_DEBUG      7   /* debug-level messages                 */

typedef struct xm_LOG_LEVEL_CONF_S
{
    MOD_ID_E  enModId;
    XM_S32    s32Level;
    XM_CHAR   cModName[16];
} LOG_LEVEL_CONF_S;

#ifndef __KERNEL__ 
/******************************************************************************
** For User Mode : XM_PRINT, XM_ASSERT, XM_TRACE
******************************************************************************/

#define XM_PRINT printf

#ifdef XM_DEBUG
    /* Using samples:   XM_ASSERT(x>y); */
    #define XM_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            printf("\nASSERT failed at:\n"\
                   "  >File name: %s\n"   \
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                    __FILE__,__FUNCTION__, __LINE__, #expr);\
            _exit(-1);\
        } \
    }while(0)

    /* Using samples: 
    ** XM_TRACE(XM_DBG_DEBUG, XM_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define XM_TRACE(level, enModId, fmt...) fprintf(stderr,##fmt)
#else
    #define XM_ASSERT(expr)
    #define XM_TRACE(level, enModId, fmt...)
#endif

#else
/******************************************************************************
** For Linux Kernel : XM_PRINT, XM_ASSERT, XM_TRACE
******************************************************************************/

#define XM_PRINT printk

extern XM_S32 XM_ChkLogLevel(XM_S32 s32Levle, MOD_ID_E enModId);
asmlinkage int XM_LOG(XM_S32 level, MOD_ID_E enModId,const char *fmt, ...);

#ifdef XM_DEBUG
    /* Using samples:   XM_ASSERT(x>y); */
    #define XM_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            panic("\nASSERT failed at:\n" \
                  "  >File name: %s\n"    \
                  "  >Function : %s\n"    \
                  "  >Line No. : %d\n"    \
                  "  >Condition: %s\n",   \
                    __FILE__,__FUNCTION__, __LINE__, #expr);\
        } \
    }while(0)

    /* Using samples: 
    ** XM_TRACE(XM_DBG_DEBUG, XM_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define XM_TRACE XM_LOG
#else
    #define XM_ASSERT(expr)
    #define XM_TRACE(level, enModId, fmt...)
#endif

#endif  /* end of __KERNEL__ */


#endif /* __XM_DEBUG_H__ */

