// SPDX-License-Identifier: GPL-2.0
//
// gk7201_fixup.c - ALL GK7201V200 vendor-module runtime fixups in one module.
//                  Merges isp_noint.ko + vpss_ctx_fix.ko + h264e_ctxfix.ko (all three
//                  hw-proven individually; see their originals for the full derivations).
//
// (Line comments only - no block comments anywhere in this file.)
//
// WHAT IT FIXES (each measured on-device, none guessed)
// -----------------------------------------------------
// 1. Leftover object tables in the vendor modules' .bss - THE root cause of the whole
//    "camera wedges when you save a setting" class. Every vendor MPP module keeps a table
//    of channel/group objects in .bss, and those tables are NOT clean once the modules
//    have finished loading: measured before majestic has ever run, g_grp_ctx[0..3],
//    g_h264e_context[0..3] and g_apstRgnCtrl[0..1] are all non-NULL. Nothing put real
//    objects there - majestic creates them, and it is not running yet. But every module's
//    exit path walks its table and tears down every non-NULL entry, and VPSS_CreateGrp
//    returns EXIST (0xa0078004) for any non-zero slot. So the modules tear down channels
//    that never existed, and refuse to create the ones that should. Fix: clear the tables
//    at insmod, which is the only moment they are provably meant to be empty. This
//    replaces three separate symptom guards that used to patch up the consequences.
// 2. irq39 storm: the Apr-2023 userspace enables an orphaned ISP-FE interrupt that fires
//    millions/sec ("irq 39: nobody cared"). Kprobe on ISP_DRV_SetIntEnable forces the
//    enable arg (r1) to 0 - the int is never enabled. Video is unaffected.
//    isp_int=1 IS NOT A FIX AND MUST NOT BE REVISITED: xm_isp registers no interrupt
//    handler at all (it imports osal_platform_get_irq_byname but NOT osal_request_irq;
//    only xm_vi imports those), so there is nothing to service the line. Measured: with
//    isp_int=1 the board storms within a second, VI's handler returns IRQ_NONE, the kernel
//    disables irq 39, and no video ever arrives. Suppressing the enable is architecturally
//    correct; what it costs is fix 6 below, which restores it a different way.
// 3. VPSS group not built for the online path: the VI-VPSS online bind never runs
//    VPSS_CreateGrp's online branch, so ClkEnable + SetChnEn never happen and no frames
//    flow. At SetVIVPSSMode time the shim writes op 3 to /dev/vpss_ctx_fix, which calls
//    the vendor's OWN full VPSS_CreateGrp - handing an existing group back to
//    VPSS_DestroyGrp first, so a restart is symmetric rather than abandoning live state.
// 4. (REMOVED - kept as a numbered entry so the surviving numbers still match the sections.)
//    VENC h264 chn0 EXIST: h264e_create_chn returned 0xa00a8004 for the same leftover-slot
//    reason as (1), and a kprobe at create entry used to zero the slot. Once (1) cleared the
//    whole table at insmod this became a symptom guard for a cause that no longer exists, so
//    it was measured rather than assumed: the probe logs whenever it acts, and across a cold
//    boot, three graceful majestic restarts and a `killall -9` + restart it never fired once.
//    It never fires even after -9 because -9 only skips USERSPACE teardown: the kernel still
//    closes the dead process's fds, and the driver's release path destroys the channel and
//    clears the slot properly. Removed, along with the a_h264e_text parameter and the
//    OFF_H264E_CREATE offset that existed only for it.
// 5. Graceful teardown Oops (a VENDOR bug): VPSS_DestroyGrp zeroes g_grp_ctx[grp] and then
//    calls VpssCleanPicQueue, which dereferences it. See teardown_fix below.
// 6. Static exposure (AE frozen) - the price of fix 2, paid back without re-enabling the
//    interrupt. One kprobe inside ISP_ISR makes it use the RAW ISP-FE interrupt status
//    instead of (status & enable), which is the single AND gate fix 2 severs. HW-confirmed:
//    exposure and luminance now track the light. See section 6 for the full derivation.
//
// NOTE: this module needs the ARM kprobes fix "reset_current_kprobe() when a pre_handler
// returns non-zero" (kernel patch 0901). Without it only the FIRST pc-redirecting probe in
// the system ever runs, and the fixes here silently stop taking effect once combined.
//
// OFFSETS are hardcoded for THIS vendor .ko set (Sep-2023, gk7205v500 kmod dir):
//   xm_vpss.ko  md5 3b5b0e790d00e6f2d398255236032320
//   (if any vendor .ko changes, re-derive: see the original three modules' headers)
// Base addresses still come from sysfs because kallsyms-by-name walks Oops this kernel:
//   Tv=$(cat /sys/module/xm_vpss/sections/.text)  Bv=$(cat /sys/module/xm_vpss/sections/.bss)
//   Ti=$(cat /sys/module/xm_isp/sections/.text)
//   Bi=$(cat /sys/module/xm_isp/sections/.bss)     Bh=$(cat /sys/module/xm_h264e/sections/.bss)
//   insmod gk7201_fixup.ko a_isp_text=$Ti a_isp_bss=$Bi a_vpss_text=$Tv a_vpss_bss=$Bv
//                          a_h264e_bss=$Bh
//   (one command; load_xm builds the full argument list, this is just the shape)
//   mdev -s        # creates /dev/vpss_ctx_fix (name kept for shim compatibility)
//
// BUILD
//   KDIR=/work/firmware/output/build/linux-custom
//   echo 'obj-m := gk7201_fixup.o' > Makefile
//   make -C $KDIR M=$PWD ARCH=arm CROSS_COMPILE=.../arm-openipc-linux-musleabi- modules

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

// ---- offsets into the vendor modules (fixed for this vendor .ko set) ----
#define OFF_ISP_SETINT     0x1244    // ISP_DRV_SetIntEnable
#define OFF_ISP_ISR_FEMASK 0x172     // ISP_ISR: the `and.w fp, fp, r0` that masks the FE status
#define OFF_ISP_ISR_CAPMASK 0x14a    // ISP_ISR: the `ands r1, r0` that masks the VICAP status
#define OFF_ISP_SWITCHMODE 0x00      // g_SwitchMode  (in .bss)
#define OFF_ISP_USEBOTHALF 0x04      // g_UseBothalf  (in .bss)
#define OFF_VPSS_CREATEGRP 0x1228    // VPSS_CreateGrp
#define OFF_VPSS_DEVCTX    0x6420    // g_dev_ctx  (in .bss)
#define OFF_VPSS_GRPCTX    0x0       // g_grp_ctx  (in .bss)
#define OFF_H264E_CTXTAB   0x0       // g_h264e_context[16] (in .bss)
#define OFF_RGN_CTRLTAB    0x08      // g_apstRgnCtrl[128]   (in xm_rgn .bss)
#define OFF_H265E_CTXTAB   0x40      // g_h265e_ctx[16]      (in xm_h265e .bss)
#define OFF_JPEGE_CTXTAB   0x4c      // g_jpege_chn_ctx[16]  (in xm_jpege .bss)
#define OFF_VPSS_DESTROYGRP 0x1574   // VPSS_DestroyGrp (r0 = grp)
#define OFF_VPSS_DGRP_AFTER 0x1724   // the instruction AFTER its `str 0,[g_grp_ctx+grp*4]`

