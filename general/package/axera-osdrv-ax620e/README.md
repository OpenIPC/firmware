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

## Sensor tuning

`files/sensor/` carries the image tuning the stock firmware runs the
camera's SC450AI with, copied unmodified from the same Xiongmai IVG-A6
firmware. Any streamer driving Axera's ISP needs it: without a bin
`AX_ISP_LoadBinParams` leaves the ISP on the sensor driver's built-in
defaults (the stock App logs "The parameters in sensor.h will be used").

| Package file | Installed at | Stock path | Size |
| --- | --- | --- | --- |
| `configs/sc450ai_{sdr,sdr_ONE,sdr_TWO,sdr_THREE,deepdark,hightemp}.bin` | `/etc/sensors/` | `/mnt/custom/isp_bin/` | 657 KB each |
| `lib/libsns_sc450ai.so` | `/usr/lib/sensors/` | `/usr/lib/` | 1.3 MB |
| `ai_isp/SC450AI_SDR_2688x1520_..._240221_AX620E.axmodel` | `/etc/sensors/ai_isp/`, only with `BR2_PACKAGE_AXERA_OSDRV_AX620E_AI_ISP` | `/mnt/custom/ai_isp/` | 4.3 MB |

- The bins are the `AX_ISP_LoadBinParams` parameter sets the stock App
  picks per scene: `_sdr` linear (the default), `_ONE`/`_TWO`/`_THREE`
  its scene variants, `_deepdark` low light, `_hightemp` the
  high-temperature gain limit. They compress to about 12 KB each in the
  squashfs.
- `libsns_sc450ai.so` is the sensor driver the streamer `dlopen`s for
  its `gSnssc450aiObj` and registers with the ISP. Like the rest of the
  stock userspace it is linked against glibc (`libc.so.6`), as Axera
  ships it. Buildroot's strip pass drops its `.comment` section on the
  way into the image; its code and data are byte-identical to the stock
  copy.
- The AI-ISP model is the NPU denoise network. The bins and
  `libsns_sc450ai.so` name models by their absolute stock path
  `/mnt/custom/ai_isp/<name>`, so the option also links
  `/mnt/custom/ai_isp` to `/etc/sensors/ai_isp`. It is off by default:
  about 1.3 MB in the xz squashfs, it takes the lite rootfs from 2948 KB
  to 4460 KB, leaving the 5 MB rootfs slot of the 16 MB NOR image too
  little room for a streamer. The bins and the sensor library add about
  110 KB. Without it a streamer has to leave AI-ISP off. (The scene bins and
  the sensor library name this `MULTI4` model; `sc450ai_sdr.bin` alone
  names an older `..._MULTI3_..._231124` one that the stock firmware
  does not ship.)
- Left out of the same partitions as not tuning: the Xiongmai App's
  encrypted `jsons/` and `CustomConfig/`, fonts and strings, and the
  `res/ax_ai/` face/body detection models.
- `BR2_OPENIPC_SNS_MODEL`, if a board sets it, limits the bins and the
  sensor library to that sensor, as in `sigmastar-osdrv-infinity6e`.
