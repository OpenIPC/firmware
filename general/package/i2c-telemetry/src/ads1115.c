
#include <stdio.h>
#include <sys/types.h> // open
#include <sys/stat.h>  // open
#include <fcntl.h>     // open
#include <unistd.h>    // read/write usleep
#include <stdlib.h>    // exit
#include <inttypes.h>  // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions

float getVin(int pin) {

  int fd;
  int ads_address = 0x48;

  uint8_t buf[10];
  int16_t val;
  uint8_t pinCodes[4];

  pinCodes[0]=0xc3;
  pinCodes[1]=0xd3;
  pinCodes[2]=0xe3;
  pinCodes[3]=0xf3;

  // open device on /dev/i2c-0
  if ((fd = open("/dev/i2c-0", O_RDWR)) < 0) {
    printf("Error: Couldn't open device! %d\n", fd);
    return 1;
  }

  // connect to ads1115 as i2c slave
  if (ioctl(fd, I2C_SLAVE, ads_address) < 0) {
    printf("Error: Couldn't find device on address!\n");
    return 1;
  }
     
  // AIN0 and GND, 4.096v, 128s/s

  buf[0] = 1;    // config register is 1
  buf[1] = pinCodes[pin];
  buf[2] = 0x85;
  if (write(fd, buf, 3) != 3) {
    perror("Write to register 1");
    exit(-1);
  }

  usleep(10000);

  // wait for conversion complete
  do {
    if (read(fd, buf, 2) != 2) {
      perror("Read conversion");
      exit(-1);
    }
  } while (buf[0] & 0x80 == 0);

  // read conversion register
  buf[0] = 0;   // conversion register is 0
  if (write(fd, buf, 1) != 1) {
    perror("Write register select");
    exit(-1);
  }
  if (read(fd, buf, 2) != 2) {
    perror("Read conversion");
    exit(-1);
  }

  // convert output and display results
  val = (int16_t)buf[0]*256 + (uint16_t)buf[1];
  
  close(fd);

  return (float)val*4.096/32768.0;
}

main() {

  int i;
  for ( i=0; i<=3; i++) {
    printf("Vin%1d (V) %5.3f\n",i,getVin(i));
  }

}
