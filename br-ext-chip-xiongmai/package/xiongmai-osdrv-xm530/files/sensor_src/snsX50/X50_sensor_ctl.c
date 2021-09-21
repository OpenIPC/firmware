#ifdef SOC_SYSTEM
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
#include "X50_sensor_ctl.h"

#ifdef SOC_NONE
#include "xm540_tmp.h"
#endif
#include "Camera.h"


extern XM_SENSOR_CHIP gSensorChip;


XM_S32 	gResolution;
XM_U8 	gSnsDevAddr = 0x30;		// 0x60
XM_U32  gSnsRegAddrByte = 0x1;	// 0x2
XM_U32  gSnsRegDataByte = 0x1;	// 0x2
XM_U8   gAppEncoderinfo = 0x0;  //0:Ahd; 1:Cvi 2:Tvi

#ifdef SOC_SYSTEM
#define I2C_DEV	"/dev/xm_i2c"
XM_S32 gI2CDevFd = -1;

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


XM_S32 sensor_read_register(XM_U32 addr)
{
	XM_S32 ret;
	I2C_DATA_S i2c_data;
	if(gI2CDevFd<0)
		OpenDev();

	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data_byte_num = gSnsRegDataByte;

	ret = ioctl(gI2CDevFd, CMD_I2C_READ, &i2c_data);
	if (ret)
	{
	    ERR("xm_i2c write faild!\n");
	    CloseDev();
	    return -1;
	}


	return i2c_data.data;
}

XM_S32 sensor_write_register(XM_U32 addr, XM_U32 data)
{
	XM_S32 ret;
	I2C_DATA_S i2c_data;
	if(gI2CDevFd<0)
		OpenDev();
	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data = data;
	i2c_data.data_byte_num = gSnsRegDataByte;

	ret = ioctl(gI2CDevFd, CMD_I2C_WRITE, &i2c_data);

	if (ret)
	{
	    ERR("xm_i2c write faild!\n");
	     CloseDev();
	    return -1;
	}

	return 0;
}


#endif


#if((defined SOC_NONE)||(defined SOC_ALIOS))
XM_S32 sensor_read_register(XM_U32 addr)
{
	XM_S32 ret;
	I2C_DATA_S i2c_data;

	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data_byte_num = gSnsRegDataByte;

	ret = XM_I2C_Ioctl(CMD_I2C_READ, &i2c_data);
	if (ret)
	{
	    ERR("xm_i2c write faild!\n");
	    return -1;
	}


	return i2c_data.data;
}

XM_S32 sensor_write_register(XM_U32 addr, XM_U32 data)
{
	XM_S32 ret;
	I2C_DATA_S i2c_data;
	i2c_data.dev_addr = gSnsDevAddr;
	i2c_data.reg_addr = addr;
	i2c_data.addr_byte_num = gSnsRegAddrByte;
	i2c_data.data = data;
	i2c_data.data_byte_num = gSnsRegDataByte;
	ret = XM_I2C_Ioctl(CMD_I2C_WRITE, &i2c_data);

	if (ret)
	{
	    ERR("xm_i2c write faild!\n");
	    return -1;
	}
	return 0;
}

#endif


void sensor_init()
{
	switch(gSensorChip)
	{
    #ifdef SENS_X50_SC5035
        case SENSOR_CHIP_SC5035:
        	sensor_init_sc5035();
        	break;
    #endif
    #ifdef SENS_X50_K03
        case SENSOR_CHIP_K03:
        	sensor_init_k03();
        	break;
    #endif
    #ifdef SENS_X50_SC5235
        case SENSOR_CHIP_SC5235:
            sensor_init_sc5235();
            break;
    #endif
    #ifdef SENS_X50_SC5300
        case SENSOR_CHIP_SC5300:
            sensor_init_sc5300();
            break;
    #endif
    #ifdef SENS_X40_SC5239
        case SENSOR_CHIP_SC5239:
            sensor_init_sc5239();
            break;
    #endif
    #ifdef SENS_X50_IMX335
        case SENSOR_CHIP_IMX335:
            sensor_init_IMX335();
            break;
    #endif
		default:	ERR("[sensor_init]this sensor chip unsupport!\n");
				break;
	}
}


void sensor_init_boot()
{
	sensor_init();
}


