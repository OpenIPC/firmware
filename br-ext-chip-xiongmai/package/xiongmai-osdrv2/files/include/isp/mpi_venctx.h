#ifndef __MPI_VENCTX_H__
#define __MPI_VENCTX_H__

#include "xm_type.h"
#include "xm_common.h"


typedef enum {
	PCLK_36M = 0,
	PCLK_74x25M = 1,
	PCLK_117M = 2,
	PCLK_148x5M = 3,
	PCLK_42M = 4,
	PCLK_144M = 5,
	PCLK_37x125M = 6,
} VENC_PCLK;


typedef struct xm_VENC_SCALER_ATTR {
	XM_U8 u8EnableH;
	XM_U8 u8EnableV;
	XM_U16 u16RatioH;		// 1024:1    >1024:Up   <1024:Dn
	XM_U16 u16RatioV;		// 1024:1    >1024:Up   <1024:Dn(XM350 just Dn)
	SIZE_S stDestSize;
	VENC_PCLK enPclk;
}VENC_SCALER_ATTR;

typedef struct xm_VENC_TX_ATTR {
	XM_U32 u32TestMode;
	XM_U32 u32BlankVal_P;
	XM_U32 u32SyncVal_P;
	XM_U32 u32CSyncVal_P;
	XM_U32 u32SubCarrierPar_P;
	XM_U32 u32YColorGain_P;

	XM_U32 u32BlankVal_N;
	XM_U32 u32SyncVal_N;
	XM_U32 u32CSyncVal_N;
	XM_U32 u32SubCarrierPar_N;
	XM_U32 u32YColorGain_N;
}VENC_TX_ATTR;

typedef struct xm_VENC_TX_FUN {
	XM_S32 (*pFun_AHD)(XM_U8 u8StdMode);
	XM_S32 (*pFun_CVI)(XM_U8 u8StdMode);
	XM_S32 (*pFun_TVI)(XM_U8 u8StdMode);
	XM_S32 (*pFun_CVBS)(XM_U8 u8StdMode);
}VENC_TX_FUN;


XM_S32 XM_MPI_VENC_SetScaler(const VENC_SCALER_ATTR *pstVenAttr);
XM_S32 XM_MPI_VENC_GetScaler(VENC_SCALER_ATTR *pstVenAttr);


/**********************************************************************
函数功能:	配置色同步
输入参数:	u8BurstMode:  0  No CSync
							1  have CSync
				u8ColorMode:	0 BW
							1 Color
输出参数:	无
返回参数:	0: Success
				-1: Failure
**********************************************************************/
XM_S32 XM_MPI_VENC_SetColor(XM_U8 u8BurstMode, XM_U8 u8ColorMode);


/***************************************************************************
函数功能:   	编码模式(Tx) 配置
输入参数:	u8VencMode:   0:AHD  1:CVI  2:TVI
				u8VstdType: 	1:PAL	2:NTSC
				u8RsltType:	0: 720P  1:1080P  2:960P  3:3M ...
输出参数:	无
返回参数:	0: 成功
				-1: 出错
Note:	Lycai
****************************************************************************/
XM_S32 XM_MPI_VENCTX_SetMode(XM_U8 u8VencMode, XM_U8 u8VstdType, XM_U8 u8RsltType);

#endif

