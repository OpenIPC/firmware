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
#ifndef _MI_DIVP_H_
#define _MI_DIVP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mi_divp_datatype.h"

#define DIVP_MAJOR_VERSION 2
#define DIVP_SUB_VERSION 4
#define MACRO_TO_STR(macro) #macro
#define DIVP_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_divp_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_divp_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_divp_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_DIVP_API_VERSION DIVP_VERSION_STR(DIVP_MAJOR_VERSION,DIVP_SUB_VERSION)

//------------------------------------------------------------------------------
/// @brief create a DIVP channel.
/// @param[out] pDivpChn: DIVP channel ID.
/// @param[in] pstAttr: Attribute of DIVP channel.
/// @return MI_SUCCESS: succeed in creating a channel.
///             MI_DIVP_ERR_INVALID_PARAM: invalid input patamter.
///             MI_DIVP_ERR_NULL_PTR: NULL poiter error.
///             MI_DIVP_ERR_FAILED: Fail to create a channel.
///             MI_DIVP_ERR_NO_RESOUCE: there is no resource.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_CreateChn (MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);

//------------------------------------------------------------------------------
/// @brief deatroy a DIVP channel.
/// @param[in] DivpChn: DIVP channel ID.
/// @return MI_SUCCESS: succeed in deatroying a DIVP channel.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_FAILED: Fail to deatroy a DIVP channel.
///             MI_DIVP_ERR_CHN_BUSY:channel is busy.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_DestroyChn(MI_DIVP_CHN DivpChn);

//------------------------------------------------------------------------------
/// @brief set attribute of DIVP channel.
/// @param[in] DivpChn: DIVP channel ID.
/// @param[in] pstAttr: Attribute of DIVP channel.
/// @return MI_SUCCESS: succeed in setting attribute of DIVP channel.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_INVALID_PARAM: Invalid input patamter.
///             MI_DIVP_ERR_NULL_PTR: NULL poiter error.
///             MI_DIVP_ERR_CHN_NOT_SUPPORT: not support.
///             MI_DIVP_ERR_FAILED: Fail to set attribute of DIVP channel.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_SetChnAttr(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);

//------------------------------------------------------------------------------
/// @brief get attribute of DIVP channel.
/// @param[in] DivpChn: DIVP channel ID.
/// @param[in] pstAttr: Attribute of DIVP channel.
/// @return MI_SUCCESS: succeed in getting attribute of DIVP channel.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_NULL_PTR: NULL poiter error.
///             MI_DIVP_ERR_FAILED: Fail to get attribute of DIVP channel.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_GetChnAttr(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);

//------------------------------------------------------------------------------
/// @brief start a DIVP channel.
/// @param[in] DivpChn: DIVP channel ID.
/// @return MI_SUCCESS: succeed in starting a DIVP channel.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_FAILED: Fail to start a DIVP channel.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_StartChn(MI_DIVP_CHN DivpChn);

//------------------------------------------------------------------------------
/// @brief stop a DIVP channel.
/// @param[in] DivpChn: DIVP channel ID.
/// @return MI_SUCCESS: succeed in stopping a DIVP channel.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_FAILED: Fail to stop a DIVP channel.
///             MI_DIVP_ERR_CHN_BUSY:channel is busy.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_StopChn(MI_DIVP_CHN DivpChn);

//------------------------------------------------------------------------------
/// @brief set attribute of DIVP channel output port.
/// @param[in] DivpChn: DIVP channel ID.
/// @param[in] pstOutputPortAttr: DIVP channel output port attribute.
/// @return MI_SUCCESS: succeed in setting attribute of DIVP channel output port.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_INVALID_PARAM: Invalid input patamter.
///             MI_DIVP_ERR_NULL_PTR: NULL poiter error.
///             MI_DIVP_ERR_FAILED: Fail to set attribute of DIVP channel output port.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_SetOutputPortAttr (MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr);

//------------------------------------------------------------------------------
/// @brief get attribute of DIVP channel output port.
/// @param[in] DivpChn: DIVP channel ID.
/// @param[out] pstOutputPortAttr: DIVP channel output port attribute.
/// @return MI_SUCCESS: succeed in getting attribute of DIVP channel output port.
///             MI_DIVP_ERR_FAILED: Fail to get attribute of DIVP channel output port.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_NULL_PTR: NULL poiter error.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_GetOutputPortAttr (MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr);

//------------------------------------------------------------------------------
/// @brief refresh a DIVP channel.
/// @param[in] DivpChn: DIVP channel ID.
/// @return MI_SUCCESS: succeed in refreshing a DIVP channel.
///             MI_DIVP_ERR_INVALID_CHNID: Invalid channel ID.
///             MI_DIVP_ERR_FAILED: Fail to refresh a DIVP channel.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_RefreshChn(MI_DIVP_CHN DivpChn);

//------------------------------------------------------------------------------
/// @brief stretch or crop image in specified memory to target memory
/// @param[in] pstSrcBuf: source buff info pointer
/// @param[in] pstSrcCrop: crop info pointer
/// @param[in] pstDstBuf: dest buff info pointer
/// @return MI_SUCCESS: succeed in stretching image
///             MI_DIVP_ERR_FAILED: Fail to stretch image
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_StretchBuf(MI_DIVP_DirectBuf_t *pstSrcBuf, MI_SYS_WindowRect_t *pstSrcCrop, MI_DIVP_DirectBuf_t *pstDstBuf);

//------------------------------------------------------------------------------
/// @brief Init DIVP module.
/// @return MI_SUCCESS: succeed in Init DIVP module.
///             MI_DIVP_ERR_FAILED: Fail to Init DIVP module.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_InitDev(MI_DIVP_InitParam_t *pstInitParam);

//------------------------------------------------------------------------------
/// @brief DeInit DIVP module.
/// @return MI_SUCCESS: succeed in DeInit DIVP module.
///             MI_DIVP_ERR_FAILED: Fail to DeInit DIVP module.
//------------------------------------------------------------------------------
MI_S32 MI_DIVP_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif
