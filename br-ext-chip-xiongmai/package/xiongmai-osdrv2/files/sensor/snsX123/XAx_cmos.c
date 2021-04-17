#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"
#include "xm_print.h"
#include "XAx_cmos.h"
#include "xm530_isp.h"
XM_U16 gu16FullLines = 750;

XM_U32 gu32MaxAGainInit;
XM_U8 gu8Fps;

XM_U8 gu8MaxShutterOfst = 4;



XM_SENSOR_CHIP gSensorChip = SENSOR_CHIP_UNKNOW;
XM_U8 gu8Resolution = 1<<RSLTION_BIT_720P;
XM_U8 gu8GainMode = SMS_TAB3;	// 增益模式
//static const ISP_CMOS_AGC_TABLE_S* pstIspAgcTable = NULL;
XM_U8 gAppEncoderinfo;	// 0: AHD  1:CVI  2:TVI
ISP_CMOS_SNS_ATTR_S gstSnsAttr = {0};
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo;
ISP_I2C_DATA_S gstIspI2CData[8];
XM_U32 gau32PreI2CData[8];

// 高温逻辑/ 普通增益Cal
GAIN_MODE_DT gstGainModeDt = {
	.stOvTab = {0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF}
};

const XM_U16 gau16GainTbl_SmatSns[64]={
1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048,2176,2304,2432,2560,2688,2816,
2944,3072,3200,3328,3456,3584,3712,3840,3968,4096,4352,4608,4864,5120,5376,5632,5888,6144,6400,6656,6912,7168,7424,
7680,7936,8192,8704,9216,9728,10240,10752,11264,11776,12288,12800,13312,13824,14336,14848,15360,15872
};

static const ISP_AWB_CALIBRATION_V2_S gstAwbCalCom = {
   {0, 0, 4096, 2537, 3013, 1771, 593, 4096},
   {4096, 4096, 0, 0, 4096, 4096, 4096, 1888},
   {-536, -2892, -1496, -4096, -3037, -3792, -1331, -2560},
   213,
   0, 1675, 0, 2485,
   {0, 540, 949, 1255, 1371, 1499, 1654, 1784, 2485, 0, 0, 0, 0, 0, 0, 0},
   {2000, 2500, 2800, 4000, 4150, 5000, 6500, 7500, 12000, 0, 0, 0, 0, 0, 0, 0},
   {1619, 1024, 2112, 0}
};
static const AE_SENSOR_DEFAULT_V2_S *pstAeSnsDef = NULL;
void (*pfn_gainLogic)(XM_U32 u32AllGain) = NULL;
void (*pfn_shutLogic)(XM_U32 u32IntTime) = NULL;
XM_U32 gu32ShutNow = 100;
XM_U32 gau32AllGain = 16;
XM_U32 gu32AGainNow = 0;
XM_U32 gu32AGainMin = 0;
XM_U32 gu32DGainNow = 0;
XM_U32 gu32DGainMin = 0;
XM_U8 gu8BlcDpcFlg = 0;

static const ISP_AWB_CALIBRATION_V2_S* pstAwbCal = NULL;
static const ISP_COLORMATRIX_AUTO_S* pstCcm = NULL;
static const ISP_CMOS_AGC_TABLE_S *pstAgcTab = NULL;
XM_U32 (*pfn_sensor_getlist)(XM_U16 *pu16Num) = NULL;
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




static XM_S32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
	if (XM_NULL == pstDef)
	{
		return -1;
	}
	pstDef->stGamma.bValid = XM_TRUE;
	ExtCfg_GammaTab_Get(0, pstDef->stGamma.au16Gamma);
	pstDef->stGamma.bValid = XM_TRUE;
	XM_MPI_ISP_Memncpy((XM_U8 *)&(pstDef->stGamma),(XM_U8 *)&(gstIspGamma),sizeof(ISP_CMOS_GAMMA_S));
	XM_MPI_ISP_Memncpy((XM_U8*)&(pstDef->stAgcTbl), (XM_U8*)pstAgcTab, sizeof(ISP_CMOS_AGC_TABLE_S));
	return 0;
}

