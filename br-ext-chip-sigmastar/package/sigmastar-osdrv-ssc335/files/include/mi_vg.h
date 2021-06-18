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

/*
 *      mi_vg.h
 *      Author: chiao.su
 */

#ifndef MI_VG_H_
#define MI_VG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define VG_MAJOR_VERSION 2
#define VG_SUB_VERSION 3
#define MACRO_TO_STR(macro) #macro
#define VG_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_vg_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_vg_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_vg_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_VG_API_VERSION VG_VERSION_STR(VG_MAJOR_VERSION,VG_SUB_VERSION)

#define MAX_NUMBER 4

typedef enum _MI_VG_RET_E
{
    MI_VG_RET_SUCCESS                              = 0x00000000,   /*VG Success*/
    MI_VG_RET_INIT_ERROR                           = 0x10000301,   /*VG init error*/
    MI_VG_RET_IC_CHECK_ERROR                       = 0x10000302,   /*VG platform check error*/
    MI_VG_RET_INVALID_USER_INFO_POINTER            = 0x10000303,   /*Invalid user information pointer*/
    MI_VG_RET_INVALID_FUNCTION_STATE               = 0x10000304,   /*Invalid function state*/
    MI_VG_RET_INVALID_OBJECT_THRESHOLD             = 0x10000305,   /*Invalid object threshold*/
    MI_VG_RET_INVALID_THRESHOLD_POINTER            = 0x10000306,   /*Invalid threshold pointer*/
    MI_VG_RET_INVALID_ENVIRONMENT_STATE            = 0x10000307,   /*Invalid environment state*/
    MI_VG_RET_INVALID_ENVIRONMENT_POINTER          = 0x10000308,   /*Invalid environment pointer*/
    MI_VG_RET_INVALID_LINE_NUMBER                  = 0x10000309,   /*Invalid line number*/
    MI_VG_RET_INVALID_LINE_POINTER                 = 0x1000030A,   /*Invalid line pointer*/
    MI_VG_RET_INVALID_LINE_COORDINATE              = 0x1000030B,   /*Invalid line coordinate*/
    MI_VG_RET_INVALID_LINE_COORDINATE_POINTER      = 0x1000030C,   /*Invalid line coordinate pointer*/
    MI_VG_RET_INVALID_LINE_MAGIC_NUMBER            = 0x1000030D,   /*Invalid line magic number*/
    MI_VG_RET_INVALID_REGION_COORDINATE_POINTER    = 0x1000030E,   /*Invalid region coordinate pointer*/
    MI_VG_RET_INVALID_REGION_MAGIC_NUMBER          = 0x1000030F,   /*Invalid region magic number*/
    MI_VG_RET_INVALID_REGION_COORDINATE            = 0x10000310,   /*Invalid region coordinate*/
    MI_VG_RET_INVALID_HANDLE                       = 0x10000311,   /*Invalid VG handle*/
    MI_VG_RET_INVALID_HANDLE_MAGIC_NUMBER          = 0x10000312,   /*Invalid handle magic number*/
    MI_VG_RET_INVALID_INPUT_POINTER                = 0x10000313,   /*Invalid input pointer*/
    MI_VG_RET_OPERATE_ERROR                        = 0x10000314,   /*VG operate error*/
    MI_VG_RET_INVALID_ALARM_POINTER                = 0x10000315,   /*Invalid alarm pointer*/
    MI_VG_RET_INVALID_DEBUG_POINTER                = 0x10000316,   /*Invalid debug pointer*/
    MI_VG_RET_INVALID_DETECT_PARAMETER_POINTER     = 0x10000317,   /*Invalid detect parameters pointer*/
    MI_VG_RET_INVALID_FUNCTION_SWITCH              = 0x10000318,   /*Invalid function switch*/
    MI_VG_RET_INVALID_DETECT_THRESHOLD             = 0x10000319    /*Invalid detection threshold*/
} MI_VG_RET;

typedef enum _VgFunction
{
    VG_VIRTUAL_GATE      = 2,
    VG_REGION_INVASION   = 3
} VgFunction;

typedef enum _VgRegion_Dir
{
    VG_REGION_ENTER      = 0,
    VG_REGION_LEAVING    = 1,
    VG_REGION_CROSS      = 2
} VgRegion_Dir;

typedef enum _VgSize_Sensitively
{
    VG_SENSITIVELY_MIN       = 0,
    VG_SENSITIVELY_LOW       = 1,
    VG_SENSITIVELY_MIDDLE    = 2,
    VG_SENSITIVELY_HIGH      = 3,
    VG_SENSITIVELY_MAX       = 4
} VgSize_Sensitively;

typedef struct _VG_Point_t
{
    int32_t x;
    int32_t y;
} MI_VG_Point_t;

