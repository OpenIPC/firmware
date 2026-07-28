// SPDX-License-Identifier: GPL-2.0
//
// gk_shim.c - the GK7201V200 majestic LD_PRELOAD fixups (libgk_shim.so).
//
// (Line comments only - no block comments anywhere in this file.)
//
// WHY THIS EXISTS
// ---------------
// On the GK7201V200 (XiongMai xm72010200) die majestic runs an Apr-2023 libxmedia userspace
// on top of the Sep-2023 vendor kernel modules. Three points of that API skew cannot be fixed
// from the kernel side, plus one plain bug in majestic's watchdog ioctl numbers. Each fix below
// was derived from disassembly and proven on hardware; the kernel-side counterpart is
// gk7201_fixup.ko.
//
// WHAT IT DOES (no configuration needed - the defaults ARE the proven recipe)
// --------------------------------------------------------------------------
// 1. HI_MPI_SYS_SetVIVPSSMode: raises the kernel g_quick_start_flag (ioctl 0x4004591f on
//    /dev/sys) for exactly one call, then restores it - the Sep-2023 xm_sys refuses the mode
//    change otherwise (allocated-but-unconfigured VI pipe contexts). Once the mode is accepted
//    it fires OP_CREATEGRP at /dev/vpss_ctx_fix so gk7201_fixup.ko re-runs the vendor's FULL
//    VPSS_CreateGrp with the online branch (ClkEnable + SetChnEn), which the mode change alone
//    skips.
// 2. ioctl 0x40044977 (VI CheckDevOpen) is rejected by the Sep-2023 driver. The open() already
//    proved the node usable and the result is otherwise unused, so it is answered 0.
//    ONE PROBE, HARDCODED, AND IT MUST STAY THAT WAY - see the warning below.
// 3. ioctl 0x40285009 (VPSS CreateGrp): fix 1 has already created and bound grp 0 in the
//    kernel, so the vendor library's own create call comes back EXIST (or, after OP_CREATEGRP,
//    some other benign state). Any non-zero result is mapped to 0.
// 4. ioctl watchdog keepalive: majestic issues the UNDIRECTED legacy number _IO('W',5) = 0x5705
//    ("movw r1, #0x5705" with no movt). Mainline's watchdog core matches the encoded
//    WDIOC_KEEPALIVE = 0x80045705 exactly, so the ping is rejected with -ENOTTY - while
//    watchdog_open() has ALREADY armed the hardware. The board then resets once per timeout
//    WITH MAJESTIC RUNNING FINE. Translating the number is what makes the watchdog usable at
//    all. Confirmed both ways on hardware. Only GETSUPPORT and SETTIMEOUT are encoded correctly
//    in majestic, which is why the timeout took effect but the feeding never did.
//    Translation is applied ONLY to a descriptor readlink() confirms is /dev/watchdog*, so no
//    MPP ioctl can ever be touched.
//
// **NEVER ADD ANOTHER IOCTL TO THE PROBE-ANSWER LIST WITHOUT PROVING IT IS INERT.**
// 0x40045024 used to be answered here as well and it cost months. It is NOT a liveness probe:
// libxmedia issues it exactly once on every freshly opened fd - the group fd AND each channel
// fd - before anything else, and it is what binds that fd to its grp/chn in the driver.
// Swallowing it left channels 1 and 2 bound to nothing, so SetChnAttr and EnableChn both
// returned 0 while the channel was never created: no substream, no MJPEG, no /image.jpg, and
// /proc/umap/vpss listing channel 0 only. Channel 0 survived purely by accident, because
// OP_CREATEGRP creates and enables it in the kernel directly - which is exactly why every
// measurement said "all the API calls succeed" while the result was wrong.
// The general lesson: when every vendor call reports success and the output is still wrong,
// suspect the layer that is FAKING success.
//
// QUIET BY DEFAULT: majestic spawns short-lived helper children (~1/s with a web client
// connected; measured 1699 banners in one session) which inherit LD_PRELOAD, so banners and
// per-call logs print only with GK_SHIM_VERBOSE=1. Errors always print.
// NOTE majestic's stderr does NOT reach syslog on this device. Never take "the shim logged
// nothing" as evidence that it did not fire - I did exactly that, deleted fix 4 as dead code,
// and turned a working watchdog into a reboot loop.
//
// ENV (all optional; nothing needs to be set for normal operation)
//   GK_SHIM_VERBOSE=1     banners + per-intercept logging
//   GK_SHIM_OFF=1         fully inert (A/B control for a bug report)
//   GK_WDT_OFF=1          do not translate majestic's legacy watchdog keepalive. The camera
//                         then reboot-loops unless `cli -s .watchdog.enabled false` is set
//                         first - see fix 4.
//
// NO VI-VPSS MODE OVERRIDE, AND NONE IS NEEDED. There used to be a GK_VIVPSS_MODE knob that
// forced VI_ONLINE_VPSS_OFFLINE, because online mode appeared unable to scale or to feed a
// second VPSS channel. That appearance was the 0x40045024 bug below, not the hardware.
// Measured on hardware after the fix, three channels each way, 15-second windows:
//     online  (majestic's own pipe0=3): chn0 20.0 fps, chn1 640x360 14.9, chn2 MJPEG 5.0
//     offline (forced pipe0=2):         chn0 16.6 fps, chn1 640x360 12.4, chn2 MJPEG 4.1
// Offline is ~17% slower on every channel and spends 14 ms of VPSS hardware time per frame
// (CostTm 14171, OffLineHWTm 13968 in /proc/umap/vpss) that online does not spend at all,
// while buying nothing: online scales a 640x360 substream perfectly well.
//
// BUILD
//   CC=output/host/bin/arm-openipc-linux-musleabi-gcc
//   $CC -shared -fPIC -O2 -Wall -o libgk_shim.so gk_shim.c -ldl
// RUN
//   LD_PRELOAD=/usr/lib/libgk_shim.so majestic

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define SYS_DEV "/dev/sys"
#define SYS_IOC_SET_QUICKSTART 0x4004591fUL
#define VI_CHECKDEVOPEN 0x40044977u
#define VPSS_CREATEGRP 0x40285009u
// HI_DEF_ERR(VPSS, LEVEL_ERROR, HI_ERR_EXIST): 0xa0078004. Measured on hardware - this is
// exactly what the vendor library returns for the create call once fix 1 has bound the group.
#define ERR_VPSS_EXIST 0xa0078004u
#define ATTR_SZ 40
#define OP_CREATEGRP 3