static XM_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	AE_SENSOR_DEFAULT_V2_S stAeSnsDef;
	XM_S32 s32Ret;
	if (XM_NULL == pstAeSnsDft)
	{
		return -1;
	}
	pstAeSnsDft->u32FullLinesStd = HD720P_LINES-4;
	pstAeSnsDft->u8AeCompensation = 50;	
	pstAeSnsDft->u32LinesPer500ms = pstAeSnsDft->u32FullLinesStd*25/2;
	pstAeSnsDft->u32FlickerFreq = 0;

	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MinISPDgain = 1 << pstAeSnsDft->u32ISPDgainShift;

	pstAeSnsDft->u32InitExp[0] = 750;
	pstAeSnsDft->u32InitExp[1] = 750;
	pstAeSnsDft->u32InitExp[2] = 750;
	pstAeSnsDft->u32InitExp[3] = 750;
	pstAeSnsDft->u16AlgMode = 0x00;
	
	pstAeSnsDft->u32UpdateCfg = 0x01000003;
	
	if(ExtCfg_CheckFun(1, ROM_FUN_AEDEF) == XM_SUCCESS)
	{
		stAeSnsDef.u32MaxIntTime = 700;
		cmos_get_ae_default_ext(&stAeSnsDef);
		pstAeSnsDef = (const AE_SENSOR_DEFAULT_V2_S *)&stAeSnsDef;

		s32Ret = ExtCfg_Lum_Get();
		if(s32Ret!= XM_FAILURE)	pstAeSnsDft->u8AeCompensation = (XM_U8)s32Ret;
	}
	
	if(pstAeSnsDef != NULL)
	{
		pstAeSnsDft->u32MaxIntTime = pstAeSnsDef->u32MaxIntTime;
		pstAeSnsDft->u32MinIntTime = pstAeSnsDef->u32MinIntTime;    
		pstAeSnsDft->u32MaxAgain = pstAeSnsDef->u32MaxAgain;
		pstAeSnsDft->u32MinAgain = pstAeSnsDef->u32MinAgain;
		pstAeSnsDft->u32MaxDgain = pstAeSnsDef->u32MaxDgain;  
		pstAeSnsDft->u32MinDgain = pstAeSnsDef->u32MinDgain;
		pstAeSnsDft->u32MaxISPDgain= pstAeSnsDef->u32MaxISPDgain;
		pstAeSnsDft->u32UpdateCfg = pstAeSnsDef->u32UpdateCfg;
		pstAeSnsDft->u32InitExp[0] = pstAeSnsDef->u32InitExp[0];
		pstAeSnsDft->u32InitExp[1] = pstAeSnsDef->u32InitExp[1];
		pstAeSnsDft->u32InitExp[2] = pstAeSnsDef->u32InitExp[2];
		pstAeSnsDft->u32InitExp[3] = pstAeSnsDef->u32InitExp[3];
		pstAeSnsDft->u16AlgMode = pstAeSnsDef->u16AlgMode;
	}
	
	gu32MaxAGainInit = pstAeSnsDef->u32MaxAgain;
	return 0;
}


#if 0
XM_S32 cmos_set_gainMode_data(XM_U8 u8Mode, GAIN_MODE_DT *pstGainDt)
{
	XM_U32 u32Len;
	u32Len = sizeof(GAIN_MODE_DT);
	XM_MPI_ISP_Memncpy((XM_U8*)&gstGainModeDt,(XM_U8*)pstGainDt, u32Len);

#ifdef DEBUG_ON
	// 调试用
	switch(u8Mode)
	{
		case SMS_LINE:
		case SOI_TAB:	DEBUG("cmos_set_gainMode_data(Line):\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stLinear.u8AgainSft);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stLinear.u8AgainMin);	DBG("\r\n");
						break;
		case SMS_TAB:
		case SMS_TAB1:
		case SMS_TAB2:
		case SMS_TAB3:	DEBUG("cmos_set_gainMode_data(SmsTab):\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u8En);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u8MinGain);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u16Reg[0]);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u32LimitUp);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u32LimitDn);	DBG("\r\n");
						break;
		case OV_TAB:		
		case OV_LINE:	
						DEBUG("cmos_set_gainMode_data(OV):\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stOvTab.u32Hold);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stOvTab.u32Again);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stOvTab.u32Dgain);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stOvTab.u32Blc);	DBG("\r\n");
						break;
		case PS_LINE:		DEBUG("cmos_set_gainMode_data(PS):\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u8En);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u16Reg[0]);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u16Reg[1]);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u32LimitUp);	DBG("\r\n");
						PrintInt(8,(XM_U32)gstGainModeDt.stHtLgc.u32LimitDn);	DBG("\r\n");
						break;
		default:			DEBUG("cmos_set_gainMode_data: NotSupport\n");
						break;

	}
	DEBUG("cmos_set_gainMode_data over!\n");
#endif
	return XM_SUCCESS;
}
#endif


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


XM_VOID cmos_gain_calc_table_com(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	if(XM_NULL == pstAeSnsGainInfo)
	{
		return;
	}
	pstAeSnsGainInfo->u32GainDb = u32InTimes;
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
	return;
}

static XM_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	const AWB_COEF_TABLE_S* pstAwb;
	if (XM_NULL == pstAwbSnsDft)
	{
		return XM_FAILURE;
	}
	pstAwb = (AWB_COEF_TABLE_S*)pstAwbCal;

	// Awb
	pstAwbSnsDft->pstRbTable = pstAwb;
	// CCM
	if(ExtCfg_CCM_Set(&(pstAwbSnsDft->stCcm)) != XM_SUCCESS)
	{
		XM_MPI_ISP_Memncpy((XM_U8*)&(pstAwbSnsDft->stCcm), (XM_U8*)pstCcm, sizeof(ISP_COLORMATRIX_AUTO_S));
	}
	return XM_SUCCESS;
}



/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
static XM_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	pstSensorExpFunc->pfn_cmos_sensor_init = NULL;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = NULL;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = NULL;
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = NULL;
	return 0;
}

static XM_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
	pstExpFuncs->pfn_cmos_fps_get = NULL;
	pstExpFuncs->pfn_cmos_fps_set           = NULL;
	pstExpFuncs->pfn_cmos_slow_framerate_set= NULL;    
	pstExpFuncs->pfn_cmos_inttime_update    = NULL;
	pstExpFuncs->pfn_cmos_gains_update      = NULL;
	pstExpFuncs->pfn_cmos_again_calc_table  = NULL;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = NULL;
	return 0;
}

static XM_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	return 0;
}


