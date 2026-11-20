# axera-osdrv-ax620u

AX620U is a distinct chip from AX620E/AX630C/AX620Q despite the shared
family name -- confirmed from its own real firmware's devicetree
(root `compatible = "axera,ax620x"`, not `"axera,ax620e"`), four
Cortex-A7 cores (AArch32), and its own vendor spin-table SMP method
(`axera,ax620x-smp`, a different `secondary-boot-reg` than AX620Q's).

**Unlike AX620E, there is no existing open-source starting point at
all here.** The AX620E kernel branch backports an existing community
port that targets the AArch64 AX630C/AX620E family specifically; none
of its clock, pinctrl or crypto drivers apply to AX620U's
`axera,ax620x` platform code, and AX620U never appears in the public
AXERA-TECH SDKs either (those cover AX620E/AX630C/AX620Q only).
Bringing up even the open-able parts (UART, GIC wiring, clocks,
pinctrl) needs new driver work against the real devicetree extracted
from a deployed AX620U camera firmware (see the `axera-ax620u` kernel
branch), not a backport.

`files/kmod/` ships Axera's closed binaries at kernel 4.19.125's
vermagic (ARMv7), pulled from that same firmware: `ax_npu`, `ax_proton`
(ISP), `ax_sys`, `ax_pool`, `ax_cmm`, `ax_osal`, `ax_dma_hal`,
`ax_hwinfo`, `ax_rtc`, `ax_ddr_retention`, `ax_pcal6416a`,
`ax_sdio_host`, `ax_logctl`, `ax_mipi`, `ax_gdc`, `ax_ivps`, `ax_ives`,
`ax_jenc`, `ax_venc`, `ax_vdec`, `ax_vo`, `ax_avs`, `ax_audio`, plus the
same Xiongmai-style peripheral modules (`XmIrCut`, `XmGpio`,
`XmGpioI2c`, `XmDemux`, `XmCommon`) as AX620E's firmware. Same
provenance as [axera-osdrv-ax620e](../axera-osdrv-ax620e/README.md):
no public source exists for the closed modules at any kernel version.

## Provenance and kernel binding

- Origin: `/usr/lib/modules/` of the stock firmware of a JFtech
  IPC_AX620U_A8 camera, copied unmodified.
- Kernel: every module carries `vermagic=4.19.125 SMP preempt mod_unload
  ARMv7 p2v8` and loads only into a kernel built from OpenIPC/linux
  `axera-ax620u` with `ax620u.lite.config` (release exactly `4.19.125`,
  no modversions). A kernel update that changes that needs new binaries.
- Licence: the modules declare `GPL`, `GPL v2` or `Dual BSD/GPL` in their
  own `modinfo`; their source is not published.
- Same model as the other vendors' binary driver packages here
  (`hisilicon-osdrv-*`, `goke-osdrv-*`, `sigmastar-osdrv-*`,
  `novatek-osdrv-nt9856x`, `xiongmai-osdrv-xm530v200`).

## Loading

Installed under `/lib/modules/4.19.125/axera` and indexed by Buildroot's
`depmod`; `load_axera` (run by `S70vendor`) loads them with `modprobe` in
the stock order, `ax_cmm` with its `cmmpool=` before anything that
depends on it. A failed load is logged and makes `load_axera` exit
non-zero.

## Sensor tuning

`files/sensor/` carries the image tuning the stock firmware runs the
camera's OS04A10 with, copied unmodified from the same JFtech
IPC_AX620U_A8 firmware. Any streamer driving Axera's ISP needs it:
without a bin `AX_ISP_LoadBinParams` leaves the ISP on the sensor
driver's built-in defaults.

| Package file | Installed at | Stock path | Size |
| --- | --- | --- | --- |
| `configs/os04a10_{sdr,deepdark}{,_ONE,_TWO,_THREE}.bin` | `/etc/sensors/` | `/mnt/custom/isp_bin/` | 976 KB each |
| `lib/libsns_os04a10.so` | `/usr/lib/sensors/` | `/usr/lib/` | 951 KB |
| `ai_isp/OS04A10_*_AX620A.pro` (3 models) | `/etc/sensors/ai_isp/` | `/mnt/custom/ai_isp/` | 880 KB in all |

- The bins are the `AX_ISP_LoadBinParams` parameter sets the stock App
  picks per scene: `_sdr` linear (the default), `_deepdark` low light,
  each with its `_ONE`/`_TWO`/`_THREE` scene variants.
- `libsns_os04a10.so` is the sensor driver the streamer `dlopen`s for
  its `gSnsos04a10Obj` and registers with the ISP; the copy in the
  firmware's `/usr` partition, which the stock App and the modules above
  run with (the other `usr` image found for this camera carries a
  byte-identical copy). Linked against glibc (`libc.so.6`), as Axera
  ships it. Buildroot's strip pass drops its `.comment` section on the
  way into the image; its code and data are byte-identical to the stock
  copy.
- The `.pro` files are the AI-ISP (NPU denoise) models. The bins and
  `libsns_os04a10.so` name them by their absolute stock path
  `/mnt/custom/ai_isp/<name>`, so `/mnt/custom/ai_isp` is a link to
  `/etc/sensors/ai_isp`. The NAND image has room for them.
- Left out of the same partitions as not tuning: the App's encrypted
  `jsons/` and `CustomConfig/`, fonts and strings, and the `res/ax_ai/`
  detection models.
- `BR2_OPENIPC_SNS_MODEL`, if a board sets it, limits the bins and the
  sensor library to that sensor.
