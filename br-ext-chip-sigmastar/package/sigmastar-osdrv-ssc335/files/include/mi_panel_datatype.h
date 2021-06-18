/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_PANEL_DATATYPE_H_
#define _MI_PANEL_DATATYPE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"


/*-------------------------------------------------------------------------------------------------
 * Enum
 ------------------------------------------------------------------------------------------------*/

typedef enum
{
    E_MI_PNL_MIPI_DSI_LANE_NONE   = 0,
    E_MI_PNL_MIPI_DSI_LANE_1      = 1,
    E_MI_PNL_MIPI_DSI_LANE_2      = 2,
    E_MI_PNL_MIPI_DSI_LANE_3      = 3,
    E_MI_PNL_MIPI_DSI_LANE_4      = 4,
}MI_PANEL_MipiDsiLaneMode_e;

typedef enum
{
    E_MI_PNL_MIPI_DSI_RGB565         = 0,
    E_MI_PNL_MIPI_DSI_RGB666         = 1,
    E_MI_PNL_MIPI_DSI_LOOSELY_RGB666 = 2,
    E_MI_PNL_MIPI_DSI_RGB888         = 3,
}MI_PANEL_MipiDsiFormat_e;

typedef enum
{
    E_MI_PNL_MIPI_DSI_CMD_MODE = 0,
    E_MI_PNL_MIPI_DSI_SYNC_PULSE = 1,
    E_MI_PNL_MIPI_DSI_SYNC_EVENT = 2,
    E_MI_PNL_MIPI_DSI_BURST_MODE = 3,
}MI_PANEL_MipiDsiCtrlMode_e;

typedef enum
{
    E_MI_PNL_INTF_TTL,                              ///< TTL  type
    E_MI_PNL_INTF_LVDS,                             ///< LVDS type
    E_MI_PNL_INTF_RSDS,                             ///< RSDS type
    E_MI_PNL_INTF_MINILVDS,                         ///< TCON
    E_MI_PNL_INTF_ANALOG_MINILVDS,                  ///< Analog TCON
    E_MI_PNL_INTF_DIGITAL_MINILVDS,                 ///< Digital TCON
    E_MI_PNL_INTF_MFC,                              ///< Ursa (TTL output to Ursa)
    E_MI_PNL_INTF_DAC_I,                        ///< DAC output
    E_MI_PNL_INTF_DAC_P,                       ///< DAC output
    E_MI_PNL_INTF_PDPLVDS,                    ///< For PDP(Vsync use Manually MODE)
    E_MI_PNL_INTF_EXT,                              ///< EXT LPLL TYPE
    E_MI_PNL_INTF_MIPI_DSI,                         ///< Mipi DSI
    E_MI_PNL_INTF_BT656,
    E_MI_PNL_INTF_BT601,                            ///< BT601
    E_MI_PNL_INTF_BT1120,                           ///< BT1120
    E_MI_PNL_INTF_MCU_TYPE,                         ///< MCU Type
    E_MI_PNL_INTF_SRGB,                             ///< sRGB
    E_MI_PNL_INTF_TTL_SPI_IF,                       ///< TTL with SPI init interface
}MI_PANEL_IntfType_e;

typedef enum
{
    E_MI_PNL_ASPECT_RATIO_4_3    = 0,   ///< set aspect ratio to 4 : 3
    E_MI_PNL_ASPECT_RATIO_WIDE,       ///< set aspect ratio to 16 : 9
    E_MI_PNL_ASPECT_RATIO_OTHER,          ///< resvered for other aspect ratio other than 4:3/ 16:9
}MI_PANEL_AspectRatio_e;

typedef enum
{
    E_MI_PNL_TI_10BIT_MODE = 0,
    E_MI_PNL_TI_8BIT_MODE = 2,
    E_MI_PNL_TI_6BIT_MODE = 3,
}MI_PANEL_TiBitMode_e;

typedef enum
{
    E_MI_PNL_OUTPUT_10BIT_MODE   = 0, //default is 10bit, becasue 8bit panel can use 10bit config and 8bit config.
    E_MI_PNL_OUTPUT_6BIT_MODE    = 1, //but 10bit panel(like PDP panel) can only use 10bit config.
    E_MI_PNL_OUTPUT_8BIT_MODE    = 2, //and some PDA panel is 6bit.
    E_MI_PNL_OUTPUT_565BIT_MODE  = 3,
}MI_PANEL_OutputFormatBitMode_e;

