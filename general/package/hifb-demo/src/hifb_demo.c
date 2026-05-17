/*
 * hifb_demo — Hisilicon framebuffer userspace API example for hi3520dv200.
 *
 * Brings up MPP via the ported SAMPLE_COMM_SYS_Init helper (sample_comm_sys.c)
 * + a minimal VO bring-up using SAMPLE_COMM_VO_StartDev / StartLayer from
 * sample_comm_vo.c. Then exercises the standard Linux fb mmap path and the
 * hifb-specific ioctls (FBIOPUT_SHOW_HIFB, FBIOPUT_ALPHA_HIFB,
 * FBIOPUT_SCREEN_ORIGIN_HIFB).
 *
 * Draws SMPTE-style 7 color bars + a bouncing rectangle at ~30 fps so the
 * effect is visible on the wired display (HDMI / VGA / CVBS depending on
 * board jumpering — note that hi3520dv200's libmpi.so ships no
 * HI_MPI_HDMI_* exports, so HDMI auto-routing depends on what the VO
 * device wires by default; on some boards CVBS or VGA is on by default).
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "hi_type.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vo.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vo.h"
#include "hifb.h"
#include "sample_comm.h"

#define VO_DEV_DHD0   0          /* HD output device */
#define VO_LAYER_VHD0 0          /* HD video layer */

static volatile sig_atomic_t g_stop;
static void on_sigint(int s) { (void)s; g_stop = 1; }

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

static const uint8_t SMPTE_BARS[7][3] = {
    {255,255,255}, {255,255,0}, {0,255,255}, {0,255,0},
    {255,0,255},   {255,0,0},   {0,0,255}
};

static void draw_color_bars(uint16_t *fb, unsigned w, unsigned h, unsigned pitch_px)
{
    const unsigned bar_w = w / 7;
    for (unsigned y = 0; y < h; ++y) {
        uint16_t *row = fb + (size_t)y * pitch_px;
        for (unsigned x = 0; x < w; ++x) {
            unsigned i = x / bar_w; if (i > 6) i = 6;
            row[x] = rgb565(SMPTE_BARS[i][0], SMPTE_BARS[i][1], SMPTE_BARS[i][2]);
        }
    }
}

static void draw_rect(uint16_t *fb, unsigned pitch_px,
                      int x0, int y0, int w, int h, uint16_t color,
                      unsigned fb_w, unsigned fb_h)
{
    if (x0 < 0) { w += x0; x0 = 0; }
    if (y0 < 0) { h += y0; y0 = 0; }
    if (x0 + w > (int)fb_w) w = (int)fb_w - x0;
    if (y0 + h > (int)fb_h) h = (int)fb_h - y0;
    if (w <= 0 || h <= 0) return;
    for (int y = 0; y < h; ++y) {
        uint16_t *row = fb + (size_t)(y0 + y) * pitch_px + x0;
        for (int x = 0; x < w; ++x) row[x] = color;
    }
}

static int mpp_bringup(VO_INTF_TYPE_E intf, VO_INTF_SYNC_E sync)
{
    VB_CONF_S vb_cfg = {0};
    VO_PUB_ATTR_S pub = {0};
    HI_S32 rc;

    /* SAMPLE_COMM_SYS_Init internally does:
     *   HI_MPI_SYS_Exit -> HI_MPI_VB_DestroyPool(0..VB_MAX_POOLS) ->
     *   HI_MPI_VB_Exit -> HI_MPI_VB_SetConf -> HI_MPI_VB_Init ->
     *   HI_MPI_SYS_SetConf(u32AlignWidth=16) -> HI_MPI_SYS_Init.
     * This is the canonical bootstrap from the vendor mpp/sample/ tree. */
    vb_cfg.u32MaxPoolCnt = 0;  /* fb-only demo, no video buffer pools */
    rc = SAMPLE_COMM_SYS_Init(&vb_cfg);
    if (rc != HI_SUCCESS) {
        fprintf(stderr, "SAMPLE_COMM_SYS_Init: 0x%x\n", rc);
        return -1;
    }

    pub.u32BgColor = 0x000000;
    pub.enIntfType = intf;
    pub.enIntfSync = sync;
    pub.bDoubleFrame = HI_FALSE;
    rc = SAMPLE_COMM_VO_StartDev(VO_DEV_DHD0, &pub);
    if (rc != HI_SUCCESS) {
        fprintf(stderr, "SAMPLE_COMM_VO_StartDev: 0x%x\n", rc);
        return -1;
    }

    rc = HI_MPI_VO_EnableVideoLayer(VO_LAYER_VHD0);
    if (rc != HI_SUCCESS) {
        /* Non-fatal: hifb's graphics overlay can still be composed by VOU
         * even if the video plane is left disabled. */
        fprintf(stderr, "HI_MPI_VO_EnableVideoLayer: 0x%x (non-fatal)\n", rc);
    }
    return 0;
}

static void mpp_teardown(void)
{
    HI_MPI_VO_DisableVideoLayer(VO_LAYER_VHD0);
    SAMPLE_COMM_VO_StopDev(VO_DEV_DHD0);
    SAMPLE_COMM_SYS_Exit();
}

