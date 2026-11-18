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

## Booting on a Xiongmai IVG-A6 (stock U-Boot 2020.04)

Flash layout: spl 64K @0, U-Boot @0x10000, env 64K @0x50000 (single
copy, CRC32), then `firmware.bin` @0x60000. The stock env boots a
squashfs `/boot/uImage` (`loadromfs ... squashfsload; axera_boot`);
OpenIPC needs, at minimum:

    setenv xmuart 0          # without it the console is muted after "Starting kernel ..."
    setenv osmem 120M        # kernel RAM; the rest (to totalmem) is ax_cmm's pool
    setenv totalmem 256M
    setenv bootargs 'mem=${osmem} console=ttyS0,115200n8 panic=20 rootfstype=squashfs root=/dev/mtdblock3 init=/init mtdparts=spi_nor.0:320k(boot),64k(env),<kernel>k(kernel),<rootfs>k(rootfs),-(rootfs_data)'
    setenv bootcmd 'setenv setargs setenv bootargs ${bootargs};run setargs;sf probe 0;sf read 0x45000000 0x60000 <uImage size>;bootm 0x45000000'

The NOR is `spi_nor.0` (devicetree label, the vendor kernel's name). The
stock U-Boot's console asks for a password after an interrupted autoboot,
so set these from the stock Linux (or write the env block). The kernel has
no mtdsplit, hence explicit kernel/rootfs sizes.

At boot `S70vendor` runs `load_axera` (ipcinfo -v says `axera`): the 21
vendor modules load in the stock loader's order, `ax_cmm` gets the DRAM
above `mem=` as its pool.

Known gaps: no majestic build for Axera (BR2_PACKAGE_MAJESTIC is off);
`ax_vdec` fails its probe in QEMU (no decoder model) exactly as on the
stock firmware there.
