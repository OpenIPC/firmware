/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_errno.h
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
#ifndef __XM_ERRNO_H__
#define __XM_ERRNO_H__

#include "xm_debug.h"




#define XM_ERR_APPID  (0x80000000L + 0x20000000L)

typedef enum xm_ERR_LEVEL_E
{
    EN_ERR_LEVEL_DEBUG = 0,  /* debug-level                                  */
    EN_ERR_LEVEL_INFO,       /* informational                                */
    EN_ERR_LEVEL_NOTICE,     /* normal but significant condition             */
    EN_ERR_LEVEL_WARNING,    /* warning conditions                           */
    EN_ERR_LEVEL_ERROR,      /* error conditions                             */
    EN_ERR_LEVEL_CRIT,       /* critical conditions                          */
    EN_ERR_LEVEL_ALERT,      /* action must be taken immediately             */
    EN_ERR_LEVEL_FATAL,      /* just for compatibility with previous version */
    EN_ERR_LEVEL_BUTT
}ERR_LEVEL_E;


/******************************************************************************
|----------------------------------------------------------------|
| 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            |
|----------------------------------------------------------------|
|<--><--7bits----><----8bits---><--3bits---><------13bits------->|
******************************************************************************/

#define XM_DEF_ERR( module, level, errid) \
    ((XM_S32)( (XM_ERR_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

/* NOTE! the following defined all common error code,
** all module must reserved 0~63 for their common error code
*/
typedef enum xm_EN_ERR_CODE_E
{
    EN_ERR_INVALID_DEVID = 1, /* invlalid device ID                           */
    EN_ERR_INVALID_CHNID = 2, /* invlalid channel ID                          */
    EN_ERR_ILLEGAL_PARAM = 3, /* at lease one parameter is illagal
                               * eg, an illegal enumeration value             */
    EN_ERR_EXIST         = 4, /* resource exists                              */
    EN_ERR_UNEXIST       = 5, /* resource unexists                            */
    
    EN_ERR_NULL_PTR      = 6, /* using a NULL point                           */
    
    EN_ERR_NOT_CONFIG    = 7, /* try to enable or initialize system, device
                              ** or channel, before configing attribute       */

    EN_ERR_NOT_SUPPORT   = 8, /* operation or type is not supported by NOW    */
    EN_ERR_NOT_PERM      = 9, /* operation is not permitted
                              ** eg, try to change   attribute           */
    EN_ERR_NOT_FINISH    = 10,/* encode one frame not finish,maybe bus error,timeout,sys reset and so on*/
    EN_ERR_INSUFF_RES    = 11,/* insufficient resource */

    EN_ERR_NOMEM         = 12,/* failure caused by malloc memory              */
    EN_ERR_NOBUF         = 13,/* failure caused by malloc buffer              */

    EN_ERR_BUF_EMPTY     = 14,/* no data in buffer                            */
    EN_ERR_BUF_FULL      = 15,/* no buffer for new data                       */

    EN_ERR_SYS_NOTREADY  = 16,/* System is not ready,maybe not initialed or 
                              ** loaded. Returning the error code when opening
                              ** a device file failed.                        */

    EN_ERR_BADADDR       = 17,/* bad address, 
                              ** eg. used for copy_from_user & copy_to_user   */

    EN_ERR_BUSY          = 18,/* resource is busy, 
                              ** eg. destroy a venc chn without unregister it */
	EN_ERR_HW_FAILED     = 19,/*encoder hardware failed*/
	EN_ERR_UND           = 20,/* undefined error */
	
    EN_ERR_BUTT          = 63,/* maxium code, private error code of all modules
                              ** must be greater than it                      */
}EN_ERR_CODE_E;


/* 
** following is an example for defining error code of VDA module
** #define XM_ERR_MD_INVALID_CHNID XM_DEF_ERR(XM_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
**
*/

#endif  /* __XM_ERRNO_H__ */

