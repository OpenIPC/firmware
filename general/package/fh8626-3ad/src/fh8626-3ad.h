/*
 * FH8626V100 user-space 3A daemon: common definitions.
 *
 * Copyright (C) 2026 Difan Zhang
 * Licensed under the Apache License, Version 2.0 or GNU GPL v2.
 */

#ifndef _FH8626_3AD_H
#define _FH8626_3AD_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/videodev2.h>

#define FH8626_3AD_VERSION "2.0.0"

/*
 * Perceived 12-bit AE level: the square root of the linear 12-bit luma,
 * scaled back to 12 bits. 1856 is mid-grey after the tone curves, a linear
 * level of 1856^2 / 4096 = 841 (20.5% of full scale). In low light the
 * target relaxes towards 1600 to limit gain and noise.
 */
#define DEFAULT_AE_TARGET_LUMA 1856U
#define DEFAULT_AE_LOW_LIGHT_TARGET 1600U

/* Logging macros */
extern int g_verbose;

#define LOG_INFO(fmt, ...) \
	fprintf(stdout, "[3AD INFO] " fmt "\n", ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
	fprintf(stderr, "[3AD WARN] " fmt "\n", ##__VA_ARGS__)

#define LOG_ERR(fmt, ...) \
	fprintf(stderr, "[3AD ERR]  " fmt "\n", ##__VA_ARGS__)

#define LOG_DBG(fmt, ...) \
	do { \
		if (g_verbose) \
			fprintf(stdout, "[3AD DBG]  " fmt "\n", ##__VA_ARGS__); \
	} while (0)

/* Integer clamp utility */
static inline int32_t clamp_i32(int32_t val, int32_t min_val, int32_t max_val)
{
	if (val < min_val)
		return min_val;
	if (val > max_val)
		return max_val;
	return val;
}

static inline uint32_t clamp_u32(uint32_t val, uint32_t min_val, uint32_t max_val)
{
	if (val < min_val)
		return min_val;
	if (val > max_val)
		return max_val;
	return val;
}

static inline int32_t abs_i32(int32_t v)
{
	return v < 0 ? -v : v;
}

/*
 * Fixed-point logarithm and exponential, base 2, in Q8 (256 = one stop).
 * The pair is accurate to about 0.2% and exactly inverse at powers of two,
 * so a controller error of zero means equal linear values.
 */
int32_t fx_log2_q8(uint64_t val);
/* 2^(q8 / 256) in Q16; q8 is clamped to [-16, +15] stops. */
uint64_t fx_exp2_q16(int32_t q8);
uint32_t fx_isqrt(uint64_t val);

/*
 * One frame of zone statistics, reduced from the ISP grid: channel means in
 * Q4 12-bit (16 = one 12-bit level) after black level and ISP digital gain,
 * before white balance, and the largest sample of each cell.
 */
#define GRID_COLS 16
#define GRID_ROWS 16
#define GRID_CELLS (GRID_COLS * GRID_ROWS)
#define GRID_MEAN_MAX 0xffffU

struct fh8626_grid {
	uint32_t sequence;
	uint64_t timestamp_ns;
	uint16_t r[GRID_CELLS], g[GRID_CELLS], b[GRID_CELLS];
	uint16_t max[GRID_CELLS];
	bool motion_valid;
	unsigned int motion_blocks, motion_total;
};

#endif /* _FH8626_3AD_H */