static unsigned long a_isp_text, a_vpss_text, a_vpss_bss, a_h264e_bss;
static unsigned long a_rgn_bss;                // g_apstRgnCtrl - the OSD region table
module_param(a_rgn_bss, ulong, 0444);
MODULE_PARM_DESC(a_rgn_bss, "xm_rgn .bss base (to clear leftover g_apstRgnCtrl entries)");
static unsigned long a_h265e_bss, a_jpege_bss;   // the other two encoders' ctx tables
module_param(a_h265e_bss, ulong, 0444);
module_param(a_jpege_bss, ulong, 0444);
MODULE_PARM_DESC(a_h265e_bss, "xm_h265e .bss base (for zero_h264ectx)");
MODULE_PARM_DESC(a_jpege_bss, "xm_jpege .bss base (for zero_h264ectx)");
static int isp_int;          // 0 = suppress the ISP interrupt (default), 1 = let it run
static unsigned long a_isp_bss;      // xm_isp .bss - only to log g_SwitchMode/g_UseBothalf
module_param(a_isp_bss, ulong, 0444);
MODULE_PARM_DESC(a_isp_bss, "xm_isp .bss base (optional; used to log g_SwitchMode/g_UseBothalf, which decide whether the ISP bottom half runs inline in hard IRQ)");
static int zero_ispsw = 1;   // clear a leftover g_SwitchMode - see section 6b
module_param(zero_ispsw, int, 0444);
MODULE_PARM_DESC(zero_ispsw, "DEFAULT 1: at insmod, if xm_isp's g_SwitchMode holds neither 0 nor 1, set it to 0. Any other value makes ISP_ISR return without doing anything, which kills AE, IQ and AWB together until majestic is restarted once. Needs a_isp_bss");
static int ae_fix = 1;       // see section 6
// 0 by default, and nothing is lost by that: the parameter is writable at runtime (0644) and
// the ticks never stop, so `echo 8 > /sys/module/gk7201_fixup/parameters/ae_nlog` produces the
// samples immediately, with no reboot. The counters in /dev/vpss_ctx_fix are the first-line
// indicator anyway. 8 was the right sample count while measuring, because only about one tick
// in four carries a non-zero status - fewer samples can show nothing but zeroes and mislead.
static int ae_nlog;          // how many ISP-FE status samples to log, then go quiet
module_param(ae_fix, int, 0444);
module_param(ae_nlog, int, 0644);
MODULE_PARM_DESC(ae_fix, "DEFAULT 1 (hw-proven): make ISP_ISR use the RAW ISP-FE interrupt status instead of (status & enable), so AE gets its per-frame tick while the interrupt itself stays disabled. 0 = off (exposure stays frozen). 2 = as 1 but synthesise bit 0 when the raw status reads 0 (not needed on this board; for one whose FE does not latch while masked). 3 = as 1 plus unmask the VICAP status too, which opens the sensor-register-config gate - only if 'vicap=0' in /dev/vpss_ctx_fix says that gate is shut");
MODULE_PARM_DESC(ae_nlog, "how many ISP-FE status/enable samples the AE fix logs before going quiet (default 0; writable at runtime, so 'echo 8 > /sys/module/gk7201_fixup/parameters/ae_nlog' gets them without a reboot)");
static int grp_w = 1920, grp_h = 1080, grp_pixfmt = 29;
static int maxlog = 20;
static int op3_destroy = 1;  // OP_CREATEGRP: unwind an existing group properly first
// Also off by default now, and for a specific reason rather than tidiness: it printed the same
// four slots that the zero_grpctx block below prints as it clears them, so a normal boot logged
// ten lines about four values. The clearing messages carry the values, so nothing is lost; and a
// driver that says nothing when there is nothing to report is the right default. Turn it on when
// you want the state BEFORE any clearing, e.g. together with zero_grpctx=0.
static int dump_ctx;         // log the VPSS context tables at insmod
// Off by default. It was on while the ISP dispatch was being reverse-engineered, and its one
// remaining open question - aenc/adec/ive register with func=(null) - is closed: audio works
// with those tables NULL, so they are not a bug. 20 lines of boot log for nothing. The
// machinery stays because it needs no offsets and no sysfs bases at all (see section 7).
static int dump_cmpi;        // log the vendor CMPI module registration table at insmod
// The isp (28) function table has been fully decoded - see section 6 - so the 8-line slot
// dump has done its job and is off. The one-line-per-module listing stays on: it is how we
// know aenc/adec/ive registered with a NULL function table, which is still an open question
// for audio.
static int dump_cmpi_id = -1; // dump this module's CMPI function table (28 = isp; -1 = none)
static int zero_grpctx = 1;  // clear leftover g_grp_ctx slots that have no g_dev_ctx
static int zero_h264ectx = 1;// clear leftover g_h264e_context slots that are not heap pointers
module_param(zero_h264ectx, int, 0444);
MODULE_PARM_DESC(zero_h264ectx, "DEFAULT 1: at insmod clear encoder channel-context slots below PAGE_OFFSET (h264e/h265e/jpege). Each module exit destroys EVERY non-NULL slot, so these leftovers make it tear down channels that never existed");
module_param(zero_grpctx, int, 0444);
MODULE_PARM_DESC(zero_grpctx, "DEFAULT 1: at insmod clear g_grp_ctx[i] where g_dev_ctx[i] is NULL - leftovers a vendor module init wrote, and the root cause of the EXIST (0xa0078004) errors");
module_param(op3_destroy, int, 0644);
module_param(dump_ctx, int, 0444);
module_param(dump_cmpi, int, 0444);
module_param(dump_cmpi_id, int, 0444);
MODULE_PARM_DESC(dump_cmpi_id, "CMPI module id whose function table to dump at insmod (-1 = none, the default; 28 = isp)");
MODULE_PARM_DESC(dump_cmpi, "1 = log the vendor CMPI module table (id/name/func) at insmod (default 0; read-only, shows which vendor modules registered and with what function table)");
MODULE_PARM_DESC(op3_destroy, "OP_CREATEGRP: call the vendor's VPSS_DestroyGrp on an existing group instead of just dropping its pointers (DEFAULT 1: fixes the zombie restart. Only triggers when a group already exists, i.e. on a majestic restart - never at boot)");
MODULE_PARM_DESC(dump_ctx, "1 = log g_grp_ctx[]/g_dev_ctx[] at insmod BEFORE any clearing (default 0; zero_grpctx already logs each slot it clears, so this only adds the state of slots that are left alone. Useful with zero_grpctx=0)");
module_param(a_isp_text, ulong, 0444);
module_param(a_vpss_text, ulong, 0444);
module_param(a_vpss_bss, ulong, 0444);
module_param(a_h264e_bss, ulong, 0444);
module_param(isp_int, int, 0444);
MODULE_PARM_DESC(isp_int, "0 = suppress the ISP interrupt (default, storm-safe); 1 = let it run so ISP AE/statistics work");
module_param(grp_w, int, 0444);
module_param(grp_h, int, 0444);
module_param(grp_pixfmt, int, 0444);
module_param(maxlog, int, 0444);
MODULE_PARM_DESC(a_isp_text, "xm_isp .text base (sysfs sections/.text)");
MODULE_PARM_DESC(a_vpss_text, "xm_vpss .text base");
MODULE_PARM_DESC(a_vpss_bss, "xm_vpss .bss base");
MODULE_PARM_DESC(a_h264e_bss, "xm_h264e .bss base (for zero_h264ectx)");
MODULE_PARM_DESC(grp_w, "VPSS group attr passed to the vendor's VPSS_CreateGrp on op 3");

