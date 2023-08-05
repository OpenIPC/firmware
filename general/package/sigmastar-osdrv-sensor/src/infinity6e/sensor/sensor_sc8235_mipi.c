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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC8238_HDR);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (4)
//#define SENSOR_MIPI_HDR_MODE (2) //0: Non-HDR mode. 1:Sony DOL mode. 2：CUS_HDR_MODE_DCG
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
//                                                                                                    �@//
//  Fill these #define value and table with correct settings                        //
//      camera can work and show preview on LCM                                 //
//                                                                                                       //
///////////////////////////////////////////////////////////////

#define SENSOR_ISP_TYPE     ISP_EXT                   //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR_DOL PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAPREC_HDR_DOL     CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
//#define SENSOR_MAXGAIN      (15875*315)/10000   /////sensor again 15.875 dgain=31.5
#define SENSOR_MAXGAIN      500                   /////sensor again 15.875 dgain=31.5
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_BAYERID_HDR_DOL      CUS_BAYER_BG//CUS_BAYER_GR
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     80                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 4
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_MCLK_SPEED_HDR_DOL  CUS_CMU_CLK_27MHZ

//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
//#define Preview_line_period                //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
//#define Line_per_second     32727
//#define vts_30fps
u32 Preview_line_period;
u32 vts_30fps;
u32 Preview_line_period_HDR_DOL = 14815;
u32 vts_30fps_HDR_DOL;
#define Preview_WIDTH       3840                    //resolution Width when preview
#define Preview_HEIGHT      2160                    //resolution Height when preview
#define Preview_MAX_FPS     15                     //fastest preview FPS
#define Preview_MIN_FPS     3                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x60                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif
static int cus_camsensor_release_handle(ms_cus_sensor *handle);
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
static int pCus_SetOrien_HDR(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
//static int sc8238_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info);
#define ABS(a)   ((a)>(0) ? (a) : (-(a)))
static int g_sensor_ae_min_gain = 1024;

CUS_MCLK_FREQ UseParaMclk(void);

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
        u32 half_lines;
        u32 half_line;
        u32 fps;
        u32 preview_fps;
        u32 max_short_exp;
        u32 line;
    } expo;
    struct {
        bool bVideoMode;
        u16 res_idx;
        //        bool binning;
        //        bool scaling;
        CUS_CAMSENSOR_ORIT  orit;
    } res;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_mf;
    bool reg_dirty;
    CUS_CAMSENSOR_ORIT cur_orien;
} sc8238_params;
// set sensor ID address and data,

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x82},
    {0x3108, 0x35},
};

I2C_ARRAY Sensor_init_table_4lane_4K30fps[] =
{
//0x02_Apollo_30fps_10bit_4lane_708.75Mbps_3840x2160(HTS=4200,VTS=2250).ini
{0x0103,0x01},
{0x0100,0x00},
{0x336c,0xc2},
{0x3624,0x45},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x5799,0x06},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e0e,0x09},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3018,0x72},
{0x3019,0x00},
{0x3031,0x0a},
{0x3037,0x20},
{0x4501,0xa4},
{0x4509,0x10},
{0x4837,0x1c},
{0x3366,0x92},
{0x3314,0x94},
{0x334c,0x10},
{0x3e09,0x40},
{0x3e08,0x03},
{0x3637,0x52},
{0x3631,0x80},
{0x3308,0x20},
{0x335d,0x60},
{0x337f,0x33},
{0x3368,0x07},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x08},
{0x330e,0x58},
{0x33af,0x48},
{0x3638,0x0a},
{0x3306,0xa8},
{0x3309,0x68},
{0x331f,0x59},
{0x330d,0x28},
{0x339e,0x24},
{0x33aa,0x24},
{0x3332,0x24},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x3628,0x83},
{0x3633,0x53},
{0x3630,0x80},
{0x3622,0xf7},
{0x363d,0x01},
{0x363e,0x00},
{0x363a,0x88},
{0x3e26,0x40},
{0x360f,0x01},
{0x367a,0x48},
{0x367b,0x78},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3670,0x4a},
{0x367c,0x48},
{0x367d,0x78},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x369c,0x48},
{0x369d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x36a2,0x48},
{0x36a3,0x78},
{0x3699,0x88},
{0x369a,0x9f},
{0x369b,0x9f},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x3364,0x16},
{0x3301,0x1c},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3241,0x00},
{0x3243,0x03},
{0x3271,0x1c},
{0x3273,0x1f},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3981,0x70},
{0x3984,0x20},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3982,0x00},
{0x3983,0x00},
{0x3980,0x71},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39c5,0x30},
{0x330b,0x48},
{0x320c,0x08},
{0x320d,0x34},
{0x320e,0x08}, 
{0x320f,0xca},
{0x3635,0x02},
{0x3902,0xc5},
{0x3038,0x44},
{0x3632,0x98},
{0x3e14,0x31},
{0x3e1b,0x3a},
{0x3248,0x04},
{0x3901,0x00},
{0x3904,0x18},
{0x3987,0x0b},
{0x363b,0x06},
{0x3905,0xd8},
{0x301f,0x02},
{0x3203,0x08},
{0x3207,0x87},
{0x3213,0x08},
{0x3e00,0x01},
{0x3e01,0x18},
{0x3e02,0xa0},
{0x3641,0x0c},
{0x36f9,0x57},
{0x36fa,0x39},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x36e9,0x53},
{0x36ea,0x39},
{0x36eb,0x06},
{0x36ec,0x05},
{0x36ed,0x24},
{0x3641,0x00},
{0x0100,0x01},
};

I2C_ARRAY Sensor_init_table_4lane_4K20fps[] =
{
//cleaned_0x32_SC8235_MIPI_27Minput_4lane_10bit_472.5Mbps_3840x2160_20fps.ini
{0x0103,0x01},
{0x0100,0x00},
{0x3018,0x72},
{0x3019,0x00},
{0x301f,0x32},
{0x3031,0x0a},
{0x3037,0x20},
{0x3038,0x44},
{0x320c,0x08},
{0x320d,0x34},
{0x3241,0x00},
{0x3243,0x03},
{0x3248,0x04},
{0x3271,0x1c},
{0x3273,0x1f},
{0x3301,0x18},
{0x3306,0x88},
{0x3308,0x20},
{0x3309,0x68},
{0x330b,0x70},
{0x330d,0x28},
{0x330e,0x48},
{0x3314,0x94},
{0x331f,0x59},
{0x3332,0x24},
{0x334c,0x10},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x335d,0x60},
{0x3364,0x16},
{0x3366,0x92},
{0x3367,0x08},
{0x3368,0x07},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x336c,0xc2},
{0x337f,0x33},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3399,0x18},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x339e,0x24},
{0x33aa,0x24},
{0x33af,0x48},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x360f,0x01},
{0x3622,0xf7},
{0x3624,0x45},
{0x3628,0x83},
{0x3630,0x80},
{0x3631,0x80},
{0x3632,0xa8},
{0x3633,0x53},
{0x3635,0x02},
{0x3637,0x52},
{0x3638,0x0a},
{0x363a,0x88},
{0x363b,0x06},
{0x363d,0x01},
{0x363e,0x00},
{0x3670,0x4a},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x367a,0x48},
{0x367b,0x78},
{0x367c,0x48},
{0x367d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x3699,0x9f},
{0x369a,0x9f},
{0x369b,0x9f},
{0x369c,0x48},
{0x369d,0x78},
{0x36a2,0x48},
{0x36a3,0x78},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x3901,0x00},
{0x3902,0xc5},
{0x3904,0x18},
{0x3905,0xd8},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3980,0x71},
{0x3981,0x70},
{0x3982,0x00},
{0x3983,0x00},
{0x3984,0x20},
{0x3987,0x0b},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39c5,0x30},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3e00,0x01},
{0x3e01,0x18},
{0x3e02,0xa0},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e0e,0x09},
{0x3e14,0x31},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e1b,0x3a},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x3e26,0x40},
{0x4501,0xa4},
{0x4509,0x10},
{0x4837,0x2a},
{0x5799,0x06},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3641,0x0c},
{0x36e9,0x56},
{0x36ea,0x39},
{0x36eb,0x0e},
{0x36ec,0x15},
{0x36ed,0x14},
{0x36f9,0x11},
{0x36fa,0x39},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x3641,0x00},
{0x0100,0x01},
};

