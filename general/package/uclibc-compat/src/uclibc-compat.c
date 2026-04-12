/*
 * uclibc-compat.c -- ABI compatibility shim for vendor binaries on musl
 *
 * OpenIPC ships vendor .so libraries compiled against uclibc (or old glibc)
 * that must run on musl.  This shim bridges the ABI gaps:
 *
 *   - struct stat translation (uclibc stat64=96B vs musl=152B)
 *   - mmap off_t mismatch (uclibc=4B vs musl=8B)
 *   - Missing symbols (__ctype_b, __assert, __fgetc_unlocked, etc.)
 *   - C11 Annex K "safe" functions (memcpy_s, snprintf_s, etc.)
 *
 * HOW TO USE:
 *
 *   Executables that load vendor .so files must link this library
 *   statically (-luclibc-compat) so its symbols appear in the
 *   executable's scope BEFORE musl's.  The dynamic linker then
 *   resolves vendor imports from the executable first.
 *
 * Struct layouts are from:
 *   - uclibc: arch/arm/include/uapi/asm/stat.h (struct stat64)
 *   - musl:   arch/arm/bits/stat.h
 *   - Verified by audit-vendor-abi.py struct probe
 *
 * See: https://github.com/OpenIPC/firmware/issues/1992
 */

#define _GNU_SOURCE
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

/* ======================================================================
 * stat/fstat/lstat -- struct stat ABI translation
 *
 * uclibc ARM stat64: 96 bytes (from kernel's struct stat64)
 * musl ARM stat:    152 bytes (from musl's bits/stat.h)
 *
 * We bypass both libc's stat() and go straight to the fstatat64
 * syscall, then translate the kernel's result into uclibc layout.
 * ====================================================================== */

struct uclibc_stat64 {
	unsigned long long	st_dev;		/*  0 */
	unsigned char		__pad0[4];	/*  8 */
	unsigned long		__st_ino;	/* 12 */
	unsigned int		st_mode;	/* 16 */
	unsigned int		st_nlink;	/* 20 */
	unsigned long		st_uid;		/* 24 */
	unsigned long		st_gid;		/* 28 */
	unsigned long long	st_rdev;	/* 32 */
	unsigned char		__pad3[4];	/* 40 */
	long long		st_size;	/* 44 */
	unsigned long		st_blksize;	/* 52 */
	unsigned long long	st_blocks;	/* 56 */
	unsigned long		st_atime_;	/* 64 */
	unsigned long		st_atime_nsec;	/* 68 */
	unsigned long		st_mtime_;	/* 72 */
	unsigned long		st_mtime_nsec;	/* 76 */
	unsigned long		st_ctime_;	/* 80 */
	unsigned long		st_ctime_nsec;	/* 84 */
	unsigned long long	st_ino;		/* 88 */
};						/* 96 bytes */

/*
 * The kernel fills struct stat64 (same as uclibc_stat64 above) directly
 * via the fstatat64 syscall.  No translation needed -- the kernel struct
 * IS the uclibc struct.
 */

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

__attribute__((visibility("default")))
int stat(const char *path, void *buf)
{
	return syscall(__NR_fstatat64, AT_FDCWD, path, buf, 0);
}

__attribute__((visibility("default")))
int lstat(const char *path, void *buf)
{
	return syscall(__NR_fstatat64, AT_FDCWD, path, buf, AT_SYMLINK_NOFOLLOW);
}

__attribute__((visibility("default")))
int fstat(int fd, void *buf)
{
	return syscall(__NR_fstatat64, fd, "", buf, AT_EMPTY_PATH);
}

/* stat64/fstat64/lstat64 -- same as above, uclibc uses these names */
__attribute__((visibility("default"))) int stat64(const char *p, void *b)  { return stat(p, b); }
__attribute__((visibility("default"))) int lstat64(const char *p, void *b) { return lstat(p, b); }
__attribute__((visibility("default"))) int fstat64(int fd, void *b)        { return fstat(fd, b); }

/* ======================================================================
 * mmap -- off_t width mismatch
 *
 * uclibc: offset is uint32_t (4 bytes, page-aligned)
 * musl:   offset is off_t (8 bytes)
 *
 * On ARM, this changes the calling convention -- the offset argument
 * occupies different registers.  Bypass entirely with raw mmap2 syscall.
 * Same approach as majestic's BROKEN_MMAP.
 * ====================================================================== */

#ifndef SYS_mmap2
#define SYS_mmap2 192
#endif

