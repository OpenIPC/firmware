#ifndef _X50_CMOS_H_
#define _X50_CMOS_H_
#include "xm530_isp.h"

extern XM_U32 gu32ShutNow;
extern XM_U32 gau32AllGain;
extern XM_U8 gu8Fps;
extern void (*pfn_gainLogic)(XM_U32 u32AllGain);
extern void (*pfn_shutLogic)(XM_U32 u32IntTime);

typedef XM_VOID (*cmos_ecd_call)(unsigned char);

extern XM_S32 cmos_init_sensor_exp_function_ps5510(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_ps5510(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_ps5510(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_ps5510(cmos_ecd_call *cmos_ecd_ptrl);

extern XM_S32 cmos_init_sensor_exp_function_sc5035(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_sc5035(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_sc5035(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_sc5035(cmos_ecd_call *cmos_ecd_ptrl);

XM_S32 cmos_set_isp_para_K03(XM_U8 u8RlstMode,XM_U8 u8Mode);
extern XM_S32 cmos_init_sensor_exp_function_k03(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_k03(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_k03(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_k03(cmos_ecd_call *cmos_ecd_ptrl);

extern XM_S32 cmos_init_sensor_exp_function_sc5235(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_sc5235(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_sc5235(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_sc5235(cmos_ecd_call *cmos_ecd_ptrl);

extern XM_S32 cmos_init_sensor_exp_function_sc335e(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_sc335e(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_sc335e(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_sc335e(cmos_ecd_call *cmos_ecd_ptrl);

extern XM_S32 cmos_init_sensor_exp_function_SC5239(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_SC5239(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_SC5239(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_SC5239(cmos_ecd_call *cmos_ecd_ptrl);

extern XM_S32 cmos_init_sensor_exp_function_IMX335(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
extern XM_S32 cmos_init_ae_exp_function_IMX335(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 cmos_init_awb_exp_function_IMX335(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern XM_S32 coms_EncoderChange_function_IMX335(cmos_ecd_call *cmos_ecd_ptrl);
#endif

