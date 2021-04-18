#ifndef _ISP_PRIVATE_H_
#define _ISP_PRIVATE_H_

#include "xm_comm_3a.h"
#if (defined SOC_ALIOS) || (defined SOC_SYSTEM)
#include <pthread.h>
#endif



typedef struct _dpc_info_
{
	XM_BOOL bEnableStatic;
	XM_BOOL bEnableDetect;
	ISP_STATIC_DP_TYPE_E enStaticDPType;	/* Select static bright/dark defect-pixel calibration. */
	
	ISP_TRIGGER_STATUS_E enTriggerStatus;	/*status of bad pixel trigger*/
	XM_U16	u16BadPixelThreshMin;			/*RW,  Range: [0, 0xFFF] */
	XM_U16	u16BadPixelThreshMax;			/*RW,  Range: [0, 0xFFF] */
	XM_U16	u16BadPixelThresh;				/*R   Range: [0, 0xFFF] */
	XM_U16	u16BadPixelCountMax;			/*RW, limit of max number of bad pixel,  Range: [0, 0x3FF] */
	XM_U16	u16BadPixelCount;				/*RW, limit of min number of bad pixel, Range: [0, 0x3FF]*/
	XM_U16	u16BadPixelTriggerTime; 		/*RW, time limit for bad pixel trigger, in frame number ,Range: [0x0, 0x640]*/
	ISP_DYDPC_ATTR_S stDyDpc;
}ISP_DPC_INFO;


typedef struct _isp_frame
{
	XM_U32	way;	
}ISP_FRAME;


typedef struct _reg_data_
{
	XM_U32 	reg_addr; 
	XM_U32 	reg_data; 
	ISP_FRAME isp_frame;
}REG_DATA_S;



typedef struct _mem_info_
{
	XM_U32 u32PhyAddr;
	XM_VOID *pVirAddr;
	XM_U32 u32Size;
	XM_U8 u8Id;
}MEM_INFO;

typedef struct xmISP_API_ATTR_S
{
	ISP_PUB_ATTR_S stPubAttr;
	ISP_CHN_ATTR_S stChnAttr;
	XM_U8 u8StatusFlag;


	ISP_GAMMA_ATTR_S stGamma;
	ISP_BLACKLVL_ATTR_S stBlc;
	ISP_SATURATION_ATTR_S stSat;
	ISP_SHARPEN_ATTR_S stSharpen;
	ISP_SHARPENV2_ATTR_S stSharpenV2;
	ISP_2DNR_ATTR_S stNr2D;
	ISP_3DNR_ATTR_S stNr3D;
	ISP_3DNRV2_ATTR_S stNr3DV2;
	ISP_ANTI_FALSECOLOR_S stAntiFalseColor;
//	ISP_DP_ATTR_S stDpc;
	ISP_DPC_INFO stDpc;
	ISP_CSC_ATTR_S stCsc;
	ISP_CHROMA_ATTR_S stChromaAttr;
	ISP_DRC_ATTR_S stDrcAttr;
	ISP_DEFOG_ATTR_S stDefogAttr;
	ISP_DCI_ATTR_S stDciAttr;
	ISP_MODULE_CTRL_U stModuleCtrl;

} ISP_API_ATTR_S;

#define ISPDEV_NAME			"/dev/isp"

#define ISP_CMD 	'I'
#define CMD_REG_WRITE      _IOW(ISP_CMD,1,REG_DATA_S)
#define CMD_REG_READ       _IOW(ISP_CMD,2,REG_DATA_S)
#define CMD_REG_READS      _IOW(ISP_CMD,3,REG_DATA_S)
#define CMD_IRQ_READ       _IOW(ISP_CMD,4,REG_DATA_S)
#define CMD_AUDIO_READ     _IO(ISP_CMD,5)
#define CMD_MIPI_CHECK_INIT    _IOW(ISP_CMD,6,MIPI_CK_CMD)
#define CMD_MIPI_CHECK_RUN     _IO(ISP_CMD,7)


#define ISP_STATUS_UN				0
#define ISP_STATUS_PUBINIT			1
#define ISP_STATUS_INIT				2
#define ISP_STATUS_RUN				3
#define ISP_STATUS_EXIT				4


