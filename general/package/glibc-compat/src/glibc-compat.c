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
 * MPP set: __xstat (struct stat ABI bridge) and __isoc99_sscanf, and by the
 * SigmaStar infinity3 (MSC313E) SDK libs: glibc's internal string names,
 * pthread_cleanup_push's cancel registration, mallinfo and backtrace.
 *
 * musl exports its own __xstat/__fxstat/__lxstat, filling its legacy
 * 104-byte struct; the vendor's buffer is glibc's 88-byte one. Lookup goes
 * in load order and musl appends libc when it is first NEEDED, so these only
 * win when the executable links -lglibc-compat ahead of libc.
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

/* glibc's internal names for the public string functions */
char *__strdup(const char *s)
{
	return strdup(s);
}

char *__strndup(const char *s, size_t n)
{
	return strndup(s, n);
}

char *__strsep_g(char **stringp, const char *delim)
{
	return strsep(stringp, delim);
}

/*
 * pthread_cleanup_push/pop as compiled against glibc. glibc longjmps into the
 * registered buffer when the thread is cancelled; musl knows nothing of it,
 * so a cancelled vendor thread exits without running its cleanup handlers.
 * The infinity3 libs cancel threads only when tearing a pipeline down
 * (pthread_cancel in libISP and libCamUtil); a normal pop runs the handler
 * inline and never reaches these.
 */
void __pthread_register_cancel(void *buf)
{
	(void)buf;
}

void __pthread_unregister_cancel(void *buf)
{
	(void)buf;
}

void __pthread_unwind_next(void *buf)
{
	(void)buf;
	pthread_exit(PTHREAD_CANCELED);
}

/* infinity3 libmi's memory and crash reports: report nothing, never crash */
struct glibc_mallinfo {
	int arena, ordblks, smblks, hblks, hblkhd;
	int usmblks, fsmblks, uordblks, fordblks, keepcost;
};

struct glibc_mallinfo mallinfo(void)
{
	struct glibc_mallinfo mi = { 0 };

	return mi;
}

int backtrace(void **buffer, int size)
{
	(void)buffer;
	(void)size;
	return 0;
}

char **backtrace_symbols(void *const *buffer, int size)
{
	(void)buffer;
	return calloc(size > 0 ? size : 1, sizeof(char *));
}
