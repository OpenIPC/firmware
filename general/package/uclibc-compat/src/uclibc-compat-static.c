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
 * (SP+4 and SP+8) as uint32_t, pick the page-aligned one.  For
 * offset=0 from either ABI this resolves to 0 cleanly; for vendor MMZ
 * mmaps with a non-zero page-aligned PA at SP+4 the heuristic still
 * picks SP+4.  Failure mode: uninit padding accidentally page-aligned
 * (~1/4096 calls); kernel still validates, so worst case is a one-off
 * EINVAL, never silent corruption.
 * ====================================================================== */

#ifndef SYS_mmap2
#define SYS_mmap2 192
#endif

static uint32_t pick_offset(uint32_t slot_a, uint32_t slot_b)
{
	int a_pa = (slot_a & 0xFFF) == 0;
	int b_pa = (slot_b & 0xFFF) == 0;
	if (a_pa && !b_pa) return slot_a;
	if (b_pa)          return slot_b;
	return slot_a;
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