#define MAX_GRP 8
#define ATTR_SZ 40

static int nlog;

// Fix 6's tick counters. Declared here rather than with the rest of section 6 because
// vfx_read (section 2, above it) reports them.
static unsigned long ae_ticks, ae_ticks_nz, ae_ticks_vicap;

// ======================= 0. get_chipid compat export ========================
//
// The vendor kernel exports get_chipid(); OpenIPC's does not, so xm_wdt.ko fails to load
// with "Unknown symbol get_chipid" and the board ends up with no /dev/watchdog (which is
// exactly what majestic reports). get_chipid is xm_wdt's ONLY missing symbol - everything
// else it needs (osal_*, platform_*, devm_ioremap_resource, ...) already resolves - so
// exporting this one function is enough to make the watchdog loadable.
//
// WHAT TO RETURN. xm_wdt's watchdog_init() calls get_chipid() first and compares the result
// against chip-id constants (0x72050500 and two neighbours). On a match it sets ONE extra
// field (`str r2,[r3,#4]`) and then falls into the same initialisation path as everything
// else; on no match it simply skips that store and continues - it does NOT bail out. That
// extra field belongs to a different die, so claiming a matching id would hand the watchdog
// a constant meant for other silicon. We therefore return this die's REAL id, 0x72010200
// (the same xm72010200 that xm_sysconfig is loaded with), and take the benign path.
// Override with chipid=<value> if a future measurement says otherwise.
static unsigned int chipid = 0x72010200;
module_param(chipid, uint, 0444);
MODULE_PARM_DESC(chipid, "value returned by get_chipid() for the vendor modules (default 0x72010200)");

unsigned int get_chipid(void)
{
	return chipid;
}
EXPORT_SYMBOL(get_chipid);

// ======================= 1. ISP int-enable suppressor =======================

static int isp_pre(struct kprobe *p, struct pt_regs *regs)
{
	if (regs->ARM_r1 != 0) {
		if (nlog++ < maxlog)
			pr_info("gkfix: isp: forced int enable(r1) %ld -> 0 (proc=%ld)\n",
				regs->ARM_r1, regs->ARM_r0);
		regs->ARM_r1 = 0;
	}
	return 0;
}
static struct kprobe kp_isp = { .pre_handler = isp_pre };

// ======================= 2. VPSS context builder ============================
// Vendor calls by address; OR 1 for Thumb BLX.

static void build_grp_attr(unsigned int *a)
{
	memset(a, 0, ATTR_SZ);
	a[0] = (unsigned)grp_w;
	a[1] = (unsigned)grp_h;
	a[2] = (unsigned)grp_pixfmt;
	a[4] = 0xffffffffu;   // srcFrameRate -1
	a[5] = 0xffffffffu;   // dstFrameRate -1
}

// /dev/vpss_ctx_fix (name kept so old and new shims work unchanged).
// write(): [u32 op][u32 grp][40B attr]
//   op 3 CREATEGRP - THE online fix: drop the group's two slots, then call the vendor's
//                    FULL VPSS_CreateGrp(grp, attr) so ClkEnable + SetChnEn run. Trigger
//                    AFTER SetVIVPSSMode (the shim does this automatically).
// Ops 0-2 were half-built variants (CtxInit/MkpCtxInit by hand, and a plain "null the
// slots") from before op 3 existed. They performed two of the dozen steps a real
// VPSS_CreateGrp does and left the result for op 3 to abandon; hw-proven redundant, and
// removed. The op byte is kept so the shim's write format does not change.
#define OP_CREATEGRP 3u

static ssize_t vfx_write(struct file *f, const char __user *ubuf, size_t len, loff_t *off)
{
	unsigned char buf[8 + ATTR_SZ];
	unsigned int op, grp;
	void **devctx = (void **)(a_vpss_bss + OFF_VPSS_DEVCTX);
	void **grpctx = (void **)(a_vpss_bss + OFF_VPSS_GRPCTX);

	if (len != sizeof(buf))
		return -EINVAL;
	if (copy_from_user(buf, ubuf, sizeof(buf)))
		return -EFAULT;
	op  = *(unsigned int *)buf;
	grp = *(unsigned int *)(buf + 4);
	if (grp >= MAX_GRP)
		return -EINVAL;

	if (op == OP_CREATEGRP) {
		int (*creategrp)(int, void *) =
			(int (*)(int, void *))((a_vpss_text + OFF_VPSS_CREATEGRP) | 1UL);
		unsigned int attr[ATTR_SZ / 4];
		int crc;

		// A group may ALREADY exist here, and simply nulling the two pointers (what this
		// code did originally) abandons it without ever calling CtxDeinit/MkpCtxDeinit.
		// The object stays registered in everything CtxInit/MkpCtxInit put it in
		// (g_stSynchCtx[grp] semaphores and queue heads, the region array, the picture
		// queues), so later code walks a context nobody owns. That is where the garbage we
		// spent days chasing comes from: a region whose wait field reads 0x80, a picqueue
		// pointing at an abandoned object, a VB pool id read as 0xc0c1b0c0.
		// It matters most on a majestic RESTART: after a SIGKILL there is no teardown at
		// all, so the previous contexts are still LIVE and still referenced by the
		// hardware - nulling them out from under the new instance is exactly why the
		// restarted majestic came back as a zombie.
		// So be symmetric: hand an existing group back to the vendor's own VPSS_DestroyGrp
		// and let it unwind its own state. If that fails, fall back to the old behaviour
		// rather than refuse to start - a working stream beats a clean data structure.
		// Only hand the vendor a group that is REAL. Measured at insmod on hardware:
		// g_grp_ctx[0..3] are already non-NULL before majestic ever runs -
		// g_grp_ctx[0] = 0xbf949e3c, which is BELOW PAGE_OFFSET, i.e. a pointer into
		// xm_vpss's own static data, and g_grp_ctx[1..3] are heap pointers - while
		// g_dev_ctx[] is NULL for all of them. Those are not groups; they are leftovers a
		// vendor module's init wrote. Passing one to VPSS_DestroyGrp Oopsed at boot in
		// VPSS_DestroyGrp+0xa9 -> VPSS_ReleaseOnlineVB+0x34, killing majestic before it
		// ever produced a frame. A group actually built by VPSS_CreateGrp has BOTH
		// pointers set and both are heap (>= PAGE_OFFSET), so require exactly that, and
		// otherwise fall through to dropping the pointers - which is the right thing for
		// a value that was never a context in the first place.
		if (op3_destroy &&
		    (unsigned long)grpctx[grp] >= PAGE_OFFSET &&
		    (unsigned long)devctx[grp] >= PAGE_OFFSET) {
			int (*destroygrp)(int) =
				(int (*)(int))((a_vpss_text + OFF_VPSS_DESTROYGRP) | 1UL);
			int drc = destroygrp(grp);

			pr_info("gkfix: vpss: OP_CREATEGRP grp %u: existing group (g_grp_ctx=%p "
				"g_dev_ctx=%p) -> real VPSS_DestroyGrp -> 0x%08x, now %p/%p\n",
				grp, grpctx[grp], devctx[grp], drc, grpctx[grp], devctx[grp]);
			if (drc || grpctx[grp] || devctx[grp])
				pr_warn("gkfix: vpss: DestroyGrp did not fully clear grp %u - "
					"falling back to dropping the pointers\n", grp);
		}
		grpctx[grp] = NULL;
		devctx[grp] = NULL;
		build_grp_attr(attr);
		crc = creategrp(grp, attr);
		pr_info("gkfix: vpss: OP_CREATEGRP grp %u: real VPSS_CreateGrp -> 0x%08x, g_grp_ctx=%p g_dev_ctx=%p\n",
			grp, crc, grpctx[grp], devctx[grp]);
		return (crc == 0 && grpctx[grp] && devctx[grp]) ? (ssize_t)len : -EIO;
	}
	pr_warn("gkfix: vpss: unknown op %u (only op %u CREATEGRP is supported)\n",
		op, OP_CREATEGRP);
	return -EINVAL;
}