I2C_ARRAY Sensor_init_table_4lane_4K15fps[] =
{
//0x20_Apollo_MIPI_27Minput_4lane_10bit_421.875Mbps_3840x2160_15fps_(hts=5000,vts=2250).ini
{0x0103,0x01},
{0x0100,0x00},
{0x3018,0x72},
{0x3019,0x00},
{0x301f,0x20},
{0x3031,0x0a},
{0x3037,0x20},
{0x3038,0x44},
{0x320c,0x09},
{0x320d,0xc4},
{0x320e,0x08},
{0x320f,0xca},
{0x3241,0x00},
{0x3243,0x03},
{0x3248,0x04},
{0x3271,0x1c},
{0x3273,0x1f},
{0x3301,0x1c},
{0x3306,0xc0},
{0x3309,0x40},
{0x330b,0xc0},
{0x330e,0x60},
{0x3314,0x94},
{0x331f,0x31},
{0x334c,0x10},
{0x335d,0x60},
{0x3364,0x16},
{0x3366,0x92},
{0x3367,0x08},
{0x3368,0x09},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x336c,0xc2},
{0x337f,0x33},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x33af,0x24},
{0x33e0,0xa0},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x360f,0x01},
{0x3622,0xf7},
{0x3624,0x45},
{0x3628,0x83},
{0x3630,0x80},
{0x3631,0x80},
{0x3632,0xa8},
{0x3633,0x22},
{0x3635,0x02},
{0x3637,0x29},
{0x3638,0x08},
{0x363a,0x88},
{0x363b,0x06},
{0x363d,0x01},
{0x363e,0x00},
{0x3670,0x4a},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x367a,0x48},
{0x367b,0x78},
{0x367c,0x48},
{0x367d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x3699,0x9f},
{0x369a,0x9f},
{0x369b,0x9f},
{0x369c,0x48},
{0x369d,0x78},
{0x36a2,0x48},
{0x36a3,0x78},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x3901,0x00},
{0x3902,0xc5},
{0x3904,0x18},
{0x3905,0xd8},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3980,0x71},
{0x3981,0x70},
{0x3982,0x00},
{0x3983,0x00},
{0x3984,0x20},
{0x3987,0x0b},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x03},
{0x3995,0xff},
{0x3996,0x00},
{0x3997,0x03},
{0x3998,0x00},
{0x3999,0x06},
{0x399a,0x00},
{0x399b,0x0e},
{0x399c,0x00},
{0x399d,0x10},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xee},
{0x39a6,0x03},
{0x39a7,0xf4},
{0x39a8,0x03},
{0x39a9,0xf6},
{0x39aa,0x03},
{0x39ab,0xfd},
{0x39ac,0x00},
{0x39ad,0x04},
{0x39ae,0x00},
{0x39af,0x06},
{0x39b0,0x00},
{0x39b1,0x0e},
{0x39b2,0x00},
{0x39b3,0x1c},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39c5,0x30},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x19},
{0x39e3,0xe9},
{0x39e4,0x0e},
{0x39e5,0x14},
{0x39e6,0x00},
{0x39e7,0x23},
{0x39e8,0x03},
{0x39e9,0xba},
{0x39ea,0x02},
{0x39eb,0x6d},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3e00,0x01},
{0x3e01,0x18},
{0x3e02,0xa0},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e0e,0x09},
{0x3e14,0x31},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e1b,0x3a},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x3e26,0x40},
{0x3f00,0x4a},
{0x4501,0xb4},
{0x4509,0x20},
{0x4837,0x2f},
{0x5799,0x06},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3641,0x0c},
{0x36e9,0x52},
{0x36ea,0xbb},
{0x36eb,0x14},
{0x36ec,0x05},
{0x36ed,0x14},
{0x36f9,0x52},
{0x36fa,0xbb},
{0x36fb,0x14},
{0x36fc,0x10},
{0x36fd,0x14},
{0x3641,0x00},
{0x0100,0x01},
};

I2C_ARRAY Sensor_init_table_4lane_5M30fps[] =
{
//cleaned_0x10_SC8235_MIPI_4lane_10bit_708.75Mbps_2592x1944_4200x2250_30fps.ini
{0x0103,0x01},
{0x0100,0x00},
{0x3018,0x72},
{0x3019,0x00},
{0x301f,0x10},
{0x3031,0x0a},
{0x3037,0x20},
{0x3038,0x44},
{0x3200,0x02},
{0x3201,0x80},
{0x3202,0x00},
{0x3203,0x78},
{0x3204,0x0c},
{0x3205,0xa7},
{0x3206,0x08},
{0x3207,0x17},
{0x3208,0x0a},
{0x3209,0x20},
{0x320a,0x07},
{0x320b,0x98},
{0x320c,0x08},
{0x320d,0x34},
{0x3210,0x00},
{0x3211,0x04},
{0x3212,0x00},
{0x3213,0x04},
{0x3241,0x00},
{0x3243,0x03},
{0x3248,0x04},
{0x3271,0x1c},
{0x3273,0x1f},
{0x3301,0x1c},
{0x3306,0xa8},
{0x3308,0x20},
{0x3309,0x68},
{0x330b,0x48},
{0x330d,0x28},
{0x330e,0x58},
{0x3314,0x94},
{0x331f,0x59},
{0x3332,0x24},
{0x334c,0x10},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x335d,0x60},
{0x3364,0x16},
{0x3366,0x92},
{0x3367,0x08},
{0x3368,0x07},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x336c,0xc2},
{0x337f,0x33},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x339e,0x24},
{0x33aa,0x24},
{0x33af,0x48},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x360f,0x01},
{0x3622,0xf7},
{0x3624,0x45},
{0x3628,0x83},
{0x3630,0x80},
{0x3631,0x80},
{0x3632,0xa8},
{0x3633,0x53},
{0x3635,0x02},
{0x3637,0x52},
{0x3638,0x0a},
{0x363a,0x88},
{0x363b,0x06},
{0x363d,0x01},
{0x363e,0x00},
{0x3670,0x4a},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x367a,0x48},
{0x367b,0x78},
{0x367c,0x48},
{0x367d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x3699,0x9f},
{0x369a,0x9f},
{0x369b,0x9f},
{0x369c,0x48},
{0x369d,0x78},
{0x36a2,0x48},
{0x36a3,0x78},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x3901,0x00},
{0x3902,0xc5},
{0x3904,0x18},
{0x3905,0xd8},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3980,0x71},
{0x3981,0x70},
{0x3982,0x00},
{0x3983,0x00},
{0x3984,0x20},
{0x3987,0x0b},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39c5,0x30},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3e00,0x01},
{0x3e01,0x18},
{0x3e02,0xa0},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e0e,0x09},
{0x3e14,0x31},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e1b,0x3a},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x3e26,0x40},
{0x4501,0xa4},
{0x4509,0x10},
{0x4837,0x1c},
{0x5799,0x06},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3641,0x0c},
{0x36e9,0x53},
{0x36ea,0x39},
{0x36eb,0x06},
{0x36ec,0x05},
{0x36ed,0x24},
{0x36f9,0x57},
{0x36fa,0x39},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x3641,0x00},
{0x0100,0x01},
};

I2C_ARRAY Sensor_init_table_4lane_5MP3_30fps[] =
{
//new_special_0x02_FT_SC8235_MIPI_27Minput_4lane_10bit_708.75Mbps_3072x1728_30fps.ini
{0x0103,0x01},
{0x0100,0x00},
{0x36e9,0x80},
{0x36f9,0x80},
{0x336c,0xc2},
{0x3624,0x45},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x5799,0x06},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e0e,0x09},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3018,0x72},
{0x3019,0x00},
{0x3031,0x0a},
{0x3037,0x20},
{0x4501,0xa4},
{0x4509,0x10},
{0x4837,0x1c},
{0x3366,0x92},
{0x3314,0x94},
{0x334c,0x10},
{0x3e09,0x40},
{0x3e08,0x03},
{0x3637,0x52},
{0x3631,0x80},
{0x3308,0x20},
{0x320c,0x08},
{0x320d,0x34},
{0x335d,0x60},
{0x337f,0x33},
{0x3368,0x07},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x08},
{0x330e,0x58},
{0x33af,0x48},
{0x3638,0x0a},
{0x3306,0xa8},
{0x3309,0x68},
{0x331f,0x59},
{0x330d,0x28},
{0x339e,0x24},
{0x33aa,0x24},
{0x3332,0x24},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x3628,0x83},
{0x3633,0x53},
{0x3630,0x80},
{0x3622,0xf7},
{0x363d,0x01},
{0x363e,0x00},
{0x363a,0x88},
{0x3e26,0x40},
{0x360f,0x01},
{0x367a,0x48},
{0x367b,0x78},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3670,0x4a},
{0x367c,0x48},
{0x367d,0x78},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x369c,0x48},
{0x369d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x36a2,0x48},
{0x36a3,0x78},
{0x3699,0x88},
{0x369a,0x9f},
{0x369b,0x9f},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x3364,0x16},
{0x3301,0x1c},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3241,0x00},
{0x3243,0x03},
{0x3271,0x1c},
{0x3273,0x1f},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3981,0x70},
{0x3984,0x20},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3982,0x00},
{0x3983,0x00},
{0x3980,0x71},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39c5,0x30},
{0x330b,0x48},
{0x36f9,0x57},
{0x36fa,0x39},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x36e9,0x53},
{0x36ea,0x39},
{0x36eb,0x06},
{0x36ec,0x05},
{0x36ed,0x24},
{0x3635,0x02},
{0x3902,0xc5},
{0x3038,0x44},
{0x3632,0x98},
{0x3e14,0x31},
{0x3e1b,0x3a},
{0x3248,0x04},
{0x3901,0x00},
{0x3904,0x18},
{0x3987,0x0b},
{0x363b,0x06},
{0x3905,0xd8},
{0x301f,0x02},
{0x3200,0x01},
{0x3201,0x88},
{0x3202,0x00},
{0x3203,0xda},
{0x3204,0x0d},
{0x3205,0x97},
{0x3206,0x07},
{0x3207,0xa9},
{0x3208,0x0c},
{0x3209,0x00},
{0x320a,0x06},
{0x320b,0xc0},
{0x3210,0x00},
{0x3211,0x08},
{0x3212,0x00},
{0x3213,0x08},
{0x3e00,0x01},
{0x3e01,0x18},
{0x3e02,0xa0},
{0x0100,0x01},
};


