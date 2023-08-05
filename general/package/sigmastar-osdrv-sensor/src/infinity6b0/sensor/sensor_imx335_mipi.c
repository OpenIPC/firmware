/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <drv_sensor_common.h>
#include <sensor_i2c_api.h>
#include <drv_sensor.h>

#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(IMX335_HDR);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

//#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
//#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE            CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL   CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
#define ENABLE            1
#define DISABLE           0
#undef SENSOR_DBG
#define SENSOR_DBG        0

#define DEBUG_INFO        0

#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) LOGD(args)
//#define SENSOR_DMSG(args...) LOGE(args)
//#define SENSOR_DMSG(args...) printf(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
///////////////////////////////////////////////////////////////
//          @@@                                              //
//         @ @@      ==  S t a r t * H e r e  ==             //
//           @@      ==  S t a r t * H e r e  ==             //
//           @@      ==  S t a r t * H e r e  ==             //
//          @@@@                                             //
//                                                           //
//      Start Step 1 --  show preview on LCM                 //
//                                                           //
//  Fill these #define value and table with correct settings //
//      camera can work and show preview on LCM              //
//                                                           //
///////////////////////////////////////////////////////////////

////////////////////////////////////
// Sensor-If Info                 //
////////////////////////////////////
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM        (lane_num)          //IMX335 Linear mode supports MIPI 2/4 Lane
#define SENSOR_MIPI_LANE_NUM_DOL    (4) //(hdr_lane_num)//IMX335 DOL mode supports MIPI 4 Lane
//#define SENSOR_MIPI_HDR_MODE        (0) //0: Non-HDR mode. 1:Sony DOL mode

#define SENSOR_ISP_TYPE             ISP_EXT             //ISP_EXT, ISP_SOC (Non-used)
//#define SENSOR_DATAFMT             CUS_DATAFMT_BAYER    //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE           CUS_SENIF_BUS_MIPI  //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE      PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR_DOL PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC             CUS_DATAPRECISION_10
#define SENSOR_DATAPREC_DOL         CUS_DATAPRECISION_10
#define SENSOR_DATAMODE             CUS_SEN_10TO12_9098  //CFG
#define SENSOR_BAYERID              CUS_BAYER_RG         //0h: CUS_BAYER_RG, 1h: CUS_BAYER_GR, 2h: CUS_BAYER_BG, 3h: CUS_BAYER_GB
#define SENSOR_BAYERID_HDR_DOL      CUS_BAYER_RG
#define SENSOR_RGBIRID              CUS_RGBIR_NONE
#define SENSOR_ORIT                 CUS_ORIT_M0F0        //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER              CUS_SEN_YCODR_YC     //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define long_packet_type_enable     0x00 //UD1~UD8 (user define)

////////////////////////////////////
// MCLK Info                      //
////////////////////////////////////
#define Preview_MCLK_SPEED          CUS_CMU_CLK_27MHZ    //CUS_CMU_CLK_24MHZ //CUS_CMU_CLK_37P125MHZ//CUS_CMU_CLK_27MHZ
#define Preview_MCLK_SPEED_HDR_DOL  CUS_CMU_CLK_27MHZ

////////////////////////////////////
// I2C Info                       //
////////////////////////////////////
#define SENSOR_I2C_ADDR              0x34                //I2C slave address
#define SENSOR_I2C_SPEED             300000              //200000 //300000 //240000                  //I2C speed, 60000~320000
//#define SENSOR_I2C_CHANNEL           1                 //I2C Channel
//#define SENSOR_I2C_PAD_MODE          2                 //Pad/Mode Number
#define SENSOR_I2C_LEGACY            I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT               I2C_FMT_A16D8       //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

////////////////////////////////////
// Sensor Signal                  //
////////////////////////////////////
#define SENSOR_PWDN_POL              CUS_CLK_POL_NEG     // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL               CUS_CLK_POL_NEG     // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG
                                                         // VSYNC/HSYNC POL can be found in data sheet timing diagram,
                                                         // Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.
#define SENSOR_VSYNC_POL             CUS_CLK_POL_NEG     // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL             CUS_CLK_POL_NEG     // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL              CUS_CLK_POL_NEG     // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

////////////////////////////////////
// Sensor ID                      //
////////////////////////////////////
//define SENSOR_ID

#undef SENSOR_NAME
#define SENSOR_NAME     IMX335

////////////////////////////////////
// Image Info                     //
////////////////////////////////////
static struct {     // LINEAR
    // Modify it based on number of support resolution
    enum {LINEAR_RES_1 = 0, LINEAR_RES_2, LINEAR_RES_END}mode;
    // Sensor Output Image info
    struct _senout{
        s32 width, height, min_fps, max_fps;
    }senout;
    // VIF Get Image Info
    struct _sensif{
        s32 crop_start_X, crop_start_y, preview_w, preview_h;
    }senif;
    // Show resolution string
    struct _senstr{
        const char* strResDesc;
    }senstr;
}imx335_mipi_linear[] = {
    {LINEAR_RES_1, {2592, 1944, 3, 25}, {0, 0, 2592, 1944}, {"2592x1944@25fps"}},
    {LINEAR_RES_2, {2592, 1944, 3, 30}, {0, 0, 2592, 1944}, {"2592x1944@30fps"}}, // Modify it
};

static struct {     // HDR
    // Modify it based on number of support resolution
    enum {HDR_RES_1 = 0, HDR_RES_2, HDR_RES_3, HDR_RES_END}mode;
    // Sensor Output Image info
    struct _hsenout{
        s32 width, height, min_fps, max_fps;
    }senout;
    // VIF Get Image Info
    struct _hsensif{
        s32 crop_start_X, crop_start_y, preview_w, preview_h;
    }senif;
    // Show resolution string
    struct _hsenstr{
        const char* strResDesc;
    }senstr;
}imx335_mipi_hdr[] = {
    {HDR_RES_1, {2592, 1944, 3, 25}, {4, 0, 2592, 1944}, {"2592x1944@25fps_HDR"}}, // Modify it
    {HDR_RES_2, {2592, 1944, 3, 20}, {4, 0, 2592, 1944}, {"2592x1944@20fps_HDR"}}, // Modify it
    {HDR_RES_3, {2592, 1944, 3, 30}, {4, 0, 2592, 1944}, {"2592x1944@30fps_HDR"}}, // Modify it
};

#define IMX335_HDR_BRL                              2228

u32 Preview_line_period;
u32 vts_30fps;
u32 Preview_MAX_FPS;
u32 Preview_line_period_HDR_DOL;
u32 vts_30fps_HDR_DOL;

//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
//#define long_packet_type_enable 0x00 //UD1~UD8 (user define)
////////////////////////////////////
// AE Info                        //
////////////////////////////////////
#define SENSOR_MAX_GAIN                             (1412 * 1024)        // max sensor again, a-gain * conversion-gain*d-gain
#define SENSOR_MIN_GAIN                             (1 * 1024)
#define SENSOR_GAIN_DELAY_FRAME_COUNT               (2)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT            (2)
#define SENSOR_GAIN_DELAY_FRAME_COUNT_HDR_DOL       (1)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT_HDR_DOL    (2)

////////////////////////////////////
// Mirror-Flip Info               //
////////////////////////////////////
#define REG_MIRROR                                  0x304E
#define REG_FLIP                                    0x304F
#define SENSOR_NOR                                  0x0
#define MIRROR_EN                                   0x1 << (0)
#define FLIP_EN                                     0x1 << (0)

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

//static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
//static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
static int pCus_SetAEUSecsHDR_DOL_SEF1(ms_cus_sensor *handle, u32 us);
static int pCus_SetAEUSecsHDR_DOL_LEF(ms_cus_sensor *handle, u32 us);

typedef struct {
    struct {
        u16 pre_div0;
        u16 div124;
        u16 div_cnt7b;
        u16 sdiv0;
        u16 mipi_div0;
        u16 r_divp;
        u16 sdiv1;
        u16 r_seld5;
        u16 r_sclk_dac;
        u16 sys_sel;
        u16 pdac_sel;
        u16 adac_sel;
        u16 pre_div_sp;
        u16 r_div_sp;
        u16 div_cnt5b;
        u16 sdiv_sp;
        u16 div12_sp;
        u16 mipi_lane_sel;
        u16 div_dac;
    } clk_tree;
    struct {
        bool bVideoMode;
        u16 res_idx;
        //        bool binning;
        //        bool scaling;
        CUS_CAMSENSOR_ORIT  orit;
    } res;
    struct {
        float sclk;
        u32 hts;
        u32 vts;
        u32 ho;
        u32 xinc;
        u32 line_freq;
        u32 us_per_line;
        u32 final_us;
        u32 final_gain;
        u32 back_pv_us;
        u32 fps;
        u32 preview_fps;
        u32 expo_lines;
        u32 expo_lef_us;
        u32 expo_sef_us;
    } expo;
    u32 max_rhs1;
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orien_dirty;
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[2];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tExpo_shr_dol1_reg[3];
    I2C_ARRAY tExpo_rhs1_reg[3];
    I2C_ARRAY tGain_hdr_dol_lef_reg[2];
    I2C_ARRAY tGain_hdr_dol_sef_reg[2];
} imx335_params;
// set sensor ID address and data,

const static I2C_ARRAY Sensor_id_table[] =
{
    {0x3003, 0x00},      // {address of ID, ID },
    {0x3033, 0x00}
};

const static I2C_ARRAY Sensor_init_table_2lane_5m25fps[] =
{
    /*"IMX335LQN All-pixel scan CSI-2_2lane 1188Mbps AD:10bit Output:10bit 27MHz Master Mode 25fps"*/
    {0x3002,0x01},  //Master mode stop
    {0xffff,0x14},  //delay
    {0x3000,0x01},  //standby
    {0xffff,0x14},  //delay
    {0x300C,0x42},  //BCWAIT_TIME
    {0x300D,0x2E},  //CPWAIT_TIME
    {0x3030,0x56},  //VMAX //101Dh: 4125 //1518h: 5400
    {0x3031,0x13},  //VMAX
    {0x3032,0x00},  //VMAX
    {0x3034,0x58},  //HMAX //2D0h: 720  //226h: 550
    {0x3035,0x02},  //HMAX
    {0x3050,0x00},  //ADBIT
    {0x314C,0xB0},  //INCKSEL1
    {0x315A,0x02},  //INCKSEL2
    {0x3168,0x8F},  //INCKSEL3
    {0x316A,0x7E},  //INCKSEL4
    {0x319D,0x00},  //MDBIT 0:10Bit  1:12Bit
    {0x31A0,0x2A},  //XH&VSOUTSEL
    {0x31A1,0x00},  //XVS/XHS pin setting
    {0x31A4,0x00},  //PULSE1_UP
    {0x31A5,0x00},  //PULSE1_UP
    {0x31A6,0x00},  //PULSE1_UP
    {0x31A8,0x00},  //PULSE1EN/PULSE1_POL
    {0x31AC,0x00},  //PULSE1_DN
    {0x31AD,0x00},  //PULSE1_DN
    {0x31AE,0x00},  //PULSE1_DN
    {0x31D4,0x00},  //XVSLNG
    {0x31D5,0x00},  //XHSLNG
    {0x31D7,0x00},
    {0x31E4,0x01},
    {0x31F3,0x01},
    {0x3288,0x21},
    {0x328A,0x02},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341C,0xFF},  //ADBIT1 10Bit:0x01FF  12Bit:0x0047
    {0x341D,0x01},  //ADBIT1 10Bit:0x01FF  12Bit:0x0047
    {0x3648,0x01},
    {0x364A,0x04},
    {0x364C,0x04},
    {0x3678,0x01},
    {0x367C,0x31},
    {0x367E,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371C,0x3D},
    {0x371D,0x3F},
    {0x372C,0x00},
    {0x372D,0x00},
    {0x372E,0x46},
    {0x372F,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xFE},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375D,0x00},
    {0x375E,0x00},
    {0x375F,0x11},
    {0x3760,0x01},
    {0x3768,0x1B},
    {0x3769,0x1B},
    {0x376A,0x1B},
    {0x376B,0x1B},
    {0x376C,0x1A},
    {0x376D,0x17},
    {0x376E,0x0F},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377A,0x89},
    {0x377B,0x00},
    {0x377C,0x08},
    {0x377D,0x01},
    {0x377E,0x23},
    {0x377F,0x02},
    {0x3780,0xD9},
    {0x3781,0x03},
    {0x3782,0xF5},
    {0x3783,0x06},
    {0x3784,0xA5},
    {0x3788,0x0F},
    {0x378A,0xD9},
    {0x378B,0x03},
    {0x378C,0xEB},
    {0x378D,0x05},
    {0x378E,0x87},
    {0x378F,0x06},
    {0x3790,0xF5},
    {0x3792,0x43},
    {0x3794,0x7A},
    {0x3796,0xA1},
    {0x3A01,0x01},  //LANEMODE 0x03:4Lane  0x01:2Lane
    {0x3000,0x00},
    {0x3002,0x00},
};

