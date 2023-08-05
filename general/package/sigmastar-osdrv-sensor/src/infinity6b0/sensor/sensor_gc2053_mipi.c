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

SENSOR_DRV_ENTRY_IMPL_BEGIN(gc2053);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

//#define _DEBUG_
//c11 extern int usleep(u32 usec);
//int usleep(u32 usec);

#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (2)
//#define SENSOR_MIPI_HDR_MODE (1) //0: Non-HDR mode. 1:Sony DOL mode
//MIPI config end.
//============================================

#define R_GAIN_REG 1
#define G_GAIN_REG 2
#define B_GAIN_REG 3


//#undef SENSOR_DBG
#define SENSOR_DBG 0

///////////////////////////////////////////////////////////////
//          @@@                                                                                       //
//       @   @@      ==  S t a r t * H e r e ==                                            //
//            @@      ==  S t a r t * H e r e  ==                                            //
//            @@      ==  S t a r t * H e r e  ==                                           //
//         @@@@                                                                                  //
//                                                                                                     //
//      Start Step 1 --  show preview on LCM                                         //
//                                                                                               //
//  Fill these #define value and table with correct settings                        //
//      camera can work and show preview on LCM                                 //
//                                                                                                       //
///////////////////////////////////////////////////////////////

#define SENSOR_ISP_TYPE     ISP_EXT                   //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_RG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     128                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 2
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
#define Preview_line_period 29630//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
//#define Line_per_second     32727
#define vts_30fps  1125//1266//1150//1090                              //for 29.091fps @ MCLK=36MHz
#define Prv_Max_line_number 2200                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       1920                    //resolution Width when preview
#define Preview_HEIGHT      1080                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x6E                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A8D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

int blk_flag=1;
int times = 0;
//static int  drv_Fnumber = 22;
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
CUS_MCLK_FREQ UseParaMclk(void);

CUS_CAMSENSOR_CAP sensor_cap = {
    .length = sizeof(CUS_CAMSENSOR_CAP),
    .version = 0x0001,
};

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
        u32 sclk;
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
        u32 line;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
} gc2053_params;
// set sensor ID address and data,

/* typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;
 */
static I2C_ARRAY Sensor_id_table[] =
{
{0xf0, 0x20},      // {address of ID, ID },
{0xf1, 0x53},
};

typedef struct {

      u32 gain;
      unsigned short again_reg_val_0;
      unsigned short again_reg_val_1;
      unsigned short again_reg_val_2;
      unsigned short again_reg_val_3;

} Gain_ARRAY;