I2C_ARRAY Sensor_init_table_4lane_4P8M30fps[] =
{
//cleaned_0x11_SC8235_MIPI_4lane_10bit_708.75Mbps_2944x1656_4200x2250_30fps.ini
{0x0103,0x01},
{0x0100,0x00},
{0x3018,0x72},
{0x3019,0x00},
{0x301f,0x11},
{0x3031,0x0a},
{0x3037,0x20},
{0x3038,0x44},
{0x3200,0x01},
{0x3201,0xd0},
{0x3202,0x01},
{0x3203,0x08},
{0x3204,0x0d},
{0x3205,0x57},
{0x3206,0x07},
{0x3207,0x87},
{0x3208,0x0b},
{0x3209,0x80},
{0x320a,0x06},
{0x320b,0x78},
{0x320c,0x08},
{0x320d,0x34},
{0x3210,0x00},
{0x3211,0x04},
{0x3212,0x00},
{0x3213,0x04},
{0x3241,0x00},
{0x3243,0x03},
{0x3248,0x04},
{0x3271,0x1c},
{0x3273,0x1f},
{0x3301,0x1c},
{0x3306,0xa8},
{0x3308,0x20},
{0x3309,0x68},
{0x330b,0x48},
{0x330d,0x28},
{0x330e,0x58},
{0x3314,0x94},
{0x331f,0x59},
{0x3332,0x24},
{0x334c,0x10},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x335d,0x60},
{0x3364,0x16},
{0x3366,0x92},
{0x3367,0x08},
{0x3368,0x07},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x336c,0xc2},
{0x337f,0x33},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x339e,0x24},
{0x33aa,0x24},
{0x33af,0x48},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x360f,0x01},
{0x3622,0xf7},
{0x3624,0x45},
{0x3628,0x83},
{0x3630,0x80},
{0x3631,0x80},
{0x3632,0xa8},
{0x3633,0x53},
{0x3635,0x02},
{0x3637,0x52},
{0x3638,0x0a},
{0x363a,0x88},
{0x363b,0x06},
{0x363d,0x01},
{0x363e,0x00},
{0x3670,0x4a},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x367a,0x48},
{0x367b,0x78},
{0x367c,0x48},
{0x367d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x3699,0x9f},
{0x369a,0x9f},
{0x369b,0x9f},
{0x369c,0x48},
{0x369d,0x78},
{0x36a2,0x48},
{0x36a3,0x78},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x3901,0x00},
{0x3902,0xc5},
{0x3904,0x18},
{0x3905,0xd8},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3980,0x71},
{0x3981,0x70},
{0x3982,0x00},
{0x3983,0x00},
{0x3984,0x20},
{0x3987,0x0b},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39c5,0x30},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3e00,0x01},
{0x3e01,0x18},
{0x3e02,0xa0},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e0e,0x09},
{0x3e14,0x31},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e1b,0x3a},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x3e26,0x40},
{0x4501,0xa4},
{0x4509,0x10},
{0x4837,0x1c},
{0x5799,0x06},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3641,0x0c},
{0x36e9,0x53},
{0x36ea,0x39},
{0x36eb,0x06},
{0x36ec,0x05},
{0x36ed,0x24},
{0x36f9,0x57},
{0x36fa,0x39},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x3641,0x00},
{0x0100,0x01},

};


I2C_ARRAY Sensor_init_table_4lane_3P6M30fps[] =
{
//cleaned_0x1e_SC8235_MIPI_27Minput_4lane_10bit_583.2Mbps_2560x1440_30fps_2592x1500.ini
{0x0103,0x01},
{0x0100,0x00},
{0x3018,0x72},
{0x3019,0x00},
{0x301f,0x1e},
{0x3031,0x0a},
{0x3037,0x20},
{0x3038,0x44},
{0x3200,0x02},
{0x3201,0x88},
{0x3202,0x02},
{0x3203,0xd8},
{0x3204,0x0c},
{0x3205,0x8f},
{0x3206,0x08},
{0x3207,0x7f},
{0x3208,0x0a},
{0x3209,0x00},
{0x320a,0x05},
{0x320b,0xa0},
{0x320c,0x0a},
{0x320d,0x20},
{0x320e,0x05},
{0x320f,0xdc},
{0x3210,0x00},
{0x3211,0x04},
{0x3212,0x00},
{0x3213,0x04},
{0x3241,0x00},
{0x3243,0x03},
{0x3248,0x04},
{0x3271,0x1c},
{0x3273,0x1f},
{0x3301,0x1c},
{0x3306,0xc8},
{0x3309,0x40},
{0x330b,0xc0},
{0x330e,0x60},
{0x3314,0x94},
{0x331f,0x31},
{0x334c,0x10},
{0x335d,0x60},
{0x3364,0x16},
{0x3366,0x92},
{0x3367,0x08},
{0x3368,0x09},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x336c,0xc2},
{0x337f,0x33},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x33af,0x24},
{0x33e0,0xa0},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x360f,0x01},
{0x3622,0xf7},
{0x3624,0x45},
{0x3628,0x83},
{0x3630,0x80},
{0x3631,0x80},
{0x3632,0xa8},
{0x3633,0x22},
{0x3635,0x02},
{0x3637,0x29},
{0x3638,0x08},
{0x363a,0x88},
{0x363b,0x06},
{0x363d,0x01},
{0x363e,0x00},
{0x3670,0x4a},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x367a,0x48},
{0x367b,0x78},
{0x367c,0x48},
{0x367d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x55},
{0x3699,0x9f},
{0x369a,0x9f},
{0x369b,0x9f},
{0x369c,0x48},
{0x369d,0x78},
{0x36a2,0x48},
{0x36a3,0x78},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x3901,0x00},
{0x3902,0xc5},
{0x3904,0x18},
{0x3905,0xd8},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3980,0x71},
{0x3981,0x70},
{0x3982,0x00},
{0x3983,0x00},
{0x3984,0x20},
{0x3987,0x0b},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x03},
{0x3995,0xff},
{0x3996,0x00},
{0x3997,0x03},
{0x3998,0x00},
{0x3999,0x06},
{0x399a,0x00},
{0x399b,0x0e},
{0x399c,0x00},
{0x399d,0x10},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xee},
{0x39a6,0x03},
{0x39a7,0xf4},
{0x39a8,0x03},
{0x39a9,0xf6},
{0x39aa,0x03},
{0x39ab,0xfd},
{0x39ac,0x00},
{0x39ad,0x04},
{0x39ae,0x00},
{0x39af,0x06},
{0x39b0,0x00},
{0x39b1,0x0e},
{0x39b2,0x00},
{0x39b3,0x1c},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39c5,0x30},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x19},
{0x39e3,0xe9},
{0x39e4,0x0e},
{0x39e5,0x14},
{0x39e6,0x00},
{0x39e7,0x23},
{0x39e8,0x03},
{0x39e9,0xba},
{0x39ea,0x02},
{0x39eb,0x6d},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3e00,0x00},
{0x3e01,0xba},
{0x3e02,0xe0},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e0e,0x09},
{0x3e14,0x31},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e1b,0x3a},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x3e26,0x40},
{0x3f00,0x4e},
{0x3f05,0x70},
{0x4501,0xb4},
{0x4509,0x20},
{0x4837,0x22},
{0x5799,0x06},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3641,0x0c},
{0x36e9,0x37},
{0x36ea,0x7d},
{0x36eb,0x14},
{0x36ec,0x05},
{0x36ed,0x14},
{0x36f9,0x37},
{0x36fa,0x7d},
{0x36fb,0x14},
{0x36fc,0x10},
{0x36fd,0x14},
{0x3641,0x00},
{0x0100,0x01},

};


I2C_ARRAY Sensor_init_table_4lane_2M60fps[] =
{
//cleaned_0x29_SC8235_MIPI_27Minput_4lane_10bit_675Mbps_1920x1080_60fps_hbin_vbin_2000x1125.ini
{0x0103,0x01},
{0x0100,0x00},
{0x3018,0x72},
{0x3019,0x00},
{0x301f,0x29},
{0x3031,0x0a},
{0x3037,0x20},
{0x3038,0x44},
{0x3203,0x08},
{0x3207,0x87},
{0x3208,0x07},
{0x3209,0x80},
{0x320a,0x04},
{0x320b,0x38},
{0x320c,0x07},
{0x320d,0xd0},
{0x320e,0x04},
{0x320f,0x65},
{0x3211,0x08},
{0x3213,0x04},
{0x3214,0x31},
{0x3215,0x31},
{0x3220,0x1c},
{0x3241,0x00},
{0x3243,0x03},
{0x3248,0x04},
{0x3271,0x1c},
{0x3273,0x1f},
{0x3301,0x1c},
{0x3306,0xa0},
{0x3308,0x20},
{0x3309,0x68},
{0x330b,0x38},
{0x330d,0x28},
{0x330e,0x58},
{0x3314,0x94},
{0x331f,0x59},
{0x3332,0x24},
{0x334c,0x10},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x335d,0x60},
{0x3364,0x16},
{0x3366,0x92},
{0x3367,0x08},
{0x3368,0x07},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x336c,0xc2},
{0x337f,0x33},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x38},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x38},
{0x339e,0x24},
{0x33aa,0x24},
{0x33af,0x48},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x360f,0x01},
{0x3620,0x48},
{0x3622,0xf7},
{0x3624,0x45},
{0x3628,0x83},
{0x3630,0x80},
{0x3631,0x80},
{0x3632,0xa8},
{0x3633,0x23},
{0x3635,0x02},
{0x3637,0x52},
{0x3638,0x0a},
{0x363a,0x88},
{0x363b,0x06},
{0x363d,0x01},
{0x363e,0x00},
{0x3670,0x4a},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x367a,0x48},
{0x367b,0x78},
{0x367c,0x48},
{0x367d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x3699,0x9f},
{0x369a,0x9f},
{0x369b,0x9f},
{0x369c,0x48},
{0x369d,0x78},
{0x36a2,0x48},
{0x36a3,0x78},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x3901,0x00},
{0x3902,0xc5},
{0x3904,0x18},
{0x3905,0xd8},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3980,0x71},
{0x3981,0x70},
{0x3982,0x00},
{0x3983,0x00},
{0x3984,0x20},
{0x3987,0x0b},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39c5,0x30},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3e00,0x00},
{0x3e01,0x8c},
{0x3e02,0x00},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e0e,0x09},
{0x3e14,0x31},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e1b,0x3a},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x3e26,0x40},
{0x4501,0xa4},
{0x4509,0x10},
{0x450d,0x08},
{0x4837,0x1c},
{0x5799,0x06},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3641,0x0c},
{0x36e9,0x40},
{0x36ea,0x31},
{0x36eb,0x06},
{0x36ec,0x05},
{0x36ed,0x24},
{0x36f9,0x44},
{0x36fa,0x31},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x3641,0x00},
{0x0100,0x01},

};

