/*
 * glibc-compat-static.c -- ABI wrappers that MUST be statically linked
 *
 * Vendor glibc binaries built against 32-bit off_t (no _FILE_OFFSET_BITS=64,
 * typical of 2014-era SDKs like Hi3520DV200's MPP) call mmap() with a
 * single-register 32-bit offset arg. musl's mmap() expects 64-bit off_t —
 * when the vendor .so binds against musl, the kernel sees uninitialised
 * stack as the high half of pgoff and rejects with EINVAL. Symptom on
 * hi3520dv200: VENC CreateChn returns 0xa007800c (EN_ERR_VENC_NOMEM)
 * because libmpi can't mmap the model_buf via /dev/venc.
 *
 * This function MUST be linked statically into the EXECUTABLE
 * (-lglibc-compat-static) so it appears in the executable's symbol
 * table. The dynamic linker then resolves vendor .so imports from the
 * executable BEFORE musl. Putting it in a shared library would override
 * musl's mmap process-wide, breaking musl's own internal mmap callers
 * (malloc, dlopen, ...).
 *
 * Pattern mirrors OpenIPC/firmware#2000 (uclibc-compat-static for the
 * hi3516cv100 vendor blobs).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>

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