__attribute__((visibility("default")))
void *mmap(void *addr, size_t len, int prot, int flags, int fd, uint32_t offset)
{
	return (void *)syscall(SYS_mmap2, addr, len, prot, flags, fd, offset >> 12);
}

__attribute__((visibility("default")))
void *mmap64(void *addr, size_t len, int prot, int flags, int fd, uint32_t offset)
{
	return (void *)syscall(SYS_mmap2, addr, len, prot, flags, fd, offset >> 12);
}

/* ======================================================================
 * Missing uclibc/glibc symbols
 * ====================================================================== */

/*
 * __ctype_b -- uclibc uses a direct pointer to the ctype classification
 * table, while musl uses __ctype_b_loc() which returns pointer-to-pointer.
 * Vendor binaries that import __ctype_b will dereference wrong indirection.
 *
 * We provide a global pointer initialized to a static ASCII table.
 * This covers the common case (vendor code doing isdigit/isalpha on
 * file paths and configuration strings).
 */
static const unsigned short int __uclibc_ctype_b_data[384] = {
	[0 ... 255] = 0,
	/* ASCII control chars 0-31, 127: iscntrl */
	[128+ 0] = 0x0002, [128+ 1] = 0x0002, [128+ 2] = 0x0002,
	[128+ 3] = 0x0002, [128+ 4] = 0x0002, [128+ 5] = 0x0002,
	[128+ 6] = 0x0002, [128+ 7] = 0x0002, [128+ 8] = 0x0002,
	/* tab, newline, vtab, formfeed, carriage return: iscntrl|isspace */
	[128+ 9] = 0x2002, [128+10] = 0x2002, [128+11] = 0x2002,
	[128+12] = 0x2002, [128+13] = 0x2002,
	[128+14] = 0x0002, [128+15] = 0x0002, [128+16] = 0x0002,
	[128+17] = 0x0002, [128+18] = 0x0002, [128+19] = 0x0002,
	[128+20] = 0x0002, [128+21] = 0x0002, [128+22] = 0x0002,
	[128+23] = 0x0002, [128+24] = 0x0002, [128+25] = 0x0002,
	[128+26] = 0x0002, [128+27] = 0x0002, [128+28] = 0x0002,
	[128+29] = 0x0002, [128+30] = 0x0002, [128+31] = 0x0002,
	/* space: isspace|isprint|isblank */
	[128+32] = 0x6001,
	/* !-/ : isprint|ispunct */
	[128+33] = 0x4004, [128+34] = 0x4004, [128+35] = 0x4004,
	[128+36] = 0x4004, [128+37] = 0x4004, [128+38] = 0x4004,
	[128+39] = 0x4004, [128+40] = 0x4004, [128+41] = 0x4004,
	[128+42] = 0x4004, [128+43] = 0x4004, [128+44] = 0x4004,
	[128+45] = 0x4004, [128+46] = 0x4004, [128+47] = 0x4004,
	/* 0-9: isprint|isdigit|isxdigit */
	[128+48] = 0x4408, [128+49] = 0x4408, [128+50] = 0x4408,
	[128+51] = 0x4408, [128+52] = 0x4408, [128+53] = 0x4408,
	[128+54] = 0x4408, [128+55] = 0x4408, [128+56] = 0x4408,
	[128+57] = 0x4408,
	/* :-@ : isprint|ispunct */
	[128+58] = 0x4004, [128+59] = 0x4004, [128+60] = 0x4004,
	[128+61] = 0x4004, [128+62] = 0x4004, [128+63] = 0x4004,
	[128+64] = 0x4004,
	/* A-F: isprint|isupper|isalpha|isxdigit */
	[128+65] = 0x4510, [128+66] = 0x4510, [128+67] = 0x4510,
	[128+68] = 0x4510, [128+69] = 0x4510, [128+70] = 0x4510,
	/* G-Z: isprint|isupper|isalpha */
	[128+71] = 0x4110, [128+72] = 0x4110, [128+73] = 0x4110,
	[128+74] = 0x4110, [128+75] = 0x4110, [128+76] = 0x4110,
	[128+77] = 0x4110, [128+78] = 0x4110, [128+79] = 0x4110,
	[128+80] = 0x4110, [128+81] = 0x4110, [128+82] = 0x4110,
	[128+83] = 0x4110, [128+84] = 0x4110, [128+85] = 0x4110,
	[128+86] = 0x4110, [128+87] = 0x4110, [128+88] = 0x4110,
	[128+89] = 0x4110, [128+90] = 0x4110,
	/* [-` : isprint|ispunct */
	[128+91] = 0x4004, [128+92] = 0x4004, [128+93] = 0x4004,
	[128+94] = 0x4004, [128+95] = 0x4004, [128+96] = 0x4004,
	/* a-f: isprint|islower|isalpha|isxdigit */
	[128+97] = 0x4520, [128+98] = 0x4520, [128+99] = 0x4520,
	[128+100]= 0x4520, [128+101]= 0x4520, [128+102]= 0x4520,
	/* g-z: isprint|islower|isalpha */
	[128+103]= 0x4120, [128+104]= 0x4120, [128+105]= 0x4120,
	[128+106]= 0x4120, [128+107]= 0x4120, [128+108]= 0x4120,
	[128+109]= 0x4120, [128+110]= 0x4120, [128+111]= 0x4120,
	[128+112]= 0x4120, [128+113]= 0x4120, [128+114]= 0x4120,
	[128+115]= 0x4120, [128+116]= 0x4120, [128+117]= 0x4120,
	[128+118]= 0x4120, [128+119]= 0x4120, [128+120]= 0x4120,
	[128+121]= 0x4120, [128+122]= 0x4120,
	/* {-~ : isprint|ispunct */
	[128+123]= 0x4004, [128+124]= 0x4004, [128+125]= 0x4004,
	[128+126]= 0x4004,
	/* DEL: iscntrl */
	[128+127]= 0x0002,
};