// Identification marker. `strings libgk_shim.so | grep gk-shim` says what a given copy is,
// which md5 cannot: the same source yields three different sums (hand-built, buildroot's
// unstripped build, and target-finalize's stripped copy). Worth having on a board where a
// stale hand-built shim in the overlay silently masks the flashed one.
const char gk_shim_id[] =
	"gk-shim: GK7201V200 v2 - single probe, create-EXIST->0, wdt ioctl xlate, no mode override";

static int (*real_ioctl)(int, int, ...);
static int off, verbose, cg_logged, wdt_off, wdt_logged;

// ---------------- 1. SetVIVPSSMode gate + online grp build ----------------

static int set_quick(uint32_t v)
{
	uint32_t val = v;
	int fd, rc;

	fd = open(SYS_DEV, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "[gk] open(%s): %s\n", SYS_DEV, strerror(errno));
		return -1;
	}
	rc = ioctl(fd, SYS_IOC_SET_QUICKSTART, &val);
	if (rc)
		fprintf(stderr, "[gk] SET_QUICKSTART=%u -> %d errno=%s\n", v, rc, strerror(errno));
	close(fd);
	return rc;
}

static void fire_creategrp_op(void)
{
	unsigned char cmd[8 + ATTR_SZ];
	unsigned op = OP_CREATEGRP;
	unsigned grp = 0;
	int dfd, n;

	memset(cmd, 0, sizeof(cmd));
	memcpy(cmd, &op, 4);
	memcpy(cmd + 4, &grp, 4);
	dfd = open("/dev/vpss_ctx_fix", O_WRONLY);
	if (dfd < 0) {
		fprintf(stderr, "[gk] open /dev/vpss_ctx_fix: %s (insmod gk7201_fixup.ko + mdev -s?)\n",
			strerror(errno));
		return;
	}
	n = write(dfd, cmd, sizeof(cmd));
	if (n != (int)sizeof(cmd))
		fprintf(stderr, "[gk] OP_CREATEGRP write -> %d (%s); dmesg 'gkfix:' has details\n",
			n, n < 0 ? strerror(errno) : "short");
	else if (verbose)
		fprintf(stderr, "[gk] OP_CREATEGRP grp %u fired (dmesg 'gkfix:' has the verdict)\n", grp);
	close(dfd);
}

int HI_MPI_SYS_SetVIVPSSMode(const void *mode)
{
	static int (*real)(const void *);
	int rc, raised = 0;

	if (!real)
		real = (int (*)(const void *))dlsym(RTLD_NEXT, "HI_MPI_SYS_SetVIVPSSMode");
	if (!real) {
		fprintf(stderr, "[gk] !! HI_MPI_SYS_SetVIVPSSMode not found\n");
		return -1;
	}
	if (off)
		return real(mode);

	if (set_quick(1) == 0)
		raised = 1;

	rc = real(mode);
	if (verbose || rc)
		fprintf(stderr, "[gk] SetVIVPSSMode -> 0x%08x%s\n", (unsigned)rc,
			rc ? "" : " (accepted)");

	if (rc == 0)
		fire_creategrp_op();

	// Restore: xm_mipi_rx also reads the flag; behaviour with it left raised is unmeasured.
	if (raised)
		set_quick(0);

	fflush(stderr);
	return rc;
}

