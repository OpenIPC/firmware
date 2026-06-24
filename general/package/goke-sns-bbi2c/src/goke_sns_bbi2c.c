// SPDX-License-Identifier: GPL-2.0
/* Bit-bang i2c adapter for the SmartSens SC223A (chip-id 0xcb3e) on a Goke
 * gk7205v200, as used in CP Plus CP-UNC-TA21L2C / Dahua-OEM cameras. On these
 * boards the sensor i2c is wired to GPIO (SDA=GPIO7_2, SCL=GPIO7_3), NOT the SoC
 * hardware i2c controller, so the stock probe never ACKs. This module registers
 * a bit-bang i2c adapter (/dev/i2c-N), drives the sensor reset/enable line
 * (GPIO8_7) high and the 27MHz MCLK, and holds the MIPI-RX reference clock
 * stable so the PHY locks. majestic's libsns reaches the sensor via this adapter
 * (the init script remaps /dev/i2c-0 to it).
 *
 * v2: per-xfer setup re-asserts GPIO8_7 + pads + MCLK(f0) ONLY. f4 (sensor-clk
 * divider) is set ONCE at init and NEVER touched per-xfer, so it does not fight
 * majestic's streaming clock. All values are module params for experimentation.
 *   permclk=1  force f0=mclkval on every xfer (override majestic's 0x11)
 *   mclkval=0x19
 *   initf4=0x38700  value written to f4 at module init (0 = leave alone)
 *   rstpulse=1  pulse GPIO8_7 low->high at init (clean sensor reset)
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO7_PA 0x120b7000
#define GPIO8_PA 0x120b8000
#define IOMUX_PA 0x112c0000
#define CRG_PA   0x12010000
#define SCL 3   /* GPIO7_3 */
#define SDA 2   /* GPIO7_2 */

static int permclk = 1;     module_param(permclk, int, 0644);
static int mclkval = 0x19;  module_param(mclkval, int, 0644);
static int initf4  = 0x38700; module_param(initf4, int, 0644);
static int rstpulse = 1;    module_param(rstpulse, int, 0644);
static int dly = 5;         module_param(dly, int, 0644);

static void __iomem *g7,*g8,*iom,*crg;
static inline void udly(void){ udelay(dly); }
static void pin_out(int pin,int v){ writel(readl(g7+0x400)|(1<<pin), g7+0x400); writel(v?(1<<pin):0, g7+((1<<pin)<<2)); }
static void pin_in(int pin){ writel(readl(g7+0x400)&~(1<<pin), g7+0x400); }
static int  pin_get(int pin){ return (readl(g7+0x3fc)>>pin)&1; }
static void scl(int v){ pin_out(SCL,v); udly(); }
static void sda1(void){ pin_in(SDA); udly(); }
static void sda0(void){ pin_out(SDA,0); udly(); }
static int  sdar(void){ pin_in(SDA); udly(); return pin_get(SDA); }
static void i2c_start(void){ sda1(); scl(1); sda0(); scl(0); }
static void i2c_stop(void){ sda0(); scl(1); sda1(); udly(); }
static int  wbyte(int b){ int i,a; for(i=7;i>=0;i--){ if((b>>i)&1) sda1(); else sda0(); scl(1); scl(0);} sda1(); scl(1); a=sdar(); scl(0); return a; }
static int  rbyte(int nack){ int i,v=0; sda1(); for(i=7;i>=0;i--){ scl(1); v=(v<<1)|sdar(); scl(0);} if(nack) sda1(); else sda0(); scl(1); scl(0); return v; }

/* Enable sensor: GPIO8_7 reset/enable HIGH, MCLK pad+f0, i2c pads. NOT f4. */
static void sns_enable(void){
    if (permclk) writel(mclkval, crg+0xf0);                 /* MCLK 27MHz (override majestic 0x11) */
    writel(0x1001, iom+0x28);                               /* SENSOR_CLK pad func1 */
    writel(0x1000, iom+0x60); writel(0x1000, iom+0x64);     /* i2c pads -> GPIO func0 */
    writel(0x1000, iom+0x48);                               /* GPIO8_7 pad -> GPIO func0 */
    writel(readl(g8+0x400)|0x80, g8+0x400);                 /* GPIO8_7 dir = output */
    writel(0x80, g8+0x200);                                 /* GPIO8_7 = HIGH (enable sensor) */
}

static int bb_xfer(struct i2c_adapter *a, struct i2c_msg *msgs, int num){
    int i,j;
    sns_enable();
    for(i=0;i<num;i++){
        struct i2c_msg *m=&msgs[i];
        int rd = m->flags & I2C_M_RD;
        i2c_start();
        if(wbyte((m->addr<<1)|(rd?1:0))!=0){ i2c_stop(); return -ENXIO; }
        if(rd){ for(j=0;j<m->len;j++) m->buf[j]=rbyte(j==m->len-1); }
        else  { for(j=0;j<m->len;j++) if(wbyte(m->buf[j])!=0){ i2c_stop(); return -EIO; } }
    }
    i2c_stop();
    return num;
}
static u32 bb_func(struct i2c_adapter *a){ return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL; }
static const struct i2c_algorithm bb_algo = { .master_xfer=bb_xfer, .functionality=bb_func };
static struct i2c_adapter bb_adap = { .owner=THIS_MODULE, .algo=&bb_algo, .name="goke-sns-bbi2c" };

static int __init bb_init(void){
    int r;
    g7=ioremap(GPIO7_PA,0x1000); g8=ioremap(GPIO8_PA,0x1000);
    iom=ioremap(IOMUX_PA,0x1000); crg=ioremap(CRG_PA,0x1000);
    if (initf4) writel(initf4, crg+0xf4);                   /* sensor-clk divider, init value */
    sns_enable();
    if (rstpulse) {                                         /* clean reset pulse so sensor wakes */
        writel(0x80, g8+0x000);                             /* (mask addr 0x000 -> all bits) keep state */
        writel(0x00, g8+0x200); msleep(5);                  /* GPIO8_7 low (assert reset) */
        writel(0x80, g8+0x200); msleep(20);                 /* GPIO8_7 high (release) */
    }
    msleep(20);
    r = i2c_add_adapter(&bb_adap);
    pr_info("goke-sns-bbi2c v2: add_adapter=%d nr=%d permclk=%d mclk=0x%x initf4=0x%x\n",
            r, bb_adap.nr, permclk, mclkval, initf4);
    return r;
}
static void __exit bb_exit(void){
    i2c_del_adapter(&bb_adap);
    iounmap(g7); iounmap(g8); iounmap(iom); iounmap(crg);
}
module_init(bb_init); module_exit(bb_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SC223A bit-bang i2c + sensor enable for gk7205v200 GPIO-wired boards");
