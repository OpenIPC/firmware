#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define BOUNDS 0x80000000
#define LENGTH 0x1A000
#define ALIGN 0x1000
#define CMDQ "E_MMAP_ID_CMDQ"
#define SSTAR 0x1F2025A4

bool mem_reg(uint32_t addr, uint32_t *data, int op);

int main() {
	char buf[128];
	int fd, size;
	uint32_t addr, total, val;

	if (!mem_reg(SSTAR, &val, 0)) {
		printf("Failed to read memory address: 0x%X\n", SSTAR);
		return -1;
	}

	val = 1 << (val >> 12);
	if (val == 64) {
		addr = 0x3FC6000;
		total = 0x4000000;
	} else if (val == 128) {
		addr = 0x7FC6000;
		total = 0x8000000;
	} else if (val == 256) {
		addr = 0xFFC6000;
		total = 0x10000000;
	}

	fd = open("/proc/mi_modules/common/memory_info", O_WRONLY);
	if (fd < 0) {
		printf("Failed to open memory_info: %s\n", strerror(errno));
		return -1;
	}

	size = sprintf(buf, "%u %u %u %u %u %u %u\n", total, total, 0, BOUNDS, 1, 1, 0);
	write(fd, buf, size);
	close(fd);

	fd = open("/proc/mi_modules/common/mmap_info", O_WRONLY);
	if (fd < 0) {
		printf("Failed to open mmap_info: %s\n", strerror(errno));
		return -1;
	}

	size = sprintf(buf, "%s %u %u %u %u %u %u %u %u\n", CMDQ, 0, addr, LENGTH, 0, ALIGN, 4, 0, 0);
	write(fd, buf, size);
	close(fd);

	return 0;
}