// ---------------- 4. legacy watchdog ioctl numbers ----------------

// _IO('W',n) -> the _IOR/_IOWR-encoded constant mainline actually matches on.
static unsigned wdt_translate(unsigned req)
{
	static const unsigned map[] = {
		0x80285700u,   // WDIOC_GETSUPPORT     _IOR('W',0,struct watchdog_info) = 40 bytes
		0x80045701u,   // WDIOC_GETSTATUS
		0x80045702u,   // WDIOC_GETBOOTSTATUS
		0x80045703u,   // WDIOC_GETTEMP
		0x80045704u,   // WDIOC_SETOPTIONS
		0x80045705u,   // WDIOC_KEEPALIVE      <- the one that actually feeds the dog
		0xc0045706u,   // WDIOC_SETTIMEOUT     _IOWR
		0x80045707u,   // WDIOC_GETTIMEOUT
		0xc0045708u,   // WDIOC_SETPRETIMEOUT  _IOWR
		0x80045709u,   // WDIOC_GETPRETIMEOUT
		0x8004570au,   // WDIOC_GETTIMELEFT
	};
	unsigned nr = req & 0xffu;

	if ((req & 0xffffff00u) != 0x5700u || nr >= sizeof(map) / sizeof(map[0]))
		return req;
	return map[nr];
}

// Deliberately NOT cached: a closed fd number gets reused, and translating a foreign ioctl
// would be far worse than one readlink() every few seconds on a call that is this rare.
static int fd_is_watchdog(int fd)
{
	char p[32], t[64];
	ssize_t n;

	snprintf(p, sizeof(p), "/proc/self/fd/%d", fd);
	n = readlink(p, t, sizeof(t) - 1);
	if (n <= 0)
		return 0;
	t[n] = '\0';
	return strncmp(t, "/dev/watchdog", 13) == 0;
}

// ---------------- 2+3+4. ioctl(): probe answer, create EXIST->0, watchdog ----------------

int ioctl(int fd, int req, ...)
{
	void *arg;
	va_list ap;
	int rc, e;

	va_start(ap, req);
	arg = va_arg(ap, void *);
	va_end(ap);

	if (!real_ioctl)
		real_ioctl = (int (*)(int, int, ...))dlsym(RTLD_NEXT, "ioctl");
	if (off)
		return real_ioctl(fd, req, arg);

	if (!wdt_off && ((unsigned)req & 0xffffff00u) == 0x5700u) {
		unsigned nreq = wdt_translate((unsigned)req);

		if (nreq != (unsigned)req && fd_is_watchdog(fd)) {
			if (verbose || !wdt_logged) {
				wdt_logged = 1;
				fprintf(stderr, "[gk] wdt ioctl 0x%04x -> 0x%08x\n",
					(unsigned)req, nreq);
				fflush(stderr);
			}
			return real_ioctl(fd, (int)nreq, arg);
		}
	}

	if ((unsigned)req == VI_CHECKDEVOPEN) {
		if (verbose)
			fprintf(stderr, "[gk] VI CheckDevOpen -> 0\n");
		return 0;
	}

	if ((unsigned)req != VPSS_CREATEGRP)
		return real_ioctl(fd, req, arg);

	errno = 0;
	rc = real_ioctl(fd, req, arg);
	e = errno;
	if (rc == 0)
		return 0;

	if (verbose || !cg_logged) {
		int is_exist = ((unsigned)rc == ERR_VPSS_EXIST) ||
			       (rc < 0 && (e == EEXIST || e == EBUSY));

		cg_logged = 1;
		fprintf(stderr, "[gk] vpss create real=0x%08x -> 0 (%s)\n", (unsigned)rc,
			is_exist ? "grp already bound by the online mode, as expected"
				 : "UNEXPECTED code - check dmesg if video is wrong");
		fflush(stderr);
	}
	return 0;
}

// ---------------- init ----------------

static void __attribute__((constructor)) gk_init_shim(void)
{
	off     = getenv("GK_SHIM_OFF") ? 1 : 0;
	verbose = getenv("GK_SHIM_VERBOSE") ? 1 : 0;
	wdt_off = getenv("GK_WDT_OFF") ? 1 : 0;

	// Quiet by default: majestic's short-lived helper children inherit LD_PRELOAD and would
	// each print this (measured: 1699 banners/session with a web client open).
	if (verbose)
		fprintf(stderr, "[gk] libgk_shim active%s\n", off ? " (OFF)" : "");
}
