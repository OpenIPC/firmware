  /*
    LM75_address.c
    --------------
      1) set the i2c address of the LM75
	* R/W pulse at end of address byte not included in address 1-0-0-1-A2-A1-A0
	* example - all lines to 0V - 1001000 = decimal 72
      2) calculate the temperature
      3) print it
	usage :- type (without the < >)
		<LM75_address><space><address as decimal>
*/

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include "smbus.h"

int main(int argc, char** argv)
{
  int address;      /* i2c bus address */

 if (argc != 2)   /* report error if we are not getting just 1 input after the program name */
 {
  printf("Error. usage: %s i2c_chip_address\n", argv[0]);
 }
  address = atoi(argv[1]); /* address is the first number after the program name */


          int f = open("/dev/i2c-0",O_RDWR);
          char buf[2];
          if (f>0) {
              ioctl(f,I2C_SLAVE,address);
              if (-1 == read(f,buf,2)) {
                  printf("read error.\n");
              } else { /* calculate the temperature*/
                  int temp = buf[0]*256+buf[1];
                  temp >>=7;
                  if (temp>512)
                      temp &= -1;
                  printf("Temperature (C)\t\t%g\n", (float)temp/2.0);
              }
          }
          return close(f);
      }
