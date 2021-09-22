/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2001-2019. All rights reserved.
 * Description: header of baby crying detection
 * Author: Hisilicon multimedia software group
 * Create: 2019-04-01
 */

#ifndef __HI_AUDIO_BCD_H__
#define __HI_AUDIO_BCD_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines the error of Baby Crying Detection. */
#define HI_ERR_BCD_INVALID_HANDLE (hi_u32)(0xA0148100) /* Handle passed to function call was invalid. */
#define HI_ERR_BCD_MEMORY_ERROR   (hi_u32)(0xA0148101) /* Memory allocation failed. */
#define HI_ERR_BCD_ILLEGAL_PARAM  (hi_u32)(0xA0148102) /* Parameter not illegal. */
#define HI_ERR_BCD_NULL_PTR       (hi_u32)(0xA0148103) /* Pointer is null. */
#define HI_ERR_BCD_NOT_PERM       (hi_u32)(0xA0148104) /* Operation not permitted. */

typedef hi_void *bcd_handle;
typedef hi_s32 (*fn_bcd_callback)(hi_void *); /* Baby Crying Detection be noticed Function Handle type */

/* Defines the configure parameters of Baby Crying Detection. */
typedef struct {
    hi_bool usr_mode;                  /* mode 0: auto mode 1: mannual. */
    hi_bool bypass;                    /* value 0: enable function; value 1: bypass function. */
    hi_u32 alarm_threshold;            /* alarm threshold. range[0, 100] */
    hi_u32 time_limit;                 /* alarm time limit(ms). */
    hi_u32 time_limit_threshold_count; /* alarm time limit threshold count. */
    hi_u32 interval_time;              /* alarm interval time(ms) */
    fn_bcd_callback callback;          /* the callback function pointer */
} hi_bcd_config;

/* Defines the process data configure of Baby Crying Detection. */
typedef struct {
    hi_s16 *data;     /* the pointer to data buf. */
    hi_s32 data_size; /* the size of data buf. */
} hi_bcd_process_data;

hi_s32 hi_baby_crying_detection_init(bcd_handle *bcd, hi_s32 sample_rate, const hi_bcd_config *bcd_config);

hi_s32 hi_baby_crying_detection_process(bcd_handle bcd, hi_bcd_process_data *input_data,
                                        hi_bcd_process_data *output_data);

hi_s32 hi_baby_crying_detection_deinit(bcd_handle bcd);

#ifdef __cplusplus
}
#endif
#endif