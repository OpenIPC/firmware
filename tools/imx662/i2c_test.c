#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define I2C_ADDR_7BIT 0x1A
#define SENSOR_CHIP_ID_REG 0x30DC

int main(int argc, char *argv[])
{
    int fd, ret;
    unsigned char buf[3];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    fprintf(stderr, "=== I2C Test (no MPI) ===\n");

    /* Step 1: Open I2C-0 */
    fprintf(stderr, "[1] Opening /dev/i2c-0...\n");
    fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "  FAILED: %s (errno=%d)\n", strerror(errno), errno);
        return 1;
    }
    fprintf(stderr, "  fd=%d\n", fd);

    /* Step 2: Try I2C_SLAVE_FORCE */
    fprintf(stderr, "[2] I2C_SLAVE_FORCE 0x%02x...\n", I2C_ADDR_7BIT);
    ret = ioctl(fd, I2C_SLAVE_FORCE, I2C_ADDR_7BIT);
    if (ret < 0) {
        fprintf(stderr, "  FAILED: %s (errno=%d)\n", strerror(errno), errno);
    } else {
        fprintf(stderr, "  OK\n");
    }

    /* Step 3: Simple write (reg 0x3000 = 0x01 - software standby) */
    fprintf(stderr, "[3] Simple write: reg 0x3000 = 0x01...\n");
    buf[0] = 0x30;
    buf[1] = 0x00;
    buf[2] = 0x01;
    ret = write(fd, buf, 3);
    if (ret < 0) {
        fprintf(stderr, "  FAILED: %s (errno=%d)\n", strerror(errno), errno);
    } else {
        fprintf(stderr, "  OK (%d bytes written)\n", ret);
    }

    /* Step 4: I2C_RDWR combined write+read (register 0x30DC = chip ID) */
    fprintf(stderr, "[4] I2C_RDWR: read reg 0x%04x...\n", SENSOR_CHIP_ID_REG);
    unsigned char reg_buf[2] = { (SENSOR_CHIP_ID_REG >> 8) & 0xFF, SENSOR_CHIP_ID_REG & 0xFF };
    unsigned char val_buf[1] = { 0 };

    memset(msgs, 0, sizeof(msgs));
    msgs[0].addr = I2C_ADDR_7BIT;
    msgs[0].flags = 0;  /* write */
    msgs[0].len = 2;
    msgs[0].buf = reg_buf;
    msgs[1].addr = I2C_ADDR_7BIT;
    msgs[1].flags = I2C_M_RD;  /* read */
    msgs[1].len = 1;
    msgs[1].buf = val_buf;

    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    ret = ioctl(fd, I2C_RDWR, &rdwr);
    if (ret < 0) {
        fprintf(stderr, "  FAILED: %s (errno=%d)\n", strerror(errno), errno);
    } else {
        fprintf(stderr, "  OK - Chip ID: 0x%02x\n", val_buf[0]);
    }

    /* Step 5: Try writing a few more registers */
    fprintf(stderr, "[5] Batch write test...\n");
    struct { unsigned short reg; unsigned char val; } test_regs[] = {
        {0x3000, 0x00},  /* exit standby */
        {0x3014, 0x01},  /* INCK_SEL */
        {0x3015, 0x02},  /* DATARATE_SEL */
        {0x3444, 0xAC},  /* PLL config */
    };
    int ok = 0, fail = 0;
    for (int i = 0; i < 4; i++) {
        buf[0] = (test_regs[i].reg >> 8) & 0xFF;
        buf[1] = test_regs[i].reg & 0xFF;
        buf[2] = test_regs[i].val;
        ret = write(fd, buf, 3);
        if (ret < 0) {
            fprintf(stderr, "  0x%04x=0x%02x: FAIL (%s)\n",
                test_regs[i].reg, test_regs[i].val, strerror(errno));
            fail++;
        } else {
            fprintf(stderr, "  0x%04x=0x%02x: OK\n", test_regs[i].reg, test_regs[i].val);
            ok++;
        }
    }
    fprintf(stderr, "  %d ok, %d fail\n", ok, fail);

    close(fd);

    /* Step 6: Try SPI as alternative */
    fprintf(stderr, "\n[6] Trying SPI (/dev/spidev0.0)...\n");
    fd = open("/dev/spidev0.0", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "  FAILED: %s\n", strerror(errno));
        return 1;
    }

    /* SPI mode 3 for IMX662 */
    unsigned char mode = SPI_MODE_3;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    fprintf(stderr, "  SPI mode: %d (%s)\n", ret, ret < 0 ? "FAIL" : "OK");

    unsigned char bits = 8;
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    fprintf(stderr, "  SPI bits: %d (%s)\n", bits, ret < 0 ? "FAIL" : "OK");

    unsigned int speed = 10000000;
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    fprintf(stderr, "  SPI speed: %d (%s)\n", speed, ret < 0 ? "FAIL" : "OK");

    /* IMX662 SPI read: send 2-byte reg address, read back data */
    /* SPI read bit = 1 in first byte MSB, so read addr = (reg >> 8) | 0x80 */
    unsigned char spi_tx[4], spi_rx[4];
    memset(spi_tx, 0, sizeof(spi_tx));
    memset(spi_rx, 0, sizeof(spi_rx));

    /* Read reg 0x30DC (chip ID) via SPI */
    spi_tx[0] = ((SENSOR_CHIP_ID_REG >> 8) & 0xFF) | 0x80;  /* read bit set */
    spi_tx[1] = SENSOR_CHIP_ID_REG & 0xFF;
    spi_tx[2] = 0x00;  /* dummy clock for data */

    struct spi_ioc_transfer xfer;
    memset(&xfer, 0, sizeof(xfer));
    xfer.tx_buf = (unsigned long)spi_tx;
    xfer.rx_buf = (unsigned long)spi_rx;
    xfer.len = 3;
    xfer.speed_hz = speed;
    xfer.bits_per_word = 8;

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (ret < 0) {
        fprintf(stderr, "  SPI transfer FAILED: %s (errno=%d)\n", strerror(errno), errno);
    } else {
        fprintf(stderr, "  SPI RX: %02x %02x %02x\n", spi_rx[0], spi_rx[1], spi_rx[2]);
    }

    close(fd);

    fprintf(stderr, "\n=== Done ===\n");
    return 0;
}