I2C_ARRAY Sensor_init_table_HDR_DOL_4lane[] =
{
//cleaned_0x23_SC8235_MIPI_27Minput_4lane_10bit_708.75Mbps_3840x2160_hdr(vc)_15fps_4200x4500.ini
{0x0103,0x01},
{0x0100,0x00},
{0x3018,0x72},
{0x3019,0x00},
{0x301f,0x23},
{0x3031,0x0a},
{0x3037,0x20},
{0x3038,0x44},
{0x320c,0x08},
{0x320d,0x34},
{0x320e,0x11},
{0x320f,0x94},
{0x3220,0x50},
{0x3241,0x00},
{0x3243,0x03},
{0x3248,0x04},
{0x3250,0x3f},
{0x3271,0x1c},
{0x3273,0x1f},
{0x3301,0x1c},
{0x3306,0xa8},
{0x3308,0x20},
{0x3309,0x68},
{0x330b,0x48},
{0x330d,0x28},
{0x330e,0x58},
{0x3314,0x98},
{0x331f,0x59},
{0x3332,0x24},
{0x334a,0x18},
{0x334c,0x10},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x335d,0x60},
{0x3360,0x40},
{0x3362,0x72},
{0x3364,0x16},
{0x3366,0x92},
{0x3367,0x08},
{0x3368,0x10},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x336c,0xc2},
{0x336f,0x58},
{0x337f,0x33},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x60},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x60},
{0x339e,0x24},
{0x33aa,0x24},
{0x33af,0x48},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x360f,0x01},
{0x3622,0xf7},
{0x3624,0x45},
{0x3628,0x83},
{0x3630,0x80},
{0x3631,0x80},
{0x3632,0xa8},
{0x3633,0x23},
{0x3635,0x02},
{0x3636,0x11},
{0x3637,0x10},
{0x3638,0x0a},
{0x363a,0x88},
{0x363b,0x06},
{0x363d,0x01},
{0x363e,0x00},
{0x3670,0x4a},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x367a,0x48},
{0x367b,0x78},
{0x367c,0x48},
{0x367d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x3699,0x88},
{0x369a,0x88},
{0x369b,0x88},
{0x369c,0x48},
{0x369d,0x78},
{0x36a2,0x48},
{0x36a3,0x78},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x3901,0x00},
{0x3902,0xc5},
{0x3904,0x18},
{0x3905,0xd8},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3980,0x71},
{0x3981,0x70},
{0x3982,0x00},
{0x3983,0x00},
{0x3984,0x20},
{0x3987,0x0b},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39c5,0x30},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3e00,0x02},
{0x3e01,0x0f},
{0x3e02,0xa0},
{0x3e04,0x20},
{0x3e05,0xc0},
{0x3e06,0x00},
{0x3e07,0x80},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e0e,0x09},
{0x3e10,0x00},
{0x3e11,0x80},
{0x3e12,0x03},
{0x3e13,0x40},
{0x3e14,0x31},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e1b,0x3a},
{0x3e1e,0x76},
{0x3e23,0x01},
{0x3e24,0x0e},
{0x3e25,0x23},
{0x3e26,0x40},
{0x4501,0xa4},
{0x4509,0x10},
{0x4816,0x51},
{0x4837,0x1c},
{0x5799,0x06},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3641,0x0c},
{0x36e9,0x53},
{0x36ea,0x39},
{0x36eb,0x06},
{0x36ec,0x05},
{0x36ed,0x24},
{0x36f9,0x57},
{0x36fa,0x39},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x3641,0x00},
{0x0100,0x01},
};

I2C_ARRAY Sensor_init_table_HDR_DOL_4lane_2M[] =
{
{0x0103,0x01},
{0x0100,0x00},
{0x336c,0xc2},
{0x3624,0x45},
{0x33e1,0x08},
{0x33e2,0x18},
{0x33e3,0x10},
{0x33e4,0x0c},
{0x33e5,0x10},
{0x33e6,0x06},
{0x33e7,0x02},
{0x33e8,0x18},
{0x33e9,0x10},
{0x33ea,0x0c},
{0x33eb,0x10},
{0x33ec,0x04},
{0x33ed,0x02},
{0x33ee,0xa0},
{0x33ef,0x08},
{0x33f4,0x18},
{0x33f5,0x10},
{0x33f6,0x0c},
{0x33f7,0x10},
{0x33f8,0x06},
{0x33f9,0x02},
{0x33fa,0x18},
{0x33fb,0x10},
{0x33fc,0x0c},
{0x33fd,0x10},
{0x33fe,0x04},
{0x33ff,0x02},
{0x5799,0x06},
{0x3e16,0x00},
{0x3e17,0xac},
{0x3e18,0x00},
{0x3e19,0xac},
{0x3e0e,0x09},
{0x3e1e,0x76},
{0x3e25,0x23},
{0x57aa,0x2f},
{0x57ab,0xff},
{0x3018,0x72},
{0x3019,0x00},
{0x3031,0x0a},
{0x3037,0x20},
{0x4501,0xa4},
{0x4509,0x10},
{0x4837,0x1c},
{0x3366,0x92},
{0x334c,0x10},
{0x3631,0x80},
{0x3308,0x20},
{0x335d,0x60},
{0x337f,0x33},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x08},
{0x330e,0x58},
{0x33af,0x48},
{0x3638,0x0a},
{0x3309,0x68},
{0x331f,0x59},
{0x330d,0x28},
{0x339e,0x24},
{0x33aa,0x24},
{0x3332,0x24},
{0x3350,0x24},
{0x3358,0x24},
{0x335c,0x24},
{0x3628,0x83},
{0x3633,0x23},
{0x3630,0x80},
{0x3622,0xf7},
{0x363d,0x01},
{0x363e,0x00},
{0x363a,0x88},
{0x3e26,0x40},
{0x360f,0x01},
{0x367a,0x48},
{0x367b,0x78},
{0x3671,0xf7},
{0x3672,0xf7},
{0x3673,0x17},
{0x3670,0x4a},
{0x367c,0x48},
{0x367d,0x78},
{0x3674,0x80},
{0x3675,0x85},
{0x3676,0xa5},
{0x369c,0x48},
{0x369d,0x78},
{0x3690,0x53},
{0x3691,0x63},
{0x3692,0x54},
{0x36a2,0x48},
{0x36a3,0x78},
{0x3699,0x88},
{0x36bb,0x48},
{0x36bc,0x78},
{0x36c9,0x05},
{0x36ca,0x05},
{0x36cb,0x05},
{0x36d0,0x30},
{0x36d1,0x48},
{0x36d2,0x78},
{0x36cc,0x00},
{0x36cd,0x10},
{0x36ce,0x1a},
{0x3364,0x16},
{0x3301,0x1c},
{0x3393,0x1c},
{0x3394,0x28},
{0x3395,0x38},
{0x3390,0x08},
{0x3391,0x18},
{0x3392,0x38},
{0x3399,0x1c},
{0x339a,0x1c},
{0x339b,0x28},
{0x339c,0x38},
{0x3396,0x08},
{0x3397,0x18},
{0x3398,0x38},
{0x3241,0x00},
{0x3243,0x03},
{0x3271,0x1c},
{0x3273,0x1f},
{0x394c,0x0f},
{0x394d,0x20},
{0x394e,0x08},
{0x394f,0x90},
{0x3981,0x70},
{0x3984,0x20},
{0x39ec,0x08},
{0x39ed,0x00},
{0x3982,0x00},
{0x3983,0x00},
{0x3980,0x71},
{0x39b4,0x0c},
{0x39b5,0x1c},
{0x39b6,0x38},
{0x39b7,0x5b},
{0x39b8,0x50},
{0x39b9,0x38},
{0x39ba,0x20},
{0x39bb,0x10},
{0x39bc,0x0c},
{0x39bd,0x16},
{0x39be,0x21},
{0x39bf,0x36},
{0x39c0,0x3b},
{0x39c1,0x2a},
{0x39c2,0x16},
{0x39c3,0x0c},
{0x39a2,0x03},
{0x39a3,0xe3},
{0x39a4,0x03},
{0x39a5,0xf2},
{0x39a6,0x03},
{0x39a7,0xf6},
{0x39a8,0x03},
{0x39a9,0xfa},
{0x39aa,0x03},
{0x39ab,0xff},
{0x39ac,0x00},
{0x39ad,0x06},
{0x39ae,0x00},
{0x39af,0x09},
{0x39b0,0x00},
{0x39b1,0x12},
{0x39b2,0x00},
{0x39b3,0x22},
{0x39c6,0x07},
{0x39c7,0xf8},
{0x39c9,0x07},
{0x39ca,0xf8},
{0x3990,0x03},
{0x3991,0xfd},
{0x3992,0x03},
{0x3993,0xfc},
{0x3994,0x00},
{0x3995,0x00},
{0x3996,0x00},
{0x3997,0x05},
{0x3998,0x00},
{0x3999,0x09},
{0x399a,0x00},
{0x399b,0x12},
{0x399c,0x00},
{0x399d,0x12},
{0x399e,0x00},
{0x399f,0x18},
{0x39a0,0x00},
{0x39a1,0x14},
{0x39cc,0x00},
{0x39cd,0x1b},
{0x39ce,0x00},
{0x39cf,0x00},
{0x39d0,0x1b},
{0x39d1,0x00},
{0x39e2,0x15},
{0x39e3,0x87},
{0x39e4,0x12},
{0x39e5,0xb7},
{0x39e6,0x00},
{0x39e7,0x8c},
{0x39e8,0x01},
{0x39e9,0x31},
{0x39ea,0x01},
{0x39eb,0xd7},
{0x39c5,0x30},
{0x3214,0x31},
{0x3620,0x48},
{0x3208,0x07},
{0x3209,0x80},
{0x3211,0x08},
{0x450d,0x08},
{0x3215,0x31},
{0x320a,0x04},
{0x320b,0x38},
{0x3641,0x00},
{0x3635,0x02},
{0x3902,0xc5},
{0x3038,0x44},
{0x3e14,0x31},
{0x3e1b,0x3a},
{0x3248,0x04},
{0x3901,0x00},
{0x3904,0x18},
{0x3987,0x0b},
{0x363b,0x06},
{0x3905,0xd8},
{0x301f,0x29},
{0x320c,0x07},
{0x320d,0xd0},
{0x3641,0x0c},
{0x36f9,0x44},
{0x36fa,0x31},
{0x36fb,0x13},
{0x36fc,0x10},
{0x36fd,0x14},
{0x36e9,0x40},
{0x36ea,0x31},
{0x36eb,0x06},
{0x36ec,0x05},
{0x36ed,0x24},
{0x3641,0x00},
{0x3306,0xa0},
{0x330b,0x38},
{0x3203,0x08},
{0x3207,0x87},
{0x3213,0x04},
{0x3802,0x01},
{0x3220,0x5c},
{0x336f,0x58},
{0x4816,0x51},
{0x3250,0x3f},
{0x320e,0x08},
{0x320f,0xca},
{0x3e23,0x00},
{0x3e24,0x8c},
{0x3314,0x98},
{0x334a,0x18},
{0x3636,0x11},
{0x3637,0x10},
{0x3e00,0x01},
{0x3e01,0x06},
{0x3e02,0xc0},
{0x3e04,0x10},
{0x3e05,0x60},
{0x3e06,0x00},
{0x3e07,0x80},
{0x3e08,0x03},
{0x3e09,0x40},
{0x3e10,0x00},
{0x3e11,0x80},
{0x3e12,0x03},
{0x3e13,0x40},
{0x3362,0x72},
{0x3360,0x40},
{0x3368,0x0e},
{0x369a,0x88},
{0x369b,0x88},
{0x3632,0xa8},//2020/06/11
{0x0100,0x01},
};


