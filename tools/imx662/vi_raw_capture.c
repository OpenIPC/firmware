/*
 * vi_raw_capture.c - Stream raw frames from VI pipe over TCP
 * with exposure/gain control and FITS save.
 *
 * Protocol (text, newline-terminated):
 *   E <lines>      - set exposure (integration time in lines, max VMAX-1)
 *   A <value>      - set analog gain (1024=1x, max 32768)
 *   D <value>      - set digital gain (1024=1x, max 16384)
 *   S [filename]   - save next frame as FITS
 *   ?              - query current values
 *
 * Response: "OK E=<exp> A=<again> D=<dgain>\n"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_vi.h"
#include "ot_common_vb.h"
#include "ot_common_video.h"
#include "ot_common_sys.h"
#include "ot_mipi_rx.h"
#include "ot_i2c.h"
#include "ot_mpi_vi.h"
#include "ot_mpi_vb.h"
#include "ot_mpi_sys.h"

#define VI_DEV_ID       0
#define VI_PIPE_ID      0
#define VI_CHN_ID       0
#define IMG_WIDTH       1920
#define IMG_HEIGHT      1080
#define I2C_DEV_ADDR    0x34
#define IMG_W_STRIDE    ((IMG_WIDTH * 12 + 127) / 128 * 128 / 8)
#define RAW12_BUF_SIZE  (IMG_W_STRIDE * IMG_HEIGHT)
#define TCP_PORT        5000
#define VMAX            1250

/* Exposure registers */
#define REG_EXP_LSB     0x3050
#define REG_EXP_MID     0x3051
#define REG_EXP_MSB     0x3052
/* Analog gain registers */
#define REG_AGAIN_LSB   0x306C
#define REG_AGAIN_MSB   0x306D
/* Digital gain registers */
#define REG_DGAIN_LSB   0x3070
#define REG_DGAIN_MSB   0x3071
/* Group hold */
#define REG_GROUP_HOLD  0x3001

static int g_i2c_fd = -1;
static volatile int g_running = 1;
static td_u32 g_exposure = 4000;
static td_u32 g_again = 1024;
static td_u32 g_dgain = 1024;
static int g_save_next = 0;
static char g_save_filename[256] = "frame.fits";

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static td_s32 i2c_write_reg(td_u16 reg, td_u8 val)
{
    unsigned char buf[3];
    buf[0] = (reg >> 8) & 0xFF;
    buf[1] = reg & 0xFF;
    buf[2] = val;
    int ret = write(g_i2c_fd, buf, 3);
    if (ret < 0) return -1;
    return 0;
}

static td_u8 i2c_read_reg(td_u16 reg)
{
    unsigned char rbuf[2] = {(reg >> 8) & 0xFF, reg & 0xFF};
    unsigned char val = 0;
    write(g_i2c_fd, rbuf, 2);
    read(g_i2c_fd, &val, 1);
    return val;
}

static void sensor_write_exposure(td_u32 exp)
{
    if (exp > VMAX - 1) exp = VMAX - 1;
    g_exposure = exp;
    /* Group hold on */
    i2c_write_reg(REG_GROUP_HOLD, 0x01);
    i2c_write_reg(REG_EXP_LSB, exp & 0xFF);
    i2c_write_reg(REG_EXP_MID, (exp >> 8) & 0xFF);
    i2c_write_reg(REG_EXP_MSB, (exp >> 16) & 0x0F);
    /* Group hold off + trigger */
    i2c_write_reg(REG_GROUP_HOLD, 0x00);
}

static void sensor_write_gain(td_u32 again, td_u32 dgain)
{
    if (again > 32768) again = 32768;
    if (dgain > 16384) dgain = 16384;
    g_again = again;
    g_dgain = dgain;
    i2c_write_reg(REG_GROUP_HOLD, 0x01);
    i2c_write_reg(REG_AGAIN_LSB, again & 0xFF);
    i2c_write_reg(REG_AGAIN_MSB, (again >> 8) & 0xFF);
    i2c_write_reg(REG_DGAIN_LSB, dgain & 0xFF);
    i2c_write_reg(REG_DGAIN_MSB, (dgain >> 8) & 0xFF);
    i2c_write_reg(REG_GROUP_HOLD, 0x00);
}

