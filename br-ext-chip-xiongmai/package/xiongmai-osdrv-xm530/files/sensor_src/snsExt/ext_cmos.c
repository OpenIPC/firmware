#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"

#include "xm_print.h"
#include "Camera.h"


#include "xm530_tmp.h"
#include "xm510_isp.h"


#include "extCfg.h"
extern XM_S32 SysActiveSizeGet(XM_U16 *pu16ImgW,XM_U16 *pu16ImgH);
extern XM_U8 	gSnsDevAddr;
extern XM_U32  gSnsRegAddrByte;
extern XM_U32  gSnsRegDataByte;
extern XM_U16 gu16FullLines;


extern XM_U8 gu8MaxShutterOfst;
extern XM_U8 gu8Fps;
extern XM_U8 gAppEncoderinfo;	// 0: AHD  1:CVI  2:TVI
extern XM_U32 gu32AGainNow;
extern XM_U32 gu32AGainMin;
extern XM_U32 gu32DGainNow;
extern XM_U32 gu32DGainMin;
extern XM_U8 gu8BlcDpcFlg;

void sensor_input_2(XM_U32 u32Addr,XM_U16 u16Num)
{
	XM_U32 u32Tmp;
	XM_U16 u16i;
	const XM_U16 *pu16Data = NULL;
	pu16Data=(const XM_U16 *)u32Addr;
	for(u16i = 0; u16i<u16Num; u16i ++)
	{
		if( (u16Num>2) && (u16i < u16Num-2) &&
			(*(pu16Data+u16i*2+0)== 0xF5) && (*(pu16Data+u16i*2+1) == 0xA5) &&
			(*(pu16Data+(u16i+1)*2+0)== 0x5A) && (*(pu16Data+(u16i+1)*2+1) == 0x5F))
		{
			u16i += 2;
			u32Tmp = *(pu16Data+u16i*2+0) & 0xFF;
			u32Tmp = (u32Tmp << 8) | (*(pu16Data+u16i*2+1)&0xFF);
			SysDelay_ms(u32Tmp);

			//DEBUG("Sleepms:\n");
			//PrintInt(8, u32Tmp); DBG("\r\n");
		}
		else
		{
			sensor_write_register((XM_U32)*(pu16Data+u16i*2+0),(XM_U32)*(pu16Data+u16i*2+1));

			//PrintHex(8, (XM_U32)*(pu16Data+u16i*2+0)); DBG("  ");
			//PrintHex(8, (XM_U32)*(pu16Data+u16i*2+1)); DBG("\r\n");
		}
	}
	return ;
}

void sensor_input_1(XM_U32 u32Addr,XM_U16 u16Num)
{
	XM_U32 u32Tmp;
	XM_U16 u16i;
	const XM_U8 *pu8Data = NULL;
	pu8Data=(const XM_U8 *)u32Addr;
	for(u16i = 0; u16i<u16Num; u16i ++)
	{
		if( (u16Num>2) && (u16i < u16Num-2) &&
			(*(pu8Data+u16i*2+0)== 0xF5) && (*(pu8Data+u16i*2+1) == 0xA5) &&
			(*(pu8Data+(u16i+1)*2+0)== 0x5A) && (*(pu8Data+(u16i+1)*2+1) == 0x5F))
		{
			u16i += 2;
			u32Tmp = *(pu8Data+u16i*2+0) & 0xFF;
			u32Tmp = (u32Tmp << 8) | (*(pu8Data+u16i*2+1)&0xFF);
			SysDelay_ms(u32Tmp);

			//DEBUG("Sleepms:\n");
			//PrintInt(8, u32Tmp); DBG("\r\n");
		}
		else
		{
			sensor_write_register((XM_U32)*(pu8Data+u16i*2+0),(XM_U32)*(pu8Data+u16i*2+1));

			//PrintHex(8, (XM_U32)*(pu8Data+u16i*2+0)); DBG("  ");
			//PrintHex(8, (XM_U32)*(pu8Data+u16i*2+1)); DBG("\r\n");
		}
	}
	return ;
}