const static I2C_ARRAY Sensor_init_table_2lane_5m30fps[] =
{
    /*"IMX335LQN All-pixel scan CSI-2_2lane 1188Mbps AD:10bit Output:10bit 27MHz Master Mode 30fps"*/
    {0x3002,0x01},  //Master mode stop
    {0xffff,0x14},  //delay
    {0x3000,0x01},  //standby
    {0xffff,0x14},  //delay
    {0x300C,0x42},  //BCWAIT_TIME
    {0x300D,0x2E},  //CPWAIT_TIME
    {0x3030,0x1D},  //VMAX //1194 :4500 //101D: 4125
    {0x3031,0x10},  //VMAX
    {0x3032,0x00},  //VMAX
    {0x3034,0x58},  //HMAX //2D0h: 720  //226h: 550
    {0x3035,0x02},  //HMAX
    {0x3050,0x00},  //ADBIT
    {0x314C,0xB0},  //INCKSEL1
    {0x315A,0x02},  //INCKSEL2
    {0x3168,0x8F},  //INCKSEL3
    {0x316A,0x7E},  //INCKSEL4
    {0x319D,0x00},  //MDBIT 0:10Bit  1:12Bit
    {0x31A0,0x2A},  //XH&VSOUTSEL
    {0x31A1,0x00},  //XVS/XHS pin setting
    {0x31A4,0x00},  //PULSE1_UP
    {0x31A5,0x00},  //PULSE1_UP
    {0x31A6,0x00},  //PULSE1_UP
    {0x31A8,0x00},  //PULSE1EN/PULSE1_POL
    {0x31AC,0x00},  //PULSE1_DN
    {0x31AD,0x00},  //PULSE1_DN
    {0x31AE,0x00},  //PULSE1_DN
    {0x31D4,0x00},  //XVSLNG
    {0x31D5,0x00},  //XHSLNG
    {0x31D7,0x00},
    {0x31E4,0x01},
    {0x31F3,0x01},
    {0x3288,0x21},
    {0x328A,0x02},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341C,0xFF},  //ADBIT1 10Bit:0x01FF  12Bit:0x0047
    {0x341D,0x01},  //ADBIT1 10Bit:0x01FF  12Bit:0x0047
    {0x3648,0x01},
    {0x364A,0x04},
    {0x364C,0x04},
    {0x3678,0x01},
    {0x367C,0x31},
    {0x367E,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371C,0x3D},
    {0x371D,0x3F},
    {0x372C,0x00},
    {0x372D,0x00},
    {0x372E,0x46},
    {0x372F,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xFE},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375D,0x00},
    {0x375E,0x00},
    {0x375F,0x11},
    {0x3760,0x01},
    {0x3768,0x1B},
    {0x3769,0x1B},
    {0x376A,0x1B},
    {0x376B,0x1B},
    {0x376C,0x1A},
    {0x376D,0x17},
    {0x376E,0x0F},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377A,0x89},
    {0x377B,0x00},
    {0x377C,0x08},
    {0x377D,0x01},
    {0x377E,0x23},
    {0x377F,0x02},
    {0x3780,0xD9},
    {0x3781,0x03},
    {0x3782,0xF5},
    {0x3783,0x06},
    {0x3784,0xA5},
    {0x3788,0x0F},
    {0x378A,0xD9},
    {0x378B,0x03},
    {0x378C,0xEB},
    {0x378D,0x05},
    {0x378E,0x87},
    {0x378F,0x06},
    {0x3790,0xF5},
    {0x3792,0x43},
    {0x3794,0x7A},
    {0x3796,0xA1},
    {0x3A01,0x01},  //LANEMODE 0x03:4Lane  0x01:2Lane
    {0x3000,0x00},
    {0x3002,0x00},
};

const static I2C_ARRAY Sensor_init_table_4lane_5m25fps[] =
{
/* "IMX335LQN All-pixel scan CSI-2_4lane 1188Mbps AD:10bit Output:10bit 27MHz Master Mode 25fps" */
    {0x3002,0x01},  //Master mode stop
    {0xffff,0x14},  //delay
    {0x3000,0x01},  //standby
    {0xffff,0x14},  //delay
    {0x300C,0x42},
    {0x300D,0x2E},
    {0x3030,0x56},  //VMAX //101Dh: 4125 //1518h: 5400
    {0x3031,0x13},  //VMAX
    {0x3032,0x00},  //VMAX
    {0x3034,0x58},  //HMAX //2D0h: 720  //226h: 550
    {0x3035,0x02},  //HMAX
    {0x314C,0xB0},
    {0x315A,0x02},
    {0x3168,0x8F},
    {0x316A,0x7E},
    {0x319D,0x00},  //MDBIT 0:10Bit  1:12Bit
    {0x31A0,0x2A},
    {0x31A1,0x00},
    {0x31A4,0x00},
    {0x31A5,0x00},
    {0x31A6,0x00},
    {0x31A8,0x00},
    {0x31AC,0x00},
    {0x31AD,0x00},
    {0x31AE,0x00},
    {0x31D4,0x00},
    {0x31D5,0x00},
    {0x31D7,0x00},
    {0x31E4,0x01},
    {0x31F3,0x01},
    {0x3288,0x21},
    {0x328A,0x02},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341c,0xFF},  //0x47--12-bit   0xff--10bit
    {0x341d,0x01},  //0x00--12-bit   0x01--10bit
    {0x3648,0x01},
    {0x364A,0x04},
    {0x364C,0x04},
    {0x3678,0x01},
    {0x367C,0x31},
    {0x367E,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371C,0x3D},
    {0x371D,0x3F},
    {0x372C,0x00},
    {0x372D,0x00},
    {0x372E,0x46},
    {0x372F,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xFE},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375D,0x00},
    {0x375E,0x00},
    {0x375F,0x11},
    {0x3760,0x01},
    {0x3768,0x1B},
    {0x3769,0x1B},
    {0x376A,0x1B},
    {0x376B,0x1B},
    {0x376C,0x1A},
    {0x376D,0x17},
    {0x376E,0x0F},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377A,0x89},
    {0x377B,0x00},
    {0x377C,0x08},
    {0x377D,0x01},
    {0x377E,0x23},
    {0x377F,0x02},
    {0x3780,0xD9},
    {0x3781,0x03},
    {0x3782,0xF5},
    {0x3783,0x06},
    {0x3784,0xA5},
    {0x3788,0x0F},
    {0x378A,0xD9},
    {0x378B,0x03},
    {0x378C,0xEB},
    {0x378D,0x05},
    {0x378E,0x87},
    {0x378F,0x06},
    {0x3790,0xF5},
    {0x3792,0x43},
    {0x3794,0x7A},
    {0x3796,0xA1},
    //{0x3A18,0x7F},  //TCLKPOST
    //{0x3A19,0x00},  //TCLKPOST
    //{0x3A1A,0x37},  //TCLKPREPARE
    //{0x3A1B,0x00},  //TCLKPREPARE
    //{0x3A1C,0x37},  //TCLKTRAIL
    //{0x3A1D,0x00},  //TCLKTRAIL
    //{0x3A1E,0xF7},  //TCLKZERO
    //{0x3A1F,0x00},  //TCLKZERO
    //{0x3A20,0x3F},  //THSPREPARE
    //{0x3A21,0x00},  //THSPREPARE
    //{0x3A22,0x6F},  //THSZERO
    //{0x3A23,0x00},  //THSZERO
    //{0x3A24,0x3F},  //THSTRAIL
    //{0x3A25,0x00},  //THSTRAIL
    //{0x3A26,0x5F},  //THSEXIT
    //{0x3A27,0x00},  //THSEXIT
    //{0x3A28,0x2F},  //TLPX
    //{0x3A29,0x00},  //TLPX
    {0x3000,0x00},
    {0x3002,0x00},
};

const static I2C_ARRAY Sensor_init_table_4lane_5m30fps[] =
{
/* "IMX335LQN All-pixel scan CSI-2_4lane 1188Mbps AD:10bit Output:10bit 27MHz Master Mode 30fps" */
    {0x3002,0x01},  //Master mode stop
    {0xffff,0x14},  //delay
    {0x3000,0x01},  //standby
    {0xffff,0x14},  //delay
    {0x300C,0x42},
    {0x300D,0x2E},
    {0x3030,0x1D},  //VMAX //1194 :4500 //101D: 4125
    {0x3031,0x10},  //VMAX
    {0x3032,0x00},  //VMAX
    {0x3034,0x58},  //HMAX //2D0h: 720  //226h: 550
    {0x3035,0x02},  //HMAX
    {0x314C,0xB0},
    {0x315A,0x02},
    {0x3168,0x8F},
    {0x316A,0x7E},
    {0x319D,0x00},  //MDBIT 0:10Bit  1:12Bit
    {0x31A0,0x2A},
    {0x31A1,0x00},
    {0x31A4,0x00},
    {0x31A5,0x00},
    {0x31A6,0x00},
    {0x31A8,0x00},
    {0x31AC,0x00},
    {0x31AD,0x00},
    {0x31AE,0x00},
    {0x31D4,0x00},
    {0x31D5,0x00},
    {0x31D7,0x00},
    {0x31E4,0x01},
    {0x31F3,0x01},
    {0x3288,0x21},
    {0x328A,0x02},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341c,0xFF},  //0x47--12-bit   0xff--10bit
    {0x341d,0x01},  //0x00--12-bit   0x01--10bit
    {0x3648,0x01},
    {0x364A,0x04},
    {0x364C,0x04},
    {0x3678,0x01},
    {0x367C,0x31},
    {0x367E,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371C,0x3D},
    {0x371D,0x3F},
    {0x372C,0x00},
    {0x372D,0x00},
    {0x372E,0x46},
    {0x372F,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xFE},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375D,0x00},
    {0x375E,0x00},
    {0x375F,0x11},
    {0x3760,0x01},
    {0x3768,0x1B},
    {0x3769,0x1B},
    {0x376A,0x1B},
    {0x376B,0x1B},
    {0x376C,0x1A},
    {0x376D,0x17},
    {0x376E,0x0F},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377A,0x89},
    {0x377B,0x00},
    {0x377C,0x08},
    {0x377D,0x01},
    {0x377E,0x23},
    {0x377F,0x02},
    {0x3780,0xD9},
    {0x3781,0x03},
    {0x3782,0xF5},
    {0x3783,0x06},
    {0x3784,0xA5},
    {0x3788,0x0F},
    {0x378A,0xD9},
    {0x378B,0x03},
    {0x378C,0xEB},
    {0x378D,0x05},
    {0x378E,0x87},
    {0x378F,0x06},
    {0x3790,0xF5},
    {0x3792,0x43},
    {0x3794,0x7A},
    {0x3796,0xA1},
    //{0x3A18,0x7F},  //TCLKPOST
    //{0x3A19,0x00},  //TCLKPOST
    //{0x3A1A,0x37},  //TCLKPREPARE
    //{0x3A1B,0x00},  //TCLKPREPARE
    //{0x3A1C,0x37},  //TCLKTRAIL
    //{0x3A1D,0x00},  //TCLKTRAIL
    //{0x3A1E,0xF7},  //TCLKZERO
    //{0x3A1F,0x00},  //TCLKZERO
    //{0x3A20,0x3F},  //THSPREPARE
    //{0x3A21,0x00},  //THSPREPARE
    //{0x3A22,0x6F},  //THSZERO
    //{0x3A23,0x00},  //THSZERO
    //{0x3A24,0x3F},  //THSTRAIL
    //{0x3A25,0x00},  //THSTRAIL
    //{0x3A26,0x5F},  //THSEXIT
    //{0x3A27,0x00},  //THSEXIT
    //{0x3A28,0x2F},  //TLPX
    //{0x3A29,0x00},  //TLPX
    {0x3000,0x00},
    {0x3002,0x00},
};

