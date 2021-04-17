#include "xm530_tmp.h"
#include "Camera.h"
#include "ext_cmos.h"
#include "ext_sensor_ctl.h"
#include "extCfg.h"
#include "xm530_isp.h"


extern XM_U32 gu32ShutNow;
extern XM_U8 gu8MaxShutterOfst;
extern XM_U8 gu8Fps;
extern unsigned char gSnsDevAddr;
extern unsigned int  gSnsRegAddrByte;
extern unsigned int  gSnsRegDataByte;
extern XM_U8 gu8Resolution;
extern XM_U16 gu16FullLines;
extern const XM_U16 gau16GainTbl_SmatSns[64];
extern ISP_SNS_REGS_INFO_S g_stSnsRegsInfo;
extern ISP_I2C_DATA_S gstIspI2CData[8];
extern XM_U32 gau32PreI2CData[8];

XM_S32 cmos_init_sensor_exp_function_soi(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_soi(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);


XM_VOID cmos_gain_calc_table_com(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo);


extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_imx323;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_imx323;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_imx323;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_imx323;
XM_U32 sensor_getlist_imx323(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_imx323(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_imx323(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_imx307;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_imx307;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_imx307;
XM_U32 sensor_getlist_imx307(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_imx307(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_imx307(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_F37;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_F37;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_F37;
XM_U32 sensor_getlist_F37(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_F37(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_F37(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_MIS2003;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_MIS2003;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_MIS2003;
XM_U32 sensor_getlist_MIS2003(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_MIS2003(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_MIS2003(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_SP2305;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_SP2305;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_SP2305;
XM_U32 sensor_getlist_SP2305(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_SP2305(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_SP2305(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern void gainLogic_SP2305(XM_U32 u32AllGain);

XM_S32 cmos_init_sensor_exp_function_f22(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_f22(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);

XM_S32 cmos_init_sensor_exp_function_sc3035(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc3035(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_awb_exp_function_sc3035(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc4236;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc4236;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc4236;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sc4236;
XM_U32 sensor_getlist_sc4236(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_sc4236(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc4236(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
void shutLogic_sc4236(XM_U32 u32IntTime);
void gainLogic_sc4236(XM_U32 u32AllGain);


extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc2235;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2235;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc2235;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sc2235;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sc2235E;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sc2235p;

XM_U32 sensor_getlist_sc2235(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_sc2235(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc2235(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern void gainLogic_sc2235(XM_U32 u32AllGain);
void shutLogic_sc2235(XM_U32 u32IntTime);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc2235P;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2235P;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc2235P;
XM_U32 sensor_getlist_sc2235P(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_sc2235P(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc2235P(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern void gainLogic_sc2235P(XM_U32 u32AllGain);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc2235E;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2235E;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc2235E;
XM_U32 sensor_getlist_sc2235E(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_sc2235E(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc2235E(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_awb_exp_function_sc2235E(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);

extern XM_U16 gu16SensorSwitch;	//2: SC2145H_A    3: SC2145H_B(SC2145H1)
extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc2145H;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2145H_A;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc2145H_B;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc2145H;
XM_U32 sensor_getlist_sc2145H(XM_U16 *pu16Num);
XM_S32 cmos_init_ae_exp_function_sc2145H(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_sensor_exp_function_sc2145H(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sc1235;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sc1235;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sc1235;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sc1235;
XM_U32 sensor_getlist_sc1235(XM_U16 *pu16Num);
XM_S32 cmos_init_ae_exp_function_sc1235(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
extern void gainLogic_sc1235(XM_U32 u32AllGain);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_h62;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_h62;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_h62;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_h62;
XM_U32 sensor_getlist_h62(XM_U16 *pu16Num);
XM_S32 cmos_init_ae_exp_function_h62(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_sensor_exp_function_h62(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_OV9732;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_OV9732;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_OV9732;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_OV9732;
XM_U32 sensor_getlist_OV9732(XM_U16 *pu16Num);
XM_S32 cmos_init_ae_exp_function_OV9732(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_sensor_exp_function_OV9732(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);

extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_sp140A;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_sp140A;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_sp140A;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_sp140A;
XM_U32 sensor_getlist_sp140A(XM_U16 *pu16Num);
XM_S32 cmos_init_ae_exp_function_sp140A(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_sensor_exp_function_sp140A(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);


extern const ISP_COLORMATRIX_AUTO_S g_stAwbCcm_h65;
extern const  ISP_AWB_CALIBRATION_V2_S gstAwbCal_h65;
extern const ISP_CMOS_AGC_TABLE_S g_stIspAgcTable_h65;
extern const AE_SENSOR_DEFAULT_V2_S gstAeSnsDef_h65;
XM_U32 sensor_getlist_h65(XM_U16 *pu16Num);


XM_U32 sensor_getlist_sc2145(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_sc2145(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc2145(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_awb_exp_function_sc2145(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);

XM_U32 sensor_getlist_sc307e(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_sc307e(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc307e(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_awb_exp_function_sc307e(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);


XM_U32 sensor_getlist_auge(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_auge(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_auge(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_awb_exp_function_auge(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);

XM_U32 sensor_getlist_sc2335(XM_U16 *pu16Num);
XM_S32 cmos_init_sensor_exp_function_sc2335(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc);
XM_S32 cmos_init_ae_exp_function_sc2335(AE_SENSOR_EXP_FUNC_S *pstExpFuncs);
XM_S32 cmos_init_awb_exp_function_sc2335(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs);

XM_U32 sensor_getlist_sc3035(XM_U16 *pu16Num);

