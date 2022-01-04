/*
 * Copyright (c) Hunan Goke,Chengdu Goke,Shandong Goke. 2021. All rights reserved.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <asm/io.h>


#define SENSOR_NAME_LEN             64
#define CHIP_NAME_STR_LEN               64
#define BOARD_NAME_LEN              64

#define SYS_WRITEL(Addr, Value) ((*(volatile unsigned int *)(Addr)) = (Value))
#define SYS_READ(Addr)          (*((volatile int *)(Addr)))

static void  *reg_crg_base=0;
static void  *reg_ddr_base=0;
static void  *reg_misc_base=0;
static void  *reg_sysctl_base=0;
static void  *reg_iocfg_vio_base=0;
static void  *reg_iocfg_ahb_base=0;
static void  *reg_iocfg_core_base=0;
static void  *reg_gpio_base=0;
static void *reg_pwm_base=0;

int g_online_flag = 0;                              /* Same as the value of the VI_VPSS_MODE_E.*/
int g_cmos_yuv_flag = 0;                            /* vi: 0--RAW, 1--DC, 2--BT1120, 3--BT656 */
char sensor_list[SENSOR_NAME_LEN] = "imx307";       /* imx307 imx327 sc4236 sc2235 sc3235 imx335 soi_f37 os05a gc2053 bt656 bt1120 imx307_2l,imx327_2l*/
char chip_list[CHIP_NAME_STR_LEN]     = "gk7205v200";  /* gk7205v200 gk7205v300 gk7202v300 gk7605v100 */
char board_list[BOARD_NAME_LEN]   = "demo";         /* sck demo*/
int g_quick_start_flag = 0;                         /* If set to 1, you need to do something in Uboot. */


typedef enum {
    BUS_TYPE_I2C = 0,
    BUS_TYPE_SPI = 1,
} BUS_TYPE;

static inline void reg_write32(unsigned long value, unsigned long mask,const void *addr)
{
    unsigned long t;

    t = SYS_READ((const volatile void *)addr);
    t &= ~mask;
    t |= value & mask;
    SYS_WRITEL((volatile void *)addr, t);
}


void sensor_clock_config(int index, unsigned int clock)
{
    reg_write32(clock << 2, 0xF << 2, reg_crg_base+0x00F0);
}

BUS_TYPE parse_sensor_bus_type(const char *name)
{
    unsigned int len;
    BUS_TYPE bus_type = BUS_TYPE_I2C;

    len = SENSOR_NAME_LEN;

    if (   (0 == strncmp("imx307", name, len))
        || (0 == strncmp("imx327", name, len))
        || (0 == strncmp("imx335", name, len))
        || (0 == strncmp("sc4236", name, len))
        || (0 == strncmp("sc2231", name, len))
        || (0 == strncmp("sc2235", name, len))
        || (0 == strncmp("gc2053", name, len))
        || (0 == strncmp("sc3235", name, len))
        || (0 == strncmp("os05a", name, len))
        || (0 == strncmp("soi_f37", name, len))
        || (0 == strncmp("imx307_2l", name, len))
        || (0 == strncmp("imx327_2l", name, len))
        || (0 == strncmp("sc500ai", name, len))
        || (0 == strncmp("gc4653_2l", name, len))
       )

    {
        bus_type = BUS_TYPE_I2C;
    }
    else if (0 == strncmp("imx206", name, len))
    {
        bus_type = BUS_TYPE_SPI;
    }
    else
    {
        printk("FUNC:%s line:%d  SNS is [%s]  !\n", __FUNCTION__,__LINE__, name);
        bus_type = BUS_TYPE_I2C;
    }

    return bus_type;
}

/*
0x0: 74.25MHz; 0x1: 72MHz;0x2: 54MHz;0x3: 24MHz;
0x4: 37.125MHz;0x5: 36MHz;0x6: 27MHz;0x7: 12MHz;
*/
int parse_sensor_clock(const char *name)
{
    int clock = 0x4;
    unsigned int len;

    len = SENSOR_NAME_LEN;

    if (    (0 == strncmp("imx327", name, len))
         || (0 == strncmp("imx307", name, len))
         || (0 == strncmp("imx335", name, len))
         || (0 == strncmp("imx307_2l", name, len))
         || (0 == strncmp("imx327_2l", name, len))
         )
    {
        clock = 0x4;
    }
    else if ((strncmp("sc4236", name, len) == 0)
         || (strncmp("soi_f37", name, len) == 0)
         || (strncmp("os05a", name, len)  == 0)) {
        clock = 0x3;
    }
    else if ((strncmp("sc2235", name, len) == 0)
        || (strncmp("sc2231", name, len) == 0)
        || (strncmp("gc2053", name, len) == 0)
        || (strncmp("gc4653_2l", name, len) == 0)
        || (strncmp("sc3235", name, len) == 0)
        || (strncmp("sc500ai", name, len) == 0))
    {
        clock = 0x6;
    }
    else
    {
        printk("FUNC:%s line:%d  SNS: is [%s]!\n", __FUNCTION__, __LINE__, name);
        return clock;
    }

    return clock;
}

