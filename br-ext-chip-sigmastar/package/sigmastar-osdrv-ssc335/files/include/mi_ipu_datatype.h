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

#ifndef _MI_IPU_DATATYPE_H_
#define _MI_IPU_DATATYPE_H_


#define MI_IPU_MAX_TENSOR_DIM 8
#define MI_IPU_CHN_MAX 16
#define MI_IPU_MAX_INPUT_TENSOR_CNT 61
#define MI_IPU_MAX_OUTPUT_TENSOR_CNT 61
#define MI_IPU_MAX_TENSOR_CNT \
                    ((MI_IPU_MAX_INPUT_TENSOR_CNT>MI_IPU_MAX_OUTPUT_TENSOR_CNT)? \
                    MI_IPU_MAX_INPUT_TENSOR_CNT:MI_IPU_MAX_OUTPUT_TENSOR_CNT)

#define MAX_TENSOR_NAME_LEN     256
#define MAX_IPU_INPUT_OUTPUT_BUF_DEPTH 3
#define MI_IPU_MAX_TIMEOUT  (20*1000)    //20 seconds
#define MI_IPU_BOOT_MAX_TIMEOUT (5*1000)   //5 seconds

#define YUV420_W_PITCH_ALIGNMENT    16
#define YUV420_H_PITCH_ALIGNMENT        2
#define XRGB_W_PITCH_ALIGNMENT          16


typedef MI_U32 MI_IPU_CHN;
typedef int (*SerializedReadFunc)(void *dst_buf,int offset, int size, char *ctx);

typedef enum
{
    MI_IPU_FORMAT_U8,
    MI_IPU_FORMAT_NV12,
    MI_IPU_FORMAT_INT16,
    MI_IPU_FORMAT_INT32,
    MI_IPU_FORMAT_INT8,
    MI_IPU_FORMAT_FP32,
    MI_IPU_FORMAT_UNKNOWN,
    MI_IPU_FORMAT_ARGB8888,
    MI_IPU_FORMAT_ABGR8888,
} MI_IPU_ELEMENT_FORMAT;

typedef enum
{
    E_IPU_ERR_INVALID_CHNID = 1,        /* invalid channel ID */
    E_IPU_ERR_CHNID_EXIST,              /* channel exists */
    E_IPU_ERR_CHNID_UNEXIST,        /* channel unexists */
    E_IPU_ERR_NOMEM,        /* failure caused by malloc memory */
    E_IPU_ERR_NOBUF,            /* failure caused by malloc buffer */
    E_IPU_ERR_BADADDR,      /* bad address, buffer address doesn't get from IPU buffer allocator */
    E_IPU_ERR_SYS_TIMEOUT,  /* system timeout*/
    E_IPU_ERR_FILE_OPERATION,   /* file cannot be open or read or write */
    E_IPU_ERR_ILLEGAL_TENSOR_BUFFER_SIZE,   /* tensor buffer size cannot meet the requirement, usually less than requirement*/
    E_IPU_ERR_ILLEGAL_BUFFER_DEPTH,     /* input or output buffer depth quantum is more than maximum number */
    E_IPU_ERR_ILLEGAL_INPUT_OUTPUT_DESC,    /* network description is illegal, usually mean input or output buffer quantum is wrong */
    E_IPU_ERR_ILLEGAL_INPUT_OUTPUT_PARAM,   /* user's input or output buffer quantum isn't match network description */
    E_IPU_ERR_MAP,  /* address mapping error */
    E_IPU_ERR_INIT_FIRMWARE,    /* fail to init ipu firmware */
    E_IPU_ERR_CREATE_CHANNEL, /* fail to create channel */
    E_IPU_ERR_DESTROY_CHANNEL,  /* fail to destroy channel */
    E_IPU_ERR_INVOKE,   /* fail to invoke */
    E_IPU_ERR_SET_MALLOC_REGION,    /* fail to set malloc region for freertos */
    E_IPU_ERR_SET_IPU_PARAMETER,    /* fail to set IPU parameter */
    E_IPU_ERR_INVALID_PITCH_ALIGNMENT,  /* invalid pitch alignment */
    E_IPU_ERR_NO_CREATED_IPU_DEVICE,   /* there is no created IPU device */
    E_IPU_ERR_GET_IPU_VERSION,      /* fail to get ipu version from ipu firmware */
    E_IPU_ERR_MISMATCH_IPU_HEAD_FILE,    /* ipu head files version isn't matching */
    E_IPU_ERR_NO_SUPPORT_REQ,        /* ipu firmware do not support this request */
    E_IPU_ERR_FAILED,   /* unexpected error */
    E_IPU_ERR_SEND_REQUEST, /* failt to send request to IPU */
    E_IPU_ERR_GET_FIRMWARE_INFO, /* fail to get ipu firmware information */
    E_IPU_ERR_NO_AVAILABLE_CHNID = 1<<8,   /* there is no available channel */
}IPU_ErrCode_e;

typedef struct MI_IPU_TensorDesc_s {
    MI_U32 u32TensorDim;
    MI_IPU_ELEMENT_FORMAT eElmFormat;
    MI_U32 u32TensorShape[MI_IPU_MAX_TENSOR_DIM]; // 3,299,299
    char name[MAX_TENSOR_NAME_LEN];
    MI_U32 u32InnerMostStride;
    MI_FLOAT fScalar;
    MI_S64 s64ZeroPoint;
    MI_S32 s32AlignedBufSize;
} MI_IPU_TensorDesc_t;

typedef struct MI_IPU_SubNet_InputOutputDesc_s {
    MI_U32 u32InputTensorCount;
    MI_U32 u32OutputTensorCount;
    MI_IPU_TensorDesc_t astMI_InputTensorDescs[MI_IPU_MAX_INPUT_TENSOR_CNT];
    MI_IPU_TensorDesc_t astMI_OutputTensorDescs[MI_IPU_MAX_OUTPUT_TENSOR_CNT];
} MI_IPU_SubNet_InputOutputDesc_t;

typedef struct MI_IPU_Tensor_s {
    //MI_IPU_TensorShape_t *pstTensorShape;
    //MI_BOOL bRewiseRGBOrder;//only valid under U8 & C=3,4 or NV12
    void      *ptTensorData[2];
    MI_PHY  phyTensorAddr[2];//notice that this is miu bus addr,not cpu bus addr.
} MI_IPU_Tensor_t;

typedef struct MI_IPU_TensorVector_s {
    MI_U32 u32TensorCount;
    MI_IPU_Tensor_t  astArrayTensors[MI_IPU_MAX_TENSOR_CNT];
} MI_IPU_TensorVector_t;

typedef struct MI_IPU_DevAttr_s {
    MI_U32 u32MaxVariableBufSize;
    MI_U32 u32YUV420_W_Pitch_Alignment; //default is 16
    MI_U32 u32YUV420_H_Pitch_Alignment; //default is 2
    MI_U32 u32XRGB_W_Pitch_Alignment;   //default is 16
} MI_IPU_DevAttr_t;

typedef struct MI_IPUChnAttr_s {
    MI_U32 u32SubNetId;
    MI_U32 u32OutputBufDepth;
    MI_U32 u32InputBufDepth;
} MI_IPUChnAttr_t;

typedef struct MI_IPU_OfflineModelStaticInfo_s {
    MI_U32 u32VariableBufferSize;
    MI_U32 u32OfflineModelSize;
} MI_IPU_OfflineModelStaticInfo_t;

#endif // !_MI_IPU_DATATYPE_H_