static const struct { td_u16 reg; td_u8 val; } imx662_init_common[] = {
    {0x3000, 0x01}, {0x3001, 0x00}, {0x3002, 0x00},
    {0x3014, 0x01},
    {0x301A, 0x00}, {0x301B, 0x00}, {0x301C, 0x00},
    {0x301E, 0x01}, {0x3020, 0x00}, {0x3021, 0x00}, {0x3026, 0x04},
    {0x3030, 0x00}, {0x3031, 0x00}, {0x3032, 0x00},
    {0x303C, 0x00}, {0x303D, 0x00},
    {0x303E, 0x90}, {0x303F, 0x07},
    {0x3044, 0x00}, {0x3045, 0x00},
    {0x3046, 0x4C}, {0x3047, 0x04},
    {0x3054, 0x0E}, {0x3055, 0x00}, {0x3056, 0x00},
    {0x3058, 0x8A}, {0x3059, 0x01}, {0x305A, 0x00},
    {0x3060, 0x16}, {0x3061, 0x01}, {0x3062, 0x00},
    {0x3064, 0xC4}, {0x3065, 0x0C}, {0x3066, 0x00},
    {0x3069, 0x00}, {0x306B, 0x00},
    {0x3070, 0x00}, {0x3071, 0x00}, {0x3072, 0x00}, {0x3073, 0x00},
    {0x3074, 0x00}, {0x3075, 0x00},
    {0x3081, 0x00},
    {0x308C, 0x00}, {0x308D, 0x01},
    {0x3094, 0x00}, {0x3095, 0x00}, {0x3096, 0x00}, {0x3097, 0x00},
    {0x309C, 0x00}, {0x309D, 0x00},
    {0x30A4, 0xAA}, {0x30A6, 0x0F},
    {0x30CC, 0x00}, {0x30CD, 0x00},
    {0x30DC, 0x32}, {0x30DD, 0x40},
    {0x3400, 0x01}, {0x3444, 0xAC},
    {0x3460, 0x21}, {0x3492, 0x08},
    {0x3B00, 0x39}, {0x3B23, 0x2D}, {0x3B45, 0x04},
    {0x3C0A, 0x1F}, {0x3C0B, 0x1E}, {0x3C38, 0x21},
    {0x3C40, 0x06}, {0x3C44, 0x00},
    {0x3CB6, 0xD8}, {0x3CC4, 0xDA},
    {0x3E24, 0x79}, {0x3E2C, 0x15}, {0x3EDC, 0x2D},
    {0x4498, 0x05}, {0x4499, 0x06}, {0x449A, 0x00}, {0x449B, 0x10},
    {0x449C, 0x19}, {0x449D, 0x00}, {0x449E, 0x32}, {0x449F, 0x01},
    {0x44A0, 0x92}, {0x44A2, 0x91}, {0x44A4, 0x8C}, {0x44A6, 0x87},
    {0x44A8, 0x82}, {0x44AA, 0x78}, {0x44AC, 0x6E}, {0x44AE, 0x69},
    {0x44B0, 0x92}, {0x44B2, 0x91}, {0x44B4, 0x8C}, {0x44B6, 0x87},
    {0x44B8, 0x82}, {0x44BA, 0x78}, {0x44BC, 0x6E}, {0x44BE, 0x69},
    {0x44C1, 0x01}, {0x44C2, 0x7F}, {0x44C3, 0x01}, {0x44C4, 0x7A},
    {0x44C5, 0x01}, {0x44C6, 0x7A}, {0x44C7, 0x01}, {0x44C8, 0x70},
    {0x44C9, 0x01}, {0x44CA, 0x6B}, {0x44CB, 0x01}, {0x44CC, 0x6B},
    {0x44CD, 0x01}, {0x44CE, 0x5C}, {0x44CF, 0x01}, {0x44D0, 0x7F},
    {0x44D1, 0x01}, {0x44D2, 0x7F}, {0x44D3, 0x01}, {0x44D4, 0x7A},
    {0x44D5, 0x01}, {0x44D6, 0x7A}, {0x44D7, 0x01}, {0x44D8, 0x70},
    {0x44D9, 0x01}, {0x44DA, 0x6B}, {0x44DB, 0x01}, {0x44DC, 0x6B},
    {0x44DD, 0x01}, {0x44DE, 0x5C}, {0x44DF, 0x01},
    {0x4534, 0x1C}, {0x4535, 0x03},
    {0x4538, 0x1C}, {0x4539, 0x1C}, {0x453A, 0x1C}, {0x453B, 0x1C},
    {0x453C, 0x1C}, {0x453D, 0x1C}, {0x453E, 0x1C}, {0x453F, 0x1C},
    {0x4540, 0x1C}, {0x4541, 0x03}, {0x4542, 0x03}, {0x4543, 0x03},
    {0x4544, 0x03}, {0x4545, 0x03}, {0x4546, 0x03}, {0x4547, 0x03},
    {0x4548, 0x03}, {0x4549, 0x03},
};