const static I2C_ARRAY Sensor_init_table_HDR_DOL_4lane5m25fps[] =
{
#if 1
/*"IMX335LQN All-pixel scan CSI-2_4lane 1188Mbps AD:10bit Output:10bit 27MHz Master Mode DOL HDR 2frameLI 25fps" */
    {0x3002,0x01},  //Master mode stop
    {0xffff,0x14},  //delay
    {0x3000,0x01},  //standby
    {0xffff,0x14},  //delay
    {0x300C,0x42},
    {0x300D,0x2E},
    {0x3030,0x9A}, //VMAX 1518: 5400 //4500 1194 //4250 109A
    {0x3031,0x10},
    {0x3032,0x00},
    {0x3034,0x5D}, //HMAX 113: 275  14A          //15Dh 349
    {0x3035,0x01},
    {0x3048,0x01},
    {0x3049,0x01},
    {0x304A,0x04},
    {0x304B,0x03},
    {0x304C,0x00},
    {0x3050,0x00},
    {0x3056,0x07}, //Y_OUT_SIZE[7:0]
    {0x3057,0x08}, //Y_OUT_SIZE[12:8]
    {0x3058,0x88},
    {0x3059,0x1D},
    {0x305A,0x00},
    {0x305C,0x12},
    {0x305D,0x00},
    {0x305E,0x00},
    {0x3068,0x22},
    {0x3069,0x01},
    {0x306A,0x00},
    {0x314C,0xB0},
    {0x315A,0x02},
    {0x3168,0x8F},
    {0x316A,0x7E},
    {0x319D,0x00},
    {0x319F,0x00}, //0x2
    {0x31A0,0x2A},
    {0x31A1,0x00},
    {0x31A4,0x00},
    {0x31A5,0x00},
    {0x31A6,0x00},
    {0x31A8,0x00},
    {0x31AC,0x00},
    {0x31AD,0x00},
    {0x31AE,0x00},
    {0x31D4,0x00},
    {0x31D5,0x00},
    {0x31D7,0x01},
    {0x31E4,0x01},
    {0x31F3,0x01},
    {0x3288,0x21},
    {0x328A,0x02},
    {0x3300,0x00},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341C,0xFF},//0x47--12-bit   0xff--10bit
    {0x341D,0x01},//
    {0x3648,0x01},
    {0x364A,0x04},
    {0x364C,0x04},
    {0x3678,0x01},
    {0x367C,0x31},
    {0x367E,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371C,0x3D},
    {0x371D,0x3F},
    {0x372C,0x00},
    {0x372D,0x00},
    {0x372E,0x46},
    {0x372F,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xFE},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375d,0x00},
    {0x375e,0x00},
    {0x375f,0x11},
    {0x3760,0x01},
    {0x3768,0x1B},
    {0x3769,0x1B},
    {0x376A,0x1B},
    {0x376B,0x1B},
    {0x376C,0x1A},
    {0x376D,0x17},
    {0x376E,0x0F},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377A,0x89},
    {0x377B,0x00},
    {0x377C,0x08},
    {0x377D,0x01},
    {0x377E,0x23},
    {0x377F,0x02},
    {0x3780,0xD9},
    {0x3781,0x03},
    {0x3782,0xF5},
    {0x3783,0x06},
    {0x3784,0xA5},
    {0x3788,0x0F},
    {0x378A,0xD9},
    {0x378B,0x03},
    {0x378C,0xEB},
    {0x378D,0x05},
    {0x378E,0x87},
    {0x378F,0x06},
    {0x3790,0xF5},
    {0x3792,0x43},
    {0x3794,0x7A},
    {0x3796,0xA1},
    //{0x37b0,0x36},
    //{0x3a01,0x03},
    //{0x3a04,0x90},
    //{0x3a05,0x12},
    //{0x3a18,0x8f},
    //{0x3a19,0x00},
    //{0x3a1a,0x4f},
    //{0x3a1b,0x00},
    //{0x3a1c,0x47},
    //{0x3a1d,0x00},
    //{0x3a1e,0x37},
    //{0x3a1f,0x01},
    //{0x3a20,0x4f},
    //{0x3a21,0x00},
    //{0x3a22,0x87},
    //{0x3a23,0x00},
    //{0x3a24,0x4f},
    //{0x3a25,0x00},
    //{0x3a26,0x7f},
    //{0x3a27,0x00},
    //{0x3a28,0x3f},
    //{0x3a29,0x00},
    {0x3000,0x00},
    {0x3002,0x00},
#else //Ori_24Mhz
    {0x3000,0x01},
    {0x3001,0x00},
    {0x3002,0x01},
    {0x3003,0x00},

    {0x300c,0x3b},
    {0x300d,0x2a},
    {0x3018,0x00},
    {0x302c,0x30},
    {0x302d,0x00},
    {0x302e,0x38},
    {0x302f,0x0a},

    {0x3030,0x9A}, //VMAX 1518: 5400 //4500 1194 //4250 109A
    {0x3031,0x10},
    {0x3032,0x00},
    {0x3034,0x5D}, //HMAX 113: 275  14A          //15Dh 349
    {0x3035,0x01},

    {0x3048,0x01},
    {0x3049,0x01},
    {0x304a,0x04},
    {0x304b,0x03},
    {0x304c,0x00},
    {0x304e,0x00},
    {0x304f,0x00},
    {0x3050,0x00},
    {0x3056,0x07},
    {0x3057,0x08},
    {0x3058,0x88},
    {0x3059,0x1d},
    {0x305a,0x00},
    {0x305c,0x12},
    {0x305d,0x00},
    {0x305e,0x00},
    {0x3060,0xe8},
    {0x3061,0x00},
    {0x3062,0x00},
    {0x3064,0x09},
    {0x3065,0x00},
    {0x3066,0x00},
    {0x3068,0x22},
    {0x3069,0x01},
    {0x306a,0x00},
    {0x306c,0x68},
    {0x306d,0x06},
    {0x306e,0x00},
    {0x3072,0x28},
    {0x3073,0x00},
    {0x3074,0xb0},
    {0x3075,0x00},
    {0x3076,0x58},
    {0x3077,0x0f},

    {0x3078,0x01},
    {0x3079,0x02},
    {0x307a,0xff},
    {0x307b,0x02},
    {0x307c,0x00},
    {0x307d,0x00},
    {0x307e,0x00},
    {0x307f,0x00},
    {0x3080,0x01},
    {0x3081,0x02},
    {0x3082,0xff},
    {0x3083,0x02},
    {0x3084,0x00},
    {0x3085,0x00},
    {0x3086,0x00},
    {0x3087,0x00},

    {0x30a4,0x33},
    {0x30a8,0x10},
    {0x30a9,0x04},
    {0x30ac,0x00},
    {0x30ad,0x00},
    {0x30b0,0x10},
    {0x30b1,0x08},
    {0x30b4,0x00},
    {0x30b5,0x00},
    {0x30b6,0x00},
    {0x30b7,0x00},

    {0x30c6,0x00},
    {0x30c7,0x00},
    {0x30ce,0x00},
    {0x30cf,0x00},
    {0x30d8,0x4c},
    {0x30d9,0x10},
    {0x30e8,0x00},
    {0x30e9,0x00},
    {0x30ea,0x00},
    {0x30eb,0x00},
    {0x30ec,0x00},
    {0x30ed,0x00},
    {0x30ee,0x00},
    {0x30ef,0x00},
    {0x3112,0x08},
    {0x3113,0x00},
    {0x3116,0x08},
    {0x3117,0x00},
    {0x314c,0xc6},
    {0x314d,0x00},
    {0x315a,0x02},
    {0x3167,0x01},
    {0x3168,0xa0},
    {0x316a,0x7e},
    {0x3199,0x00},
    {0x319d,0x00},
    {0x319e,0x01},
    {0x319f,0x00},
    {0x31a0,0x2a},
    {0x31a1,0x00},
    {0x31a4,0x00},
    {0x31a5,0x00},
    {0x31a6,0x00},
    {0x31a8,0x00},
    {0x31ac,0x00},
    {0x31ad,0x00},
    {0x31ae,0x00},
    {0x31d4,0x00},
    {0x31d5,0x00},
    {0x31d7,0x01},
    {0x31e4,0x01},
    {0x31e8,0x00},
    {0x31f3,0x01},
    {0x3200,0x01},
    {0x3288,0x21},
    {0x328a,0x02},
    {0x3300,0x00},
    {0x3302,0x32},
    {0x3303,0x00},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341c,0xff},//0x47--12-bit   0xff--10bit
    {0x341d,0x01},//
    {0x3648,0x01},
    {0x364a,0x04},
    {0x364c,0x04},
    {0x3678,0x01},
    {0x367c,0x31},
    {0x367e,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371c,0x3d},
    {0x371d,0x3f},
    {0x372c,0x00},
    {0x372d,0x00},
    {0x372e,0x46},
    {0x372f,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xfe},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375d,0x00},
    {0x375e,0x00},
    {0x375f,0x11},
    {0x3760,0x01},
    {0x3768,0x1b},
    {0x3769,0x1b},
    {0x376a,0x1b},
    {0x376b,0x1b},
    {0x376c,0x1a},
    {0x376d,0x17},
    {0x376e,0x0f},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377a,0x89},
    {0x377b,0x00},
    {0x377c,0x08},
    {0x377d,0x01},
    {0x377e,0x23},
    {0x377f,0x02},
    {0x3780,0xd9},
    {0x3781,0x03},
    {0x3782,0xf5},
    {0x3783,0x06},
    {0x3784,0xa5},
    {0x3788,0x0f},
    {0x378a,0xd9},
    {0x378b,0x03},
    {0x378c,0xeb},
    {0x378d,0x05},
    {0x378e,0x87},
    {0x378f,0x06},
    {0x3790,0xf5},
    {0x3792,0x43},
    {0x3794,0x7a},
    {0x3796,0xa1},
    {0x37b0,0x36},
    {0x3a01,0x03},
    {0x3a04,0x90},
    {0x3a05,0x12},
    {0x3a18,0x8f},
    {0x3a19,0x00},
    {0x3a1a,0x4f},
    {0x3a1b,0x00},
    {0x3a1c,0x47},
    {0x3a1d,0x00},
    {0x3a1e,0x37},
    {0x3a1f,0x01},
    {0x3a20,0x4f},
    {0x3a21,0x00},
    {0x3a22,0x87},
    {0x3a23,0x00},
    {0x3a24,0x4f},
    {0x3a25,0x00},
    {0x3a26,0x7f},
    {0x3a27,0x00},
    {0x3a28,0x3f},
    {0x3a29,0x00},

    {0x3000,0x00},
    {0x3002,0x00},
#endif
};

const static I2C_ARRAY Sensor_init_table_HDR_DOL_4lane5m20fps[] =
{
#if 1
/*"IMX335LQN All-pixel scan CSI-2_4lane 1188Mbps AD:10bit Output:10bit 27MHz Master Mode DOL HDR 2frameLI 25fps" */
    {0x3002,0x01},  //Master mode stop
    {0xffff,0x14},  //delay
    {0x3000,0x01},  //standby
    {0xffff,0x14},  //delay
    {0x300C,0x42},
    {0x300D,0x2E},
    {0x3030,0xC0}, //VMAX 1518: 5400 //4500 1194 //4250 109A
    {0x3031,0x14},
    {0x3032,0x00},
    {0x3034,0x5D}, //HMAX 113: 275  14A          //15Dh 349
    {0x3035,0x01},
    {0x3048,0x01},
    {0x3049,0x01},
    {0x304A,0x04},
    {0x304B,0x03},
    {0x304C,0x00},
    {0x3050,0x00},
    {0x3056,0x07}, //Y_OUT_SIZE[7:0]
    {0x3057,0x08}, //Y_OUT_SIZE[12:8]
    {0x3058,0x88},
    {0x3059,0x1D},
    {0x305A,0x00},
    {0x305C,0x12},
    {0x305D,0x00},
    {0x305E,0x00},
    {0x3068,0x22},
    {0x3069,0x01},
    {0x306A,0x00},
    {0x314C,0xB0},
    {0x315A,0x02},
    {0x3168,0x8F},
    {0x316A,0x7E},
    {0x319D,0x00},
    {0x319F,0x00}, //0x2
    {0x31A0,0x2A},
    {0x31A1,0x00},
    {0x31A4,0x00},
    {0x31A5,0x00},
    {0x31A6,0x00},
    {0x31A8,0x00},
    {0x31AC,0x00},
    {0x31AD,0x00},
    {0x31AE,0x00},
    {0x31D4,0x00},
    {0x31D5,0x00},
    {0x31D7,0x01},
    {0x31E4,0x01},
    {0x31F3,0x01},
    {0x3288,0x21},
    {0x328A,0x02},
    {0x3300,0x00},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341C,0xFF},//0x47--12-bit   0xff--10bit
    {0x341D,0x01},//
    {0x3648,0x01},
    {0x364A,0x04},
    {0x364C,0x04},
    {0x3678,0x01},
    {0x367C,0x31},
    {0x367E,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371C,0x3D},
    {0x371D,0x3F},
    {0x372C,0x00},
    {0x372D,0x00},
    {0x372E,0x46},
    {0x372F,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xFE},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375d,0x00},
    {0x375e,0x00},
    {0x375f,0x11},
    {0x3760,0x01},
    {0x3768,0x1B},
    {0x3769,0x1B},
    {0x376A,0x1B},
    {0x376B,0x1B},
    {0x376C,0x1A},
    {0x376D,0x17},
    {0x376E,0x0F},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377A,0x89},
    {0x377B,0x00},
    {0x377C,0x08},
    {0x377D,0x01},
    {0x377E,0x23},
    {0x377F,0x02},
    {0x3780,0xD9},
    {0x3781,0x03},
    {0x3782,0xF5},
    {0x3783,0x06},
    {0x3784,0xA5},
    {0x3788,0x0F},
    {0x378A,0xD9},
    {0x378B,0x03},
    {0x378C,0xEB},
    {0x378D,0x05},
    {0x378E,0x87},
    {0x378F,0x06},
    {0x3790,0xF5},
    {0x3792,0x43},
    {0x3794,0x7A},
    {0x3796,0xA1},
    //{0x37b0,0x36},
    //{0x3a01,0x03},
    //{0x3a04,0x90},
    //{0x3a05,0x12},
    //{0x3a18,0x8f},
    //{0x3a19,0x00},
    //{0x3a1a,0x4f},
    //{0x3a1b,0x00},
    //{0x3a1c,0x47},
    //{0x3a1d,0x00},
    //{0x3a1e,0x37},
    //{0x3a1f,0x01},
    //{0x3a20,0x4f},
    //{0x3a21,0x00},
    //{0x3a22,0x87},
    //{0x3a23,0x00},
    //{0x3a24,0x4f},
    //{0x3a25,0x00},
    //{0x3a26,0x7f},
    //{0x3a27,0x00},
    //{0x3a28,0x3f},
    //{0x3a29,0x00},
    {0x3000,0x00},
    {0x3002,0x00},