I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

I2C_ARRAY mirror_reg[] =
{
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
};

I2C_ARRAY mirror_reg_HDR[] =
{
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
    {0x3812, 0x00},
    {0x4505, 0x80},
    {0x3812, 0x30},
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

static I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},
    {0x3e07, 0x00},
    {0x3e08, 0x00|0x03},
    {0x3e09, 0x40}, //low bit, 0x40 - 0x7f, step 1/64

};
static I2C_ARRAY gain_reg_HDR_DOL_SEF[] = {
    {0x3e10, 0x00},
    {0x3e11, 0x00},
    {0x3e12, 0x00|0x03},
    {0x3e13, 0x10}, //low bit, 0x40 - 0x7f, step 1/64
    };

I2C_ARRAY expo_reg[] = {
    {0x3e00, 0x00}, //expo [3:0]
    {0x3e01, 0x30}, // expo[7:0]
    {0x3e02, 0x00}, // expo[7:4]
};
I2C_ARRAY expo_reg_HDR_DOL_SEF[] = {
    {0x3e04, 0x21}, // expo[7:0]
    {0x3e05, 0x00}, // expo[7:4]
};

I2C_ARRAY vts_reg[] = {
    {0x320e, 0x08},
    {0x320f, 0xca}
};
I2C_ARRAY max_short_exp_reg[] = {
    {0x3e23, 0x01},
    {0x3e24, 0x0e}
};

#if 0
CUS_INT_TASK_ORDER def_order = {
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
#define SENSOR_NAME sc8238

#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

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
static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
/*     sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
    } */

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(1000);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);

    // power -> high, reset -> high
    SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    //sensor_if->Set3ATaskOrder(handle, def_order);
    // pure power on
    //ISP_config_io(handle);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(5000);
    //handle->i2c_bus->i2c_open(handle->i2c_bus,&handle->i2c_cfg);

    return SUCCESS;
}