static const struct { td_u16 reg; td_u8 val; } imx662_init_mode[] = {
    {0x3015, 0x02},
    {0x3018, 0x00},
    {0x3040, 0x00},
    {0x3022, 0x00},
    {0x3023, 0x01},
    {0x301B, 0x00},
    {0x3A50, 0x62},
    {0x3A51, 0x01},
    {0x3A52, 0x19},
    {0x3028, 0xE2},
    {0x3029, 0x04},
    {0x302A, 0x00},
    {0x302C, 0xBC},
    {0x302D, 0x07},
};

static td_s32 init_sensor_full(void)
{
    td_s32 ok = 0, fail = 0;
    td_u32 total;

    total = sizeof(imx662_init_common) / sizeof(imx662_init_common[0]);
    fprintf(stderr, "  Phase 1: %u regs... ", total);
    fflush(stderr);
    for (td_u32 i = 0; i < total; i++) {
        if (i2c_write_reg(imx662_init_common[i].reg, imx662_init_common[i].val) == 0) ok++;
        else fail++;
        usleep(100);
    }
    fprintf(stderr, "%d ok %d fail\n", ok, fail);

    fprintf(stderr, "  Phase 2: exit standby + PLL lock...\n");
    i2c_write_reg(0x3000, 0x00);
    i2c_write_reg(0x3001, 0x00);
    usleep(100000);

    int ok2 = 0, fail2 = 0;
    total = sizeof(imx662_init_mode) / sizeof(imx662_init_mode[0]);
    fprintf(stderr, "  Phase 3: %u regs... ", total);
    fflush(stderr);
    for (td_u32 i = 0; i < total; i++) {
        if (i2c_write_reg(imx662_init_mode[i].reg, imx662_init_mode[i].val) == 0) ok2++;
        else fail2++;
        usleep(100);
    }
    fprintf(stderr, "%d ok %d fail\n", ok2, fail2);

    /* Apply default exposure/gain */
    sensor_write_exposure(g_exposure);
    sensor_write_gain(g_again, g_dgain);

    return (fail == 0 && fail2 == 0) ? 0 : -1;
}

static td_s32 enable_mclk_and_reset_sensor(void)
{
    td_s32 fd;
    lane_divide_mode_t lane_mode = LANE_DIVIDE_MODE_0;
    sns_clk_source_t clk_source = 0;
    sns_rst_source_t rst_source = 0;

    fd = open("/dev/ot_mipi_rx", O_RDWR);
    if (fd < 0) return -1;

    ioctl(fd, OT_MIPI_SET_HS_MODE, &lane_mode);
    ioctl(fd, OT_MIPI_ENABLE_SENSOR_CLOCK, &clk_source);
    ioctl(fd, OT_MIPI_RESET_SENSOR, &rst_source);
    usleep(10000);
    ioctl(fd, OT_MIPI_UNRESET_SENSOR, &rst_source);
    usleep(100000);
    close(fd);
    return 0;
}

static int send_all(int fd, const void *buf, size_t len)
{
    const char *p = (const char *)buf;
    while (len > 0) {
        ssize_t n = send(fd, p, len, MSG_NOSIGNAL);
        if (n <= 0) return -1;
        p += n;
        len -= n;
    }
    return 0;
}