#else //Ori_24Mhz
    {0x3000,0x01},
    {0x3001,0x00},
    {0x3002,0x01},
    {0x3003,0x00},
    {0x300c,0x3b},
    {0x300d,0x2a},
    {0x3018,0x00},
    {0x302c,0x30},
    {0x302d,0x00},
    {0x302e,0x38},
    {0x302f,0x0a},
    {0x3030,0x5e},  //1A5Eh : 6750  // 16A8h : 5800
    {0x3031,0x1a},
    {0x3032,0x00},
    {0x3033,0x00},
    {0x3034,0x13},  //113h : 275   // 140h : 320
    {0x3035,0x01},
    {0x3048,0x01},
    {0x3049,0x01},
    {0x304a,0x04},
    {0x304b,0x03},
    {0x304c,0x00},
    {0x304e,0x00},
    {0x304f,0x00},
    {0x3050,0x00},
    {0x3056,0x07},
    {0x3057,0x08},
    {0x3058,0x14},
    {0x3059,0x28},
    {0x305a,0x00},
    {0x305c,0x12},
    {0x305d,0x00},
    {0x305e,0x00},
    {0x3060,0xe8},
    {0x3061,0x00},
    {0x3062,0x00},
    {0x3064,0x09},
    {0x3065,0x00},
    {0x3066,0x00},
    {0x3068,0x22},
    {0x3069,0x01},
    {0x306a,0x00},
    {0x306c,0x68},
    {0x306d,0x06},
    {0x306e,0x00},
    {0x3072,0x28},
    {0x3073,0x00},
    {0x3074,0xb0},
    {0x3075,0x00},
    {0x3076,0x58},
    {0x3077,0x0f},
    {0x3078,0x01},
    {0x3079,0x02},
    {0x307a,0xff},
    {0x307b,0x02},
    {0x307c,0x00},
    {0x307d,0x00},
    {0x307e,0x00},
    {0x307f,0x00},
    {0x3080,0x01},
    {0x3081,0x02},
    {0x3082,0xff},
    {0x3083,0x02},
    {0x3084,0x00},
    {0x3085,0x00},
    {0x3086,0x00},
    {0x3087,0x00},
    {0x30a4,0x33},
    {0x30a8,0x10},
    {0x30a9,0x04},
    {0x30ac,0x00},
    {0x30ad,0x00},
    {0x30b0,0x10},
    {0x30b1,0x08},
    {0x30b4,0x00},
    {0x30b5,0x00},
    {0x30b6,0x00},
    {0x30b7,0x00},

    {0x30c6,0x00},
    {0x30c7,0x00},
    {0x30ce,0x00},
    {0x30cf,0x00},
    {0x30d8,0x4c},
    {0x30d9,0x10},
    {0x30e8,0x00},
    {0x30e9,0x00},
    {0x30ea,0x00},
    {0x30eb,0x00},
    {0x30ec,0x00},
    {0x30ed,0x00},
    {0x30ee,0x00},
    {0x30ef,0x00},
    {0x3112,0x08},
    {0x3113,0x00},
    {0x3116,0x08},
    {0x3117,0x00},
    {0x314c,0xc6},
    {0x314d,0x00},
    {0x315a,0x02},
    {0x3167,0x01},
    {0x3168,0xa0},
    {0x316a,0x7e},
    {0x3199,0x00},
    {0x319d,0x00},
    {0x319e,0x01},
    {0x319f,0x00},
    {0x31a0,0x2a},
    {0x31a1,0x00},
    {0x31a4,0x00},
    {0x31a5,0x00},
    {0x31a6,0x00},
    {0x31a8,0x00},
    {0x31ac,0x00},
    {0x31ad,0x00},
    {0x31ae,0x00},
    {0x31d4,0x00},
    {0x31d5,0x00},
    {0x31d7,0x01},
    {0x31e4,0x01},
    {0x31e8,0x00},
    {0x31f3,0x01},
    {0x3200,0x01},
    {0x3288,0x21},
    {0x328a,0x02},
    {0x3300,0x00},
    {0x3302,0x32},
    {0x3303,0x00},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341c,0xff},//0x47--12-bit   0xff--10bit
    {0x341d,0x01},//
    {0x3648,0x01},
    {0x364a,0x04},
    {0x364c,0x04},
    {0x3678,0x01},
    {0x367c,0x31},
    {0x367e,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371c,0x3d},
    {0x371d,0x3f},
    {0x372c,0x00},
    {0x372d,0x00},
    {0x372e,0x46},
    {0x372f,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xfe},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375d,0x00},
    {0x375e,0x00},
    {0x375f,0x11},
    {0x3760,0x01},
    {0x3768,0x1b},
    {0x3769,0x1b},
    {0x376a,0x1b},
    {0x376b,0x1b},
    {0x376c,0x1a},
    {0x376d,0x17},
    {0x376e,0x0f},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377a,0x89},
    {0x377b,0x00},
    {0x377c,0x08},
    {0x377d,0x01},
    {0x377e,0x23},
    {0x377f,0x02},
    {0x3780,0xd9},
    {0x3781,0x03},
    {0x3782,0xf5},
    {0x3783,0x06},
    {0x3784,0xa5},
    {0x3788,0x0f},
    {0x378a,0xd9},
    {0x378b,0x03},
    {0x378c,0xeb},
    {0x378d,0x05},
    {0x378e,0x87},
    {0x378f,0x06},
    {0x3790,0xf5},
    {0x3792,0x43},
    {0x3794,0x7a},
    {0x3796,0xa1},
    {0x37b0,0x36},
    {0x3a01,0x03},
    {0x3a04,0x90},
    {0x3a05,0x12},
    {0x3a18,0x8f},
    {0x3a19,0x00},
    {0x3a1a,0x4f},
    {0x3a1b,0x00},
    {0x3a1c,0x47},
    {0x3a1d,0x00},
    {0x3a1e,0x37},
    {0x3a1f,0x01},
    {0x3a20,0x4f},
    {0x3a21,0x00},
    {0x3a22,0x87},
    {0x3a23,0x00},
    {0x3a24,0x4f},
    {0x3a25,0x00},
    {0x3a26,0x7f},
    {0x3a27,0x00},
    {0x3a28,0x3f},
    {0x3a29,0x00},

    {0x3000,0x00},
    {0x3002,0x00},
#endif
};

const static I2C_ARRAY Sensor_init_table_HDR_DOL_4lane5m30fps[] =
{
/*"IMX335LQN All-pixel scan CSI-2_4lane 1188Mbps AD:10bit Output:10bit 27MHz Master Mode DOL HDR 2frameLI 30fps"*/
#if 1
    {0x3002,0x01},  //Master mode stop
    {0xffff,0x14},  //delay
    {0x3000,0x01},  //standby
    {0xffff,0x14},  //delay
    {0x300C,0x42},
    {0x300D,0x2E},
    {0x3030,0x00},  //4096 1000h
    {0x3031,0x10},
    {0x3032,0x00},
    {0x3034,0x2C},  //HMAX 300 : 12Ch
    {0x3035,0x01},
    {0x3048,0x01},
    {0x3049,0x01},
    {0x304A,0x04},
    {0x304B,0x03},
    {0x304C,0x00},
    {0x3050,0x00},
    {0x3056,0x07}, //Y_OUT_SIZE[7:0]
    {0x3057,0x08}, //Y_OUT_SIZE[12:8]
    {0x3058,0x1C},
    {0x3059,0x22},
    {0x305A,0x00},
    {0x305C,0x12},
    {0x305D,0x00},
    {0x305E,0x00},
    {0x3068,0x22},
    {0x3069,0x01},
    {0x306A,0x00},
    {0x314C,0xB0},
    {0x315A,0x02},
    {0x3168,0x8F},
    {0x316A,0x7E},
    {0x319D,0x00},
    {0x319F,0x00}, //0x2
    {0x31A0,0x2A},
    {0x31A1,0x00},
    {0x31A4,0x00},
    {0x31A5,0x00},
    {0x31A6,0x00},
    {0x31A8,0x00},
    {0x31AC,0x00},
    {0x31AD,0x00},
    {0x31AE,0x00},
    {0x31D4,0x00},
    {0x31D5,0x00},
    {0x31D7,0x01},
    {0x31E4,0x01},
    {0x31F3,0x01},
    {0x3288,0x21},
    {0x328A,0x02},
    {0x3300,0x00},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341C,0xFF},//0x47--12-bit   0xff--10bit
    {0x341D,0x01},//
    {0x3648,0x01},
    {0x364A,0x04},
    {0x364C,0x04},
    {0x3678,0x01},
    {0x367C,0x31},
    {0x367E,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371C,0x3D},
    {0x371D,0x3F},
    {0x372C,0x00},
    {0x372D,0x00},
    {0x372E,0x46},
    {0x372F,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xFE},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375d,0x00},
    {0x375e,0x00},
    {0x375f,0x11},
    {0x3760,0x01},
    {0x3768,0x1B},
    {0x3769,0x1B},
    {0x376A,0x1B},
    {0x376B,0x1B},
    {0x376C,0x1A},
    {0x376D,0x17},
    {0x376E,0x0F},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377A,0x89},
    {0x377B,0x00},
    {0x377C,0x08},
    {0x377D,0x01},
    {0x377E,0x23},
    {0x377F,0x02},
    {0x3780,0xD9},
    {0x3781,0x03},
    {0x3782,0xF5},
    {0x3783,0x06},
    {0x3784,0xA5},
    {0x3788,0x0F},
    {0x378A,0xD9},
    {0x378B,0x03},
    {0x378C,0xEB},
    {0x378D,0x05},
    {0x378E,0x87},
    {0x378F,0x06},
    {0x3790,0xF5},
    {0x3792,0x43},
    {0x3794,0x7A},
    {0x3796,0xA1},
    //{0x37b0,0x36},
    //{0x3a01,0x03},
    //{0x3a04,0x90},
    //{0x3a05,0x12},
    //{0x3a18,0x8f},
    //{0x3a19,0x00},
    //{0x3a1a,0x4f},
    //{0x3a1b,0x00},
    //{0x3a1c,0x47},
    //{0x3a1d,0x00},
    //{0x3a1e,0x37},
    //{0x3a1f,0x01},
    //{0x3a20,0x4f},
    //{0x3a21,0x00},
    //{0x3a22,0x87},
    //{0x3a23,0x00},
    //{0x3a24,0x4f},
    //{0x3a25,0x00},
    //{0x3a26,0x7f},
    //{0x3a27,0x00},
    //{0x3a28,0x3f},
    //{0x3a29,0x00},
    {0x3000,0x00},
    {0x3002,0x00},
