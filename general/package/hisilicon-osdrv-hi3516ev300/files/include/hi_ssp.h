/*
 * extdrv/include/hi_ssp.h for Linux .
 *
 * History:
 *      2006-4-11 create this file
 */

#ifndef __HI_SSP_H__
#define __HI_SSP_H__

#define SSP_READ_ALT    0x1
#define SSP_WRITE_ALT    0X3

typedef struct hiSPI_DATA_S
{
    unsigned int     spi_no;
    unsigned char    dev_addr;
    unsigned int     dev_byte_num;
    unsigned int     reg_addr;
    unsigned int     addr_byte_num;
    unsigned int     data;
      unsigned int     data_byte_num;
}SPI_DATA_S;


#endif

