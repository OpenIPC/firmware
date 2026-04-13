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

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>

/* ======================================================================
 * stat/fstat/lstat -- struct stat ABI translation
 *
 * uclibc ARM stat64: 96 bytes (from kernel's struct stat64)
 * musl ARM stat:    152 bytes
 *
 * The kernel fills struct stat64 directly via the fstatat64 syscall.
 * Vendor binaries expect the 96-byte kernel layout, so we bypass musl
 * entirely and syscall directly.
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

int stat(const char *path, void *buf)
{
	return syscall(__NR_fstatat64, AT_FDCWD, path, buf, 0);
}

int lstat(const char *path, void *buf)
{
	return syscall(__NR_fstatat64, AT_FDCWD, path, buf, AT_SYMLINK_NOFOLLOW);
}

int fstat(int fd, void *buf)
{
	return syscall(__NR_fstatat64, fd, "", buf, AT_EMPTY_PATH);
}

int stat64(const char *p, void *b)  { return stat(p, b); }
int lstat64(const char *p, void *b) { return lstat(p, b); }
int fstat64(int fd, void *b)        { return fstat(fd, b); }

/* ======================================================================
 * mmap -- off_t width mismatch
 *
 * uclibc: offset is uint32_t (4 bytes, page-aligned)
 * musl:   offset is off_t (8 bytes)
 *
 * On ARM, this changes the calling convention.  Bypass with raw syscall.
 * ====================================================================== */

#ifndef SYS_mmap2
#define SYS_mmap2 192
#endif

void *mmap(void *addr, size_t len, int prot, int flags, int fd, uint32_t offset)
{
	return (void *)syscall(SYS_mmap2, addr, len, prot, flags, fd, offset >> 12);
}

void *mmap64(void *addr, size_t len, int prot, int flags, int fd, uint32_t offset)
{
	return (void *)syscall(SYS_mmap2, addr, len, prot, flags, fd, offset >> 12);
}