static I2C_ARRAY Sensor_init_table[] =
{
                        /****system****/
{0xfe, 0x80},
{0xfe, 0x80},
{0xfe, 0x80},
{0xfe, 0x00},
{0xf2, 0x00},//[1]I2C_open_ena [0]pwd_dn
{0xf3, 0x00},//0f//00[3]Sdata_pad_io [2:0]Ssync_pad_io
{0xf4, 0x36},//[6:4]pll_ldo_set
{0xf5, 0xc0},//[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
{0xf6, 0x44},//[7:3]wpllclk_div [2:0]refmp_div
{0xf7, 0x01},//[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
{0xf8, 0x2c},//[7:0]pllmp_div
{0xf9, 0x42},//[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
{0xfc, 0x8e},                           /****CISCTL & ANALOG****/
{0xfe, 0x00},
{0x87, 0x18},//[6]aec_delay_mode
{0xee, 0x30},//[5:4]dwen_sramen
{0xd0, 0xb7},//ramp_en
{0x03, 0x04},
{0x04, 0x60},
{0x05, 0x04},//05
{0x06, 0x4c},//60//[11:0]hb
{0x07, 0x00},
{0x08, 0x19},
{0x09, 0x00},
{0x0a, 0x02}, //cisctl row start
{0x0b, 0x00},
{0x0c, 0x02}, //cisctl col start
{0x0d, 0x04},
{0x0e, 0x40},
{0x12, 0xe2}, //vsync_ahead_mode
{0x13, 0x16},
{0x19, 0x0a}, //ad_pipe_num
{0x21, 0x1c},//eqc1fc_eqc2fc_sw
{0x28, 0x0a},//16//eqc2_c2clpen_sw
{0x29, 0x24},//eq_post_width
{0x2b, 0x04},//c2clpen --eqc2
{0x32, 0xf8}, //[5]txh_en ->avdd28
{0x37, 0x03}, //[3:2]eqc2sel=0
{0x39, 0x15},//17 //[3:0]rsgl
{0x43, 0x07},//vclamp
{0x44, 0x40}, //0e//post_tx_width
{0x46, 0x0b},
{0x4b, 0x20}, //rst_tx_width
{0x4e, 0x08}, //12//ramp_t1_width
{0x55, 0x20}, //read_tx_width_pp
{0x66, 0x05}, //18//stspd_width_r1
{0x67, 0x05}, //40//5//stspd_width_r
{0x77, 0x01}, //dacin offset x31
{0x78, 0x00}, //dacin offset
{0x7c, 0x93}, //[1:0] co1comp
{0x8c, 0x12}, //12 ramp_t1_ref
{0x8d, 0x92},//90
{0x90, 0x00},
{0x9d, 0x10},
{0xce, 0x7c},//70//78//[4:2]c1isel
{0xd2, 0x41},//[5:3]c2clamp
{0xd3, 0xdc},//ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
{0xe6, 0x50},//ramps offset                 /*gain*/
{0xb6, 0xc0},
{0xb0, 0x70},
{0xb1, 0x01},
{0xb2, 0x00},
{0xb3, 0x00},
{0xb4, 0x00},
{0xb8, 0x01},
{0xb9, 0x00},
/*blk*/
{0x26, 0x30},//23
{0xfe, 0x01},
{0x40, 0x23},
{0x55, 0x07},
{0x58, 0x00},//random noise disable

{0x60, 0x40}, //[7:0]WB_offset
{0xfe, 0x04},
{0x14, 0x78}, //g1 ratio
{0x15, 0x78}, //r ratio
{0x16, 0x78}, //b ratio
{0x17, 0x78}, //g2 ratio
/*window*/
{0xfe, 0x01},
{0x92, 0x00}, //win y1
{0x94, 0x03}, //win x1
{0x95, 0x04},
{0x96, 0x38}, //[10:0]out_height
{0x97, 0x07},
{0x98, 0x80}, //[11:0]out_width
/*ISP*/
{0xfe, 0x01},
{0x58, 0x00},
{0x01, 0x05},//03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
{0x02, 0x89}, //[7:0]BFF_sram_mode
{0x04, 0x01}, //[0]DD_en
{0x07, 0xa6},
{0x08, 0xa9},
{0x09, 0xa8},
{0x0a, 0xa7},
{0x0b, 0xff},
{0x0c, 0xff},
{0x0f, 0x00},
{0x50, 0x1c},
{0x89, 0x03},
{0xfe, 0x04},
{0x28, 0x86},//84
{0x29, 0x86},//84
{0x2a, 0x86},//84
{0x2b, 0x68},//84
{0x2c, 0x68},//84
{0x2d, 0x68},//84
{0x2e, 0x68},//83
{0x2f, 0x68},//82
{0x30, 0x4f},//82
{0x31, 0x68},//82
{0x32, 0x67},//82
{0x33, 0x66},//82
{0x34, 0x66},//82
{0x35, 0x66},//82
{0x36, 0x66},//64
{0x37, 0x66},//68
{0x38, 0x62},
{0x39, 0x62},
{0x3a, 0x62},
{0x3b, 0x62},
{0x3c, 0x62},
{0x3d, 0x62},
{0x3e, 0x62},
{0x3f, 0x62},
/****DVP & MIPI****/
{0xfe, 0x01},
{0x9a, 0x06},//[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
{0xfe, 0x00},
{0x7b, 0x2a},//[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
{0x23, 0x2d},//[3]rst_rc [2:1]drv_sync [0]pwd_rc
{0xfe, 0x03},
{0x01, 0x27},//20//27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
{0x02, 0x56},//[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
{0x03, 0xb6},//b2//b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
{0x12, 0x80},
{0x13, 0x07},//LWC
{0x15, 0x12},//[1:0]clk_lane_mode
{0xfe, 0x00},
{0x3e, 0x91},//40//91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]mipi_set_auto_disable [2]RAW8_mode [1]ine_sync_mode [0]double_lane_en

};

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
     {0x17, 0x80},    // bit[1:0]
};

