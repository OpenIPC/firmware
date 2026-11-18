# axera-osdrv-ax620e

## What builds from source

VIN/CSI2 capture, the VC8000E encoder, the Hantro decoder path and
EIP-130 crypto are backported into the `axera-ax620e` kernel branch, at
Linux 5.15.73 -- the exact kernel version a real deployed Xiongmai
IVG-A6 (AX620E) camera firmware's own modules were built against,
confirmed via each module's `vermagic=` string. They build and install
as ordinary in-tree kernel drivers; this package does not carry them.

## What this package carries

`files/kmod/` ships Axera's closed binaries at that same 5.15.73
vermagic, pulled from the same firmware: `ax_sys`, `ax_pool`, `ax_cmm`,
`ax_base` (the foundation every other module depends on), `ax_proton`
(ISP 3A), `ax_npu` (NPU), `ax_venc`, `ax_vdec`, `ax_jenc`, `ax_ivps`,
`ax_mipi_rx`, `ax_tdp`, `ax_gdc`, `ax_vpp`, `ax_vo`, `ax_audio`, plus
Xiongmai's own peripheral modules from the same firmware (`XmIrCut`,
`XmGpio`, `XmGpioI2c`, `XmDemux`, `XmCommon`). No public source exists
for the Axera modules at any kernel version -- checked directly
against the vendor SDK's `osdrv` driver trees (a `Makefile` and zero
`.c` files per subsystem) and independently confirmed by a third-party
audit in `sipeed/maix_ax620e_sdk` issue #2, which found the same gap in
AXERA-TECH's newest public release.

## Provenance and kernel binding

- Origin: the vendor modules of a deployed Xiongmai IVG-A6 (AX620E)
  camera firmware, `/usr/lib/modules/`, copied unmodified. They are
  Axera's binary-only media stack as Axera ships it to camera makers.
- Kernel: every module carries `vermagic=5.15.73 SMP preempt mod_unload
  aarch64`, and loads only into a kernel built from OpenIPC/linux
  `axera-ax620e` with `ax620e.lite.config` (release exactly `5.15.73`, no
  modversions, KPROBES on so `struct module` matches). A kernel update
  that changes any of that needs a new set of binaries from Axera.
- Licence: the modules declare `GPL`, `GPL v2` or `Dual BSD/GPL` in their
  own `modinfo`; their source is not published.
- Same model as the other vendors' binary driver packages in this tree,
  e.g. `hisilicon-osdrv-hi3516ev200`, `goke-osdrv-gk7205v200`,
  `sigmastar-osdrv-infinity6e`, `novatek-osdrv-nt9856x`,
  `xiongmai-osdrv-xm530v200`: the SoC vendor's media modules, at the
  kernel version they were built for, are what the camera's media
  pipeline runs on; the kernel itself builds from source.

## Loading

The package installs the modules under `/lib/modules/5.15.73/axera`,
where Buildroot's target-finalize `depmod` indexes them. `load_axera`
(in `files/script/`, run by `S70vendor`) loads them with `modprobe` in
the stock loader's order, which matches every module's `depends=`;
`ax_cmm` goes first of the Axera modules with its `cmmpool=`, so nothing
pulls it in without one. A failed load is logged and makes `load_axera`
exit non-zero.