void sensor_init()
{
	XM_U32 u32Addr;
	XM_U16 u16Num;
	if(pfn_sensor_getlist == NULL)	return;
	
	u32Addr = (*pfn_sensor_getlist)(&u16Num);
	if((gSnsRegAddrByte == 2) || (gSnsRegDataByte == 2))
		sensor_input_2(u32Addr,u16Num);
	else
		sensor_input_1(u32Addr,u16Num);
	DEBUG("sensor_init Ok!\n");
}


int sensor_register_callback(ISP_DEV IspDev)
{
	XM_S32 s32Ret;
	ALG_LIB_S stLib;
	ISP_SENSOR_REGISTER_S stIspRegister;
	AE_SENSOR_REGISTER_S  stAeRegister;
	AWB_SENSOR_REGISTER_S stAwbRegister;
	IspDev = (IspDev<0)?0:((IspDev>(ISP_NUM_MAX-1))?(ISP_NUM_MAX-1):IspDev);
	gu32AGainNow = 0;
	gu32AGainMin = 0;
	gu32DGainNow = 0;
	gu32DGainMin = 0;	
	gu8BlcDpcFlg = 0;
	pstAwbCal = NULL;
	pstCcm = NULL;
	pstAgcTab = NULL;
	pstAeSnsDef = NULL;
	cmos_init_sensor_exp_function(&(stIspRegister.stSnsExp));
	cmos_init_ae_exp_function(&(stAeRegister.stSnsExp));
	cmos_init_awb_exp_function(&(stAwbRegister.stSnsExp));
	switch(gSensorChip)
	{
#ifdef SENS_X1X_SOIH65
		case SENSOR_CHIP_H65:
				cmos_init_sensor_exp_function_soi(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_soi(&(stAeRegister.stSnsExp));

			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_h65;
				pstCcm = &g_stAwbCcm_h65;
				pstAgcTab = &g_stIspAgcTable_h65;
				pstAeSnsDef = &gstAeSnsDef_h65;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_h65;
			#endif
				break;
#endif

#ifdef SENS_X1X_SP140A
		case SENSOR_CHIP_SP140A:
				cmos_init_sensor_exp_function_sp140A(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sp140A(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_sp140A;
				pstCcm = &g_stAwbCcm_sp140A;
				pstAgcTab = &g_stIspAgcTable_sp140A;
				pstAeSnsDef = &gstAeSnsDef_sp140A;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sp140A;
			#endif
				break;
#endif
#ifdef SENS_X1X_SOIH62
		case SENSOR_CHIP_H62:
				cmos_init_sensor_exp_function_h62(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_h62(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_h62;
				pstCcm = &g_stAwbCcm_h62;
				pstAgcTab = &g_stIspAgcTable_h62;
				pstAeSnsDef = &gstAeSnsDef_h62;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_h62;
			#endif
				break;
#endif
#ifdef SENS_X1X_OV9732
		case SENSOR_CHIP_OV9732:
				cmos_init_sensor_exp_function_OV9732(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_OV9732(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_OV9732;
				pstCcm = &g_stAwbCcm_OV9732;
				pstAgcTab = &g_stIspAgcTable_OV9732;
				pstAeSnsDef = &gstAeSnsDef_OV9732;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_OV9732;
			#endif
				break;
#endif
#ifdef SENS_X1X_SC1235
		case SENSOR_CHIP_SC1235:
				cmos_init_sensor_exp_function_sc2235(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc1235(&(stAeRegister.stSnsExp));
				pfn_gainLogic = gainLogic_sc1235;
				pfn_shutLogic = shutLogic_sc2235;
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_sc1235;
				pstCcm = &g_stAwbCcm_sc1235;
				pstAgcTab = &g_stIspAgcTable_sc1235;
				pstAeSnsDef = &gstAeSnsDef_sc1235;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc1235;
			#endif
				break;
#endif
#ifdef SENS_X1X_SC2145H
		case SENSOR_CHIP_SC2145H:
				cmos_init_sensor_exp_function_sc2145H(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc2145H(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				if(gu16SensorSwitch==2)
				{
					pstAwbCal = &gstAwbCal_sc2145H_A;
				}
				else
				{
					pstAwbCal = &gstAwbCal_sc2145H_B;
				}
				pstCcm = &g_stAwbCcm_sc2145H;
				pstAgcTab = &g_stIspAgcTable_sc2145H;
				pstAeSnsDef = &gstAeSnsDef_sc2235;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc2145H;
			#endif
				break;
#endif
#ifdef SENS_X20_IMX323
		case SENSOR_CHIP_IMX323:
				cmos_init_sensor_exp_function_imx323(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_imx323(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_imx323;
				pstCcm = &g_stAwbCcm_imx323;
				pstAgcTab = &g_stIspAgcTable_imx323;
				pstAeSnsDef = &gstAeSnsDef_imx323;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_imx323;
			#endif
				break;
#endif
#ifdef SENS_X20_IMX307
		case SENSOR_CHIP_IMX307:
				cmos_init_sensor_exp_function_imx307(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_imx307(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_imx307;
				pstCcm = &g_stAwbCcm_imx307;
				pstAgcTab = &g_stIspAgcTable_imx307;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_imx307;
			#endif
				break;
#endif
#ifdef SENS_X20_F37
		case SENSOR_CHIP_F37:
				cmos_init_sensor_exp_function_F37(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_F37(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_F37;
				pstCcm = &g_stAwbCcm_F37;
				pstAgcTab = &g_stIspAgcTable_F37;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_F37;
			#endif
				break;
#endif
#ifdef SENS_X20_SC2335
		case SENSOR_CHIP_SC2335:
				cmos_init_sensor_exp_function_sc2335(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc2335(&(stAeRegister.stSnsExp));
				cmos_init_awb_exp_function_sc2335(&(stAwbRegister.stSnsExp));
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc2335;
				break;
#endif
#ifdef SENS_X20_SP2305
		case SENSOR_CHIP_SP2305:
				cmos_init_sensor_exp_function_SP2305(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_SP2305(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_SP2305;
				pstCcm = &g_stAwbCcm_SP2305;
				pstAgcTab = &g_stIspAgcTable_SP2305;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_SP2305;
			#endif
				break;
#endif
#ifdef SENS_X20_MIS2003
		case SENSOR_CHIP_MIS2003:
				cmos_init_sensor_exp_function_MIS2003(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_MIS2003(&(stAeRegister.stSnsExp));
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_MIS2003;
				pstCcm = &g_stAwbCcm_MIS2003;
				pstAgcTab = &g_stIspAgcTable_MIS2003;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_MIS2003;
			#endif
				break;
#endif
#ifdef SENS_X20_SC2235
		case SENSOR_CHIP_SC2235:
				cmos_init_sensor_exp_function_sc2235(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc2235(&(stAeRegister.stSnsExp));
				pfn_gainLogic = gainLogic_sc2235;
				pfn_shutLogic = shutLogic_sc2235;
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_sc2235;
				pstCcm = &g_stAwbCcm_sc2235;
				pstAgcTab = &g_stIspAgcTable_sc2235;
				pstAeSnsDef = &gstAeSnsDef_sc2235;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc2235;
			#endif
				break;
#endif
#ifdef SENS_X20_SC2235P
		case SENSOR_CHIP_SC2235P:
				cmos_init_sensor_exp_function_sc2235P(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc2235P(&(stAeRegister.stSnsExp));
				pfn_gainLogic = gainLogic_sc2235P;
				pfn_shutLogic = shutLogic_sc2235;
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_sc2235P;
				pstCcm = &g_stAwbCcm_sc2235P;
				pstAgcTab = &g_stIspAgcTable_sc2235P;
				pstAeSnsDef = &gstAeSnsDef_sc2235p;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc2235P;
			#endif
				break;
#endif
#ifdef SENS_X20_SC307E
		case SENSOR_CHIP_SC307E:
				cmos_init_sensor_exp_function_sc307e(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc307e(&(stAeRegister.stSnsExp));
				cmos_init_awb_exp_function_sc307e(&(stAwbRegister.stSnsExp));
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc307e;
				break;
#endif
#ifdef SENS_X20_SC2235E
		case SENSOR_CHIP_SC2235E:
				cmos_init_sensor_exp_function_sc2235E(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc2235E(&(stAeRegister.stSnsExp));
				cmos_init_awb_exp_function_sc2235E(&(stAwbRegister.stSnsExp));
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc2235E;
				break;
#endif
#ifdef SENS_X20_SC2145
		case SENSOR_CHIP_SC2145:
				cmos_init_sensor_exp_function_sc2145(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc2145(&(stAeRegister.stSnsExp));
				cmos_init_awb_exp_function_sc2145(&(stAwbRegister.stSnsExp));
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc2145;
				break;
#endif

#ifdef SENS_X20_SC3035
		case SENSOR_CHIP_SC3035:
				cmos_init_sensor_exp_function_sc3035(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc3035(&(stAeRegister.stSnsExp));
				break;
#endif
#ifdef SENS_X30_SC4236
		case SENSOR_CHIP_SC4236:
				cmos_init_sensor_exp_function_sc4236(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc4236(&(stAeRegister.stSnsExp));
				pfn_gainLogic = gainLogic_sc4236;
				pfn_shutLogic = shutLogic_sc4236;
			#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
				pstAwbCal = &gstAwbCal_sc4236;
				pstCcm = &g_stAwbCcm_sc4236;
				pstAgcTab = &g_stIspAgcTable_sc4236;
				pstAeSnsDef = &gstAeSnsDef_sc4236;
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_sc4236;
			#endif
				break;
#endif
#ifdef SENS_X30_AUGE
		case SENSOR_CHIP_AUGE:
				cmos_init_sensor_exp_function_auge(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_auge(&(stAeRegister.stSnsExp));
				cmos_init_awb_exp_function_auge(&(stAwbRegister.stSnsExp));
				gu8MaxShutterOfst = 4;
				pfn_sensor_getlist = sensor_getlist_auge;
				break;
#endif
#ifdef SENS_X30_SC3035
		case SENSOR_CHIP_SC3035:
				cmos_init_sensor_exp_function_sc3035(&(stIspRegister.stSnsExp));
				cmos_init_ae_exp_function_sc3035(&(stAeRegister.stSnsExp));
				cmos_init_awb_exp_function_sc3035(&(stAwbRegister.stSnsExp));
				pfn_sensor_getlist = sensor_getlist_sc3035;
				gu8MaxShutterOfst = 4;
				break;
#endif

		default:	
#ifdef DEBUG_ON
				ERR("Not Support this sensor!\n");
#endif
				return XM_FAILURE;
				break;
	}

	if(ExtCfg_CheckFun(1, ROM_FUN_SNSINIT) == XM_SUCCESS)
		stIspRegister.stSnsExp.pfn_cmos_sensor_init = sensor_init_ext;
	else
		stIspRegister.stSnsExp.pfn_cmos_sensor_init = sensor_init;

	if(ExtCfg_CheckFun(1, ROM_FUN_FPS) == XM_SUCCESS)
		stAeRegister.stSnsExp.pfn_cmos_fps_set = cmos_fps_set_ex;
	if(ExtCfg_CheckFun(1, ROM_FUN_MF) == XM_SUCCESS)
		stIspRegister.stSnsExp.pfn_cmos_set_mirror_flip = cmos_set_mirror_flip_ex;
	if(ExtCfg_CheckFun(1, ROM_FUN_TABAGAIN) == XM_SUCCESS)
		stAeRegister.stSnsExp.pfn_cmos_again_calc_table = cmos_again_calc_table_ex;
	if(ExtCfg_CheckFun(1, ROM_FUN_TABDGAIN) == XM_SUCCESS)
		stAeRegister.stSnsExp.pfn_cmos_dgain_calc_table = cmos_dgain_calc_table_ex;
#if 0
	if(ExtCfg_CheckFun(1, ROM_FUN_UPDATEGAIN) == XM_SUCCESS)
		stAeRegister.stSnsExp.pfn_cmos_gains_update = cmos_gains_update_ex;
	if(ExtCfg_CheckFun(1, ROM_FUN_UPDATESHUT) == XM_SUCCESS)
		stAeRegister.stSnsExp.pfn_cmos_inttime_update = cmos_inttime_update_ex;
#endif
	if(ExtCfg_CheckFun(1, ROM_FUN_GAINLOGIC) == XM_SUCCESS)
		pfn_gainLogic = gainLogic_ex;
	if(ExtCfg_CheckFun(1, ROM_FUN_SHUTLOGIC) == XM_SUCCESS)
		pfn_shutLogic = shutLogic_ex;

	if(ExtCfg_GetExtCfgFlg() && (stIspRegister.stSnsExp.pfn_cmos_sensor_init))
	{
		stIspRegister.stSnsExp.pfn_cmos_sensor_init();
	}
	
	s32Ret = XM_MPI_ISP_SensorRegCallBack((XM_S32)gSensorChip, (const ISP_SENSOR_REGISTER_S * const)&stIspRegister);
	if (s32Ret)
	{
		return s32Ret;
	}
	stLib.s32Id = (XM_S32)gSensorChip;
	XM_MPI_ISP_Memncpy((XM_U8*)stLib.acLibName, (XM_U8*)XM_AE_LIB_NAME, sizeof(XM_AE_LIB_NAME));
	s32Ret = XM_MPI_AE_SensorRegCallBack(IspDev, &stLib, (XM_S32)gSensorChip, &stAeRegister);
	if (s32Ret)
	{
		return s32Ret;
	}

	stLib.s32Id = (XM_S32)gSensorChip;
	XM_MPI_ISP_Memncpy((XM_U8*)stLib.acLibName, (XM_U8*)XM_AWB_LIB_NAME, sizeof(XM_AWB_LIB_NAME));
	s32Ret = XM_MPI_AWB_SensorRegCallBack(&stLib, (XM_S32)gSensorChip, &stAwbRegister);
	if (s32Ret)
	{
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
        return s32Ret;
    }
    
    stLib.s32Id = (XM_S32)gSensorChip;
    s32Ret = XM_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, (XM_S32)gSensorChip);
    if (s32Ret)
    {
        return s32Ret;
    }
    return 0;
}



int sensor_mode_set(XM_U8 u8Mode)
{
	return 0;
}

int  sensor_mode_get()
{
	int s32RsFlg = 1<<RSLTION_BIT_BUTT;
	return s32RsFlg;
}


static int sensor_get_chip_in(I2C_DATA_S *pstI2CData)
{
	XM_U32 u32SnsChipFilt = SENSOR_CHIP_UNKNOW;
	XM_S32 s32Id,s32Tmp,s32version=0;
	if(pstI2CData == NULL)
		return -1;

START_TAB:
	switch(gSensorChip)
	{
		case SENSOR_CHIP_UNKNOW:
			//break;
			;
		case SENSOR_CHIP_H62:
		case SENSOR_CHIP_H65:
			if((u32SnsChipFilt != SENSOR_CHIP_H62) &&
				(u32SnsChipFilt != SENSOR_CHIP_H65))
			{
				// H42/H81/H62
				gSnsDevAddr = 0x30;
				gSnsRegAddrByte = 0x1;
				gSnsRegDataByte = 0x1;
				sensor_write_register(0x12, 0x80);
				sensor_write_register(0x12, 0x80);
				s32Tmp = sensor_read_register(0x0A);
				s32Tmp = sensor_read_register(0x0A);
				s32Id = sensor_read_register(0x0B); 
				s32Id = ((s32Tmp<<8) |s32Id);
				if(s32Id == 0xA062) 		// H62
				{
					gSensorChip = SENSOR_CHIP_H62;
					DEBUG("H62!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x0A65)	// H65
				{
					gSensorChip = SENSOR_CHIP_H65;
					DEBUG("H65!\n");
					goto RET_TAB;
				}
				// rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}
		case SENSOR_CHIP_IMX323:
			if(u32SnsChipFilt != SENSOR_CHIP_IMX323)
			{
				gSnsDevAddr = 0x1A;
				gSnsRegAddrByte = 0x2;
				gSnsRegDataByte = 0x1;
				s32Id = sensor_read_register(0x3013); 
				s32Id = sensor_read_register(0x3013); 
				s32Id = sensor_read_register(0x3013); 
				if(s32Id == 0x40)
				{
					gSensorChip = SENSOR_CHIP_IMX323;
					DEBUG("IMX323!\n");
					goto RET_TAB;
				}
				//rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}
		case SENSOR_CHIP_F37:
			if(u32SnsChipFilt != SENSOR_CHIP_F37)
			{
				gSnsDevAddr = 0x40;
				gSnsRegAddrByte = 0x1;
				gSnsRegDataByte = 0x1;
				s32Id = sensor_read_register(0x0A);
				s32Id = ((s32Id<<8)|(sensor_read_register(0x0B)));
				if(s32Id == 0x0F37)
				{
					gSensorChip = SENSOR_CHIP_F37;
					goto RET_TAB;
				}
				//rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}
		case SENSOR_CHIP_IMX291:
		case SENSOR_CHIP_IMX307:
			if((u32SnsChipFilt != SENSOR_CHIP_IMX291) &&
				(u32SnsChipFilt != SENSOR_CHIP_IMX307))
			{
				gSnsDevAddr = 0x1A;
				gSnsRegAddrByte = 0x2;
				gSnsRegDataByte = 0x1;
				sensor_write_register(0x3003, 0x01);
				SysDelay_ms(500);
				s32Id = sensor_read_register(0x3008);
				s32Id = ((s32Id<<8)|(sensor_read_register(0x301E)));
				if(s32Id == 0xA0B2)
				{
					// IMX291
					s32Id = sensor_read_register(0x31dc) & 0x7;
					if((s32Id == 0x00)||(s32Id == 0x01))//291和290做到了一起
					{
						gSensorChip = SENSOR_CHIP_IMX291;
						goto RET_TAB;
					}
					// IMX307
					s32Id = sensor_read_register(0x31dc) & 0x6;
					if(s32Id == 0x4)
					{
						gSensorChip = SENSOR_CHIP_IMX307;
						goto RET_TAB;
					}
				}

				//rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}
		//SmartSns
		case SENSOR_CHIP_SC1235:
		case SENSOR_CHIP_SC2235E:
		case SENSOR_CHIP_SC2235P:
		case SENSOR_CHIP_SC2235:
		case SENSOR_CHIP_SC307E:
		case SENSOR_CHIP_SC2145H:
		case SENSOR_CHIP_SC4236:
		case SENSOR_CHIP_SC2145:
		case SENSOR_CHIP_AUGE:
		case SENSOR_CHIP_SC2335:
		case SENSOR_CHIP_SC3035:
			if( (u32SnsChipFilt != SENSOR_CHIP_SC1235) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC2235E) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC2235P) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC2235) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC307E) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC2145H) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC4236) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC2145) &&
				(u32SnsChipFilt != SENSOR_CHIP_AUGE) &&
				(u32SnsChipFilt != SENSOR_CHIP_SC3035)&&
				(u32SnsChipFilt != SENSOR_CHIP_SC2335))
			{
				gSnsDevAddr = 0x30;
				gSnsRegAddrByte = 0x2;
				gSnsRegDataByte = 0x1;
				sensor_write_register(0x0103,0x01);  // reset all registers
				sensor_write_register(0x0103,0x01);  // reset all registers
				SysDelay_ms(100);
				s32Tmp = sensor_read_register(0x3107);
				s32Tmp = sensor_read_register(0x3107);
				s32Id = (s32Tmp<<8) | sensor_read_register(0x3108); 
				s32version = sensor_read_register(0x3109);
				if(s32Id == 0x1235)
				{
					gSensorChip = SENSOR_CHIP_SC1235;
					DEBUG("SC1235!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x2232)
				{
					if(s32version == 0x20)
					{
						gSensorChip = SENSOR_CHIP_SC2235E;
						DEBUG("SC2235E!\n");
					}
					else	// 0x01
					{
						gSensorChip = SENSOR_CHIP_SC2235P;
						DEBUG("SC2235P!\n");
					}
					goto RET_TAB;
				}
				else if(s32Id == 0x2235)
				{
					gSensorChip = SENSOR_CHIP_SC2235;
					DEBUG("SC2235!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x2238)
				{
					gSensorChip = SENSOR_CHIP_SC307E;
					DEBUG("SC307E!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x1245)
				{
					gSensorChip = SENSOR_CHIP_SC2145H;
					gu16SensorSwitch = sensor_read_register(0x3020); 
					DEBUG("SC2145H_%c!\n",(gu16SensorSwitch)==2?'A':'B');
					goto RET_TAB;
				}
				else if(s32Id == 0x3235)
				{
					gSensorChip = SENSOR_CHIP_SC4236;
					DEBUG("SC4236!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x2145)	
				{
					gSensorChip = SENSOR_CHIP_SC2145;
					DEBUG("SC2145!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0xCC05)	
				{
					gSensorChip = SENSOR_CHIP_AUGE;
					DEBUG("AUGE!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x3035)
				{
					gSensorChip = SENSOR_CHIP_SC3035;
					DEBUG("SC3035!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0xCB14)
				{
					gSensorChip = SENSOR_CHIP_SC2335;
					DEBUG("SC2335!\n");
					goto RET_TAB;
				}
				//rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}	
		case SENSOR_CHIP_SP140A:
		case SENSOR_CHIP_SP2305:
			if((u32SnsChipFilt != SENSOR_CHIP_SP140A)&&(u32SnsChipFilt != SENSOR_CHIP_SP2305))
			{
				gSnsDevAddr = 0x3C;
				gSnsRegAddrByte = 0x1;
				gSnsRegDataByte = 0x1;
				sensor_write_register(0xfd,0x00);
				sensor_write_register(0xfd,0x00);
				s32Tmp = sensor_read_register(0x02);
				s32Id = sensor_read_register(0x03); 
				s32Id = ((s32Tmp<<8) |s32Id);
				if(s32Id == 0x140A)
				{
					gSensorChip = SENSOR_CHIP_SP140A;
					DEBUG("SC140A!\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x2735)
				{
					gSensorChip = SENSOR_CHIP_SP2305;
					DEBUG("SP2305!\n");
					goto RET_TAB;
				}
				//rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}
		case SENSOR_CHIP_MIS2006:
		case SENSOR_CHIP_MIS2003:
			if((u32SnsChipFilt != SENSOR_CHIP_MIS2006)&&
				(u32SnsChipFilt != SENSOR_CHIP_MIS2003))
			{
				gSnsDevAddr = 0x30;
				gSnsRegAddrByte = 0x2;
				gSnsRegDataByte = 0x1;
				sensor_write_register(0x3006,0x01);  // reset all registers
				SysDelay_ms(100);
				s32Tmp = sensor_read_register(0x3000);
				s32Id = sensor_read_register(0x3001); 
				s32Id = ((s32Tmp<<8) |s32Id);
				s32version = sensor_read_register(0x3002);
				if(s32Id == 0x2003)
				{
					gSensorChip = SENSOR_CHIP_MIS2003;
					DEBUG("MIS2003 !\n");
					goto RET_TAB;
				}
				else if(s32Id == 0x2006)
				{
					if(s32version == 0x01)
					{
						gSensorChip = SENSOR_CHIP_MIS2006;
						DEBUG("MIS2006 VER 01!\n");
					}
					else	// 0x01
					{
						gSensorChip = SENSOR_CHIP_MIS2006;
						DEBUG("MIS2006 VER Other!\n");
					}
					goto RET_TAB;
				}
				//rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}
		case SENSOR_CHIP_OV9732:
			if(u32SnsChipFilt != SENSOR_CHIP_OV9732)
			{
				gSnsDevAddr = 0x36;
				gSnsRegAddrByte = 0x2;
				gSnsRegDataByte = 0x1;
				//sensor_write_register(0x3013,0x01);  // reset all registers
				//SysDelay_ms(100);
				s32Tmp = sensor_read_register(0x300A);
				s32Id = sensor_read_register(0x300B); 
				s32Id = ((s32Tmp<<8) |s32Id);
				if(s32Id == 0x9732)
				{
					gSensorChip = SENSOR_CHIP_OV9732;
					DEBUG("OV9732 !\n");
					goto RET_TAB;
				}
				//rescan
				if(gSensorChip != SENSOR_CHIP_UNKNOW)
				{
					u32SnsChipFilt = gSensorChip;
					gSensorChip = SENSOR_CHIP_UNKNOW;
					goto START_TAB;
				}
			}
		default:
			//rescan
			if(gSensorChip != SENSOR_CHIP_UNKNOW)
			{
				u32SnsChipFilt = gSensorChip;
				gSensorChip = SENSOR_CHIP_UNKNOW;
				goto START_TAB;
			}
			gSensorChip = SENSOR_CHIP_UNKNOW;
			break;
	}
	gSensorChip = SENSOR_CHIP_UNKNOW;
RET_TAB:
	pstI2CData->dev_addr = gSnsDevAddr;
	pstI2CData->addr_byte_num = gSnsRegAddrByte;
	pstI2CData->data_byte_num = gSnsRegDataByte;
	return gSensorChip;
}


int sensor_get_chip(I2C_DATA_S *pstI2CData)
{
	XM_U32 u32Addr;
	XM_U8 u8i;
	XM_U8 au8Data[4];
	// 1. 获取I2C 地址
	if(ExtCfg_BaseAddrGet(0, ROM_FUN_SNSI2C, &u32Addr) == XM_SUCCESS)
	{
		for(u8i=0;u8i<3;u8i++)
		{
			ExtCfg_Read_V2(1,u32Addr+u8i, &au8Data[u8i]);
		}
		gSnsDevAddr = au8Data[0];
		gSnsRegAddrByte = (au8Data[1]>>4)&0x0F;
		gSnsRegDataByte  = au8Data[1]&0x0F;
		gSensorChip = au8Data[2];
	}
	else
	{
		sensor_get_chip_in(pstI2CData);
	}

	
	pstI2CData->dev_addr = gSnsDevAddr;
	pstI2CData->addr_byte_num = gSnsRegAddrByte;
	pstI2CData->data_byte_num = gSnsRegDataByte;

#ifdef DEBUG_ON
	DEBUG("sensor_get_chip:\n");
	PrintHex(8, gSnsDevAddr); DBG(" ");
	PrintHex(8, gSnsRegAddrByte);DBG(" ");
	PrintHex(8, gSnsRegDataByte);DBG(" ");
	PrintHex(8, gSensorChip);
	DBG("\r\n");
#endif
	return gSensorChip;
}


void  sensor_set_chip(XM_S32 s32SnsChip)
{
	gSensorChip = s32SnsChip;
}

XM_U16 gu16ObTemp = 0;
int  sensor_get_temp(XM_S16 *ps16Temp)
{
    if(ps16Temp == NULL)
		return XM_FAILURE;
	*ps16Temp = gu16ObTemp;
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
	return XM_SUCCESS;
}

void sensor_set_fps(XM_U8 channel , XM_U8 frame)
{
	return;		
}




//u8Mode:  0:Normal  1:XM AHD  2: CVBS
void sensor_set_isp_para(XM_U8 u8Mode)
{
	return;
}


XM_S32 sensor_set_reslotionType(PROFILE enResType)
{
	return XM_SUCCESS;
}


/*************************************************************************************
输入参数: 	u8Mode(0:Stop  1:Restart)
*************************************************************************************/
void sensor_restart(XM_U8 u8Mode)
{
	XM_MPI_ISP_SetVsyncIntEn(0,0);
	switch(gSensorChip)
	{
#ifdef	SENS_X20_SC2235P
		case SENSOR_CHIP_SC2235P:
			if(u8Mode != 1)
			{
				sensor_write_register(0x0103,0x01);
				sensor_write_register(0x0100,0x00);
				DEBUG("SC2235P stop stream!\n");
			}
			break;
#endif
		default:
			break;
	}
	XM_MPI_ISP_SetVsyncIntEn(0,2);
}


