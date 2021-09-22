#if 0
//#ifdef SOC_SYSTEM
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif
#include "xm_type.h"
#include "xm_i2c.h"
#include "xm_print.h"
#include "Camera.h"
#include "XAx_cmos.h"
//#define DEBUG_ON

extern XM_SENSOR_CHIP gSensorChip;


XM_S32 	gResolution;
XM_U8 	gSnsDevAddr = 0x30;		// 0x60
XM_U32  gSnsRegAddrByte = 0x1;	// 0x2
XM_U32  gSnsRegDataByte = 0x1;	// 0x2

#if 0
#define I2C_DEV		"/dev/xm_i2c"
#define I2C_DEV1	"/dev/xm_i2c1"
XM_S32 gI2CDevFd = -1;
XM_S32 gI2CDevFd1 = -1;

static XM_S32 OpenDev()
{

	gI2CDevFd = open(I2C_DEV, 0);
	if(gI2CDevFd<0)
	{
	    ERR("Open hi_i2c error!\n");
	    return -1;
	}
	return 0;
}

static XM_S32 CloseDev()
{
	if(gI2CDevFd>=0)
	{
	    close(gI2CDevFd);
	}
	return 0;
}

static XM_S32 OpenDev1()
{
	gI2CDevFd1 = open(I2C_DEV1, 0);
	if(gI2CDevFd1<0)
	{
	    ERR("Open hi_i2c1 error!\n");
	    return -1;
	}
	return 0;
}

static XM_S32 CloseDev1()
{
	if(gI2CDevFd1>=0)
	{
	    close(gI2CDevFd1);
	}
	return 0;
}

XM_S32 sensor_read_register_sch(XM_U8 I2cCh, XM_U32 addr)
{
	XM_S32 ret;
	I2C_DATA_S i2c_data;

	if(I2cCh == 0)
	{
		if(gI2CDevFd<0)
			OpenDev();
	}
	else
	{
		if(gI2CDevFd1<0)
			OpenDev1();
	}

	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data_byte_num = gSnsRegDataByte;

	if(I2cCh == 0)
	{
		ret = ioctl(gI2CDevFd, CMD_I2C_READ, &i2c_data);
		if (ret)
		{
		    ERR("xm_i2c read faild!\n");
		    CloseDev();
		    return -1;
		}
	}
	else
	{
		ret = ioctl(gI2CDevFd1, CMD_I2C_READ, &i2c_data);
		if (ret)
		{
		    ERR("xm_i2c1 read faild!\n");
		    CloseDev1();
		    return -1;
		}
	}
	return i2c_data.data;
}

XM_S32 sensor_read_register(XM_U32 addr)
{
	return sensor_read_register_sch(0,addr);
}

XM_S32 sensor_write_register_sch(XM_U8 I2cCh, XM_U32 addr, XM_U32 data)
{
	XM_S32 ret;
	I2C_DATA_S i2c_data;
	if(I2cCh == 0)
	{
		if(gI2CDevFd<0)
			OpenDev();
	}
	else
	{
		if(gI2CDevFd1<0)
			OpenDev1();
	}
	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data = data;
	i2c_data.data_byte_num = gSnsRegDataByte;

	if(I2cCh == 0)
	{
		ret = ioctl(gI2CDevFd, CMD_I2C_WRITE, &i2c_data);
		if (ret)
		{
		    ERR("xm_i2c write faild!\n");
		     CloseDev();
		    return -1;
		}
	}
	else
	{
		ret = ioctl(gI2CDevFd1, CMD_I2C_WRITE, &i2c_data);
		if (ret)
		{
		    ERR("xm_i2c1 write faild!\n");
		     CloseDev1();
		    return -1;
		}
	}
	return 0;
}
XM_S32 sensor_write_register(XM_U32 addr, XM_U32 data)
{
	return sensor_write_register_sch(0,addr,data);
}

#else
XM_S32 sensor_read_register_sch(XM_U8 I2cCh, XM_U32 addr)
{
	I2C_DATA_S i2c_data;

	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data_byte_num = gSnsRegDataByte;

	XM_I2C_Ioctl(CMD_I2C_READ, &i2c_data);
	return i2c_data.data;
}
XM_S32 sensor_read_register(XM_U32 addr)
{
	return sensor_read_register_sch(0,addr);
}

XM_S32 sensor_write_register_sch(XM_U8 I2cCh, XM_U32 addr, XM_U32 data)
{
	I2C_DATA_S i2c_data;
	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data = data;
	i2c_data.data_byte_num = gSnsRegDataByte;
	XM_I2C_Ioctl(CMD_I2C_WRITE, &i2c_data);
	return 0;
}
XM_S32 sensor_write_register(XM_U32 addr, XM_U32 data)
{
	return sensor_write_register_sch(0,addr,data);
}
#endif


