#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define IOCFG2_BASE  0x17940000
#define IOCFG2_SIZE  0x1000
#define GPIO6_BASE   0x11096000
#define GPIO6_SIZE   0x1000

/* I2C0 pin mux registers */
#define I2C0_SDA_REG (IOCFG2_BASE + 0x98)
#define I2C0_SCL_REG (IOCFG2_BASE + 0x9C)

/* IOCFG value for I2C function (func 5) */
#define IOCFG_I2C_FUNC  0x1135
/* IOCFG value for GPIO function (func 0) with pull-up */
#define IOCFG_GPIO_FUNC 0x1100

/* GPIO register offsets */
#define GPIO_DATA   0x3FC
#define GPIO_DIR    0x400
#define GPIO_SET    0x100
#define GPIO_CLR    0x140

static volatile uint32_t *map_reg(uint32_t phys_addr, size_t size)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        fprintf(stderr, "open /dev/mem: %s\n", strerror(errno));
        return NULL;
    }
    void *map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phys_addr);
    close(fd);
    if (map == MAP_FAILED) {
        fprintf(stderr, "mmap 0x%x: %s\n", phys_addr, strerror(errno));
        return NULL;
    }
    return (volatile uint32_t *)map;
}

static inline void reg_write(volatile uint32_t *base, uint32_t offset, uint32_t val)
{
    *(base + offset / 4) = val;
}

static inline uint32_t reg_read(volatile uint32_t *base, uint32_t offset)
{
    return *(base + offset / 4);
}

static inline void udelay(int us)
{
    usleep(us);
}

static inline void mdelay(int ms)
{
    usleep(ms * 1000);
}

static void gpio_set(volatile uint32_t *gpio, int pin, int val)
{
    if (val)
        reg_write(gpio, GPIO_SET, 1 << pin);
    else
        reg_write(gpio, GPIO_CLR, 1 << pin);
}

static void gpio_dir(volatile uint32_t *gpio, int pin, int output)
{
    uint32_t dir = reg_read(gpio, GPIO_DIR);
    if (output)
        dir |= (1 << pin);
    else
        dir &= ~(1 << pin);
    reg_write(gpio, GPIO_DIR, dir);
}

static int gpio_get(volatile uint32_t *gpio, int pin)
{
    uint32_t data = reg_read(gpio, GPIO_DATA);
    return (data >> pin) & 1;
}

int main(int argc, char *argv[])
{
    fprintf(stderr, "=== I2C Bus Recovery (GPIO bit-bang) ===\n");

    /* Map IOCFG2 for pin mux */
    volatile uint32_t *iocfg2 = map_reg(IOCFG2_BASE, IOCFG2_SIZE);
    if (!iocfg2) return 1;

    /* Map GPIO6 for bit-bang */
    volatile uint32_t *gpio6 = map_reg(GPIO6_BASE, GPIO6_SIZE);
    if (!gpio6) return 1;

    /* Step 1: Read current pin state */
    uint32_t sda_val = reg_read(iocfg2, 0x98);
    uint32_t scl_val = reg_read(iocfg2, 0x9C);
    fprintf(stderr, "[1] Current I2C0 pin mux: SDA=0x%x, SCL=0x%x\n", sda_val, scl_val);

    /* Step 2: Switch pins to GPIO (func 0) */
    fprintf(stderr, "[2] Switching I2C0 pins to GPIO mode...\n");
    reg_write(iocfg2, 0x98, IOCFG_GPIO_FUNC);
    reg_write(iocfg2, 0x9C, IOCFG_GPIO_FUNC);
    udelay(10);

    /* Step 3: Configure SCL as output, SDA as input */
    fprintf(stderr, "[3] Configuring GPIO6_6 (SDA) as input, GPIO6_7 (SCL) as output...\n");
    gpio_dir(gpio6, 6, 0);  /* SDA = input */
    gpio_dir(gpio6, 7, 1);  /* SCL = output */
    udelay(10);

    /* Step 4: Read SDA level */
    int sda_level = gpio_get(gpio6, 6);
    fprintf(stderr, "[4] SDA level before recovery: %d\n", sda_level);

    /* Step 5: Generate 9 clock pulses on SCL to free SDA */
    fprintf(stderr, "[5] Generating 9 clock pulses on SCL...\n");
    for (int i = 0; i < 9; i++) {
        gpio_set(gpio6, 7, 0);
        udelay(5);
        gpio_set(gpio6, 7, 1);
        udelay(5);
        sda_level = gpio_get(gpio6, 6);
        fprintf(stderr, "  Clock %d: SDA=%d\n", i + 1, sda_level);
        if (sda_level == 1) {
            fprintf(stderr, "  SDA released after %d clocks!\n", i + 1);
            break;
        }
    }

    /* Step 6: Generate STOP condition (SDA low->high while SCL high) */
    fprintf(stderr, "[6] Generating STOP condition...\n");
    gpio_dir(gpio6, 6, 1);  /* SDA as output */
    gpio_set(gpio6, 6, 0);  /* SDA low */
    udelay(5);
    gpio_set(gpio6, 7, 1);  /* SCL high */
    udelay(5);
    gpio_set(gpio6, 6, 1);  /* SDA high -> STOP */
    udelay(5);

    /* Step 7: Check SDA/SCL levels */
    gpio_dir(gpio6, 6, 0);  /* SDA back to input */
    udelay(10);
    sda_level = gpio_get(gpio6, 6);
    int scl_level = gpio_get(gpio6, 7);
    fprintf(stderr, "[7] After recovery: SDA=%d, SCL=%d\n", sda_level, scl_level);

    /* Step 8: Restore I2C function */
    fprintf(stderr, "[8] Restoring I2C0 pin mux...\n");
    reg_write(iocfg2, 0x98, IOCFG_I2C_FUNC);
    reg_write(iocfg2, 0x9C, IOCFG_I2C_FUNC);
    mdelay(10);

    /* Step 9: Try I2C read */
    fprintf(stderr, "[9] Trying I2C read (chip ID 0x30DC)...\n");
    int fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "  open /dev/i2c-0: %s\n", strerror(errno));
    } else {
        /* I2C_SLAVE_FORCE with 7-bit address */
        if (ioctl(fd, I2C_SLAVE_FORCE, 0x1A) < 0) {
            fprintf(stderr, "  I2C_SLAVE_FORCE: %s\n", strerror(errno));
        } else {
            /* Write reg address, then read back */
            unsigned char buf[3];
            buf[0] = 0x30;
            buf[1] = 0xDC;
            int ret = write(fd, buf, 2);
            if (ret < 0) {
                fprintf(stderr, "  write reg addr: %s (errno=%d)\n", strerror(errno), errno);
            } else {
                memset(buf, 0, sizeof(buf));
                ret = read(fd, buf, 1);
                if (ret < 0) {
                    fprintf(stderr, "  read data: %s (errno=%d)\n", strerror(errno), errno);
                } else {
                    fprintf(stderr, "  Chip ID: 0x%02x (expected 0x32)\n", buf[0]);
                }
            }
        }
        close(fd);
    }

    fprintf(stderr, "\n=== Done ===\n");
    return 0;
}