/////////////////////////////////////////////////////////////////
//       @@@@@@                                                                                    //
//                 @@                                                                                    //
//             @@@                                                                                      //
//       @       @@                                                                                    //
//         @@@@                                                                                        //
//                                                                                                          //
//      Step 3 --  complete camera features                                              //
//                                                                                                         //
//                                                                                                         //
//  camera set EV, MWB, orientation, contrast, sharpness                          //
//   , saturation, and Denoise can work correctly.                                     //
//                                                                                                          //
/////////////////////////////////////////////////////////////////


static I2C_ARRAY mirr_flip_table[] =
{
    #if 0
     {0x17, 0x83},    // bit[1:0]

     {0x17, 0x82},    // bit[1:0]

     {0x17, 0x81},    // bit[1:0]

     {0x17, 0x80},    // bit[1:0]
    #endif
     {0x17, 0x80},    // bit[1:0]

     {0x17, 0x81},    // bit[1:0]

     {0x17, 0x82},    // bit[1:0]

     {0x17, 0x83},    // bit[1:0]

};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;


static int g_sensor_ae_min_gain = 1024;//1280;

static I2C_ARRAY gain_reg[] = {
    {0xfe, 0x00},
    {0xb4, 0x00},
    {0xb3, 0x00},
    {0xb8, 0x00},
    {0xb9, 0x00},
    {0xb1, 0x00},
    {0xb2, 0x00},
};


static I2C_ARRAY expo_reg[] = {
    {0x04, 0xd0},
    {0x03, 0x05},
};


static I2C_ARRAY vts_reg[] = {
    {0x41, 0x05},
    {0x42, 0xdc},

};

I2C_ARRAY temperature_reg[] = {
    {0x0c, 0x30},
    {0x0d, 0x30},
    {0x0e, 0x30},
    {0x0f, 0x30},
};

static Gain_ARRAY gain_table[]={
                                {1024 , 0x00, 0x00,0x01,0x00},
                                {1230 , 0x00, 0x10,0x01,0x0c},
                                {1440 , 0x00, 0x20,0x01,0x1b},
                                {1730 , 0x00, 0x30,0x01,0x2c},
                                {2032 , 0x00, 0x40,0x01,0x3f},
                                {2380 , 0x00, 0x50,0x02,0x16},
                                {2880 , 0x00, 0x60,0x02,0x35},
                                {3460 , 0x00, 0x70,0x03,0x16},
                                {4080 , 0x00, 0x80,0x04,0x02},
                                {4800 , 0x00, 0x90,0x04,0x31},
                                {5776 , 0x00, 0xa0,0x05,0x32},
                                {6760 , 0x00, 0xb0,0x06,0x35},
                                {8064 , 0x00, 0xc0,0x08,0x04},
                                {9500 , 0x00, 0x5a,0x09,0x19},
                                {11552, 0x00, 0x83,0x0b,0x0f},
                                {13600, 0x00, 0x93,0x0d,0x12},
                                {16132, 0x00, 0x84,0x10,0x00},
                                {18912, 0x00, 0x94,0x12,0x3a},
                                {22528, 0x01, 0x2c,0x1a,0x02},
                                {27036, 0x01, 0x3c,0x1b,0x20},
                                {32340, 0x00, 0x8c,0x20,0x0f},
                                {38256, 0x00, 0x9c,0x26,0x07},
                                {45600, 0x02, 0x64,0x36,0x21},
                                {53912, 0x02, 0x74,0x37,0x3a},
                                {63768, 0x00, 0xc6,0x3d,0x02},
                                {76880, 0x00, 0xdc,0x3f,0x3f},
                                {92300, 0x02, 0x85,0x3f,0x3f},
                                {108904, 0x02, 0x95,0x3f,0x3f},
                                {123568, 0x00, 0xce,0x3f,0x3f},
};

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
#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
//#define SENSOR_DMSG(args...) LOGE(args)
#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
#undef SENSOR_NAME
#define SENSOR_NAME gc2053


#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

static int cus_camsensor_release_handle(ms_cus_sensor *handle);

/////////////////// sensor hardware dependent //////////////
#if 0
static int ISP_config_io(ms_cus_sensor *handle) {
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s]", __FUNCTION__);

    sensor_if->HsyncPol(handle, handle->HSYNC_POLARITY);
    sensor_if->VsyncPol(handle, handle->VSYNC_POLARITY);
    sensor_if->ClkPol(handle, handle->PCLK_POLARITY);
    sensor_if->BayerFmt(handle, handle->bayer_id);
    sensor_if->DataBus(handle, handle->sif_bus);

    sensor_if->DataPrecision(handle, handle->data_prec);
    sensor_if->FmtConv(handle,  handle->data_mode);
    return SUCCESS;
}
#endif