//函数功能: 从配置文件读取数据 写入Sensor 
//u32Addr:  数据起始地址
//u16Num: 寄存器个数
//u8Mode: 0:Read From E2		1:Read From RAM
int sensor_write_ex(XM_U8 u8Mode, XM_U32 u32Addr, XM_U16 u16Num)
{
	XM_U16 au16RegAddr[2];
	XM_U16 au16RegData[2];
	XM_U16 u16i,u16Len;
	XM_S32 (*pFun_cfgRead)(XM_U8 u8Bytes, XM_U32 u32Add,XM_U8 *pu8Data) = NULL;
	if(u16Num > 0)
	{
	#ifdef DEBUG_ON
		DEBUG("sensor_write_ex:\n");
	#endif
		pFun_cfgRead = ExtCfg_Read_V2;
		u16Len = gSnsRegAddrByte+gSnsRegDataByte;
		for(u16i = 0; u16i<u16Num; u16i ++)
		{
			au16RegAddr[0] = au16RegData[0] = 0;
			au16RegAddr[1] = au16RegData[1] = 0;
			(*pFun_cfgRead)(gSnsRegAddrByte, u32Addr, (XM_U8*)&au16RegAddr[0]);					// Addr
			(*pFun_cfgRead)(gSnsRegDataByte, u32Addr+gSnsRegAddrByte, (XM_U8*)&au16RegData[0]);	// Data
			(*pFun_cfgRead)(gSnsRegAddrByte, u32Addr+u16Len, (XM_U8*)&au16RegAddr[1]);					// Addr
			(*pFun_cfgRead)(gSnsRegDataByte, u32Addr+gSnsRegAddrByte+u16Len, (XM_U8*)&au16RegData[1]);	// Data
			if( (u16Num>2) && (u16i < u16Num-2) &&
				(au16RegAddr[0]== 0xF5) && (au16RegData[0] == 0xA5) &&
				(au16RegAddr[1]== 0x5A) && (au16RegData[1] == 0x5F))
			{
				u16i += 2;
				au16RegAddr[0] = au16RegData[0] = 0;
				u32Addr += (XM_U32)u16Len*2;
				(*pFun_cfgRead)(gSnsRegAddrByte, u32Addr, (XM_U8*)&au16RegAddr[0]);
				(*pFun_cfgRead)(gSnsRegDataByte, u32Addr+gSnsRegAddrByte, (XM_U8*)&au16RegData[0]);
				
				au16RegAddr[0] = au16RegAddr[0] & 0xFF;
				au16RegAddr[0] = (au16RegAddr[0] << 8) | (au16RegData[0]&0xFF);
				SysDelay_ms((XM_U32)au16RegAddr[0]);
			#ifdef DEBUG_ON
				DBG("Sleepms:");
				PrintInt(8, au16RegAddr[0]);
				ENTER();
			#endif
			}
			else
			{
				sensor_write_register((XM_U32)au16RegAddr[0],(XM_U32)au16RegData[0]);
			#ifdef DEBUG_ON
				PrintHex(8, au16RegAddr[0]); DBG("  ");
				PrintHex(8, au16RegData[0]);
				ENTER();
			#endif
			}
			u32Addr += u16Len;
		}
	#ifdef DEBUG_ON
		DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	#endif
	}
	return XM_SUCCESS;
}






