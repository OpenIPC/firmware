/*
 * uclibc-compat-static.c -- ABI wrappers that MUST be statically linked
 *
 * These functions override musl's implementations for symbols that musl
 * DOES export but with incompatible ABI (different struct sizes, different
 * argument widths).
 *
 * They MUST be linked statically into the executable (-luclibc-compat-static)
 * so they appear in the executable's symbol table.  The dynamic linker then
 * resolves vendor .so imports from the executable BEFORE musl.
 *
 * They MUST NOT be in a shared library (.so) because that would override
 * musl's implementations process-wide, breaking musl's own internal calls
 * and any code expecting musl's ABI.
 *
 * See: https://github.com/OpenIPC/firmware/issues/1992
 */

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

/* ======================================================================
 * stat/fstat/lstat -- struct stat ABI translation
 *
 * The kernel fills its struct stat64 (104 B on ARM) via the fstatat64
 * syscall.  Vendor binaries built against a modern uClibc expect that
 * kernel layout, so for ordinary files we pass their buffer straight to
 * the syscall -- unchanged behaviour.
 *
 * The exception is the oldest HiSilicon SoCs (Hi3518 V100 / hi3516cv100,
 * Hi3516A V100), whose 2015 libmpi was built against uClibc 0.9.32 where
 * `struct stat` is the NON-LFS 88-byte layout.  Such a libmpi reserves an
 * 88-byte stack buffer; writing the 104-byte kernel struct into it (e.g.
 * the stat()+S_ISCHR() probe that HI_MPI_VDA_CreateChn runs on its
 * register device node) overflows by 16 bytes and clobbers the adjacent
 * VDA_CHN_ATTR_S pointer -> ERR_VDA_NULL_PTR (0xa0098006), so hardware
 * motion detection never starts.
 *
 * We cannot widen those buffers, but the only stats that overflow are on
 * CHARACTER DEVICE nodes (the vendor probes /dev/... for S_IFCHR + rdev).
 * The device-identity fields the vendor reads -- st_mode (off 16) and
 * st_rdev (off 32) -- sit at identical offsets in the 88-, 96- and
 * 104-byte layouts, and a device has no meaningful size/blocks/time.  So
 * for character devices we emit exactly the 88-byte uClibc layout: it
 * never overflows ANY vendor buffer (88 <= 96/104) and carries correct
 * device identity on every SoC.  Everything else keeps the raw kernel
 * layout, so musl-built code in the same process and regular-file stats
 * are completely unaffected.
 *
 * See: https://github.com/OpenIPC/firmware/issues/1992
 * ====================================================================== */

#ifndef __NR_fstatat64
#define __NR_fstatat64 327
#endif
#ifndef AT_FDCWD
#define AT_FDCWD (-100)
#endif
#ifndef AT_EMPTY_PATH
#define AT_EMPTY_PATH 0x1000
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

#define UCC_S_IFMT  0170000
#define UCC_S_IFCHR 0020000

struct ucc_kstat64 {            /* ARM kernel struct stat64 (104 bytes) */
	unsigned long long st_dev;
	unsigned char      __pad0[4];
	unsigned long      __st_ino;
	unsigned int       st_mode;
	unsigned int       st_nlink;
	unsigned long      st_uid;
	unsigned long      st_gid;
	unsigned long long st_rdev;
	unsigned char      __pad3[4];
	long long          st_size;
	unsigned long      st_blksize;
	unsigned long long st_blocks;
	unsigned long      st_atime, st_atime_nsec;
	unsigned long      st_mtime, st_mtime_nsec;
	unsigned long      st_ctime, st_ctime_nsec;
	unsigned long long st_ino;
};

struct ucc_ustat88 {            /* uClibc 0.9.32 ARM non-LFS struct stat (88 bytes) */
	unsigned long long st_dev;
	unsigned char      __pad0[4];
	unsigned long      __st_ino;
	unsigned int       st_mode;
	unsigned int       st_nlink;
	unsigned long      st_uid;
	unsigned long      st_gid;
	unsigned long long st_rdev;
	unsigned char      __pad3[4];
	long               st_size;
	unsigned long      st_blksize;
	unsigned long      st_blocks;
	unsigned long      st_atime, st_atime_nsec;
	unsigned long      st_mtime, st_mtime_nsec;
	unsigned long      st_ctime, st_ctime_nsec;
	unsigned long      __unused4, __unused5;
};

/* Layouts are 32-bit ARM specific (long==4); validate only on that target. */
#if defined(__arm__)
typedef char ucc_ustat88_is_88[sizeof(struct ucc_ustat88) == 88 ? 1 : -1];
typedef char ucc_kstat64_is_104[sizeof(struct ucc_kstat64) == 104 ? 1 : -1];
#endif

