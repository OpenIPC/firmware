/*
 * glibc-compat.c -- ABI shim that lets vendor binaries linked against
 * libc.so.6 (glibc) run on a musl rootfs.
 *
 * Hi3520DV200's vendor MPP (libmpi.so etc.) is glibc-linked, but glibc
 * itself can't run on the SoC's 3.0.8 kernel (glibc 2.38 requires kernel
 * >= 3.2). The cv100 musl pattern handles uClibc-linked vendors via
 * libuclibc-compat.so + libc.so.0 symlink; we mirror that for glibc.
 *
 * Symbols re-implemented here are the glibc-only ones used by the V200
 * MPP set: __xstat (struct stat ABI bridge) and __isoc99_sscanf.
 */

#define _GNU_SOURCE
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * glibc ARM 32-bit `struct stat` (from glibc-2.11 sys/stat.h, _STAT_VER
 * = 3, ABI used by V200 MPP). musl's struct stat has a different layout.
 * Copy the fields the vendor binary is most likely to read; zero the
 * rest. Vendor MPP only uses stat() for device-node existence checks
 * and st_mode bits, so the precise byte offsets of timespecs etc. are
 * not exercised on this ABI.
 */
/*
 * glibc on ARM 32-bit defines st_atime/st_mtime/st_ctime as macros expanding
 * to st_atim.tv_sec etc. Build the structure with raw timestamp fields named
 * differently and fill the layout out manually so the layout the vendor reads
 * comes out at the right byte offsets.
 */
struct glibc_stat {
	uint64_t st_dev;
	uint16_t __pad1;
	uint32_t glibc_st_ino;
	uint32_t glibc_st_mode;
	uint32_t glibc_st_nlink;
	uint32_t glibc_st_uid;
	uint32_t glibc_st_gid;
	uint64_t st_rdev;
	uint16_t __pad2;
	int32_t  glibc_st_size;
	int32_t  glibc_st_blksize;
	int32_t  glibc_st_blocks;
	int32_t  glibc_st_atime_sec;
	uint32_t glibc_st_atime_nsec;
	int32_t  glibc_st_mtime_sec;
	uint32_t glibc_st_mtime_nsec;
	int32_t  glibc_st_ctime_sec;
	uint32_t glibc_st_ctime_nsec;
	uint32_t __unused4;
	uint32_t __unused5;
};

static int fill_stat(struct glibc_stat *gst, const struct stat *mst)
{
	memset(gst, 0, sizeof(*gst));
	gst->st_dev               = mst->st_dev;
	gst->glibc_st_ino         = mst->st_ino;
	gst->glibc_st_mode        = mst->st_mode;
	gst->glibc_st_nlink       = mst->st_nlink;
	gst->glibc_st_uid         = mst->st_uid;
	gst->glibc_st_gid         = mst->st_gid;
	gst->st_rdev              = mst->st_rdev;
	gst->glibc_st_size        = (int32_t)mst->st_size;
	gst->glibc_st_blksize     = (int32_t)mst->st_blksize;
	gst->glibc_st_blocks      = (int32_t)mst->st_blocks;
	gst->glibc_st_atime_sec   = (int32_t)mst->st_atim.tv_sec;
	gst->glibc_st_atime_nsec  = (uint32_t)mst->st_atim.tv_nsec;
	gst->glibc_st_mtime_sec   = (int32_t)mst->st_mtim.tv_sec;
	gst->glibc_st_mtime_nsec  = (uint32_t)mst->st_mtim.tv_nsec;
	gst->glibc_st_ctime_sec   = (int32_t)mst->st_ctim.tv_sec;
	gst->glibc_st_ctime_nsec  = (uint32_t)mst->st_ctim.tv_nsec;
	return 0;
}

int __xstat(int ver, const char *path, struct glibc_stat *buf)
{
	(void)ver;
	struct stat mst;
	int r = stat(path, &mst);
	if (r == 0)
		fill_stat(buf, &mst);
	return r;
}

int __lxstat(int ver, const char *path, struct glibc_stat *buf)
{
	(void)ver;
	struct stat mst;
	int r = lstat(path, &mst);
	if (r == 0)
		fill_stat(buf, &mst);
	return r;
}

int __fxstat(int ver, int fd, struct glibc_stat *buf)
{
	(void)ver;
	struct stat mst;
	int r = fstat(fd, &mst);
	if (r == 0)
		fill_stat(buf, &mst);
	return r;
}

int __isoc99_sscanf(const char *str, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = vsscanf(str, fmt, ap);
	va_end(ap);
	return r;
}
