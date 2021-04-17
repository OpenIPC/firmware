/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: definition of hi_common_qr.h
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-09-02
 */
#ifndef _HI_COMMOM_QR_H_
#define _HI_COMMOM_QR_H_

#include "hi_type.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define HI_QR_MAX_PAYLOAD_LEN 256

typedef enum {
    HI_QR_STATE_DECODE_SUCCESS   = 0x00,
    HI_QR_STATE_NON_EXISTENT     = 0x01,
    HI_QR_STATE_DECODE_FAIL      = 0x02,
    HI_QR_STATE_DATA_OVERFLOW    = 0x03,
    HI_QR_STATE_BUTT
} hi_qr_decode_state;

#define HI_ERR_QR_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_QR, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_QR_EXIST             HI_DEF_ERR(HI_ID_QR, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_QR_NULL_PTR          HI_DEF_ERR(HI_ID_QR, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_QR_NO_MEM            HI_DEF_ERR(HI_ID_QR, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_QR_NOT_READY         HI_DEF_ERR(HI_ID_QR, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

typedef struct {
    hi_qr_decode_state state;
    hi_s32  version;
    hi_s32  ecc_level;
    hi_s32  mask;
    hi_s32  data_type;
    hi_s32  payload_len;
    hi_u32  eci;
    hi_u8   payload[HI_QR_MAX_PAYLOAD_LEN];
} hi_qr_info;

typedef struct {
    hi_u64 phys_addr;
    hi_u64 virt_addr;
    hi_u32 mem_size;
} hi_qr_mem_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_COMMON_QR_H_ */
