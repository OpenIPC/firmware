#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

unsigned long parse_int (char *str);

void dumphex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

int main (int argc, char *argv[]) {
	unsigned long addr, length;

	int devmem;
	void *mapping;

	long page_size;
	off_t map_base, extra_bytes;

	char *buf;
	ssize_t ret;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s ADDR LENGTH\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	addr = parse_int(argv[1]);
	length = parse_int(argv[2]);

	devmem = open("/dev/mem", O_RDONLY);
	if (devmem == -1) {
		perror("Could not open /dev/mem");
		goto open_fail;
	}

	page_size = sysconf(_SC_PAGE_SIZE);
	map_base = addr & ~(page_size - 1);
	extra_bytes = addr - map_base;

	mapping = mmap(NULL, length + extra_bytes, PROT_READ, MAP_SHARED,
	               devmem, map_base);
	if (mapping == MAP_FAILED) {
		perror("Could not map memory");
		goto map_fail;
	}

	buf = malloc(length);
	if (buf == NULL) {
		fprintf(stderr, "Failed to allocate memory\n");
		goto alloc_fail;
	}

	memcpy(buf, (char *)mapping + extra_bytes, length);
	
	dumphex(buf,length);

	free(buf);

alloc_fail:
	munmap(mapping, length + extra_bytes);

map_fail:
	close(devmem);

open_fail:
	return EXIT_SUCCESS;
}

unsigned long parse_int (char *str) {
	long long result;
	char *endptr; 

	result = strtoll(str, &endptr, 0);
	if (str == '\0' || *endptr != '\0') {
		fprintf(stderr, "\"%s\" is not a valid number\n", str);
		exit(EXIT_FAILURE);
	}

	return (unsigned long)result;
}
