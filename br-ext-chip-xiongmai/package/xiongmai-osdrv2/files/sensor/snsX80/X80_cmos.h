#ifndef _X50_CMOS_H_
#define _X50_CMOS_H_
#include "xm530_isp.h"

extern XM_U32 gu32ShutNow;
extern XM_U32 gau32AllGain;
extern XM_U8 gu8Fps;
extern void (*pfn_gainLogic)(XM_U32 u32AllGain);
extern void (*pfn_shutLogic)(XM_U32 u32IntTime);

typedef XM_VOID (*cmos_ecd_call)(unsigned char);

extern XM_S32 cmos_init_sensor_exp_function_apollo(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_apollo(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_apollo(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_apollo(cmos_ecd_call *cmos_ecd_ptrl);


#endif

