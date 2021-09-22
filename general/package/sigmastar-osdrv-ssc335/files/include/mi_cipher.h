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
#ifndef _MI_CIPHER_H_
#define _MI_CIPHER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"
#include "mi_cipher_datatype.h"

#define CIPHER_MAJOR_VERSION 2
#define CIPHER_SUB_VERSION 3
#define MACRO_TO_STR(macro) #macro
#define CIPHER_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_cipher_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_cipher_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_cipher_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_CIPHER_API_VERSION CIPHER_VERSION_STR(CIPHER_MAJOR_VERSION,CIPHER_SUB_VERSION)

MI_S32 MI_CIPHER_Init(void);
MI_S32 MI_CIPHER_Uninit (void);
MI_S32 MI_CIPHER_CreateHandle(MI_HANDLE *phandle);
MI_S32 MI_CIPHER_DestroyHandle(MI_HANDLE handle);
MI_S32 MI_CIPHER_ConfigHandle(MI_HANDLE handle, MI_CIPHER_Config_t *pconfig);
MI_S32 MI_CIPHER_Encrypt(MI_HANDLE handle, void* srcAddr, void* dstAddr , MI_U32 u32ByteLen, MI_U32 *pu32dstByteLen);
MI_S32 MI_CIPHER_Decrypt(MI_HANDLE handle, void* srcAddr, void* dstAddr, MI_U32 u32ByteLen, MI_U32 *pu32dstByteLen);
MI_S32 MI_CIPHER_HashInit(MI_CIPHER_HASH_ALGO_e eHashAlgoType, MI_HANDLE *pHashHandle);
MI_S32 MI_CIPHER_HashUnInit(MI_HANDLE hHashHandle);
MI_S32 MI_CIPHER_HashUpdate(MI_HANDLE hHashHandle , MI_U8 *pu8InputData, MI_U32 u32IDataLen);
MI_S32 MI_CIPHER_HashFinal(MI_HANDLE hHashHandle, MI_U8 *pu8OutputHash, MI_U32 *pu32OutputHashLen);
MI_S32 MI_CIPHER_RsaPublicEncrypt(MI_CIPHER_RSA_PUB_ENC_t *pstRsa, MI_U8 *pu8Input, MI_U32 u32InLen, MI_U8 *pu8Output, MI_U32 *pu32OutLen);
MI_S32 MI_CIPHER_RsaPrivateDecrypt(MI_CIPHER_RSA_PRI_ENC_t *pstRsa, MI_U8 *pu8Input, MI_U32 u32InLen, MI_U8 *pu8Output, MI_U32 *pu32OutLen);
MI_S32 MI_CIPHER_RsaPrivateEncrypt(MI_CIPHER_RSA_PRI_ENC_t *pstRsa, MI_U8 *pu8Input, MI_U32 u32InLen, MI_U8 *pu8Output, MI_U32 *pu32OutLen);
MI_S32 MI_CIPHER_RsaPublicDecrypt(MI_CIPHER_RSA_PUB_ENC_t *pstRsa, MI_U8 *pu8Input, MI_U32 u32InLen, MI_U8 *pu8Output, MI_U32 *pu32OutLen);
MI_S32 MI_CIPHER_RsaSign(MI_CIPHER_RSA_SIGN_t *pstRsaSign, MI_U8 *pu8InHashData, MI_U32 u32HashDataLen, MI_U8 *pu8OutSign, MI_U32 *pu32OutSignLen);
MI_S32 MI_CIPHER_RsaVerify(MI_CIPHER_RSA_VERIFY_t *pstRsaVerify, MI_U8 *pu8InHashData, MI_U32 u32HashDataLen, MI_U8 *pu8InSign, MI_U32 u32InSignLen);
MI_S32 MI_CIPHER_InitDev(MI_CIPHER_InitParam_t *pstInitParam);
MI_S32 MI_CIPHER_DeInitDev(void);

#ifdef __cplusplus
}
#endif

#endif ///_MI_CIPHER_H_