//static u32 timeGetTimeU(void)
//{
//    CamOsTimespec_t tRes;
//    CamOsGetMonotonicTime(&tRes);
//    return (tRes.nSec * 1000000)+(tRes.nNanoSec/1000);
//}
//static u32 TStart = 0;

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //TStart = timeGetTimeU();
    /*PAD and CSI*/
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);   ///???
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);  ///???
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);    //=========   ????

    /*Power ON*/
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);

    /*Reset PIN*/
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(1000);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    /*MCLK ON*/
    sensor_if->MCLK(idx, 1, handle->mclk);
     SENSOR_USLEEP(5000);
    //CamOsPrintf("pCus_poweron = %d us \n",timeGetTimeU()-TStart);
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_USLEEP(1000);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    return SUCCESS;
}

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    int i,n;
    int table_length= ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];
    SensorReg_Write(0xfe,0x00);

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

      if( SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
          break;
      else
          continue;
    }

    //convert sensor id to u32 format
    for(i=0;i<table_length;++i)
    {
      if( id_from_sensor[i].data != Sensor_id_table[i].data )
        return FAIL;
      //*id = id_from_sensor[i].data;
      *id = ((*id)+ id_from_sensor[i].data)<<8;
    }

    *id >>= 8;
    SENSOR_DMSG("[%s]gc2053 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int gc2053_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
//static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);
    //TStart = timeGetTimeU();

        for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
        {
            if(Sensor_init_table[i].reg==0xffff)
            {
                msleep(Sensor_init_table[i].data);
            }
            else
            {
                cnt = 0;
                while(SensorReg_Write(Sensor_init_table[i].reg,Sensor_init_table[i].data) != SUCCESS)
                {
                    cnt++;
                    //SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                    if(cnt>=10)
                    {
                        //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                        return FAIL;
                    }
                    msleep(10);
                }
            }
        }

/*
        for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
        {
            if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
            {
                //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                return FAIL;
            }
        }
*/

    pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    pCus_SetAEUSecs(handle, 30000);
    pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    //CamOsPrintf("pCus_init = %d us \n",timeGetTimeU()-TStart);

    return SUCCESS;
}
/*
int pCus_release(ms_cus_sensor *handle)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sensor_if->PCLK(NULL,CUS_PCLK_OFF);
    return SUCCESS;
}
*/

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
    u32 num_res = handle->video_res_supported.num_res;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init;
            break;

        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    short HFlip;//,VFlip;

    SensorReg_Write(0xfe,0x00);//page 0
    SensorReg_Read(0x17, &HFlip);
  //  SensorReg_Read(0x1d, &VFlip);

    if(((HFlip&0x03)==0x03)) //&&((VFlip&0x80)==0))
       *orit = CUS_ORIT_M0F0;
    else if(((HFlip&0x03)==0x02)) //&&((VFlip&0x80)==0))
       *orit = CUS_ORIT_M1F0;
    else if(((HFlip&0x03)==0x01)) //&&((VFlip&0x80)!=0))
       *orit = CUS_ORIT_M0F1;
    else if(((HFlip&0x03)==0x00)) //&&((VFlip&0x80)!=0))
       *orit = CUS_ORIT_M1F1;

    //SENSOR_DMSG("mirror:%x\r\n", HFlip&0x80);
    //SENSOR_DMSG("Flip:%x\r\n", VFlip&0x80);

    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    //gc2053_params *params = (gc2053_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            SensorReg_Write(0xfe,0x00);//page 0
            SensorReg_Write(mirr_flip_table[0].reg,mirr_flip_table[0].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[0].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[0].data;
         //  handle->bayer_id=  CUS_BAYER_BG;
            break;
        case CUS_ORIT_M1F0:
            SensorReg_Write(0xfe,0x00);//page 0
            SensorReg_Write(mirr_flip_table[1].reg,mirr_flip_table[1].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[1].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[1].data;
    //  handle->bayer_id= CUS_BAYER_BG;
            break;
        case CUS_ORIT_M0F1:
            SensorReg_Write(0xfe,0x00);//page 0
            SensorReg_Write(mirr_flip_table[2].reg,mirr_flip_table[2].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[2].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[2].data;
     // handle->bayer_id= CUS_BAYER_GR;
            break;
        case CUS_ORIT_M1F1:
            SensorReg_Write(0xfe,0x00);//page 0
            SensorReg_Write(mirr_flip_table[3].reg,mirr_flip_table[3].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[3].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[3].data;
     // handle->bayer_id= CUS_BAYER_GR;
            break;
        default :
            SensorReg_Write(0xfe,0x00);//page 0
            SensorReg_Write(mirr_flip_table[0].reg,mirr_flip_table[0].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[0].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[0].data;
    //  handle->bayer_id= CUS_BAYER_BG;
            break;
    }
    //SensorReg_Write(0xef,0x01);
 //   SensorReg_Write(0x09,1);

   // params->orient_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    gc2053_params *params = (gc2053_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = vts_reg[0].data << 8 | vts_reg[1].data ;

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return  params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    int vts=0;
    gc2053_params *params = (gc2053_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    //return SUCCESS;

    if(fps>=5 && fps <= 30){
      params->expo.fps = fps;
      params->expo.vts=  (vts_30fps*30)/fps;
    }else if(fps>=5000 && fps <= 30000){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30000)/fps;
    }else{
      //params->expo.vts=vts_30fps;
      //params->expo.fps=30;
      //SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
      return FAIL;
    }

    if ((params->expo.line) > (params->expo.vts)-4) {
        vts = params->expo.line + 4;
    }else
        vts = params->expo.vts;
    vts_reg[0].data = (vts >> 8) & 0x003f;
    vts_reg[1].data = (vts >> 0) & 0x00ff;


    params->dirty = true;
    return SUCCESS;
}

#if 0
static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap) {
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
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status){
    gc2053_params *params = (gc2053_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:
        if(params->dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
            SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
            params->dirty = false;
        }
        break;
        default :
        break;
    }

    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
//    gc2053_params *params = (gc2053_params *)handle->private_data;
    u32 lines = 0;

    lines  = (u32)(expo_reg[0].data&0xff);
    lines |= (u32)(expo_reg[1].data&0x3f)<<8;

    *us = (lines*Preview_line_period)/1000;


    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0;
    gc2053_params *params = (gc2053_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;

    if(lines<1) lines=1;
    if (lines > params->expo.vts-4) {
        vts = lines + 4;
    }else
        vts = params->expo.vts;

    params->expo.line = lines;
    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    expo_reg[0].data = (lines) & 0x00ff;
    expo_reg[1].data = (lines>>8) & 0x003f;
    vts_reg[0].data = (vts >> 8) & 0x003f;
    vts_reg[1].data = (vts >> 0) & 0x00ff;


    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
  //  gc2053_params *params = (gc2053_params *)handle->private_data;

    u32 Again_0 = 1, Dgain_0 = 1;
    u8 i;
    u32  Fine_again = 1024;


    for(i = 0;i < sizeof(gain_table)/sizeof(Gain_ARRAY);i++ )
        {
            if(( gain_table[i].again_reg_val_0 ==gain_reg[1].data) && (gain_table[i].again_reg_val_1 ==gain_reg[2].data))
            {
                Fine_again = gain_table[i].gain;
                break;
            }
        }

     Dgain_0 = (gain_reg[5].data & 0xf) * 64 + (gain_reg[6].data >> 2);
     Again_0 = Fine_again;
     *gain =(u32)((Again_0 * Dgain_0)/64);



    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {

    gc2053_params *params = (gc2053_params *)handle->private_data;
    u32 dgain = 1;
    u8 dgain_0 = 1, dgain_1 = 0;
    u8 i = 0,tmp = 0;
    int temperature_value = 0;
    params->expo.final_gain = gain;

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAX_GAIN  * 1024) {
        gain = SENSOR_MAX_GAIN  * 1024;
    }

    if(0 == times % 4)
    {
    SensorReg_Write(0xfe, 0x04);
    SensorReg_Read(0x0c,&temperature_reg[0].data);
    SensorReg_Read(0x0d,&temperature_reg[1].data);
    SensorReg_Read(0x0e,&temperature_reg[2].data);
    SensorReg_Read(0x0f,&temperature_reg[3].data);
    SensorReg_Write(0xfe, 0x00);
     if(times > 20)
      {
          times = 0;
      }
    }
    temperature_value = (temperature_reg[0].data + temperature_reg[1].data + temperature_reg[2].data + temperature_reg[3].data) >> 2;

        if(temperature_value >= 0x64)
        {
            if(gain > gain_table[16].gain)
            {
                tmp = 16;
                blk_flag=2;
            }
            else
            {
              for(i = 0;i < 16;i++ )
              {
                if((gain >= gain_table[i].gain) && (gain < gain_table[i + 1].gain))
                {
                    tmp = i;
                    blk_flag=2;
                    break;
                }
              }
            }
         }
         else if(temperature_value <= 0x18)
        {

                for(i = 0;i < sizeof(gain_table)/sizeof(Gain_ARRAY) - 1;i++ )
                {
                    if((gain >= gain_table[i].gain) && (gain < gain_table[i + 1].gain))
                    {
                        tmp = i;
                        blk_flag=1;
                        break;
                    }
                    else
                    {
                        blk_flag=1;
                        tmp = sizeof(gain_table)/sizeof(Gain_ARRAY) -1;
                    }
                }
        }
            else
        {
            if(blk_flag==2)
            {
                if(gain >= gain_table[16].gain)
                {
                //gain = gain_table[16].gain;
                tmp = 16;
                }
               else
              {
                for(i = 0;i < 16;i++ )
                {
                    if((gain >= gain_table[i].gain) && (gain < gain_table[i + 1].gain))
                    {
                    tmp = i;
                    break;
                   }
                }
              }
            }
            if(blk_flag==1)
            {

                for(i = 0;i < sizeof(gain_table)/sizeof(Gain_ARRAY) - 1;i++ )
                {
                    if((gain >= gain_table[i].gain) && (gain < gain_table[i + 1].gain))
                    {
                        tmp = i;
                        break;
                    }
                    else
                    {
                        blk_flag=1;
                        tmp = sizeof(gain_table)/sizeof(Gain_ARRAY) - 1;
                    }
                }
            }
        }


    dgain =(gain*64)/(gain_table[tmp].gain);
    dgain_0 = (dgain)>>6;
    dgain_1 =(dgain & 0x3f)<<2;
    gain_reg[1].data  = gain_table[tmp].again_reg_val_0;
    gain_reg[2].data  = gain_table[tmp].again_reg_val_1;
    gain_reg[3].data  = gain_table[tmp].again_reg_val_2;
    gain_reg[4].data  = gain_table[tmp].again_reg_val_3;

    gain_reg[5].data  = dgain_0;
    gain_reg[6].data  = dgain_1;


    SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,gain_reg[1].data,gain_reg[2].data);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;//30
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min =handle->sat_mingain;
    *max = SENSOR_MAX_GAIN*1024;
    return SUCCESS;
}

static int gc2053_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min =  Preview_line_period*1;//2
    info->step = Preview_line_period*1;//2
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
  //  u32 i, j;
/*
    for(i=0,j=0;i< num ;i++,j+=2){
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }
    //SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);
*/
    return SUCCESS;
}

static int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    gc2053_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(gc2053_params));
    params = (gc2053_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"gc2053_MIPI");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt    = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_NONE;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0; //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    SENSOR_DMSG(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");

    // i2c

    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED;

    //polarity
    /////////////////////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error
    /////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;
    handle->ae_shutter_delay    = 2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 1;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;


    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init    ;

    handle->pCus_sensor_poweron     = pCus_poweron ;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID   ;

    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien          = pCus_GetOrien      ;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien      ;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS      ;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS      ;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = gc2053_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    //handle->pCus_sensor_GetAETrigger_mode      = pCus_GetAETrigger_mode;
    //handle->pCus_sensor_SetAETrigger_mode      = pCus_SetAETrigger_mode;
    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;

    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

     //sensor calibration
//    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = gc2053_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line = 100;
    params->dirty = false;
    params->orient_dirty = false;
    return SUCCESS;
}

static int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_OFF);
    //sensor_if->SetCSI_Clk(handle,CUS_CSI_CLK_DISABLE);
    if (handle && handle->private_data) {
        SENSOR_DMSG("[%s] release handle, handle %x, private data %x",
                __FUNCTION__,
                (int)handle,
                (int)handle->private_data);
        CamOsMemRelease(handle->private_data);
        handle->private_data = NULL;
    }
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END(  gc2053,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL
                         );

