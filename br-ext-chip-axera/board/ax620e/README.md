# AX620E / AX630C

Kernel: OpenIPC/linux `axera-ax620e`, Linux 5.15.73 -- the version the
closed Axera media modules of a real Xiongmai IVG-A6 (AX620E) camera were
built against (their `vermagic=`). The branch carries the Axera SDK
drivers ported to 5.15 that the camera and those modules need: clocks,
pinctrl, GPIO, resets, the vendor SPI NOR controller driver + AX_DMAPER,
EQOS Ethernet + internal EPHY, UART, watchdog (also the reboot path), the
built-in OSAL (`AX_OSAL_*`) with `/proc/ax_proc`, hwinfo, os_mem,
ax_hrtimer, and the board devicetree `axera/ax620e-xm-ipc.dts` (the stock
camera's own DT turned back into source).

`ax620e.lite.config` is the branch's `ax620e_xm_ipc_defconfig`. Where the
vendor modules depend on it, it matches the vendor kernel: SMP, PREEMPT,
HZ=100, module unload, no modversions, KPROBES (sizes `struct module`
like the vendor `.ko`). Release must be exactly `5.15.73`.

To build against a local kernel tree instead of the branch tarball:

    make BOARD=ax620e_lite LINUX_OVERRIDE_SRCDIR=/path/to/linux

## Flash layout and bootloader

The standard OpenIPC NOR layout (lite): boot 256K, env 64K @0x40000,
kernel 2M @0x50000 (`uImage`, LZMA multi-file with the DTB), rootfs 5M
@0x250000, rootfs_data after it; the NOR is `spi_nor.0` (the devicetree
label the vendor kernel and its `mtdparts=` use).

The boot partition holds the vendor SPL (64K @0, kept: the boot ROM loads
and checks it, it brings up DDR and loads U-Boot's image from 0x10000) and
the OpenIPC U-Boot built from the Axera SDK's U-Boot 2020.04
(`openipc_ax620e_nor_defconfig`, ax_gzip payload behind the SDK image
header, signed with the SDK's test key -- the key the stock image carries).
It starts with OpenIPC's default environment (`bootcmd=run bootflash`,
`osmem`, `extras`, `uk`/`ur`/`fwupd`), saves it on first start, and boots
the kernel slot. With the stock Xiongmai U-Boot instead, the firmware has to
sit at 0x60000 and the env needs `xmuart=0` and the OpenIPC bootcmd.

At boot `S70vendor` runs `load_axera` (ipcinfo -v says `axera`): the 21
vendor modules load in the stock loader's order, `ax_cmm` gets the DRAM
above `mem=` as its pool.

## Tested

In QEMU only (OpenIPC/qemu-bench `-M ax620e`, `-global
axera-spi-nor.env-from-flash=on`), from a whole-flash image of the vendor
SPL, the OpenIPC U-Boot, this build's `uImage` at 0x50000 and
`rootfs.squashfs` at 0x250000, env blank:

- U-Boot saves its default env and boots the kernel slot; kernel
  `5.15.73`, `mtdparts=spi_nor.0:256k(boot),64k(env),2048k(kernel),5120k(rootfs),-(rootfs_data)`;
- squashfs root, `/dev/mtdblock4` (rootfs_data) mounted as jffs2 on
  `/overlay`, overlayfs on `/`;
- first login sets the root password; `ipcinfo -v/-c/-f` print
  `axera`/`ax620e`/`ax620e`; `S70vendor` runs `load_axera`: all 21
  modules in `modules.dep` load (`lsmod`), no failure logged, no oops;
- a file written to `/etc` and the password survive a reboot.

Real hardware has not run this image yet: clocks, EPHY, watchdog, SPI
dual reads and the vendor SPL's secure-boot check are unverified there.

Known gaps: no majestic build for Axera (BR2_PACKAGE_MAJESTIC is off);
`ax_vdec` fails its probe in QEMU (no decoder model) exactly as on the
stock firmware there.
