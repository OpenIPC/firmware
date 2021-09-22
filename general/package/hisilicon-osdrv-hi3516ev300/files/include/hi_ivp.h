/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: defination of hi_ivp.h
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2018/10/26
 */
#ifndef _HI_IVP_H_
#define _HI_IVP_H_

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define HI_IVP_MAX_VENC_CHN_NUM 16
#define HI_IVP_MAX_VIPIPE_NUM 2
#define HI_IVP_MAX_CLASS 2
#define HI_IVP_CLASS_NAME_LEN 32

typedef struct{
    hi_u64  physical_addr; /* RW;The physical address of the memory */
    hi_u64  virtual_addr; /* RW;The virtual address of the memory */
    hi_u32  memory_size;    /* RW;The size of memory */
}hi_ivp_mem_info;

typedef struct{
    hi_float threshold;
}hi_ivp_ctrl_attr;

typedef struct{
    hi_s32 x;
    hi_s32 y;
    hi_u32 width;
    hi_u32 height;
}hi_ivp_rect;

typedef struct{
	hi_ivp_rect rect;
	hi_float quality;
}hi_ivp_obj;

typedef struct{
    hi_char class_name[HI_IVP_CLASS_NAME_LEN];
    hi_u32 rect_num;
    hi_u32 rect_capcity;
    hi_ivp_obj *objs;
}hi_ivp_obj_of_one_class;

typedef struct{
    hi_s32 class_num;
    hi_ivp_obj_of_one_class obj_class[HI_IVP_MAX_CLASS];
}hi_ivp_obj_array;

typedef struct{
    hi_bool enable;
    hi_u32 threshold; /* RW;Range: [1,1024] */
}hi_ivp_roi_attr;

typedef enum{
    HI_IVP_ROI_MB_MODE_4X4,
    HI_IVP_ROI_MB_MODE_8X8,
    HI_IVP_ROI_MB_MODE_16X16,
    HI_IVP_ROI_MB_MODE_BUTT
}hi_ivp_roi_mb_mode;

typedef struct{
    hi_ivp_roi_mb_mode roi_mb_mode;
    hi_u32 img_width; /* equal to the width of processing frame */
    hi_u32 img_height; /* equal to the height of processing frame */
    hi_u8 *mb_map; /* for 4x4 mb mode, alloc (DIV_UP(img_width,4))*(DIV_UP(img_height,4)) bytes */
}hi_ivp_roi_map;

