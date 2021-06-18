/*
* mi_sed_datatype.h- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: XXXX <XXXX@sigmastar.com.cn>
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/
#ifndef __MI_SED_DATATYPE_
#define __MI_SED_DATATYPE_

#include "mi_common_datatype.h"
#include "mi_sys_datatype.h"



#define SED_MAX_CHN_NUM    (64)
#define SED_MAX_ROI_NUM_PER_CHN    (16)
#define SED_MAX_TARGET_CHN_NUM_PER_CHN    (8)
#define SED_MAX_CUS_DEF_ALGOPARAM_NUM    (10)

#define MI_DEF_SED_ERR(err) MI_DEF_ERR(29, E_MI_ERR_LEVEL_ERROR, err)

/* invalid channel ID */
#define MI_ERR_SED_INVALID_CHNID     MI_DEF_SED_ERR(E_MI_ERR_INVALID_CHNID)
/* at lease one parameter is illegal, e.g, an illegal enumeration value  */
#define MI_ERR_SED_ILLEGAL_PARAM     MI_DEF_SED_ERR(E_MI_ERR_ILLEGAL_PARAM)
/* channel exists */
#define MI_ERR_SED_EXIST             MI_DEF_SED_ERR(E_MI_ERR_EXIST)
/*UN exist*/
#define MI_ERR_SED_UNEXIST           MI_DEF_SED_ERR(E_MI_ERR_UNEXIST)
/* using a NULL point */
#define MI_ERR_SED_NULL_PTR          MI_DEF_SED_ERR(E_MI_ERR_NULL_PTR)
/* failure caused by malloc memory */
#define MI_ERR_SED_NOMEM             MI_DEF_SED_ERR(E_MI_ERR_NOMEM)
/* channel not start*/
#define MI_ERR_SED_CHN_NOT_STARTED   MI_DEF_SED_ERR(E_MI_ERR_CHN_NOT_STARTED)
/* channel not stop*/
#define MI_ERR_SED_CHN_NOT_STOPED   MI_DEF_SED_ERR(E_MI_ERR_CHN_NOT_STOPED)
/* channedl not enable*/
#define MI_ERR_SED_NOT_ENABLE 		MI_DEF_SED_ERR(E_MI_ERR_NOT_ENABLE)

//==== Variables ====
typedef MI_S32 MI_SED_TARGET_CHN;

//==== enumerates ====
typedef enum
{
    E_MI_IVEOBJDETECT_ALGOPARAM = 0x0,//MD based object detection, for ROI object tracking
    E_MI_CNNOBJDETECT_ALGOPARAM = 0x1,//CNN based object detection, for ROI object tracking
    E_MI_MOTIONDETECT_ALGOPARAM = 0x2,//IVE cased motion detection, for AVBR motion detection
    E_MI_ALGOPARAM_MAX,
} MI_SED_AlgoType_e;

//==== Structures ====
typedef struct MI_SED_InputAttr_s
{
    MI_U32 u32Width;
    MI_U32 u32Height;
    MI_U32 u32FrameRateNum;
    MI_U32 u32FrameRateDen;
    MI_SYS_ChnPort_t stInputPort;
} MI_SED_InputAttr_t;


typedef struct MI_SED_IveObjDetect_Algo_s
{
    MI_U32 u32VdfChn;
    MI_U8 u8Sensitivity;
} MI_SED_IveObjDetect_Algo_t;

typedef struct MI_SED_CNNObjDetect_Algo_s
{
    MI_U32 u32MaxVariableBufSize;
    MI_U32 u32IPUChnId;
    MI_U8 *u8FwImagePath;
    MI_U8 *u8ModelImagePath;
} MI_SED_CNNObjDetect_Algo_t;


typedef struct MI_SED_MotionObjDetect_Algo_s
{
    MI_IVE_HANDLE iveHandle;

}MI_SED_MotionObjDetect_Algo_t;

typedef struct MI_SED_Rect_s
{
    MI_U32 u32Left;
    MI_U32 u32Top;
    MI_U32 u32Width;
    MI_U32 u32Height;
} MI_SED_Rect_t;

typedef struct MI_SED_RectInfo_s
{
    MI_U32 u32RectCount;
    MI_SED_Rect_t stRect[SED_MAX_ROI_NUM_PER_CHN];
} MI_SED_RectInfo_t;

typedef struct MI_SED_AlgoAttr_t
{
    MI_SED_AlgoType_e eType;
    union
    {
        MI_SED_IveObjDetect_Algo_t stIveObjDetectAlgo;
        MI_SED_CNNObjDetect_Algo_t stCNNObjDetectAlgo;
        MI_SED_MotionObjDetect_Algo_t stMotionObjDetectAlgo;
    };
} MI_SED_AlgoAttr_t;

typedef struct MI_SED_TargetAttr_s
{
    MI_S32 s32RltQp;
} MI_SED_TargetAttr_t;

typedef struct MI_SED_DetectorAttr_s
{
    //need implement
    //include: input yuv width/height
    //         MD/OD Algo, CNN human detec
    //         Max ROI number
    //         Background Foreground
    //         framerate
    MI_SED_InputAttr_t stInputAttr;
    MI_SED_AlgoAttr_t stAlgoAttr;    //Need custom algo result
    MI_SED_TargetAttr_t stTargetAttr;   //Target attribute for smart encoding
} MI_SED_DetectorAttr_t;

#endif
