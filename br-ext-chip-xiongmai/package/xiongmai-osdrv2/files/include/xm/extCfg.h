#ifndef _EXTCFG_H_
#define _EXTCFG_H_

#define DEBUG_ON	(1)
//#define RES_960		(1)
#include "xm_type.h"
#include "xm_common.h"
#include "xm_comm_isp.h"

#define FUN_NUM  	(35)

#define BUFFER_START	(0x800)
#define ROM_BASE  		(0x32000)
#define MARK_ID			(0xA55A5AA5)
#define MARK_ID_TO_SYS		(0xA55A5AB5)

// 公用模块地址
#define ROM_COM_MARK		(0)
#define ROM_MD_VER			(4)
#define ROM_MD_SNSNUM		(7)
#define ROM_MD_SNSBASE		(8)

#define COM_FUN_REG_F		(0)
#define COM_FUN_REG_B		(1)
#define COM_FUN_MENU		(2)




//  模块内的地址偏移(基于每个sensor配置起始点)  gau32SnsBaseAddr
#define ROM_OFST_SNSID			(0)
#define ROM_OFST_FUNMASK		(1)
#define ROM_OFST_SNSI2C			(5)
#define ROM_OFST_ISPCUT			(9)

#define ROM_OFST_SNSCHECK		(21)


// Sensor I2C
#define I2C_OFST_RESOLUTION	(3)
// ISP CUT
#define ISPCUT_OFST_X_PAL		(0)
#define ISPCUT_OFST_Y_PAL		(2)
#define ISPCUT_OFST_WIDHT		(4)
#define ISPCUT_OFST_HEIGHT		(6)



#define SUPPORT_NUM		(8)


#define ROM_FUN_SNSI2C			(0)
#define ROM_FUN_ISPCUT			(1)
#define ROM_FUN_ISPINIT_FRONT		(2)
#define ROM_FUN_SNSINIT			(3)
#define ROM_FUN_FPS				(4)
#define ROM_FUN_MF				(5)
#define ROM_FUN_AEDEF			(6)
#define ROM_FUN_TABAGAIN		(7)
#define ROM_FUN_TABDGAIN		(8)
#define ROM_FUN_UPDATEGAIN		(9)
#define ROM_FUN_UPDATESHUT		(10)
#define ROM_FUN_ISPINIT			(11)
#define ROM_FUN_PN_ISPSET		(12)
#define ROM_FUN_AWB			(13)
#define ROM_FUN_CCM			(14)
#define ROM_FUN_SHARP			(15)
#define ROM_FUN_2DNR			(16)
#define ROM_FUN_3DNR			(17)
#define ROM_FUN_DYDPC			(18)
#define ROM_FUN_BLC				(19)
#define ROM_FUN_CON				(20)
#define ROM_FUN_SAT				(21)
#define ROM_FUN_CHROMA			(22)
#define ROM_FUN_AFC				(23)
#define ROM_FUN_NR3D			(24)
#define ROM_FUN_GAMMATAB		(25)
#define ROM_FUN_LUM				(26)
#define ROM_FUN_OTHER			(27)
#define ROM_FUN_GAINMODE		(28)
#define ROM_FUN_VENC			(29)
#define ROM_FUN_AEAWBINIT		(30)
#define ROM_FUN_AEAWBINFO		(31)
#define ROM_FUN_STABDEAL		(32)
#define ROM_FUN_GAINLOGIC		(33)
#define ROM_FUN_SHUTLOGIC		(34)


typedef enum {
// SmartSns
	SMS_LINE = 0x10,
	SMS_TAB  = 0x11,   // 标准查表
	SMS_TAB1 = 0x15,  // +带高温逻辑
	SMS_TAB2 = 0x19,  // +带增益逻辑
	SMS_TAB3 = 0x1D,  // +带高温、增益逻辑
 //晶像
	SOI_TAB = 0x21, // 

// Sony
	SONY_TAB = 0x31, // 

// Ov
	OV_LINE = 0x40, 	// 线性
	OV_TAB = 0x41,	// 查表
// Ps
	PS_LINE	= 0x50,

} GAIN_MODE_TYPE;