/*****************************************************************************
*   Prototype    : hi_ivp_init
*   Description  : Intelligent Video Process(IVP) initialization.
*   Parameters   : HI_VOID
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_init(hi_void);

/*****************************************************************************
*   Prototype    : hi_ivp_deinit
*   Description  : Intelligent Video Process(IVP) exit.
*   Parameters   : HI_VOID.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_deinit(hi_void);

/*****************************************************************************
*   Prototype    : hi_ivp_load_resource_from_memory
*   Description  : Load resource from memory.
*   Parameters   : const hi_ivp_mem_info *ivp_file_mem_info:Input
                   hi_s32 *ivp_handle:Output
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_load_resource_from_memory(const hi_ivp_mem_info *ivp_file_mem_info, hi_s32 *ivp_handle);

/*****************************************************************************
*   Prototype    : hi_ivp_unload_resource
*   Description  : Unload resource.
*   Parameters   : hi_s32 ivp_handle:Input
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_unload_resource(hi_s32 ivp_handle);

/*****************************************************************************
*   Prototype    : hi_ivp_set_ctrl_attr
*   Description  : Set ctrl param,include SMD threshold.
*   Parameters   : hi_s32 ivp_handle:Input
                   const hi_ivp_ctrl_attr *ivp_ctrl_attr:input
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_set_ctrl_attr(hi_s32 ivp_handle, const hi_ivp_ctrl_attr *ivp_ctrl_attr);

/*****************************************************************************
*   Prototype    : hi_ivp_get_ctrl_attr
*   Description  : Get ctrl param,include SMD threshold.
*   Parameters   : hi_s32 ivp_handle:Input
                   hi_ivp_ctrl_attr *ivp_ctrl_attr:Output
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_get_ctrl_attr(hi_s32 ivp_handle, hi_ivp_ctrl_attr *ivp_ctrl_attr);

/*****************************************************************************
*   Prototype    : hi_ivp_set_fd_ctrl_attr
*   Description  : Set FD ctrl param,include FD threshold. Only Hi3518EV300 support.
*   Parameters   : hi_s32 ivp_handle:Input
                   const hi_ivp_ctrl_attr *ivp_ctrl_attr:input
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2019/05/22
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_set_fd_ctrl_attr(hi_s32 ivp_handle, const hi_ivp_ctrl_attr *ivp_ctrl_attr);

/*****************************************************************************
*   Prototype    : hi_ivp_get_fd_ctrl_attr
*   Description  : Get FD ctrl param,include FD threshold. Only Hi3518EV300 support.
*   Parameters   : hi_s32 ivp_handle:Input
                   hi_ivp_ctrl_attr *ivp_ctrl_attr:Output
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2019/05/22
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_get_fd_ctrl_attr(hi_s32 ivp_handle, hi_ivp_ctrl_attr *ivp_ctrl_attr);

/*****************************************************************************
*   Prototype    : hi_ivp_set_venc_low_bitrate
*   Description  : Enable or disable venc low bitrate.
*   Parameters   : hi_s32 ivp_handle:Input
                   hi_s32 venc_chn:Input
                   hi_bool enable:Input
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_set_venc_low_bitrate(hi_s32 ivp_handle, hi_s32 venc_chn, hi_bool enable);

/*****************************************************************************
*   Prototype    : hi_ivp_get_venc_low_bitrate
*   Description  : Get status of venc low bitrate.
*   Parameters   : hi_s32 ivp_handle:Input
                   hi_s32 venc_chn:Input
                   hi_bool *enable:Output
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_get_venc_low_bitrate(hi_s32 ivp_handle, hi_s32 venc_chn, hi_bool *enable);

/*****************************************************************************
*   Prototype    : hi_ivp_set_advance_isp
*   Description  : Enable or disable advance Isp Attr.
*   Parameters   : hi_s32 ivp_handle:Input
                   hi_s32 vi_pipe:Input
                   hi_bool enable:Input
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_set_advance_isp(hi_s32 ivp_handle, hi_s32 vi_pipe, hi_bool enable);

/*****************************************************************************
*   Prototype    : hi_ivp_get_advance_isp
*   Description  : Get status of advance ISP attr.
*   Parameters   : hi_s32 ivp_handle:Input
                   hi_s32 vi_pipe:Input
                   hi_bool *enable:Output
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_get_advance_isp(hi_s32 ivp_handle, hi_s32 vi_pipe, hi_bool *enable);

/*****************************************************************************
*   Prototype    : hi_ivp_set_roi_attr
*   Description  : Set ROI Attr.
*   Parameters   : hi_s32 ivp_handle:Input
                   const hi_ivp_roi_attr *roi_attr:Input
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2019/3/6
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_set_roi_attr(hi_s32 ivp_handle, const hi_ivp_roi_attr *roi_attr);

/*****************************************************************************
*   Prototype    : hi_ivp_get_roi_attr
*   Description  : Get ROI Attr.
*   Parameters   : hi_s32 ivp_handle:Input
                   hi_ivp_roi_attr *roi_attr:Output
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2019/3/6
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_get_roi_attr(hi_s32 ivp_handle, hi_ivp_roi_attr *roi_attr);

/*****************************************************************************
*   Prototype    : hi_ivp_set_roi_map
*   Description  : Set ROI Map.
*   Parameters   : hi_s32 ivp_handle:Input
                   const hi_ivp_roi_map *roi_map:Input
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2019/3/6
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_set_roi_map(hi_s32 ivp_handle, const hi_ivp_roi_map *roi_map);

/*****************************************************************************
*   Prototype    : hi_ivp_process
*   Description  : Process.
*   Parameters   : hi_s32 ivp_handle:Input
                   const VIDEO_FRAME_INFO_S *src_frame:Input
                   hi_bool *obj_alarm:Output,alarm info.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_process(hi_s32 ivp_handle, const VIDEO_FRAME_INFO_S *src_frame, hi_bool *obj_alarm);

/*****************************************************************************
*   Prototype    : hi_ivp_process_ex
*   Description  : Process.
*   Parameters   : hi_s32 ivp_handle:Input
                   const VIDEO_FRAME_INFO_S *src_frame:Input
                   hi_ivp_obj_array *obj_array:Output
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018/10/26
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
hi_s32 hi_ivp_process_ex(hi_s32 ivp_handle, const VIDEO_FRAME_INFO_S *src_frame, hi_ivp_obj_array *obj_array);

/* Error Code */
typedef enum hiEN_IVP_ERR_CODE_E {
    ERR_IVP_READ_FILE      = 0x41,   /* IVP read file error */
    ERR_IVP_OPERATE_FILE   = 0x42,   /* IVP operate file error */
    ERR_IVP_PROCESS_ERR    = 0x43,
    ERR_IVP_INIT_FAIL      = 0x44,   /* IVP init fail */
    ERR_IVP_EXIT_FAIL      = 0x45,   /* IVP exit fail */
    ERR_IVP_LOAD_RESOURCE_FAIL = 0x46,

    ERR_IVP_BUTT
}EN_IVP_ERR_CODE_E;

/************************************************IVP error code ***********************************/
#define HI_ERR_IVP_NULL_PTR          HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_IVP_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_IVP_NOT_SURPPORT      HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_IVP_INIT_FAIL         HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, ERR_IVP_INIT_FAIL)
#define HI_ERR_IVP_EXIT_FAIL         HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, ERR_IVP_EXIT_FAIL)
#define HI_ERR_IVP_NOMEM             HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_IVP_EXIST             HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_IVP_UNEXIST           HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_IVP_READ_FILE         HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, ERR_IVP_READ_FILE)
#define HI_ERR_IVP_OPERATE_FILE      HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, ERR_IVP_OPERATE_FILE)
#define HI_ERR_IVP_PROCESS_ERR       HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, ERR_IVP_PROCESS_ERR)
#define HI_ERR_IVP_LOAD_RESOURCE_FAIL       HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, ERR_IVP_LOAD_RESOURCE_FAIL)
#define HI_ERR_IVP_BUSY              HI_DEF_ERR(HI_ID_IVP, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif/*_HI_IVP_H_*/

