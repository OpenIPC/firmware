
#ifndef _MPI_VDAM_H_
#define _MPI_VDAM_H_

#include "xm_comm_vdam.h"
//#include "ispMsg.h"

#define TOTAL_VI_CHN_NUM 1 


void XM_MPI_VDA_Setmovblefalg(XM_U8 channel,XM_U8 enble);
void XM_MPI_VDA_Getmovblefalg(XM_U8 channel,XM_U8 *enble);
void XM_MPI_VDA_SetMovMark(XM_U8 channel,VDA_MOTION_MOVE_RESULT *pstInfo);
void XM_MPI_VDA_GetMovMark(XM_U8 channel,VDA_MOTION_MOVE_RESULT *rstInfo);
void XM_MPI_VDA_Setshbleflag(XM_U8 channel,XM_U8 enble);
void XM_MPI_VDA_Getshbleflag(XM_U8 channel,XM_U8 *enble);
void XM_MPI_VDA_SetShdMark(XM_U8 channel,XM_U8 *shmark);
void XM_MPI_VDA_GetShdMark(XM_U8 channel,XM_U8 *shdmark);
int XM_MPI_VDA_SetMovAttr(XM_U8 channel,VDA_MOTION_MOVE_INIT *pstApInfo);
void XM_MPI_VDA_GetMovAttr(XM_U8 channel,VDA_MOTION_MOVE_INIT* gstApInfo);
void XM_MPI_VDA_CreatChn(XM_U8 VdaChn);
void XM_MPI_VDA_DestroyChn(XM_U8 VdaChn);
int XM_MPI_VDA_SetShAttr(XM_U8 channel,VDA_MOTION_SHELTER_INIT* pstApInfo);
int XM_MPI_VDA_GetShAttr(XM_U8 channel,VDA_MOTION_SHELTER_INIT* pstApInfo);
int XM_MPI_VDA_GetMovData(XM_U8 channel,VDA_MOTION_MOVE_RESULT* pstApInfo);
void XM_MPI_VDA_AfApinit(void);
int XM_MPI_VDA_Afgetdata(XM_U32 *rData);
int XM_MPI_VDA_VdaApInit(XM_U8 channel);
XM_U32 XM_MPI_VDA_VdaMovCmd(XM_U8 channel,VDA_MOTION_MOVE_RECDATA *pstApInfo);
int XM_MPI_VDA_VdaShCmd(XM_U8 channel,VDA_MOTION_SHELTER_RECDATA *pstApInfo);
int XM_MPI_VDA_VdaApiCmd(XM_U8 channel,ISP_AE_STAT_2_S* pstApInfo);



#endif 
 