XM_S32 cmos_set_mirror_flip_ex(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U8 u8SnsRegNum,u8IspRegNum,u8Tmp,u8Mode = 0;
	XM_U8 u8i;
	XM_U16 u16Ofst = 0;
	XM_U32 u32Addr,u32Tmp;
	XM_U16 u16HOfst,u16VOfst;
	u8SnsRegNum = 0;
	u8IspRegNum = 0;
	if(ExtCfg_BaseAddrGet(0xFF, ROM_FUN_MF, &u32Addr) != XM_SUCCESS)
		return XM_FAILURE;
	ExtCfg_Read_V2(1,u32Addr, &u8Mode);
	ExtCfg_Read_V2(1,u32Addr+1, &u8SnsRegNum);
	ExtCfg_Read_V2(1,u32Addr+2, &u8IspRegNum);

	u32Tmp = (XM_U32)u8SnsRegNum*(gSnsRegAddrByte+gSnsRegDataByte)*4;
	u32Tmp += (XM_U32)u8IspRegNum*8*4;
	u32Tmp += (((u8Mode>>6)&0x01)+((u8Mode>>7)&0x01))*4;			// cut

	u32Addr += 3;
	u8Tmp = 0;
	if(gu8Fps==30)
	{
		u8Tmp = 1;
	}
	u8Tmp += gAppEncoderinfo*2;
	for(u8i = 0; u8i < 6; u8i ++)
	{
		if(u8i == u8Tmp)
		{
			break;
		}
		if((1<<u8i) & u8Mode)
			u32Addr += u32Tmp;
	}
	u16Ofst = 2*u8Flip+u8Mirror;
	u8Mode = u8Mode>>6;
	// IspCut
	u16HOfst = 0xFFFF;
	u16VOfst = 0xFFFF;
	if(u8Mode > 0)	// Need Cut
	{
		ExtCfg_Read_V2(1,u32Addr+u16Ofst, &u8Tmp);
		switch(u8Mode)
		{
			// H
			case 0x01:	u16HOfst = u8Tmp;
						u32Addr += 4;
						break;
			// V
			case 0x02:	u16VOfst = u8Tmp;
						u32Addr += 4;
						break;
			// H & V
			case 0x03:	u16HOfst = u8Tmp;
						ExtCfg_Read_V2(1,u32Addr+u16Ofst+4, &u8Tmp);
						u16VOfst = u8Tmp;
						u32Addr += 8;
						break;
			default:		
#ifdef DEBUG_ON
						ERR("cmos_set_mirror_flip_ex failed!\n");
#endif
						break;
		}
	}
	u32Tmp = (XM_U32)u16Ofst*u8SnsRegNum*(gSnsRegAddrByte+gSnsRegDataByte);
#ifdef DEBUG_ON	
	DEBUG("cmos_set_mirror_flip_ex:\n");
	PrintInt(8, (XM_S32)gu8Fps);
	PrintInt(8, (XM_S32)gAppEncoderinfo); ENTER();
	PrintInt(8, (XM_S32)u8Mirror);
	PrintInt(8, (XM_S32)u8Flip); ENTER();
	PrintInt(8, (XM_S32)u16HOfst);
	PrintInt(8, (XM_S32)u16VOfst); ENTER();
#endif
	sensor_write_ex(1, u32Addr+u32Tmp, (XM_U16)u8SnsRegNum);

	VI_WinSet(0, u8Mirror, u8Flip, u16HOfst, u16VOfst);

#if 0		
	if(u16HOfst != 0xFFFF)
	{
		DEBUG("Cut H:\n");
		PrintInt(8, (XM_U32)u16HOfst);	ENTER();

	}
	if(u16VOfst != 0xFFFF)
	{
		DEBUG("Cut V:\n");
		PrintInt(8, (XM_U32)u16VOfst);	ENTER();
	}
#endif
	u32Addr += (XM_U32)4*u8SnsRegNum*(gSnsRegAddrByte+gSnsRegDataByte);
	u32Tmp = (XM_U32)u16Ofst*u8IspRegNum*8;
	ExtCft_WriteIsp(1, u32Addr+u32Tmp, u8IspRegNum);
#ifdef DEBUG_ON
	DEBUG("cmos_set_mirror_flip_ex over ~~~~\n");
#endif

	return XM_SUCCESS;
}