__attribute__((visibility("default")))
const unsigned short int *__ctype_b = &__uclibc_ctype_b_data[128];

/* __assert -- uclibc: __assert(msg, file, line), no func parameter */
__attribute__((visibility("default")))
void __assert(const char *msg, const char *file, int line)
{
	fprintf(stderr, "%s:%d: assertion '%s' failed\n", file, line, msg);
	_exit(134);
}

/* __fgetc_unlocked / __fputc_unlocked -- uclibc internals */
__attribute__((visibility("default")))
int __fgetc_unlocked(FILE *stream)
{
	return fgetc(stream);
}

__attribute__((visibility("default")))
int __fputc_unlocked(int c, FILE *stream)
{
	return fputc(c, stream);
}

/* _stdlib_mb_cur_max -- uclibc internal, return 1 for C/POSIX locale */
__attribute__((visibility("default")))
size_t _stdlib_mb_cur_max(void)
{
	return 1;
}

/* __aeabi_d2iz -- ARM EABI double-to-int, missing from musl */
__attribute__((visibility("default")))
int __aeabi_d2iz(double x)
{
	return (int)x;
}

/* ======================================================================
 * C11 Annex K "safe" functions (HiSilicon's libsecurec uses these)
 * ====================================================================== */

typedef int errno_t;

__attribute__((visibility("default")))
errno_t memcpy_s(void *dest, size_t dmax, const void *src, size_t slen)
{
	if (!dest || !src || slen > dmax)
		return EINVAL;
	memcpy(dest, src, slen);
	return 0;
}

__attribute__((visibility("default")))
errno_t memmove_s(void *dest, size_t dmax, const void *src, size_t slen)
{
	if (!dest || !src || slen > dmax)
		return EINVAL;
	memmove(dest, src, slen);
	return 0;
}

__attribute__((visibility("default")))
errno_t memset_s(void *dest, size_t dmax, int c, size_t n)
{
	if (!dest || n > dmax)
		return EINVAL;
	memset(dest, c, n);
	return 0;
}

__attribute__((visibility("default")))
errno_t strncpy_s(char *dest, size_t dmax, const char *src, size_t slen)
{
	if (!dest || !src || dmax == 0)
		return EINVAL;
	size_t n = slen < dmax ? slen : dmax - 1;
	memcpy(dest, src, n);
	dest[n] = '\0';
	return 0;
}

__attribute__((visibility("default")))
errno_t strncat_s(char *dest, size_t dmax, const char *src, size_t slen)
{
	if (!dest || !src || dmax == 0)
		return EINVAL;
	size_t dlen = strlen(dest);
	if (dlen >= dmax)
		return EINVAL;
	size_t remaining = dmax - dlen - 1;
	size_t n = slen < remaining ? slen : remaining;
	memcpy(dest + dlen, src, n);
	dest[dlen + n] = '\0';
	return 0;
}

__attribute__((visibility("default")))
int snprintf_s(char *dest, size_t dmax, const char *fmt, ...)
{
	if (!dest || dmax == 0 || !fmt)
		return -1;
	va_list ap;
	va_start(ap, fmt);
	int ret = vsnprintf(dest, dmax, fmt, ap);
	va_end(ap);
	return ret;
}