static ssize_t vfx_read(struct file *f, char __user *ubuf, size_t len, loff_t *off)
{
	void **devctx = (void **)(a_vpss_bss + OFF_VPSS_DEVCTX);
	void **grpctx = (void **)(a_vpss_bss + OFF_VPSS_GRPCTX);
	char line[512];
	int n = 0, g;

	if (*off)
		return 0;
	for (g = 0; g < MAX_GRP; g++) {
		if (!grpctx[g] && !devctx[g])
			continue;
		n += scnprintf(line + n, sizeof(line) - n,
			       "grp%d: g_grp_ctx=%p g_dev_ctx=%p\n", g, grpctx[g], devctx[g]);
		if (n >= (int)sizeof(line) - 200)  // leave room for the ae and isp lines below
			break;
	}
	if (n == 0)
		n = scnprintf(line, sizeof(line), "all grp slots NULL\n");
	// The AE tick counters. These separate the two failure modes that look identical from
	// userspace: "our probe never fires" (ticks stays 0 - ISP_ISR is not reaching 0x172)
	// from "it fires but the status is always 0" (ticks climbs, nz stays 0 - the hardware
	// does not latch while masked, so use ae_fix=2). Read it twice a second apart.
	// vicap= is the OTHER gate: g_astIspDrvCtx+0x2e4c, which is what lets ISP_IrqRoute call
	// ISP_DRV_RegConfigSensor, i.e. what actually pushes AE's computed exposure and gain into
	// the sensor. If it stays 0 while with-status climbs, AE is running blind: it computes
	// values, nothing writes them, luminance never responds and it winds up to maximum gain.
	if (ae_fix)
		n += scnprintf(line + n, sizeof(line) - n,
			       "ae: ticks=%lu with-status=%lu vicap=%lu\n",
			       ae_ticks, ae_ticks_nz, ae_ticks_vicap);
	// LIVE values, not the insmod snapshot. g_SwitchMode must read 0 or 1 or ISP_ISR is a
	// no-op; this is the one line that tells you whether the ISP's ISR is allowed to run at
	// all, which is upstream of every AE, IQ and AWB symptom.
	if (a_isp_bss) {
		unsigned int sw = ~0u, ub = ~0u;

		probe_kernel_read(&sw, (void *)(a_isp_bss + OFF_ISP_SWITCHMODE), 4);
		probe_kernel_read(&ub, (void *)(a_isp_bss + OFF_ISP_USEBOTHALF), 4);
		n += scnprintf(line + n, sizeof(line) - n,
			       "isp: g_SwitchMode=0x%08x%s g_UseBothalf=0x%08x\n",
			       sw, (sw > 1) ? " (INVALID - ISP_ISR does nothing)" : "", ub);
	}
	if (len < (size_t)n)
		return -EINVAL;
	if (copy_to_user(ubuf, line, n))
		return -EFAULT;
	*off += n;
	return n;
}

static const struct file_operations vfx_fops = {
	.owner = THIS_MODULE,
	.write = vfx_write,
	.read  = vfx_read,
};

static struct miscdevice vfx_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "vpss_ctx_fix",   // keep the historical name - shims write here
	.fops  = &vfx_fops,
};


// ==== 5. teardown_fix: defer DestroyGrp's zeroing of g_grp_ctx (OPT-IN, default off) ====
//
// PROVEN ROOT CAUSE of the graceful-teardown crash (this is a VENDOR bug, not ours):
//   VPSS_DestroyGrp zeroes g_grp_ctx[grp] at .text+0x1720, and then at .text+0x179e calls
//   VpssCleanPicQueue(grp, idx) - which loads g_grp_ctx through its OWN literal
//   (reloc at .text+0x97a0 -> g_grp_ctx):
//       9740  ldr.w r4,[r3, r0, lsl #2]   ; r4 = g_grp_ctx[grp]   (NULL by now)
//       9744  add.w r5, r4, r1, lsl #2
//       9750  ldr.w r3,[r5, #0x574]       ; -> fault at 0x00000574
//   The observed first Oops was exactly "NULL pointer dereference at virtual address
//   00000574" with PC=.text+0x9750 and LR=.text+0x17a2. So the vendor teardown dereferences
//   a pointer it has just cleared. Nothing we do triggers it; a graceful stop always hits it.
//   (It also explains why `killall -9` is safe: no teardown runs at all.)
//
// THE FIX: keep g_grp_ctx[grp] valid for the rest of DestroyGrp and clear it only when
// DestroyGrp returns. Safe because the memory it points at is NOT freed during DestroyGrp -
// VPSS_MkpCtxDeinit is never called from there (only VPSS_DRV_CtxDeinit, which frees the
// OTHER context, g_dev_ctx).
//   - kretprobe entry on VPSS_DestroyGrp: remember grp and the current g_grp_ctx[grp]
//   - kprobe at .text+0x1724 (right after the store, where r0 still holds grp): write the
//     saved pointer back
//   - kretprobe return: set g_grp_ctx[grp] = NULL, i.e. do what the vendor meant to do
// All three sites are process context. HW-CONFIRMED 2026-07-26: with this on, a graceful
// majestic restart (and therefore saving settings in the web UI) no longer kills the board,
// so it is ON by default. Pass teardown_fix=0 to disable.
static int teardown_fix = 1;
module_param(teardown_fix, int, 0444);
MODULE_PARM_DESC(teardown_fix, "DEFAULT 1: defer VPSS_DestroyGrp's g_grp_ctx zeroing so VpssCleanPicQueue does not deref NULL (hw-confirmed fix)");

static void *dgrp_saved[MAX_GRP];
static unsigned long dgrp_fixes;

static int dgrp_entry(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	unsigned long grp = regs->ARM_r0;
	void **g = (void **)(a_vpss_bss + OFF_VPSS_GRPCTX);

	*(unsigned long *)ri->data = grp;
	if (grp < MAX_GRP)
		dgrp_saved[grp] = g[grp];
	return 0;
}

static int dgrp_ret(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	unsigned long grp = *(unsigned long *)ri->data;
	void **g = (void **)(a_vpss_bss + OFF_VPSS_GRPCTX);

	if (grp < MAX_GRP) {
		g[grp] = NULL;          // what the vendor intended, but only now
		dgrp_saved[grp] = NULL;
	}
	return 0;
}

static struct kretprobe krp_dgrp = {
	.entry_handler = dgrp_entry,
	.handler = dgrp_ret,
	.data_size = sizeof(unsigned long),
	.maxactive = 8,
};

