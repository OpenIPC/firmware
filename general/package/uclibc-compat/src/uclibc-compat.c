/*
 * uclibc-compat.c -- ABI compatibility shim for vendor binaries on musl
 *
 * OpenIPC ships vendor .so libraries compiled against uclibc (or old glibc)
 * that must run on musl.  This shim provides symbols that are MISSING from
 * musl -- symbols that vendor binaries import but musl does not export.
 *
 * IMPORTANT: This .so must NOT export symbols that musl already provides
 * (stat, fstat, lstat, mmap, etc).  Doing so would override musl's
 * implementations process-wide, breaking all code that expects musl's
 * ABI (64-bit off_t, 152-byte struct stat).
 *
 * For struct-passing functions where the ABI differs (stat, mmap), the
 * fix must be in the executable via static linking (-luclibc-compat-static),
 * so the override only affects vendor .so lookups through the executable's
 * symbol scope, not musl's own internal calls.
 *
 * See: https://github.com/OpenIPC/firmware/issues/1992
 */

#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* ======================================================================
 * Missing uclibc/glibc symbols -- safe to export from .so because
 * musl does NOT provide these.
 * ====================================================================== */

/*
 * __ctype_b -- uclibc uses a direct pointer to the ctype classification
 * table, while musl uses __ctype_b_loc() which returns pointer-to-pointer.
 * Vendor binaries that import __ctype_b will dereference wrong indirection.
 *
 * We provide a global pointer initialized to a static ASCII table.
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

/*
 * __ctype_tolower -- uclibc's tolower() lookup table, indexed the same way
 * as __ctype_b: the exported pointer aims at element 128 of a 384-entry
 * table so the valid index range is [-128, 255].
 *
 * The element type is int16_t, NOT the int32_t that glibc uses.  This is
 * not a guess: the consuming code in ingenic-osdrv-t20 libalog.so scales
 * the index by 2 and reads with a signed halfword load --
 *
 *     lw   $3, -0x7fbc($gp)   ; &__ctype_tolower
 *     lw   $3, 0x0($3)        ; the pointer itself
 *     sll  $2, $2, 0x1        ; index * 2  -> 2-byte elements
 *     lh   $2, 0x0($2)        ; signed halfword
 *
 * Using glibc's 4-byte layout here would silently return garbage for every
 * tolower() the vendor libraries perform.  Note that a glibc-built blob
 * (e.g. rockchip-osdrv-rv11xx) would want the int32_t layout instead, so
 * this table is only correct for the uclibc-built platforms it targets.
 */
static const int16_t __uclibc_ctype_tolower_data[384] = {
	 -128,  -127,  -126,  -125,  -124,  -123,  -122,  -121,  -120,  -119,  -118,  -117,
	 -116,  -115,  -114,  -113,  -112,  -111,  -110,  -109,  -108,  -107,  -106,  -105,
	 -104,  -103,  -102,  -101,  -100,   -99,   -98,   -97,   -96,   -95,   -94,   -93,
	  -92,   -91,   -90,   -89,   -88,   -87,   -86,   -85,   -84,   -83,   -82,   -81,
	  -80,   -79,   -78,   -77,   -76,   -75,   -74,   -73,   -72,   -71,   -70,   -69,
	  -68,   -67,   -66,   -65,   -64,   -63,   -62,   -61,   -60,   -59,   -58,   -57,
	  -56,   -55,   -54,   -53,   -52,   -51,   -50,   -49,   -48,   -47,   -46,   -45,
	  -44,   -43,   -42,   -41,   -40,   -39,   -38,   -37,   -36,   -35,   -34,   -33,
	  -32,   -31,   -30,   -29,   -28,   -27,   -26,   -25,   -24,   -23,   -22,   -21,
	  -20,   -19,   -18,   -17,   -16,   -15,   -14,   -13,   -12,   -11,   -10,    -9,
	   -8,    -7,    -6,    -5,    -4,    -3,    -2,    -1,     0,     1,     2,     3,
	    4,     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
	   16,    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
	   28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
	   40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
	   52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
	/* 'A'-'Z' (65-90) fold to 'a'-'z' (97-122); everything else is identity */
	   64,    97,    98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
	  108,   109,   110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
	  120,   121,   122,    91,    92,    93,    94,    95,    96,    97,    98,    99,
	  100,   101,   102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
	  112,   113,   114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
	  124,   125,   126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
	  136,   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
	  148,   149,   150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
	  160,   161,   162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
	  172,   173,   174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
	  184,   185,   186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
	  196,   197,   198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
	  208,   209,   210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
	  220,   221,   222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
	  232,   233,   234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
	  244,   245,   246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
};

__attribute__((visibility("default")))
const int16_t *__ctype_tolower = &__uclibc_ctype_tolower_data[128];

/*
 * __stdin -- uclibc implements the stdin macro as a FILE* variable named
 * __stdin, so vendor binaries import the variable rather than musl's
 * `extern FILE *const stdin`.  musl's stdin is not a constant expression,
 * so the aliasing has to happen in a constructor.
 *
 * This is only safe as long as the vendor code treats the FILE* as opaque
 * (passing it to fgets/fread/fileno).  Code that reaches inside the FILE
 * struct would still break: uclibc's and musl's layouts differ.
 */
