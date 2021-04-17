#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"
#include "Camera.h"
#include "xm_print.h"
#include "X80_cmos.h"
ISP_CMOS_SNS_ATTR_S gstSnsAttr = {0};
XM_U32 gu32ShutNow = 100;
XM_U32 gau32AllGain = 16;
XM_U8 gu8Fps;
void (*pfn_gainLogic)(XM_U32 u32AllGain) = NULL;
void (*pfn_shutLogic)(XM_U32 u32IntTime) = NULL;

#ifdef SOC_NONE
#include "xm540_tmp.h"
#else
#include "xm510_tmp.h"
#endif





extern unsigned char gSnsDevAddr;	// 0x20
extern unsigned int  gSnsRegAddrByte;	// 0x2
extern unsigned int  gSnsRegDataByte;	// 0x2
extern unsigned char gAppEncoderinfo;  //0:Ahd; 1:Tvi 2:Cvi

static cmos_ecd_call cmos_ecd_ptr=NULL ;
XM_SENSOR_CHIP gSensorChip = SENSOR_CHIP_UNKNOW;
XM_U8 gu8Resolution = 1<<RSLTION_BIT_720P;
XM_U8 gu8SnsMode = 0;	// 0: Day 	1:Night

const  ISP_CMOS_GAMMA_S gstIspGamma = 		//gamma 96
{
	1,
	{	
	0x000,0x00d,0x01a,0x027,0x034,0x041,0x04d,0x059,0x065,0x070,0x07c,0x087,0x092,0x09d,0x0a7,0x0b2,
	0x0bc,0x0c7,0x0d1,0x0da,0x0e4,0x0ee,0x0f7,0x100,0x109,0x112,0x11b,0x124,0x12d,0x135,0x13d,0x146,
	0x14e,0x15d,0x16d,0x17c,0x18a,0x198,0x1a6,0x1b4,0x1c1,0x1ce,0x1da,0x1e6,0x1f2,0x1fe,0x209,0x214,
	0x21f,0x233,0x247,0x25a,0x26c,0x27e,0x28f,0x29f,0x2ae,0x2bd,0x2cb,0x2d9,0x2e6,0x2f3,0x2ff,0x30b,
	0x316,0x321,0x32c,0x337,0x341,0x34a,0x354,0x35d,0x366,0x36e,0x377,0x37f,0x387,0x38e,0x396,0x39d,
	0x3a4,0x3ab,0x3b2,0x3b8,0x3bf,0x3c5,0x3cb,0x3d1,0x3d7,0x3dc,0x3e2,0x3e7,0x3ec,0x3f2,0x3f7,0x3fc,
	0x3ff 
	}
};



int sensor_register_callback(ISP_DEV IspDev)
{
	XM_S32 s32Ret;
	ALG_LIB_S stLib;
	ISP_SENSOR_REGISTER_S stIspRegister;
	AE_SENSOR_REGISTER_S  stAeRegister;
	AWB_SENSOR_REGISTER_S stAwbRegister;

	IspDev = (IspDev<0)?0:((IspDev>(ISP_NUM_MAX-1))?(ISP_NUM_MAX-1):IspDev);
  	sensor_init();
	stAeRegister.stSnsExp.pfn_cmos_fps_get = NULL;
	switch(gSensorChip)
	{
		#ifdef SENS_X80_APOLLO
		case SENSOR_CHIP_APOLLO:
			cmos_init_sensor_exp_function_apollo(&(stIspRegister.stSnsExp));
			cmos_init_ae_exp_function_apollo(&(stAeRegister.stSnsExp));
			cmos_init_awb_exp_function_apollo(&(stAwbRegister.stSnsExp));
			break;
        #endif
		default:
				break;
	}
	s32Ret = XM_MPI_ISP_SensorRegCallBack((XM_S32)gSensorChip, (const ISP_SENSOR_REGISTER_S * const)&stIspRegister);
	if (s32Ret)
	{
		ERR("sensor register callback function failed!\n");
		return s32Ret;
	}
	stLib.s32Id = (XM_S32)gSensorChip;
	XM_MPI_ISP_Memncpy((XM_U8*)stLib.acLibName, (XM_U8*)XM_AE_LIB_NAME, sizeof(XM_AE_LIB_NAME));
	s32Ret = XM_MPI_AE_SensorRegCallBack(IspDev, &stLib, (XM_S32)gSensorChip, &stAeRegister);
	if (s32Ret)
	{
		ERR("sensor register callback function to ae lib failed!\n");
		return s32Ret;
	}
	stLib.s32Id = (XM_S32)gSensorChip;
	XM_MPI_ISP_Memncpy((XM_U8*)stLib.acLibName, (XM_U8*)XM_AWB_LIB_NAME, sizeof(XM_AWB_LIB_NAME));
	s32Ret = XM_MPI_AWB_SensorRegCallBack(&stLib, (XM_S32)gSensorChip, &stAwbRegister);
	if (s32Ret)
	{
		ERR("sensor register callback function to awb lib failed!\n");
		return s32Ret;
	}
	return 0;
}

