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
 * Dual-ABI peek (subtle):
 *
 * The static lib lives in the executable, so it intercepts mmap() calls
 * from BOTH the vendor blob (glibc 32-bit off_t) AND from musl-built code
 * that gets linked into the same binary (e.g. LVGL's lv_linux_fbdev, or
 * any static library that issues mmap from user code). ARM EABI places the
 * 6th arg differently depending on off_t width:
 *
 *   glibc 32-bit off_t: offset at SP+4 (the slot right after `fd`)
 *   musl  64-bit off_t: SP+4..7 is alignment padding, offset_lo at SP+8
 *
 * A `void *mmap(..., uint32_t offset)` signature reads SP+4. For musl
 * callers that slot holds uninitialised stack — garbage — and the kernel
 * rejects the resulting pgoff with EINVAL. The bug is invisible for
 * minimal vendor-only consumers (which is what PR #2110 tested) but bites
 * the moment a consumer mixes vendor MPP with non-trivial musl-static
 * code: e.g. a dvr_home demo that statically links LVGL fails to mmap
 * /dev/fb0 even though the bare-mmap path in a simpler binary works.
 *
 * Fix: declare mmap/mmap64 variadic, va_arg-peek BOTH candidate slots
 * (SP+4 and SP+8) as uint32_t — uint32_t avoids the 8-byte alignment that
 * va_arg(ap, uint64_t) would apply on ARM — then pick the slot whose
 * value is page-aligned. For the common offset=0 case from either ABI it
 * resolves to 0 cleanly; for vendor MMZ mmaps with a non-zero page-aligned
 * physical address at SP+4 the heuristic still picks SP+4. The only
 * failure mode is musl's uninit-padding happening to be page-aligned by
 * coincidence (~1/4096 calls) — and that only matters if the musl caller
 * ALSO has offset=0 and the picked SP+4 value happens to be a valid
 * unrelated mapping (much rarer); for any value the kernel still validates,
 * so worst case is EINVAL on a single startup call, never silent corruption.
 *
 * Pattern mirrors OpenIPC/firmware#2000 (uclibc-compat-static for the
 * hi3516cv100 vendor blobs); see also PR #2110 for the glibc V2 SoC port
 * and PR #1993 for the audit tool that finds these mismatches statically.
 */

#define _GNU_SOURCE
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>

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