int is_coms(const char *name)
{
    unsigned int len;

    len = SENSOR_NAME_LEN;

    if (   (0 == strncmp("bt1120", name, len))
        || (0 == strncmp("bt656", name, len))
        || (0 == strncmp("bt601", name, len))
        || (0 == strncmp("sc2235", name, len)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int clkcfg(const char *s, int cmos_yuv_flag ,const char *sensor)
{
    SYS_WRITEL(reg_crg_base+0x00A4, 0x00000000);
    if (0 == strncmp("gk7205v200", s, CHIP_NAME_STR_LEN))
    {
        SYS_WRITEL(reg_crg_base+0x00A0, 0x00240400);
        SYS_WRITEL(reg_crg_base+0x00F4, 0x00110000);
        SYS_WRITEL(reg_crg_base+0x00FC, 0x00000008);
    }
    else if (0 == strncmp("gk7202v300", s, CHIP_NAME_STR_LEN))
    {
        SYS_WRITEL(reg_crg_base+0x00A0, 0x00240400);
        SYS_WRITEL(reg_crg_base+0x00F4, 0x00110000);
        SYS_WRITEL(reg_crg_base+0x00FC, 0x00000008);
    }
    else if (   (strncmp("gk7205v300", s, CHIP_NAME_STR_LEN) == 0)
             || (strncmp("gk7605v100", s, CHIP_NAME_STR_LEN) == 0))
    {
        SYS_WRITEL(reg_crg_base+0x00A0, 0x0);
        if ((strncmp("imx335", sensor, SENSOR_NAME_LEN) == 0)
            || (strncmp("os05a", sensor, SENSOR_NAME_LEN) == 0)
            || (strncmp("soi_f37", sensor, SENSOR_NAME_LEN) == 0))
        {
            SYS_WRITEL(reg_crg_base + 0x00F4, 0x00000000);
        }
        else
        {
            SYS_WRITEL(reg_crg_base+0x00F4, 0x00110000);
        }
          SYS_WRITEL(reg_crg_base+0x00FC, 0x00000008);
    }
    else
    {

    }

    if (cmos_yuv_flag > 0)
    {
        SYS_WRITEL(reg_crg_base+0x00F4, 0x00038000);    /* VICAP:coms*/
    }

    return 0;
}

void set_vi_vpss_mode(int online_flag)
{
    if (0 == online_flag || 1 == online_flag) /* VI_OFFLINE_VPSS_OFFLINE/VI_OFFLINE_VPSS_ONLINE */
    {
        SYS_WRITEL(reg_misc_base + 0x080, 0x00060005);
        SYS_WRITEL(reg_misc_base + 0x084, 0x54755676);
        SYS_WRITEL(reg_misc_base + 0x088, 0x00000006);
        SYS_WRITEL(reg_misc_base + 0x090, 0x00070005);
        SYS_WRITEL(reg_misc_base + 0x094, 0x54665677);
        SYS_WRITEL(reg_misc_base + 0x098, 0x00000006);
        SYS_WRITEL(reg_misc_base + 0x058, 0x00000001);
        SYS_WRITEL(reg_misc_base + 0x070, 0x00000123);
        SYS_WRITEL(reg_misc_base + 0x07c, 0x00000123);

    }
    else if(2 == online_flag)/* VI_ONLINE_VPSS_OFFLINE */
    {
        SYS_WRITEL(reg_misc_base + 0x080, 0x00060005);
        SYS_WRITEL(reg_misc_base + 0x084, 0x44654766);
        SYS_WRITEL(reg_misc_base + 0x088, 0x00000006);
        SYS_WRITEL(reg_misc_base + 0x090, 0x00070005);
        SYS_WRITEL(reg_misc_base + 0x094, 0x54655777);
        SYS_WRITEL(reg_misc_base + 0x098, 0x00000006);
        SYS_WRITEL(reg_misc_base + 0x058, 0x00000001);
        SYS_WRITEL(reg_misc_base + 0x070, 0x00000123);
        SYS_WRITEL(reg_misc_base + 0x07c, 0x00000123);
    }
    else/* VI_ONLINE_VPSS_ONLINE */
    {
        SYS_WRITEL(reg_misc_base + 0x080, 0x00060005);
        SYS_WRITEL(reg_misc_base + 0x084, 0x54655766);
        SYS_WRITEL(reg_misc_base + 0x088, 0x00000006);
        SYS_WRITEL(reg_misc_base + 0x090, 0x00070005);
        SYS_WRITEL(reg_misc_base + 0x094, 0x54755777);
        SYS_WRITEL(reg_misc_base + 0x098, 0x00000006);
        SYS_WRITEL(reg_misc_base + 0x058, 0x00000001);
        SYS_WRITEL(reg_misc_base + 0x070, 0x00000123);
        SYS_WRITEL(reg_misc_base + 0x07c, 0x00000123);
    }
}
EXPORT_SYMBOL(set_vi_vpss_mode);

void sys_ctl(int online_flag)
{

    set_vi_vpss_mode(online_flag);

    SYS_WRITEL(reg_misc_base + 0x000, 0x0); /*VGS&GZIP, VEDU&Boot Ram*/
    SYS_WRITEL(reg_misc_base + 0x010, 0x35); /*VGS GZIP*/

    SYS_WRITEL(reg_ddr_base + 0x0200, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0210, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0220, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0230, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0240, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0250, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0260, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0270, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0280, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x0290, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x02a0, 0x00110000);
    SYS_WRITEL(reg_ddr_base + 0x02b0, 0x00110000);

    SYS_WRITEL(reg_ddr_base + 0x0204, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0214, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0224, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0234, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0244, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0254, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0264, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0274, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0284, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0294, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x02a4, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x02b4, 0x01234567);

    SYS_WRITEL(reg_ddr_base + 0x0208, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0218, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0228, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0238, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0248, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0258, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0268, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0278, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0288, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x0298, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x02a8, 0x01234567);
    SYS_WRITEL(reg_ddr_base + 0x02b8, 0x01234567);

    SYS_WRITEL(reg_ddr_base + 0x410c, 0x0000000b);
    SYS_WRITEL(reg_ddr_base + 0x4110, 0x0000000b);
    SYS_WRITEL(reg_ddr_base + 0x41f0, 0x00000001);
    SYS_WRITEL(reg_ddr_base + 0x40ac, 0x00000000);
    SYS_WRITEL(reg_ddr_base + 0x4300, 0x00000080);
}

void i2c0_for_mipi_sensor_pin_mux_gk7205v300(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0030, 0x1d00);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0034, 0x1d00);
    if (0 == strncmp("sck", board_list, BOARD_NAME_LEN)) {
        udelay(5);
    }
    SYS_WRITEL(reg_iocfg_vio_base + 0x0030, 0x1d01);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0034, 0x1d01);
}
void i2c0_for_mipi_sensor_pin_mux_gk7205v200(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0030, 0x1c00);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0034, 0x1c00);
    if (0 == strncmp("sck", board_list, BOARD_NAME_LEN)) {
        udelay(5);
    }
    SYS_WRITEL(reg_iocfg_vio_base + 0x0030, 0x1c01);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0034, 0x1c01);
}

