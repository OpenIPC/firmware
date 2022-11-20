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
	{"gc2083"},
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
	{"imx307"},
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