static int send_frame_header(int fd, td_u16 w, td_u16 h, td_u16 stride, td_u32 size)
{
    unsigned char hdr[12];
    hdr[0] = w & 0xFF; hdr[1] = (w >> 8) & 0xFF;
    hdr[2] = h & 0xFF; hdr[3] = (h >> 8) & 0xFF;
    hdr[4] = stride & 0xFF; hdr[5] = (stride >> 8) & 0xFF;
    hdr[6] = 0; hdr[7] = 0;
    hdr[8] = size & 0xFF; hdr[9] = (size >> 8) & 0xFF;
    hdr[10] = (size >> 16) & 0xFF; hdr[11] = (size >> 24) & 0xFF;
    return send_all(fd, hdr, 12);
}

/* FITS write: simple 16-bit unsigned, 1920x1080, no compression */
static int write_fits(const char *filename, const td_u8 *raw_data,
                       td_u32 width, td_u32 height, td_u32 stride)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) return -1;

    /* FITS header block (2880 bytes) */
    char header[2880];
    memset(header, ' ', sizeof(header));
    int pos = 0;

    #define FITS_CARD(key, val) do { \
        snprintf(header + pos, 81, "%-8s= %-*s", key, (int)(sizeof(header) - pos - 20), val); \
        pos = ((pos / 80) + 1) * 80; \
    } while(0)

    FITS_CARD("SIMPLE", "T");
    FITS_CARD("BITPIX", "16");
    FITS_CARD("NAXIS", "2");
    FITS_CARD("NAXIS1", "1920");
    FITS_CARD("NAXIS2", "1080");
    FITS_CARD("DATATYPE", "UINT16");
    FITS_CARD("BSCALE", "1.0");
    FITS_CARD("BZERO", "0.0");

    char comment[81];
    snprintf(comment, 81, "EXPOSURE= %d lines", g_exposure);
    memcpy(header + pos, comment, 80); pos += 80;
    snprintf(comment, 81, "AGAIN    = %d (1024=1x)", g_again);
    memcpy(header + pos, comment, 80); pos += 80;
    snprintf(comment, 81, "DGAIN    = %d (1024=1x)", g_dgain);
    memcpy(header + pos, comment, 80); pos += 80;
    snprintf(comment, 81, "INSTRUME = IMX662 Hi3516CV610");
    memcpy(header + pos, comment, 80); pos += 80;

    /* End card */
    memset(header + pos, ' ', 80);
    memcpy(header + pos, "END", 3);
    pos = 2880;

    fwrite(header, 1, 2880, fp);

    /* Convert raw12 packed to 16-bit unsigned (12-bit left-aligned in 16-bit) */
    for (td_u32 y = 0; y < height; y++) {
        const td_u8 *row = raw_data + (td_u64)y * stride;
        for (td_u32 x = 0; x < width; x += 2) {
            td_u32 idx = (x * 3) / 2;
            if (idx + 2 >= stride) break;
            td_u16 p0 = ((td_u16)row[idx] << 4) | ((td_u16)(row[idx + 1] >> 4) & 0x0F);
            td_u16 p1 = (((td_u16)(row[idx + 1] & 0x0F)) << 8) | (td_u16)row[idx + 2];
            /* FITS is big-endian */
            td_u8 b0, b1;
            b0 = (p0 >> 8) & 0xFF; b1 = p0 & 0xFF;
            fwrite(&b0, 1, 1, fp); fwrite(&b1, 1, 1, fp);
            b0 = (p1 >> 8) & 0xFF; b1 = p1 & 0xFF;
            fwrite(&b0, 1, 1, fp); fwrite(&b1, 1, 1, fp);
        }
    }

    /* Pad to 2880-byte block */
    long pos_now = ftell(fp);
    long remainder = pos_now % 2880;
    if (remainder != 0) {
        char pad[2880];
        memset(pad, 0, sizeof(pad));
        fwrite(pad, 1, 2880 - remainder, fp);
    }

    fclose(fp);
    fprintf(stderr, "  FITS saved: %s (%ux%u)\n", filename, width, height);
    return 0;
}