typedef struct _VG_Line_t
{
    MI_VG_Point_t px;   //point x
    MI_VG_Point_t py;   //point y
    MI_VG_Point_t pdx;  //point direction x
    MI_VG_Point_t pdy;  //point direction y
} MI_VgLine_t;

typedef struct _VG_Region_t
{
    MI_VG_Point_t p_one;    //point one
    MI_VG_Point_t p_two;    //point two
    MI_VG_Point_t p_three;  //point three
    MI_VG_Point_t p_four;   //point four

    int region_dir;         //Region direction;
} MI_VgRegion_t;

typedef struct _MI_VgSet_t
{
    //Common Information
    float object_size_thd;
    uint16_t line_number;
    uint8_t indoor;

    //Line info
    MI_VG_Point_t fp[MAX_NUMBER];   //First point
    MI_VG_Point_t sp[MAX_NUMBER];   //Second point
    MI_VG_Point_t fdp[MAX_NUMBER];  //First direction point
    MI_VG_Point_t sdp[MAX_NUMBER];  //Second direction point

    //Function
    uint8_t function_state;

    //Region info
    MI_VG_Point_t first_p;   //First point
    MI_VG_Point_t second_p;  //Second point
    MI_VG_Point_t third_p;   //Third point
    MI_VG_Point_t fourth_p;  //Fourth point

    //Region direction
    uint8_t region_direction;

    //Magic_number
    int32_t magic_number;
} MI_VgSet_t;

typedef struct _MI_VgBoundingBox_t
{
    int32_t up;       //Min y
    int32_t down;     //Max y
    int32_t left;     //Min x
    int32_t right;    //Max x
} MI_VgBoundingBox_t;

typedef struct _MI_VgResult_t
{
    int32_t alarm[MAX_NUMBER];
    int32_t alarm_cnt;
    MI_VgBoundingBox_t bounding_box[20];
} MI_VgResult_t;

typedef struct _MI_VdDetectThd_t
{
    uint8_t function_switch;
    uint8_t detect_thd;
} MI_VgDetectThd;

typedef struct _MI_VgDebug_t
{
    //Common Information
    uint16_t background_state;
    uint32_t version;  //Modify date
    uint32_t debug_object_size;
    uint32_t debug_state;

    //Line info
    MI_VG_Point_t dfp[MAX_NUMBER];   //Debug First point
    MI_VG_Point_t dsp[MAX_NUMBER];   //Debug Second point
    MI_VG_Point_t dfdp[MAX_NUMBER];  //Debug First direction point
    MI_VG_Point_t dsdp[MAX_NUMBER];  //Debug Second direction point
} MI_VgDebug_t;

typedef  void*  MI_VG_HANDLE;

MI_VG_HANDLE MI_VG_Init(MI_VgSet_t* vg_user_info, uint16_t width, uint16_t height);

MI_VG_RET MI_VG_Uninit(MI_VG_HANDLE vg_handle);

MI_VG_RET MI_VG_Reset(MI_VG_HANDLE vg_handle);

MI_VG_RET MI_VG_SetFunction(MI_VgSet_t* vg_user_info, uint8_t function_state);

MI_VG_RET MI_VG_SetObjSizeThd(MI_VgSet_t* vg_user_info, float size_thd);

MI_VG_RET MI_VG_GetObjSizeThd(MI_VgSet_t* vg_user_info, float* size_thd);

MI_VG_RET MI_VG_SetScene(MI_VgSet_t* vg_user_info, int8_t scene);

MI_VG_RET MI_VG_GetScene(MI_VgSet_t* vg_user_info, int8_t* scene);

MI_VG_RET MI_VG_SetLineNumber(MI_VgSet_t* vg_user_info, uint16_t lineno);

MI_VG_RET MI_VG_GetLineNumber(MI_VgSet_t* vg_user_info, uint16_t* lineno);

MI_VG_RET MI_VG_SetLineAndDir(MI_VgSet_t* vg_user_info, MI_VgLine_t* line_coordinate, uint16_t lineno);

MI_VG_RET MI_VG_GetLineAndDir(MI_VgSet_t* vg_user_info, MI_VgLine_t* line_coordinate, uint16_t lineno);

MI_VG_RET MI_VG_SetRegionAndDir(MI_VgSet_t* vg_user_info, MI_VgRegion_t* region_coordinate);

MI_VG_RET MI_VG_SetDetectThd(MI_VG_HANDLE vg_handle, MI_VgDetectThd* detect_parms);

MI_VG_RET MI_VG_Run(MI_VG_HANDLE vg_handle, uint8_t* _ucMask);

MI_VG_RET MI_VG_GetResult(MI_VG_HANDLE vg_handle, MI_VgResult_t *cross_alarm);

MI_VG_RET MI_VG_GetDebugInfo(MI_VG_HANDLE vg_handle, MI_VgDebug_t *debug_info);


#ifdef __cplusplus
}
#endif

#endif /* MI_VG_H_ */
