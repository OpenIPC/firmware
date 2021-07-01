/*
 Sample code for the BH1750
 */
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include "smbus.h"

#define BH1750FVI_I2C_ADDRESS 0x23 // ADDR > 0.7 VCC)
//#define BH1750FVI_I2C_ADDRESS  0x53 // ADDR < 0.3 VCC)
#define DEBUG 0

#define PowerDown    0x00
#define PowerOn    0x01
#define Reset      0x07
#define ContinuHigh   0x10
#define ContinuLow   0x13
#define OneTimeHigh   0x20
#define OneTimeLow   0x23

int main(int argc, char **argv)
{
    int fd;
    char *fileName = "/dev/i2c-0";
    int retCode;
    int readSize;
    unsigned int res;
    unsigned int lux;
    char buf[5];
    int i;

    // Open port for reading and writing
    if ((fd = open(fileName, O_RDWR)) < 0) {
        printf("open error\n");
        exit(1);
    }

    // Set the port options and set the address of the device
    if (ioctl(fd, I2C_SLAVE, BH1750FVI_I2C_ADDRESS) < 0) {
        printf("ioctl error\n");
        close(fd);
        exit(1);
    }

    retCode=i2c_smbus_write_byte(fd, PowerOn);
if(DEBUG)printf("Power On retCode=%d\n",retCode);
    if (retCode < 0) {
        printf("PowerOn error\n");
        close(fd);
        exit(1);
    }

    retCode=i2c_smbus_write_byte(fd, ContinuHigh);
if(DEBUG)printf("ContinuHigh retCode=%d\n",retCode);
    if (retCode < 0) {
        printf("ContinuHigh error\n");
        close(fd);
        exit(1);
    }

    // Set i<10 for more one check
    for(i=0;i<1;i++) {
        sleep(3);
        readSize = read (fd, buf, 2);
if(DEBUG)printf("read readSize=%d %x %x\n",readSize,buf[0],buf[1]);

        res = buf[0]*256+buf[1];
if(DEBUG)printf("res=%x\n",res);
        lux = res / 1.2;
    //    printf("Lux=%d\n",lux);
        printf("Brightness (Lux)\t%0.1f\n", ((double)lux));
    }

    retCode=i2c_smbus_write_byte(fd, PowerDown);
    close(fd);

    exit (0);
}