// SPDX-License-Identifier: MIT
/*
 * Load the Ambarella S3L DSP microcode through /dev/ucode.
 *
 *   load_ucode DIR
 *
 * A drop-in for the vendor's load_ucode: dsp.ko describes where each file
 * goes (UCODE_GET_INFO), the files are copied into the microcode window it
 * maps, and the version the DSP will run is printed. The DSP itself is not
 * started here; it boots when IAV first enters the idle state.
 *
 * The window is mapped uncached by dsp.ko, so no flush is needed.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define UCODE_DEV "/dev/ucode"
#define UCODE_MAX_ITEMS 4

struct ucode_item {
	uint32_t addr_offset;
	char filename[24];
};

struct ucode_info {
	uint32_t map_size;
	uint32_t nr_item;
	struct ucode_item item[UCODE_MAX_ITEMS];
};

struct ucode_version {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint32_t edition_num;
	uint32_t edition_ver;
	uint32_t arch;
};

#define UCODE_GET_INFO _IOR('u', 0, struct ucode_info)
#define UCODE_GET_VERSION _IOR('u', 1, struct ucode_version)
#define UCODE_LOADED _IOW('u', 2, uint32_t)

struct ucode_file {
	uint8_t *data;
	size_t size;
};

static int read_file(const char *dir, const char *name, size_t room,
		     struct ucode_file *f)
{
	char path[256];
	struct stat st;
	size_t done = 0;
	int fd, ret = -1;

	snprintf(path, sizeof(path), "%s/%s", dir, name);
	if ((fd = open(path, O_RDONLY)) < 0 || fstat(fd, &st) < 0) {
		fprintf(stderr, "load_ucode: %s: %s\n", path, strerror(errno));
		goto out;
	}
	if (st.st_size <= 0 || (size_t)st.st_size > room) {
		fprintf(stderr, "load_ucode: %s is %lld bytes, needs 1..%zu\n",
			path, (long long)st.st_size, room);
		goto out;
	}
	f->size = st.st_size;
	if (!(f->data = malloc(f->size))) {
		fprintf(stderr, "load_ucode: %s: %s\n", path, strerror(errno));
		goto out;
	}

	while (done < f->size) {
		ssize_t n = read(fd, f->data + done, f->size - done);
		if (n <= 0) {
			fprintf(stderr, "load_ucode: reading %s: %s\n", path,
				n ? strerror(errno) : "file shrank");
			goto out;
		}
		done += n;
	}
	ret = 0;
out:
	if (fd >= 0)
		close(fd);
	return ret;
}

int main(int argc, char **argv)
{
	struct ucode_info info;
	struct ucode_version ver;
	struct ucode_file files[UCODE_MAX_ITEMS] = { 0 };
	uint8_t *map;
	int fd, ret = 1;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s DIR\n", argv[0]);
		return 1;
	}

	if ((fd = open(UCODE_DEV, O_RDWR)) < 0) {
		fprintf(stderr, "load_ucode: %s: %s\n", UCODE_DEV,
			strerror(errno));
		return 1;
	}

	memset(&info, 0, sizeof(info));
	if (ioctl(fd, UCODE_GET_INFO, &info) < 0) {
		fprintf(stderr, "load_ucode: UCODE_GET_INFO: %s\n",
			strerror(errno));
		goto out;
	}
	if (!info.map_size || !info.nr_item ||
	    info.nr_item > UCODE_MAX_ITEMS) {
		fprintf(stderr, "load_ucode: bad layout: %u items in 0x%x\n",
			info.nr_item, info.map_size);
		goto out;
	}

	/* Every file is read in full before the window is touched: a set that
	 * failed half way would leave old and new microcode mixed in it. */
	for (uint32_t i = 0; i < info.nr_item; i++) {
		struct ucode_item *it = &info.item[i];
		/* Each file may run up to the next one's offset, the last one
		 * to the end of the window. */
		uint32_t end = info.map_size;

		for (uint32_t j = 0; j < info.nr_item; j++)
			if (info.item[j].addr_offset > it->addr_offset &&
			    info.item[j].addr_offset < end)
				end = info.item[j].addr_offset;

		it->filename[sizeof(it->filename) - 1] = '\0';
		if (it->addr_offset >= info.map_size) {
			fprintf(stderr, "load_ucode: %s at 0x%x is outside 0x%x\n",
				it->filename, it->addr_offset, info.map_size);
			goto out;
		}
		if (read_file(argv[1], it->filename, end - it->addr_offset,
			      &files[i]))
			goto out;
	}

	map = mmap(NULL, info.map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		   0);
	if (map == MAP_FAILED) {
		fprintf(stderr, "load_ucode: mmap 0x%x: %s\n", info.map_size,
			strerror(errno));
		goto out;
	}

	for (uint32_t i = 0; i < info.nr_item; i++) {
		struct ucode_item *it = &info.item[i];

		memcpy(map + it->addr_offset, files[i].data, files[i].size);
		printf("%s: %zu bytes at +0x%06x\n", it->filename,
		       files[i].size, it->addr_offset);
	}

	if (ioctl(fd, UCODE_LOADED, 0) < 0) {
		fprintf(stderr, "load_ucode: UCODE_LOADED: %s\n",
			strerror(errno));
		goto unmap;
	}

	if (ioctl(fd, UCODE_GET_VERSION, &ver) == 0)
		printf("ucode version %u/%u/%u %u.%u\n", ver.year, ver.month,
		       ver.day, ver.edition_num, ver.edition_ver);
	ret = 0;

unmap:
	munmap(map, info.map_size);
out:
	for (int i = 0; i < UCODE_MAX_ITEMS; i++)
		free(files[i].data);
	close(fd);
	return ret;
}