typedef struct _gamma_data_dn_s
{
	XM_U8 u8Num;			// <= 3  gamma条数
	XM_U8 au8Idx[3];			// =u8Num
	XM_U32 au32Exp[2];		// =u8Num-1
	XM_U32 u32ThrDlt;
}GAMMA_DATA_DN_S;


typedef struct _lum_data_dn_s
{
	XM_U8 u8Num;			// <= 4
	XM_U8 au8Lum[3];			// =u8Num
	XM_U32 au32Exp[3];		// =u8Num
}LUM_DATA_DN_S;



typedef struct _other_data_s
{
	XM_U8 u8Size;
	XM_U8 u8Gain50;
	XM_U8 u8Gain100;
	XM_U8 u8CscLum;
	XM_U8 u8CscSat;
	XM_U8 u8CscContrast;
	XM_U8 u8CscHue;
	XM_U8 u8FlipMirror;
	XM_U8 u8VencVstd;
	XM_U8 u8Rslt;		// 0xFF: 由内部决定
	XM_U8 u8Rsv[6];
}OHTER_DATA_S;



// 高温逻辑(和线性模式共用变量)
typedef struct _HT_LOGIC_DT
{
	XM_U8 u8En;
	XM_U8 u8MinGain;
	XM_U16 u16Reg[2];		// L  H
	XM_U32 u32LimitUp;
	XM_U32 u32LimitDn;
/************************************************
线性模式时
	XM_U8 u8AgainSft;		// Again cal中移位(高4位<；低4位>)
	XM_U8 u8AgainMin;		// 最小增益
	XM_U16 u16Rsv[2];		// 无意义
	XM_U32 u32Rsv1;		// 无意义
	XM_U32 u32Rsv2;		// 无意义

************************************************/
}HT_LOGIC_DT;


typedef struct _OV_AE_DATA
{
	XM_U32 u32Hold;
	XM_U32 u32Again;
	XM_U32 u32Dgain;
	XM_U32 u32Blc;
}AE_OV_TAB_DT;

typedef struct _AE_LINEAR_DT
{
	XM_U8 u8AgainSft;		// Again cal中移位(高4位<；低4位>)
	XM_U8 u8AgainMin;		// 最小增益
}AE_LINEAR_DT;


typedef union _GAIN_MODE_DT_U
{
	HT_LOGIC_DT stHtLgc;  		// SmartSns 高温逻辑
 	AE_LINEAR_DT stLinear;		// 线性增益
 	AE_OV_TAB_DT stOvTab;		// OV
}GAIN_MODE_DT;


typedef struct xm_AE_INIT_S
{
 	XM_U8 u8Speed;
	XM_U8 u8Tolerance;
 	XM_U8 u8Speed_Stab;
	XM_U8 u8Tolerance_Stab;
	XM_U32 u32UpdateCfg_Stab;
	XM_U8 u8AntiFlicker;
	XM_U8 u8AntiFlicker_Freq;
	XM_U8 u8BlackDelayFrame;
	XM_U8 u8WhiteDelayFrame;
	XM_U8 u8ExpMode;	// 0: Auto 1:Manual
	XM_U32 u32ExpManual;
	XM_U32 u32MinAGain;
	XM_U32 u32MaxAGain;
	XM_U32 u32MinDGain;
	XM_U32 u32MaxDGain;
	XM_U32 u32MinIspDGain;
	XM_U32 u32MaxIspDGain;
}AE_INIT_S;

typedef struct xm_AWB_INIT_S
{
 	XM_U8 u8HighColorTemp;
	XM_U8 u8LowColorTemp;
}AWB_INIT_S;


typedef struct xm_NR3D_INIT_S
{
	XM_U8 u8Size;
	XM_U8 u8FrameCnt;
	XM_U8 u8Ctrl;
	XM_U16 u16Width;
	XM_U16 u16Height;
	XM_U16 u16ChangePoint;
	XM_U32 u32PhyAddr;
	XM_U32 u32BaseAddr[8];
	XM_U32 u32YAddr;
	XM_U32 u32ErrAddr;
}NR3D_INIT_S;