static int dgrp_restore(struct kprobe *p, struct pt_regs *regs)
{
	unsigned long grp = regs->ARM_r0;
	void **g = (void **)(a_vpss_bss + OFF_VPSS_GRPCTX);

	if (grp < MAX_GRP && dgrp_saved[grp] && !g[grp]) {
		g[grp] = dgrp_saved[grp];
		if (dgrp_fixes++ == 0)
			pr_info("gkfix: vpss: kept g_grp_ctx[%lu]=%p alive across DestroyGrp "
				"(VpssCleanPicQueue would have dereferenced NULL)\n",
				grp, g[grp]);
	}
	return 0;
}
static struct kprobe kp_dgrp_restore = { .pre_handler = dgrp_restore };


// ============ 6. AE / exposure: give the ISP its per-frame tick back =========
//
// THE SYMPTOM. Exposure was static: majestic's /metrics showed isp_avelum 0 no matter how
// the sensor was lit, isp_exposureismax 1, isp_exptime 39881 (the 25 fps ceiling) and
// isp_again pinned at 1768 - and task_seconds{hi_isp_run} did not move between two reads.
// AE ran exactly ONCE at startup, read luminance 0, drove exposure to the maximum and then
// slept forever. (It also broke automatic night mode, which majestic drives off isp_again.)
//
// WHERE IT BREAKS. hi_isp_run blocks in ioctl(/dev/isp_dev, _IOR('I',1,4)) - confirmed via
// /proc/<tid>/syscall - and is released by ISP_DRV_WakeUpThread. Following that back through
// the vendor code (xm_isp exports nothing, so all of this is by disassembly):
//
//   irq 39 -> xm_vi handler -> VI_COMM_CapIrqRoute+0x3a -> VI_DRV_IspIsr
//          -> CMPI_GetModuleFuncById(28)[+112] == ISP_ISR
//          -> ISP_DoIntBottomHalf -> ISP_IrqRoute -> ISP_DRV_WakeUpThread -> AE runs
//
// VI_COMM_CapIrqRoute calls VI_DRV_IspIsr UNCONDITIONALLY, before any mode or validity
// check, so ISP_ISR already runs once per frame today - video works, so VI's capture
// interrupt is certainly firing. The chain is intact except for one value.
//
// ISP_ISR writes four status words into g_astIspDrvCtx[pipe]:
//     +0x2e4c = VICAP    status & enable, bit 0     (gates the sensor register config)
//     +0x2e50 = ISP-FE   status & enable            <-- this one
//     +0x2e54 = VICAP    status & enable, bits 1-2
//     +0x2e58 = vicap_ch status, bit 0              (not enable-masked)
// and ISP_IrqRoute reaches ISP_DRV_WakeUpThread (xm_isp+0x8c9e) only under
// `cmp r8,#0 / beq` where r8 = (+0x2e50 & 1). So the AE wakeup hangs off the ISP-FE status
// alone - and the ISP-FE enable mask is exactly what fix 2 forces to 0. Everything else in
// the chain still works; we severed one AND gate.
//
// THE FIX. In ISP_ISR at xm_isp+0x172 the masking is a single instruction:
//     162: ldr.w r0, [fp, #0xf0]     ; r0 = ISP-FE raw interrupt status
//     166: ldr.w fp, [fp, #0xf8]     ; fp = ISP-FE interrupt enable  (0, because of fix 2)
//     172: and.w fp, fp, r0          ; fp = status & enable          == 0
//     176: str.w fp, [r2, #0xd48]    ; -> g_astIspDrvCtx[pipe]+0x2e50
// A kprobe at 0x172 sets fp = r0, so the instruction computes r0 & r0 and the context gets
// the RAW status. The enable register is not touched: it stays 0, the ISP-FE can never
// drive the interrupt line, and the storm of fix 2 remains physically impossible. We are
// not injecting an interrupt or synthesising a frame - we let the vendor's own ISR see a
// status word it already reads two instructions earlier.
//
// WHY THE RAW STATUS IS TRUSTWORTHY HERE. ISP_DRV_SetIntEnable(pipe, 0) only clears bits in
// the enable register at +0xf8; the status register at +0xf0 is separate and write-1-to-
// clear. And ISP_ISR already clears it every frame from the UNMASKED value:
//     19a: cbz r0, 1a6  /  1a2: str.w r0, [r1, #0xf0]
// So +0xf0 is not an accumulated smear - it is freshly latched since the previous frame,
// which is precisely the per-frame tick AE needs.
//
// HW-CONFIRMED 2026-07-26. The one thing the code could not settle was whether the hardware
// latches status at all while the enable mask is clear. It does:
//     gkfix: ae: ISP-FE status=0x0000000f enable=0x00000000 -> ctx+0x2e50=0x0000000f
//     gkfix: ae: ISP-FE status=0x0000001b enable=0x00000000 -> ctx+0x2e50=0x0000001b
// With a stream running: isp_avelum went from a permanent 0 to tracking the light (1 in the
// dark to 111 under a lamp), and the full AE control range came back -
//     dark   : isp_again 32768 (max, 32x), isp_exptime 124948 (125 ms slow shutter)
//     bright : isp_again 1024  (1x),       isp_exptime 30     (30 us)
// where before the fix all three numbers were frozen for ever at 1768 / 39881 / 0.
// task_seconds{hi_isp_run} climbs instead of sitting still. isp_again lags isp_avelum by a
// second or two, which is just AE moving its slower axis after exposure - normal behaviour.
// The best evidence that this ticks the RIGHT clock rather than just some clock: with-status
// is consistently one quarter of ticks, and it stays that ratio across every measurement.
// ae_fix=2 (synthesise bit 0) therefore turned out to be unnecessary. It is kept because it
// costs three lines and it is the fallback if another board's FE does not latch while masked.
//
// ONE THING THIS FIX DOES NOT DO, and it is not an AE bug: after a cold boot the counters read
// ticks=0 and AE stays at its startup guess until majestic is restarted once. ticks=0 means
// ISP_ISR is not being called at all, i.e. VI's capture interrupt is not firing - so this is
// upstream of everything here, in how the pipeline is first brought up. It is tracked
// separately. The counters in /dev/vpss_ctx_fix are the cheap indicator for it: ticks=0 with a
// stream open means no VI capture interrupt, whatever the picture looks like.
//
// SCOPE - AND A CORRECTION. This fix as written opens only the ISP-FE gate. The first version
// of this comment claimed the other two words were "already correct because +0x2e4c/+0x2e54
// come from VICAP, whose interrupts VI enables and which we never touch". THAT IS WRONG, and
// the vendor code says so: ISP_DRV_SetIntEnable writes through TWO register bases,
// reg_ispfe_base_va AND reg_vicap_base_va (literals at xm_isp+0x1354 and +0x1350), and its
// enable==0 path at 0x12ea clears VICAP enable bit 0 (conditionally, on the pipe-mode byte at
// ctx+0x100) and bits 1-2 (unconditionally) as well as the FE bits. The VICAP block is SHARED:
// VI has its own enable bits in it, and the ISP has these. So fix 2 shuts the sensor-config
// gate too, by the same mechanism.
// MEASURED, AND IT SETTLES IT THE OTHER WAY: the vicap counter reads 0 on this board - that
// gate really is shut - and AE nevertheless works over its full range with ae_fix=1. Dark:
// again 32768, exptime 124948. Bright: again 1024, exptime 30. So opening the FE gate alone
// is sufficient, and my expectation that ae_fix=3 would be "the necessary second half" was
// wrong. ae_fix=3 works too (vicap then tracks with-status exactly, and AE reaches the same
// range) but it buys nothing, so the default stays at 1 - which is also the more conservative
// choice, since it leaves the VICAP block, shared with VI, completely alone.
// ae_fix=3 is kept as a documented fallback for a board where the FE gate is not enough.
static int ae_armed;   // ae_ticks/ae_ticks_nz are declared above vfx_read, which reports them

