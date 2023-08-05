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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(gc4653);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

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

#define SENSOR_ISP_TYPE     ISP_EXT                 //ISP_EXT, ISP_SOC
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI     //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_GR            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     (77660)/100                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 2
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period 22222//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
//#define Line_per_second     32727
#define vts_30fps  1500//1266//1150//1090                              //for 29.091fps @ MCLK=36MHz
//#define Prv_Max_line_number 2200                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       2560                    //resolution Width when preview
#define Preview_HEIGHT      1440                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x52                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
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

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
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
        u32 line;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[7];
    I2C_ARRAY tExpo_reg[2];
    I2C_ARRAY tMirror_reg[1];
} gc4653_params;
// set sensor ID address and data,

/* typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;
 */
const static I2C_ARRAY Sensor_id_table[] =
{
    {0x03f0, 0x46},     // {address of ID, ID },
    {0x03f1, 0x53},
};

typedef struct {
      u32 gain;
      unsigned short again_reg_val_0;
      unsigned short again_reg_val_1;
      unsigned short again_reg_val_2;
      unsigned short again_reg_val_3;
      unsigned short again_reg_val_4;
} Gain_ARRAY;

const static I2C_ARRAY Sensor_init_table[] =
{
/****************************************/
//version 6.3
//mclk 27Mhz
//mipiclk 648Mhz
//framelength 1500
//linelength 4800
//pclk 216Mhz
//rowtime 22.2222us
//pattern grbg

/****************************************/

/****************************************/
/*SYSTEM*/
/****************************************/
{0x03fe, 0xf0},
{0x03fe, 0x00},
{0x0317, 0x00},
{0x0320, 0x77},
{0x0324, 0xc8},
{0x0325, 0x06},
{0x0326, 0x60},
{0x0327, 0x03},
{0x0334, 0x40},
{0x0336, 0x60},
{0x0337, 0x82},
{0x0315, 0x25},
{0x031c, 0xc6},
/****************************************/
/*frame structure*/
/****************************************/
{0x0287, 0x18},
{0x0084, 0x00},
{0x0087, 0x50},
{0x029d, 0x08},
{0x0290, 0x00},
//AHD 30 other need change
{0x0340, 0x05},
{0x0341, 0xdc},
{0x0345, 0x06},
{0x034b, 0xb0},
{0x0352, 0x08},
{0x0354, 0x08},
/****************************************/
/*ANALOG CIRCUIT*/
/****************************************/
{0x02d1, 0xb0},
{0x023c, 0x05},
{0x0223, 0xfb},
{0x0232, 0xc4},
{0x0279, 0x53},
{0x02d3, 0x01},
{0x0243, 0x06},
{0x02ce, 0xbf},
{0x02ee, 0x30},
{0x026f, 0x70},
{0x0257, 0x09},
{0x0211, 0x02},
{0x0219, 0x09},
{0x023f, 0x2d},
{0x0518, 0x00},
{0x0519, 0x01},
{0x0515, 0x08},
{0x02d9, 0x3f},
{0x02da, 0x02},
{0x02db, 0xe8},
{0x02e6, 0x20},
{0x021b, 0x10},
{0x0252, 0x22},
{0x024e, 0x22},
{0x02c4, 0x01},
{0x021d, 0x07},
{0x024a, 0x01},
{0x02ca, 0x02},
{0x0262, 0x10},
{0x029a, 0x20},
{0x021c, 0x0e},
{0x0298, 0x03},
{0x029c, 0x00},
{0x027e, 0x14},
{0x02c2, 0x10},
{0x0540, 0x20},
{0x0546, 0x01},
{0x0548, 0x01},
{0x0544, 0x01},
{0x0242, 0x1b},
{0x02c0, 0x1b},
{0x02c3, 0x20},
{0x02e4, 0x10},
{0x022e, 0x00},
{0x027b, 0x3f},
{0x0269, 0x0f},
{0x02d2, 0x40},
{0x027c, 0x08},
{0x023a, 0x2e},
{0x0245, 0xce},
{0x0530, 0x20},
{0x0531, 0x02},
{0x0228, 0x28},
{0x02ab, 0x00},
{0x0250, 0x00},
{0x0221, 0x28},
{0x02ac, 0x00},
{0x02a5, 0x02},
{0x0260, 0x0b},
{0x0216, 0x04},
{0x0299, 0x1C},
{0x02bb, 0x0d},
{0x02a3, 0x02},
{0x02a4, 0x02},
{0x021e, 0x02},
{0x024f, 0x08},
{0x028c, 0x08},
{0x0532, 0x3f},
{0x0533, 0x02},
{0x0277, 0x38},
{0x0276, 0x60},
{0x0239, 0xc0},
/*exp*/
{0x0202, 0x05},
{0x0203, 0xd0},
/*gain*/
{0x0205, 0xc0},
{0x02b0, 0x60}, //0x90
/*dpc*/
{0x0002, 0xa9},
{0x0004, 0x01},
/****************************************/
/*mipi*/
/****************************************/
//30fps
{0x0342, 0x06},
{0x0343, 0x40},
{0x03fe, 0x10},
{0x03fe, 0x00},
{0x0106, 0x78},
{0x0108, 0x0c},
{0x0114, 0x01},
{0x0115, 0x12},
{0x0180, 0x46},
{0x0181, 0x30},
{0x0182, 0x05},
{0x0185, 0x01},
{0x03fe, 0x10},
{0x03fe, 0x00},
{0x0100, 0x09},
};

