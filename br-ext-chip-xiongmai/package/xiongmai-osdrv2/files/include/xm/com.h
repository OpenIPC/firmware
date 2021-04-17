/*********************************************************************
Copyright (C), 2015-8-26, JuFeng.Tech. Co., Ltd.
File name: 	com.h
Author:		
Versoin: 	       1.00
Data: 		2015-08-26
Desc:		串口源文件，实现串口通信
Ohters:		// 其他说明
Function List:	

**********************************************************************/
#ifndef __COM_H__
#define __COM_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define uint8 unsigned char
#define uint32 unsigned int
#define uint16 unsigned short
#define ISP_USED	(0)
#define UART_0		(0)
#define COM_TimeOut0		(0x00000001) 
#define COM_TimeOut1		(0x00010000) 
typedef enum {
    E_RECV_OK,
    E_RECV_TIMEOUT
}eCOMERR;


#define TRUE			1
#define FALSE			0



/**************************** 调用函数库声明 *******************************/

/*
 指令定义
 */
#define COM_CMDLEN			13	//( AA BB CC CC CC CC DD DD DD DD xx xx xx) 

// 串口处理模式
#define CMD_DEBUGMODE		0XFE
#define CMD_BOOTMODE		0x95
#define CMD_VISCAMIN_MODE		0x80
#define CMD_VISCAMAX_MODE		0x8F

/***** DEBUG 模式指令定义 *****/

#define CMD_DEBUG_REGWRTIE		0x05
#define CMD_DEBUG_REGREAD		0x06


#define CMD_SENSOR_WRITE        0x07
#define CMD_SENSOR_READ         0x08

#define CMD_MOTOR_WRITE			0x09
#define CMD_MOTOR_READ			0x0A

#define CMD_SPI_WRITE			0x0B
#define CMD_SPI_READ			0x0C

#define CMD_FLASH_WRITE			0x0F
#define CMD_FLASH_READ			0x10

#define CMD_DEBUG_FLASHWRTIE	0x0aa   //0x
#define CMD_DEBUG_FLASHREAD		0x0ab
#define CMD_DEBUG_FLASHERASE	0x0ac

#define CMD_DEBUG_TIMEOUTPARA_SET	0x01A
#define CMD_DEBUG_FLASHPARA_SET		0x01B

#define CMD_DEBUG_BAUDRATE_SET		0x01C
#define CMD_DEBUG_BOOTMODE_EN_SET	0x0D
#define CMD_DEBUG_FLASHPRO_SET		0x0E

#define CMD_DEBUG_AHD			0x5a

/*
UART 调试：
1、REG 读写；
2、FLASH byte读写(带校验， 使能)；
3、FLASH BLOCK ERASE(带校验, 使能)；

4、各模块超时变量查询及设置(带校验)；
5、FLASH 类型设置（操作指令可设置）(带校验)；

6、波特率设置(带校验)（默认波特率9600）;
7、UART BootLoader模式使能；（上位机控制）(带校验)；
8、FLASH写保护功能（由上位机控制）(带校验)；
      保护为chip protect;
      分断电记忆和断电不记忆两种；

*/




/***** bootloader 模式定义 *****/
#define CMD_BOOT_FLASH_WRITE		0x22
#define	CMD_BOOT_FLASH_READ			0x42

#define	CMD_BOOT_FLASH_CHECKSUM		0x52
#define	CMD_BOOT_FLASH_BLANKCHECK	0x62

#define	CMD_BOOT_FLASH_ERASE		0x82
#define CMD_BOOT_CHIP_ERASE			0xA2







/******************变量声明**************************/







/******************函数声明**************************/

void Com_Debugprocess_Anomaly(uint8 uart_NO);
void Com_FlashProcess(uint8 uart_NO);

uint8 getbyte(uint32 timeout);
void ISR_UART0(void);
void putdword(unsigned char ComNum, unsigned int c);




#endif

