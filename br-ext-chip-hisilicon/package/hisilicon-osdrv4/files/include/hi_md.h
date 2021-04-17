/******************************************************************************

  Copyright (C), 2001-2015, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_md.h
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
	Modification: Modify MD_ATTR_S
******************************************************************************/
#ifndef _HI_MD_H_
#define _HI_MD_H_

#include "hi_ive.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*
* Definition md algorithm mode
*/
typedef enum hiMD_ALG_MODE_E {
    MD_ALG_MODE_BG = 0x0,  /* Base on background image */
    MD_ALG_MODE_REF = 0x1, /* Base on reference image */

    MD_ALG_MODE_BUTT
} MD_ALG_MODE_E;
/*
* Definition of md attribute
*/
typedef struct hiMD_ATTR_S {
    MD_ALG_MODE_E enAlgMode;         /* Md algorithm mode */
    IVE_SAD_MODE_E enSadMode;        /* Sad mode */
    IVE_SAD_OUT_CTRL_E enSadOutCtrl; /* Sad output ctrl */
    HI_U32 u32Width;                 /* Image width */
    HI_U32 u32Height;                /* Image height */
    HI_U16 u16SadThr;                /* Sad thresh */
    IVE_CCL_CTRL_S stCclCtrl;        /* Ccl ctrl */
    IVE_ADD_CTRL_S stAddCtrl;        /* Add ctrl */
} MD_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _HI_MD_H_ */