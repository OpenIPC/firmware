/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_debug.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2006/12/09
Last Modified :
Description   : all struct and maro definition for debug
Function List :
History       :
 1.Date        : 2006/11/03
   Author      : c42025
   Modification: Created file

2.Date        : 2008/03/03
  Author      : c42025
  Modification: 1. mv definition LOG_ERRLEVEL_E to here form file "hi_errno.h", 
  but it will be obsolete in next version.
  2. add new macro definition for debug levle.
  3. change macro defintion HI_TRACE

3.Date        : 2008/03/05
  Author      : c42025
  Modification: Change the definition of  LOG_ERRLEVEL_E

4.Date        : 2008/10/31
  Author      : z44949
  Modification: Delete the definition of  LOG_ERRLEVEL_E

5.Date        :   2010/11/03
  Author      :   z44949
  Modification:   Remove some unnecessary typedef
   
******************************************************************************/
#ifndef __HI_DEBUG_H__
#define __HI_DEBUG_H__

#ifndef __KERNEL__
#include <stdarg.h>
#endif

#include "hi_type.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define _EX__FILE_LINE(fxx,lxx) "[File]:"fxx"\n[Line]:"#lxx"\n[Info]:"
#define EX__FILE_LINE(fxx,lxx) _EX__FILE_LINE(fxx,lxx)
#define __FILE_LINE__ EX__FILE_LINE(__FILE__, __LINE__)

#define HI_DBG_EMERG      0   /* system is unusable                   */
#define HI_DBG_ALERT      1   /* action must be taken immediately     */
#define HI_DBG_CRIT       2   /* critical conditions                  */
#define HI_DBG_ERR        3   /* error conditions                     */
#define HI_DBG_WARN       4   /* warning conditions                   */
#define HI_DBG_NOTICE     5   /* normal but significant condition     */
#define HI_DBG_INFO       6   /* informational                        */
#define HI_DBG_DEBUG      7   /* debug-level messages                 */

typedef struct hiLOG_LEVEL_CONF_S
{
    MOD_ID_E  enModId;
    HI_S32    s32Level;
    HI_CHAR   cModName[16];
} LOG_LEVEL_CONF_S;

#ifndef __KERNEL__ 
/******************************************************************************
** For User Mode : HI_PRINT, HI_ASSERT, HI_TRACE
******************************************************************************/

#define HI_PRINT printf

#ifdef HI_DEBUG
    /* Using samples:   HI_ASSERT(x>y); */
    #define HI_ASSERT(expr)               \
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
    ** HI_TRACE(HI_DBG_DEBUG, HI_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define HI_TRACE(level, enModId, fmt...) fprintf(stderr,##fmt)
#else
    #define HI_ASSERT(expr)
    #define HI_TRACE(level, enModId, fmt...)
#endif

#else
/******************************************************************************
** For Linux Kernel : HI_PRINT, HI_ASSERT, HI_TRACE
******************************************************************************/

#define HI_PRINT printk

extern HI_S32 HI_ChkLogLevel(HI_S32 s32Levle, MOD_ID_E enModId);
asmlinkage int HI_LOG(HI_S32 level, MOD_ID_E enModId,const char *fmt, ...);

#ifdef HI_DEBUG
    /* Using samples:   HI_ASSERT(x>y); */
    #define HI_ASSERT(expr)               \
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
    ** HI_TRACE(HI_DBG_DEBUG, HI_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define HI_TRACE HI_LOG
#else
    #define HI_ASSERT(expr)
    #define HI_TRACE(level, enModId, fmt...)
#endif

#endif  /* end of __KERNEL__ */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DEBUG_H__ */

