/*
 * copyright (c) hisilicon technologies co., ltd. 2016-2019. all rights reserved.
 * description: thine_241a242_serdes.c
 * author: hisilicon multimedia software group
 * create: 2019-05-17
 */

#include "thine_241a242_serdes.h"

static inline void delay_us(const int us)
{
    udelay(us);
    return;
}

/* for 242 */
void write_serdes_rx_register(unsigned char i2c_dev, const unsigned int reg_addr, const unsigned int data)
{
    unsigned int reg_addr_num = 2;
    unsigned int data_byte_num = 1;
    unsigned char dev_addr = THINE_DEVICE_ADDR;

    serdes_i2c_write(i2c_dev, dev_addr, reg_addr, reg_addr_num, data, data_byte_num);

    return;
}

void write_serdes_rx_1bype_register(unsigned char i2c_dev, const unsigned int reg_addr, const unsigned int data)
{
    unsigned int reg_addr_num = 1;
    unsigned int data_byte_num = 1;
    unsigned char dev_addr = THINE_DEVICE_ADDR;

    serdes_i2c_write(i2c_dev, dev_addr, reg_addr, reg_addr_num, data, data_byte_num);

    return;
}

int read_serdes_rx_register(unsigned char i2c_dev, const unsigned int reg_addr)
{
    unsigned int r_data = 0;
    unsigned char dev_addr = (THINE_DEVICE_ADDR + 1);

    serdes_i2c_read(i2c_dev, dev_addr, reg_addr, &r_data);
    return r_data;
}

/* for 241 */
void write_serdes_tx_1bype_register(unsigned char i2c_dev, unsigned int reg_addr, const unsigned int data)
{
    unsigned int reg_addr_num = 1;
    unsigned int data_byte_num = 1;
    unsigned char dev_addr = (sensor_dev_addr[i2c_dev] << 1);

    serdes_i2c_write(i2c_dev, dev_addr, reg_addr, reg_addr_num, data, data_byte_num);
    return;
}

void write_serdes_tx_2bype_register(unsigned char i2c_dev, const unsigned int reg_addr, const unsigned int data)
{
    unsigned int reg_addr_num = 2;
    unsigned int data_byte_num = 1;
    unsigned char dev_addr = (sensor_dev_addr[i2c_dev] << 1);

    serdes_i2c_write(i2c_dev, dev_addr, reg_addr, reg_addr_num, data, data_byte_num);
    return;
}