int sensor_unregister_callback(ISP_DEV IspDev)
{
    XM_S32 s32Ret;
    ALG_LIB_S stLib;
    IspDev = (IspDev<0)?0:((IspDev>(ISP_NUM_MAX-1))?(ISP_NUM_MAX-1):IspDev);
    s32Ret = XM_MPI_ISP_SensorUnRegCallBack((XM_S32)gSensorChip);
    if (s32Ret)
    {
        ERR("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = (XM_S32)gSensorChip;
    s32Ret = XM_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, (XM_S32)gSensorChip);
    if (s32Ret)
    {
        ERR("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }
    return 0;
}



int sensor_mode_set(XM_U8 u8Mode)
{
	if(u8Mode & (1<< RSLTION_BIT_1080P))
		gu8Resolution = 1<<RSLTION_BIT_1080P;
	else if(u8Mode & (1<< RSLTION_BIT_960P))
		gu8Resolution = 1<<RSLTION_BIT_960P;
	else if(u8Mode & (1<< RSLTION_BIT_1536P))
		gu8Resolution = 1<<RSLTION_BIT_1536P;
	else if(u8Mode & (1<< RSLTION_BIT_4MP))
		gu8Resolution = 1<<RSLTION_BIT_4MP;
	else if(u8Mode & (1<< RSLTION_BIT_5MP))
		gu8Resolution = 1<<RSLTION_BIT_5MP;
	else if(u8Mode & (1<< RSLTION_BIT_8MP))
		gu8Resolution = 1<<RSLTION_BIT_8MP;
	else 
		gu8Resolution = 1<<RSLTION_BIT_5MP;

	return 0;
}

int  sensor_mode_get()
{
	int s32RsFlg = 1<<RSLTION_BIT_BUTT;
	switch(gSensorChip)
	{
		case SENSOR_CHIP_PS5510:
		case SENSOR_CHIP_SC5035:
		case SENSOR_CHIP_K03:
		case SENSOR_CHIP_SC5235:
		case SENSOR_CHIP_SC5300:
		case SENSOR_CHIP_IMX335:
				s32RsFlg=(1<<RSLTION_BIT_5MP);
				break;
		case SENSOR_CHIP_APOLLO:
				s32RsFlg=(1<<RSLTION_BIT_8MP);
				break;
        case SENSOR_CHIP_SC5239:
				s32RsFlg=(1<<RSLTION_BIT_4MP);
				break;
		default:	ERR("sensor_set_chip not support this sensor!\n");
				break;
	}
	return s32RsFlg;
}


int  sensor_get_chip(I2C_DATA_S *pstI2CData)
{
	XM_S32	s32Tmp,s32Id;
	if(pstI2CData == NULL)
		return -1;

	//SmartSns
	gSnsDevAddr = 0x30;
	gSnsRegAddrByte = 0x2;
	gSnsRegDataByte = 0x1;
	sensor_write_register(0x0103, 0x01);
	SysDelay_ms(100);
	s32Tmp = sensor_read_register(0x3107);
	s32Id = sensor_read_register(0x3108); 
	s32Id = ((s32Tmp<<8) |s32Id);
	//DEBUG("SENSOR ID = %x\n",s32Id);
	if(s32Id == 0x5235)
	{
        gSensorChip = SENSOR_CHIP_SC5235;
		goto RET_TAB;
	}
	else if(s32Id == 0x5300)
	{
        gSensorChip = SENSOR_CHIP_SC5300;
		goto RET_TAB;
	}
	else if(s32Id == 0x3235)
	{
		gSensorChip = SENSOR_CHIP_SC5239;
		DEBUG("SC5239!\n");
		goto RET_TAB;
	}
	else if(s32Id == 0x8235)
	{
		gSensorChip = SENSOR_CHIP_APOLLO;
		DEBUG("APOLLO!\n");
		goto RET_TAB;
	}
	// IMX335
	gSnsDevAddr = 0x1A;
	gSnsRegAddrByte = 0x2;
	gSnsRegDataByte = 0x1;
	s32Id = sensor_read_register(0x3302); 
	if(s32Id == 0x32) 	// K03
	{
		gSensorChip = SENSOR_CHIP_IMX335;
		goto RET_TAB;
	}
	// PS5510
	gSnsDevAddr = 0x18;
	gSnsRegAddrByte = 0x2;
	gSnsRegDataByte = 0x2;
	gSensorChip = SENSOR_CHIP_PS5510;
#if 0
	s32Id = sensor_read_register(0x3000);
	if(s32Id == 0x2604)
	{
		gSensorChip = SENSOR_CHIP_AR0330;
		goto RET_TAB;
	}
#endif

RET_TAB:	

	pstI2CData->dev_addr = gSnsDevAddr;
	pstI2CData->addr_byte_num = gSnsRegAddrByte;
	pstI2CData->data_byte_num = gSnsRegDataByte;
	return gSensorChip;
}


void  sensor_set_chip(XM_S32 s32SnsChip)
{
	switch(s32SnsChip)
	{
		case SENSOR_CHIP_K03:
			gSnsDevAddr = 0x40;
			gSnsRegAddrByte = 0x1;
			gSnsRegDataByte = 0x1;
			break;
		case SENSOR_CHIP_IMX335:
			gSnsDevAddr = 0x1A;
			gSnsRegAddrByte = 0x2;
			gSnsRegDataByte = 0x1;
			break;
        case SENSOR_CHIP_SC5235:
		case SENSOR_CHIP_SC5300:
		case SENSOR_CHIP_SC5239:
		case SENSOR_CHIP_APOLLO:
			gSnsDevAddr = 0x30;
			gSnsRegAddrByte = 0x2;
			gSnsRegDataByte = 0x1;
			break;
		default:	ERR("sensor_set_chip not support this sensor!\n");
				break;
	}
	gSensorChip = s32SnsChip;
}

int  sensor_get_temp(XM_S16 *ps16Temp)
{
	return XM_SUCCESS;
}

int sensor_set_ob(XM_U16 u16Ob)
{
	return XM_SUCCESS;
}


void  sensor_set_dnMode(XM_U8 u8Mode)
{
	;
}

int sensor_set_encoderinfo(XM_U8 *u8pDta)
{
	if(u8pDta == NULL)
		return XM_FAILURE;
	gAppEncoderinfo = *u8pDta;
	if(cmos_ecd_ptr != NULL)
		cmos_ecd_ptr(*u8pDta);
	return XM_SUCCESS;
}
int sensor_get_encoderinfo(XM_U8 *u8pDta)
{
	if(u8pDta == NULL)
		return XM_FAILURE;
	*u8pDta = gAppEncoderinfo ;
		return XM_SUCCESS;
}


void sensor_set_fps(XM_U8 channel , XM_U8 frame)
{
	return;		
}


/*************************************************************************************
输入参数:u8Mode 
				0x00: AHD
				0x01: CVI
				0x02: TVI
				0x03: CVBS
				0x10: XVI
*************************************************************************************/
void sensor_set_isp_para(XM_U8 u8Mode)
{
	;
}


/*************************************************************************************
输入参数: 	u8Mode(0:Stop  1:Restart)
*************************************************************************************/
void sensor_restart(XM_U8 u8Mode)
{
	return;	
}


XM_S32 cmos_set_sns_attr(ISP_CMOS_SNS_ATTR_S *pstSnsAttr)
{
	if(pstSnsAttr == NULL)
	{
		ERR("cmos_set_sns_attr failed!\n");
		return XM_FAILURE;
	}
	XM_MPI_ISP_Memncpy((XM_U8*)&gstSnsAttr, (XM_U8*)pstSnsAttr, sizeof(ISP_CMOS_SNS_ATTR_S));
	return XM_SUCCESS;
}


XM_S32 cmos_get_sns_attr(ISP_CMOS_SNS_ATTR_S *pstSnsAttr)
{
	if(pstSnsAttr == NULL)
	{
		ERR("cmos_get_sns_attr failed!\n");
		return XM_FAILURE;
	}
	XM_MPI_ISP_Memncpy((XM_U8*)pstSnsAttr, (XM_U8*)&gstSnsAttr, sizeof(ISP_CMOS_SNS_ATTR_S));
	return XM_SUCCESS;
}