XM_S32 cmos_get_ae_default_ext(AE_SENSOR_DEFAULT_V2_S *pstAeSnsDft)
{
	XM_U32 u32Addr,u32Tmp;
	XM_U8 u8i;
	if(ExtCfg_BaseAddrGet(0, ROM_FUN_AEDEF, &u32Addr) != XM_SUCCESS)
		return XM_FAILURE;

	// MinShutter
	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u32MinIntTime = u32Tmp;
	// MinAgain
	u32Addr += 1;
	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u32MinAgain = u32Tmp<<4;
	// MaxAgain
	u32Addr += 1;
	u32Tmp = 0;
	ExtCfg_Read_V2(2,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u32MaxAgain = u32Tmp<<4;
	// MinDgain
	u32Addr += 2;
	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u32MinDgain = u32Tmp<<4;
	//MaxDgain
	u32Addr += 1;
	u32Tmp = 0;
	ExtCfg_Read_V2(2,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u32MaxDgain = u32Tmp<<4;
	//MaxIspDgain
	u32Addr += 2;
	u32Tmp = 0;
	ExtCfg_Read_V2(2,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u32MaxISPDgain = u32Tmp<<4;
	//UpdateCfg
	u32Addr += 2;
	u32Tmp = 0;
	ExtCfg_Read_V2(4,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u32UpdateCfg = u32Tmp;
	//MaxShutterOfst
	u32Addr += 4;
	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr, (XM_U8*)&u32Tmp);
	gu8MaxShutterOfst = u32Tmp;

	//InitExp
	u32Addr += 1;
	for(u8i=0; u8i<4; u8i ++)
	{
		u32Tmp = 0;
		ExtCfg_Read_V2(4,u32Addr, (XM_U8*)&u32Tmp);
		pstAeSnsDft->u32InitExp[u8i] = u32Tmp;
		u32Addr += 4;
	}
	u32Tmp = 0;
	ExtCfg_Read_V2(2,u32Addr, (XM_U8*)&u32Tmp);
	pstAeSnsDft->u16AlgMode = u32Tmp;
	u32Addr += 2;

#if(defined SOC_SYSTEM) || (defined SOC_ALIOS)
	ExtCfg_BaseAddrGet(0, ROM_FUN_AEAWBINFO, &u32Addr);
	u32Tmp = 0;
	ExtCfg_Read_V2(4,u32Addr+1, (XM_U8*)&u32Tmp);
	for(u8i=0; u8i<4; u8i ++)
	{
		pstAeSnsDft->u32InitExp[u8i] = u32Tmp;
	}
#endif

	
#ifdef DEBUG_ON
	DEBUG("cmos_get_ae_default_ext:\n");
	PrintInt(8, pstAeSnsDft->u32MinIntTime); ENTER();
	PrintInt(8, pstAeSnsDft->u32MaxIntTime); ENTER();
	PrintInt(8, pstAeSnsDft->u32MinAgain); ENTER();
	PrintInt(8, pstAeSnsDft->u32MaxAgain); ENTER();
	PrintInt(8, pstAeSnsDft->u32MinDgain); ENTER();
	PrintInt(8, pstAeSnsDft->u32MaxDgain); ENTER();
	PrintInt(8, pstAeSnsDft->u32MaxISPDgain); ENTER();
	PrintHex(8, pstAeSnsDft->u32UpdateCfg); ENTER();
	PrintInt(8, gu8MaxShutterOfst); ENTER();
	PrintInt(8, pstAeSnsDft->u32InitExp[0]); ENTER();
	PrintInt(8, pstAeSnsDft->u32InitExp[1]); ENTER();
	PrintInt(8, pstAeSnsDft->u32InitExp[2]); ENTER();
	PrintInt(8, pstAeSnsDft->u32InitExp[3]); ENTER();
	PrintHex(8, (XM_U32)pstAeSnsDft->u16AlgMode); ENTER();
#endif
	return 0;
}



//u8GainMode:  0:Again		1:Dgain
static void cmos_gain_calc_table(	XM_U8 u8GainMode,
										XM_U32 u32InTimes,
										AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
#ifdef DEBUG_ON
	static XM_U8 su8Flg = 0;
#endif
	XM_U32 u32Addr,u32Tmp,u32AddrTmp,u32Min;
	XM_U8 u8i,u8Num,u8Mode,u8Width,u8BitOfstR,u8BitOfstL;
	XM_U32 tmp = 0;
	if(XM_NULL == pstAeSnsGainInfo)
	{
#ifdef DEBUG_ON
		ERR("null pointer when get ae sensor gain info  value!\n");
#endif
		return;
	}
	if(u8GainMode)
		ExtCfg_BaseAddrGet(0, ROM_FUN_TABDGAIN, &u32Addr);
	else
		ExtCfg_BaseAddrGet(0, ROM_FUN_TABAGAIN, &u32Addr);
	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr, (XM_U8*)&u32Tmp);
	u8Mode = u32Tmp;
	if(u8Mode==2)		// Table
	{
		u32Tmp = 0;
		ExtCfg_Read_V2(1,u32Addr+1, (XM_U8*)&u32Tmp);
		u8Num = u32Tmp;
		u32Tmp = 0;
		ExtCfg_Read_V2(1,u32Addr+2, (XM_U8*)&u32Tmp);
		u8BitOfstR = u32Tmp&0x7;
		u8BitOfstL = (u32Tmp>>3)&0x07;
		u8Width = ((u32Tmp>>6)&0x03)+1;
		u32Addr += 3;
		
		u32AddrTmp = u32Addr+(XM_U32)u8Width*0;
		u32Tmp = 0;
		ExtCfg_Read_V2(u8Width,u32AddrTmp, (XM_U8*)&u32Tmp);
		u32Min = (u32Tmp<<u8BitOfstL)>>u8BitOfstR;

		u32AddrTmp = u32Addr+(XM_U32)u8Width*(u8Num-1);
		u32Tmp = 0;
		ExtCfg_Read_V2(u8Width,u32AddrTmp, (XM_U8*)&u32Tmp);
		u32Tmp = (u32Tmp<<u8BitOfstL)>>u8BitOfstR;
#ifdef DEBUG_ON
		if(!(su8Flg & (1<<u8GainMode)))
		{
			su8Flg |= (1<<u8GainMode);
			DEBUG("cmos_gain_calc_table:\n");
			PrintInt(8, (int)u8GainMode); ENTER();
			PrintInt(8, (int)u8Num);ENTER();
			for(u8i = 0x0; u8i < u8Num; u8i++)       
			{
				u32AddrTmp = u32Addr+(XM_U32)u8Width*u8i;
				u32Tmp = 0;
				ExtCfg_Read_V2(u8Width,u32AddrTmp, (XM_U8*)&u32Tmp);
				u32Tmp = (u32Tmp<<u8BitOfstL)>>u8BitOfstR;
				PrintInt(8, (int)u32Tmp); DBG("  ");
			}
			DBG("Over~~~"); ENTER();
		}
#endif
		if (u32InTimes >= u32Tmp) 
		{
			tmp = (u8Num-1);
			pstAeSnsGainInfo->u32SnsTimes = u32Tmp;
		}
		else
		{
			for(u8i = 0x0; u8i <= u8Num-1; u8i++)        
			{           
				u32AddrTmp = u32Addr+(XM_U32)u8Width*u8i;
				u32Tmp = 0;
				ExtCfg_Read_V2(u8Width,u32AddrTmp, (XM_U8*)&u32Tmp);
				u32Tmp = (u32Tmp<<u8BitOfstL)>>u8BitOfstR;
				if(u32InTimes <u32Tmp)            
				{                
					break;            
				}            
				tmp = u8i;
				pstAeSnsGainInfo->u32SnsTimes = u32Tmp;
			}    
		}
		pstAeSnsGainInfo->u32GainDb = tmp;

		if(u8GainMode)
		{
			gu32DGainNow = pstAeSnsGainInfo->u32SnsTimes;
			gu32DGainMin = u32Min;
		}
		else
		{
			gu32AGainNow = pstAeSnsGainInfo->u32SnsTimes;
			gu32AGainMin = u32Min;
		}
	}
}


XM_VOID cmos_again_calc_table_ex(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	cmos_gain_calc_table(0, u32InTimes, pstAeSnsGainInfo);
	return;
}


XM_VOID cmos_dgain_calc_table_ex(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	cmos_gain_calc_table(1, u32InTimes, pstAeSnsGainInfo);
	return;
}

#if 0
// 思特威方式1 :200w查表
static XM_VOID cmos_gains_update_sms1(XM_U32 u32Again, XM_U32 u32Dgain)
{
	static XM_U32 su32AGain = 0xFFFFFFF;
	static XM_U32 su32DGain = 0xFFFFFFF;
	unsigned int tmp[2];
	if((su32AGain == u32Again) && (su32DGain==u32Dgain))
		return ;
	su32AGain = u32Again;
	su32DGain = u32Dgain; 

	// 2. Update
	tmp[0] = (u32Again&0x0F) | 0x10;
	tmp[1] = u32Again/16;
	switch(tmp[1])
	{
		case 0:	tmp[1] = 0x3;
				break;
		case 1:	tmp[1] = 0x7;
				break;
		case 2:	tmp[1] = 0xF;
				break;
		case 3:
		default:	tmp[1] = 0x1F;
				break;
	}
	switch(u32Dgain)
	{
		case 1:	u32Dgain = 0;
				break;
		case 2:	u32Dgain = 1;
				break;
		case 4:	u32Dgain = 3;
				break;
		default:	
#ifdef DEBUG_ON
				ERR("Err!\n");
#endif
				break;
	}
	tmp[1] |= (u32Dgain<<5);
	sensor_write_register(0x3e08, tmp[1]);
	sensor_write_register(0x3e09, tmp[0]);
	return;
}

XM_VOID cmos_gains_update_ex(XM_U32 u32Again, XM_U32 u32Dgain)
{
	XM_U32 u32Addr;
	XM_U8 u8Mode;
	ExtCfg_BaseAddrGet(0, ROM_FUN_UPDATEGAIN, &u32Addr);
	ExtCfg_Read_V2(1,u32Addr, &u8Mode);
	if(u8Mode == 0x11)
	{
		cmos_gains_update_sms1(u32Again, u32Dgain);
	}
	return;
}



XM_VOID cmos_inttime_update_ex(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTimeLst = 0xFFFFFFF;
	XM_U32 u32Addr,u32Tmp,u32Mask,u32Data;
	XM_U8 u8Num,u8i;
	if(su32IntTimeLst == u32IntTime)	return;
	su32IntTimeLst = u32IntTime;
	
	ExtCfg_BaseAddrGet(0, ROM_FUN_UPDATESHUT, &u32Addr);
	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr, (XM_U8*)&u32Tmp);
	if(u32Tmp == 1)	u32IntTime = gu16FullLines - u32IntTime;
	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr+1, (XM_U8*)&u32Tmp);
	u32IntTime = u32IntTime<<((u32Tmp>>4)&0x0F);
	u32IntTime = u32IntTime>>(u32Tmp&0x0F);

	u32Tmp = 0;
	ExtCfg_Read_V2(1,u32Addr+2, (XM_U8*)&u32Tmp);
	u8Num = u32Tmp;
	u32Addr = u32Addr+3;
	u32Mask = (1<<8*gSnsRegDataByte)-1;
	//DEBUG("Time:\n");
	for(u8i=0; u8i<u8Num; u8i ++)
	{
		u32Tmp = 0;
		ExtCfg_Read_V2(gSnsRegAddrByte,u32Addr, (XM_U8*)&u32Tmp);
		u32Data = u32IntTime&u32Mask;
		sensor_write_register(u32Tmp, u32Data);

		u32Addr += gSnsRegAddrByte;
		u32IntTime = u32IntTime>>(8*gSnsRegDataByte);

		//PrintHex(8, (XM_U64)u32Tmp);
		//PrintHex(8, (XM_U64)u32Data); DBG("\r\n");
	}
	//DBG("\r\n");
	return;
}
#endif