typedef enum
{
    E_MI_PNL_CHG_DCLK   = 0,      ///<change output DClk to change Vfreq.
    E_MI_PNL_CHG_HTOTAL = 1,      ///<change H total to change Vfreq.
    E_MI_PNL_CHG_VTOTAL = 2,      ///<change V total to change Vfreq.
}MI_PANEL_OutputTimingMode_e;

typedef enum
{
    E_MI_PNL_CH_SWAP_0,
    E_MI_PNL_CH_SWAP_1,
    E_MI_PNL_CH_SWAP_2,
    E_MI_PNL_CH_SWAP_3,
    E_MI_PNL_CH_SWAP_4,
}MI_PANEL_ChannelSwapType_e;

typedef enum
{
    E_MI_PNL_MIPI_DSI_PACKET_TYPE_DCS = 0,
    E_MI_PNL_MIPI_DSI_PACKET_TYPE_GENERIC = 1,
}MI_PANEL_MipiDsiPacketType_e;
/*-------------------------------------------------------------------------------------------------
 * Structures
 ------------------------------------------------------------------------------------------------*/

typedef struct
{
    MI_BOOL bEn;
}MI_PANEL_PowerConfig_t;

typedef struct
{
    MI_BOOL bEn;
    MI_U16  u16Duty;
    MI_U16  u16Period;
}MI_PANEL_BackLightConfig_t;

typedef struct
{
    MI_BOOL bEn;
    MI_U16 u16Freq;// khz
    MI_U16 u16Ratio;// 1/1000
}MI_PANEL_SscConfig_t;

typedef struct
{
    MI_U8 u8HsTrail;
    MI_U8 u8HsPrpr;
    MI_U8 u8HsZero;
    MI_U8 u8ClkHsPrpr;
    MI_U8 u8ClkHsExit;
    MI_U8 u8ClkTrail;
    MI_U8 u8ClkZero;
    MI_U8 u8ClkHsPost;
    MI_U8 u8DaHsExit;
    MI_U8 u8ContDet;

    MI_U8 u8Lpx;
    MI_U8 u8TaGet;
    MI_U8 u8TaSure;
    MI_U8 u8TaGo;

    MI_U16 u16Hactive;
    MI_U16 u16Hpw;
    MI_U16 u16Hbp;
    MI_U16 u16Hfp;

    MI_U16 u16Vactive;
    MI_U16 u16Vpw;
    MI_U16 u16Vbp;
    MI_U16 u16Vfp;

    MI_U16 u16Bllp;
    MI_U16 u16Fps;

    MI_PANEL_MipiDsiLaneMode_e enLaneNum;
    MI_PANEL_MipiDsiFormat_e enformat;
    MI_PANEL_MipiDsiCtrlMode_e enCtrl;

    MI_U8  *pu8CmdBuf;
    MI_U32 u32CmdBufSize;

    MI_U8 u8SyncCalibrate;
    MI_U16 u16VirHsyncSt;
    MI_U16 u16VirHsyncEnd;
    MI_U16 u16VsyncRef;
    MI_U16 u16DataClkSkew;

    MI_U8  u8PolCh0;  // channel 0 polarity, 0:HW default, 1:positive, 2:negative
    MI_U8  u8PolCh1;  // channel 1 polarity, 0:HW default, 1:positive, 2:negative
    MI_U8  u8PolCh2;  // channel 2 polarity, 0:HW default, 1:positive, 2:negative
    MI_U8  u8PolCh3;  // channel 3 polarity, 0:HW default, 1:positive, 2:negative
    MI_U8  u8PolCh4;  // channel 4 polarity, 0:HW default, 1:positive, 2:negative

    MI_PANEL_MipiDsiPacketType_e ePacketType;
}MI_PANEL_MipiDsiConfig_t;

typedef struct
{
    MI_U16 u16HSyncWidth;
    MI_U16 u16HSyncBackPorch;
    MI_U16 u16HSyncFrontPorch;

    MI_U16 u16VSyncWidth;
    MI_U16 u16VSyncBackPorch;
    MI_U16 u16VSyncFrontPorch;

    MI_U16 u16HStart;
    MI_U16 u16VStart;

    MI_U16 u16Height;
    MI_U16 u16Width;

    MI_U16 u16HTotal;
    MI_U16 u16VTotal;

    MI_U16 u16Dclk;
}MI_PANEL_TimingConfig_t;