static int ae_fe_pre(struct kprobe *p, struct pt_regs *regs)
{
	unsigned long raw = regs->ARM_r0;   // ISP-FE raw interrupt status
	unsigned long en  = regs->ARM_fp;   // ISP-FE interrupt enable (0 while fix 2 is armed)
	// r1 still holds the VICAP masked status computed at 0x14a - it is stored to ctx+0x2e4c
	// twelve bytes further on. Free to observe here, which is why the vicap counter needs no
	// probe of its own.
	unsigned long vicap = regs->ARM_r1;

	ae_ticks++;
	if (raw)
		ae_ticks_nz++;
	if (vicap)
		ae_ticks_vicap++;
	if (ae_nlog > 0) {
		ae_nlog--;
		pr_info("gkfix: ae: ISP-FE status=0x%08lx enable=0x%08lx -> ctx+0x2e50=0x%08lx, "
			"VICAP masked=0x%08lx (tick %lu)\n", raw, en, raw, vicap, ae_ticks);
	}
	// Escape hatch for the one thing the disassembly could not settle: if the hardware does
	// not latch status while the enable mask is clear, there is nothing to copy. Bit 0 is
	// what ISP_IrqRoute tests, and r0 has to be set too because the probed instruction ANDs
	// with it. This fabricates a frame tick, so it is opt-in, not the default.
	if (ae_fix == 2 && !raw) {
		regs->ARM_r0 = 1;
		regs->ARM_fp = 1;
		return 0;
	}
	regs->ARM_fp = raw;
	return 0;
}
static struct kprobe kp_ae_fe = { .pre_handler = ae_fe_pre };

// ae_fix=3 only. The second gate: ctx+0x2e4c comes from the VICAP interrupt status, masked
// the same way at 0x14a (`ands r1, r0`, r1 = enable, r0 = raw status), and it is what lets
// ISP_IrqRoute reach ISP_DRV_RegConfigSensor - the call that pushes AE's computed exposure and
// gain into the sensor. VI enables its own capture interrupts, so this one is EXPECTED to work
// already; the vicap counter says whether it does. Only if that counter stays 0 is this probe
// the answer, which is why it is opt-in and not part of the hw-proven default.
static int ae_cap_pre(struct kprobe *p, struct pt_regs *regs)
{
	regs->ARM_r1 = regs->ARM_r0;   // use the raw VICAP status, not (status & enable)
	return 0;
}
static struct kprobe kp_ae_cap = { .pre_handler = ae_cap_pre };
static int ae_cap_armed;


// ======================= init / exit ========================================

static int arm_kp(struct kprobe *k, unsigned long addr, const char *what)
{
	int r;

	k->symbol_name = NULL;   // by ADDRESS - name walk Oopses this kernel
	k->addr = (kprobe_opcode_t *)addr;
	r = register_kprobe(k);
	if (r)
		pr_err("gkfix: kprobe %s @0x%lx failed: %d\n", what, addr, r);
	else
		pr_info("gkfix: %s armed @0x%lx\n", what, addr);
	return r;
}

// Every encoder module keeps a 16-entry table of channel-context pointers in .bss and its
// module-exit walks the whole table, destroying every non-NULL slot. Measured on this SoC the
// tables are NOT clean at module-load time: xm_h264e's slot 0 held 0xbf9c34b0, a pointer into
// the module's own static data. Tearing down such a "channel" reads string/data bytes as
// struct fields, which is where VB_DestroyPool got 0x636e6576 ("venc") and where
// h264e_destroy_chn went on to flush a cache range at NULL and kfree junk.
// CLEAR EVERY NON-NULL SLOT, not just the obviously-bogus ones. gk7201_fixup is insmod'ed at
// the END of load_xm, before majestic starts, and nothing in the vendor module load sequence
// creates an encoder channel - majestic does. So at this moment NO slot can legitimately hold
// a context, whatever it looks like.
// An earlier version of this only cleared pointers below PAGE_OFFSET, on the theory that a
// real context is kmalloc'd and therefore in the heap. That was wrong and the hardware said
// so: slots 1..3 held heap pointers (c10d8400, c1378f40, c13fa840) which teardown then
// happily destroyed, and the registers at the resulting Oops decoded to ASCII - 0x656a616d
// = "maje" (majestic), 0x7373622e = ".bss", 0x7361696c = "lias" (modalias). They were freed
// slab pages recycled for module-loading strings, i.e. leftovers just like the module-data
// pointer, only harder to recognise. Address range is not evidence of ownership; TIMING is.
static void clear_leftover_ctx(const char *who, unsigned long bss, unsigned long off, int n)
{
	void **t = (void **)(bss + off);
	int i, cleared = 0;

	if (!bss)
		return;
	for (i = 0; i < n; i++) {
		if (!t[i])
			continue;
		pr_info("gkfix: %s: clearing leftover ctx[%d]=%p\n", who, i, t[i]);
		t[i] = NULL;
		cleared++;
	}
	if (cleared)
		pr_info("gkfix: %s: cleared %d leftover chn ctx slot(s) - module exit will no "
			"longer tear down channels that never existed\n", who, cleared);
}

// ============ 7. CMPI module-table dump - DIAGNOSTIC, read-only ==============
//
// The vendor modules do not call each other through exported symbols; xm_isp exports NOTHING
// (its __ksymtab is empty). They register a function table under a numeric module ID with
// xm_base's CMPI_RegisterModule, and other modules fetch it with CMPI_GetModuleFuncById.
// That is how xm_vi's interrupt handler is meant to reach the ISP: look up the ISP module by
// ID and call through its table, which ends in an osal_wakeup of the queue that hi_isp_run
// blocks on (measured: hi_isp_run sits in ioctl(/dev/isp_dev, _IOR('I',1,4)) and only ever
// returns via the wait's timeout, which is why isp_avelum stays 0 and AE never updates).
//
// So the question this dump answers is: is the ISP module in that table at all, under which
// ID, and with what function table? A missing entry, a different ID than xm_vi asks for, or a
// NULL table are three different bugs with three different fixes, and guessing between them
// has already cost this port enough.
//
// UNLIKE EVERYTHING ELSE IN THIS FILE, this needs no hardcoded offsets and no sysfs bases:
// xm_base really does EXPORT_SYMBOL these, so we link against them by name and the kernel
// resolves them at insmod. (Verify with: objdump -s -j __ksymtab_strings xm_base.ko - plain
// nm shows nothing because the local ksymtab symbols are stripped.)
//
// Both are pure walks of a doubly-linked list with no side effects - verified by
// disassembling them before calling them:
//     CMPI_GetModuleName(id)      -> &entry[8]   (inline 16-byte name) or NULL
//     CMPI_GetModuleFuncById(id)  -> entry[0x34] (the function table)  or NULL
// Do NOT be tempted to use CMPI_QueryModules for this. Despite the name it is not an
// enumerator: it CALLS a callback in every registered module and checks the reply. That is a
// side effect, and diagnostics must not have any.
extern void *CMPI_GetModuleFuncById(int id);
extern char *CMPI_GetModuleName(int id);

