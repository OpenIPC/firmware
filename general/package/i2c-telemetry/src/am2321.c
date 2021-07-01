/*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*
* Project Home - https://github.com/takagi/am2321
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>              /* for O_RDWR */
#include <string.h>             /* for memcpy */
#include <linux/i2c-dev.h>      /* for I2C_SLAVE */

#include <unistd.h>             /* add by paulerr*/
#include <sys/time.h>           /* add by paulerr*/


/* I2C character device */
#define I2C_DEVICE "/dev/i2c-0"

/* I2C address of AM2321 sensor in 7 bits
 * - the first 7 bits should be passed to ioctl system call
 *   because the least 1 bit of the address represents read/write
 *   and the i2c driver takes care of it
 */
#define AM2321_ADDR (0xB8 >> 1)


/*
 *  udelay function
 */

long timeval_to_usec( struct timeval tm ) {
  return tm.tv_sec * 1000000 + tm.tv_usec;
}

void udelay( long us ) {
  struct timeval current;
  struct timeval start;
  
  gettimeofday( &start, NULL );
  do {
    gettimeofday( &current, NULL );
  } while( timeval_to_usec( current ) - timeval_to_usec( start ) < us );
}


/*
 *  CRC16
 */

unsigned short crc16( unsigned char *ptr, unsigned char len ) {
  unsigned short crc = 0xFFFF;
  unsigned char i;

  while( len-- )
  {
    crc ^= *ptr++;
    for( i = 0; i < 8; i++ ) {
      if( crc & 0x01 ) {
	crc >>= 1;
	crc ^= 0xA001;
      } else {
	crc >>= 1;
      }
    }
  }

  return crc;
}

unsigned char crc16_low( unsigned short crc ) {
  return crc & 0xFF;
}

unsigned char crc16_high( unsigned short crc ) {
  return crc >> 8;
}


/*
 *  st_am2321 Structure
 */

typedef struct {
  unsigned char data[8];
} st_am2321;

void __check_crc16( st_am2321 measured ) {
  unsigned short crc_m, crc_s;

  crc_m = crc16( measured.data, 6 );
  crc_s = (measured.data[7] << 8) + measured.data[6];
  if ( crc_m != crc_s ) {
    fprintf( stderr, "am2321: CRC16 does not match\n" );
    exit( 1 );
  }

  return;
}

st_am2321 __st_am2321( unsigned char* data ) {
  st_am2321 result;
  memcpy( result.data, data, 8 );
  __check_crc16( result );
  return result;
}

void am2321_dump( st_am2321 measured ) {
  printf( "[ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]\n",
          measured.data[0], measured.data[1],
          measured.data[2], measured.data[3],
          measured.data[4], measured.data[5],
          measured.data[6], measured.data[7] );
  return;
}

short __am2321_temperature( st_am2321 measured ) {
  return (measured.data[4] << 8) + measured.data[5];
}

short am2321_temperature_integral( st_am2321 measured ) {
  return __am2321_temperature( measured ) / 10;
}

short am2321_temperature_fraction( st_am2321 measured ) {
  return __am2321_temperature( measured ) % 10;
}

short __am2321_humidity( st_am2321 measured ) {
  return (measured.data[2] << 8) + measured.data[3];
}

short am2321_humidity_integral( st_am2321 measured ) {
  return __am2321_humidity( measured ) / 10;
}

short am2321_humidity_fraction( st_am2321 measured ) {
  return __am2321_humidity( measured ) % 10;
}


/*
 *  Measurement function
 */

st_am2321 am2321() {
  int fd;
  int ret;
  int retry_cnt;
  unsigned char data[8];

  /* open I2C device */
  fd = open( I2C_DEVICE, O_RDWR );
  if ( fd < 0 ) {
    perror( "am2321(1)" );
    exit( 1 );
  }

  /* set address of I2C device in 7 bits */
  ret = ioctl( fd, I2C_SLAVE, AM2321_ADDR );
  if ( ret < 0 ) {
    perror( "am2321(2)" );
    exit( 2 );
  }

 retry_cnt = 0;
 retry:

  /* wake I2C device up */
  write( fd, NULL, 0);

  /* write measurement request */
  data[0] = 0x03; data[1] = 0x00; data[2] = 0x04;
  ret = write( fd, data, 3 );
  if ( ret < 0 && retry_cnt++ < 5 ) {
    fprintf( stderr, "am2321: retry\n" );
    udelay( 1000 );
    goto retry;
  }
  if ( ret < 0 ) {
    perror( "am2321(3)" );
    exit( 3 );
  }

  /* wait for having measured */
  udelay( 1500 );

  /* read measured result */
  memset( data, 0x00, 8 );
  ret = read( fd, data, 8 );
  if ( ret < 0 ) {
    perror( "am2321(4)" );
    exit( 4 );
  }

  /* close I2C device */
  close( fd );

  return __st_am2321( data );
}

st_am2321 am2321_stub() {
  unsigned char data[] = { 0x03, 0x04, 0x01, 0x41,
                           0x00, 0xEA, 0x21, 0x8F };
  return __st_am2321( data );
}


/*
 *  Print functions
 */

void print_am2321( st_am2321 measured ) {
  printf( "%d.%d %d.%d\n",
          am2321_temperature_integral( measured ),
          am2321_temperature_fraction( measured ),
          am2321_humidity_integral( measured ),
          am2321_humidity_fraction( measured ) );
  return;
}

void print_am2321_human_readable( st_am2321 measured ) {
  printf( "Temperature (C)\t\t%d.%d\n",
          am2321_temperature_integral( measured ),
          am2321_temperature_fraction( measured ) );
  printf( "Humidity (%%)\t\t%d.%d\n",
          am2321_humidity_integral( measured ),
          am2321_humidity_fraction( measured ) );
  return;
}

/*
 *  Main
 */

#define OPT_HUMAN_READABLE 0x1
#define OPT_STUB 0x2

int print_help() {
  fprintf( stderr,
           "Usage: am2321 [-r] [-s]\n"
           "Get temperature and humidity measured with Aosong's AM2321 sensor.\n"
           "  -r    human readable output\n"
           "  -s    not measure with AM2321, instead stub of 23.4[C] and 32.1[%]\n" );
  exit( 1 );
}

int parse_options( int argc, char* argv[]) {
  int options = 0;
  int flags = 0;

  while( 1+flags < argc && argv[1+flags][0] == '-' ) {
    switch( argv[1+flags][1] ) {
    case 'r': options |= OPT_HUMAN_READABLE; break;
    case 's': options |= OPT_STUB; break;
    default:
      fprintf( stderr, "am2321: Unsupported option \"%s\"\n", argv[1+flags] );
      print_help();
      exit( 1 );
    }
    flags++;
  }

  return options;
}

int is_opt_human_readable( int options ) {
  return options & OPT_HUMAN_READABLE;
}

int is_opt_stub( int options ) {
  return options & OPT_STUB;
}

int main( int argc, char* argv[] ) {
  int options;
  st_am2321 measured;

  /* parse the given options */
  options = parse_options( argc, argv );

  /* measure temperature and humidity */
  measured = ! is_opt_stub( options ) ? am2321() : am2321_stub();

  /* print measured temperature and humidity */
  if ( ! is_opt_human_readable( options ) ) {
    print_am2321( measured );
  } else {
    print_am2321_human_readable( measured );
  }

  return 0;
}