//ISP_MODULE_CTRL_U
#define ISP_MODULE_VIDEOTEST	(1<<0)
#define ISP_MODULE_DPC			(1<<1)
#define ISP_MODULE_BLC			(1<<2)
#define ISP_MODULE_GAMMAFE		(1<<3)
#define ISP_MODULE_SHANDING		(1<<4)
#define ISP_MODULE_WBGAIN		(1<<5)
#define ISP_MODULE_NR2D			(1<<6)
#define ISP_MODULE_NR3D			(1<<7)
#define ISP_MODULE_DRC			(1<<8)
#define ISP_MODULE_CCM			(1<<9)
#define ISP_MODULE_GAMMA		(1<<10)
#define ISP_MODULE_DEMOIRE		(1<<11)
#define ISP_MODULE_CC			(1<<12)
#define ISP_MODULE_SHARPNESS	(1<<13)
#define ISP_MODULE_RSV			(1<<14)
#define ISP_MODULE_ALL			(1<<31)



extern ISP_AE_REGISTER_S 		gAlgAeFun;
extern ISP_AWB_REGISTER_S 		gAlgAwbFun;


extern ISP_API_ATTR_S *pstIspApiAttr;


#ifdef SOC_SYSTEM
extern pthread_mutex_t gIspMutex;
extern pthread_mutex_t gIspMutex3;
extern pthread_mutex_t gIspMutex4;
extern XM_S32 gIspDevFd;

XM_S32 Open_IspDev(XM_VOID);
XM_S32 Close_IspDev(XM_VOID);

// ÐÅºÅ³õÊ¼»¯
XM_S32 IspIrqSignal_Init();

XM_S32 IspPthradMutex_lock(pthread_mutex_t *mutex);
XM_S32 IspPthradMutex_unLock(pthread_mutex_t *mutex);

XM_S32 Write_IspReg(XM_U32 u32Addr, XM_U32 u32Value);
XM_S32 Read_IspReg(XM_U32 u32Addr);

#elif (defined SOC_ALIOS)
extern pthread_mutex_t gIspMutex;
extern pthread_mutex_t gIspMutex3;
extern pthread_mutex_t gIspMutex4;

XM_S32 IspIrqSignal_Init();

XM_S32 IspPthradMutex_lock(pthread_mutex_t *mutex);
XM_S32 IspPthradMutex_unLock(pthread_mutex_t *mutex);

XM_S32 Write_IspReg(XM_U32 u32Addr, XM_U32 u32Value);
XM_S32 Read_IspReg(XM_U32 u32Addr);

#elif (defined SOC_NONE)
extern XM_S32 gIspMutex;
extern XM_S32 gIspMutex4;

XM_S32 IspPthradMutex_lock(XM_S32 *mutex);
XM_S32 IspPthradMutex_unLock(XM_S32 *mutex);

#define Write_IspReg(u32Addr, u32Value)  	(*((volatile unsigned long *)(u32Addr)) = (u32Value)) 
#define Read_IspReg(u32Addr)				(*((volatile unsigned long *)(u32Addr)))
#endif



XM_VOID* IspMmap(XM_U32 u32PhyAddr, XM_U32 u32Size);
XM_S32 IspMunmap(XM_VOID* pVirAddr, XM_U32 u32Size);
XM_S32 IspDPCScan(XM_U16 *pu16Addr,ISP_STDPC_ATTR_S *pstStDPAttr);
XM_VOID IspAutoScaneTask();
XM_S32 IspAePhyStatWinInit(XM_U16 u16TotalH, XM_U16 u16TotalV);
XM_S32 ispMemnCpy(XM_S8 *pSrc,XM_S8 *pDst, XM_U32 u32Num);
XM_S32 ispMemnSet(XM_S8 *pSrc,XM_S8 pDat, XM_U32 u32Num);

XM_S32 IspInit(XM_U32 u32SizeH, XM_U32 u32SizeV);

// Awb
XM_VOID IspAwbInit(ISP_AWB_WDW_ATTR_S *pstWinAttr);

XM_S32 IspAwbRsltSet(ISP_AWB_RESULT_S* const pstAwbRslt);


#endif