// Dump one module's function table. The consumers call it by byte offset - xm_vi alone has
// 40 lookups of id 28 (isp) and reaches at least +124, so the table is >= 32 entries, and
// +100 is by far the hottest slot (174 call sites across all its lookups). A NULL at an
// offset a caller actually uses is the bug signature we are looking for: the lookup succeeds,
// the caller loads the slot, and calls nothing.
// probe_kernel_read, not a plain deref: we do not know the real length of the table, and a
// diagnostic must never be the thing that panics the board.
static void dump_cmpi_functab(int id)
{
	void *tab = CMPI_GetModuleFuncById(id);
	char *name = CMPI_GetModuleName(id);
	int i, nulls = 0, shown = 0;

	if (!tab) {
		pr_info("gkfix: cmpi: id %d ('%.16s') has no function table to dump\n",
			id, name ? name : "(none)");
		return;
	}
	for (i = 0; i < 32; i += 4) {
		unsigned long w[4];
		int k, ok = 1;

		for (k = 0; k < 4; k++) {
			if (probe_kernel_read(&w[k], (char *)tab + (i + k) * 4,
					      sizeof(w[k]))) {
				ok = 0;
				break;
			}
		}
		if (!ok)
			break;
		pr_info("gkfix: cmpi: %.16s tab+%3d: %08lx %08lx %08lx %08lx\n",
			name ? name : "?", i * 4, w[0], w[1], w[2], w[3]);
		for (k = 0; k < 4; k++)
			if (!w[k])
				nulls++;
		shown += 4;
	}
	pr_info("gkfix: cmpi: id %d ('%.16s') table at %p: %d of %d slots are NULL\n",
		id, name ? name : "?", tab, nulls, shown);
}

static void dump_cmpi_table(void)
{
	int id, found = 0;

	// Look up by ID rather than walking the list ourselves: the list head is a module-local
	// literal we would have to hardcode, and the whole point here is to need no offsets.
	// Vendor MPP module IDs are small; 64 covers them with room to spare.
	for (id = 0; id < 64; id++) {
		char *name = CMPI_GetModuleName(id);
		void *fn = CMPI_GetModuleFuncById(id);

		if (!name && !fn)
			continue;
		found++;
		// %.16s: the name is an inline buffer in the entry, not guaranteed NUL-terminated.
		pr_info("gkfix: cmpi: id %2d name '%.16s' func=%p\n",
			id, name ? name : "(none)", fn);
	}
	pr_info("gkfix: cmpi: %d module(s) registered\n", found);
	if (dump_cmpi_id >= 0)
		dump_cmpi_functab(dump_cmpi_id);
}

