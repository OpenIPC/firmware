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
#ifndef __MI_OD_H__
#define __MI_OD_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
typedef enum _MI_OD_RET_E
{
    MI_OD_RET_SUCCESS                        = 0x00000000,    /*OD API execution success*/

    MI_OD_RET_INIT_ERROR                    = 0x10000501,   /*OD init error*/
    MI_OD_RET_IC_CHECK_ERROR                = 0x10000502,   /*Incorrect platform check for OD*/
    MI_OD_RET_INVALID_HANDLE                = 0x10000503,   /*Invalid OD handle*/
    MI_OD_RET_INVALID_PARAMETER             = 0x10000504,   /*Invalid OD parameter*/
    MI_OD_RET_INVALID_WINDOW                = 0x10000505,   /*Invalid window*/
    MI_OD_RET_INVALID_COLOR_TYPE            = 0x10000506,   /*Invalid color tpye*/
} MI_OD_RET;
typedef enum _MI_OD_WIN_STATE
{
     MI_OD_WIN_STATE_NON_TAMPER = 0,
     MI_OD_WIN_STATE_TAMPER = 1,
     MI_OD_WIN_STATE_NO_FEATURE = 2,
     MI_OD_WIN_STATE_FAIL = -1,
} MI_OD_WIN_STATE;

typedef enum
{
    OD_Y = 1,
    OD_COLOR_MAX
} ODColor_e;

typedef enum
{
    OD_WINDOW_1X1 = 0,
    OD_WINDOW_2X2,
    OD_WINDOW_3X3,
    OD_WINDOW_MAX
} ODWindow_e;

typedef struct ODPoint_s
{
    uint16_t x;
    uint16_t y;
} ODPoint_t;

typedef struct ODROI_s
{
    uint8_t num;
    ODPoint_t pnt[8];
} ODROI_t;

typedef struct MI_OD_IMG_s
{
    void *pu32PhyAddr;
    uint8_t *pu8VirAddr;
} MI_OD_IMG_t;

typedef struct MI_OD_static_param_s
{
    uint16_t inImgW;
    uint16_t inImgH;
	uint32_t inImgStride;
    ODColor_e nClrType;
    ODWindow_e div;
    ODROI_t roi_od;
    int32_t alpha;
    int32_t M;
    int32_t MotionSensitivity;

} MI_OD_static_param_t;

typedef struct MI_OD_param_s
{
    int32_t thd_tamper;
    int32_t tamper_blk_thd;
    int32_t min_duration;
} MI_OD_param_t;

typedef void* OD_HANDLE;

uint32_t MI_OD_GetLibVersion();
OD_HANDLE MI_OD_Init(MI_OD_static_param_t *static_param, MI_OD_param_t *param);
int32_t MI_OD_Run(OD_HANDLE odHandle, const MI_OD_IMG_t* yImage);
MI_OD_RET MI_OD_GetDetectWindowSize(OD_HANDLE odHandle, uint16_t* st_x, uint16_t* st_y, uint16_t* div_w, uint16_t* div_h);
MI_OD_WIN_STATE    MI_OD_GetWindowResult(OD_HANDLE odHandle, uint8_t col, uint8_t row);
MI_OD_RET MI_OD_SetAttr(OD_HANDLE odHandle, MI_OD_param_t *param);
MI_OD_RET MI_OD_GetAttr(OD_HANDLE odHandle, MI_OD_param_t *param);
void MI_OD_Uninit(OD_HANDLE odHandle);
#ifdef __cplusplus
}
#endif

#endif //__MI_OD_H__