const I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

const I2C_ARRAY PatternTbl[] = {
    //pattern mode
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

const I2C_ARRAY mirror_reg[] = {
    {0x0101, 0x00},
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

static int g_sensor_ae_min_gain = 1024;//1280;

const static I2C_ARRAY gain_reg[] = {
    {0x02b3, 0x00},
    {0x0519, 0x04},
    {0x02d9, 0x42},
    {0x02b8, 0x01},
    {0x02b9, 0x00},
    {0x020e, 0x01},
    {0x020f, 0x00},
};

const static I2C_ARRAY expo_reg[] = {
    {0x0203, 0xd0}, // expo[7:0]
    {0x0202, 0x05}, // expo[15:8]
};

const static I2C_ARRAY vts_reg[] = {
    {0x0340, 0x05},
    {0x0341, 0xdc},
};

 static Gain_ARRAY gain_table[]={
           //02b3, 0519, 02d9, 02b8, 02b9
     { 1024, 0x00, 0x04, 0x42, 0x01, 0x00},
     { 1213, 0x20, 0x05, 0x43, 0x01, 0x0B},
     { 1434, 0x01, 0x04, 0x42, 0x01, 0x19},
     { 1699, 0x21, 0x05, 0x43, 0x01, 0x2a},
     { 2048, 0x02, 0x04, 0x42, 0x02, 0x00},
     { 2427, 0x22, 0x05, 0x43, 0x02, 0x17},
     { 2867, 0x03, 0x05, 0x43, 0x02, 0x33},
     { 3398, 0x23, 0x06, 0x44, 0x03, 0x14},
     { 4096, 0x04, 0x06, 0x44, 0x04, 0x00},
     { 4854, 0x24, 0x08, 0x46, 0x04, 0x2f},
     { 5734, 0x05, 0x08, 0x46, 0x05, 0x26},
     { 6795, 0x25, 0x0a, 0x48, 0x06, 0x28},
     { 8192, 0x06, 0x0c, 0x4a, 0x08, 0x00},
     { 9708, 0x26, 0x0d, 0x4b, 0x09, 0x1E},
     {11469, 0x46, 0x0f, 0x4d, 0x0B, 0x0C},
     {13591, 0x66, 0x11, 0x4f, 0x0D, 0x11},
     {16384, 0x0e, 0x13, 0x51, 0x10, 0x00},
     {19415, 0x2e, 0x16, 0x54, 0x12, 0x3D},
     {22938, 0x4e, 0x19, 0x57, 0x16, 0x19},
     {27181, 0x6e, 0x1b, 0x59, 0x1A, 0x22},
     {32768, 0x1e, 0x1e, 0x5c, 0x20, 0x00},
     {38830, 0x3e, 0x21, 0x5f, 0x25, 0x3A},
     {45875, 0x5e, 0x25, 0x63, 0x2C, 0x33},
     {54362, 0x7e, 0x29, 0x67, 0x35, 0x05},
     {65536, 0x9e, 0x2d, 0x6b, 0x40, 0x00},
     {77660, 0xbe, 0x40, 0x78, 0x4B, 0x35},
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
#define SENSOR_NAME gc4653


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
    SENSOR_DMSG("[%s]gc4653 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int gc4653_SetPatternMode(ms_cus_sensor *handle,u32 mode)
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
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
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

                if(cnt>=10)
                {

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
    char sen_data;
    sen_data = mirror_reg[0].data;
    SENSOR_DMSG("mirror:%x\r\n", sen_data);
    switch(sen_data) {
    case 0x03:
        *orit = CUS_ORIT_M0F0;
        break;
    case 0x02:
        *orit = CUS_ORIT_M1F0;
        break;
    case 0x01:
        *orit = CUS_ORIT_M0F1;
        break;
    case 0x00:
        *orit = CUS_ORIT_M1F1;
        break;
    }

    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    gc4653_params *params = (gc4653_params *)handle->private_data;
    switch(orit) {
    case CUS_ORIT_M0F0:
        params->tMirror_reg[0].data = 0x03;
        params->orient_dirty = true;
        //params->dirty = true;
        break;
    case CUS_ORIT_M1F0:
        params->tMirror_reg[0].data = 0x2;
        params->orient_dirty = true;
        //params->dirty = true;
        break;
    case CUS_ORIT_M0F1:
        params->tMirror_reg[0].data = 0x01;
        params->orient_dirty = true;
        //params->dirty = true;
        break;
    case CUS_ORIT_M1F1:
        params->tMirror_reg[0].data = 0x00;
        params->orient_dirty = true;
        //params->dirty = true;
        break;
    }

    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    gc4653_params *params = (gc4653_params *)handle->private_data;
    return  params->expo.fps;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    int vts=0;
    gc4653_params *params = (gc4653_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

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
    params->tVts_reg[0].data = (vts >> 8) & 0x003f;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;
    //vts_reg[0].data = ((params->expo.vts - 12 ) >> 8) & 0x001f;
    //vts_reg[1].data = ((params->expo.vts - 12) >> 0) & 0x00ff;

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
    gc4653_params *params = (gc4653_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

            break;
        case CUS_FRAME_ACTIVE:
            if(params->orient_dirty)
            {
                SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, ARRAY_SIZE(mirror_reg));
                params->orient_dirty = false;
		    }
            if(params->dirty)
            {
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                params->dirty = false;
            }
            break;
        default :
            break;
    }

    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    gc4653_params *params = (gc4653_params *)handle->private_data;
    u32 lines = 0;

    lines  = (u32)(params->tExpo_reg[0].data&0xff);
    lines |= (u32)(params->tExpo_reg[1].data&0x3f)<<8;

    *us = (lines*Preview_line_period)/1000;


    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0;
    gc4653_params *params = (gc4653_params *)handle->private_data;

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
    params->tExpo_reg[0].data = (lines) & 0x00ff;
    params->tExpo_reg[1].data = (lines>>8) & 0x003f;
    params->tVts_reg[0].data = (vts >> 8) & 0x003f;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    gc4653_params *params = (gc4653_params *)handle->private_data;
    u32 Again_0 = 1, Dgain_0 = 1;
    u8 i;
    u32  Fine_again = 1024;

    for(i = 0;i < sizeof(gain_table)/sizeof(Gain_ARRAY);i++ )    {
        if(( gain_table[i].again_reg_val_0 == params->tGain_reg[1].data) && (gain_table[i].again_reg_val_1 == params->tGain_reg[2].data))
        {
            Fine_again = gain_table[i].gain;
            break;
        }
    }

    Dgain_0 = (params->tGain_reg[5].data & 0xf) * 64 + (params->tGain_reg[6].data >> 2);
    Again_0 = Fine_again;
    *gain =(u32)((Again_0 * Dgain_0)/64);

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    gc4653_params *params = (gc4653_params *)handle->private_data;
    u32 dgain = 1;
//  u32 gain_64 = gain;
    u8 dgain_0 = 1, dgain_1 = 0;
    u8 i = 0,tmp = 0;
//    int temperature_value;
    params->expo.final_gain = gain;

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAX_GAIN  * 1024) {
        gain = SENSOR_MAX_GAIN  * 1024;
    }

    for(i = 1;i < sizeof(gain_table)/sizeof(Gain_ARRAY);i++ )
    {
        if((gain >= gain_table[i-1].gain) && (gain < gain_table[i].gain))
        {
            tmp = i - 1;
            break;
        }
        else
            tmp = sizeof(gain_table)/sizeof(Gain_ARRAY) - 1;
    }

    dgain =(gain*64)/(gain_table[tmp].gain);
    dgain_0 = (dgain)>>6;
    dgain_1 =(dgain & 0x3f)<<2;
    params->tGain_reg[0].data  = gain_table[tmp].again_reg_val_0;
    params->tGain_reg[1].data  = gain_table[tmp].again_reg_val_1;
    params->tGain_reg[2].data  = gain_table[tmp].again_reg_val_2;
    params->tGain_reg[3].data  = gain_table[tmp].again_reg_val_3;
    params->tGain_reg[4].data  = gain_table[tmp].again_reg_val_4;

    params->tGain_reg[5].data  = dgain_0;
    params->tGain_reg[6].data  = dgain_1;
    SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,gain_reg[1].data,gain_reg[2].data);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 30;
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min =handle->sat_mingain;
    *max = SENSOR_MAX_GAIN*1024;
    return SUCCESS;
}

static int gc4653_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min =  Preview_line_period*1;//2
    info->step = Preview_line_period*1;//2
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    gc4653_params *params;
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
    params = (gc4653_params *)handle->private_data;
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"gc4653_MIPI");

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
    handle->video_res_supported.res[0].nOutputWidth= 2560;
    handle->video_res_supported.res[0].nOutputHeight= 1440;
    SENSOR_DMSG(handle->video_res_supported.res[0].strResDesc, "2560x1440@30fps");

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
    handle->pCus_sensor_SetPatternMode = gc4653_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = gc4653_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line = 100;
    params->dirty = false;
    params->orient_dirty = false;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  gc4653,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            gc4653_params
                         );
