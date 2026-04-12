/******************************************************************************

  Copyright (C), 2013-2014, Hisilicon Tech. Co., Ltd.

******************************************************************************/


#ifndef __HI_PQ_BIN_H__
#define __HI_PQ_BIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


typedef struct hiPQ_REG_ATTR_S
{
    unsigned int u32IspRegAddr;
    unsigned int u32IspRegSize;
    unsigned int u32IspExtRegAddr;
    unsigned int u32IspExtRegSize;
    unsigned int u32AeExtRegAddr;
    unsigned int u32AeExtRegSize;
    unsigned int u32AwbExtRegAddr;
    unsigned int u32AwbExtRegSize;
}HI_PQ_REG_ATTR_S;



typedef struct hiPQ_BIN_EXTRA_S
{
    unsigned char  au8Author[32];   /* describe author */
    unsigned char  au8Desc[1024];   /* describe something if you need*/
    unsigned char  au8Time[32];     /* create time,yy mm dd hh mm ss */
}HI_PQ_BIN_EXTRA_S;



typedef struct hiPQ_BIN_ATTR_S
{
    unsigned int   u32ChipId;       /* 0x3518a 0x3516a*/
    HI_PQ_REG_ATTR_S  stIspRegAttr;    /*the isp reg addrs get from isp\
                                    get this para form HI_MPI_ISP_GetISPRegAttr*/
}HI_PQ_BIN_ATTR_S;


/*****************************************************************************
*   Prototype    : HI_PQ_BIN_Init
*   Description: : init bin attr 
*   Input        : stBinAttr: 
*   Output       : 
*   Return Value : 0: Success;
*****************************************************************************/
void HI_PQ_BIN_Init(HI_PQ_BIN_ATTR_S stBinAttr);


/*****************************************************************************
*   Prototype    : HI_PQ_BIN_GenerateBinFile
*   Description: : to generate a bin file 
*   Input        : fp: the pointer of file to be generated
*                  pstBinExtra: the isp reg addrs get from HI_MPI_ISP_GetISPRegAttr
*   Output       : 
*   Return Value : 0: Success;
*                  Error codes: 0xCB000001:input para is null
*                               0xCB000002:stIspRegAttr is wrong
*                               0xCB000003:malloc error
*                               0xCB000004:chipid not match
*****************************************************************************/

int HI_PQ_BIN_GenerateBinFile(FILE* fp,HI_PQ_BIN_EXTRA_S* pstBinExtra);


/*****************************************************************************
*   Prototype    : HI_PQ_BIN_GetBinExtraAttr
*   Description: : get imformation form struct HI_PQ_BIN_EXTRA_S 
*   Input        : pu8FileData:the real data of bin file to be parsed
*   Output       : pstBinExtraAttr
*   Return Value : 0: Success;
*                  Error codes: 0xCB000001:input para is null
*****************************************************************************/

int HI_PQ_BIN_GetBinExtraAttr(unsigned char* pu8FileData,HI_PQ_BIN_EXTRA_S* pstBinExtraAttr);



/*****************************************************************************
*   Prototype    : HI_PQ_BIN_ParseBinData
*   Description: : to parse bin data
*   Input        : pu8FileData:the real data of bin file to be parsed
                   u32DataLen:the data length of bin file 
*   Output       : 
*   Return Value : 0: Success;
*                  Error codes: 0xCB000001:input para is null
*                               0xCB000003:malloc error
*                               0xCB000005:lost some data
*                               0xCB000006:the size of file is not accordance
*                               0xCB000007:invalid reg label
*                               0xCB000008:data erro
*****************************************************************************/

int HI_PQ_BIN_ParseBinData(unsigned char* pu8FileData,unsigned int u32DataLen);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef __HI_PQ_BIN_H__ */

