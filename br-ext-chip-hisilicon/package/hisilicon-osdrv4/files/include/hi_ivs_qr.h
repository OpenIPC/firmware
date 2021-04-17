/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: definition of hi_ivs_qr.h
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-09-02
 */
#ifndef _HI_IVS_QR_H_
#define _HI_IVS_QR_H_

#include "hi_common_qr.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*
 *   Prototype    : hi_ivs_qr_init
 *   Description  : qr init context.
 *   Parameters   : void
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Spec         :
 *   History:
 *       1.  Date         : 2019/09/02
 *           Author       :
 *           Modification : Created function
 */
hi_s32 hi_ivs_qr_init(hi_void);

/*
 *   Prototype    : hi_ivs_qr_get_buf_size
 *   Description  : get qr buf size.
 *   Parameters   : hi_u32    max_img_width       Max input image width
 *                  hi_u32    max_img_height      Max input image height
 *                  hi_u32    *size               output buffer size
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Spec         :
 *   History:
 *       1.  Date         : 2019/09/02
 *           Author       :
 *           Modification : Created function
 */
hi_s32 hi_ivs_qr_get_buf_size(hi_u32 max_img_width, hi_u32 max_img_height, hi_u32 *size);

/*
 *   Prototype    : hi_ivs_qr_proc
 *   Description  : qr process.
 *   Parameters   : VIDEO_FRAME_INFO_S          *src_frame        input frame
 *                  hi_qr_mem_info               *buf              buffer
 *                  hi_qr_info                   *qr_info          output QR info
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Spec         :
 *   History:
 *       1.  Date         : 2019/09/02
 *           Author       :
 *           Modification : Created function
 */
hi_s32 hi_ivs_qr_proc(VIDEO_FRAME_INFO_S *src_frame, hi_qr_mem_info *buf, hi_qr_info *qr_info);

/*
 *   Prototype    : hi_ivs_qr_deinit
 *   Description  : qr deinit.
 *   Parameters   : void
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Spec         :
 *   History:
 *       1.  Date         : 2019/09/02
 *           Author       :
 *           Modification : Created function
 */
hi_s32 hi_ivs_qr_deinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_IVS_QR_H_ */