/* the function of sensor set fps */
XM_VOID cmos_fps_set_ex(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U8 u8MaxOfst = gu8MaxShutterOfst;
	XM_U32 u32Addr;
	XM_U8 u8SnsRegNum,u8IspRegNum;
	XM_U16 u16MaxLine;
	XM_U8 u8Mode;
	XM_U32 u32Len;
	if(pstAeSnsDft == NULL)
	{
#ifdef DEBUG_ON
		ERR("cmos_fps_set_ex err!~~~~\n");
#endif
		return;
	}
#ifdef DEBUG_ON
	DEBUG("cmos_fps_set_ex start:\n");
#endif

	if(ExtCfg_BaseAddrGet(0, ROM_FUN_FPS, &u32Addr)==XM_SUCCESS)
	{
		u8SnsRegNum = 0;
		u8IspRegNum = 0;
		u16MaxLine =  1125;
	
		ExtCfg_Read_V2(1,u32Addr, &u8Mode);
		ExtCfg_Read_V2(1,u32Addr+1, &u8SnsRegNum);
		ExtCfg_Read_V2(1,u32Addr+2, &u8IspRegNum);
		u32Len = (gSnsRegDataByte+gSnsRegAddrByte)*u8SnsRegNum+8*u8IspRegNum+3;
		u32Addr += 3;

		switch(gAppEncoderinfo)
		{
			// CVI
			case 1:	if(u8Mode&0x02)
						u32Addr+= u32Len*2;
					break;
			// TVI
			case 2:	if(u8Mode&0x04)
					{
						if(u8Mode&0x02) 	u32Addr+= u32Len*4;
						else				u32Addr+= u32Len*2;
					}
					break;
			// AHD
			case 0:
			default:	break;
		}
		if(u8Fps == 30)	// NTSC
		{
			u32Addr += u32Len; 
		}
		ExtCfg_Read_V2(1,u32Addr, &u8Fps);
		u32Addr += 1;
		ExtCfg_Read_V2(2,u32Addr, (XM_U8*)&u16MaxLine);
		u32Addr += 2;
		sensor_write_ex(1, u32Addr, (XM_U16)u8SnsRegNum);

		// Write Isp Reg
		u32Addr += (XM_U32)(gSnsRegAddrByte+gSnsRegDataByte)*u8SnsRegNum;
		ExtCft_WriteIsp(1, u32Addr, u8IspRegNum);

		gu16FullLines = u16MaxLine;
		pstAeSnsDft->u32MaxIntTime = (XM_U32)gu16FullLines-u8MaxOfst;
		pstAeSnsDft->u32LinesPer500ms = (XM_U32)gu16FullLines*u8Fps/2;
		pstAeSnsDft->u32FullLinesStd = (XM_U32)gu16FullLines;
	}
	gu8Fps = u8Fps;

#ifdef DEBUG_ON
	PrintInt(8, (XM_U32)gu8Fps);	ENTER();
	PrintInt(8, (XM_U32)gu16FullLines);	ENTER();
	DEBUG("cmos_fps_set_ex over~~~~~~\n");
#endif

	return;
}

