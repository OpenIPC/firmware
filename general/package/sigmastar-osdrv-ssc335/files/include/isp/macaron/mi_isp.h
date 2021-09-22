/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

/*
*   mi_isp.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _MI_ISP_H_
#define _MI_ISP_H_
//#include <pthread.h>

#include "mi_common.h"
#include "mi_isp_datatype.h"
#include "mi_vpe_datatype.h"
#include "mi_isp_iq.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MI_ISP_OK                       (0)
#define MI_ISP_NOT_SUPPORT              (1)
#define MI_ISP_FAILURE                  (-1)
#define MI_ISP_API_MAX_PARM_NUMBRE      (6)
#define MI_ISP_API_ISPMID_NOT_SUPPORT   (5)

    /************************************* Customer 3A API ***********************************/
    MI_S32 MI_ISP_AE_GetAeHwAvgStats(MI_U32 Channel, MI_ISP_AE_HW_STATISTICS_t *data);
    MI_S32 MI_ISP_AWB_GetAwbHwAvgStats(MI_U32 Channel, MI_ISP_AWB_HW_STATISTICS_t *data);
    MI_S32 MI_ISP_AWB_GetAwbHwAvgStatsShort(MI_U32 Channel, MI_ISP_AWB_HW_STATISTICS_t *data);
    MI_S32 MI_ISP_AE_GetHisto0HwStats(MI_U32 Channel, MI_ISP_HISTO_HW_STATISTICS_t *data);
    MI_S32 MI_ISP_AE_GetHisto1HwStats(MI_U32 Channel, MI_ISP_HISTO_HW_STATISTICS_t *data);
    MI_S32 MI_ISP_AE_GetRgbIrHistoHwStats(MI_U32 Channel, MI_ISP_RGBIR_HISTO_HW_STATISTICS_t *data);
    MI_S32 MI_ISP_CUS3A_Enable(MI_U32 Channel, Cus3AEnable_t *data);
    MI_S32 MI_ISP_CUS3A_GetAeInitStatus(MI_U32 Channel, CusAEInitParam_t *data);
    MI_S32 MI_ISP_CUS3A_GetAeStatus(MI_U32 Channel, CusAEInfo_t *data);
    MI_S32 MI_ISP_CUS3A_SetAeParam(MI_U32 Channel, CusAEResult_t *data);
    MI_S32 MI_ISP_CUS3A_GetAwbStatus(MI_U32 Channel, CusAWBInfo_t *data);
    MI_S32 MI_ISP_CUS3A_SetAwbParam(MI_U32 Channel, CusAWBResult_t *data);
    MI_S32 MI_ISP_CUS3A_SetAEWindowBlockNumber(MI_U32 Channel, MS_CUST_AE_WIN_BLOCK_NUM_TYPE_e *data);
    MI_S32 MI_ISP_CUS3A_SetAEHistogramWindow(MI_U32 Channel, CusAEHistWin_t*data);
    MI_S32 MI_ISP_CUS3A_SetAWBSampling(MI_U32 Channel, CusAWBSample_t *data);

    MI_S32 MI_ISP_CUS3A_GetAFStats(MI_U32 Channel, CusAFStats_t *data);
    MI_S32 MI_ISP_CUS3A_SetAFWindow(MI_U32 Channel, CusAFWin_t *data);
    MI_S32 MI_ISP_CUS3A_GetAFWindow(MI_U32 Channel, CusAFWin_t *data);
    MI_S32 MI_ISP_CUS3A_SetAFFilter(MI_U32 Channel, CusAFFilter_t *data);
    MI_S32 MI_ISP_CUS3A_GetAFFilter(MI_U32 Channel, CusAFFilter_t *data);
    MI_S32 MI_ISP_CUS3A_SetAFFilterSq(MI_U32 Channel, CusAFFilterSq_t *data);
    MI_S32 MI_ISP_CUS3A_GetAFFilterSq(MI_U32 Channel, CusAFFilterSq_t *data);
    MI_S32 MI_ISP_CUS3A_SetAFRoiMode(MI_U32 Channel, CusAFRoiMode_t *data);
    MI_S32 MI_ISP_CUS3A_GetAFRoiMode(MI_U32 Channel, CusAFRoiMode_t *data);

    MI_S32 MI_ISP_CUS3A_GetImageResolution(MI_U32 Channel, CusImageResolution_t *data);
    MI_S32 MI_ISP_CUS3A_EnableISPOutImage(MI_U32 Channel, CusISPOutImage_t *data);
    MI_S32 MI_ISP_CUS3A_GetISPOutImageCount(MI_U32 Channel, MI_U32 *data);
    MI_S32 MI_ISP_CUS3A_CaptureHdrRawImage(MI_U32 Channel, CusHdrRawImage_t *data);
    MI_S32 MI_ISP_GetFrameMetaInfo(MI_U32 Channel, IspFrameMetaInfo_t *data);
    MI_S32 MI_ISP_ReadSensorData(MI_U32 Channel, CusSensorI2cParam_t *data);
    MI_S32 MI_ISP_WriteSensorData(MI_U32 Channel, CusSensorI2cParam_t *data);
    /************************************* Customer 3A API END********************************/
    /************************************* Debug  API ***********************************/
    MI_S32 MI_ISP_SetDebugLevel_AE(MI_U32 Channel, MI_S32 *data);
    MI_S32 MI_ISP_GetDebugLevel_AE(MI_U32 Channel, MI_S32 *data);
    MI_S32 MI_ISP_SetDebugLevel_AWB(MI_U32 Channel, MI_S32 *data);
    MI_S32 MI_ISP_GetDebugLevel_AWB(MI_U32 Channel, MI_S32 *data);
    /************************************* Debug  API END********************************/

    MI_S32 MI_ISP_SET_ALL(int Channel, MI_U16 ApiId, MI_U32 ApiLen, MI_U8* pApiBuf);
    MI_S32 MI_ISP_GET_ALL(int Channel, MI_U16 ApiId, MI_U32* ApiLen, MI_U8* pApiBuf);

    MI_S32 MI_ISP_API_CmdLoadBinFile(MI_U32 Channel, char* filepath, MI_U32 user_key);
    MI_S32 MI_ISP_API_CmdLoadCaliData(MI_U32 Channel, MI_ISP_CALI_ITEM_e eCaliItem, char* filepath);

    typedef MI_S32 (*MI_ISP_ApiAgent_fp)(MI_VPE_IspApiHeader_t *pstData, void* pData);

    /*Api agent for SigmaStar user space 3A library*/
    MI_S32 MI_ISP_RegisterIspApiAgent(MI_U32 Channel, MI_ISP_ApiAgent_fp fpApiSet, MI_ISP_ApiAgent_fp fpApiGet);
#if !defined(DUAL_OS) && !defined(__RTOS__) //for pure linux
    MI_S32 MI_ISP_EnableUserspace3A(MI_U32 Channel,MI_ISP_API_USERSPACE3A_ATTR_t *pAttr);
    MI_S32 MI_ISP_DisableUserspace3A(MI_U32 Channel);
#else //for DualOS-Linux , RTK
    inline MI_S32 MI_ISP_EnableUserspace3A(MI_U32 Channel,MI_ISP_API_USERSPACE3A_ATTR_t *pAttr) {return 0;}
    inline MI_S32 MI_ISP_DisableUserspace3A(MI_U32 Channel) {return 0;}
#endif

    /*Common API, get isp root path*/
    MI_S32 MI_ISP_GetIspRoot(MI_U32 Channel, MI_ISP_ROOT_PATH_T *data);

#ifdef __cplusplus
}   //end of extern C
#endif

#endif  //_MI_ISP_H_
