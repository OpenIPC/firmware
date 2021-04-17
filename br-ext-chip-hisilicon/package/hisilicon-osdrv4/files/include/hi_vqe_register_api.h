#ifndef __HI_VQE_REGISTER_API_H__
#define __HI_VQE_REGISTER_API_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* VQE module function api, return pointer to handle of VQE module */

//record
HI_VOID * HI_VQE_RECORD_GetHandle(HI_VOID);
//HPF
HI_VOID * HI_VQE_HPF_GetHandle(HI_VOID);
//AEC
HI_VOID * HI_VQE_AEC_GetHandle(HI_VOID);
//ANR
HI_VOID * HI_VQE_ANR_GetHandle(HI_VOID);
//RNR
HI_VOID * HI_VQE_RNR_GetHandle(HI_VOID);
//HDR
HI_VOID * HI_VQE_HDR_GetHandle(HI_VOID);
//DRC
HI_VOID * HI_VQE_DRC_GetHandle(HI_VOID);
//PEQ
HI_VOID * HI_VQE_PEQ_GetHandle(HI_VOID);
//AGC
HI_VOID * HI_VQE_AGC_GetHandle(HI_VOID);
//EQ
HI_VOID * HI_VQE_EQ_GetHandle(HI_VOID);
//RESAMPLE
HI_VOID * HI_VQE_RESAMPLE_GetHandle(HI_VOID);
//GAIN
HI_VOID * HI_VQE_GAIN_GetHandle(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_VQE_REGISTER_H__ */

