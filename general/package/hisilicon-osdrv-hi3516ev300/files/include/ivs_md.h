/******************************************************************************

  Copyright (C), 2001-2015, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : ivs_md.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (IVE) group
  Created       : 2014/11/10
  Description   :
  History       :
  1.Date        : 2014/11/10
    Author      :
    Modification: Created file
  2.Date        : 2015/11/24
	Author      :
	Modification: Add HI_IVS_MD_Process sad parameter

******************************************************************************/

#ifndef _HI_IVS_MD_H_
#define _HI_IVS_MD_H_

#include "hi_md.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*****************************************************************************
*   Prototype    : HI_IVS_MD_Init
*   Description  : Motion Detection(MD) initialization.
*   Parameters   : HI_VOID.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014/11/11
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_IVS_MD_Init(HI_VOID);

/*****************************************************************************
*   Prototype    : HI_IVS_MD_Exit
*   Description  : Motion Detection(MD) exit.
*   Parameters   : HI_VOID.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014/11/11
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_IVS_MD_Exit(HI_VOID);

/*****************************************************************************
*   Prototype    : HI_IVS_MD_CreateChn
*   Description  : Create Motion Detection(MD) Chn.
*   Parameters   : MD_CHN			MdChn		Md chn.
*                  MD_ATTR_S		*pstMdAttr	Md attribute parameters
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014/11/11
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_IVS_MD_CreateChn(MD_CHN MdChn, MD_ATTR_S *pstMdAttr);

/*****************************************************************************
*   Prototype    : HI_IVS_MD_DestroyChn
*   Description  : Destroy Motion Detection(MD) chn.
*   Parameters   : MD_CHN		MdChn   Md chn that would be destroy.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014/11/11
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_IVS_MD_DestroyChn(MD_CHN MdChn);

/*****************************************************************************
*   Prototype    : HI_IVS_MD_SetChnAttr
*   Description  : Set Motion Detection(MD) chn attribute.
*   Parameters   : MD_CHN			MdChn		Md chn.
*                  MD_ATTR_S		*pstMdAttr	Md attribute parameters
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2015/04/10
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_IVS_MD_SetChnAttr(MD_CHN MdChn, MD_ATTR_S *pstMdAttr);

/*****************************************************************************
*   Prototype    : HI_IVS_MD_GetChnAttr
*   Description  : Get Motion Detection(MD) chn attribute.
*   Parameters   : MD_CHN			MdChn		Md chn.
*                  MD_ATTR_S		*pstMdAttr	Md attribute parameters
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2015/04/10
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_IVS_MD_GetChnAttr(MD_CHN MdChn, MD_ATTR_S *pstMdAttr);

/*****************************************************************************
*   Prototype    : HI_IVS_MD_GetBg
*   Description  : Get Motion Detection(MD) background image.
*   Parameters   : MD_CHN				MdChn		Md chn.
*                  IVE_DST_IMAGE_S	*pstBg		Output background image
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014/11/11
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_IVS_MD_GetBg(MD_CHN MdChn, IVE_DST_IMAGE_S *pstBg);

/*****************************************************************************
*   Prototype    : HI_IVS_MD_Process
*   Description  : Motion Detection(MD) process.
*   Parameters   : MD_CHN					MdChn	    Md chn.
*                  IVE_SRC_IMAGE_S		*pstCur	    Current image
* 				    IVE_SRC_IMAGE_S		*pstRef    Reference image
* 				    IVE_DST_IMAGE_S      *pstSad    Output result of sad value
* 				    IVE_DST_MEM_INFO_S	*pstBlob   Output blob
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2014/11/11
*           Author       :
*           Modification : Created function
*       2.  Date         : 2015/11/24
*           Author       :
*           Modification : Add pstSad parameter
*
*****************************************************************************/
HI_S32 HI_IVS_MD_Process(MD_CHN MdChn, IVE_SRC_IMAGE_S *pstCur, IVE_SRC_IMAGE_S *pstRef,
                         IVE_DST_IMAGE_S *pstSad, IVE_DST_MEM_INFO_S *pstBlob);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_IVS_MD_H_ */