void i2c0_for_vi_raw_sensor_pin_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0060, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0064, 0x1002);
}

void i2c0_pin_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0030, 0x1d01);
    SYS_WRITEL(reg_iocfg_vio_base+0x0034, 0x1d01);
}

void i2c1_pin_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x00A0, 0x1c01);
    SYS_WRITEL(reg_iocfg_vio_base+0x00A4, 0x1c01);
    SYS_WRITEL(reg_iocfg_ahb_base+0x009C, 0x1000);

    reg_write32(0x1, 0x1, reg_gpio_base+0x9400);
    reg_write32(0x0, 0x1, reg_gpio_base+0x9004);
    udelay(100);
    reg_write32(0x1, 0xF, reg_gpio_base+0x9004);
}

void aic31_reset_gk7205v200_sck(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0044, 0x1a00);
    SYS_WRITEL(reg_gpio_base+0x5400, 0x00ff);
    SYS_WRITEL(reg_gpio_base+0x5008, 0x0000);
    udelay(100);
    SYS_WRITEL(reg_gpio_base+0x5008, 0x0002);
}

void aic31_reset_gk7205v300_sck(void)
{
    SYS_WRITEL(reg_iocfg_ahb_base+0x009C, 0x1000);
    SYS_WRITEL(reg_gpio_base+0x9400, 0x0001);
    SYS_WRITEL(reg_gpio_base+0x9004, 0x0000);
    udelay(100);
    SYS_WRITEL(reg_gpio_base+0x9004, 0x0001);
}

void i2c1_for_aic31_pin_mux_gk7205v200(void)
{
    SYS_WRITEL(reg_iocfg_ahb_base + 0x000c, 0x0603); /* 0x0:GPIO0_3, 0x3:I2C1_SCL */
    SYS_WRITEL(reg_iocfg_ahb_base + 0x0010, 0x1003); /* 0x0:GPIO0_4, 0x3:I2C1_SDA */
}

