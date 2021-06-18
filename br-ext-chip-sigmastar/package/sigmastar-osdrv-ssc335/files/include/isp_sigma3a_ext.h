/* SigmaStar 3A extension api */
#ifndef ISP_SIGMA3A_EXT_H
#define ISP_SIGMA3A_EXT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "mi_vpe_datatype.h"

typedef struct
{
    MI_U32 u32HeadSize;    //Size of MIIspApiHeader_t
    MI_U32 u32DataLen;     //Data length;
    MI_U32 u32CtrlID;      //Function ID
    MI_U32 u32Channel;     //Isp channel number
    MI_U32 u32Dir;     //Command direction, 1: Set, 2: Get
    MI_S32 s32Ret;         //Isp api retuen value
} MI_CUS3A_IspApiHeader_t;

typedef struct
{
    MI_CUS3A_IspApiHeader_t stHeader;
    MI_U8 u8Data[0];
}MI_CUS3A_IspApiCmd_t;

typedef struct
{
    MI_U32 u32Dir;  // 0: Set, 1: Get
    //MI_VPE_IspApiData_t stApiData;
    MI_VPE_IspApiHeader_t stApiHeader;
    void* pData;
}MI_CUS3A_CtrlCmd_t;

MI_S32 Cus3A_GetIspApiData(MI_VPE_IspApiHeader_t *pCmd, void *pstData);
MI_S32 Cus3A_SetIspApiData(MI_VPE_IspApiHeader_t *pCmd, void *pstData);

#ifdef __cplusplus
}
#endif

#endif