static void process_command(const char *cmd)
{
    if (cmd[0] == 'E' && cmd[1] == ' ') {
        td_u32 val = atoi(cmd + 2);
        sensor_write_exposure(val);
        fprintf(stderr, "  EXP=%u\n", g_exposure);
    } else if (cmd[0] == 'A' && cmd[1] == ' ') {
        td_u32 val = atoi(cmd + 2);
        sensor_write_gain(val, g_dgain);
        fprintf(stderr, "  AGAIN=%u\n", g_again);
    } else if (cmd[0] == 'D' && cmd[1] == ' ') {
        td_u32 val = atoi(cmd + 2);
        sensor_write_gain(g_again, val);
        fprintf(stderr, "  DGAIN=%u\n", g_dgain);
    } else if (cmd[0] == 'S') {
        g_save_next = 1;
        if (strlen(cmd) > 2)
            snprintf(g_save_filename, sizeof(g_save_filename), "%s", cmd + 2);
        fprintf(stderr, "  Will save next frame as FITS\n");
    } else if (cmd[0] == '?') {
        fprintf(stderr, "  E=%u A=%u D=%u VMAX=%d\n", g_exposure, g_again, g_dgain, VMAX);
    }
}

int main(int argc, char *argv[])
{
    td_s32 ret;
    int port = TCP_PORT;
    ot_vb_pool pool = OT_VB_INVALID_POOL_ID;

    if (argc > 1) port = atoi(argv[1]);
    if (port <= 0) port = TCP_PORT;

    fprintf(stderr, "=== VI Stream + Control (port %d) ===\n", port);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    fprintf(stderr, "[0] MPI init...\n");
    ret = ot_mpi_sys_init();
    if (ret != TD_SUCCESS) {
        fprintf(stderr, "  sys_init 0x%x, cleaning...\n", ret);
        ot_mpi_vi_disable_chn(VI_PIPE_ID, VI_CHN_ID);
        ot_mpi_vi_stop_pipe(VI_PIPE_ID);
        ot_mpi_vi_destroy_pipe(VI_PIPE_ID);
        ot_mpi_vi_disable_dev(VI_DEV_ID);
        ot_mpi_sys_exit();
        usleep(500000);
        ret = ot_mpi_sys_init();
        if (ret != TD_SUCCESS) {
            fprintf(stderr, "  sys_init FAILED: 0x%x\n", ret);
            return 1;
        }
    }
    fprintf(stderr, "  OK\n");

    fprintf(stderr, "[2] VB init...\n");
    ot_mpi_vb_exit();
    usleep(50000);
    ot_vb_cfg vb_cfg;
    memset(&vb_cfg, 0, sizeof(vb_cfg));
    vb_cfg.max_pool_cnt = 1;
    vb_cfg.common_pool[0].blk_size = RAW12_BUF_SIZE;
    vb_cfg.common_pool[0].blk_cnt = 4;
    vb_cfg.common_pool[0].remap_mode = OT_VB_REMAP_MODE_NONE;
    strncpy(vb_cfg.common_pool[0].mmz_name, "anonymous", sizeof(vb_cfg.common_pool[0].mmz_name) - 1);
    ret = ot_mpi_vb_set_cfg(&vb_cfg);
    fprintf(stderr, "  set_cfg: 0x%x\n", ret);
    ret = ot_mpi_vb_init();
    fprintf(stderr, "  init: 0x%x\n", ret);

    fprintf(stderr, "[3] VB pool...\n");
    ot_vb_pool_cfg pool_cfg;
    memset(&pool_cfg, 0, sizeof(pool_cfg));
    pool_cfg.blk_size = RAW12_BUF_SIZE;
    pool_cfg.blk_cnt = 4;
    pool_cfg.remap_mode = OT_VB_REMAP_MODE_NONE;
    strncpy(pool_cfg.mmz_name, "anonymous", sizeof(pool_cfg.mmz_name) - 1);
    pool = ot_mpi_vb_create_pool(&pool_cfg);
    if (pool == OT_VB_INVALID_POOL_ID) { fprintf(stderr, "  FAILED\n"); goto cleanup; }
    fprintf(stderr, "  pool=%u\n", pool);
    ot_mpi_vb_init_mod_common_pool(OT_VB_UID_VI);

    fprintf(stderr, "[4] VI-VPSS mode...\n");
    ot_vi_vpss_mode vpss_mode;
    memset(&vpss_mode, 0, sizeof(vpss_mode));
    for (int i = 0; i < 4; i++) vpss_mode.mode[i] = OT_VI_OFFLINE_VPSS_OFFLINE;
    ot_mpi_sys_set_vi_vpss_mode(&vpss_mode);

    fprintf(stderr, "[5] MCLK + sensor...\n");
    enable_mclk_and_reset_sensor();

    fprintf(stderr, "[5b] MIPI RX...\n");
    int mipi_fd = open("/dev/ot_mipi_rx", O_RDWR);
    if (mipi_fd >= 0) {
        combo_dev_attr_t dev_attr;
        memset(&dev_attr, 0, sizeof(dev_attr));
        dev_attr.devno = 0;
        dev_attr.input_mode = INPUT_MODE_MIPI;
        dev_attr.data_rate = MIPI_DATA_RATE_X1;
        dev_attr.img_rect.width = IMG_WIDTH;
        dev_attr.img_rect.height = IMG_HEIGHT;
        dev_attr.mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
        dev_attr.mipi_attr.wdr_mode = OT_MIPI_WDR_MODE_NONE;
        dev_attr.mipi_attr.lane_id[0] = 0;
        dev_attr.mipi_attr.lane_id[1] = -1;
        dev_attr.mipi_attr.lane_id[2] = -1;
        dev_attr.mipi_attr.lane_id[3] = -1;
        ioctl(mipi_fd, OT_MIPI_SET_DEV_ATTR, &dev_attr);
        combo_dev_t devno = 0;
        ioctl(mipi_fd, OT_MIPI_UNRESET_MIPI, &devno);
        usleep(10000);
        close(mipi_fd);
    }

    fprintf(stderr, "[6] Sensor I2C...\n");
    g_i2c_fd = open("/dev/i2c-0", O_RDWR);
    if (g_i2c_fd >= 0) {
        ret = ioctl(g_i2c_fd, OT_I2C_SLAVE_FORCE, (I2C_DEV_ADDR >> 1));
        if (ret < 0) { fprintf(stderr, "  I2C fail\n"); close(g_i2c_fd); g_i2c_fd = -1; }
        else {
            unsigned char rid_reg[2] = {0x30, 0xDC};
            unsigned char rid_val = 0;
            write(g_i2c_fd, rid_reg, 2);
            read(g_i2c_fd, &rid_val, 1);
            fprintf(stderr, "  Chip ID=0x%02X\n", rid_val);
            if (init_sensor_full() == 0) fprintf(stderr, "  Sensor OK\n");
        }
    }
    usleep(200000);

    fprintf(stderr, "[7-9] VI pipeline...\n");
    ot_mpi_vi_disable_chn(VI_PIPE_ID, VI_CHN_ID);
    ot_mpi_vi_stop_pipe(VI_PIPE_ID);
    ot_mpi_vi_unbind(VI_DEV_ID, VI_PIPE_ID);
    ot_mpi_vi_disable_dev(VI_DEV_ID);
    usleep(50000);

    ot_vi_dev_attr dev_attr;
    memset(&dev_attr, 0, sizeof(dev_attr));
    dev_attr.intf_mode = OT_VI_INTF_MODE_MIPI;
    dev_attr.work_mode = OT_VI_WORK_MODE_MULTIPLEX_1;
    dev_attr.scan_mode = OT_VI_SCAN_PROGRESSIVE;
    dev_attr.data_type = OT_VI_DATA_TYPE_RAW;
    dev_attr.in_size.width = IMG_WIDTH;
    dev_attr.in_size.height = IMG_HEIGHT;
    dev_attr.data_rate = OT_DATA_RATE_X1;
    dev_attr.component_mask[0] = 0xFFF0000;
    ret = ot_mpi_vi_set_dev_attr(VI_DEV_ID, &dev_attr);
    if (ret != TD_SUCCESS) { fprintf(stderr, "  set_dev_attr: 0x%x\n", ret); goto cleanup; }
    ret = ot_mpi_vi_enable_dev(VI_DEV_ID);
    if (ret != TD_SUCCESS) { fprintf(stderr, "  enable_dev: 0x%x\n", ret); goto cleanup; }
    ret = ot_mpi_vi_bind(VI_DEV_ID, VI_PIPE_ID);
    if (ret != TD_SUCCESS) { fprintf(stderr, "  bind: 0x%x\n", ret); goto cleanup; }

    ot_vi_pipe_attr pipe_attr;
    memset(&pipe_attr, 0, sizeof(pipe_attr));
    pipe_attr.isp_bypass = TD_TRUE;
    pipe_attr.size.width = IMG_WIDTH;
    pipe_attr.size.height = IMG_HEIGHT;
    pipe_attr.pixel_format = OT_PIXEL_FORMAT_RGB_BAYER_12BPP;
    pipe_attr.compress_mode = OT_COMPRESS_MODE_NONE;
    pipe_attr.frame_rate_ctrl.src_frame_rate = -1;
    pipe_attr.frame_rate_ctrl.dst_frame_rate = -1;
    ot_mpi_vi_stop_pipe(VI_PIPE_ID);
    ot_mpi_vi_create_pipe(VI_PIPE_ID, &pipe_attr);
    ot_mpi_vi_set_pipe_attr(VI_PIPE_ID, &pipe_attr);
    ot_mpi_vi_start_pipe(VI_PIPE_ID);

    ot_vi_chn_attr chn_attr;
    memset(&chn_attr, 0, sizeof(chn_attr));
    chn_attr.size.width = IMG_WIDTH;
    chn_attr.size.height = IMG_HEIGHT;
    chn_attr.pixel_format = OT_PIXEL_FORMAT_RGB_BAYER_12BPP;
    chn_attr.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
    chn_attr.video_format = OT_VIDEO_FORMAT_LINEAR;
    chn_attr.compress_mode = OT_COMPRESS_MODE_NONE;
    chn_attr.depth = 2;
    chn_attr.frame_rate_ctrl.src_frame_rate = -1;
    chn_attr.frame_rate_ctrl.dst_frame_rate = -1;
    ot_mpi_vi_disable_chn(VI_PIPE_ID, VI_CHN_ID);
    ot_mpi_vi_set_chn_attr(VI_PIPE_ID, VI_CHN_ID, &chn_attr);
    ret = ot_mpi_vi_enable_chn(VI_PIPE_ID, VI_CHN_ID);
    if (ret != TD_SUCCESS) { fprintf(stderr, "  enable_chn: 0x%x\n", ret); goto cleanup; }
    fprintf(stderr, "  VI OK\n");

    fprintf(stderr, "[10] TCP server on port %d...\n", port);
    fflush(stderr);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) goto cleanup;
    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) { close(server_fd); goto cleanup; }
    if (listen(server_fd, 1) < 0) { close(server_fd); goto cleanup; }
    fprintf(stderr, "  Waiting for client on port %d...\n", port);
    fflush(stderr);

    int mem_fd = open("/dev/mem", O_RDONLY | O_SYNC);
    if (mem_fd < 0) goto cleanup;

    int mem_mmaped = 0;
    void *mem_map = NULL;
    td_u64 mem_map_len = 0;
    td_u64 mem_map_phys = 0;

    /* Command receive buffer */
    char cmd_buf[512];
    int cmd_len = 0;

    while (g_running) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (client_fd < 0) continue;

        fprintf(stderr, "  Client: %s\n", inet_ntoa(cli_addr.sin_addr));
        fflush(stderr);

        /* Set non-blocking for select() */
        int flags = fcntl(client_fd, F_GETFL, 0);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

        int frame_count = 0;
        int consecutive_fail = 0;
        cmd_len = 0;

        while (g_running) {
            /* Check for incoming commands via select() */
            fd_set rfds;
            struct timeval tv = {0, 10000}; /* 10ms */
            FD_ZERO(&rfds);
            FD_SET(client_fd, &rfds);

            if (select(client_fd + 1, &rfds, NULL, NULL, &tv) > 0) {
                char tmp[256];
                ssize_t n = recv(client_fd, tmp, sizeof(tmp) - 1, 0);
                if (n <= 0) break; /* client disconnected */

                for (ssize_t i = 0; i < n; i++) {
                    if (tmp[i] == '\n' || tmp[i] == '\r') {
                        if (cmd_len > 0) {
                            cmd_buf[cmd_len] = '\0';
                            process_command(cmd_buf);

                            /* Send response */
                            char resp[128];
                            snprintf(resp, sizeof(resp), "OK E=%u A=%u D=%u\n", g_exposure, g_again, g_dgain);
                            send_all(client_fd, resp, strlen(resp));
                            cmd_len = 0;
                        }
                    } else if (cmd_len < (int)sizeof(cmd_buf) - 1) {
                        cmd_buf[cmd_len++] = tmp[i];
                    }
                }
            }

            /* Capture and send frame */
            ot_video_frame_info frame_info;
            memset(&frame_info, 0, sizeof(frame_info));
            ret = ot_mpi_vi_get_chn_frame(VI_PIPE_ID, VI_CHN_ID, &frame_info, 3000);
            if (ret != TD_SUCCESS) {
                consecutive_fail++;
                if (consecutive_fail > 5) break;
                continue;
            }
            consecutive_fail = 0;

            const ot_video_frame *vf = &frame_info.video_frame;
            td_u32 stride = vf->stride[0];
            td_u32 height = vf->height;
            td_u64 phys = vf->phys_addr[0];
            td_u64 map_size = (td_u64)stride * height;
            td_u64 page_phys = phys & ~0xFFFULL;
            td_u64 page_off = phys - page_phys;
            td_u64 map_len = ((map_size + page_off + 0xFFF) & ~0xFFFULL);

            if (!mem_mmaped || mem_map_phys != page_phys || mem_map_len != map_len) {
                if (mem_mmaped) munmap(mem_map, mem_map_len);
                mem_map = mmap(NULL, map_len, PROT_READ, MAP_SHARED, mem_fd, page_phys);
                if (mem_map == MAP_FAILED) {
                    ot_mpi_vi_release_chn_frame(VI_PIPE_ID, VI_CHN_ID, &frame_info);
                    break;
                }
                mem_map_phys = page_phys;
                mem_map_len = map_len;
                mem_mmaped = 1;
            }

            td_u8 *src = (td_u8 *)mem_map + page_off;

            /* Save FITS if requested */
            if (g_save_next) {
                write_fits(g_save_filename, src, vf->width, height, stride);
                g_save_next = 0;
            }

            /* Send frame over TCP */
            if (send_frame_header(client_fd, (td_u16)vf->width, (td_u16)height, (td_u16)stride, map_size) < 0) {
                ot_mpi_vi_release_chn_frame(VI_PIPE_ID, VI_CHN_ID, &frame_info);
                break;
            }
            if (send_all(client_fd, src, (size_t)map_size) < 0) {
                ot_mpi_vi_release_chn_frame(VI_PIPE_ID, VI_CHN_ID, &frame_info);
                break;
            }

            ot_mpi_vi_release_chn_frame(VI_PIPE_ID, VI_CHN_ID, &frame_info);
            frame_count++;
            if ((frame_count % 30) == 0) {
                fprintf(stderr, "  Sent %d frames, exp=%u again=%u dgain=%u\n",
                        frame_count, g_exposure, g_again, g_dgain);
                fflush(stderr);
            }
        }

        fprintf(stderr, "  Disconnected (%d frames)\n", frame_count);
        fflush(stderr);
        close(client_fd);
    }

    if (mem_mmaped) munmap(mem_map, mem_map_len);
    close(mem_fd);
    close(server_fd);

cleanup:
    fprintf(stderr, "Cleanup...\n");
    fflush(stderr);
    if (g_i2c_fd >= 0) close(g_i2c_fd);
    ot_mpi_vi_disable_chn(VI_PIPE_ID, VI_CHN_ID);
    ot_mpi_vi_stop_pipe(VI_PIPE_ID);
    ot_mpi_vi_destroy_pipe(VI_PIPE_ID);
    ot_mpi_vi_disable_dev(VI_DEV_ID);
    ot_mpi_vi_unbind(VI_DEV_ID, VI_PIPE_ID);
    usleep(200000);
    fprintf(stderr, "Done.\n");
    fflush(stderr);
    _exit(0);
}