void gainLogic_ex(XM_U32 u32AllGain)
{
	static XM_U8 su8Idx = 0xFF;//[bit0~bit3]:Vstd   [bit4~bit7]:Agc

	XM_U32 u32Addr,u32AddrTmp;
	XM_U8 u8RegNum,u8GainNum,u8i,u8Gain,u8Mode,u8FpsModeNow,u8j;
	XM_U32 u32Reg,u32Val;
	ExtCfg_BaseAddrGet(0, ROM_FUN_GAINLOGIC, &u32Addr);
	ExtCfg_Read_V2(1,u32Addr+2, &u8RegNum);
	ExtCfg_Read_V2(1,u32Addr+3, &u8GainNum);
	ExtCfg_Read_V2(1,u32Addr+4, &u8Mode);
	gu8BlcDpcFlg = u8Mode & 0xF0;
	u8Mode = u8Mode & 0x0F;
	u32Val = gSnsRegDataByte*u8RegNum*(u8GainNum+1);
	//RegList
	u32Addr += 5;
	//gainList
	u32AddrTmp = u32Addr+(XM_U32)u8RegNum*gSnsRegAddrByte;
	for(u8i=0;u8i<u8GainNum;u8i ++)
	{
		ExtCfg_Read_V2(1,u32AddrTmp+u8i, &u8Gain);
		if(u32AllGain<u8Gain)
			break;
	}
	u32AddrTmp += u8GainNum;
	//RegDataList
	switch(gu8Fps)
	{
		case 30:	u8FpsModeNow = 1;
				break;
		case 50:	u8FpsModeNow = 2;
				break;
		case 60:	u8FpsModeNow = 3;
				break;
		case 25:	
		default:	u8FpsModeNow = 0;
				break;
	}
	if(((su8Idx&0x0F) != u8FpsModeNow) || (((su8Idx>>4)&0x0F) != u8i))
	{
		su8Idx = ((u8i&0x0F)<<4)|(u8FpsModeNow&0x0F);

		for(u8j=0; u8j < 4; u8j ++)
		{
			if(u8j == u8FpsModeNow)
				break;
			if((1<<u8j) & u8Mode)
			{
				u32AddrTmp += u32Val;
			}
		}
		u32AddrTmp += (XM_U32)u8RegNum*gSnsRegDataByte*u8i;
#ifdef DEBUG_ON
		DBG("gainLogic:\r\n");
		PrintInt(8, u32AllGain);
		DBG("\r\n");
#endif
		for(u8i=0;u8i<u8RegNum;u8i ++)
		{
			u32Reg = 0;
			ExtCfg_Read_V2(gSnsRegAddrByte,u32Addr+gSnsRegAddrByte*u8i, (XM_U8*)&u32Reg);
			u32Val = 0;
			ExtCfg_Read_V2(gSnsRegDataByte,u32AddrTmp+gSnsRegDataByte*u8i, (XM_U8*)&u32Val);
			sensor_write_register(u32Reg, u32Val);
	
	#ifdef DEBUG_ON
			PrintHex(8, u32Reg);
			PrintHex(8, u32Val);
			DBG("\r\n");
	#endif
		}
#ifdef DEBUG_ON
		DBG("\r\n");
#endif	
	}
}

