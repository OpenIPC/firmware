#ifndef __MPI_VI_H__
#define __MPI_VI_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "hi_comm_vi.h"

HI_S32 HI_MPI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);
HI_S32 HI_MPI_VI_GetDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);

HI_S32 HI_MPI_VI_EnableDev(VI_DEV ViDev);
HI_S32 HI_MPI_VI_DisableDev(VI_DEV ViDev);

HI_S32 HI_MPI_VI_SetChnAttr(VI_CHN ViChn, const VI_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VI_GetChnAttr(VI_CHN ViChn, VI_CHN_ATTR_S *pstAttr);

/* The following 3 functions are only for vichn minor attributes */
HI_S32 HI_MPI_VI_SetChnMinorAttr(VI_CHN ViChn,const VI_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VI_GetChnMinorAttr(VI_CHN ViChn,VI_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VI_ClearChnMinorAttr(VI_CHN ViChn);

HI_S32 HI_MPI_VI_EnableChn(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableChn(VI_CHN ViChn);

HI_S32 HI_MPI_VI_GetFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo);
HI_S32 HI_MPI_VI_GetFrameTimeOut(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_U32 u32MilliSec);
HI_S32 HI_MPI_VI_ReleaseFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo);
HI_S32 HI_MPI_VI_SetFrameDepth(VI_CHN ViChn, HI_U32 u32Depth);
HI_S32 HI_MPI_VI_GetFrameDepth(VI_CHN ViChn, HI_U32 *pu32Depth);

HI_S32 HI_MPI_VI_SetUserPic(VI_CHN ViChn, VI_USERPIC_ATTR_S *pstUsrPic);
HI_S32 HI_MPI_VI_EnableUserPic(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableUserPic(VI_CHN ViChn);

/* These functions are used to start the cascade mode. VI cascade mode can work normally Only when they have been called */
HI_S32 HI_MPI_VI_EnableCascade(VI_DEV ViDev);
HI_S32 HI_MPI_VI_DisableCascade(VI_DEV ViDev);
HI_S32 HI_MPI_VI_EnableCascadeChn(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableCascadeChn(VI_CHN ViChn);

/* Normally, these functions are not necessary in typical business */
HI_S32 HI_MPI_VI_ChnBind(VI_CHN ViChn, const VI_CHN_BIND_ATTR_S *pstChnBindAttr);
HI_S32 HI_MPI_VI_ChnUnBind(VI_CHN ViChn);
HI_S32 HI_MPI_VI_GetChnBind(VI_CHN ViChn, VI_CHN_BIND_ATTR_S *pstChnBindAttr);

HI_S32 HI_MPI_VI_SetDevAttrEx(VI_DEV ViDev, const VI_DEV_ATTR_EX_S *pstDevAttrEx);
HI_S32 HI_MPI_VI_GetDevAttrEx(VI_DEV ViDev, VI_DEV_ATTR_EX_S *pstDevAttrEx);

HI_S32 HI_MPI_VI_GetFd(VI_CHN ViChn);

HI_S32 HI_MPI_VI_Query(VI_CHN ViChn, VI_CHN_STAT_S *pstStat);

HI_S32 HI_MPI_VI_EnableChnInterrupt(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableChnInterrupt(VI_CHN ViChn);

HI_S32 HI_MPI_VI_SetFlashConfig(VI_DEV ViDev, const VI_FLASH_CONFIG_S *pstFlashConfig);
HI_S32 HI_MPI_VI_GetFlashConfig(VI_DEV ViDev, VI_FLASH_CONFIG_S *pstFlashConfig);
HI_S32 HI_MPI_VI_FlashTrigger(VI_DEV ViDev, HI_BOOL bEnable);

HI_S32 HI_MPI_VI_SetExtChnAttr(VI_CHN ViChn, const VI_EXT_CHN_ATTR_S *pstExtChnAttr);
HI_S32 HI_MPI_VI_GetExtChnAttr(VI_CHN ViChn, VI_EXT_CHN_ATTR_S *pstExtChnAttr);

HI_S32 HI_MPI_VI_SetLDCAttr(VI_CHN ViChn, const VI_LDC_ATTR_S *pstLDCAttr);
HI_S32 HI_MPI_VI_GetLDCAttr(VI_CHN ViChn, VI_LDC_ATTR_S *pstLDCAttr);

HI_S32 HI_MPI_VI_SetCSCAttr(VI_DEV ViDev, const VI_CSC_ATTR_S *pstCSCAttr);
HI_S32 HI_MPI_VI_GetCSCAttr(VI_DEV ViDev, VI_CSC_ATTR_S *pstCSCAttr);

HI_S32 HI_MPI_VI_SetRotate(VI_CHN ViChn, const ROTATE_E enRotate);
HI_S32 HI_MPI_VI_GetRotate(VI_CHN ViChn, ROTATE_E *penRotate);

HI_S32 HI_MPI_VI_GetChnLuma(VI_CHN ViChn, VI_CHN_LUM_S *pstLuma);

HI_S32 HI_MPI_VI_EnableChn422to420(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableChn422to420(VI_CHN ViChn);

HI_S32 HI_MPI_VI_SetChnScanMode(VI_CHN ViChn, VI_SCAN_MODE_E enScanMode);
HI_S32 HI_MPI_VI_GetChnScanMode(VI_CHN ViChn, VI_SCAN_MODE_E *penScanMode);

/* for 3520D use */
HI_S32 HI_MPI_VI_SetPortMode(VI_DEV ViDev, const VI_PORTMODE_E enPortMode);
HI_S32 HI_MPI_VI_GetPortMode(VI_DEV ViDev, VI_PORTMODE_E *penPortMode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VI_H__ */