static int pCus_post_init(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
    }
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_USLEEP(1000);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
    sensor_if->MCLK(idx, 0, handle->mclk);

    params->cur_orien = CUS_ORIT_M0F0;

    SENSOR_USLEEP(2000);//mantis:1690203
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

    SensorReg_Write(0xef,0x00);

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
    SENSOR_DMSG("[%s]sc8238 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int sc8238_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);

static int pCus_init_mipi4lane_linear_4K30fps(ms_cus_sensor *handle)
{
    short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_4K30fps);i++)
    {
        if(Sensor_init_table_4lane_4K30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_4K30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_4K30fps[i].reg, Sensor_init_table_4lane_4K30fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    pr_info("[%s]  vts_reg_sef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
    return SUCCESS;
}

static int pCus_init_mipi4lane_linear_4K20fps(ms_cus_sensor *handle)
{
    short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_4K20fps);i++)
    {
        if(Sensor_init_table_4lane_4K20fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_4K20fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_4K20fps[i].reg, Sensor_init_table_4lane_4K20fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }
    //BL control
    //SensorReg_Write(0x3907,0x01);
    //SensorReg_Write(0x3908,0x11);

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    pr_info("[%s]  vts_reg_sef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_mipi4lane_linear_4K15fps(ms_cus_sensor *handle)
{
   // short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_4K15fps);i++)
    {
        if(Sensor_init_table_4lane_4K15fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_4K15fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_4K15fps[i].reg, Sensor_init_table_4lane_4K15fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }
#if 0
    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }
#endif
    //BL control
  //  SensorReg_Write(0x3907,0x01);
  //  SensorReg_Write(0x3908,0x11);

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_mipi4lane_linear_5M30fps(ms_cus_sensor *handle)
{
  //  short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_5M30fps);i++)
    {
        if(Sensor_init_table_4lane_5M30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_5M30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_5M30fps[i].reg, Sensor_init_table_4lane_5M30fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }
#if 0
    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }
#endif
    //BL control
   // SensorReg_Write(0x3907,0x01);
  //  SensorReg_Write(0x3908,0x11);

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

////////
static int pCus_init_mipi4lane_linear_5MP3_30fps(ms_cus_sensor *handle)
{
  //  short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_5MP3_30fps);i++)
    {
        if(Sensor_init_table_4lane_5MP3_30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_5MP3_30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_5MP3_30fps[i].reg, Sensor_init_table_4lane_5MP3_30fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }
#if 0
    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }
#endif
    //BL control
   // SensorReg_Write(0x3907,0x01);
  //  SensorReg_Write(0x3908,0x11);

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}




static int pCus_init_mipi4lane_linear_4P8M30fps(ms_cus_sensor *handle)
{
   // short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_4P8M30fps);i++)
    {
        if(Sensor_init_table_4lane_4P8M30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_4P8M30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_4P8M30fps[i].reg, Sensor_init_table_4lane_4P8M30fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }
#if 0
    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }
#endif
    //BL control
 //   SensorReg_Write(0x3907,0x01);
  //  SensorReg_Write(0x3908,0x11);

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_mipi4lane_linear_3P6M30fps(ms_cus_sensor *handle)
{
   // short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_3P6M30fps);i++)
    {
        if(Sensor_init_table_4lane_3P6M30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_3P6M30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_3P6M30fps[i].reg, Sensor_init_table_4lane_3P6M30fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }
#if 0
    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }
#endif
    //BL control
   // SensorReg_Write(0x3907,0x01);
  //  SensorReg_Write(0x3908,0x11);

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}
static int pCus_init_mipi4lane_linear_2M60fps(ms_cus_sensor *handle)
{
  //  short ver=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_2M60fps);i++)
    {
        if(Sensor_init_table_4lane_2M60fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_2M60fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_2M60fps[i].reg, Sensor_init_table_4lane_2M60fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }
#if 0
    SensorReg_Read(0x3040,&ver);
    if(ver == 0){
        SensorReg_Write(0x36c9,0x01);
        SensorReg_Write(0x36ca,0x00);
        SensorReg_Write(0x36cb,0x00);
    }else if(ver == 1){
        SensorReg_Write(0x36c9,0x05);
        SensorReg_Write(0x36ca,0x05);
        SensorReg_Write(0x36cb,0x05);
    }
#endif
    //BL control
 //   SensorReg_Write(0x3907,0x01);
  //  SensorReg_Write(0x3908,0x11);

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}


static int pCus_init_mipi4lane_HDR_DOL(ms_cus_sensor *handle)
{
  //  short ver=0;
   sc8238_params *params = (sc8238_params *)handle->private_data;
    int i,cnt=0;
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_4lane);i++)
    {
        if(Sensor_init_table_HDR_DOL_4lane[i].reg==0xffff)
        {
            //MsSleep(RTK_MS_TO_TICK(1));//usleep(1000*Sensor_init_table_HDR_DOL_4lane[i].data);
            SENSOR_MSLEEP(Sensor_init_table_HDR_DOL_4lane[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("reg =  %x, data = %x\n", Sensor_init_table_HDR_DOL_4lane[i].reg, Sensor_init_table_HDR_DOL_4lane[i].data);
            while(SensorReg_Write(Sensor_init_table_HDR_DOL_4lane[i].reg,Sensor_init_table_HDR_DOL_4lane[i].data) != SUCCESS)
            {
                cnt++;
                 SENSOR_DMSG("Sensor_init_table_HDR_DOL_4lane -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //printf("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //SensorReg_Read( Sensor_init_table_HDR_DOL_4lane[i].reg, &sen_data );
            //UartSendTrace("sc8238 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HDR_DOL_4lane[i].reg, Sensor_init_table_HDR_DOL_4lane[i].data, sen_data);
        }
    }

    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

     pCus_SetOrien_HDR(handle, params->cur_orien);
    // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
     vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
     vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;

    return SUCCESS;
}

static int pCus_init_mipi4lane_HDR_DOL_2M(ms_cus_sensor *handle)
{
  //  short ver=0;
   sc8238_params *params = (sc8238_params *)handle->private_data;
    int i,cnt=0;
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_4lane_2M);i++)
    {
        if(Sensor_init_table_HDR_DOL_4lane_2M[i].reg==0xffff)
        {
            //MsSleep(RTK_MS_TO_TICK(1));//usleep(1000*Sensor_init_table_HDR_DOL_4lane_2M[i].data);
            SENSOR_MSLEEP(Sensor_init_table_HDR_DOL_4lane_2M[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("reg =  %x, data = %x\n", Sensor_init_table_HDR_DOL_4lane_2M[i].reg, Sensor_init_table_HDR_DOL_4lane_2M[i].data);
            while(SensorReg_Write(Sensor_init_table_HDR_DOL_4lane_2M[i].reg,Sensor_init_table_HDR_DOL_4lane_2M[i].data) != SUCCESS)
            {
                cnt++;
                 SENSOR_DMSG("Sensor_init_table_HDR_DOL_4lane_2M -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //printf("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                //usleep(10*1000);
            }
        }
    }
    //DPC patch
    SensorReg_Write(0x5780,0x57);
    SensorReg_Write(0x5785,0x00);
    SensorReg_Write(0x5786,0x00);
    SensorReg_Write(0x5799,0x07); //20190710

     pCus_SetOrien_HDR(handle, params->cur_orien);
     vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
     vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;

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
    sc8238_params *params = (sc8238_params *)handle->private_data;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {

        case 0: //"3840x2160@15fps"
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_4K15fps;
            vts_30fps=2250;
            params->expo.vts = vts_30fps;
            params->expo.fps = 15;
            Preview_line_period  = 29630;
            break;
        case 1: //"3840x2160@20fps"
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_4K20fps;
            vts_30fps=2250;//3375
            params->expo.vts = vts_30fps;
            params->expo.fps = 20;
            Preview_line_period  = 22222;///14815
            break;
		case 2: //"3072x1728@30fps"
            handle->video_res_supported.ulcur_res = 2;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_5MP3_30fps;
            vts_30fps=2250;//3375
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 14815;///14815
            break;			
        case 3: //"2592x1944@30fps"
            handle->video_res_supported.ulcur_res = 3;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_5M30fps;
            vts_30fps=2250;//3375
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 14815;///14815
            break;
        case 4: //"2944x1656@30fps"
            handle->video_res_supported.ulcur_res = 4;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_4P8M30fps;
            vts_30fps=2250;//3375
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 14815;///14815
            break;
        case 5: //"2560x1440@30fps"
            handle->video_res_supported.ulcur_res = 5;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_3P6M30fps;
            vts_30fps=1500;
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 22222;
            break;
       case 6: //"1920x1080@60fps"
            handle->video_res_supported.ulcur_res = 6;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_2M60fps;
            vts_30fps=1125;
            params->expo.vts = vts_30fps;
            params->expo.fps = 60;
            Preview_line_period  = 14815;
            break;
        case 7: //"3840x2160@30fps"
            handle->video_res_supported.ulcur_res = 7;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear_4K30fps;
            vts_30fps=2250;//3375
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 14815;///14815
            break;
        default:
            break;
    }

    return SUCCESS;
}
static int pCus_SetVideoRes_HDR_DOL(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;
    sc8238_params *params = (sc8238_params *)handle->private_data;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
            handle->pCus_sensor_init = pCus_init_mipi4lane_HDR_DOL;
            }
            vts_30fps_HDR_DOL = 4500;//3375
            Preview_line_period_HDR_DOL  = 14815;///14815
            params->expo.vts = vts_30fps_HDR_DOL;
            params->expo.fps = 15;
            params->expo.max_short_exp=260;
            break;
        case 1:
            handle->video_res_supported.ulcur_res = 1;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
            handle->pCus_sensor_init = pCus_init_mipi4lane_HDR_DOL_2M;
            }
            vts_30fps_HDR_DOL = 2250;//3375
            Preview_line_period_HDR_DOL  = 14815;///14815
            params->expo.vts = vts_30fps_HDR_DOL;
            params->expo.fps = 15;
            params->expo.max_short_exp=130;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    sen_data = mirror_reg[0].data;
    SENSOR_DMSG("mirror:%x\r\n", sen_data);
    switch(sen_data) {
      case 0x00:
        *orit = CUS_ORIT_M0F0;
        break;
      case 0x06:
        *orit = CUS_ORIT_M1F0;
        break;
      case 0x60:
        *orit = CUS_ORIT_M0F1;
        break;
      case 0x66:
        *orit = CUS_ORIT_M1F1;
        break;
      }
      return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
   sc8238_params *params = (sc8238_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    switch(orit)
    {
        case CUS_ORIT_M0F0:
            mirror_reg[0].data = 0x00;
            params->cur_orien = CUS_ORIT_M0F0;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M1F0:
            mirror_reg[0].data = 0x06;
            params->cur_orien = CUS_ORIT_M1F0;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M0F1:
            mirror_reg[0].data = 0x60;
            params->cur_orien = CUS_ORIT_M0F1;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M1F1:
            mirror_reg[0].data = 0x66;
            params->cur_orien = CUS_ORIT_M1F1;
            params->reg_mf = true;
            break;
        default :
            break;
    }
    return SUCCESS;
}

static int pCus_SetOrien_HDR(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
   sc8238_params *params = (sc8238_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    switch(orit)
    {
        case CUS_ORIT_M0F0:
            mirror_reg_HDR[0].data = 0x00;
            mirror_reg_HDR[2].data = 0x80;
            params->cur_orien = CUS_ORIT_M0F0;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M1F0:
            mirror_reg_HDR[0].data = 0x06;
            mirror_reg_HDR[2].data = 0x83;
            params->cur_orien = CUS_ORIT_M1F0;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M0F1:
            mirror_reg_HDR[0].data = 0x60;
            mirror_reg_HDR[2].data = 0x80;
            params->cur_orien = CUS_ORIT_M0F1;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M1F1:
            mirror_reg_HDR[0].data = 0x66;
            mirror_reg_HDR[2].data = 0x83;
            params->cur_orien = CUS_ORIT_M1F1;
            params->reg_mf = true;
            break;
        default :
            break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[0].data << 8) | (vts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts=0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);
    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*max_fps)/fps;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000))/fps;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.line > 2* (params->expo.vts) -10){
        vts = (params->expo.line + 11)/2;
    }else{
        vts = params->expo.vts;
    }
    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
    return SUCCESS;

}

static int pCus_GetFPS_HDR_DOL_SEF(ms_cus_sensor *handle)
{
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[0].data << 8) | (vts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_DOL_SEF(ms_cus_sensor *handle, u32 fps)
{
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    //pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps_HDR_DOL*max_fps)/fps >> 1) << 1;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_sef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps_HDR_DOL*(max_fps*1000))/fps >> 1) << 1;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_sef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else{
        pr_info("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

}
static int pCus_SetFPS_hdr_dol_lef(ms_cus_sensor *handle, u32 fps)
{
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
//  pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps_HDR_DOL*max_fps)/fps >> 1) << 1;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_lef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps_HDR_DOL*(max_fps*1000))/fps >> 1) << 1;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_lef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else{
        pr_info("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

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

    sc8238_params *params = (sc8238_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        break;
        case CUS_FRAME_ACTIVE:
        if(params->reg_mf)
        {
            SensorRegArrayW((I2C_ARRAY*)mirror_reg, ARRAY_SIZE(mirror_reg));
            params->reg_mf = false;
        }
        if(params->reg_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
            SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
            params->reg_dirty = false;
        }
        break;
        default :
        break;
    }
    return SUCCESS;
}
static int pCus_AEStatusNotifyHDR_DOL_LEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    //sc8238_params *params = (sc8238_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        break;
        case CUS_FRAME_ACTIVE:
        break;
        default :
        break;
    }
    return SUCCESS;

}

static int pCus_AEStatusNotifyHDR_DOL_SEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
   sc8238_params *params = (sc8238_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);

             break;
        case CUS_FRAME_ACTIVE:
        if(params->reg_mf)
        {
            SensorRegArrayW((I2C_ARRAY*)mirror_reg_HDR, ARRAY_SIZE(mirror_reg_HDR));
            params->reg_mf = false;
        }
       if(params->reg_dirty)
        {
           SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
           SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
           SensorRegArrayW((I2C_ARRAY*)expo_reg_HDR_DOL_SEF, ARRAY_SIZE(expo_reg_HDR_DOL_SEF));
           SensorRegArrayW((I2C_ARRAY*)gain_reg_HDR_DOL_SEF, ARRAY_SIZE(gain_reg_HDR_DOL_SEF));
           SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
           params->reg_dirty = false;
        }
        break;
        default :
             break;
    }
    return SUCCESS;
}
static int pCus_SetAEUSecsHDR_DOL_LEF(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0,dou_lines = 0,vts = 0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));
    dou_lines = (1000*us)/(Preview_line_period_HDR_DOL*2); // Preview_line_period in ns
    half_lines = 4*dou_lines;
    if(half_lines<8) half_lines=8;
    if (half_lines >  2 * (params->expo.vts-params->expo.max_short_exp-10)) {
        half_lines = 2 * (params->expo.vts-params->expo.max_short_exp-10);
    }
    else
     vts=params->expo.vts;
//  SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;

    expo_reg[0].data = (half_lines>>16) & 0x0f;
    expo_reg[1].data =  (half_lines>>8) & 0xff;
    expo_reg[2].data = (half_lines>>0) & 0xf0;
 //   pr_info("[%s]  expo_reg : %x ,%x , %x\n\n", __FUNCTION__,expo_reg[0].data,expo_reg[1].data,expo_reg[2].data);
    for (i = 0; i < ARRAY_SIZE(expo_reg); i++)
    {
      if (expo_reg[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
     }
    return SUCCESS;

}

static int pCus_SetAEUSecsHDR_DOL_SEF(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0,dou_lines = 0,vts = 0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {
        {0x3e04, 0x21}, // expo[7:0]
        {0x3e05, 0x00}, // expo[7:4]
    };
    memcpy(expo_reg_temp, expo_reg_HDR_DOL_SEF, sizeof(expo_reg_HDR_DOL_SEF));

    dou_lines = (1000*us)/(Preview_line_period_HDR_DOL*2); // Preview_line_period in ns
    half_lines = 4*dou_lines;
    if(half_lines<8) half_lines=8;
    if (half_lines >  2 * (params->expo.max_short_exp-8)) {
        half_lines = 2 * (params->expo.max_short_exp-8);
    }
    else
     vts=params->expo.vts;
//  SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;

    expo_reg_HDR_DOL_SEF[0].data =  (half_lines>>8) & 0xff;
    expo_reg_HDR_DOL_SEF[1].data = (half_lines>>0) & 0xf0;
 //   pr_info("[%s]  expo_reg_HDR_DOL_SEF : %x , %x\n\n", __FUNCTION__,expo_reg_HDR_DOL_SEF[0].data,expo_reg_HDR_DOL_SEF[1].data);
    for (i = 0; i < ARRAY_SIZE(expo_reg_HDR_DOL_SEF); i++)
    {
      if (expo_reg_HDR_DOL_SEF[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
     }
    return SUCCESS;

}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {

  u32 lines = 0;
  lines |= (u32)(expo_reg[0].data&0x0f)<<16;
  lines |= (u32)(expo_reg[1].data&0xff)<<8;
  lines |= (u32)(expo_reg[2].data&0xf0)<<0;
  lines >>= 4;
  *us = (lines*Preview_line_period)/1000/2; //return us

  SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    int i;
    u32 half_lines = 0,vts = 0;
    sc8238_params *params = (sc8238_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<3) half_lines=3;
    if (half_lines >  2 * (params->expo.vts)-10) {
        vts = (half_lines+11)/2;
    }
    else
        vts=params->expo.vts;

    params->expo.line = half_lines;
    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;

    expo_reg[0].data = (half_lines>>16) & 0x0f;
    expo_reg[1].data =  (half_lines>>8) & 0xff;
    expo_reg[2].data = (half_lines>>0) & 0xf0;
    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;

    for (i = 0; i < ARRAY_SIZE(expo_reg); i++)
    {
      if (expo_reg[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
     }
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    int rc = 0;
    u8 Dgain = 1,  Coarse_gain = 1;

    Coarse_gain = ((gain_reg[2].data&0x1C)>>2) +1;
    Dgain = ((gain_reg[0].data&0x0f) + 1);

    *gain = (Coarse_gain*Dgain*(gain_reg[1].data)*(gain_reg[3].data))/2;

    return rc;
}
static int pCus_GetAEGainHDR_DOL_SEF(ms_cus_sensor *handle, u32* gain) {
    int rc = 0;
    u8 Dgain = 1,  Coarse_gain = 1;

    Coarse_gain = ((gain_reg_HDR_DOL_SEF[2].data&0x1C)>>2) +1;
    Dgain = ((gain_reg_HDR_DOL_SEF[0].data&0x0f) + 1);

    *gain = (Coarse_gain*Dgain*(gain_reg_HDR_DOL_SEF[1].data)*(gain_reg_HDR_DOL_SEF[3].data))/2;


    return rc;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Ana_gain = 1;
    u64 Fine_againx64 = 64,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Ana_gain_reg = 0, Fine_again_reg= 0x40,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x40},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 1;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 2;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 4;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x0f;
    }
    else if (gain <=  16256)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x1f;
    }
    else if (gain <  32512)
    {
        Dgain = 1;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
    else if (gain <  65024)
    {
        Dgain = 2;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x01;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
     else if (gain < 127 * 1024)
    {
        Dgain = 4;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x03;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
     else if (gain < 254 * 1024)
    {
        Dgain = 8;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x07;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
     else if (gain <= SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x0f;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }

    if (gain <= 16256)
    {
        Fine_againx64 = abs(8 * gain/ (Dgain * Ana_gain * Fine_dgainx128));
        //Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_again_reg = Fine_againx64;
        //Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_dgain_reg = Fine_dgainx128;
    }
   //  pr_info("[%s]  gain : %d,%lld,%lld,%d, %d\n\n", __FUNCTION__,gain,Fine_again,Fine_dgainx128,Dgain,Ana_gain);
    // pr_info("[%s]  gain_reg : %x ,%x ,%x , %x\n\n", __FUNCTION__,Fine_again_reg,Ana_gain_reg,Fine_dgain_reg,Dgain_reg);

    gain_reg[3].data = Fine_again_reg;
    gain_reg[2].data = Ana_gain_reg;
    gain_reg[1].data = Fine_dgain_reg;
    gain_reg[0].data = Dgain_reg & 0xF;
    //pr_info("[%s]  gain_reg : %x ,%x ,%x , %x\n\n", __FUNCTION__,gain_reg[3].data,gain_reg[2].data,gain_reg[1].data,gain_reg[0].data);

    for (i = 0; i < ARRAY_SIZE(gain_reg); i++)
    {
      if (gain_reg[i].data != gain_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }
    return SUCCESS;
}

static int pCus_SetAEGainHDR_DOL_SEF(ms_cus_sensor *handle, u32 gain) {
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Ana_gain = 1;
    u64 Fine_againx64 = 64,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Ana_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e10, 0x00},
        {0x3e11, 0x80},
        {0x3e12, (0x00|0x03)},
        {0x3e13, 0x40},
    };
    memcpy(gain_reg_temp, gain_reg_HDR_DOL_SEF, sizeof(gain_reg_HDR_DOL_SEF));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 1;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 2;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 4;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x0f;
    }
    else if (gain <=  16256)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x1f;
    }
    else if (gain <  32512)
    {
        Dgain = 1;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
    else if (gain <  65024)
    {
        Dgain = 2;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x01;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
     else if (gain < 127 * 1024)
    {
        Dgain = 4;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x03;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
     else if (gain < 254 * 1024)
    {
        Dgain = 8;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x07;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
     else if (gain <= SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x0f;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }

    if (gain <= 16256)
    {
        Fine_againx64 = abs(8 * gain/ (Dgain * Ana_gain * Fine_dgainx128));
        //Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_again_reg = Fine_againx64;
        //Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_dgain_reg = Fine_dgainx128;
    }
  // pr_info("[%s]  gain : %d,%lld,%lld,%d, %d\n\n", __FUNCTION__,gain,Fine_again,Fine_dgain,Dgain,Ana_gain);
  // pr_info("[%s]  gain_reg : %x ,%x ,%x , %x\n\n", __FUNCTION__,Fine_again_reg,Ana_gain_reg,Fine_dgain_reg,Dgain_reg);

    gain_reg_HDR_DOL_SEF[3].data = Fine_again_reg;
    gain_reg_HDR_DOL_SEF[2].data = Ana_gain_reg;
    gain_reg_HDR_DOL_SEF[1].data = Fine_dgain_reg;
    gain_reg_HDR_DOL_SEF[0].data = Dgain_reg & 0xF;
    //pr_info("[%s]  gain_reg : %x ,%x ,%x , %x\n\n", __FUNCTION__,gain_reg_HDR_DOL_SEF[3].data,gain_reg_HDR_DOL_SEF[2].data,gain_reg_HDR_DOL_SEF[1].data,gain_reg_HDR_DOL_SEF[0].data);

    for (i = 0; i < ARRAY_SIZE(gain_reg_HDR_DOL_SEF); i++)
    {
      if (gain_reg_HDR_DOL_SEF[i].data != gain_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }
    return SUCCESS;
}


static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 30;
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
  *min = 1024;
  *max = SENSOR_MAXGAIN*1024;
  return SUCCESS;
}

static int sc8238_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 3;
    info->step = Preview_line_period;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_lef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (Preview_line_period_HDR_DOL * 4);
    info->step = Preview_line_period_HDR_DOL*2;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_sef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    sc8238_params *params = (sc8238_params *)handle->private_data;
    info->max = Preview_line_period_HDR_DOL * params->expo.max_short_exp;
    info->min = (Preview_line_period_HDR_DOL * 4);
    info->step = Preview_line_period_HDR_DOL*2;
    return SUCCESS;
}
static int pCus_poweron_hdr_dol_lef(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

static int pCus_poweroff_hdr_dol_lef(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}
static int pCus_GetSensorID_hdr_dol_lef(ms_cus_sensor *handle, u32 *id)
{
    *id = 0;
     return SUCCESS;
}
static int pCus_init_hdr_dol_lef(ms_cus_sensor *handle)
{
    return SUCCESS;
}
#if 0
static int pCus_GetVideoRes_hdr_dol_lef( ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res )
{
    *res = &handle->video_res_supported.res[res_idx];
    return SUCCESS;
}

static int pCus_SetVideoRes_hdr_dol_lef( ms_cus_sensor *handle, u32 res )
{
    handle->video_res_supported.ulcur_res = 0; //TBD
    return SUCCESS;
}
#endif
/* static int pCus_GetOrien_hdr_dol_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    *orit = CUS_ORIT_M0F0;
    return SUCCESS;
}

static int pCus_SetOrien_hdr_dol_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    return SUCCESS;
} */

static int pCus_GetFPS_hdr_dol_lef(ms_cus_sensor *handle)
{
    sc8238_params *params = (sc8238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[0].data << 8) | (vts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_setCaliData_gain_linearity_hdr_dol_lef(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    return SUCCESS;
}
static int pCus_SetAEGain_cal_hdr_dol_lef(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}
static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    sc8238_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (sc8238_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"sc8238_MIPI");

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
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= 15;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 3840;
    handle->video_res_supported.res[0].nOutputHeight= 2160;
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@15fps");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= 20;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth= 3840;
    handle->video_res_supported.res[1].nOutputHeight= 2160;
    sprintf(handle->video_res_supported.res[1].strResDesc, "3840x2160@20fps");

	
	handle->video_res_supported.num_res = 3;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[2].width = 3072;
    handle->video_res_supported.res[2].height = 1728;
    handle->video_res_supported.res[2].max_fps= 30;
    handle->video_res_supported.res[2].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[2].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[2].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[2].nOutputWidth= 3072;
    handle->video_res_supported.res[2].nOutputHeight= 1728;
    sprintf(handle->video_res_supported.res[2].strResDesc, "3072x1728@30fps");
	
	
    handle->video_res_supported.num_res = 4;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[3].width = 2592;
    handle->video_res_supported.res[3].height = 1944;
    handle->video_res_supported.res[3].max_fps= 30;
    handle->video_res_supported.res[3].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[3].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[3].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[3].nOutputWidth= 2592;
    handle->video_res_supported.res[3].nOutputHeight= 1944;
    sprintf(handle->video_res_supported.res[3].strResDesc, "2592x1944@30fps");

    handle->video_res_supported.num_res = 5;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[4].width = 2944;
    handle->video_res_supported.res[4].height = 1656;
    handle->video_res_supported.res[4].max_fps= 30;
    handle->video_res_supported.res[4].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[4].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[4].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[4].nOutputWidth= 2944;
    handle->video_res_supported.res[4].nOutputHeight= 1656;
    sprintf(handle->video_res_supported.res[4].strResDesc, "2944x1656@30fps");

    handle->video_res_supported.num_res = 6;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[5].width = 2560;
    handle->video_res_supported.res[5].height = 1440;
    handle->video_res_supported.res[5].max_fps= 30;
    handle->video_res_supported.res[5].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[5].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[5].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[5].nOutputWidth= 2560;
    handle->video_res_supported.res[5].nOutputHeight= 1440;
    sprintf(handle->video_res_supported.res[5].strResDesc, "2560x1440@30fps");

    handle->video_res_supported.num_res = 7;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[6].width = 1920;
    handle->video_res_supported.res[6].height = 1080;
    handle->video_res_supported.res[6].max_fps= 60;
    handle->video_res_supported.res[6].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[6].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[6].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[6].nOutputWidth= 1920;
    handle->video_res_supported.res[6].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[6].strResDesc, "1920x1080@60fps");

    handle->video_res_supported.num_res = 8;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[7].width = Preview_WIDTH;
    handle->video_res_supported.res[7].height = Preview_HEIGHT;
    handle->video_res_supported.res[7].max_fps= 30;
    handle->video_res_supported.res[7].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[7].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[7].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[7].nOutputWidth= 3840;
    handle->video_res_supported.res[7].nOutputHeight= 2160;
    sprintf(handle->video_res_supported.res[7].strResDesc, "3840x2160@30fps");

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
    handle->pCus_sensor_init        = pCus_init_mipi4lane_linear_4K15fps;

    handle->pCus_sensor_poweron     = pCus_poweron ;
    handle->pCus_sensor_poweroff    = pCus_poweroff;
    handle->pCus_sensor_post_init   = pCus_post_init;

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
    handle->pCus_sensor_SetPatternMode = sc8238_SetPatternMode;
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
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = sc8238_GetShutterInfo;
   // params->expo.vts=vts_30fps;
   // params->expo.fps = 30;
    params->expo.line= 1000;
    Preview_line_period  = 25720;
    params->expo.fps = 15;
    params->reg_dirty = false;
    params->reg_mf = false;
    return SUCCESS;
}
int cus_camsensor_init_handle_hdr_dol_sef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc8238_params *params = NULL;

    cus_camsensor_init_handle_linear(drv_handle);
    params = (sc8238_params *)handle->private_data;

    sprintf(handle->model_id,"sc8238_MIPI_HDR_SEF");

    handle->bayer_id    = SENSOR_BAYERID_HDR_DOL;
    handle->RGBIR_id    = SENSOR_RGBIRID;

    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 3840;
    handle->video_res_supported.res[0].nOutputHeight= 2160;
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@15fps");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = 1920;
    handle->video_res_supported.res[1].height = 1080;
    handle->video_res_supported.res[1].max_fps= 30;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth= 1920;
    handle->video_res_supported.res[1].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[1].strResDesc, "1920x1080@30fps");


    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
    handle->mclk                        = Preview_MCLK_SPEED_HDR_DOL;

    handle->pCus_sensor_init        = pCus_init_mipi4lane_HDR_DOL;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_DOL_SEF;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_DOL_SEF; //TBD

    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotifyHDR_DOL_SEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_SEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGainHDR_DOL_SEF;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGainHDR_DOL_SEF;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_sef;
    params->expo.vts = vts_30fps_HDR_DOL;
    params->expo.fps = 15;
    params->expo.max_short_exp = 130;
    handle->data_prec   = SENSOR_DATAPREC_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc8238_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (sc8238_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"sc8238_MIPI_HDR_LEF");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt    = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC_HDR_DOL;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID_HDR_DOL;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num =  0; //Long frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 3840;
    handle->video_res_supported.res[0].nOutputHeight= 2160;
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@15fps");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = 1920;
    handle->video_res_supported.res[1].height = 1080;
    handle->video_res_supported.res[1].max_fps= 30;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth= 1920;
    handle->video_res_supported.res[1].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[1].strResDesc, "1920x1080@30fps");


    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED_HDR_DOL;

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
    handle->ae_gain_delay       = 2;//0;//1;
    handle->ae_shutter_delay    = 2;//1;//2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_hdr_dol_lef;
    //handle->pCus_sensor_powerupseq  = pCus_powerupseq   ;
    handle->pCus_sensor_poweron     = pCus_poweron_hdr_dol_lef;
    handle->pCus_sensor_poweroff    = pCus_poweroff_hdr_dol_lef;
    handle->pCus_sensor_post_init   = pCus_post_init;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID_hdr_dol_lef;
    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
    handle->pCus_sensor_GetOrien          = pCus_GetOrien;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien_HDR;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_hdr_dol_lef;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_hdr_dol_lef;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap_hdr_dol_lef;
    handle->pCus_sensor_SetPatternMode = sc8238_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotifyHDR_DOL_LEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_LEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain_hdr_dol_lef;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    //handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs_hdr_dol_lef;

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal_hdr_dol_lef;
    handle->pCus_sensor_setCaliData_gain_linearity= pCus_setCaliData_gain_linearity_hdr_dol_lef;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_lef;

    params->expo.vts = vts_30fps_HDR_DOL;
    params->expo.fps = 15;
    params->reg_dirty = false;

    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  SC8235_HDR,
                            cus_camsensor_init_handle_linear,
                            cus_camsensor_init_handle_hdr_dol_sef,
                            cus_camsensor_init_handle_hdr_dol_lef,
                            sc8238_params
                         );


