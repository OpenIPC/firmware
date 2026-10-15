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

/* dl_iterate_phdr, dlsym(RTLD_NEXT) and getauxval for the mmap dispatch
 * below; glibc reads it at the first include. */
#define _GNU_SOURCE
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
 * mmap -- off_t width mismatch (dual-ABI)
 *
 * uclibc: offset is uint32_t (4 bytes, page-aligned)
 * musl:   offset is off_t (8 bytes)
 *
 * On ARM EABI the 6th arg lands in different stack slots per off_t width:
 *   uclibc 32-bit: offset at SP+4
 *   musl   64-bit: SP+4..7 is alignment padding, offset at SP+8..15
 *
 * This symbol sits in the consumer executable, so every mmap() in the
 * process that is not musl's own internal one reaches it, from callers of
 * both ABIs.  mmap/mmap64 are therefore variadic and read both slots.
 *
 * Which slot is the offset is decided by who called:
 *
 *  - A caller in a SHARED OBJECT is dispatched exactly, by that object's
 *    libc: one that NEEDs uClibc's libc.so.0 passes the 32-bit offset at
 *    SP+4, anything else passes musl's 64-bit one at SP+8.  Guessing here was
 *    wrong for musl callers: SP+4 is padding they never write, and padding is
 *    often 0, which is page-aligned -- so a musl library mapping offset
 *    0x1000 (libispapp_XM530.so's register pages) silently got page 0.
 *
 *  - A caller in the EXECUTABLE cannot be dispatched that way: uClibc-built
 *    vendor archives (libmpi.a, libh264.a, ...) are linked into it beside
 *    musl code.  For those the slot is still inferred: the vendor slot (SP+4)
 *    carries the uClibc 32-bit offset, which is ALWAYS page-aligned for a
 *    valid mmap2, so it wins whenever it is page-aligned -- vendor
 *    register/MMZ mmaps are then deterministically correct (this matters on
 *    the oldest HiSilicon SoCs, where VI/ISP init issues many register mmaps
 *    and an earlier "prefer SP+8" heuristic mis-mapped a register page ->
 *    external abort).  For musl code in the executable SP+4 is padding and
 *    the real low offset is at SP+8, used when SP+4 is unaligned.  That
 *    padding is often 0, and 0 is page-aligned, so musl code in the
 *    executable that maps at a nonzero offset silently gets offset 0.
 *    Such code must not call mmap(): issue SYS_mmap2 itself, as majestic
 *    does for /dev/mem.
 *
 * The mapping itself goes through the next mmap in the lookup chain rather
 * than straight to the kernel: under AddressSanitizer that is its
 * interceptor, which must see every mapping (a raw syscall left them
 * invisible and an instrumented process died with a bus error).  Before the
 * constructor below has run -- a vendor library's own constructor -- the
 * syscall is issued directly.
 * ====================================================================== */

#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <sys/auxv.h>
#include <sys/types.h>

/* The two-slot reading is an ILP32 little-endian fact: there a 64-bit off_t
 * follows fd as padding, low word, high word.  On a 64-bit target uClibc and
 * musl agree -- one register-sized slot -- so the offset is read once and the
 * caller never needs classifying. */
#if __SIZEOF_POINTER__ == 4 && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define UCC_DUAL_ABI 1
#else
#define UCC_DUAL_ABI 0
#endif

#if UCC_DUAL_ABI && !defined(SYS_mmap2)
#define SYS_mmap2 192
#endif

static void *(*ucc_next_mmap)(void *, size_t, int, int, int, off_t);

/* Resolved once, before any thread exists, so mmap() reads it without a
 * race. */
__attribute__((constructor)) static void ucc_resolve_next_mmap(void)
{
	ucc_next_mmap = (void *(*)(void *, size_t, int, int, int, off_t))
		dlsym(RTLD_NEXT, "mmap");
}

#if UCC_DUAL_ABI
enum ucc_caller { UCC_CALLER_EXEC, UCC_CALLER_UCLIBC, UCC_CALLER_MUSL };

struct ucc_lookup {
	uintptr_t pc;
	uintptr_t exec_phdr;
	int found;
	enum ucc_caller kind;
};

static int ucc_classify(struct dl_phdr_info *info, size_t size, void *data)
{
	struct ucc_lookup *l = data;
	const ElfW(Dyn) *dyn = NULL;
	int hit = 0;
	(void)size;

	for (int i = 0; i < info->dlpi_phnum; i++) {
		const ElfW(Phdr) *ph = &info->dlpi_phdr[i];
		uintptr_t lo = info->dlpi_addr + ph->p_vaddr;
		if (ph->p_type == PT_LOAD && l->pc >= lo && l->pc < lo + ph->p_memsz)
			hit = 1;
		else if (ph->p_type == PT_DYNAMIC)
			dyn = (const ElfW(Dyn) *)lo;
	}
	if (!hit)
		return 0;

	l->found = 1;
	if ((uintptr_t)info->dlpi_phdr == l->exec_phdr || !dyn) {
		l->kind = UCC_CALLER_EXEC;
		return 1;
	}

	/* glibc relocates DT_STRTAB in place and musl leaves it as a vaddr, so
	 * add the load bias only to a value that still lies below it. */
	uintptr_t strtab = 0;
	for (const ElfW(Dyn) *d = dyn; d->d_tag != DT_NULL; d++)
		if (d->d_tag == DT_STRTAB)
			strtab = d->d_un.d_ptr;
	if (strtab && strtab < info->dlpi_addr)
		strtab += info->dlpi_addr;

	l->kind = UCC_CALLER_MUSL;
	for (const ElfW(Dyn) *d = dyn; strtab && d->d_tag != DT_NULL; d++)
		if (d->d_tag == DT_NEEDED &&
		    !strcmp((const char *)(strtab + d->d_un.d_val), "libc.so.0"))
			l->kind = UCC_CALLER_UCLIBC;
	return 1;
}

static enum ucc_caller ucc_caller_of(const void *pc)
{
	struct ucc_lookup l = {
		.pc = (uintptr_t)pc,
		.exec_phdr = (uintptr_t)getauxval(AT_PHDR),
	};
	dl_iterate_phdr(ucc_classify, &l);
	return l.found ? l.kind : UCC_CALLER_EXEC;
}

static uint64_t ucc_offset(const void *caller, uint32_t sp4, uint32_t sp8,
			   uint32_t sp12)
{
	switch (ucc_caller_of(caller)) {
	case UCC_CALLER_UCLIBC:
		return sp4;
	case UCC_CALLER_MUSL:
		return ((uint64_t)sp12 << 32) | sp8;
	default:
		return (sp4 & 0xFFF) == 0 ? sp4 : sp8;
	}
}

#endif

static void *ucc_mmap(void *addr, size_t len, int prot, int flags, int fd,
		      uint64_t offset)
{
	if (ucc_next_mmap)
		return ucc_next_mmap(addr, len, prot, flags, fd, (off_t)offset);
#if UCC_DUAL_ABI
	return (void *)syscall(SYS_mmap2, addr, len, prot, flags, fd,
			       (unsigned long)(offset >> 12));
#else
	return (void *)syscall(SYS_mmap, addr, len, prot, flags, fd,
			       (off_t)offset);
#endif
}

#if UCC_DUAL_ABI
void *mmap(void *addr, size_t len, int prot, int flags, int fd, ...)
{
	va_list ap;
	va_start(ap, fd);
	uint32_t sp4 = va_arg(ap, uint32_t);   /* SP+4  -- uClibc offset */
	uint32_t sp8 = va_arg(ap, uint32_t);   /* SP+8  -- musl offset, low */
	uint32_t sp12 = va_arg(ap, uint32_t);  /* SP+12 -- musl offset, high */
	va_end(ap);
	return ucc_mmap(addr, len, prot, flags, fd,
			ucc_offset(__builtin_return_address(0), sp4, sp8, sp12));
}

void *mmap64(void *addr, size_t len, int prot, int flags, int fd, ...)
{
	va_list ap;
	va_start(ap, fd);
	uint32_t sp4 = va_arg(ap, uint32_t);
	uint32_t sp8 = va_arg(ap, uint32_t);
	uint32_t sp12 = va_arg(ap, uint32_t);
	va_end(ap);
	return ucc_mmap(addr, len, prot, flags, fd,
			ucc_offset(__builtin_return_address(0), sp4, sp8, sp12));
}
#else
void *mmap(void *addr, size_t len, int prot, int flags, int fd, ...)
{
	va_list ap;
	va_start(ap, fd);
	off_t offset = va_arg(ap, off_t);
	va_end(ap);
	return ucc_mmap(addr, len, prot, flags, fd, (uint64_t)offset);
}

void *mmap64(void *addr, size_t len, int prot, int flags, int fd, ...)
{
	va_list ap;
	va_start(ap, fd);
	off_t offset = va_arg(ap, off_t);
	va_end(ap);
	return ucc_mmap(addr, len, prot, flags, fd, (uint64_t)offset);
}
#endif