#else
    {0x3000,0x01},
    {0x3001,0x00},
    {0x3002,0x01},
    {0x3003,0x00},

    {0x300c,0x3b},
    {0x300d,0x2a},
    {0x3018,0x00},
    {0x302c,0x30},
    {0x302d,0x00},
    {0x302e,0x38},
    {0x302f,0x0a},

    {0x3030,0x9A}, //VMAX
    {0x3031,0x11},
    {0x3032,0x00},
    {0x3033,0x00},
    {0x3034,0x13}, //HMAX
    {0x3035,0x01},
    {0x3048,0x01},
    {0x3049,0x01},
    {0x304a,0x04},
    {0x304b,0x03},
    {0x304c,0x00},
    {0x304e,0x00},
    {0x304f,0x00},
    {0x3050,0x00},
    {0x3056,0x07},
    {0x3057,0x08},
    {0x3058,0x9c},
    {0x3059,0x18},
    {0x305a,0x00},
    {0x305c,0x12},
    {0x305d,0x00},
    {0x305e,0x00},
    {0x3060,0xe8},
    {0x3061,0x00},
    {0x3062,0x00},
    {0x3064,0x09},
    {0x3065,0x00},
    {0x3066,0x00},
    {0x3068,0x22},
    {0x3069,0x01},
    {0x306a,0x00},
    {0x306c,0x68},
    {0x306d,0x06},
    {0x306e,0x00},
    {0x3072,0x28},
    {0x3073,0x00},
    {0x3074,0xb0},
    {0x3075,0x00},
    {0x3076,0x58},
    {0x3077,0x0f},

    {0x3078,0x01},
    {0x3079,0x02},
    {0x307a,0xff},
    {0x307b,0x02},
    {0x307c,0x00},
    {0x307d,0x00},
    {0x307e,0x00},
    {0x307f,0x00},
    {0x3080,0x01},
    {0x3081,0x02},
    {0x3082,0xff},
    {0x3083,0x02},
    {0x3084,0x00},
    {0x3085,0x00},
    {0x3086,0x00},
    {0x3087,0x00},

    {0x30a4,0x33},
    {0x30a8,0x10},
    {0x30a9,0x04},
    {0x30ac,0x00},
    {0x30ad,0x00},
    {0x30b0,0x10},
    {0x30b1,0x08},
    {0x30b4,0x00},
    {0x30b5,0x00},
    {0x30b6,0x00},
    {0x30b7,0x00},

    {0x30c6,0x00},
    {0x30c7,0x00},
    {0x30ce,0x00},
    {0x30cf,0x00},
    {0x30d8,0x4c},
    {0x30d9,0x10},
    {0x30e8,0x00},
    {0x30e9,0x00},
    {0x30ea,0x00},
    {0x30eb,0x00},
    {0x30ec,0x00},
    {0x30ed,0x00},
    {0x30ee,0x00},
    {0x30ef,0x00},
    {0x3112,0x08},
    {0x3113,0x00},
    {0x3116,0x08},
    {0x3117,0x00},
    {0x314c,0xc6},
    {0x314d,0x00},
    {0x315a,0x02},
    {0x3167,0x01},
    {0x3168,0xa0},
    {0x316a,0x7e},
    {0x3199,0x00},
    {0x319d,0x00},
    {0x319e,0x01},
    {0x319f,0x02},
    {0x31a0,0x2a},
    {0x31a1,0x00},
    {0x31a4,0x00},
    {0x31a5,0x00},
    {0x31a6,0x00},
    {0x31a8,0x00},
    {0x31ac,0x00},
    {0x31ad,0x00},
    {0x31ae,0x00},
    {0x31d4,0x00},
    {0x31d5,0x00},
    {0x31d7,0x01},
    {0x31e4,0x01},
    {0x31e8,0x00},
    {0x31f3,0x01},
    {0x3200,0x01},
    {0x3288,0x21},
    {0x328a,0x02},
    {0x3300,0x00},
    {0x3302,0x32},
    {0x3303,0x00},
    {0x3414,0x05},
    {0x3416,0x18},
    {0x341c,0xff},//0x47--12-bit   0xff--10bit
    {0x341d,0x01},//
    {0x3648,0x01},
    {0x364a,0x04},
    {0x364c,0x04},
    {0x3678,0x01},
    {0x367c,0x31},
    {0x367e,0x31},
    {0x3706,0x10},
    {0x3708,0x03},
    {0x3714,0x02},
    {0x3715,0x02},
    {0x3716,0x01},
    {0x3717,0x03},
    {0x371c,0x3d},
    {0x371d,0x3f},
    {0x372c,0x00},
    {0x372d,0x00},
    {0x372e,0x46},
    {0x372f,0x00},
    {0x3730,0x89},
    {0x3731,0x00},
    {0x3732,0x08},
    {0x3733,0x01},
    {0x3734,0xfe},
    {0x3735,0x05},
    {0x3740,0x02},
    {0x375d,0x00},
    {0x375e,0x00},
    {0x375f,0x11},
    {0x3760,0x01},
    {0x3768,0x1b},
    {0x3769,0x1b},
    {0x376a,0x1b},
    {0x376b,0x1b},
    {0x376c,0x1a},
    {0x376d,0x17},
    {0x376e,0x0f},
    {0x3776,0x00},
    {0x3777,0x00},
    {0x3778,0x46},
    {0x3779,0x00},
    {0x377a,0x89},
    {0x377b,0x00},
    {0x377c,0x08},
    {0x377d,0x01},
    {0x377e,0x23},
    {0x377f,0x02},
    {0x3780,0xd9},
    {0x3781,0x03},
    {0x3782,0xf5},
    {0x3783,0x06},
    {0x3784,0xa5},
    {0x3788,0x0f},
    {0x378a,0xd9},
    {0x378b,0x03},
    {0x378c,0xeb},
    {0x378d,0x05},
    {0x378e,0x87},
    {0x378f,0x06},
    {0x3790,0xf5},
    {0x3792,0x43},
    {0x3794,0x7a},
    {0x3796,0xa1},
    {0x37b0,0x36},
    {0x3a01,0x03},
    {0x3a04,0x90},
    {0x3a05,0x12},
    {0x3a18,0x8f},
    {0x3a19,0x00},
    {0x3a1a,0x4f},
    {0x3a1b,0x00},
    {0x3a1c,0x47},
    {0x3a1d,0x00},
    {0x3a1e,0x37},
    {0x3a1f,0x01},
    {0x3a20,0x4f},
    {0x3a21,0x00},
    {0x3a22,0x87},
    {0x3a23,0x00},
    {0x3a24,0x4f},
    {0x3a25,0x00},
    {0x3a26,0x7f},
    {0x3a27,0x00},
    {0x3a28,0x3f},
    {0x3a29,0x00},

    {0x3000,0x00},
    {0x3002,0x00},
#endif
};

static I2C_ARRAY PatternTbl[] = {
    {0x308c,0x20}, //colorbar pattern , bit 0 to enable
};

const static I2C_ARRAY gain_HDR_DOL_LEF_reg[] =
{
    {0x30E8, 0x00},// bit0-7 low
    {0x30E9, 0x00},// bit0-2(8-10)
};

const static I2C_ARRAY gain_HDR_DOL_SEF1_reg[] =
{
    {0x30EA, 0x00},// bit0-7 low
    {0x30EB, 0x00},// bit0-2(8-10)
};

const static I2C_ARRAY expo_shr_dol1_reg[] =
{ //SEL
   {0x305e, 0x00},  // bit0-3(16-18)
   {0x305d, 0x00},  // bit0-7(8-15)
   {0x305c, 0x12},  // bit0-7
};

const I2C_ARRAY expo_rhs1_reg[] =
{ //SEL
   {0x306a, 0x00},  // bit0-3(16-18)
   {0x3069, 0x00},  // bit0-7(8-15)
   {0x3068, 0xCE},  // bit0-7
};

const static I2C_ARRAY mirr_flip_table[] =
{
    {0x304e, 0x00},     //M0F0
    {0x304f, 0x00},     //M0F0
    {0x3081, 0x02},
    {0x3083, 0x02},
    {0x30b6, 0x00},
    {0x30b7, 0x00},
    {0x3016, 0x08},

    {0x304e, 0x01},     //M1F0
    {0x304f, 0x00},     //M1F0
    {0x3081, 0x02},
    {0x3083, 0x02},
    {0x30b6, 0x00},
    {0x30b7, 0x00},
    {0x3016, 0x08},

    {0x304e, 0x00},     //M0F1
    {0x304f, 0x01},     //M0F1
    {0x3081, 0xfe},
    {0x3083, 0xfe},
    {0x30b6, 0xfa},
    {0x30b7, 0x01},
    {0x3016, 0x02},

    {0x304e, 0x01},     //M1F1
    {0x304f, 0x01},     //M1F1
    {0x3081, 0xfe},
    {0x3083, 0xfe},
    {0x30b6, 0xfa},
    {0x30b7, 0x01},
    {0x3016, 0x02},
};

const static I2C_ARRAY gain_reg[] = {
    {0x30E8, 0x00},// bit0-7 low
    {0x30E9, 0x00},// bit0-2(8-10)
};

//static int g_sensor_ae_min_gain = 1024;
static CUS_GAIN_GAP_ARRAY gain_gap_compensate[16] = {  //compensate  gain gap
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}
};

const static I2C_ARRAY expo_reg[] = {
   {0x305a, 0x00},  // bit0-3(16-18)
   {0x3059, 0x00},  // bit0-7(8-15)
   {0x3058, 0x09},  // bit0-7
};

const static I2C_ARRAY vts_reg[] = {
   {0x3032, 0x00},  // bit0-3(16-18)
   {0x3031, 0x11},  // bit0-7(8-15)
   {0x3030, 0x94},  // bit0-7
};

/////////////////////////////////////////////////////////////////
//       @@@@@@@                                               //
//           @@                                                //
//          @@                                                 //
//          @@@                                                //
//       @   @@                                                //
//        @@@@                                                 //
//                                                             //
//      Step 3 --  complete camera features                    //
//                                                             //
//  camera set EV, MWB, orientation, contrast, sharpness       //
//   , saturation, and Denoise can work correctly.             //
//                                                             //
/////////////////////////////////////////////////////////////////
#if 0
static CUS_INT_TASK_ORDER def_order = {
        .RunLength = 9,
        .Orders = {
                CUS_INT_TASK_AE|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AWB|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AE|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AWB|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AE|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AWB|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
        },
};
#endif

/////////// function definition ///////////////////
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////
#if 0
static int ISP_config_io(ms_cus_sensor *handle) {
#if 0
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s]", __FUNCTION__);

    sensor_if->HsyncPol(handle, handle->HSYNC_POLARITY);
    sensor_if->VsyncPol(handle, handle->VSYNC_POLARITY);
    sensor_if->ClkPol(handle, handle->PCLK_POLARITY);
    sensor_if->BayerFmt(handle, handle->bayer_id);
    sensor_if->DataBus(handle, handle->sif_bus);

    sensor_if->DataPrecision(handle, handle->data_prec);
    sensor_if->FmtConv(handle,  handle->data_mode);
#endif
    return SUCCESS;
}
#endif

static int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

/*******I5/I6 Support MCLK List*******
 *    CUS_CMU_CLK_27MHZ,
 *    CUS_CMU_CLK_21P6MHZ,
 *    CUS_CMU_CLK_12MHZ,
 *    CUS_CMU_CLK_5P4MHZ,
 *    CUS_CMU_CLK_36MHZ,
 *    CUS_CMU_CLK_54MHZ,
 *    CUS_CMU_CLK_43P2MHZ,
 *    CUS_CMU_CLK_61P7MHZ,
 *    CUS_CMU_CLK_72MHZ,
 *    CUS_CMU_CLK_48MHZ,
 *    CUS_CMU_CLK_24MHZ,
 *    CUS_CMU_CLK_37P125MHZ,
 ******End of Support MCLK List*******/
 #if 0
static CUS_MCLK_FREQ UseParaMclk(const char *mclk)
{
    if (strcmp(mclk, "27M") == 0) {
        return CUS_CMU_CLK_27MHZ;
    } else if (strcmp(mclk, "12M") == 0) {
        return CUS_CMU_CLK_12MHZ;
    } else if (strcmp(mclk, "36M") == 0) {
        return CUS_CMU_CLK_36MHZ;
    } else if (strcmp(mclk, "48M") == 0) {
        return CUS_CMU_CLK_48MHZ;
    } else if (strcmp(mclk, "54M") == 0) {
        return CUS_CMU_CLK_54MHZ;
    } else if (strcmp(mclk, "24M") == 0) {
        return CUS_CMU_CLK_24MHZ;
    } else if (strcmp(mclk, "37.125M") == 0) {
        return CUS_CMU_CLK_37P125MHZ;
    }
    return Preview_MCLK_SPEED;
}
#endif

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 1);
    }

    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    //Sensor board PWDN Enable, 1.8V & 2.9V need 30ms then Pull High
    SENSOR_MSLEEP(31);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_UDELAY(1);
    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_DMSG("Sensor Power On finished\n");
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    sensor_if->MCLK(idx, 0, handle->mclk);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
    handle->orient = SENSOR_ORIT;

    return SUCCESS;
}

/////////////////// Check Sensor Product ID /////////////////////////
#if 0
static int pCus_CheckSensorProductID(ms_cus_sensor *handle)
{
    u16 sen_id_msb, sen_id_lsb, sen_data;

    /* Read Product ID */
    SensorReg_Read(0x3f12, &sen_id_lsb);
    SensorReg_Read(0x3f13, &sen_id_msb);//CHIP_ID_r3F13
    sen_data = ((sen_id_lsb & 0x0F) << 8) | (sen_id_lsb & 0xF0) | (sen_id_msb & 0x0F);
#if 0
    if (sen_data != CHIP_ID) {
        printk("[***ERROR***]Check Product ID Fail: 0x%x\n", sen_data);
        return FAIL;
    }
#endif
    return SUCCESS;
}
#endif
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    int i,n;
    int table_length= ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

    for(n=0;n<table_length;++n)
    {
      id_from_sensor[n].reg = Sensor_id_table[n].reg;
      id_from_sensor[n].data = 0;
    }

    *id =0;
    if(table_length>8) table_length=8;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(n=0;n<4;++n) //retry , until I2C success
    {
      if(n>2) return FAIL;

      if(/* SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == */SUCCESS) //read sensor ID from I2C
          break;
      else
          SENSOR_MSLEEP(1);
    }

    //convert sensor id to u32 format
    for(i=0;i<table_length;++i)
    {
      if( id_from_sensor[i].data != Sensor_id_table[i].data )
        return FAIL;
      *id = id_from_sensor[i].data;
    }

    SENSOR_DMSG("[%s]IMX335 sensor ,Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    return SUCCESS;
}