static int __init gk_init(void)
{
	int rc;

	if (!a_isp_text || !a_vpss_text || !a_vpss_bss || !a_h264e_bss) {
		pr_err("gkfix: need all 4 bases from sysfs sections (xm_isp/.text, xm_vpss/.text+.bss, xm_h264e/.bss)\n");
		return -EINVAL;
	}
	// State of the VPSS context tables BEFORE we touch anything. This is what settled the
	// question the whole port hinged on: something during load_xm DOES leave g_grp_ctx[0..3]
	// non-NULL, which is why VPSS_CreateGrp returned EXIST (0xa0078004) on its own. Kept on
	// by default - it is two lines of boot log, and it is the evidence for everything below.
	// Both tables are in xm_vpss .bss: g_grp_ctx at +0 (8 entries), g_dev_ctx at +0x6420.
	if (dump_cmpi)
		dump_cmpi_table();
	if (dump_ctx) {
		void **g = (void **)(a_vpss_bss + OFF_VPSS_GRPCTX);
		void **d = (void **)(a_vpss_bss + OFF_VPSS_DEVCTX);
		int i;

		for (i = 0; i < MAX_GRP; i++)
			if (g[i] || d[i])
				pr_info("gkfix: vpss: at insmod grp %d: g_grp_ctx=%p g_dev_ctx=%p\n",
					i, g[i], d[i]);
		pr_info("gkfix: vpss: context tables dumped (only non-empty slots listed)\n");
	}
	// MEASURED: g_grp_ctx[0..3] are non-NULL before majestic runs, with g_dev_ctx[] NULL
	// throughout - g_grp_ctx[0] even points into xm_vpss's static data (0xbf949e3c, below
	// PAGE_OFFSET). A group is only real if VPSS_CreateGrp built it, and that sets BOTH
	// slots, so a set grp ctx with a NULL dev ctx is a leftover, not a context.
	// This is the ROOT of the EXIST errors we worked around in three separate places:
	// VPSS_CreateGrp+0x42 does `ldr r3,[g_grp_ctx + grp*4]` and returns 0xa0078004 whenever
	// that slot is non-zero. Clearing the leftovers here removes the cause instead of
	// guarding each victim, and it also stops a teardown that walks all 8 groups from
	// handing one of these to VPSS_DestroyGrp (which Oopses in VPSS_ReleaseOnlineVB).
	if (zero_grpctx) {
		void **g = (void **)(a_vpss_bss + OFF_VPSS_GRPCTX);
		void **d = (void **)(a_vpss_bss + OFF_VPSS_DEVCTX);
		int i, n = 0;

		for (i = 0; i < MAX_GRP; i++) {
			if (g[i] && !d[i]) {
				pr_info("gkfix: vpss: clearing leftover g_grp_ctx[%d]=%p "
					"(g_dev_ctx NULL, so it was never a real group)\n", i, g[i]);
				g[i] = NULL;
				n++;
			}
		}
		if (n)
			pr_info("gkfix: vpss: cleared %d leftover grp ctx slot(s) - "
				"VPSS_CreateGrp will no longer return EXIST for them\n", n);
	}
	if (zero_h264ectx) {
		clear_leftover_ctx("h264e", a_h264e_bss, OFF_H264E_CTXTAB, 16);
		clear_leftover_ctx("h265e", a_h265e_bss, OFF_H265E_CTXTAB, 16);
		clear_leftover_ctx("jpege", a_jpege_bss, OFF_JPEGE_CTXTAB, 16);
		// Same shape a third time, and this one is a table of 128 OSD regions.
		// RgnExit (xm_rgn+0x33e4) walks g_apstRgnCtrl and skips only NULL:
		//     33f0 ldr.w r0,[r4,#4]!   / 33f4 cbz r0,3426
		//     33fa ldr   r3,[r4]       ; the region
		//     33fc ldr.w r1,[r3,#0x134]; a list head inside it
		//     3400 ldr   r5,[r1]       ; <-- faulted here, r3 being exactly the region
		//                                   a now-removed guard had just skipped in RgnNotify
		// Regions are created by majestic, so at insmod none can exist. Clearing the table
		// removes the bogus regions themselves instead of guarding each place that touches
		// them - the old guard only covered RgnNotify's wake, and RgnExit is a second user.
		clear_leftover_ctx("rgn", a_rgn_bss, OFF_RGN_CTRLTAB, 128);
	}
	// The ISP interrupt shares the hardware line with VI_CAP0 (both are SPI 43 in the DT;
	// Linux irq 39). Historically enabling it stormed ("nobody cared") and took VI capture
	// down with it - but that was measured while the VPSS contexts were still broken. With
	// isp_int=1 the suppressor is not armed, so the ISP interrupt runs and ISP AE/statistics
	// can update (exposure is frozen without it). Keep the default at 0 until a run proves
	// the storm is gone.
	// ==== 6b. g_SwitchMode: why a cold boot had no AE, no IQ and the wrong colours ====
	//
	// g_SwitchMode selects which half of ISP_ISR runs, and it must hold 0 or 1:
	//     10c: ldr r3,[g_SwitchMode] / 112: cmp r3,#0 / 114: bne 1d8
	//     1d8: cmp r3,#1             / 1da: bne 11a  -> returns 2, having done NOTHING
	// Anything else and ISP_ISR is a no-op on every interrupt. Measured at insmod on this SoC:
	// 0xBF90CF5C - a pointer into module static data, the same leftover-.bss signature as
	// g_grp_ctx[0] in fix 1. So on a cold boot the ISP's ISR does nothing at all, and since
	// hi_isp_run is the thread that applies AE, IQ, CCM and AWB, all of those die together:
	// exposure frozen at its startup guess, and a black/blue picture instead of the vendor's
	// colours. Video keeps flowing because VI->ISP->VPSS->VENC is hardware-linked in
	// VI_ONLINE_VPSS_ONLINE and VENC has its own interrupt. Restarting majestic once fixed all
	// three symptoms - the stopping instance evidently leaves the flag at a sane 0.
	//
	// MEASUREMENT THAT PROVED IT WAS NOT SOMETHING ELSE: on a cold boot /proc/interrupts shows
	// VI_CAP0 (irq 39) registered AND its count climbing, while our AE tick counter stays at 0.
	// So the interrupt fires and the handler runs; the chain dies inside ISP_ISR, at the very
	// first branch. That rules out request_irq and the interrupt mask.
	//
	// A NOTE ON HOW THIS WAS NEARLY MISSED, because it is the kind of mistake worth recording:
	// this exact value was logged from the start and I dismissed it, arguing that majestic must
	// initialise the flag because fix 6's probe - which sits on the g_SwitchMode==0 path - does
	// fire. The probe does fire, but every measurement that showed it firing had been taken
	// AFTER a majestic restart. The inference was sound and the premise was not.
	//
	// WHY WRITING 0 IS SAFE: 0 is what the working state holds (fix 6's probe only runs on the
	// ==0 path, so the post-restart system that works has g_SwitchMode == 0), it is what C
	// guarantees for a .bss scalar, and we run after xm_isp's init but before majestic. Only a
	// value that is neither 0 nor 1 is touched, so a deliberate 1 is left alone.
	// g_UseBothalf is deliberately NOT written: it is also garbage here, but its correct
	// working value has not been measured, and it only chooses WHERE the bottom half runs.
	// The read side of /dev/vpss_ctx_fix reports both live values.
	if (a_isp_bss) {
		unsigned int sw = ~0u, ub = ~0u;

		probe_kernel_read(&sw, (void *)(a_isp_bss + OFF_ISP_SWITCHMODE), 4);
		probe_kernel_read(&ub, (void *)(a_isp_bss + OFF_ISP_USEBOTHALF), 4);
		pr_info("gkfix: isp: at insmod g_SwitchMode=0x%08x g_UseBothalf=0x%08x\n", sw, ub);
		if (zero_ispsw && sw > 1) {
			unsigned int zero = 0;

			probe_kernel_write((void *)(a_isp_bss + OFF_ISP_SWITCHMODE), &zero, 4);
			probe_kernel_read(&sw, (void *)(a_isp_bss + OFF_ISP_SWITCHMODE), 4);
			pr_info("gkfix: isp: g_SwitchMode was neither 0 nor 1 - ISP_ISR would have "
				"returned without doing anything on every interrupt, so AE, IQ and "
				"AWB would all have been dead until a majestic restart. Set to "
				"0x%08x\n", sw);
		}
	} else if (zero_ispsw) {
		pr_warn("gkfix: isp: no a_isp_bss - cannot check g_SwitchMode; if a cold boot has "
			"frozen exposure and blue-ish colours, that is why\n");
	}
	if (!isp_int) {
		rc = arm_kp(&kp_isp, a_isp_text + OFF_ISP_SETINT, "isp int-suppress");
		if (rc)
			return rc;
	} else {
		pr_info("gkfix: isp int-suppress NOT armed (isp_int=1) - ISP interrupt will run; "
			"watch for an irq39 storm\n");
	}
	// Fix 6. Armed after the suppressor so a failure here cannot leave the interrupt
	// enabled: if this probe does not register we simply keep the old static exposure,
	// which is a working camera with a bad picture rather than no camera.
	if (ae_fix) {
		if (arm_kp(&kp_ae_fe, a_isp_text + OFF_ISP_ISR_FEMASK, "ae ISP-FE raw status")) {
			pr_warn("gkfix: ae: probe failed - exposure will stay static (video is "
				"unaffected)\n");
		} else {
			ae_armed = 1;
			pr_info("gkfix: ae: ISP_ISR will use the raw ISP-FE status%s; read "
				"/dev/vpss_ctx_fix for the tick counters\n",
				ae_fix == 2 ? " (ae_fix=2: bit 0 synthesised when it reads 0)" : "");
		}
	}
	if (ae_fix == 3 && ae_armed) {
		if (arm_kp(&kp_ae_cap, a_isp_text + OFF_ISP_ISR_CAPMASK, "ae VICAP raw status"))
			pr_warn("gkfix: ae: VICAP probe failed - only the ISP-FE gate is open\n");
		else
			ae_cap_armed = 1;
	}
	rc = misc_register(&vfx_dev);
	if (rc) {
		pr_err("gkfix: misc_register failed: %d\n", rc);
		goto out_ae;
	}
	if (teardown_fix) {
		krp_dgrp.kp.symbol_name = NULL;
		krp_dgrp.kp.addr = (kprobe_opcode_t *)(a_vpss_text + OFF_VPSS_DESTROYGRP);
		if (register_kretprobe(&krp_dgrp))
			pr_err("gkfix: teardown_fix: kretprobe VPSS_DestroyGrp failed\n");
		else if (arm_kp(&kp_dgrp_restore, a_vpss_text + OFF_VPSS_DGRP_AFTER,
				"vpss DestroyGrp g_grp_ctx keep-alive"))
			unregister_kretprobe(&krp_dgrp);
		else
			pr_info("gkfix: teardown_fix active - graceful majestic stop should survive\n");
	}
	pr_info("gkfix: all fixups active (run mdev -s for /dev/vpss_ctx_fix)\n");
	return 0;

out_ae:
	if (ae_cap_armed)
		unregister_kprobe(&kp_ae_cap);
	if (ae_armed)
		unregister_kprobe(&kp_ae_fe);
	if (!isp_int)
		unregister_kprobe(&kp_isp);
	return rc;
}

static void __exit gk_exit(void)
{
	misc_deregister(&vfx_dev);
	if (ae_cap_armed)
		unregister_kprobe(&kp_ae_cap);
	if (ae_armed)
		unregister_kprobe(&kp_ae_fe);
	if (!isp_int)
		unregister_kprobe(&kp_isp);
	pr_info("gkfix: unloaded after %d logged events, %lu ae ticks (%lu with status)\n",
		nlog, ae_ticks, ae_ticks_nz);
}

module_init(gk_init);
module_exit(gk_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GK7201V200 vendor-module fixups: leftover .bss object tables, ISP int storm, VPSS online grp build, teardown Oops, AE/exposure tick");