__attribute__((visibility("default")))
FILE *__stdin = NULL;

__attribute__((constructor))
static void __uclibc_compat_init(void)
{
	__stdin = stdin;
}

/*
 * __cmsg_nxthdr -- the out-of-line helper behind CMSG_NXTHDR.  glibc and
 * uclibc emit a call to it; musl expands CMSG_NXTHDR entirely inline and so
 * exports nothing.
 *
 * glibc's version walks the buffer with raw pointer arithmetic and trusts
 * the caller to have obtained cmsg from CMSG_FIRSTHDR.  Ancillary data
 * ultimately comes from the kernel via recvmsg(), so this one validates the
 * cursor against the control buffer first and does every bounds test in
 * size_t offsets: computing `cmsg + CMSG_ALIGN(cmsg_len)` and only then
 * checking it would already be undefined behaviour for a corrupt length.
 */
__attribute__((visibility("default")))
struct cmsghdr *__cmsg_nxthdr(struct msghdr *mhdr, struct cmsghdr *cmsg)
{
	if (!mhdr || !cmsg || !mhdr->msg_control)
		return NULL;

	unsigned char *base = (unsigned char *)mhdr->msg_control;
	size_t buflen = mhdr->msg_controllen;
	unsigned char *cur = (unsigned char *)cmsg;

	/* The cursor itself has to lie inside the control buffer. */
	if (cur < base || (size_t)(cur - base) > buflen)
		return NULL;

	size_t offset = (size_t)(cur - base);
	size_t remaining = buflen - offset;
	if (remaining < sizeof(struct cmsghdr))
		return NULL;

	size_t len = cmsg->cmsg_len;
	if (len < sizeof(struct cmsghdr) || len > remaining)
		return NULL;

	/* CMSG_ALIGN can round up past the end, so compare before advancing. */
	size_t step = CMSG_ALIGN(len);
	if (step < len || step > remaining)
		return NULL;

	remaining -= step;
	if (remaining < sizeof(struct cmsghdr))
		return NULL;

	struct cmsghdr *nxt = (struct cmsghdr *)(cur + step);
	size_t nxt_len = nxt->cmsg_len;
	if (nxt_len < sizeof(struct cmsghdr) || nxt_len > remaining)
		return NULL;

	return nxt;
}

/*
 * pthread cancellation cleanup ABI -- glibc/uclibc compile
 * pthread_cleanup_push()/pop() into calls to these three helpers, which
 * maintain a per-thread stack of cleanup buffers consulted only while a
 * cancellation is actually unwinding.  musl implements cleanup handlers
 * with a different, entirely internal mechanism and exports nothing here.
 *
 * LIMITATION: register/unregister are no-ops, which is exactly correct for
 * the overwhelmingly common path where the thread is never cancelled --
 * push/pop then have no observable effect.  If a thread IS cancelled, the
 * handlers pushed this way do not run, so a vendor library that relies on
 * pthread_cancel to release a lock or free a buffer will leak it.  No
 * OpenIPC platform is known to cancel a vendor thread, and the alternative
 * is the library failing to load at all.  __pthread_unwind_next is only
 * reached once cancellation is already unwinding, so it terminates the
 * thread the way the caller expects rather than returning.
 *
 * The buffer is opaque to us, so it is taken as void * -- except where the
 * headers already declare these (glibc), which would make a differing
 * signature a hard error rather than a warning.
 */
#ifdef __GLIBC__
typedef __pthread_unwind_buf_t *uclibc_compat_cancel_buf;
#else
typedef void *uclibc_compat_cancel_buf;
#endif

__attribute__((visibility("default")))
void __pthread_register_cancel(uclibc_compat_cancel_buf buf)
{
	(void)buf;
}

__attribute__((visibility("default")))
void __pthread_unregister_cancel(uclibc_compat_cancel_buf buf)
{
	(void)buf;
}

__attribute__((visibility("default"), noreturn))
void __pthread_unwind_next(uclibc_compat_cancel_buf buf)
{
	(void)buf;
	pthread_exit(PTHREAD_CANCELED);
}

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

/* __aeabi_d2iz -- ARM EABI double-to-int, missing from musl.
 * Cannot use C cast (int)x because GCC emits __aeabi_d2iz for it,
 * causing infinite recursion.  Implement the conversion manually
 * by extracting the IEEE 754 double fields. */
__attribute__((visibility("default")))
int __aeabi_d2iz(double x)
{
	union { double d; unsigned long long u; } u = { .d = x };
	int sign = (u.u >> 63) ? -1 : 1;
	int exp = ((u.u >> 52) & 0x7FF) - 1023;
	if (exp < 0) return 0;
	if (exp > 30) return sign > 0 ? 0x7FFFFFFF : (int)0x80000000;
	unsigned long long mantissa = (u.u & 0x000FFFFFFFFFFFFFULL) | 0x0010000000000000ULL;
	int result;
	if (exp >= 52)
		result = (int)(mantissa << (exp - 52));
	else
		result = (int)(mantissa >> (52 - exp));
	return sign * result;
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