typedef struct xm_STABDEAL_S
{
	XM_U8 u8Size;
	XM_U8 u8DealAfterStab;
	XM_U16 u16FmRunNum;
	XM_U16 u16StabFmID;
	XM_U8 u8IspRegNum;
	XM_U32 u32IspAddrData;
}STABDEAL_S;

XM_S32 ExtCfg_BaseAddrGet(XM_U8 u8Sns, XM_U32 u32Module, XM_U32 *pu32Addr);

XM_S32 ExtCft_Init();

//返回值:   0: 匹配上了		-1: 没有匹配上
XM_S32 ExtCfg_IsLock();

/************************************************************************
函数功能:	根据SensorID 获取外部配置文件该sensor索引
输入参数:	u8SnsID: 目标sensor ID
输出参数:	*pu8SnsIdx  在配置文件中索引
返回参数:	0:		存在该sensor
				-1: 		不存在该sensor
************************************************************************/
XM_S32 ExtCft_GetSnsIdx(XM_U8 u8SnsID,XM_U8 *pu8SnsIdx);


// 设置sensor 的索引 (确认匹配上Sensor)
XM_S32 ExtCfg_SetSnsIdx(XM_U8 u8SnsIdx, XM_U8 u8SnsAdWidth,XM_U8 u8SnsDtWidth);

// 获取SensorID
XM_S32 ExtCft_GetSnsId(XM_U8 *pu8SnsId);

// 获取配置文件中的Sensor个数
XM_S32 ExtCft_GetSnsNum(XM_U8 *pu8Num);

/*********************************************************
函数功能:	获取ISP裁剪参数
输入参数:	u8Fps:	帧率模式(25/30/50/60)
输出参数:	pstRect:
					ISP裁剪参数
返回参数:	-1:		不支持
				0:		支持
*********************************************************/
XM_S32 ExtCft_GetIspWndRect(XM_U8 u8Fps,RECT_S* pstRect);

//获取分辨率
XM_S32 ExtCft_GetSnsResolution(XM_U8 *pu8Res);

//u8Mode:    	0: Common Funciton
//			1: Sensor Function
//验证是否支持该功能(功能是否使用外部配置)
XM_S32 ExtCfg_CheckFun(XM_U8 u8Mode, XM_U32 u32Fun);

XM_S32 ExtCfg_PN_IspSet(XM_U8 u8Encode, XM_U8 u8VstdMode);

//u8DnMode:   	0: Color		1:BW
//u8XviEn:		0:Disable		1:Enable
XM_S32 ExtCfg_SetMode(XM_U8 u8XviEn, XM_U8 u8DnMode);

XM_S32 ExtCfg_IspInit();


/************************************************************************
函数功能:	获取Gamma 参数
输入参数:	*pu8Mode:		0:Color	1:Bw
输出参数:	*pu8Mode
					0: 死区    1:过渡
				*pstGamm:
					对应模式(Color/Bw)的参数
注:	   每个模式Gamma最大 3条
************************************************************************/
XM_S32 ExtCfg_Gamma_Get(XM_U8 *pu8Mode,GAMMA_DATA_DN_S*pstGamm);
/************************************************************************
函数功能:	获取GammaTable
输入参数:	u8GmId:   ID(第u8GmId - 0x40   条)       
输出参数:	*pu16Tab:
					Gamma表
注:
	u8GmId>= 0x40
************************************************************************/
XM_S32 ExtCfg_GammaTab_Get(XM_U8 u8GmId,XM_U16 *pu16Tab);


/************************************************************************
函数功能:	获取lum参数
输入参数:	无
输出参数:	无
返回参数: 	-1: 不支持
				其他: 亮度值			
************************************************************************/
XM_S32 ExtCfg_Lum_Get();


// 获取Other参数
XM_S32 ExtCfg_Other_Get(OHTER_DATA_S *pstData);


// 初始化ISP 寄存器
XM_S32 ExtCfg_IspRegInit(XM_U8 u8Mode);