void i2c1_for_aic31_pin_mux_gk7205v300(void)
{
    SYS_WRITEL(reg_iocfg_ahb_base + 0x000c, 0x1033); /* 0x0:GPIO0_3, 0x3:I2C1_SCL */
    SYS_WRITEL(reg_iocfg_ahb_base + 0x0010, 0x1033); /* 0x0:GPIO0_4, 0x3:I2C1_SDA */
}

void i2c2_for_mipi_sensor_pin_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0038, 0x1c01);
    SYS_WRITEL(reg_iocfg_vio_base+0x003c, 0x1c01);
}

void i2c2_for_vi_raw_sensor_pin_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0058, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x005c, 0x1002);
}

void i2c2_pin_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x003c, 0x1c01);
    SYS_WRITEL(reg_iocfg_vio_base+0x0038, 0x1c01);

    SYS_WRITEL(reg_iocfg_vio_base+0x0044, 0x1a00);

    SYS_WRITEL(reg_gpio_base+0x5400, 0x02);
    SYS_WRITEL(reg_gpio_base+0x5008, 0x00);
    udelay(100);
    SYS_WRITEL(reg_gpio_base+0x5008, 0x02);

}


void spi0_pin_mux(void)
{
    reg_write32(0x1 << 12, 0x1 << 12, reg_crg_base+0x01bc); /* SPI0 clock for LCD*/

    SYS_WRITEL(reg_iocfg_vio_base+0x00A0, 0x1024);
    SYS_WRITEL(reg_iocfg_vio_base+0x00A4, 0x1024);
    SYS_WRITEL(reg_iocfg_vio_base+0x009C, 0x1034);

    SYS_WRITEL(reg_iocfg_ahb_base+0x009C, 0x1000);
    SYS_WRITEL(reg_gpio_base+0x9400, 0x0001);
    SYS_WRITEL(reg_gpio_base+0x9004, 0x0000);
    udelay(100);
    SYS_WRITEL(reg_gpio_base+0x9004, 0x0001);
}

void spi0_pin_mux_gk7205v200(void)
{
    reg_write32(0x1 << 12, 0x1 << 12, reg_crg_base+0x01bc); /* SPI0 clock for LCD*/

    SYS_WRITEL(reg_iocfg_vio_base+0x0038, 0x1037);
    SYS_WRITEL(reg_iocfg_vio_base+0x003C, 0x1407);
    SYS_WRITEL(reg_iocfg_vio_base+0x0040, 0x1037);

    SYS_WRITEL(reg_iocfg_vio_base+0x0044, 0x1a00);
    SYS_WRITEL(reg_gpio_base+0x5400, 0x0002);
    SYS_WRITEL(reg_gpio_base+0x5008, 0x0000);
    udelay(100);
    SYS_WRITEL(reg_gpio_base+0x5008, 0x0002);
}

void spi1_pin_mux(void)
{
    SYS_WRITEL(reg_iocfg_ahb_base+0x008C, 0x1011);
    SYS_WRITEL(reg_iocfg_ahb_base+0x0084, 0x1431);
    SYS_WRITEL(reg_iocfg_ahb_base+0x0090, 0x1001);
    SYS_WRITEL(reg_iocfg_ahb_base+0x0088, 0x1011);
}

void sensor_cfg_for_mipi_sensor_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0028, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x002c, 0x1001);

}

void sensor_cfg_for_raw_sensor_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0048, 0x1a02);
    SYS_WRITEL(reg_iocfg_vio_base+0x0054, 0x1a02);

}

void vi_mipi_rx_mux_gk7205v300(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0000, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0004, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0008, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x000C, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0010, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0014, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0018, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x001c, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0020, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0024, 0x1000);
}

void vi_mipi_rx_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0000, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0004, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0008, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x000C, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0010, 0x1000);
    SYS_WRITEL(reg_iocfg_vio_base+0x0014, 0x1000);
}
EXPORT_SYMBOL(vi_mipi_rx_mux);

void vi_bt1120_mode_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0000, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0004, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0008, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x000C, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0010, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0014, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0018, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x001c, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0020, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0024, 0x1001);

    SYS_WRITEL(reg_iocfg_vio_base+0x0060, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x004C, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0050, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0070, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0068, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x0058, 0x1001);
    SYS_WRITEL(reg_iocfg_vio_base+0x005C, 0x1001);
}


void vi_bt656_mode_mux(void)
{

}
void vi_raw_mode_mux_gk7205v300(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0000, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0004, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0008, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x000C, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0010, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0014, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0018, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x001c, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0020, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0024, 0x1003);

    SYS_WRITEL(reg_iocfg_vio_base+0x0050, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x004C, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x005C, 0x1007);
    SYS_WRITEL(reg_iocfg_vio_base+0x0060, 0x1007);
    SYS_WRITEL(reg_iocfg_vio_base+0x0064, 0x1007);
}

