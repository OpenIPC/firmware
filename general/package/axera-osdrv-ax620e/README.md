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

`load_axera` (in `files/script/`) insmods them in the correct
dependency order, read from each module's own `depends=` via `modinfo`.
