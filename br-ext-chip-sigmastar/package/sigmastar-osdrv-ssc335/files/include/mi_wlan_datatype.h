/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef __MI_WLAN_DATATYPE_H__
#define __MI_WLAN_DATATYPE_H__
#ifdef __cplusplus
extern "C" {
#endif

#define DBG_LEVEL_FATAL       (1)
#define DBG_LEVEL_ERROR       (2)
#define DBG_LEVEL_WARN        (3)
#define DBG_LEVEL_INFO        (4)
#define DBG_LEVEL_DEBUG       (5)
#define DBG_LEVEL_ENTRY       (6)

#define DBG_LEVEL DBG_LEVEL_WARN

#define COLOR_NONE          "\033[0m"
#define COLOR_BLACK         "\033[0;30m"
#define COLOR_BLUE          "\033[0;34m"
#define COLOR_GREEN         "\033[0;32m"
#define COLOR_CYAN          "\033[0;36m"
#define COLOR_RED           "\033[0;31m"
#define COLOR_YELLOW        "\033[1;33m"
#define COLOR_WHITE         "\033[1;37m"

#define DBG_ENTRY(fmt, args...)             ({do{if(DBG_LEVEL>=DBG_LEVEL_ENTRY){printf(COLOR_CYAN"[WLAN_ENTRY]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(COLOR_NONE);}}while(0);})
#define DBG_DEBUG(fmt, args...)             ({do{if(DBG_LEVEL>=DBG_LEVEL_DEBUG){printf(COLOR_WHITE"[WLAN_BDG]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(COLOR_NONE);}}while(0);})
#define DBG_INFO(fmt, args...)              ({do{if(DBG_LEVEL>=DBG_LEVEL_INFO) {printf(COLOR_GREEN"[WLAN_INFO]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(COLOR_NONE);}}while(0);})
#define DBG_WRN(fmt, args...)              ({do{if(DBG_LEVEL>=DBG_LEVEL_WARN) {printf(COLOR_YELLOW"[WLAN_WRN]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(COLOR_NONE);}}while(0);})
#define DBG_ERR(fmt, args...)             ({do{if(DBG_LEVEL>=DBG_LEVEL_ERROR){printf(COLOR_RED"[WLAN_ERR]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(COLOR_NONE);}}while(0);})
#define DBG_FATAL(fmt, args...)             ({do{if(DBG_LEVEL>=DBG_LEVEL_FATAL){printf(COLOR_RED"[WLAN_FATAL]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(COLOR_NONE);}}while(0);})



typedef enum
{
    E_WLAN_CMD_INIT = 0,
    E_WLAN_CMD_OPEN,
    E_WLAN_CMD_CLOSE,
    E_WLAN_CMD_DEINIT,
    E_WLAN_CMD_SCAN,
    E_WLAN_CMD_CONNECT,
    E_WLAN_CMD_CONNECT_addNetWork,
    E_WLAN_CMD_DISCONNECT,
    E_WLAN_CMD_STATUS,
    E_WLAN_CMD_VERSION,
    E_WLAN_CMD_CHANNEL,
    E_WLAN_CMD_NUM
} E_WLAN_CMD;






#ifdef __cplusplus
}
#endif

#endif