void vi_bt656_mux_gk7205v300(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0000, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0004, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0008, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x000C, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0010, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0018, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x001c, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0020, 0x1003);
    SYS_WRITEL(reg_iocfg_vio_base+0x0024, 0x1003);
}
void vi_raw_mode_mux_gk7205v200(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0000, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0004, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x000C, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0008, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0014, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0010, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0028, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x002C, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0030, 0x1C02);
    SYS_WRITEL(reg_iocfg_vio_base+0x0034, 0x1C02);
    SYS_WRITEL(reg_iocfg_vio_base+0x0050, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x004C, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0040, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x003C, 0x1C02);
    SYS_WRITEL(reg_iocfg_vio_base+0x0038, 0x1C02);
}

void vi_slave_mode_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x009C, 0x1002);
    SYS_WRITEL(reg_iocfg_vio_base+0x0098, 0x1002);
}

void vo_bt1120_mode_mux_gk7205v300(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0048, 0x1A46);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0074, 0x1066);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0070, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0080, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0084, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0088, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x008C, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0090, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0094, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x006C, 0x1A26);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0068, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0058, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x005C, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0060, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0064, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x004C, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0050, 0x1026);
}

void vo_bt1120_mode_mux_gk7205v200(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0048, 0x1A46);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0074, 0x1066);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0070, 0x1026);
    SYS_WRITEL(reg_iocfg_ahb_base + 0x0040, 0x1A66);
    SYS_WRITEL(reg_iocfg_ahb_base + 0x0044, 0x1166);
    SYS_WRITEL(reg_iocfg_ahb_base + 0x0048, 0x1166);
    SYS_WRITEL(reg_iocfg_ahb_base + 0x004C, 0x1166);
    SYS_WRITEL(reg_iocfg_ahb_base + 0x0050, 0x1166);
    SYS_WRITEL(reg_iocfg_ahb_base + 0x0054, 0x1166);
    SYS_WRITEL(reg_iocfg_vio_base + 0x006C, 0x1A26);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0068, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0058, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x005C, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0060, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0064, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x004C, 0x1026);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0050, 0x1026);
}

void vo_bt656_mode_mux_gk7205v300(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0048, 0x1a43);
    SYS_WRITEL(reg_iocfg_vio_base+0x006C, 0x1a03);
    SYS_WRITEL(reg_iocfg_vio_base+0x0068, 0x1023);
    SYS_WRITEL(reg_iocfg_vio_base+0x0058, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x005C, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x0060, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x0064, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x004C, 0x1023);
    SYS_WRITEL(reg_iocfg_vio_base+0x0050, 0x1023);
}
void vo_bt656_mode_mux_gk7205v200(void)
{
    SYS_WRITEL(reg_iocfg_vio_base+0x0048, 0x1a63);
    SYS_WRITEL(reg_iocfg_vio_base+0x006C, 0x1a33);
    SYS_WRITEL(reg_iocfg_vio_base+0x0068, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x0058, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x005C, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x0060, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x0064, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x004C, 0x1033);
    SYS_WRITEL(reg_iocfg_vio_base+0x0050, 0x1033);
}


void vo_16bit_lcd_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0048, 0x1A65);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0074, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0070, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0080, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0084, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0088, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x008C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0090, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0094, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x006C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0068, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0058, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x005C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0060, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0064, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0078, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x007C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0054, 0x1A35);
    SYS_WRITEL(reg_iocfg_vio_base + 0x004C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0050, 0x1035);

    reg_write32(0x1 << 7, 0x1 << 7, reg_crg_base+0x01bc); /* PWM clock for LCD*/
    SYS_WRITEL(reg_iocfg_ahb_base+0x0010, 0x1001); /* PWM 1*/
    SYS_WRITEL(reg_pwm_base+0x0020, 0x3e8);
    SYS_WRITEL(reg_pwm_base+0x0024, 0x355);
    SYS_WRITEL(reg_pwm_base+0x0028, 0xa);
    SYS_WRITEL(reg_pwm_base+0x002c, 0x5);
}

void vo_8bit_lcd_for_sckb_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0048, 0x1A65);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0074, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0070, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x006C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0068, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0058, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x005C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0060, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0064, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0054, 0x1A35);
    SYS_WRITEL(reg_iocfg_vio_base + 0x004c, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0050, 0x1035);
}

void vo_8bit_lcd_for_sckb_mux_gk7205v200(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0048, 0x1A55);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0074, 0x1075);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0070, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x006C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0068, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0058, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x005C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0060, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0064, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0054, 0x1A35);
    SYS_WRITEL(reg_iocfg_vio_base + 0x004c, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0050, 0x1035);
}

