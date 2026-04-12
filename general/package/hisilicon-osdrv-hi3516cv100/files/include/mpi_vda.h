/******************************************************************************
*
*  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.
*
*******************************************************************************
*  File Name     : mpi_vda.h
*  Version       : Initial Draft
*  Author        : j00169368,l64467
*  Created       : 2011/1/7
*  Last Modified :
*  Description   : mpi functions declaration
*  Function List :
*
*
*  History:
* 
*       1.  Date         : 2011/1/7
*           Author       : j00169368,l64467
*           Modification : Created file
*
******************************************************************************/

#ifndef __MPI_VDA_H__
#define __MPI_VDA_H__

#include "hi_comm_vda.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/*****************************************************************************
 Prototype       : HI_MPI_VDA_CreateChn
 Description     : Create the vda channel.
 Input           : VdaChn
                   pstAttr
 Output          : None
 Return Value    : 
 Process         : 	
 					
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_CreateChn(VDA_CHN VdaChn, const VDA_CHN_ATTR_S *pstAttr);




/*****************************************************************************
 Prototype       : HI_MPI_VDA_DestroyChn
 Description     : Destroy the vda channel.
 Input           : VdaChn
 Output          : None
 Return Value    : 
 Process         : Firstly,stop to receive image,and then unbind to vi,lastly,destroy the channel.
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_DestroyChn(VDA_CHN VdaChn);





/*****************************************************************************
 Prototype       : HI_MPI_VDA_GetChnAttr
 Description     : Get the channel's attribute.
 Input           : VdaChn
 Output          : pstAttr
 Return Value    : 
 Process         : Return the chanel's attribute.
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_GetChnAttr(VDA_CHN VdaChn, VDA_CHN_ATTR_S *pstAttr);




/*****************************************************************************
 Prototype       : HI_MPI_VDA_SetChnAttr
 Description     : Set the channel's attribute,it must be used after calling HI_MPI_VDA_CreateChn.
 Input           : VdaChn
                   pstAttr
 Output          : None
 Return Value    : 
 Process         : 	
 Note			 : Only the dynamic attribute can be changed.
 										
  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_SetChnAttr(VDA_CHN VdaChn, const VDA_CHN_ATTR_S *pstAttr);






/*****************************************************************************
 Prototype       : HI_MPI_VDA_StartRecvPic
 Description     : The VDA channel start to receive image.
 Input           : VdaChn
 Output          : None
 Return Value    : 
 Process         : Set the flag which indicates whether the channel start to receive image to true.
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_StartRecvPic(VDA_CHN VdaChn);




/*****************************************************************************
 Prototype       : HI_MPI_VDA_StopRecvPic
 Description     : The VDA channel stop to receive image.
 Input           : VdaChn
 Output          : None
 Return Value    : 
 Process         : 	Set the flag which indicates whether the channel start to receive image to false.
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_StopRecvPic(VDA_CHN VdaChn);




/*****************************************************************************
 Prototype       : HI_MPI_VDA_GetData
 Description     : Get the the result.
 Input           : VdaChn
                   u32BlockFlag
 Output          : pstVDAData
 Return Value    : 
 Process         : 
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_GetData(VDA_CHN VdaChn, VDA_DATA_S *pstVdaData, HI_BOOL bBlock);




/*****************************************************************************
 Prototype       : HI_MPI_VDA_ReleaseData
 Description     : Release the result.
 Input           : VdaChn
                   pstVDAData
 Output          : None
 Return Value    : 
 Process         :	
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_ReleaseData(VDA_CHN VdaChn, const VDA_DATA_S* pstVdaData);



/*****************************************************************************
 Prototype       : HI_MPI_VDA_Reset
 Description     : Reset OD region.
 Input           : VdaChn
                   s32RgnIndex
 Output          : None
 Return Value    : 
 Process         :
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_ResetOdRegion(VDA_CHN VdaChn, HI_S32 s32RgnIndex);



/*****************************************************************************
 Prototype       : HI_MPI_VDA_Query
 Description     : Query the state of VDA channel.
 Input           : VdaChn
 Output          : stChnStat
 Return Value    : 
 Process         : 	
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_Query(VDA_CHN VdaChn, VDA_CHN_STAT_S *pstChnStat);




/*****************************************************************************
 Prototype       : HI_MPI_VDA_GetFd
 Description     : Get the file handle.
 Input           : VdaChn
 Output          : None
 Return Value    : 
 Process         :
 Note			 : 

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_GetFd(VDA_CHN VdaChn);


/*****************************************************************************
 Prototype       : HI_MPI_VDA_UpdateRef
 Description     : Update the reference image.
 Input           : VdaChn
                   pstRefFrame
 Output          : None
 Return Value    : 
 Process         :
 Note			 : 1,Only be used when the reference mode is user mode.

  History         
  1.Date         : 2011/1/10
    Author       : l64467,j00169368
    Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_UpdateRef(VDA_CHN VdaChn, const VIDEO_FRAME_INFO_S *pstRefFrame);


/*****************************************************************************
 Prototype       : HI_MPI_VDA_UserSendPic
 Description     :User send image
 Input           : VdaChn
                   pstRefFrame
                   bBlock
                   u32MilliSec
 Output          : None
 Return Value    : 
 Process         :
 Note			 : 

  History         
  1.Date         : 2012/05/08
  Author       : s00187460
  Modification : Created function

*****************************************************************************/
HI_S32 HI_MPI_VDA_UserSendPic(VDA_CHN VdaChn, const VIDEO_FRAME_INFO_S *pstUserFrame, HI_BOOL bBlock, HI_U32 u32MilliSec);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __MPI_VDA_H__ */
