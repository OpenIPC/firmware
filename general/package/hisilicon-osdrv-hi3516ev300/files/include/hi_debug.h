/******************************************************************************
Copyright (C), 2016, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_debug.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2016/07/15
Last Modified :
Description   : all struct and maro definition for debug
Function List :
******************************************************************************/
#ifndef __HI_DEBUG_H__
#define __HI_DEBUG_H__

#ifndef __KERNEL__
#include <stdio.h>
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

/* #ifdef HI_DEBUG */
#ifdef CONFIG_HI_LOG_TRACE_SUPPORT
    /* Using samples:   HI_ASSERT(x>y); */
    #define HI_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            printf("\nASSERT at:\n"       \
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                   __FUNCTION__, __LINE__, #expr);\
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
//#include "hi_osal.h"
#define HI_PRINT osal_printk

extern HI_S32 HI_ChkLogLevel(HI_S32 s32Levle, MOD_ID_E enModId);

int HI_LOG(HI_S32 level, MOD_ID_E enModId,const char *fmt, ...) __attribute__((format(printf,3,4)));

/* #ifdef HI_DEBUG */
#ifdef CONFIG_HI_LOG_TRACE_SUPPORT
    /* Using samples:   HI_ASSERT(x>y); */
    #define HI_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            osal_panic("\nASSERT at:\n"   \
                  "  >Function : %s\n"    \
                  "  >Line No. : %d\n"    \
                  "  >Condition: %s\n",   \
                  __FUNCTION__, __LINE__, #expr);\
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


#define HI_PRINT_BLOCK(pu8Datablock, u32Length)  \
{  \
    HI_U32 u32ii = 0;  \
    HI_U8* pu8VirAddr = (HI_U8*)(pu8Datablock);  \
    if(HI_NULL != pu8VirAddr)\
    {\
        HI_PRINT("\n[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, #pu8Datablock);  \
        for (u32ii = 0; u32ii < (u32Length);)  \
        {  \
            HI_PRINT(" %02X", *pu8VirAddr);\
            pu8VirAddr++;\
            u32ii++;\
            if(0 == (u32ii % 16))HI_PRINT("\n");\
        }  \
        HI_PRINT("\n\n");\
    }\
    else\
    {\
        HI_PRINT("\n[Func]:%s [Line]:%d [Info]:pointer(%s) is null!\n", __FUNCTION__, __LINE__, #pu8Datablock);  \
    }\
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DEBUG_H__ */