//u8DnMode:   	0: Color		1:BW
//u8XviEn:		0:Disable		1:Enable
XM_S32 ExtCfg_SetMode(XM_U8 u8XviEn, XM_U8 u8DnMode);

//从Flash/E2PROM中读取
XM_S32 ExtCfg_Read_V2(XM_U8 u8Bytes, XM_U32 u32Add,XM_U8 *pu8Data);

XM_S32 ExtCfg_Write_V2(XM_U16 u16Bytes, XM_U32 u32Add,XM_U8 *pu8Data);


//写入Flash/E2PROM
XM_S32 ExtCfg_Write(XM_U8 u8Bytes, XM_U32 u32Addr,XM_U8 *pu8Data);



//函数功能: 从配置文件读取数据 写入Isp寄存器
//u32Addr:  数据起始地址(配置文件)
//u8Num: 寄存器个数
//u8Mode: 0:Read From E2		1:Read From RAM
XM_S32 ExtCft_WriteIsp(XM_U8 u8Mode, XM_U32 u32Addr, XM_U16 u16Num);

/************************************************************************
函数功能:	ExtCft_GainModeGet
输入参数:	u8Mode:
					0:  获取GainMode
					1:  获取GainLogic地址
输出参数:	*pu32Data:
					GainMode  			(u8Mode == 0)
					GainLogic BaseAddr  	(u8Mode == 1)
返回参数:	-1:  failed/ not exist
				0:    Ok/exit
************************************************************************/
XM_S32 ExtCft_GainModeGet(XM_U8 u8Mode, XM_U32 *pu32Data);



/***********************************************************************
函数名称:	ExtCft_GetVersion
函数功能:	获取外部配置版本信息
输入参数:	无
输出参数:	指向3个字节的buffer 地址
					年(1Byte)
					月(1Byte)
					日(1Byte)
返回参数:	0:	成功
				-1: 	出错
Note:Lycai
***********************************************************************/
XM_S32 ExtCft_GetVersion(XM_U8 *pau8Data);



/***********************************************************************
函数名称:	ExtCfg_VencSet
函数功能:	不同编码、视频支持进行外部配置参数
输入参数:	u8VencMode: 当前制式
					bit0: AHD_PAL
					bit1: AHD_NTSC
					bit2: CVI_PAL
					bit3: CVI_NTSC
					bit4: TVI_PAL
					bit5: TVI_NTSC
					bit6: CVBS_PAL
					bit7: CVBS_NTSC

输出参数:	无
返回参数:	0:	成功
				-1: 	出错
Note:Lycai
***********************************************************************/
XM_S32 ExtCfg_VencSet(XM_U8 u8VencMode);

XM_S32 ExtCfg_Read_RAM(XM_U8 u8Bytes, XM_U32 u32Add,XM_U8 *pu8Data);

XM_S32 ExtCfg_CCM_Set(ISP_COLORMATRIX_AUTO_S *pstCCM);

XM_S32 ExtCfg_IspDataInit(XM_U8 u8Venc, XM_U8 u8Std);

XM_S32 ExtCfg_Init(XM_U8 *pu8Buffer);

XM_S32 ExtCfg_Nr3DInit(NR3D_INIT_S *pstNr3D);

XM_S32 ExtCfg_StabDeal(STABDEAL_S *pstStabDeal);

XM_S32 ExtCfg_AeAwbInit(XM_U8 u8Mode, XM_U8 u8Vstd);

/***********************************************************************
函数名称:	ExtCfg_GetExtCfgFlg
函数功能:	判断是否需要配置
输入参数:	无
输出参数:	无
返回参数:	0:	不用配置(前面(ROM)已配置过)
				1:    需要配置(前面未配置过)
				2: 	需要配置(无配置表)
				-1: 	出错
Note:Lycai

	ROM 和ISP不能重复配置部分配置前判断
***********************************************************************/
XM_S32 ExtCfg_GetExtCfgFlg();


void PrintInt(unsigned char u8Num,int u32Data);
void PrintHex(unsigned char u8Num,int u32Data);

#endif
