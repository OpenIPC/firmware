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