void vo_8bit_lcd_for_demo_mux(void)
{
    SYS_WRITEL(reg_iocfg_vio_base + 0x0048, 0x1A65);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0074, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0070, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x006C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0068, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0058, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x005C, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0060, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0064, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0054, 0x1A35);
    SYS_WRITEL(reg_iocfg_vio_base + 0x004c, 0x1035);
    SYS_WRITEL(reg_iocfg_vio_base + 0x0050, 0x1035);
}

void i2s0_pin_mux_gk7205v200(void)
{
    /* reuse with JTAG, open when running audio */
    SYS_WRITEL(reg_iocfg_core_base + 0x0010, 0x0025); /* I2S_MCLK */
    SYS_WRITEL(reg_iocfg_core_base + 0x0014, 0x1E65); /* I2S_BCLK */
    SYS_WRITEL(reg_iocfg_core_base + 0x0018, 0x1D35); /* I2S_WS */
    SYS_WRITEL(reg_iocfg_core_base + 0x001C, 0x1405); /* I2S_SD_RX */
    SYS_WRITEL(reg_iocfg_core_base + 0x0020, 0x1D35); /* I2S_SD_TX */
}

void i2s0_pin_mux_gk7205v300(void)
{
    /* reuse with JTAG, open when running audio */
    SYS_WRITEL(reg_iocfg_core_base + 0x0010, 0x0025); /* I2S_MCLK */
    SYS_WRITEL(reg_iocfg_core_base + 0x0014, 0x1E65); /* I2S_BCLK */
    SYS_WRITEL(reg_iocfg_core_base + 0x0018, 0x1D35); /* I2S_WS */
    SYS_WRITEL(reg_iocfg_core_base + 0x001C, 0x1405); /* I2S_SD_RX */
    SYS_WRITEL(reg_iocfg_core_base + 0x0020, 0x1D35); /* I2S_SD_TX */
}

void amp_unmute_mux_gk7205v200_demo(void)
{
    SYS_WRITEL(reg_iocfg_core_base + 0x0000, 0x1000); /* 0x0:GPIO1_0(now), 0x1:LSADC_CH0 */
    reg_write32(0x1, 0x1, reg_gpio_base + 0x1400);
    reg_write32(0x1, 0x1, reg_gpio_base + 0x1004);
}

void amp_unmute_mux_gk7205v300_sck(void)
{
    SYS_WRITEL(reg_iocfg_ahb_base+0x00A0, 0x1000); /* 0x0:GPIO9_1(now), 0x1:PWM3 */
    reg_write32(0x1 << 1, 0x1 << 1, reg_gpio_base+0x9400);
    reg_write32(0x1 << 1, 0x1 << 1, reg_gpio_base+0x9008);
}

void amp_unmute_mux_gk7202v300(void)
{
    SYS_WRITEL(reg_iocfg_core_base+0x0014, 0x1E02); /* 0x0:JTAG_TCK, 0x1:SPI1_SCLK, 0x2:GPIO1_5(now), 0x4:UART2_TXD, 0x5:I2S_BCLK */
    reg_write32(0x1 << 5, 0x1 << 5, reg_gpio_base+0x1400);
    reg_write32(0x1 << 5, 0x1 << 5, reg_gpio_base+0x1080);
    //SYS_WRITEL(reg_gpio_base+0x1400, 0x20);
    //SYS_WRITEL(reg_gpio_base+0x1080, 0x20);
}

void lsadc_mux(void)
{
	SYS_WRITEL(reg_iocfg_core_base+0x0000, 0x1001); /* 0x0:GPIO1_0, 0x1:LSADC_CH0(now) */
	SYS_WRITEL(reg_iocfg_core_base+0x0004, 0x1001); /* 0x0:GPIO1_1, 0x1:LSADC_CH1(now) */
}

void ir_mux(void)
{
	SYS_WRITEL(reg_iocfg_ahb_base+0x0008, 0x1D02); /* 0x0:GPIO0_0, 0x1:UPDATE_MODE, 0x2:IR_IN(now) */
}

