/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

/*
*   mi_isp_api_id.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _TUNINGSERVER_API_ID_H_
#define _TUNINGSERVER_API_ID_H_

#ifdef __cplusplus
extern "C"
{
#endif
#define ID_API_COMMON_BASE              (0x0000)
#define ID_API_COMMON_END               (0x0FFF)

#if 0
#define ID_API_ISP_BASE                 (0x1000)
#define ID_API_ISP_END                  (0x1FFF)

#define ID_API_CALIBRATION_20_BASE      (0x2000)
#define ID_API_CALIBRATION_20_END       (0x22FF)

#define ID_API_MAINTOOL_BASE            (0x2300)
#define ID_API_MAINTOOL_END             (0x25FF)

#define ID_API_PLUGIN_BASE              (0x2600)
#define ID_API_PLUGIN_END               (0x28FF)
#else
#define ID_API_ISP_IQ_BASE              (0x1000)
#define ID_API_ISP_IQ_END               (0x13FF)

#define ID_API_ISP_AE_BASE              (0x1400)
#define ID_API_ISP_AE_END               (0x17FF)

#define ID_API_ISP_AWB_BASE             (0x1800)
#define ID_API_ISP_AWB_END              (0x1BFF)

#define ID_API_ISP_AF_BASE              (0x1C00)
#define ID_API_ISP_AF_END               (0x1FFF)

#define ID_API_ISP_OTHER_BASE           (0x2000)
#define ID_API_ISP_OTHER_END            (0x23FF)

#define ID_API_CALIBRATION_20_BASE      (0x2400)
#define ID_API_CALIBRATION_20_END       (0x27FF)

#define ID_API_MAINTOOL_BASE            (0x2800)
#define ID_API_MAINTOOL_END             (0x2BFF)

#define ID_API_PLUGIN_BASE              (0x2C00)
#define ID_API_PLUGIN_END               (0x2DFF)

#define ID_API_CUS3A_BASE               (0x2E00)
#define ID_API_CUS3A_END                (0x2FFF)

#define ID_API_DEBUG_LEVEL_BASE         (0x2FFF)
#define ID_API_DEBUG_LEVEL_END          (0x3100)

#define ID_API_RESERVED_BASE            (0x3101)
#define ID_API_RESERVED_END             (0x3FFF)

#endif

#define ID_CHIP_I1                      (0x6d65031B)
#define ID_CHIP_I3                      (0x6d65031C)
#define ID_CHIP_I2                      (0x6d65031D)
#define ID_CHIP_I5                      (0x6d65031E)
#define ID_CHIP_I6                      (0x6d650320)
#define ID_CHIP_I6E                     (0x6d650321)
#define ID_CHIP_I6B0                    (0x6d650322)
#define ID_CHIP_M5                      (0x6d65031F)

//================================================================
//  ID Defined : Common  API
//================================================================
#define ID_API_COMMON_I2C_ACCESS                    ( ID_API_COMMON_BASE +  32 ) //SET_API_ID_MI_ISP_WriteI2C
//#define ID_API_COMMON_QuerySensorInfo               ( ID_API_COMMON_BASE +  33 ) //GET_API_ID_MI_ISP_QuerySensorInfo
#define ID_API_COMMON_MIRROR                        ( ID_API_COMMON_BASE +  34 )
#define ID_API_COMMON_ROTATE                        ( ID_API_COMMON_BASE +  35 )
#define ID_API_COMMON_SENSOR_ID                     ( ID_API_COMMON_BASE +  36 )

#define ID_API_COMMON_FileID                        ( ID_API_COMMON_BASE + 100 ) //I1 or I3 or I2 or I5 ...
#define ID_API_COMMON_CHANNEL_ID                    ( ID_API_COMMON_BASE + 101 )
#define ID_API_COMMON_ISPROOT                       ( ID_API_COMMON_BASE + 102 )

//================================================================
//  ID Defined : ISP  API
//================================================================
#define ID_API_ISP_IQ_VERSION_INFO                  ( ID_API_ISP_IQ_BASE  +  1 ) //  1. 4097
#define ID_API_ISP_IQ_PARAM_INIT_STATUS             ( ID_API_ISP_IQ_BASE  +  2 ) //  2. 4098
#define ID_API_ISP_IQ_FAST_MODE                     ( ID_API_ISP_IQ_BASE  +  3 ) //  3. 4099
#define ID_API_ISP_IQ_COLORTOGRAY                   ( ID_API_ISP_IQ_BASE  +  4 ) //  4. 4100
#define ID_API_ISP_IQ_CONTRAST                      ( ID_API_ISP_IQ_BASE  +  5 ) //  5. 4101
#define ID_API_ISP_IQ_BRIGHTNESS                    ( ID_API_ISP_IQ_BASE  +  6 ) //  6. 4102
#define ID_API_ISP_IQ_LIGHTNESS                     ( ID_API_ISP_IQ_BASE  +  7 ) //  7. 4103
#define ID_API_ISP_IQ_RGBGAMMA                      ( ID_API_ISP_IQ_BASE  +  8 ) //  8. 4104
#define ID_API_ISP_IQ_YUVGAMMA                      ( ID_API_ISP_IQ_BASE  +  9 ) //  9. 4105
#define ID_API_ISP_IQ_SATURATION                    ( ID_API_ISP_IQ_BASE  + 10 ) // 10. 4106
#define ID_API_ISP_IQ_DEFOG                         ( ID_API_ISP_IQ_BASE  + 11 ) // 11. 4107
#define ID_API_ISP_IQ_CCM                           ( ID_API_ISP_IQ_BASE  + 12 ) // 12. 4108
#define ID_API_ISP_IQ_ANTI_FALSE_COLOR              ( ID_API_ISP_IQ_BASE  + 13 ) // 13. 4109
#define ID_API_ISP_IQ_NR3D                          ( ID_API_ISP_IQ_BASE  + 14 ) // 14. 4110
#define ID_API_ISP_IQ_NR2D_DESPIKE                  ( ID_API_ISP_IQ_BASE  + 15 ) // 15. 4111
#define ID_API_ISP_IQ_NR2D_LUMA                     ( ID_API_ISP_IQ_BASE  + 16 ) // 16. 4112
#define ID_API_ISP_IQ_NR2D_CHROMA                   ( ID_API_ISP_IQ_BASE  + 17 ) // 17. 4113
#define ID_API_ISP_IQ_SHARPNESS                     ( ID_API_ISP_IQ_BASE  + 18 ) // 18. 4114
#define ID_API_ISP_IQ_CROSSTALK                     ( ID_API_ISP_IQ_BASE  + 19 ) // 19. 4115
#define ID_API_ISP_IQ_BLACK_LEVEL                   ( ID_API_ISP_IQ_BASE  + 20 ) // 20. 4116
#define ID_API_ISP_IQ_BLACK_LEVEL_P1                ( ID_API_ISP_IQ_BASE  + 21 ) // 21. 4117
#define ID_API_ISP_IQ_WDR                           ( ID_API_ISP_IQ_BASE  + 22 ) // 22. 4118
#define ID_API_ISP_IQ_DEFECT_PIXEL                  ( ID_API_ISP_IQ_BASE  + 23 ) // 23. 4119
#define ID_API_ISP_IQ_HSV                           ( ID_API_ISP_IQ_BASE  + 24 ) // 24. 4120
#define ID_API_ISP_IQ_RGBIR                         ( ID_API_ISP_IQ_BASE  + 25 ) // 25. 4121
#define ID_API_ISP_IQ_FPN                           ( ID_API_ISP_IQ_BASE  + 26 ) // 26. 4122
#define ID_API_ISP_IQ_PFC                           ( ID_API_ISP_IQ_BASE  + 27 ) // 27. 4123
#define ID_API_ISP_IQ_DM                            ( ID_API_ISP_IQ_BASE  + 28 ) // 28. 4124
#define ID_API_ISP_IQ_COLOR_TRANSFORM               ( ID_API_ISP_IQ_BASE  + 29 ) // 29. 4125
#define ID_API_ISP_IQ_HDR                           ( ID_API_ISP_IQ_BASE  + 30 ) // 30. 4126
#define ID_API_ISP_IQ_EFFECT                        ( ID_API_ISP_IQ_BASE  + 31 ) // 31. 4127
#define ID_API_ISP_IQ_SYS_MCNR_MEMORY               ( ID_API_ISP_IQ_BASE  + 32 ) // 32. 4128
#define ID_API_ISP_IQ_LSC                           ( ID_API_ISP_IQ_BASE  + 33 ) // 33. 4129
#define ID_API_ISP_IQ_PARAM_MODE                    ( ID_API_ISP_IQ_BASE  + 34 ) // 34. 4130
#define ID_API_ISP_IQ_LinearityLUT                  ( ID_API_ISP_IQ_BASE  + 35 ) // 35. 4131 //Get Gamma data for CCM calibration
#define ID_API_ISP_IQ_OBCCALIB                      ( ID_API_ISP_IQ_BASE  + 36 ) // 36. 4132 //Get OB data for Gamma or CCM calibration
#define ID_API_ISP_IQ_NR3D_P1                       ( ID_API_ISP_IQ_BASE  + 37 ) // 37. 4133
#define ID_API_ISP_IQ_WDR_FC_CURVE                  ( ID_API_ISP_IQ_BASE  + 38 ) // 38. 4134
#define ID_API_ISP_IQ_R2Y                           ( ID_API_ISP_IQ_BASE  + 39 ) // 39. 4135
#define ID_API_ISP_IQ_ALSC                          ( ID_API_ISP_IQ_BASE  + 40 ) // 40. 4136
#define ID_API_ISP_IQ_LSC_CTRL                      ( ID_API_ISP_IQ_BASE  + 41 ) // 41. 4137
#define ID_API_ISP_IQ_ALSC_CTRL                     ( ID_API_ISP_IQ_BASE  + 42 ) // 42. 4138
#define ID_API_ISP_IQ_DEFECT_PIXEL_CLUSTER          ( ID_API_ISP_IQ_BASE  + 43 ) // 43. 4139
#define ID_API_ISP_IQ_QUERY_CCM_INFO                ( ID_API_ISP_IQ_BASE  + 44 ) // 44. 4140
#define ID_API_ISP_IQ_NR2D_LUMA_ADV                 ( ID_API_ISP_IQ_BASE  + 45 ) // 45. 4141
#define ID_API_ISP_IQ_NR2D_CHROMA_ADV               ( ID_API_ISP_IQ_BASE  + 46 ) // 46. 4142
#define ID_API_ISP_IQ_PFC_EX                        ( ID_API_ISP_IQ_BASE  + 47 ) // 47. 4143
#define ID_API_ISP_IQ_HDR_EX                        ( ID_API_ISP_IQ_BASE  + 48 ) // 48. 4144
#define ID_API_ISP_IQ_SHP_EX                        ( ID_API_ISP_IQ_BASE  + 49 ) // 49. 4145
#define ID_API_ISP_IQ_NR_3D_EX                      ( ID_API_ISP_IQ_BASE  + 50 ) // 50. 4146
#define ID_API_ISP_IQ_DUMMY                         ( ID_API_ISP_IQ_BASE  + 51 ) // 51. 4147
#define ID_API_ISP_IQ_DUMMY_EX                      ( ID_API_ISP_IQ_BASE  + 52 ) // 52. 4148
#define ID_API_ISP_IQ_WDR_FC_CURVE_FULL             ( ID_API_ISP_IQ_BASE  + 53 ) // 53. 4149
#define ID_API_ISP_IQ_ADAPTIVE_GAMMA                ( ID_API_ISP_IQ_BASE  + 54 ) // 54. 4150
#define ID_API_ISP_IQ_NR2D_CHROMA_PRE               ( ID_API_ISP_IQ_BASE  + 55 ) // 55. 4151
#define ID_API_ISP_IQ_TEMP                          ( ID_API_ISP_IQ_BASE  + 56 ) // 56. 4152
#define ID_API_ISP_IQ_TEMP_INFO                     ( ID_API_ISP_IQ_BASE  + 57 ) // 57. 4153

#define ID_API_ISP_AE_HIST_WEIGHT_Y                 ( ID_API_ISP_AE_BASE  +  1 ) //  1. 5121
#define ID_API_ISP_AE_QUERY_EXPOSURE_INFO           ( ID_API_ISP_AE_BASE  +  2 ) //  2. 5122
#define ID_API_ISP_AE_EV_COMP                       ( ID_API_ISP_AE_BASE  +  3 ) //  3. 5123
#define ID_API_ISP_AE_EXPO_MODE                     ( ID_API_ISP_AE_BASE  +  4 ) //  4. 5124
#define ID_API_ISP_AE_MANUAL_EXPO                   ( ID_API_ISP_AE_BASE  +  5 ) //  5. 5125
#define ID_API_ISP_AE_STATE                         ( ID_API_ISP_AE_BASE  +  6 ) //  6. 5126
#define ID_API_ISP_AE_TARGET                        ( ID_API_ISP_AE_BASE  +  7 ) //  7. 5127
#define ID_API_ISP_AE_CONVERGE                      ( ID_API_ISP_AE_BASE  +  8 ) //  8. 5128
#define ID_API_ISP_AE_EXPOSURE_LIMIT                ( ID_API_ISP_AE_BASE  +  9 ) //  9. 5129
#define ID_API_ISP_AE_PLAIN_LONG_EXPO_TABLE         ( ID_API_ISP_AE_BASE  + 10 ) // 10. 5130
#define ID_API_ISP_AE_PLAIN_SHORT_EXPO_TABLE        ( ID_API_ISP_AE_BASE  + 11 ) // 11. 5131
#define ID_API_ISP_AE_WINDOW_WGT_MODE               ( ID_API_ISP_AE_BASE  + 12 ) // 12. 5132
#define ID_API_ISP_AE_WINDOW_WGT                    ( ID_API_ISP_AE_BASE  + 13 ) // 13. 5133
#define ID_API_ISP_AE_FLICKER                       ( ID_API_ISP_AE_BASE  + 14 ) // 14. 5134
#define ID_API_ISP_AE_STRATEGY                      ( ID_API_ISP_AE_BASE  + 15 ) // 15. 5135
#define ID_API_ISP_AE_RGBIRExposureAttr             ( ID_API_ISP_AE_BASE  + 16 ) // 16. 5136
#define ID_API_ISP_AE_HDR                           ( ID_API_ISP_AE_BASE  + 17 ) // 17. 5137
#define ID_API_ISP_AE_MANUAL_EXPO_SHORT             ( ID_API_ISP_AE_BASE  + 18 ) // 18. 5138
#define ID_API_ISP_AE_FLICKER_EX                    ( ID_API_ISP_AE_BASE  + 19 ) // 19. 5139
#define ID_API_ISP_AE_QUERY_FLICKER_EX_INFO         ( ID_API_ISP_AE_BASE  + 20 ) // 20. 5140
#define ID_API_ISP_AE_STABILIZER                    ( ID_API_ISP_AE_BASE  + 21 ) // 21. 5141
#define ID_API_ISP_AE_STRATEGY_EX                   ( ID_API_ISP_AE_BASE  + 22 ) // 22. 5142
#define ID_API_ISP_AE_QUERY_STRATEGY_EX_INFO        ( ID_API_ISP_AE_BASE  + 23 ) // 24. 5143

#define ID_API_ISP_AWB_QUERY_WHITE_BALANCE_INFO     ( ID_API_ISP_AWB_BASE +  1 ) //  1. 6145
#define ID_API_ISP_AWB_ATTR                         ( ID_API_ISP_AWB_BASE +  2 ) //  2. 6146
#define ID_API_ISP_AWB_ATTR_EX                      ( ID_API_ISP_AWB_BASE +  3 ) //  3. 6147
#define ID_API_ISP_AWB_MULTI_LS_ATTR                ( ID_API_ISP_AWB_BASE +  4 ) //  4. 6148
#define ID_API_ISP_AWB_CT_WEIGHT                    ( ID_API_ISP_AWB_BASE +  5 ) //  5. 6149
#define ID_API_ISP_AWB_CTMWB                        ( ID_API_ISP_AWB_BASE +  6 ) //  5. 6150

//#define ID_API_ISP_AF_QUERY_FOCUS_INFO              ( ID_API_ISP_AF_BASE  +  1 ) //  1. 7169
//#define ID_API_ISP_AF_WINDOW                        ( ID_API_ISP_AF_BASE  +  2 ) //  2. 7170
//#define ID_API_ISP_AF_FILTER                        ( ID_API_ISP_AF_BASE  +  3 ) //  3. 7171


//#define ID_API_VENC_RcParamEx                     ( ID_API_ISP_OTHER_BASE +  1 ) // 1. 8193 --> old ( ID_API_ISP_BASE + 35 ) //35. 4131
//#define ID_API_VI_SensorFrameRate                 ( ID_API_ISP_OTHER_BASE +  2 ) // 2. 8194 --> old ( ID_API_ISP_BASE + 36 ) //36. 4132
//#define ID_API_VENC_Resolution                    ( ID_API_ISP_OTHER_BASE +  3 ) // 3. 8195 --> old ( ID_API_ISP_BASE + 37 ) //37. 4133

//================================================================
//  ID Defined : Calibration 2.0 API
//================================================================
#define ID_API_CALIBRATION_20_CaliBVAV              ( ID_API_CALIBRATION_20_BASE +  1) //  1. 9217 //SET_API_ID_CALIBRATION_CaliBVAV = 61,
#define ID_API_CALIBRATION_20_Apply                 ( ID_API_CALIBRATION_20_BASE +  2) //  2. 9218 //SET_API_ID_CALIBRATION_ApplyBVAV = 62,
#define ID_API_CALIBRATION_20_CaliData              ( ID_API_CALIBRATION_20_BASE +  3) //  3. 9219 //SET_API_ID_CALIBRATION_SetCaliData = 63,
#define ID_API_CALIBRATION_20_CaliPath              ( ID_API_CALIBRATION_20_BASE +  4) //  4. 9220 //SET_API_ID_CALIBRATION_SetCaliPath = 64,
#define ID_API_CALIBRATION_20_CaliDBPath            ( ID_API_CALIBRATION_20_BASE +  5) //  5. 9221 //SET_API_ID_CALIBRATION_SetCaliDBPath
#define ID_API_CALIBRATION_20_CaliInfo              ( ID_API_CALIBRATION_20_BASE +  6) //  6. 9222 //GET_API_ID_CALIBRATION_GetCaliInfo

#define ID_API_CALIBRATION_20_CaliAWB               ( ID_API_CALIBRATION_20_BASE +  7) //  7. 9223
#define ID_API_CALIBRATION_20_CaliOB                ( ID_API_CALIBRATION_20_BASE +  8) //  8. 9224
#define ID_API_CALIBRATION_20_CaliMinGain           ( ID_API_CALIBRATION_20_BASE +  9) //  9. 9225
#define ID_API_CALIBRATION_20_CaliShutterLinearity  ( ID_API_CALIBRATION_20_BASE + 10) // 10. 9226
#define ID_API_CALIBRATION_20_CaliGainLinearity     ( ID_API_CALIBRATION_20_BASE + 11) // 11  9227
#define ID_API_CALIBRATION_20_CaliDPC               ( ID_API_CALIBRATION_20_BASE + 12) // 12. 9228
#define ID_API_CALIBRATION_20_CaliALSC              ( ID_API_CALIBRATION_20_BASE + 13) // 13. 9229
#define ID_API_CALIBRATION_20_CaliFPN               ( ID_API_CALIBRATION_20_BASE + 14) // 14. 9230

#define ID_API_CALIBRATION_30_ApplyAWB              ( ID_API_CALIBRATION_20_BASE + 15) // 15. 9231
#define ID_API_CALIBRATION_30_ApplyOBC              ( ID_API_CALIBRATION_20_BASE + 16) // 16. 9232
#define ID_API_CALIBRATION_30_ApplyMinGain          ( ID_API_CALIBRATION_20_BASE + 17) // 17. 9233
#define ID_API_CALIBRATION_30_ApplyShutterLinearity ( ID_API_CALIBRATION_20_BASE + 18) // 18. 9234
#define ID_API_CALIBRATION_30_ApplyGainLinearity    ( ID_API_CALIBRATION_20_BASE + 19) // 19  9235
#define ID_API_CALIBRATION_30_ApplySDC              ( ID_API_CALIBRATION_20_BASE + 20) // 20. 9236
#define ID_API_CALIBRATION_30_ApplyALSC             ( ID_API_CALIBRATION_20_BASE + 21) // 21. 9237
#define ID_API_CALIBRATION_30_ApplyFPN              ( ID_API_CALIBRATION_20_BASE + 22) // 22. 9238
#define ID_API_CALIBRATION_30_ApplyLSC              ( ID_API_CALIBRATION_20_BASE + 23) // 21. 9239

//================================================================
//  ID Defined : MainTool  API
//================================================================
#define ID_API_MAINTOOL_QUERY_SENSOR_INFO           ( ID_API_MAINTOOL_BASE + 1 ) //  1. 10241
#define ID_API_MAINTOOL_IQ_INDEX                    ( ID_API_MAINTOOL_BASE + 2 ) //  2. 10242
#define ID_API_MAINTOOL_BYPASS                      ( ID_API_MAINTOOL_BASE + 3 ) //  3. 10243

//================================================================
//  ID Defined : Plugin  API
//================================================================
#define ID_API_PLUGIN_I2C_ACCESS                    ( ID_API_PLUGIN_BASE + 1 ) //  1. 11265 //SET_API_ID_MI_ISP_WriteI2C
#define ID_API_PLUGIN_WBCT                          ( ID_API_PLUGIN_BASE + 3 ) //  3. 11267 //SetWBCTCaliAttr
#define ID_API_PLUGIN_AWBCTStats                    ( ID_API_PLUGIN_BASE + 4 ) //  4. 11268
#define ID_API_PLUGIN_AWBHWStats                    ( ID_API_PLUGIN_BASE + 5 ) //  5. 11269
#define ID_API_PLUGIN_CUR_WBCT                      ( ID_API_PLUGIN_BASE + 6 ) //  6. 11270 //GetCurCTCaliAttr

//================================================================
//  ID Defined : Cus3A  API
//================================================================
#define ID_API_CUS_AE_HW_AVG_STATS                  ( ID_API_CUS3A_BASE  + 1 ) // 1. 11777
#define ID_API_CUS_AE_HW_HISTO_0_STATS              ( ID_API_CUS3A_BASE  + 2 ) // 2. 11778
#define ID_API_CUS_AE_HW_HISTO_1_STATS              ( ID_API_CUS3A_BASE  + 3 ) // 3. 11779
#define ID_API_CUS_AE_GET_INIT_STATUS               ( ID_API_CUS3A_BASE  + 4 ) // 4. 11780
#define ID_API_CUS_AE_GET_CUR_STATUS                ( ID_API_CUS3A_BASE  + 5 ) // 5. 11781
#define ID_API_CUS_AE_SET_PARAM                     ( ID_API_CUS3A_BASE  + 6 ) // 6. 11782
#define ID_API_CUS_AWB_HW_AVG_STATS                 ( ID_API_CUS3A_BASE  + 7 ) // 7. 11783
#define ID_API_CUS_3A_ENABLE                        ( ID_API_CUS3A_BASE  + 8 ) // 8. 11784
#define ID_API_CUS_AWB_GET_CUR_STATUS               ( ID_API_CUS3A_BASE  + 9 ) // 9. 11785
#define ID_API_CUS_AWB_SET_PARAM                    ( ID_API_CUS3A_BASE  + 10) //10. 11786
#define ID_API_CUS_AE_WINDOW_BLOCK_NUMBER           ( ID_API_CUS3A_BASE  + 11 )//11. 11787
#define ID_API_CUS_AWB_SAMPLING                     ( ID_API_CUS3A_BASE  + 12 )//12. 11788
#define ID_API_CUS_AF_STATS                         ( ID_API_CUS3A_BASE  + 13 )//13. 11789
#define ID_API_CUS_AF_WINDOW                        ( ID_API_CUS3A_BASE  + 14 )//14. 11790
#define ID_API_CUS_AF_FILTER                        ( ID_API_CUS3A_BASE  + 15 )//15. 11791
#define ID_API_CUS_AF_FILTER_SQUARE                 ( ID_API_CUS3A_BASE  + 16 )//16. 11792
#define ID_API_CUS_AF_MODE                          ( ID_API_CUS3A_BASE  + 17 )//17. 11793
#define ID_API_CUS_AE_HISTOGRAM_WINDOW              ( ID_API_CUS3A_BASE  + 18 )//18. 11794
#define ID_API_CUS_GET_IMAGE_RESOLUTION             ( ID_API_CUS3A_BASE  + 19 )//19. 11795
#define ID_API_CUS_ENABLE_ISP_OUT_IMAGE             ( ID_API_CUS3A_BASE  + 20 )//20. 11796
#define ID_API_CUS_GET_ISP_OUT_IMAGE_COUNT          ( ID_API_CUS3A_BASE  + 21 )//21. 11797
#define ID_API_CUS_CAPTURE_HDR_RAW_IMAGE            ( ID_API_CUS3A_BASE  + 22 )//22. 11798
#define ID_API_CUS_FRAME_META_INFO                  ( ID_API_CUS3A_BASE  + 23 )//23. 11799
#define ID_API_CUS_I2C_ACCESS                       ( ID_API_CUS3A_BASE  + 24 )//24. 11800
#define ID_API_CUS_AWB_HW_AVG_STATS_SHORT           ( ID_API_CUS3A_BASE  + 25 ) // 25. 11801
#define ID_API_CUS_CAPTURE_RAW_IMAGE                ( ID_API_CUS3A_BASE  + 26 )//26. 11802
#define ID_API_CUS_CUSTOMER_AE_CTRL                 ( ID_API_CUS3A_BASE  + 27 )//27. 11803 , Customer defined AE control only
#define ID_API_CUS_CUSTOMER_AWB_CTRL                ( ID_API_CUS3A_BASE  + 28 )//28. 11804 , Customer defined AWB control only
#define ID_API_CUS_CUSTOMER_AF_CTRL                 ( ID_API_CUS3A_BASE  + 29 )//29. 11805 , Customer defined AF control only
#define ID_API_CUS_AE_HW_RGBIR_HISTO_STATS              ( ID_API_CUS3A_BASE  + 30 ) // 30. 11806
#ifdef ENABLE_EARLY_AE_DONE
#define ID_API_CUS_AE_DONE_RATIO                    ( ID_API_CUS3A_BASE  + 27 )//27. 11803
#endif
#define ID_API_CUS_AE_CROP_SIZE                     ( ID_API_CUS3A_BASE  + 31 )//34. 11807
#define ID_API_CUS_AWB_CROP_SIZE                    ( ID_API_CUS3A_BASE  + 32 )//35. 11808

//================================================================
//  ID Defined : Debug Level  API
//================================================================
#define ID_API_ISP_DeBugLevel_AE                    ( ID_API_DEBUG_LEVEL_BASE  + 1 ) // 1. 12288
#define ID_API_ISP_DeBugLevel_AWB                   ( ID_API_DEBUG_LEVEL_BASE  + 2 ) // 2. 12289
#define ID_API_ISP_DeBugLevel_AWB_PERIOD            ( ID_API_DEBUG_LEVEL_BASE  + 3 ) // 3. 12290

#define ID_API_REGISTER_API_SET_CB                  ( ID_API_RESERVED_BASE + 1) //1. register API_SET callback
#define ID_API_REGISTER_API_GET_CB                  ( ID_API_RESERVED_BASE + 2) //2. register API_GET callback

#ifdef __cplusplus
}   //end of extern C
#endif

#endif  //_TUNINGSERVER_API_ID_H_
