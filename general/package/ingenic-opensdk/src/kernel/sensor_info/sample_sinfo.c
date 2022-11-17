/*
 * sample_sinfo.c
 *
 * two ways to get sensor info
 *
 * 1. open /dev/sinfo; ioctl TOCTL_SINFO_GET
 *
 * 2. echo 1 >/proc/jz/sinfo/info; cat /proc/jz/sinfo/info
 *
 * */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define SENSOR_INFO_IOC_MAGIC  'S'
#define IOCTL_SINFO_GET			_IO(SENSOR_INFO_IOC_MAGIC, 100)
#define IOCTL_SINFO_FLASH		_IO(SENSOR_INFO_IOC_MAGIC, 101)

#define SENSOR_TYPE_INVALID	-1

enum SENSOR_TYPE
{
	SENSOR_TYPE_OV9712=0,
	SENSOR_TYPE_OV9732,
	SENSOR_TYPE_OV9750,
	SENSOR_TYPE_JXH42,
	SENSOR_TYPE_SC1035,
	SENSOR_TYPE_SC1135,
	SENSOR_TYPE_SC1045,
	SENSOR_TYPE_SC1145,
	SENSOR_TYPE_AR0130,
	SENSOR_TYPE_JXH61,
	SENSOR_TYPE_GC1024,
	SENSOR_TYPE_GC1064,
	SENSOR_TYPE_GC2023,
	SENSOR_TYPE_BF3115,
	SENSOR_TYPE_IMX225,
	SENSOR_TYPE_OV2710,
	SENSOR_TYPE_IMX323,
	SENSOR_TYPE_SC2135,
	SENSOR_TYPE_SP1409,
	SENSOR_TYPE_JXH62,
	SENSOR_TYPE_BG0806,
	SENSOR_TYPE_OV4689,
	SENSOR_TYPE_JXF22,
	SENSOR_TYPE_IMX322,
	SENSOR_TYPE_IMX291,
	SENSOR_TYPE_OV2735,
	SENSOR_TYPE_SC3035,
	SENSOR_TYPE_AR0237,
	SENSOR_TYPE_SC2145,
	SENSOR_TYPE_JXH65,
	SENSOR_TYPE_SC2300,
	SENSOR_TYPE_OV2735B,
	SENSOR_TYPE_JXV01,
	SENSOR_TYPE_PS5230,
	SENSOR_TYPE_PS5250,
	SENSOR_TYPE_OV2718,
	SENSOR_TYPE_OV2732,
	SENSOR_TYPE_SC2235,
	SENSOR_TYPE_JXK02,
	SENSOR_TYPE_OV7740,
	SENSOR_TYPE_HM2140,
	SENSOR_TYPE_GC2033,
	SENSOR_TYPE_JXF28,
	SENSOR_TYPE_OS02B10,
	SENSOR_TYPE_OS05A10,
	SENSOR_TYPE_SC2232,
	SENSOR_TYPE_SC2232H,
	SENSOR_TYPE_SC2230,
	SENSOR_TYPE_SC4236,
	SENSOR_TYPE_SC1245,
	SENSOR_TYPE_SC1245A,
	SENSOR_TYPE_GC1034,
	SENSOR_TYPE_SC1235,
	SENSOR_TYPE_JXF23,
	SENSOR_TYPE_PS5270,
	SENSOR_TYPE_SP140A,
	SENSOR_TYPE_SC2310,
	SENSOR_TYPE_HM2131,
	SENSOR_TYPE_MIS2003,
	SENSOR_TYPE_JXK03,
	SENSOR_TYPE_SC5235,
	SENSOR_TYPE_OV5648,
	SENSOR_TYPE_PS5280,
	SENSOR_TYPE_JXF23S,
	SENSOR_TYPE_GC2053,
	SENSOR_TYPE_SC4335,
	SENSOR_TYPE_PS5260,
	SENSOR_TYPE_OS04B10,
	SENSOR_TYPE_JXK05,
	SENSOR_TYPE_JXH63,
	SENSOR_TYPE_SC2335,
	SENSOR_TYPE_JXF37,
	SENSOR_TYPE_GC4653,
	SENSOR_TYPE_C23A98,
	SENSOR_TYPE_SC3335,
	SENSOR_TYPE_SC3235,
	SENSOR_TYPE_SC200AI,
	SENSOR_TYPE_SC401AI,
	SENSOR_TYPE_SC500AI,
	SENSOR_TYPE_JXQ03,
	SENSOR_TYPE_JXQ03P,
	SENSOR_TYPE_GC2083,
};

typedef struct SENSOR_INFO_S
{
	unsigned char *name;
} SENSOR_INFO_T;

SENSOR_INFO_T g_sinfo[] =
{
	{"ov9712"},
	{"ov9732"},
	{"ov9750"},
	{"jxh42"},
	{"sc1035"},
	{"sc1135"},
	{"sc1045"},
	{"sc1145"},
	{"ar0130"},
	{"jxh61"},
	{"gc1024"},
	{"gc1064"},
	{"gc2023"},
	{"bf3115"},
	{"imx225"},
	{"ov2710"},
	{"imx323"},
	{"sc2135"},
	{"sp1409"},
	{"jxh62"},
	{"bg0806"},
	{"ov4689"},
	{"jxf22"},
	{"imx322"},
	{"imx291"},
	{"ov2735"},
	{"sc3035"},
	{"ar0237"},
	{"sc2145"},
	{"jxh65"},
	{"sc2300"},
	{"ov2735b"},
	{"jxv01"},
	{"ps5230"},
	{"ps5250"},
	{"ov2718"},
	{"ov2732"},
	{"sc2235"},
	{"jxk02"},
	{"ov7740"},
	{"hm2140"},
	{"gc2033"},
	{"jxf28"},
	{"os02b10"},
	{"os05a10"},
	{"sc2232"},
	{"sc2232h"},
	{"sc2230"},
	{"sc4236"},
	{"sc1245"},
	{"sc1245a"},
	{"gc1034"},
	{"sc1235"},
	{"jxf23"},
	{"ps5270"},
	{"sp140a"},
	{"sc2310"},
	{"hm2131"},
	{"mis2003"},
	{"jxk03"},
	{"sc5235"},
	{"ov5648"},
	{"ps5280"},
	{"jxf23s"},
	{"gc2053"},
	{"sc4335"},
	{"ps5260"},
	{"os04b10"},
	{"jxk05"},
	{"jxh63"},
	{"sc2335"},
	{"jxf37"},
	{"gc4653"},
	{"c23a98"},
	{"sc3335"},
	{"sc3235"},
	{"sc200ai"},
	{"sc401ai"},
	{"sc500ai"},
	{"jxq03"},
	{"jxq03p"},
	{"gc2083"},
};
int main(int argc,char **argv)
{
	int ret  = 0;
	int fd   = 0;
	int data = -1;
	/* open device file */
	fd = open("/dev/sinfo", O_RDWR);
	if (-1 == fd) {
		printf("err: open failed\n");
		return -1;
	}
	/* iotcl to get sensor info. */
	/* cmd is IOCTL_SINFO_GET, data note sensor type according to SENSOR_TYPE */

	ret = ioctl(fd,IOCTL_SINFO_GET,&data);
	if (0 != ret) {
		printf("err: ioctl failed\n");
		return ret;
	}
	if (SENSOR_TYPE_INVALID == data)
		printf("##### sensor not found\n");
	else
		printf("##### sensor : %s\n", g_sinfo[data].name);

	/* close device file */
	close(fd);
	return 0;
}
