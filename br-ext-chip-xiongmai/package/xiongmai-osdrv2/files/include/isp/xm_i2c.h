/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************
File Name	: xm_i2c.h
Version 		: Initial Draft
Author		: XM Isp software group
Created 		: 2015/6/27

Description 	: The common data type defination
Function List	:
History :
1.Date		: 2015/6/27
  Author		: Lycai
  Modification	: creat
******************************************************************************/

#ifndef __XM_I2C_H__
#define __XM_I2C_H__




typedef struct I2C_DATA_S
{
	unsigned char	dev_addr; 
	unsigned int 	reg_addr; 
	unsigned int 	addr_byte_num; 
	unsigned int 	data; 
  	unsigned int 	data_byte_num;
  	
}I2C_DATA_S ;
 
typedef struct I2C_DATA_STR_
{
	unsigned char	dev_addr; 
//	unsigned int 	reg_addr; 
	unsigned char 	*data; 
	unsigned char 	addr_byte_num; 
  	unsigned char  	data_byte_num;
  	
}I2C_DATA_STR ;

#define READ_OPERATION     (1)
#define WRITE_OPERATION    0xfe
#define CMD_I2C_WRITE      0x01
#define CMD_I2C_READ       0x03


/* I2C_CTRL_REG */
#define I2C_ENABLE             (1 << 8)
#define I2C_UNMASK_TOTAL       (1 << 7)
#define I2C_UNMASK_START       (1 << 6)
#define I2C_UNMASK_END         (1 << 5)
#define I2C_UNMASK_SEND        (1 << 4)
#define I2C_UNMASK_RECEIVE     (1 << 3)
#define I2C_UNMASK_ACK         (1 << 2)
#define I2C_UNMASK_ARBITRATE   (1<< 1)
#define I2C_UNMASK_OVER        (1 << 0)
#define I2C_UNMASK_ALL         (I2C_UNMASK_START | I2C_UNMASK_END | \
                                I2C_UNMASK_SEND | I2C_UNMASK_RECEIVE | \
                                I2C_UNMASK_ACK | I2C_UNMASK_ARBITRATE | \
                                I2C_UNMASK_OVER)

/* I2C_COM_REB */
#define I2C_SEND_ACK (~(1 << 4))
#define I2C_START (1 << 3)
#define I2C_READ (1 << 2)
#define I2C_WRITE (1 << 1)
#define I2C_STOP (1 << 0)

/* I2C_ICR_REG */
#define I2C_CLEAR_START (1 << 6)
#define I2C_CLEAR_END (1 << 5)
#define I2C_CLEAR_SEND (1 << 4)
#define I2C_CLEAR_RECEIVE (1 << 3)
#define I2C_CLEAR_ACK (1 << 2)
#define I2C_CLEAR_ARBITRATE (1 << 1)
#define I2C_CLEAR_OVER (1 << 0)
#define I2C_CLEAR_ALL (I2C_CLEAR_START | I2C_CLEAR_END | \
                       I2C_CLEAR_SEND | I2C_CLEAR_RECEIVE | \
                       I2C_CLEAR_ACK | I2C_CLEAR_ARBITRATE | \
                       I2C_CLEAR_OVER)

/* I2C_SR_REG */
#define I2C_BUSY (1 << 7)
#define I2C_START_INTR (1 << 6)
#define I2C_END_INTR (1 << 5)
#define I2C_SEND_INTR (1 << 4)
#define I2C_RECEIVE_INTR (1 << 3)
#define I2C_ACK_INTR (1 << 2)
#define I2C_ARBITRATE_INTR (1 << 1)
#define I2C_OVER_INTR (1 << 0)


int XM_I2C_WriteConfig(unsigned char dev_addr);
int XM_I2C_Write(unsigned char dev_addr, unsigned int reg_addr, unsigned int addr_byte_num, unsigned int data, unsigned int data_byte_num);
int XM_I2C_Read (unsigned char dev_addr, unsigned int reg_addr, unsigned int addr_byte_num, unsigned int data_byte_num);



#endif


