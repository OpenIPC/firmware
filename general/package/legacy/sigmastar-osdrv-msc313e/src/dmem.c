/*
 * dmem -req <size> [name] -- reserve a block of the infinity3 kernel's direct
 * memory under a name, as load_mstar does for the media buffers before the
 * middleware starts. <size> is decimal, or hex with 0x.
 *
 * The msys driver checks a version header on every request (MSYS_DMEM_INFO,
 * IOCTL_MSYS_VERSION 0x0100 in drivers/mstar/include/mdrv_msys_io_st.h) and
 * refuses one without it, so the request has to carry it.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define IOCTL_MSYS_REQUEST_DMEM	_IO('S', 0x10)
#define VERCHK_HEADER		0x4D530000
#define IOCTL_MSYS_VERSION	0x0100

struct msys_dmem_info {
	unsigned int verchk_version;
	char name[16];
	unsigned int length;
	unsigned long long phys;
	unsigned long long kvirt;
	unsigned int option;
	unsigned int verchk_size;
} __attribute__((packed));

int main(int argc, char **argv)
{
	struct msys_dmem_info info;
	int fd;

	if (argc < 3 || argc > 4 || strcmp(argv[1], "-req")) {
		fprintf(stderr, "usage: dmem -req <size> [name]\n");
		return 1;
	}

	memset(&info, 0, sizeof(info));
	info.verchk_version = VERCHK_HEADER | IOCTL_MSYS_VERSION;
	info.verchk_size = sizeof(info);
	info.length = strtoul(argv[2], NULL, 0);
	if (argc == 4)
		snprintf(info.name, sizeof(info.name), "%s", argv[3]);

	fd = open("/dev/msys", O_RDWR);
	if (fd < 0) {
		perror("dmem: /dev/msys");
		return 1;
	}
	if (ioctl(fd, IOCTL_MSYS_REQUEST_DMEM, &info)) {
		fprintf(stderr, "dmem: %s: request for 0x%x failed\n",
			info.name, info.length);
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}