static int ucc_stat(int dirfd, const char *path, void *buf, int flags)
{
	struct ucc_kstat64 k;
	int r = syscall(__NR_fstatat64, dirfd, path, &k, flags);
	if (r != 0)
		return r;
	if ((k.st_mode & UCC_S_IFMT) == UCC_S_IFCHR) {
		struct ucc_ustat88 *u = buf;
		u->st_dev     = k.st_dev;
		u->__st_ino   = k.__st_ino;
		u->st_mode    = k.st_mode;
		u->st_nlink   = k.st_nlink;
		u->st_uid     = k.st_uid;
		u->st_gid     = k.st_gid;
		u->st_rdev    = k.st_rdev;
		u->st_size    = (long)k.st_size;
		u->st_blksize = k.st_blksize;
		u->st_blocks  = (unsigned long)k.st_blocks;
		u->st_atime   = k.st_atime;  u->st_atime_nsec = k.st_atime_nsec;
		u->st_mtime   = k.st_mtime;  u->st_mtime_nsec = k.st_mtime_nsec;
		u->st_ctime   = k.st_ctime;  u->st_ctime_nsec = k.st_ctime_nsec;
		u->__unused4 = 0;            u->__unused5 = 0;
	} else {
		memcpy(buf, &k, sizeof(k));
	}
	return r;
}

int stat(const char *path, void *buf)  { return ucc_stat(AT_FDCWD, path, buf, 0); }
int lstat(const char *path, void *buf) { return ucc_stat(AT_FDCWD, path, buf, AT_SYMLINK_NOFOLLOW); }
int fstat(int fd, void *buf)           { return ucc_stat(fd, "", buf, AT_EMPTY_PATH); }

int stat64(const char *p, void *b)  { return stat(p, b); }
int lstat64(const char *p, void *b) { return lstat(p, b); }
int fstat64(int fd, void *b)        { return fstat(fd, b); }

/* ======================================================================
 * mmap -- off_t width mismatch (dual-ABI peek)
 *
 * uclibc: offset is uint32_t (4 bytes, page-aligned)
 * musl:   offset is off_t (8 bytes)
 *
 * On ARM EABI the 6th arg lands in different stack slots per off_t width:
 *   uclibc 32-bit: offset at SP+4
 *   musl   64-bit: SP+4..7 is alignment padding, offset_lo at SP+8
 *
 * The static lib intercepts mmap() for both ABIs since it sits in the
 * consumer executable.  A plain `uint32_t offset` signature reads SP+4 —
 * fine for the uclibc vendor blob, but musl-built static code in the
 * same binary (e.g. LVGL fbdev) gets uninitialised padding from SP+4 and
 * the kernel rejects with EINVAL.  Symptom: lv_linux_fbdev fails to mmap
 * /dev/fb0 even though a simpler bare-mmap binary in the same rootfs
 * works fine.
 *
 * Fix: declare mmap/mmap64 variadic, va_arg-peek BOTH candidate slots
 * (SP+4 and SP+8) as uint32_t.  The vendor slot (SP+4) carries the uClibc
 * 32-bit offset, which is ALWAYS page-aligned for a valid mmap2, so we
 * prefer it whenever it is page-aligned -- vendor register/MMZ mmaps are
 * then deterministically correct (this matters on the oldest HiSilicon
 * SoCs, where VI/ISP init issues many register mmaps and an earlier
 * "prefer SP+8" heuristic mis-mapped a register page -> external abort).
 * For musl callers SP+4 is alignment padding (rarely page-aligned) and the
 * real low offset is at SP+8, so we fall through to SP+8 when SP+4 is
 * unaligned.  Residual failure mode: musl padding accidentally page-aligned
 * and non-zero (~1/4096 calls); the kernel still validates, so worst case
 * is a one-off EINVAL, never silent corruption.
 * ====================================================================== */

#ifndef SYS_mmap2
#define SYS_mmap2 192
#endif

static uint32_t pick_offset(uint32_t slot_a, uint32_t slot_b)
{
	if ((slot_a & 0xFFF) == 0) return slot_a;  /* vendor 32-bit offset (SP+4) */
	return slot_b;                             /* musl 64-bit offset low (SP+8) */
}

void *mmap(void *addr, size_t len, int prot, int flags, int fd, ...)
{
	va_list ap;
	va_start(ap, fd);
	uint32_t slot_a = va_arg(ap, uint32_t);  /* SP+4 — vendor ABI */
	uint32_t slot_b = va_arg(ap, uint32_t);  /* SP+8 — musl ABI low */
	va_end(ap);
	uint32_t offset = pick_offset(slot_a, slot_b);
	return (void *)syscall(SYS_mmap2, addr, len, prot, flags, fd, offset >> 12);
}

void *mmap64(void *addr, size_t len, int prot, int flags, int fd, ...)
{
	va_list ap;
	va_start(ap, fd);
	uint32_t slot_a = va_arg(ap, uint32_t);
	uint32_t slot_b = va_arg(ap, uint32_t);
	va_end(ap);
	uint32_t offset = pick_offset(slot_a, slot_b);
	return (void *)syscall(SYS_mmap2, addr, len, prot, flags, fd, offset >> 12);
}