int pinmux(const char *chip_name, const char *board, const int cmos_yuv_flag)
{
    if (0 == strncmp("gk7202v300", chip_name, CHIP_NAME_STR_LEN))
    {
        if(!g_quick_start_flag)
        {
            i2c0_for_mipi_sensor_pin_mux_gk7205v200();
            sensor_cfg_for_mipi_sensor_mux();
        }
        vi_mipi_rx_mux();

        /*audio*/
        amp_unmute_mux_gk7202v300();
    }
    else if (0 == strncmp("gk7205v200", chip_name, CHIP_NAME_STR_LEN))
    {
        if (0 == strncmp("demo", board, BOARD_NAME_LEN))
        {
             /*mipi*/
            if (cmos_yuv_flag == 0)
            {
                i2c0_for_mipi_sensor_pin_mux_gk7205v200();
                sensor_cfg_for_mipi_sensor_mux();
                vi_mipi_rx_mux();
            } /*cmos*/
            else if (cmos_yuv_flag == 1)
            {
                i2c0_for_vi_raw_sensor_pin_mux();
                sensor_cfg_for_raw_sensor_mux();
                vi_raw_mode_mux_gk7205v200();
            }
            else
            {
            }

            /*audio*/
            amp_unmute_mux_gk7205v200_demo();
        }
        else if (0 == strncmp("sck", board, BOARD_NAME_LEN))
        {
            i2c0_for_mipi_sensor_pin_mux_gk7205v200();
            sensor_cfg_for_mipi_sensor_mux();
            vi_mipi_rx_mux();

            i2c2_pin_mux();
            vo_bt1120_mode_mux_gk7205v200();
            //vo_bt656_mode_mux_gk7205v200();
            //spi0_pin_mux_gk7205v200(); //lcd
            //vo_8bit_lcd_for_sckb_mux_gk7205v200();

            /*audio*/
            amp_unmute_mux_gk7205v200_demo();
            //aic31_reset_gk7205v200_sck();
            //i2c1_for_aic31_pin_mux_gk7205v200();
            //i2s0_pin_mux_gk7205v200();
        }
    }
    else if (0 == strncmp("gk7205v300", chip_name, CHIP_NAME_STR_LEN))
    {
        if (0 == strncmp("demo", board, BOARD_NAME_LEN))
        {
            i2c0_for_mipi_sensor_pin_mux_gk7205v300();
            sensor_cfg_for_mipi_sensor_mux();

            spi0_pin_mux(); //lcd
            vo_8bit_lcd_for_demo_mux();

            /*mipi*/
            if (cmos_yuv_flag == 0)
            {
                vi_mipi_rx_mux_gk7205v300();
            } /*cmos*/
            else if (cmos_yuv_flag == 1)
            {
                vi_raw_mode_mux_gk7205v300();
            }
            else
            {
            }

            /*audio*/
            amp_unmute_mux_gk7205v200_demo();
        }
        else if (0 == strncmp("sck", board, BOARD_NAME_LEN))
        {
            i2c1_pin_mux();
            vo_bt1120_mode_mux_gk7205v300();
            //vo_bt656_mode_mux_gk7205v300();

            //spi0_pin_mux(); //lcd
            //vo_8bit_lcd_for_sckb_mux();
            //vo_16bit_lcd_mux();

            /*mipi*/
            if (cmos_yuv_flag == 0)
            {
                i2c0_for_mipi_sensor_pin_mux_gk7205v300();
                sensor_cfg_for_mipi_sensor_mux();
                vi_mipi_rx_mux_gk7205v300();
            } /*cmos*/
            else if (cmos_yuv_flag == 1)
            {
                i2c0_for_mipi_sensor_pin_mux_gk7205v300();
                sensor_cfg_for_mipi_sensor_mux();
                vi_raw_mode_mux_gk7205v200();
            }
            /*bt1120*/
            else if (cmos_yuv_flag == 2) {
                spi1_pin_mux();//bt1120
                vi_bt1120_mode_mux();
            } else if (cmos_yuv_flag == 3) {//bt656
               i2c0_for_mipi_sensor_pin_mux_gk7205v300();
               sensor_cfg_for_mipi_sensor_mux();
               vi_bt656_mux_gk7205v300();
            }
            else
            {
            }

            /*audio*/
            amp_unmute_mux_gk7205v300_sck();
            //aic31_reset_gk7205v300_sck();
            //i2c1_for_aic31_pin_mux_gk7205v300();
            //i2s0_pin_mux_gk7205v300();
        }

    }
    else
    {
    }

	/*ir pin mux*/
	//ir_mux();

	/*lsadc pin mux*/
	//lsadc_mux();

    return 0;
}


void sensor_bus_pin_mux(int index, BUS_TYPE bus_type, const char *name)
{
    unsigned int len;

    len = SENSOR_NAME_LEN;

    if (0 == strncmp("imx206", name, len))
    {
        //vi_slave_mode_mux();
    }

    if (BUS_TYPE_I2C == bus_type)
    {
    }
    else if (BUS_TYPE_SPI == bus_type)
    {
    }
}


int sensor_config(const char *sensor_name)
{
    int index = 0;
    int clock;
    BUS_TYPE bus_type;

    //printk("\n==========sensor: %s==========\n",sensor_name);

    clock = parse_sensor_clock(sensor_name);
    bus_type = parse_sensor_bus_type(sensor_name);

    sensor_bus_pin_mux(index, bus_type, sensor_name);
    sensor_clock_config(index, clock);
    return 0;
}

