/* LD_PRELOAD shim for majestic on hi3518ev200/OpenIPC.
 *
 * Majestic builds combo_dev_attr_t with raw_data_type=0 (uninitialized)
 * and the kernel rejects it: "invalid raw_data_type, must be in [2,5)".
 *
 * Layout of combo_dev_attr_t for MIPI mode:
 *   offset 0..3  : input_mode_t input_mode
 *   offset 4..7  : mipi_dev_attr_t.raw_data_type   <-- patch this to 2 (RAW10)
 *   offset 8..23 : mipi_dev_attr_t.lane_id[8] (shorts)
 *
 * HI_MIPI_SET_DEV_ATTR ioctl number = 0x41306d01 (_IOW('m',1,combo_dev_attr_t)).
 *
 * Build (OpenIPC musl toolchain):
 *   arm-openipc-linux-musleabi-gcc -O2 -fPIC -shared \
 *     -o libfixmipi.so libfixmipi.c -ldl
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dlfcn.h>

#define HI_MIPI_SET_DEV_ATTR 0x41306d01UL

typedef int (*ioctl_fn)(int fd, int req, ...);

static ioctl_fn real_ioctl = NULL;

int ioctl(int fd, int req, ...)
{
    va_list ap;
    void *argp;

    va_start(ap, req);
    argp = va_arg(ap, void *);
    va_end(ap);

    if (!real_ioctl)
        real_ioctl = (ioctl_fn)dlsym(RTLD_NEXT, "ioctl");

    /* VIU_DEV_ATTR_SET (ioctl 0x40804900: type='I'=0x49, nr=0, dir=write, 128 bytes).
     * Patch enIntfMode (offset 0) to VI_MODE_MIPI=5 and au32CompMask[0] (offset 8)
     * to 0xfff00000 — matches vendor's MIPI dev attr (per vendor diag SNAPSHOT.md). */
    if (argp != NULL && (unsigned int)req == 0x40804900U) {
        uint32_t *p32 = (uint32_t *)argp;
        if (p32[0] != 5) {
            p32[0] = 5;
            p32[2] = 0xfff00000;
            fprintf(stderr, "[libfixmipi] VIU dev_attr: intf=5 mask0=0xfff00000\n");
        }
    }

    if ((unsigned int)req == HI_MIPI_SET_DEV_ATTR && argp != NULL) {
        uint32_t *p32 = (uint32_t *)argp;
        int16_t  *p16 = (int16_t  *)argp;
        if (p32[0] == 0 /* MIPI */) {
            /* mipi_dev_attr_t at union offset 4: u32 raw_data_type + s16 lane_id[8].
             * KC110 OV2735 is 2-lane MIPI per vendor diag. */
            p32[1] = 2;          /* RAW_DATA_10BIT */
            p16[4] = 0;          /* lane_id[0] */
            p16[5] = 1;          /* lane_id[1] */
            p16[6] = -1; p16[7] = -1; p16[8] = -1;
            p16[9] = -1; p16[10] = -1; p16[11] = -1;
            fprintf(stderr, "[libfixmipi] MIPI dev_attr: rdt=2 lanes=[0,1]\n");
        }
    }

    return real_ioctl(fd, req, argp);
}