typedef struct
{
    MI_U16 u16DrvCurrent;
}MI_PANEL_DrvCurrentConfig_t;

typedef struct
{
    MI_BOOL bEn;
    MI_U16  u16R;
    MI_U16  u16G;
    MI_U16  u16B;
}MI_PANEL_TestPatternConfig_t;

typedef struct
{
    const char *pPanelName;          ///<  PanelName
    MI_U8             u8Dither;         ///<  PANEL_DITHER, keep the setting
    MI_PANEL_IntfType_e eIntfType;        ///<  PANEL_LINK

    ///////////////////////////////////////////////
    // Board related setting
    ///////////////////////////////////////////////
    MI_U8 u8DualPort;          ///<  VOP_21[8], MOD_4A[1],    PANEL_DUAL_PORT, refer to u8DoubleClk
    MI_U8 u8SwapPort;          ///<  MOD_4A[0],    PANEL_SWAP_PORT, refer to "LVDS output app note" A/B channel swap
    MI_U8 u8SwapOdd_ML;          ///<  PANEL_SWAP_ODD_ML
    MI_U8 u8SwapEven_ML;          ///<  PANEL_SWAP_EVEN_ML
    MI_U8 u8SwapOdd_RB;          ///<  PANEL_SWAP_ODD_RB
    MI_U8 u8SwapEven_RB;          ///<  PANEL_SWAP_EVEN_RB

    MI_U8 u8SwapLVDS_POL;         ///<  MOD_40[5], PANEL_SWAP_LVDS_POL, for differential P/N swap
    MI_U8 u8SwapLVDS_CH;          ///<  MOD_40[6], PANEL_SWAP_LVDS_CH, for pair swap
    MI_U8 u8PDP10BIT;          ///<  MOD_40[3], PANEL_PDP_10BIT ,for pair swap
    MI_U8 u8LVDS_TI_MODE;          ///<  MOD_40[2], PANEL_LVDS_TI_MODE, refer to "LVDS output app note"

    ///////////////////////////////////////////////
    // For TTL Only
    ///////////////////////////////////////////////
    MI_U8 u8DCLKDelay;                ///<  PANEL_DCLK_DELAY
    MI_U8 u8InvDCLK;              ///<  MOD_4A[4],     PANEL_INV_DCLK
    MI_U8 u8InvDE;              ///<  MOD_4A[2],       PANEL_INV_DE
    MI_U8 u8InvHSync;              ///<  MOD_4A[12],    PANEL_INV_HSYNC
    MI_U8 u8InvVSync;              ///<  MOD_4A[3],    PANEL_INV_VSYNC

    ///////////////////////////////////////////////
    // Output driving current setting
    ///////////////////////////////////////////////
    // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
    MI_U8 u8DCKLCurrent;              ///<  define PANEL_DCLK_CURRENT
    MI_U8 u8DECurrent;                ///<  define PANEL_DE_CURRENT
    MI_U8 u8ODDDataCurrent;           ///<  define PANEL_ODD_DATA_CURRENT
    MI_U8 u8EvenDataCurrent;          ///<  define PANEL_EVEN_DATA_CURRENT

    ///////////////////////////////////////////////
    // panel on/off timing
    ///////////////////////////////////////////////
    MI_U16 u16OnTiming1;         ///<  time between panel & data while turn on power
    MI_U16 u16OnTiming2;        ///<  time between data & back light while turn on power
    MI_U16 u16OffTiming1;       ///<  time between back light & data while turn off power
    MI_U16 u16OffTiming2;      ///<  time between data & panel while turn off power

    ///////////////////////////////////////////////
    // panel timing spec.
    ///////////////////////////////////////////////
    // sync related
    MI_U16 u16HSyncWidth;         ///<  VOP_01[7:0], PANEL_HSYNC_WIDTH
    MI_U16 u16HSyncBackPorch;     ///<  PANEL_HSYNC_BACK_PORCH, no register setting, provide value for query only
    MI_U16 u16VSyncWidth;         ///<  define PANEL_VSYNC_WIDTH
    MI_U16 u16VSyncBackPorch;     ///<  define PANEL_VSYNC_BACK_PORCH

    // DE related
    MI_U16 u16HStart;             ///<  VOP_04[11:0], PANEL_HSTART, DE H Start (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
    MI_U16 u16VStart;            ///<  VOP_06[11:0], PANEL_VSTART, DE V Start
    MI_U16 u16Width;            ///< PANEL_WIDTH, DE width (VOP_05[11:0] = HEnd = HStart + Width - 1)
    MI_U16 u16Height;          ///< PANEL_HEIGHT, DE height (VOP_07[11:0], = Vend = VStart + Height - 1)

    // DClk related
    MI_U16 u16MaxHTotal;            ///<  PANEL_MAX_HTOTAL. Reserved for future using.
    MI_U16 u16HTotal;                 ///<  VOP_0C[11:0], PANEL_HTOTAL
    MI_U16 u16MinHTotal;          ///<  PANEL_MIN_HTOTAL. Reserved for future using.
    MI_U16 u16MaxVTotal;           ///<  PANEL_MAX_VTOTAL. Reserved for future using.
    MI_U16 u16VTotal;               ///<  VOP_0D[11:0], PANEL_VTOTAL
    MI_U16 u16MinVTotal;            ///<  PANEL_MIN_VTOTAL. Reserved for future using.
    MI_U16 u16MaxDCLK;            ///<  PANEL_MAX_DCLK. Reserved for future using.
    MI_U16 u16DCLK;              ///<  LPLL_0F[23:0], PANEL_DCLK
    MI_U16 u16MinDCLK;                  ///<  PANEL_MIN_DCLK. Reserved for future using.

    ///<  spread spectrum
    MI_U16 u16SpreadSpectrumStep;            ///<  move to board define, no use now.
    MI_U16 u16SpreadSpectrumSpan;            ///<  move to board define, no use now.

    MI_U8 u8PwmPeriodL;                    ///<  Initial Dimming Value
    MI_U8 u8PwmPeriodH;                     ///<  Max Dimming Value
    MI_U8 u8PwmDuty;                     ///<  Min Dimming Value

    MI_U8 u8DeinterMode;          ///<  define PANEL_DEINTER_MODE,  no use now
    MI_PANEL_AspectRatio_e ePanelAspectRatio; ///<  Panel Aspect Ratio, provide information to upper layer application for aspect ratio setting.

    MI_U16 u16LVDSTxSwapValue;
    MI_PANEL_TiBitMode_e eTiBitMode;                         ///< MOD_4B[1:0], refer to "LVDS output app note"
    MI_PANEL_OutputFormatBitMode_e eOutputFormatBitMode;

    MI_U8 u8SwapOdd_RG;          ///<  define PANEL_SWAP_ODD_RG
    MI_U8 u8SwapEven_RG;          ///<  define PANEL_SWAP_EVEN_RG
    MI_U8 u8SwapOdd_GB;          ///<  define PANEL_SWAP_ODD_GB
    MI_U8 u8SwapEven_GB;          ///<  define PANEL_SWAP_EVEN_GB

    /**
     *  Others
     */
    MI_U8 u8DoubleClk;              ///<  LPLL_03[7], define Double Clock ,LVDS dual mode
    MI_U32 u32MaxSET;             ///<  define PANEL_MAX_SET
    MI_U32 u32MinSET;             ///<  define PANEL_MIN_SET
    MI_PANEL_OutputTimingMode_e eOutTimingMode;   ///<Define which panel output timing change mode is used to change VFreq for same panel
    MI_U8 u8NoiseDith;        ///<  PAFRC mixed with noise dither disable

    MI_PANEL_ChannelSwapType_e eCh0;
    MI_PANEL_ChannelSwapType_e eCh1;
    MI_PANEL_ChannelSwapType_e eCh2;
    MI_PANEL_ChannelSwapType_e eCh3;
    MI_PANEL_ChannelSwapType_e eCh4;
}MI_PANEL_ParamConfig_t;

typedef struct MI_PANEL_InitParam_s
{
    MI_PANEL_IntfType_e eIntfType;
} MI_PANEL_InitParam_t;

#ifdef __cplusplus
}
#endif
#endif //_MI_PANEL_DATATYPE_H_