void set_serdes_data_rate(unsigned char i2c_dev, serdes_mode_t serdes_mode)
{
#if SERDES_DEBUG
    int result = 0;
#endif
    if (serdes_mode == SERDES_MODE_4LANE_LINEAR) {
        write_serdes_rx_register(i2c_dev, 0x1609, 0x00);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1609);
        osal_printk("read 242 0x1609: 0x%x, should be 0x00\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160A, 0x0C);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160A);
        osal_printk("read 242 0x160A: 0x%x, should be 0x0C\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160B, 0x03);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160B);
        osal_printk("read 242 0x160B: 0x%x, should be 0x03\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160C, 0x01);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160C);
        osal_printk("read 242 0x160C: 0x%x, should be 0x01\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160D, 0x0C);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160D);
        osal_printk("read 242 0x160D: 0x%x, should be 0x0C\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160E, 0x00);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160E);
        osal_printk("read 242 0x160E: 0x%x, should be 0x00\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160F, 0x01);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160F);
        osal_printk("read 242 0x160F: 0x%x, should be 0x01\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1610, 0x00);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1610);
        osal_printk("read 242 0x1610: 0x%x, should be 0x00\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1611, 0x02);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1611);
        osal_printk("read 242 0x1611: 0x%x, should be 0x02\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1612, 0x02);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1612);
        osal_printk("read 242 0x1612: 0x%x, should be 0x02\n", result);
        #endif
    } else if ((serdes_mode == SERDES_MODE_2LANE_LINEAR) ||
               (serdes_mode == SERDES_MODE_4LANE_WDR)) {
        write_serdes_rx_register(i2c_dev, 0x1609, 0x03);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1609);
        osal_printk("read 242 0x1609: 0x%x, should be 0x03\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160A, 0x0F);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160A);
        osal_printk("read 242 0x160A: 0x%x, should be 0x0F\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160B, 0x05);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160B);
        osal_printk("read 242 0x160B: 0x%x, should be 0x05\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160C, 0x01);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160C);
        osal_printk("read 242 0x160C: 0x%x, should be 0x01\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160D, 0x0A);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160D);
        osal_printk("read 242 0x160D: 0x%x, should be 0x0A\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160E, 0x02);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160E);
        osal_printk("read 242 0x160E: 0x%x, should be 0x02\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160F, 0x02);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160F);
        osal_printk("read 242 0x160F: 0x%x, should be 0x02\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1610, 0x03);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1610);
        osal_printk("read 242 0x1610: 0x%x, should be 0x03\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1611, 0x02);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1611);
        osal_printk("read 242 0x1611: 0x%x, should be 0x02\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1612, 0x04);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1612);
        osal_printk("read 242 0x1612: 0x%x, should be 0x04\n", result);
        #endif
    } else if (serdes_mode == SERDES_MODE_2LANE_WDR) {
        write_serdes_rx_register(i2c_dev, 0x1609, 0x04);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1609);
        osal_printk("read 242 0x1609: 0x%x, should be 0x04\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160A, 0x23);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160A);
        osal_printk("read 242 0x160A: 0x%x, should be 0x23\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160B, 0x08);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160B);
        osal_printk("read 242 0x160B: 0x%x, should be 0x08\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160C, 0x01);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160C);
        osal_printk("read 242 0x160C: 0x%x, should be 0x01\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160D, 0x11);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160D);
        osal_printk("read 242 0x160D: 0x%x, should be 0x11\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160E, 0x09);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160E);
        osal_printk("read 242 0x160E: 0x%x, should be 0x09\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x160F, 0x06);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x160F);
        osal_printk("read 242 0x160F: 0x%x, should be 0x06\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1610, 0x06);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1610);
        osal_printk("read 242 0x1610: 0x%x, should be 0x06\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1611, 0x10);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1611);
        osal_printk("read 242 0x1611: 0x%x, should be 0x10\n", result);
        #endif

        write_serdes_rx_register(i2c_dev, 0x1612, 0x08);
        #if SERDES_DEBUG
        result = read_serdes_rx_register(i2c_dev, 0x1612);
        osal_printk("read 242 0x1612: 0x%x, should be 0x08\n", result);
        #endif
    }
}

void get_serdes_cfg(serdes_mode_t serdes_mode, serdes_config *cfg)
{
    if (serdes_mode == SERDES_MODE_4LANE_LINEAR) {
        osal_memcpy(cfg, &imx307_4_lane_linear_12_bit_30_fps_cfg, sizeof(serdes_config));
    } else if (serdes_mode == SERDES_MODE_4LANE_WDR) {
        osal_memcpy(cfg, &imx307_4_lane_wdr_12_bit_30_fps_cfg, sizeof(serdes_config));
    } else if (serdes_mode == SERDES_MODE_2LANE_LINEAR) {
        osal_memcpy(cfg, &imx307_2_lane_linear_12_bit_30_fps_cfg, sizeof(serdes_config));
    } else {
        osal_memcpy(cfg, &imx307_2_lane_wdr_12_bit_30_fps_cfg, sizeof(serdes_config));
    }
}

void serdes_241_242_init(unsigned char i2c_dev, serdes_mode_t serdes_mode)
{
    int i = 0;
    serdes_config cfg;
#if SERDES_DEBUG
    int result = 0;
#endif

    get_serdes_cfg(serdes_mode, &cfg);

    /*---------------------------------------------------------
    step1: THCV242 sub-link mode : THCV241A ID
    ------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0050, 0x34);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0050);
    osal_printk("read 242 0x0050: 0x%x, should be 0x34\n", result);
    #endif

    /*---------------------------------------------------------
    step1: THCV242 sub-link mode : pass throuth mode
    ------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0004, 0x03);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0004);
    osal_printk("read 242 0x0004: 0x%x, should be 0x3\n", result);
    #endif

    /*---------------------------------------------------------
    step2: THCV242 sub link en/pol : lane 0 enable polling off
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0010, 0x10); // polling off

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0010);
    osal_printk("read 242 0x0010: 0x%x, should be 0x10\n", result);
    #endif

    /*---------------------------------------------------------
    step3: THCV242 sub link power on
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x1704, 0x01);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1704);
    osal_printk("read 242 0x1704: 0x%x, should be 0x1\n", result);
    #endif

    /*---------------------------------------------------------
    step4: THCV242 sub link tx term 50ohm
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0102, 0x02); // 50ohm

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0102);
    osal_printk("read 242 0x0102: 0x%x, should be 0x2\n", result);
    #endif

    /*---------------------------------------------------------
    step5: THCV242 sub link tx drv 12ma
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0103, 0x02); // 12m_a

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0103);
    osal_printk("read 242 0x0103: 0x%x, should be 0x2\n", result);
    #endif

    /*---------------------------------------------------------
    step6: THCV242 sub link rx term : 50ohm
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0104, 0x02); // 50ohm

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0104);
    osal_printk("read 242 0x0104: 0x%x, should be 0x2\n", result);
    #endif

    /*---------------------------------------------------------
    step7: THCV242 rx drv : 12ma
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0105, 0x02); // 12mA

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0105);
    osal_printk("read 242 0x0105: 0x%x, should be 0x2\n", result);
    #endif

    /*---------------------------------------------------------
    step8: THCV242 reserved setting : key is unlocked
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0100, 0x03);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0100);
    osal_printk("read 242 0x0100: 0x%x, should be 0x3\n", result);
    #endif

    /*---------------------------------------------------------
    step9: THCV242 reserved setting : key2 is unlocked
    ------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x010F, 0x25);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x010F);
    osal_printk("read 242 0x010F: 0x%x, should be 0x25\n", result);
    #endif

    /*---------------------------------------------------------
    step10: THCV242 reserved setting : sub link width
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x010A, 0x15); // 0x15 -> 0x18 -> 0x15

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x010A);
    osal_printk("read 242 0x010A: 0x%x, should be 0x15\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x0031, 0x02);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0031);
    osal_printk("read 242 0x0031: 0x%x, should be 0x2\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x0032, 0x10);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0032);
    osal_printk("read 242 0x0032: 0x%x, should be 0x10\n", result);
    #endif

    /*---------------------------------------------------------
    step13: THCV241 bank is 00
    ---------------------------------------------------------*/
    write_serdes_tx_2bype_register(i2c_dev, 0x00FE, 0x11);

    write_serdes_rx_register(i2c_dev, 0x0032, 0x00);

    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0032);
    osal_printk("read 242 0x0032: 0x%x, should be 0x00\n", result);
    #endif

    /*---------------------------------------------------------
    step14: THCV241 sub link tx term / drv setting:50ohm 12ma
    ---------------------------------------------------------*/
    write_serdes_tx_1bype_register(i2c_dev, 0x00F2, 0x22); // 50ohm/12mA

    write_serdes_tx_1bype_register(i2c_dev, 0x00F3, 0x22); // 50ohm/12mA

    /*---------------------------------------------------------
    step15: THCV241 reserved setting : key is unlocked
    ---------------------------------------------------------*/
    write_serdes_tx_1bype_register(i2c_dev, 0x00F0, 0x03);

    /*---------------------------------------------------------
    THCV241 reserved setting : key 2 is unlocked
    ---------------------------------------------------------*/
    write_serdes_tx_1bype_register(i2c_dev, 0x00FF, 0x19);

    /*---------------------------------------------------------
    step16: THCV241 reserved setting : sub link width
    ---------------------------------------------------------*/
    write_serdes_tx_1bype_register(i2c_dev, 0x00F6, 0x18); // 0x15

    /*---------------------------------------------------------
    step16: THCV241 I2C freq   400k_hz
    ---------------------------------------------------------*/
    write_serdes_tx_1bype_register(i2c_dev, 0x00C9, 0x05);
    write_serdes_tx_1bype_register(i2c_dev, 0x00CA, 0x05);

    /*---------------------------------------------------------
    step17: THCV242 sub link en/pol : lane 0 enable polling on
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x0010, 0x11);


    /*---------------------------------------------------------
    step18: THCV241 bank is 10
    ---------------------------------------------------------*/
    write_serdes_tx_1bype_register(i2c_dev, 0x00FE, 0x21);

    write_serdes_tx_1bype_register(i2c_dev, 0x002C, 0x01);

    write_serdes_tx_1bype_register(i2c_dev, 0x002D, cfg.pll_value[i]);
    i += 1;

    write_serdes_tx_1bype_register(i2c_dev, 0x0076, 0x10);

    write_serdes_tx_1bype_register(i2c_dev, 0x0000, 0x00);

    write_serdes_tx_1bype_register(i2c_dev, 0x0001, 0x00);

    write_serdes_tx_1bype_register(i2c_dev, 0x000F, 0x01);

    write_serdes_tx_1bype_register(i2c_dev, 0x0011, cfg.pll_value[i]);
    i += 1;

    write_serdes_tx_1bype_register(i2c_dev, 0x0012, 0x00);
    write_serdes_tx_1bype_register(i2c_dev, 0x0013, 0x00);
    write_serdes_tx_1bype_register(i2c_dev, 0x0014, 0x00);

    write_serdes_tx_1bype_register(i2c_dev, 0x0015, cfg.pll_value[i]);
    i += 1;

    write_serdes_tx_1bype_register(i2c_dev, 0x0016, 0x01);

    write_serdes_tx_1bype_register(i2c_dev, 0x002B, 0x07);

    write_serdes_tx_1bype_register(i2c_dev, 0x002F, 0x00);

    write_serdes_tx_1bype_register(i2c_dev, 0x0005, 0x01);
    write_serdes_tx_1bype_register(i2c_dev, 0x0006, 0x01);

    write_serdes_tx_1bype_register(i2c_dev, 0x0027, 0x00);

    /*---------------------------------------------------------
    step19: THCV242
    ---------------------------------------------------------*/
    write_serdes_rx_register(i2c_dev, 0x1010, 0xA1);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1010);
    osal_printk("read 242 0x1010: 0x%x, should be 0xa1\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1021, 0x20);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1021);
    osal_printk("read 242 0x1021: 0x%x, should be 0x20\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1022, cfg.pll_value[i]);
    i += 1;
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1022);
    osal_printk("read 242 0x1022: 0x%x, should be 0x01\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1023, cfg.pll_value[i]);
    i += 1;
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1023);
    osal_printk("read 242 0x1023: 0x%x, should be 0x41--4L linear, 0x21--4L wdr\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1024, 0x00);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1024);
    osal_printk("read 242 0x1024: 0x%x, should be 0x00\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1025, 0x00);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1025);
    osal_printk("read 242 0x1025: 0x%x, should be 0x00\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1026, 0x00);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1026);
    osal_printk("read 242 0x1026: 0x%x, should be 0x00\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1027, 0x07);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1027);
    osal_printk("read 242 0x1027: 0x%x, should be 0x07\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1028, cfg.pll_value[i]);
    i += 1;
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1028);
    osal_printk("read 242 0x1028: 0x%x, should be 0x00--4lane, 0x2--2lane\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1030, 0x00);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1030);
    osal_printk("read 242 0x1030: 0x%x, should be 0x00\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1100, 0x01);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1100);
    osal_printk("read 242 0x1100: 0x%x, should be 0x01\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1101, 0x01);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1101);
    osal_printk("read 242 0x1101: 0x%x, should be 0x01\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1102, 0x01);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1102);
    osal_printk("read 242 0x1102: 0x%x, should be 0x01\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1600, 0x1A);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1600);
    osal_printk("read 242 0x1600: 0x%x, should be 0x1a\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1605, cfg.pll_value[i]);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1605);
    osal_printk("read 242 0x1605: 0x%x, should be 0x2b--4lane, 0x29--2lane\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1606, 0x45);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1606);
    osal_printk("read 242 0x1606: 0x%x, should be 0x45\n", result);
    #endif

    set_serdes_data_rate(i2c_dev, serdes_mode);

    write_serdes_rx_register(i2c_dev, 0x1704, 0x11);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1704);
    osal_printk("read 242 0x1704: 0x%x, should be 0x11\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x1703, 0x01);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x1703);
    osal_printk("read 242 0x1703: 0x%x, should be 0x01\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x0032, 0x10);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0032);
    osal_printk("read 242 0x0032: 0x%x, should be 0x10\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x0040, 0x1A);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0040);
    osal_printk("read 242 0x0040: 0x%x, should be 0x1a\n", result);
    #endif

    write_serdes_rx_register(i2c_dev, 0x0041, 0x1A);
    #if SERDES_DEBUG
    result = read_serdes_rx_register(i2c_dev, 0x0041);
    osal_printk("read 242 0x0041: 0x%x, should be 0x1a\n", result);
    #endif

    return;
}

void serdes_242_reset(unsigned char i2c_dev)
{
    write_serdes_rx_register(i2c_dev, 0x1701, 0x3F);
    write_serdes_rx_register(i2c_dev, 0x1702, 0x01);  // SW reset
    write_serdes_rx_register(i2c_dev, 0x1703, 0x00);  // PLL power down
    write_serdes_rx_register(i2c_dev, 0x1704, 0x00);  // Vx1HS Sub-Link power down
    write_serdes_rx_register(i2c_dev, 0x0050, 0x34);
    write_serdes_rx_register(i2c_dev, 0x0004, 0x03);
    write_serdes_rx_register(i2c_dev, 0x0010, 0x10);
    write_serdes_rx_register(i2c_dev, 0x1704, 0x01);  // Vx1HS Sub-Link power on
    write_serdes_rx_register(i2c_dev, 0x0102, 0x02);
    write_serdes_rx_register(i2c_dev, 0x0103, 0x02);
    write_serdes_rx_register(i2c_dev, 0x0104, 0x02);
    write_serdes_rx_register(i2c_dev, 0x0105, 0x02);
    write_serdes_rx_register(i2c_dev, 0x0100, 0x03);
    write_serdes_rx_register(i2c_dev, 0x010F, 0x25);
    write_serdes_rx_register(i2c_dev, 0x010A, 0x15);
}

void serdes_241a_reset(unsigned char i2c_dev)
{
    write_serdes_rx_1bype_register(i2c_dev, 0x0031, 0x02);
    write_serdes_rx_1bype_register(i2c_dev, 0x0032, 0x00); // 1byte address, 1byte data access
    write_serdes_tx_1bype_register(i2c_dev, 0x00FE, 0x21); // THCV241 bank is 10
    write_serdes_tx_1bype_register(i2c_dev, 0x0005, 0x00); // PLL Soft Reset Active
    write_serdes_tx_1bype_register(i2c_dev, 0x0006, 0x00); // Vx1HS Soft Reset Active
    write_serdes_tx_1bype_register(i2c_dev, 0x0021, 0x00); // MIPI Logic Reset
    write_serdes_tx_1bype_register(i2c_dev, 0x0022, 0x00); // Logic Reset
    write_serdes_tx_1bype_register(i2c_dev, 0x0023, 0x00); // MIPI Clock Reset

    delay_us(1000);

    write_serdes_tx_1bype_register(i2c_dev, 0x0021, 0x01); // MIPI Logic Reset
    write_serdes_tx_1bype_register(i2c_dev, 0x0022, 0x01); // Logic Reset
    write_serdes_tx_1bype_register(i2c_dev, 0x0023, 0x01); // MIPI Clock Reset

    write_serdes_tx_1bype_register(i2c_dev, 0x00FE, 0x00);
}

void serdes_241_242_exit(unsigned char i2c_dev)
{
    serdes_242_reset(i2c_dev);

    serdes_241a_reset(i2c_dev);
}

void serdes_thine_init(int i2c_dev, serdes_mode_t serdes_mode)
{
    serdes_241_242_init(i2c_dev, serdes_mode);
}

void serdes_thine_exit(int i2c_dev)
{
    serdes_241_242_exit(i2c_dev);
}

void serdes_reset_unreset_sensor(unsigned char i2c_dev, int reset)
{
    const unsigned int tmp = reset ? 0x00 : 0x30;  // 10/11 -> 00/30

    write_serdes_rx_register(i2c_dev, 0x0032, 0x00);
    write_serdes_tx_1bype_register(i2c_dev, 0xFE, 0x21);
    write_serdes_tx_1bype_register(i2c_dev, 0x3E, tmp);
    write_serdes_rx_register(i2c_dev, 0x0032, 0x10);
}
EXPORT_SYMBOL(serdes_reset_unreset_sensor);

void serdes_enable_disable_sensor_clock(unsigned char i2c_dev, int enable)
{
    const unsigned int tmp = enable ? 0x10 : 0x50;

    write_serdes_rx_register(i2c_dev, 0x0032, 0x00);
    write_serdes_tx_1bype_register(i2c_dev, 0xFE, 0x21);
    write_serdes_tx_1bype_register(i2c_dev, 0x76, tmp);
    write_serdes_rx_register(i2c_dev, 0x0032, 0x10);
}
EXPORT_SYMBOL(serdes_enable_disable_sensor_clock);