int main(int argc, char **argv)
{
    int run_secs = 30;
    const char *fbdev = "/dev/fb0";
    VO_INTF_TYPE_E intf = VO_INTF_HDMI;
    VO_INTF_SYNC_E sync = VO_OUTPUT_720P60;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-t") && i + 1 < argc) run_secs = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-f") && i + 1 < argc) fbdev = argv[++i];
        else if (!strcmp(argv[i], "-cvbs")) { intf = VO_INTF_CVBS; sync = VO_OUTPUT_PAL; }
        else if (!strcmp(argv[i], "-vga"))  { intf = VO_INTF_VGA;  sync = VO_OUTPUT_1024x768_60; }
        else if (!strcmp(argv[i], "-h")) {
            printf("usage: %s [-t secs] [-f /dev/fbN] [-cvbs|-vga]\n"
                   "  default: HDMI 720P60 on /dev/fb0 for 30 s\n", argv[0]);
            return 0;
        }
    }

    signal(SIGINT,  on_sigint);
    signal(SIGTERM, on_sigint);

    printf("hifb_demo: bringing up MPP (intf=0x%x sync=%d)\n",
           (unsigned)intf, (int)sync);
    if (mpp_bringup(intf, sync) < 0) return 1;

    printf("hifb_demo: open %s\n", fbdev);
    int fd = open(fbdev, O_RDWR);
    if (fd < 0) { perror("open"); mpp_teardown(); return 1; }

    struct fb_var_screeninfo v;
    struct fb_fix_screeninfo f;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &v) < 0) { perror("FBIOGET_VSCREENINFO"); return 1; }
    if (ioctl(fd, FBIOGET_FSCREENINFO, &f) < 0) { perror("FBIOGET_FSCREENINFO"); return 1; }
    printf("hifb_demo: %ux%u bpp=%u line_length=%u smem_len=%u\n",
           v.xres, v.yres, v.bits_per_pixel, f.line_length, f.smem_len);

    if (v.bits_per_pixel != 16) {
        fprintf(stderr, "WARN: expected 16bpp RGB565, got %u\n", v.bits_per_pixel);
    }

    void *mem = mmap(NULL, f.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) { perror("mmap"); return 1; }

    HIFB_ALPHA_S a = {0};
    a.bAlphaEnable  = HI_TRUE;
    a.bAlphaChannel = HI_FALSE;
    a.u8GlobalAlpha = 0xFF;
    if (ioctl(fd, FBIOPUT_ALPHA_HIFB, &a) < 0) perror("FBIOPUT_ALPHA_HIFB (non-fatal)");

    HIFB_POINT_S p = { 0, 0 };
    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &p) < 0) perror("FBIOPUT_SCREEN_ORIGIN_HIFB (non-fatal)");

    HI_BOOL show = HI_TRUE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &show) < 0) perror("FBIOPUT_SHOW_HIFB");

    const unsigned pitch_px = f.line_length / 2;
    const unsigned w = v.xres, h = v.yres;
    uint16_t *fb = (uint16_t *)mem;

    printf("hifb_demo: drawing color bars + moving square for ~%ds (Ctrl-C to stop)\n",
           run_secs);

    draw_color_bars(fb, w, h, pitch_px);

    int rx = 0, ry = (int)h / 2 - 32, rw = 96, rh = 64;
    int vx = 6, vy = 3;
    uint16_t rect_color = rgb565(0, 0, 0);

    struct timespec t0, now;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    while (!g_stop) {
        /* Erase old rectangle by repainting the bar slice it covered. */
        for (int y = ry; y < ry + rh && y < (int)h; ++y) {
            if (y < 0) continue;
            uint16_t *row = fb + (size_t)y * pitch_px;
            for (int x = rx; x < rx + rw && x < (int)w; ++x) {
                if (x < 0) continue;
                unsigned bi = (unsigned)x / (w / 7);
                if (bi > 6) bi = 6;
                row[x] = rgb565(SMPTE_BARS[bi][0], SMPTE_BARS[bi][1], SMPTE_BARS[bi][2]);
            }
        }
        rx += vx; ry += vy;
        if (rx < 0 || rx + rw > (int)w) vx = -vx;
        if (ry < 0 || ry + rh > (int)h) vy = -vy;
        if (rx < 0) rx = 0;
        if (ry < 0) ry = 0;
        if (rx + rw > (int)w) rx = (int)w - rw;
        if (ry + rh > (int)h) ry = (int)h - rh;
        draw_rect(fb, pitch_px, rx, ry, rw, rh, rect_color, w, h);

        usleep(33 * 1000);

        clock_gettime(CLOCK_MONOTONIC, &now);
        if (run_secs > 0 && now.tv_sec - t0.tv_sec >= run_secs) break;
    }

    printf("hifb_demo: cleanup\n");
    show = HI_FALSE;
    ioctl(fd, FBIOPUT_SHOW_HIFB, &show);
    munmap(mem, f.smem_len);
    close(fd);
    mpp_teardown();
    return 0;
}