static int imx335_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i;
    switch(mode)
    {
    case 1:
        PatternTbl[0].data = 0x21; //enable
        break;
    case 0:
        PatternTbl[0].data &= 0xFE; //disable
        break;
    default:
        PatternTbl[0].data &= 0xFE; //disable
        break;
      }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi2lane_5m25fps_linear(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2lane_5m25fps);i++)
    {
        if(Sensor_init_table_2lane_5m25fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2lane_5m25fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2lane_5m25fps[i].reg,Sensor_init_table_2lane_5m25fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_2lane_5m25fps[i].reg, Sensor_init_table_2lane_5m25fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_2lane_5m25fps[i].reg, &sen_data );
            if(Sensor_init_table_2lane_5m25fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_2lane_5m25fps[i].reg);
                //printk("IMX335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_2lane_5m25fps[i].reg, Sensor_init_table_2lane_5m25fps[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi2lane_5m30fps_linear(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2lane_5m30fps);i++)
    {
        if(Sensor_init_table_2lane_5m30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2lane_5m30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2lane_5m30fps[i].reg,Sensor_init_table_2lane_5m30fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_2lane_5m30fps[i].reg, Sensor_init_table_2lane_5m30fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_2lane_5m30fps[i].reg, &sen_data );
            if(Sensor_init_table_2lane_5m30fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_2lane_5m30fps[i].reg);
                //printk("IMX335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_2lane_5m30fps[i].reg, Sensor_init_table_2lane_5m30fps[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi4lane_5m30fps_linear(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_5m30fps);i++)
    {
        if(Sensor_init_table_4lane_5m30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_5m30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_5m30fps[i].reg,Sensor_init_table_4lane_5m30fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_4lane_5m30fps[i].reg, Sensor_init_table_4lane_5m30fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_4lane_5m30fps[i].reg, &sen_data );
            if(Sensor_init_table_4lane_5m30fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_4lane_5m30fps[i].reg);
                //printk("IMX335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_4lane_5m30fps[i].reg, Sensor_init_table_4lane_5m30fps[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi4lane_5m25fps_linear(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_5m25fps);i++)
    {
        if(Sensor_init_table_4lane_5m25fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_5m25fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_5m25fps[i].reg,Sensor_init_table_4lane_5m25fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_4lane_5m25fps[i].reg, Sensor_init_table_4lane_5m25fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_4lane_5m25fps[i].reg, &sen_data );
            if(Sensor_init_table_4lane_5m25fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_4lane_5m25fps[i].reg);
                //printk("IMX335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_4lane_5m25fps[i].reg, Sensor_init_table_4lane_5m25fps[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi4lane5m25fps_HDR_DOL(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_4lane5m25fps);i++)
    {
        if(Sensor_init_table_HDR_DOL_4lane5m25fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HDR_DOL_4lane5m25fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_HDR_DOL_4lane5m25fps[i].reg,Sensor_init_table_HDR_DOL_4lane5m25fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_HDR_DOL_4lane5m25fps[i].reg, Sensor_init_table_HDR_DOL_4lane5m25fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_HDR_DOL_4lane5m25fps[i].reg, &sen_data );
            if(Sensor_init_table_HDR_DOL_4lane5m25fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_HDR_DOL_4lane5m25fps[i].reg);
                //printk("IMX335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HDR_DOL_4lane5m25fps[i].reg, Sensor_init_table_HDR_DOL_4lane5m25fps[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi4lane5m20fps_HDR_DOL(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_4lane5m20fps);i++)
    {
        if(Sensor_init_table_HDR_DOL_4lane5m20fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HDR_DOL_4lane5m20fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_HDR_DOL_4lane5m20fps[i].reg,Sensor_init_table_HDR_DOL_4lane5m20fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_HDR_DOL_4lane5m20fps[i].reg, Sensor_init_table_HDR_DOL_4lane5m20fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_HDR_DOL_4lane5m20fps[i].reg, &sen_data );
            if(Sensor_init_table_HDR_DOL_4lane5m20fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_HDR_DOL_4lane5m20fps[i].reg);
                //printk("IMX335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HDR_DOL_4lane5m20fps[i].reg, Sensor_init_table_HDR_DOL_4lane5m20fps[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi4lane5m30fps_HDR_DOL(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_4lane5m30fps);i++)
    {
        if(Sensor_init_table_HDR_DOL_4lane5m30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HDR_DOL_4lane5m30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_HDR_DOL_4lane5m30fps[i].reg,Sensor_init_table_HDR_DOL_4lane5m30fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_HDR_DOL_4lane5m30fps[i].reg, Sensor_init_table_HDR_DOL_4lane5m30fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_HDR_DOL_4lane5m30fps[i].reg, &sen_data );
            if(Sensor_init_table_HDR_DOL_4lane5m30fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_HDR_DOL_4lane5m30fps[i].reg);
                //printk("IMX335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HDR_DOL_4lane5m30fps[i].reg, Sensor_init_table_HDR_DOL_4lane5m30fps[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_GetVideoResNum( ms_cus_sensor *handle, u32 *ulres_num)
{
    *ulres_num = handle->video_res_supported.num_res;
    return SUCCESS;
}

static int pCus_GetVideoRes(ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}

static int pCus_GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}

static int pCus_SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            if(lane_num == 2){
                handle->pCus_sensor_init = pCus_init_mipi2lane_5m25fps_linear;
            }
            else if (lane_num == 4){
                handle->pCus_sensor_init = pCus_init_mipi4lane_5m25fps_linear;
            }
            else{
                handle->pCus_sensor_init = pCus_init_mipi4lane_5m25fps_linear;
            }
            vts_30fps = 4950;
            Preview_MAX_FPS = 25;
            Preview_line_period  = 8080;
            break;
        case 1:
            handle->video_res_supported.ulcur_res = 1;
            if(lane_num == 2){
                handle->pCus_sensor_init = pCus_init_mipi2lane_5m30fps_linear;
            }
            else if (lane_num == 4){
                handle->pCus_sensor_init = pCus_init_mipi4lane_5m30fps_linear;
            }
            else{
                handle->pCus_sensor_init = pCus_init_mipi4lane_5m30fps_linear;
            }

            vts_30fps  = 4125;
            Preview_MAX_FPS = 30;
            Preview_line_period = 8080;
            break;
        default:
            break;
    }
    params->expo.vts = vts_30fps;
    params->expo.fps = Preview_MAX_FPS;

    return SUCCESS;
}

static int pCus_SetVideoRes_HDR_DOL(ms_cus_sensor *handle, u32 res_idx)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = &handle->sensor_if_api;
    u32 num_res = handle->video_res_supported.num_res;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {

        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_mipi4lane5m25fps_HDR_DOL;
            vts_30fps_HDR_DOL = 4250;
            params->expo.vts = vts_30fps_HDR_DOL;
            Preview_MAX_FPS = 25;
            params->expo.fps=Preview_MAX_FPS;
            Preview_line_period_HDR_DOL = 9411; //8889
            params->max_rhs1 = 290;
            break;
        case 1:
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init_mipi4lane5m20fps_HDR_DOL;
            vts_30fps_HDR_DOL = 5312;
            params->expo.vts = vts_30fps_HDR_DOL;
            Preview_MAX_FPS = 20;
            params->expo.fps=Preview_MAX_FPS;
            Preview_line_period_HDR_DOL = 9411;
            params->max_rhs1 = 290;
            break;
        case 2:
            handle->video_res_supported.ulcur_res = 2;
            handle->pCus_sensor_init = pCus_init_mipi4lane5m30fps_HDR_DOL;
            vts_30fps_HDR_DOL = 4096;
            params->expo.vts = vts_30fps_HDR_DOL;
            Preview_MAX_FPS = 30;
            params->expo.fps= Preview_MAX_FPS;
            Preview_line_period_HDR_DOL = 8318;
            params->max_rhs1 = 290;
            break;
        default:
            break;
    }
    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    short Horiz_Inv = 0;
    short Verti_Inv = 0;
    short Orien_Mode = 0;
    SensorReg_Read(0x304e, &Horiz_Inv);
    SensorReg_Read(0x304f, &Verti_Inv);
    Horiz_Inv &= 0x01;
    Verti_Inv &= 0X01;
    Orien_Mode = Horiz_Inv |(Verti_Inv << 2);
    switch(Orien_Mode)
    {
        case 0x00:
            *orit = CUS_ORIT_M0F0;
        break;
        case 0x01:
            *orit = CUS_ORIT_M1F0;
        break;
        case 0x02:
            *orit = CUS_ORIT_M0F1;
        break;
        case 0x03:
            *orit = CUS_ORIT_M1F1;
        break;
    }
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    imx335_params *params = (imx335_params *)handle->private_data;

    handle->orient = orit;
    params->orien_dirty = true;

    return SUCCESS;
}

static int DoOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length = table_length/4;
    int i,j;

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            handle->orient = CUS_ORIT_M0F0;
            for(i = 0, j = 0; i < seg_length; i++, j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x00);
                //SensorReg_Write(0x304f,0x00);
            }
            break;
        case CUS_ORIT_M1F0:
            for(i = seg_length, j = 0; i < seg_length*2; i++, j++){
                handle->orient = CUS_ORIT_M1F0;
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x01);
                //SensorReg_Write(0x304f,0x00);
            }
            break;
        case CUS_ORIT_M0F1:
            for(i = seg_length * 2, j = 0; i < seg_length*3; i++, j++){
                handle->orient = CUS_ORIT_M0F1;
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x00);
                //SensorReg_Write(0x304f,0x01);
            }
            break;
        case CUS_ORIT_M1F1:
            for(i = seg_length * 3, j = 0; i < seg_length*4; i++, j++){
                handle->orient = CUS_ORIT_M1F1;
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x01);
                //SensorReg_Write(0x304e,0x01);
            }
            break;
        default :
            handle->orient = CUS_ORIT_M0F0;
            break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts = 0;
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000) + fps * 500 )/ (fps * 1000);
    }else if((fps>=(min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts = (vts_30fps*(max_fps*1000) + (fps>>1))/fps;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=25;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.expo_lines > params->expo.vts -2){
        vts = params->expo.expo_lines + 8;
    }else{
        vts = params->expo.vts;
    }
    params->expo.vts = vts;
    pCus_SetAEUSecs(handle, params->expo.expo_lef_us);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS_HDR_DOL_SEF1(ms_cus_sensor *handle)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_DOL_SEF1(ms_cus_sensor *handle, u32 fps)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts = (vts_30fps_HDR_DOL*(max_fps*1000) + fps * 500 )/ (fps * 1000);
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts = (vts_30fps_HDR_DOL*(max_fps*1000) + (fps>>1))/fps;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=30;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
      return FAIL;
    }
    params->dirty = true; //reg need to update = true;
    pCus_SetAEUSecsHDR_DOL_SEF1(handle, params->expo.expo_sef_us);

    return SUCCESS;
}
#if 0
static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
{
    if (cap)
        memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
    else     return FAIL;
    return SUCCESS;
}
#endif

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    imx335_params *params = (imx335_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty || params->orien_dirty) {
                SensorReg_Write(0x3001,1);
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));

                if(params->orien_dirty) {
                    DoOrien(handle, handle->orient);
                    params->orien_dirty = false;
                }
                SensorReg_Write(0x3001,0);
                params->dirty = false;
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_AEStatusNotifyHDR_DOL_SEF1(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    //imx335_params *params = (imx335_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);
             break;
        case CUS_FRAME_ACTIVE:
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    u32 lines = 0;
    imx335_params *params = (imx335_params *)handle->private_data;

    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<16;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[2].data&0xff)<<0;

    *us = (lines*Preview_line_period)/1000;
    SENSOR_DMSG("[%s] sensor expo lines/us %u,%u us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0,activeline = 0;
    imx335_params *params = (imx335_params *)handle->private_data;

    params->expo.expo_lef_us = us;

    lines = (1000 * us) / Preview_line_period;
    if(lines < 9) lines = 9;
    params->expo.expo_lines = lines;

    if (lines >params->expo.vts-1)
        vts = lines +1;
    else
        vts = params->expo.vts;

    SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    activeline = vts - lines;
    if(activeline < 9) activeline = 9;

    params->tExpo_reg[0].data = (activeline>>16) & 0x000f;
    params->tExpo_reg[1].data = (activeline>>8) & 0x00ff;
    params->tExpo_reg[2].data = (activeline>>0) & 0x00ff;

    params->tVts_reg[0].data = (vts >> 16) & 0x000f;
    params->tVts_reg[1].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (vts >> 0) & 0x00ff;

    params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEUSecsHDR_DOL_SEF1(ms_cus_sensor *handle, u32 us)
{
    u32 qua_lines = 0, lines = 0, long_lines = 0,vts = 0, fsc = 0;
    u32 rhs1 = 0, shs1 = 0, shs0 = 0;
    imx335_params *params = (imx335_params *)handle->private_data;

    params->expo.expo_sef_us = us;
    qua_lines = (1000 * us) / Preview_line_period_HDR_DOL /4;
    vts = params->expo.vts;
    shs0 = (params->tExpo_reg[0].data << 16) | (params->tExpo_reg[1].data << 8) | (params->tExpo_reg[2].data << 0);
    fsc = vts * 2;
    long_lines = fsc - shs0;
    params->expo.expo_lines = long_lines;

    //params->max_rhs1 = 290;
    rhs1 = params->max_rhs1;//(params->tExpo_rhs1_reg[0].data << 16) | (params->tExpo_rhs1_reg[1].data << 8) | (params->tExpo_rhs1_reg[2].data << 0);

    if(qua_lines <= 1)
        qua_lines = 1;
    if((4*qua_lines) > (rhs1- 18))
        qua_lines = (rhs1 - 18)/4;

    lines = 4*qua_lines;
    if((rhs1 - 18) <= lines){
        shs1 = 18;
    }
    else if((rhs1 <= params->max_rhs1) && (rhs1 <= shs0 - 18)){
        shs1 = rhs1 - lines;
        if((shs1 < 18) || (shs1 > (rhs1 - 4))){ //Check boundary
            //shs1 = 0;
            //UartSendTrace("[SEF1 NG1]");
        }
    }
    else{
        //UartSendTrace("[SEF1 NG2]");
    }

    params->tExpo_shr_dol1_reg[0].data = (shs1 >> 16) & 0x000f;
    params->tExpo_shr_dol1_reg[1].data = (shs1 >> 8) & 0x00ff;
    params->tExpo_shr_dol1_reg[2].data = (shs1 >> 0) & 0x00ff;
    params->tExpo_rhs1_reg[0].data = (rhs1 >> 16) & 0x000f;
    params->tExpo_rhs1_reg[1].data = (rhs1 >> 8) & 0x00ff;
    params->tExpo_rhs1_reg[2].data = (rhs1 >> 0) & 0x00ff;

    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    //int rc = SensorRegArrayR((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
    unsigned short temp_gain;
  //  *gain=params->expo.final_gain;
    temp_gain=gain_reg[0].data;

    *gain=(u32)(10^((temp_gain*3)/200))*1024;
    if (gain_reg[1].data & 0x10)
       *gain = (*gain) * 2;

    SENSOR_DMSG("[%s] get gain/reg (1024=1X)= %u/0x%x\n", __FUNCTION__, *gain,gain_reg[0].data);
    //return rc;
    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    //double gain_double;
    u64 gain_double;
    params->expo.final_gain = gain;

    if(gain<1024)
       gain=1024;
    else if(gain>=3980*1024)
        gain=3980*1024;

    gain_double = 20*(intlog10(gain)-intlog10(1024));
    params->tGain_reg[0].data=(u16)(((gain_double*10)>> 24)/3);

    SENSOR_DMSG("[%s] set gain/reg=%u/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    //extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 i=0;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u64 gain_double;

    params->expo.final_gain = gain;
    if(gain < SENSOR_MIN_GAIN)
        gain = SENSOR_MIN_GAIN;
    else if(gain >= SENSOR_MAX_GAIN)
        gain = SENSOR_MAX_GAIN;
    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    gain_double = 20*(intlog10(gain)-intlog10(1024));
    params->tGain_reg[0].data=(u16)(((gain_double*10)>> 24)/3) & 0x00ff;
    params->tGain_reg[1].data=(u16)((((gain_double*10)>> 24)/3) >> 8) & 0x0007;

    SENSOR_DMSG("[%s] set gain/reg=%u/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data);
    params->dirty = true;
    return SUCCESS;
}

static void pCus_SetAEGainHDR_DOL_Calculate(u32 gain, u16 *gain_reg)
{
    //double gain_double;
    u64 gain_double;

    if(gain < SENSOR_MIN_GAIN){
      gain = SENSOR_MIN_GAIN;
    }
    else if(gain >= SENSOR_MAX_GAIN){
      gain = SENSOR_MAX_GAIN;
    }
    gain_double = 20*(intlog10(gain)-intlog10(1024));
    *gain_reg=(u16)(((gain_double*10)>> 24)/3) & 0x07ff;
}

static int pCus_SetAEGainHDR_DOL_SEF1(ms_cus_sensor *handle, u32 gain)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    u16 gain_reg = 0;

    pCus_SetAEGainHDR_DOL_Calculate(gain, &gain_reg);
    params->tGain_hdr_dol_sef_reg[0].data = gain_reg & 0x00ff;
    params->tGain_hdr_dol_sef_reg[1].data = (gain_reg>>8) & 0x0007;

    SENSOR_DMSG("[%s] set gain/reg=%u/0x%x\n", __FUNCTION__, gain, params->tGain_hdr_dol_sef_reg[0].data);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = 1;
    *max = 1000000/imx335_mipi_linear[0].senout.min_fps;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = SENSOR_MIN_GAIN;//handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;//3980*1024;
    return SUCCESS;
}

static int IMX335_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/imx335_mipi_linear[0].senout.min_fps;
    info->min = (Preview_line_period * 9);
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
    u32 i, j;

    for(i=0,j=0;i< num ;i++,j+=2){
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }
    return SUCCESS;
}

static int IMX335_GetShutterInfoHDR_DOL_SEF1(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    info->max = Preview_line_period_HDR_DOL * params->max_rhs1;
    info->min = (Preview_line_period_HDR_DOL * 4);
    info->step = Preview_line_period_HDR_DOL;
    return SUCCESS;
}

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx335_params *params;
    int res;

    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    ////////////////////////////////////
    // private data allocation & init //
    ////////////////////////////////////
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }

    params = (imx335_params *)handle->private_data;
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    //memcpy(params->tExpo_rhs1_reg, expo_rhs1_reg, sizeof(expo_rhs1_reg));
    //memcpy(params->tExpo_shr_dol1_reg, expo_shr_dol1_reg, sizeof(expo_shr_dol1_reg));
    //memcpy(params->tGain_hdr_dol_lef_reg, gain_HDR_DOL_LEF_reg, sizeof(gain_HDR_DOL_LEF_reg));
    //memcpy(params->tGain_hdr_dol_sef_reg, gain_HDR_DOL_SEF1_reg, sizeof(gain_HDR_DOL_SEF1_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX335_MIPI");

    ////////////////////////////////////
    //    i2c config                  //
    ////////////////////////////////////
    handle->i2c_cfg.mode          = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt           = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D8;
    handle->i2c_cfg.address       = SENSOR_I2C_ADDR;      //0x34;
    handle->i2c_cfg.speed         = SENSOR_I2C_SPEED;     //300000;

    ////////////////////////////////////
    //    mclk                        //
    ////////////////////////////////////
    handle->mclk                    = Preview_MCLK_SPEED;//UseParaMclk(SENSOR_DRV_PARAM_MCLK());

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    handle->isp_type              = SENSOR_ISP_TYPE;
    //handle->data_fmt              = SENSOR_DATAFMT;
    handle->sif_bus               = SENSOR_IFBUS_TYPE;
    handle->data_prec             = SENSOR_DATAPREC;
    handle->data_mode             = SENSOR_DATAMODE;
    handle->bayer_id              = SENSOR_BAYERID;
    handle->RGBIR_id              = SENSOR_RGBIRID;
    handle->orient                = SENSOR_ORIT;
    //handle->YC_ODER               = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num    = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order   = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode  = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode    = CUS_HDR_MODE_NONE;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0; //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
   //handle->video_res_supported.num_res = LINEAR_RES_END;
    for (res = 0; res < LINEAR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = imx335_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = imx335_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = imx335_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = imx335_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = imx335_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = imx335_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = imx335_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = imx335_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, imx335_mipi_linear[res].senstr.strResDesc);
    }

    ////////////////////////////////////
    //    Sensor polarity             //
    ////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error

    ////////////////////////////////////////
    // Sensor Status Control and Get Info //
    ////////////////////////////////////////
    handle->pCus_sensor_release         = cus_camsensor_release_handle;
    handle->pCus_sensor_init            = pCus_init_mipi4lane_5m30fps_linear;
    //handle->pCus_sensor_powerupseq      = pCus_powerupseq;
    handle->pCus_sensor_poweron         = pCus_poweron;
    handle->pCus_sensor_poweroff        = pCus_poweroff;
    handle->pCus_sensor_GetSensorID     = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum  = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes     = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes     = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien        = pCus_GetOrien;
    handle->pCus_sensor_SetOrien        = pCus_SetOrien;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode  = imx335_SetPatternMode;

    ////////////////////////////////////
    //    AE parameters               //
    ////////////////////////////////////
    handle->ae_gain_delay              = SENSOR_GAIN_DELAY_FRAME_COUNT;
    handle->ae_shutter_delay           = SENSOR_SHUTTER_DELAY_FRAME_COUNT;
    handle->ae_gain_ctrl_num           = 1;
    handle->ae_shutter_ctrl_num        = 1;
    handle->sat_mingain                = SENSOR_MIN_GAIN;  //calibration
    //handle->dgain_remainder = 0;

    ////////////////////////////////////
    //  AE Control and Get Info       //
    ////////////////////////////////////
    // unit: micro seconds
    //handle->pCus_sensor_GetAETrigger_mode      = pCus_GetAETrigger_mode;
    //handle->pCus_sensor_SetAETrigger_mode      = pCus_SetAETrigger_mode;
    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;
    //handle->pCus_sensor_GetDGainRemainder = pCus_GetDGainRemainder;

    //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = IMX335_GetShutterInfo;

    params->expo.vts        = vts_30fps;
    params->expo.expo_lines = 5000;
    params->dirty           = false;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_sef1(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx335_params *params = NULL;
    int res;

    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    ////////////////////////////////////
    // private data allocation & init //
    ////////////////////////////////////
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }

    params = (imx335_params *)handle->private_data;
    memcpy(params->tExpo_rhs1_reg, expo_rhs1_reg, sizeof(expo_rhs1_reg));
    memcpy(params->tExpo_shr_dol1_reg, expo_shr_dol1_reg, sizeof(expo_shr_dol1_reg));
    memcpy(params->tGain_hdr_dol_sef_reg, gain_HDR_DOL_SEF1_reg, sizeof(gain_HDR_DOL_SEF1_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX335_MIPI_HDR_SEF");

    ////////////////////////////////////
    //    i2c config                  //
    ////////////////////////////////////
    handle->i2c_cfg.mode          = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt           = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address       = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed         = SENSOR_I2C_SPEED;     //320000;

    ////////////////////////////////////
    //    mclk                        //
    ////////////////////////////////////
    handle->mclk                  = Preview_MCLK_SPEED_HDR_DOL;//UseParaMclk(SENSOR_DRV_PARAM_MCLK());

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //handle->isp_type              = SENSOR_ISP_TYPE;
    //handle->data_fmt              = SENSOR_DATAFMT;
    handle->sif_bus               = SENSOR_IFBUS_TYPE;
    handle->data_prec             = SENSOR_DATAPREC_DOL;
    handle->data_mode             = SENSOR_DATAMODE;
    handle->bayer_id              = SENSOR_BAYERID_HDR_DOL;
    handle->RGBIR_id              = SENSOR_RGBIRID;

    handle->interface_attr.attr_mipi.mipi_lane_num                = SENSOR_MIPI_LANE_NUM_DOL;
    handle->interface_attr.attr_mipi.mipi_data_format             = CUS_SEN_INPUT_FORMAT_RGB;
    handle->interface_attr.attr_mipi.mipi_hsync_mode              = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode                = CUS_HDR_MODE_SONY_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    for (res = 0; res < HDR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = imx335_mipi_hdr[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = imx335_mipi_hdr[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = imx335_mipi_hdr[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = imx335_mipi_hdr[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = imx335_mipi_hdr[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = imx335_mipi_hdr[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = imx335_mipi_hdr[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = imx335_mipi_hdr[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, imx335_mipi_hdr[res].senstr.strResDesc);
    }

    ////////////////////////////////////
    //    Sensor polarity             //
    ////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error

    ////////////////////////////////////////
    // Sensor Status Control and Get Info //
    ////////////////////////////////////////
    handle->pCus_sensor_release        = cus_camsensor_release_handle;
    handle->pCus_sensor_init           = pCus_init_mipi4lane5m25fps_HDR_DOL;
    handle->pCus_sensor_poweron        = pCus_poweron;               // Need to check
    handle->pCus_sensor_poweroff       = pCus_poweroff;
    handle->pCus_sensor_GetSensorID    = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_SetVideoRes    = pCus_SetVideoRes_HDR_DOL;
    handle->pCus_sensor_GetVideoRes    = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes = pCus_GetCurVideoRes;

    handle->pCus_sensor_GetOrien       = pCus_GetOrien;              // Need to check
    handle->pCus_sensor_SetOrien       = pCus_SetOrien;              // Need to check
    handle->pCus_sensor_GetFPS         = pCus_GetFPS_HDR_DOL_SEF1;
    handle->pCus_sensor_SetFPS         = pCus_SetFPS_HDR_DOL_SEF1;

    ////////////////////////////////////
    //    AE parameters               //
    ////////////////////////////////////
    handle->ae_gain_delay              = SENSOR_GAIN_DELAY_FRAME_COUNT_HDR_DOL;
    handle->ae_shutter_delay           = SENSOR_SHUTTER_DELAY_FRAME_COUNT_HDR_DOL;
    handle->ae_gain_ctrl_num           = 2;
    handle->ae_shutter_ctrl_num        = 2;
    handle->sat_mingain                = SENSOR_MIN_GAIN;      //g_sensor_ae_min_gain;
    //handle->dgain_remainder = 0;

    ////////////////////////////////////
    //  AE Control and Get Info       //
    ////////////////////////////////////
    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotifyHDR_DOL_SEF1;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_SEF1;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGainHDR_DOL_SEF1;
    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

    handle->pCus_sensor_GetShutterInfo  = IMX335_GetShutterInfoHDR_DOL_SEF1;
    params->expo.vts                    = vts_30fps_HDR_DOL;
    params->expo.expo_lines             = 4250;
    return SUCCESS;
}

//lef functions
static int pCus_init_HDR_DOL_LEF(ms_cus_sensor *handle)
{
    return SUCCESS;
}

static int pCus_poweron_HDR_DOL_LEF(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

static int pCus_poweroff_HDR_DOL_LEF(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

static int pCus_GetSensorID_HDR_DOL_LEF(ms_cus_sensor *handle, u32 *id)
{
    *id = 0;
     return SUCCESS;
}

static int pCus_GetOrien_HDR_DOL_LEF(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    *orit = CUS_ORIT_M0F0;
    return SUCCESS;
}

static int pCus_SetOrien_HDR_DOL_LEF(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    return SUCCESS;
}

static int pCus_GetFPS_HDR_DOL_LEF(ms_cus_sensor *handle)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_DOL_LEF(ms_cus_sensor *handle, u32 fps)
{
    u32 vts = 0;
    imx335_params *params = (imx335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR_DOL*max_fps)/fps;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR_DOL*(max_fps*1000))/fps;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=30;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.expo_lines > 2 * params->expo.vts - params->max_rhs1 -18){
        vts = (params->expo.expo_lines + params->max_rhs1 + 18) / 2;
    }else{
        vts = params->expo.vts;
    }

    pCus_SetAEUSecsHDR_DOL_LEF(handle, params->expo.expo_lef_us);

    return SUCCESS;
}
#if 0
static int pCus_GetSensorCap_hdr_dol_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
{
    if (cap)
        memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
    else     return FAIL;
    return SUCCESS;
}
#endif
static int imx335_SetPatternMode_hdr_dol_lef(ms_cus_sensor *handle,u32 mode)
{
    return SUCCESS;
}

static int pCus_AEStatusNotifyHDR_DOL_LEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    imx335_params *params = (imx335_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty || params->orien_dirty)
            {
                SensorReg_Write(0x3001,1);
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_shr_dol1_reg, ARRAY_SIZE(expo_shr_dol1_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_rhs1_reg, ARRAY_SIZE(expo_rhs1_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_hdr_dol_lef_reg, ARRAY_SIZE(gain_HDR_DOL_LEF_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_hdr_dol_sef_reg, ARRAY_SIZE(gain_HDR_DOL_SEF1_reg));

                if(params->orien_dirty) {
                    DoOrien(handle, handle->orient);
                    params->orien_dirty = false;
                }
                SensorReg_Write(0x3001,0);
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs_HDR_DOL_LEF(ms_cus_sensor *handle, u32 *us)
{
    *us = 0;
    return SUCCESS;
}

static int pCus_SetAEUSecsHDR_DOL_LEF(ms_cus_sensor *handle, u32 us)
{
    u32 qua_lines = 0,lines = 0, half_vts = 0, vts = 0, shr_dol0 = 0, fsc = 0;
    imx335_params *params = (imx335_params *)handle->private_data;

    qua_lines = (1000 * us) / Preview_line_period_HDR_DOL / 4;
   // lines=us/Preview_line_period_HDR_DOL;
    params->expo.expo_lef_us = us;

    if (4 * qua_lines > 2 * params->expo.vts - params->max_rhs1 - 18) { // shs2 > max_rhs1 +2
        half_vts = (4 * qua_lines + params->max_rhs1 + 19) / 4;
    }
    else{
        half_vts = params->expo.vts / 2;
    }

    SENSOR_DMSG("[%s] us %u, qua_lines %u, vts %u\n", __FUNCTION__,
                us,
                qua_lines,
                params->expo.vts
                );

    //exposure limit lines = fsc - (shs2 + 1) = fsc - 1 - ( rhs1 + 2 ~ fsc - 2) = 1 ~ fsc - 104 (rhs1 fix to 101)
    vts = half_vts * 2;
    fsc = half_vts * 4;
    if(qua_lines < 1)  // shs2 < fsc - 2
        qua_lines = 1;
    if(4 * qua_lines > fsc - params->max_rhs1 - 18)
        qua_lines = (fsc - params->max_rhs1 - 18) / 4;

    lines = 4*qua_lines;
    params->expo.expo_lines = lines;

    shr_dol0 = fsc - lines;
#if 0
    printk("[%s] us %u, qua_lines %u, vts %u shr_dol0 %u\n", __FUNCTION__,
                us,
                qua_lines,
                params->expo.vts,
                shr_dol0
                );
#endif
    params->tExpo_reg[0].data = (shr_dol0 >> 16) & 0x000f;
    params->tExpo_reg[1].data = (shr_dol0 >> 8) & 0x00ff;
    params->tExpo_reg[2].data = (shr_dol0 >> 0) & 0x00ff;

    params->tVts_reg[0].data = (vts >> 16) & 0x000f;
    params->tVts_reg[1].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (vts >> 0) & 0x00ff;

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEGain_HDR_DOL_LEF(ms_cus_sensor *handle, u32* gain)
{
    *gain = 0;
    return SUCCESS;
}

static int pCus_SetAEGainHDR_DOL_LEF(ms_cus_sensor *handle, u32 gain)
{
    imx335_params *params = (imx335_params *)handle->private_data;
    u16 gain_reg = 0;

    pCus_SetAEGainHDR_DOL_Calculate(gain, &gain_reg);
    params->tGain_hdr_dol_lef_reg[0].data = gain_reg & 0x00ff;
    params->tGain_hdr_dol_lef_reg[1].data = (gain_reg>>8) & 0x0007;

    SENSOR_DMSG("[%s] set gain/reg=%u/0x%x\n", __FUNCTION__, gain, params->tGain_hdr_dol_lef_reg[0].data);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain_HDR_DOL_LEF(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs_HDR_DOL_LEF(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = 1;
    *max = 1000000/imx335_mipi_linear[0].senout.min_fps;
    return SUCCESS;
}

static int pCus_SetAEGain_cal_hdr_dol_lef(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity_hdr_dol_lef(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    return SUCCESS;
}

static int IMX335_GetShutterInfo_hdr_dol_lef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/imx335_mipi_linear[0].senout.min_fps;
    info->min = (Preview_line_period_HDR_DOL * 4);
    info->step = Preview_line_period_HDR_DOL;
    return SUCCESS;
}

static int cus_camsensor_init_handle_hdr_dol_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx335_params *params;
    s32 res;

    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }

    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (imx335_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tGain_hdr_dol_lef_reg, gain_HDR_DOL_LEF_reg, sizeof(gain_HDR_DOL_LEF_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX335_MIPI_HDR_LEF");

    ////////////////////////////////////
    //    i2c config                  //
    ////////////////////////////////////
    handle->i2c_cfg.mode          = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt           = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address       = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed         = SENSOR_I2C_SPEED;     //320000;

    ////////////////////////////////////
    //    mclk                        //
    ////////////////////////////////////
    handle->mclk                  = Preview_MCLK_SPEED_HDR_DOL;//UseParaMclk(SENSOR_DRV_PARAM_MCLK());
    //sensor_if->MCLK(0,1,handle->mclk);

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    handle->isp_type               = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt               = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus                = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec              = SENSOR_DATAPREC_DOL;  //CUS_DATAPRECISION_8;
    handle->data_mode              = SENSOR_DATAMODE;
    handle->bayer_id               = SENSOR_BAYERID_HDR_DOL;   //CUS_BAYER_GB;
    handle->RGBIR_id               = SENSOR_RGBIRID;
    handle->orient                 = SENSOR_ORIT;        //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num                = SENSOR_MIPI_LANE_NUM_DOL;
    handle->interface_attr.attr_mipi.mipi_data_format             = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order               = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode              = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode                = CUS_HDR_MODE_SONY_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num =  0; //Long frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    for (res = 0; res < HDR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = imx335_mipi_hdr[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = imx335_mipi_hdr[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = imx335_mipi_hdr[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = imx335_mipi_hdr[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = imx335_mipi_hdr[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = imx335_mipi_hdr[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = imx335_mipi_hdr[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = imx335_mipi_hdr[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, imx335_mipi_hdr[res].senstr.strResDesc);
    }

    ////////////////////////////////////
    //    Sensor polarity             //
    ////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error

    ////////////////////////////////////////
    // Sensor Status Control and Get Info //
    ////////////////////////////////////////
    handle->pCus_sensor_release         = cus_camsensor_release_handle;
    handle->pCus_sensor_init            = pCus_init_HDR_DOL_LEF;
    //handle->pCus_sensor_powerupseq    = pCus_powerupseq;
    handle->pCus_sensor_poweron         = pCus_poweron_HDR_DOL_LEF;
    handle->pCus_sensor_poweroff        = pCus_poweroff_HDR_DOL_LEF;
    handle->pCus_sensor_GetSensorID     = pCus_GetSensorID_HDR_DOL_LEF;
    //handle->pCus_sensor_GetVideoResNum  = NULL;
    //handle->pCus_sensor_GetVideoRes     = NULL;
    //handle->pCus_sensor_GetCurVideoRes  = NULL;
    //handle->pCus_sensor_SetVideoRes     = NULL;   // Need to check

    handle->pCus_sensor_GetOrien        = pCus_GetOrien_HDR_DOL_LEF;
    handle->pCus_sensor_SetOrien        = pCus_SetOrien_HDR_DOL_LEF;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_DOL_LEF;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_DOL_LEF;

    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap_hdr_dol_lef;
    handle->pCus_sensor_SetPatternMode  = imx335_SetPatternMode_hdr_dol_lef;

    ////////////////////////////////////
    //    AE parameters               //
    ////////////////////////////////////
    handle->ae_gain_delay               = SENSOR_GAIN_DELAY_FRAME_COUNT_HDR_DOL;
    handle->ae_shutter_delay            = SENSOR_SHUTTER_DELAY_FRAME_COUNT_HDR_DOL;
    handle->ae_gain_ctrl_num            = 2;
    handle->ae_shutter_ctrl_num         = 2;
    handle->sat_mingain                 = SENSOR_MIN_GAIN;
    //handle->dgain_remainder = 0;

    ////////////////////////////////////
    //  AE Control and Get Info       //
    ////////////////////////////////////
    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotifyHDR_DOL_LEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs_HDR_DOL_LEF;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_LEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain_HDR_DOL_LEF;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGainHDR_DOL_LEF;
    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain_HDR_DOL_LEF;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs_HDR_DOL_LEF;
    //handle->pCus_sensor_GetDGainRemainder = pCus_GetDGainRemainder;

    //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal_hdr_dol_lef;
    handle->pCus_sensor_setCaliData_gain_linearity = pCus_setCaliData_gain_linearity_hdr_dol_lef;
    handle->pCus_sensor_GetShutterInfo  = IMX335_GetShutterInfo_hdr_dol_lef;

    params->expo.vts                    = vts_30fps_HDR_DOL;
    params->expo.expo_lines             = 673;
    params->expo.fps                    = 25;
    params->dirty                       = false;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(IMX335_HDR,
                            cus_camsensor_init_handle_linear,
                            cus_camsensor_init_handle_hdr_dol_sef1,
                            cus_camsensor_init_handle_hdr_dol_lef,
                            imx335_params
                         );