void shutLogic_ex(XM_U32 u32IntTime)
{
	static XM_U32 su32IntTime = 0;

	XM_U32 u32Addr,u32AddrTmp;
	XM_U8 u8RegNum,u8RegNum2,u8i;
	XM_U32 u32Reg,u32Val;
	XM_U16 u16ThrL,u16ThrH;
	if(su32IntTime == u32IntTime)	return;
	su32IntTime = u32IntTime;
	
	ExtCfg_BaseAddrGet(0, ROM_FUN_SHUTLOGIC, &u32Addr);
	ExtCfg_Read_V2(1,u32Addr+2, &u8RegNum);
	ExtCfg_Read_V2(1,u32Addr+3, &u8RegNum2);
	ExtCfg_Read_V2(2,u32Addr+4, (XM_U8 *)&u16ThrL);
	ExtCfg_Read_V2(2,u32Addr+6, (XM_U8 *)&u16ThrH);

#ifdef DEBUG_ON
	DBG("shutLogic:\r\n");
	PrintInt(8, u32IntTime);
	DBG("\r\n");
#endif

	// logic1
	u32Addr += 8;
	u32AddrTmp = 0;
	if(u32IntTime<u16ThrL)
	{
		u32AddrTmp = u32Addr+(XM_U32)u8RegNum*gSnsRegAddrByte;
	}
	else if(u32IntTime>u16ThrH)
	{
		u32AddrTmp = u32Addr+(XM_U32)u8RegNum*gSnsRegAddrByte+(XM_U32)u8RegNum*gSnsRegDataByte;
	}
	if(u32AddrTmp != 0)
	{
		for(u8i=0;u8i<u8RegNum;u8i ++)
		{
			u32Reg = 0;
			ExtCfg_Read_V2(gSnsRegAddrByte,u32Addr+gSnsRegAddrByte*u8i, (XM_U8*)&u32Reg);
			u32Val = 0;
			ExtCfg_Read_V2(gSnsRegDataByte,u32AddrTmp+gSnsRegDataByte*u8i, (XM_U8*)&u32Val);
			sensor_write_register(u32Reg, u32Val);
	
	#ifdef DEBUG_ON
			PrintHex(8, u32Reg);
			PrintHex(8, u32Val);
			DBG("\r\n");
	#endif
		}
	}
	// logic2
	if(u8RegNum2 != 0)
	{
		u32Addr += (XM_U32)u8RegNum*gSnsRegAddrByte+(XM_U32)u8RegNum*gSnsRegDataByte*2;
		u32AddrTmp = u32Addr+(XM_U32)u8RegNum2*gSnsRegAddrByte;
		for(u8i=0;u8i<u8RegNum2;u8i ++)
		{
			u32Reg = 0;
			ExtCfg_Read_V2(gSnsRegAddrByte,u32Addr+gSnsRegAddrByte*u8i, (XM_U8*)&u32Reg);
			u32Val = 0;
			ExtCfg_Read_V2(gSnsRegDataByte,u32AddrTmp+gSnsRegDataByte*u8i, (XM_U8*)&u32Val);
			sensor_write_register(u32Reg, u32Val);
	
	#ifdef DEBUG_ON
			PrintHex(8, u32Reg);
			PrintHex(8, u32Val);
			DBG("\r\n");
	#endif
		}
	}
}

