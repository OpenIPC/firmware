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
#ifndef _MI_CIPHER_DATATYPE_H_
#define _MI_CIPHER_DATATYPE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"
#include "mi_common_datatype.h"

/* min length of encrypt, unit: byte*/
#define MI_CIPHER_MIN_CRYPT_LEN    8

/*max length of encrypt, unit: byte*/
#define MI_CIPHER_MAX_CRYPT_LEN      0xfffff

#define MI_CIPHER_KEY_SIZE_MAX    32
#define MI_CIPHER_AES_BLOCK_SIZE  16
#define MI_CIPHER_MAX_DEVICE_NUM   1
#define CIPHER_MAX_RSA_KEY_LEN    (512)
#define DRV_MAX_RSA_KEY_LEN       (128)


/*-------------------------------------------------------------------------------------------------
 * Enum
 ------------------------------------------------------------------------------------------------*/
 /*cipher work mode*/
typedef enum
{
    MI_CIPHER_ALG_AES_ECB = 0,
    MI_CIPHER_ALG_AES_CBC,
    MI_CIPHER_ALG_AES_CTR,
} MI_CIPHER_ALG_e;

typedef enum
{
    MI_CIPHER_HASH_ALG_SHA1,
    MI_CIPHER_HASH_ALG_SHA256,
    MI_CIPHER_HASH_TYPE_BUTT,
} MI_CIPHER_HASH_ALGO_e;

typedef enum
{
    MI_CIPHER_RSA_ENC_SCHEME_NO_PADDING,            /**< without padding */
    MI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1,       /**< PKCS#1 RSAES-OAEP-SHA1 padding*/
    MI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256,     /**< PKCS#1 RSAES-OAEP-SHA256 padding*/
    MI_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5,      /**< PKCS#1 RSAES-PKCS1_V1_5 padding*/
    MI_CIPHER_RSA_ENC_SCHEME_BUTT,
}MI_CIPHER_RSA_ENC_SCHEME_E;

typedef enum
{
    MI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1 = 0x100, /**< PKCS#1 RSASSA_PKCS1_V15_SHA1 signature*/
    MI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA256 signature*/
    MI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1,         /**< PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/
    MI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/
    MI_CIPHER_RSA_SIGN_SCHEME_BUTT,
}MI_CIPHER_RSA_SIGN_SCHEME_E;

/*-------------------------------------------------------------------------------------------------
 * Defines
 ------------------------------------------------------------------------------------------------*/

/* CIPHER Module ErrorCode */
#define MI_CIPHER_ERR_INVALID_DEVID     (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID))
#define MI_CIPHER_ERR_ILLEGAL_PARAM     (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM))
#define MI_CIPHER_ERR_NOT_ENABLED       (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_ENABLE))
#define MI_CIPHER_ERR_NOT_DISABLED      (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_DISABLE))
#define MI_CIPHER_ERR_NULL_PTR          (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR))
#define MI_CIPHER_ERR_INVALID_CHNID     (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID))
#define MI_CIPHER_ERR_NOT_CONFIG        (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG))
#define MI_CIPHER_ERR_NOT_SUPPORT       (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT))
#define MI_CIPHER_ERR_NOT_PERM          (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM))
#define MI_CIPHER_ERR_NOMEM             (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM))
#define MI_CIPHER_ERR_NOBUF             (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOBUF))
#define MI_CIPHER_ERR_BUF_EMPTY         (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY))
#define MI_CIPHER_ERR_BUF_FULL          (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL))
#define MI_CIPHER_ERR_SYS_NOTREADY      (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY))
#define MI_CIPHER_ERR_BUSY              (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY))
#define MI_CIPHER_ERR_MOD_NOTINIT       (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_INIT))
#define MI_CIPHER_ERR_MOD_INITED        (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INITED))
#define MI_CIPHER_ERR_FAILED            (MI_DEF_ERR( E_MI_MODULE_ID_CIPHER, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED))


/*-------------------------------------------------------------------------------------------------
 * Structures
 ------------------------------------------------------------------------------------------------*/
// bits
typedef enum {
    E_MI_CIPHER_KEY_SIZE_128 = 0,
    E_MI_CIPHER_KEY_SIZE_192,
    E_MI_CIPHER_KEY_SIZE_256,
} MI_CIPHER_KeySize_e;

typedef struct MI_CIPHER_Config_s
{
    MI_CIPHER_KeySize_e eKeySize;
    MI_U8    key[MI_CIPHER_KEY_SIZE_MAX];
    MI_U8    iv[MI_CIPHER_AES_BLOCK_SIZE];
    MI_CIPHER_ALG_e eAlg;
} MI_CIPHER_Config_t;

typedef struct MI_CIPHER_RSA_PUB_Key_s
{
    MI_U8*   pu8ExpE;
    MI_U8*   pu8ModN;
    MI_U32   expSize;
    MI_U32   modSize;
} MI_CIPHER_RSA_PUB_Key_t;

typedef struct MI_CIPHER_RSA_PRI_Key_s
{
    MI_U8*   pu8ExpD;
    MI_U8*   pu8ModN;
    MI_U32   expSize;
    MI_U32   modSize;
} MI_CIPHER_RSA_PRI_Key_t;

typedef struct MI_CIPHER_RSA_PUB_ENC_s
{
    MI_CIPHER_RSA_ENC_SCHEME_E eRsaAlgoType;
    MI_CIPHER_RSA_PUB_Key_t stPubKey;
} MI_CIPHER_RSA_PUB_ENC_t;

typedef struct MI_CIPHER_RSA_PRI_ENC_s
{
    MI_CIPHER_RSA_ENC_SCHEME_E eRsaAlgoType;
    MI_CIPHER_RSA_PRI_Key_t stPriKey;
} MI_CIPHER_RSA_PRI_ENC_t;

typedef struct MI_CIPHER_RSA_SIGN_s
{
    MI_CIPHER_RSA_SIGN_SCHEME_E eRsaAlgoType;
    MI_CIPHER_RSA_PRI_Key_t stPriKey;
} MI_CIPHER_RSA_SIGN_t;

typedef struct MI_CIPHER_RSA_VERIFY_s
{
    MI_CIPHER_RSA_SIGN_SCHEME_E eRsaAlgoType;
    MI_CIPHER_RSA_PUB_Key_t stPubKey;
} MI_CIPHER_RSA_VERIFY_t;

typedef struct MI_CIPHER_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_CIPHER_InitParam_t;

#ifdef __cplusplus
}
#endif
#endif //_MI_CIPHER_DATATYPE_H_