void  sysconfig_exit(void);
void sysconfig_instant_exit(void);

int  sysconfig_init(void)
{
    reg_crg_base = (void*)ioremap_nocache(0x12010000, 0X10000);
    if (NULL == reg_crg_base)
    {
        goto out;
    }

    reg_sysctl_base = (void*)ioremap_nocache(0x12020000, 0x8000);
    if (NULL == reg_sysctl_base)
    {
        goto out;
    }

    reg_misc_base = (void*)ioremap_nocache(0x12028000, 0x8000);
    if (NULL == reg_misc_base)
    {
        goto out;
    }

    reg_ddr_base = (void*)ioremap_nocache(0x120d0000, 0x10000);
    if (NULL == reg_ddr_base)
    {
        goto out;
    }

    reg_iocfg_vio_base = (void*)ioremap_nocache(0x112c0000, 0x10000);
    if (NULL == reg_iocfg_vio_base)
    {
        goto out;
    }

    reg_iocfg_core_base = (void*)ioremap_nocache(0x120c0000, 0x10000);
    if (NULL == reg_iocfg_core_base)
    {
        goto out;
    }

    reg_iocfg_ahb_base = (void*)ioremap_nocache(0x100c0000, 0x10000);
    if (NULL == reg_iocfg_ahb_base)
    {
        goto out;
    }

    reg_gpio_base = (void*)ioremap_nocache(0x120b0000, 0x10000);
    if (NULL == reg_gpio_base)
    {
        goto out;
    }

	reg_pwm_base = (void*)ioremap_nocache(0x12080000, 0x10000);
    if (NULL == reg_pwm_base)
    {
        goto out;
    }

    if (strncmp("gk7605v100", chip_list, CHIP_NAME_STR_LEN) == 0) {
        pinmux("gk7205v300", board_list, g_cmos_yuv_flag);
        clkcfg("gk7205v300", g_cmos_yuv_flag, sensor_list);
    }
    else {
        pinmux(chip_list, board_list, g_cmos_yuv_flag);
        clkcfg(chip_list, g_cmos_yuv_flag, sensor_list);
    }

    sys_ctl(g_online_flag);

    if(!g_quick_start_flag)
    {
        sensor_config(sensor_list);
    }

    printk("==== online_flag=%d, cmos_yuv_flag=%d, sensor=%s, chip=%s, board=%s====\n",
        g_online_flag, g_cmos_yuv_flag, sensor_list, chip_list, board_list);
    printk("==== g_quick_start_flag=%d ====\n",g_quick_start_flag);
    printk("sysconfig init success!\n");

out:
    sysconfig_instant_exit();
    return 0;
}

void sysconfig_instant_exit(void)
{
    if (NULL != reg_crg_base)
    {
        iounmap(reg_crg_base);
        reg_crg_base = 0;
    }

    if (NULL != reg_sysctl_base)
    {
        iounmap(reg_sysctl_base);
        reg_sysctl_base = 0;
    }

    if (NULL != reg_ddr_base)
    {
        iounmap(reg_ddr_base);
        reg_ddr_base = 0;
    }

    if (NULL != reg_iocfg_core_base)
    {
        iounmap(reg_iocfg_core_base);
        reg_iocfg_core_base = 0;
    }

    if (NULL != reg_iocfg_ahb_base)
    {
        iounmap(reg_iocfg_ahb_base);
        reg_iocfg_ahb_base = 0;
    }

    if (NULL != reg_gpio_base)
    {
        iounmap(reg_gpio_base);
        reg_gpio_base = 0;
    }

    if (NULL != reg_pwm_base)
    {
        iounmap(reg_pwm_base);
        reg_pwm_base = 0;
    }

    return;
}

void __exit sysconfig_exit(void)
{
    if (NULL != reg_misc_base)
    {
        iounmap(reg_misc_base);
        reg_misc_base = 0;
    }

    if (NULL != reg_iocfg_vio_base)
    {
        iounmap(reg_iocfg_vio_base);
        reg_iocfg_vio_base = 0;
    }
    sysconfig_instant_exit();

    return;
}



#ifndef __LITEOS__
module_param(g_online_flag, int, S_IRUGO);
module_param(g_cmos_yuv_flag, int, 0600);
module_param_string(sensors, sensor_list, SENSOR_NAME_LEN, 0600);
module_param_string(chip, chip_list, CHIP_NAME_STR_LEN, 0600);
module_param_string(board, board_list, BOARD_NAME_LEN, 0600);

module_init(sysconfig_init);
module_exit(sysconfig_exit);
#endif



MODULE_LICENSE("GPL");
