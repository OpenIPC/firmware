/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only */
/* Copyright (C) 2026 Difan Zhang */
/* Independent ISP statistics consumer; the video application owns capture. */
#include <getopt.h>
#include <glob.h>
#include <time.h>
#include <sys/sysmacros.h>
#include <sys/file.h>
#include <sys/wait.h>
#include "linux/fh8626-isp.h"
#include <linux/v4l2-subdev.h>
#include <linux/v4l2-controls.h>
#include "fh8626-3ad.h"
#include "awb-controller.h"
#include "ae-controller.h"
#include "isp-tuning.h"

#ifndef V4L2_CID_IMAGE_PROC_CLASS_BASE
#define V4L2_CID_IMAGE_PROC_CLASS_BASE		(V4L2_CTRL_CLASS_IMAGE_PROC | 0x900)
#endif
#ifndef V4L2_CID_DIGITAL_GAIN
#define V4L2_CID_DIGITAL_GAIN			(V4L2_CID_IMAGE_PROC_CLASS_BASE + 5)
#endif

int g_verbose;
static volatile sig_atomic_t running = 1;

/*
 * Reserve stdio before opening devices or the readiness pipe, so background
 * redirection never replaces a device, lock or pipe descriptor.
 */
static int reserve_standard_fds(void)
{
	int fd;
	for (fd = STDIN_FILENO; fd <= STDERR_FILENO; fd++) {
		int nullfd;
		if (fcntl(fd, F_GETFD) >= 0)
			continue;
		if (errno != EBADF)
			return -1;
		nullfd = open("/dev/null", O_RDWR);
		if (nullfd < 0)
			return -1;
		if (nullfd != fd) {
			if (dup2(nullfd, fd) < 0) {
				int saved = errno;
				close(nullfd);
				errno = saved;
				return -1;
			}
			close(nullfd);
		}
	}
	return 0;
}

/* The lock follows the ISP device, including explicit -i aliases. */
static int lock_isp(int fd)
{
	struct stat st;
	char path[128];
	int lockfd;
	if (fstat(fd, &st) < 0 || !S_ISCHR(st.st_mode)) {
		errno = ENODEV;
		return -1;
	}
	snprintf(path, sizeof(path), "/var/run/fh8626-3ad.%u.%u.lock",
		 major(st.st_rdev), minor(st.st_rdev));
	lockfd = open(path, O_RDWR | O_CREAT | O_CLOEXEC, 0644);
	if (lockfd < 0)
		return -1;
	if (flock(lockfd, LOCK_EX | LOCK_NB) < 0) {
		int saved = errno;
		close(lockfd);
		errno = saved;
		return -1;
	}
	/* Never unlink a lock inode while another process can be opening it. */
	return lockfd;
}

static int write_pidfile(const char *path)
{
	FILE *file;
	int ret;
	if (!path[0])
		return 0;
	file = fopen(path, "w");
	if (!file)
		return -1;
	ret = fprintf(file, "%ld\n", (long)getpid()) < 0 ? -1 : 0;
	if (fclose(file))
		ret = -1;
	if (ret)
		unlink(path);
	return ret;
}

/* Called only after devices, controls and event subscription are ready. */
static int start_background(const char *pidfile)
{
	int ready[2], nullfd, logfd;
	pid_t child;
	char status = 0;
	ssize_t bytes;
	if (pipe(ready) < 0)
		return -1;
	fflush(NULL);
	child = fork();
	if (child < 0) {
		close(ready[0]);
		close(ready[1]);
		return -1;
	}
	if (child) {
		struct pollfd pfd = { .fd = ready[0], .events = POLLIN };
		int ret;
		close(ready[1]);
		do {
			ret = poll(&pfd, 1, 5000);
		} while (ret < 0 && errno == EINTR);
		if (ret <= 0) {
			/* No ready process may outlive a failed service start. */
			kill(child, SIGKILL);
			close(ready[0]);
			waitpid(child, NULL, WNOHANG);
			_exit(EXIT_FAILURE);
		}
		do {
			bytes = read(ready[0], &status, 1);
		} while (bytes < 0 && errno == EINTR);
		close(ready[0]);
		if (bytes != 1 || status != 1) {
			waitpid(child, NULL, WNOHANG);
			_exit(EXIT_FAILURE);
		}
		_exit(EXIT_SUCCESS);
	}
	close(ready[0]);
	if (setsid() < 0 || chdir("/") < 0)
		goto fail;
	nullfd = open("/dev/null", O_RDONLY);
	if (nullfd < 0)
		goto fail;
	logfd = open("/var/run/fh8626-3ad.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (logfd < 0) {
		close(nullfd);
		goto fail;
	}
	if (dup2(nullfd, STDIN_FILENO) < 0 || dup2(logfd, STDOUT_FILENO) < 0 ||
	    dup2(logfd, STDERR_FILENO) < 0) {
		close(nullfd);
		close(logfd);
		goto fail;
	}
	if (nullfd > STDERR_FILENO)
		close(nullfd);
	if (logfd > STDERR_FILENO)
		close(logfd);
	setvbuf(stdout, NULL, _IOLBF, 0);
	if (write_pidfile(pidfile) < 0)
		goto fail;
	status = 1;
	do {
		bytes = write(ready[1], &status, 1);
	} while (bytes < 0 && errno == EINTR);
	close(ready[1]);
	if (bytes != 1) {
		if (pidfile[0])
			unlink(pidfile);
		return -1;
	}
	return 0;
fail:
	close(ready[1]);
	return -1;
}

static void stop(int sig)
{
	(void)sig;
	running = 0;
}

static bool name_matches(const char *name, const char *driver)
{
	size_t n = strlen(driver);
	return !strncmp(name, driver, n) &&
		(!name[n] || name[n] == '\n' || name[n] == ' ');
}

static int discover_subdev(char *path, size_t size, const char *driver)
{
	glob_t paths = {0};
	unsigned int found = 0;
	size_t i;
	int ret = glob("/sys/class/video4linux/v4l-subdev*/name", 0, NULL, &paths);
	if (ret) {
		globfree(&paths);
		return -1;
	}
	for (i = 0; i < paths.gl_pathc; i++) {
		char name[128], node[128];
		FILE *file = fopen(paths.gl_pathv[i], "r");
		if (!file)
			continue;
		if (fgets(name, sizeof(name), file) &&
		    name_matches(name, driver) &&
		    sscanf(paths.gl_pathv[i], "/sys/class/video4linux/%127[^/]/name", node) == 1) {
			found++;
			snprintf(path, size, "/dev/%.110s", node);
		}
		fclose(file);
	}
	globfree(&paths);
	return found == 1 ? 0 : -1;
}

static const char * const s_supported_sensors[] = {
	"gc1054"
};

static int sensor_is_supported(int fd, char *detected_name, size_t max_len)
{
	struct stat st;
	char path[128], name[128];
	FILE *file;
	size_t i;

	if (fstat(fd, &st) < 0 || !S_ISCHR(st.st_mode)) {
		errno = ENODEV;
		return -1;
	}
	snprintf(path, sizeof(path), "/sys/dev/char/%u:%u/name",
		 major(st.st_rdev), minor(st.st_rdev));
	file = fopen(path, "r");
	if (!file)
		return -1;
	if (!fgets(name, sizeof(name), file)) {
		fclose(file);
		errno = ENODEV;
		return -1;
	}
	fclose(file);

	for (i = 0; i < sizeof(s_supported_sensors)/sizeof(s_supported_sensors[0]); i++) {
		if (name_matches(name, s_supported_sensors[i])) {
			if (detected_name && max_len > 0)
				snprintf(detected_name, max_len, "%s", s_supported_sensors[i]);
			return 0;
		}
	}
	errno = ENODEV;
	return -1;
}

static int discover_sensor_subdev(char *path, size_t size, char *driver_out, size_t driver_size)
{
	glob_t paths = {0};
	unsigned int found = 0;
	size_t i, s;
	if (glob("/sys/class/video4linux/v4l-subdev*/name", 0, NULL, &paths)) {
		globfree(&paths);
		return -1;
	}
	for (i = 0; i < paths.gl_pathc; i++) {
		char name[128], node[128];
		FILE *file = fopen(paths.gl_pathv[i], "r");
		if (!file)
			continue;
		if (fgets(name, sizeof(name), file)) {
			for (s = 0; s < sizeof(s_supported_sensors)/sizeof(s_supported_sensors[0]); s++) {
				if (!name_matches(name, s_supported_sensors[s]))
					continue;
				if (sscanf(paths.gl_pathv[i], "/sys/class/video4linux/%127[^/]/name", node) == 1) {
					found++;
					snprintf(path, size, "/dev/%.110s", node);
					if (driver_out && driver_size)
						snprintf(driver_out, driver_size, "%s", s_supported_sensors[s]);
				}
				break;
			}
		}
		fclose(file);
	}
	globfree(&paths);
	return found == 1 ? 0 : -1;
}

static int read_gains(int fd, struct awb_controller *awb)
{
	struct v4l2_ext_control values[3] = {
		{ .id = V4L2_CID_RED_BALANCE },
		{ .id = V4L2_CID_FH8626_WB_GAIN_G },
		{ .id = V4L2_CID_BLUE_BALANCE },
	};
	struct v4l2_ext_controls ctrls = {
		.ctrl_class = V4L2_CTRL_CLASS_USER, .count = 3, .controls = values,
	};
	unsigned int i;
	for (i = 0; i < 3; i++) {
		struct v4l2_queryctrl q = { .id = values[i].id };
		if (ioctl(fd, VIDIOC_QUERYCTRL, &q) < 0 ||
		    q.type != V4L2_CTRL_TYPE_INTEGER || q.step != 1 ||
		    q.minimum != AWB_MIN_GAIN_Q9 || q.maximum != AWB_MAX_GAIN_Q9 ||
		    (q.flags & (V4L2_CTRL_FLAG_DISABLED | V4L2_CTRL_FLAG_READ_ONLY |
				V4L2_CTRL_FLAG_INACTIVE))) {
			LOG_ERR("ISP control 0x%x does not expose the Q9 gain contract", q.id);
			errno = EPROTO;
			return -1;
		}
	}
	if (ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls) < 0)
		return -1;
	awb->current_gain_r = values[0].value;
	awb->current_gain_g = values[1].value;
	awb->current_gain_b = values[2].value;
	return 0;
}

static int write_gains(int fd, const struct awb_controller *awb)
{
	struct v4l2_ext_control values[3] = {
		{ .id = V4L2_CID_RED_BALANCE, .value = awb->current_gain_r },
		{ .id = V4L2_CID_FH8626_WB_GAIN_G, .value = awb->current_gain_g },
		{ .id = V4L2_CID_BLUE_BALANCE, .value = awb->current_gain_b },
	};
	struct v4l2_ext_controls ctrls = {
		.ctrl_class = V4L2_CTRL_CLASS_USER, .count = 3, .controls = values,
	};
	return ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
}

static unsigned int popcount32(uint32_t v)
{
	v = v - ((v >> 1) & 0x55555555U);
	v = (v & 0x33333333U) + ((v >> 2) & 0x33333333U);
	return (((v + (v >> 4)) & 0x0f0f0f0fU) * 0x01010101U) >> 24;
}

/*
 * Read the zone statistics and reduce them to channel means. Returns 1 for
 * a valid frame, 0 when the ISP has none, -1 on error.
 */
static int read_grid(int fd, struct fh8626_isp_grid_stats *raw, struct fh8626_grid *grid)
{
	struct v4l2_ext_control value = {
		.id = V4L2_CID_FH8626_GRID_STATS, .size = sizeof(*raw), .ptr = raw,
	};
	struct v4l2_ext_controls ctrls = {
		.ctrl_class = V4L2_CTRL_CLASS_USER, .count = 1, .controls = &value,
	};
	unsigned int i;

	if (ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls) < 0)
		return -1;
	if (value.size != sizeof(*raw) || raw->version != FH8626_ISP_STATS_VERSION ||
	    raw->cols != GRID_COLS || raw->rows != GRID_ROWS) {
		errno = EPROTO;
		return -1;
	}
	if (!(raw->flags & FH8626_ISP_STATS_VALID))
		return 0;
	grid->sequence = raw->sequence;
	grid->timestamp_ns = ((uint64_t)raw->timestamp_ns_high << 32) | raw->timestamp_ns_low;
	for (i = 0; i < GRID_CELLS; i++) {
		uint32_t count = raw->cells[i].count, sum;
		uint32_t m[FH8626_ISP_GRID_CHANNELS];
		unsigned int c;

		/* sum[c] * 4 / count is the mean of colour c; Q4. */
		for (c = 0; c < FH8626_ISP_GRID_CHANNELS; c++) {
			sum = raw->cells[i].sum[c];
			if (!count)
				m[c] = 0;
			else if (sum < 1U << 26)
				m[c] = (sum << 6) / count;
			else if (((uint64_t)sum << 6) / count > GRID_MEAN_MAX)
				m[c] = GRID_MEAN_MAX;
			else
				m[c] = (uint32_t)(((uint64_t)sum << 6) / count);
			if (m[c] > GRID_MEAN_MAX)
				m[c] = GRID_MEAN_MAX;
		}
		grid->r[i] = (uint16_t)m[0];
		grid->g[i] = (uint16_t)((m[1] + m[2]) / 2);
		grid->b[i] = (uint16_t)m[3];
		grid->max[i] = raw->cells[i].max > GRID_MEAN_MAX ? GRID_MEAN_MAX :
			       (uint16_t)raw->cells[i].max;
	}
	grid->motion_valid = raw->flags & FH8626_ISP_GRID_MOTION_VALID;
	grid->motion_total = raw->motion_cols * raw->motion_rows;
	grid->motion_blocks = 0;
	if (grid->motion_valid)
		for (i = 0; i < FH8626_ISP_MOTION_WORDS; i++)
			grid->motion_blocks += popcount32(raw->motion[i]);
	return 1;
}

/*
 * Scene motion for the noise filters, 0..256.
 *
 * Two measurements, the larger wins: the share of the ISP's motion blocks
 * that are set, and the share of zone cells whose green mean changed by
 * more than its noise after removing the global level change. A frame in
 * which an exposure or white-balance change first shows sets every motion
 * block, so it is skipped; a frame in which nearly all blocks are set while
 * few cells changed shape is a global change too. Three times the share,
 * capped, is the motion level: a third of the picture moving is the most.
 * The level rises at once and falls by an eighth per frame.
 */
struct motion_state {
	uint16_t prev[GRID_CELLS];
	bool have_prev, floor_valid;
	uint32_t level, raw, md, cells;
	uint32_t md_mean, md_dev;	/* Q4 */
	uint32_t md_last, floor_gain;
};

/*
 * At high gain noise alone sets motion blocks, and in bursts. Learn the
 * block share that noise gives, as a running mean and mean deviation
 * (1/64 per frame, restarted when the gain changes by more than 2x), and
 * count only the excess over mean + 3 deviations that two frames in a row
 * show: moving objects persist, noise bursts do not. Motion that lasts
 * longer than a few seconds is slowly learned as noise.
 */
static uint32_t md_excess(struct motion_state *ms, uint32_t md, uint32_t gain)
{
	uint32_t thr, excess, d;

	if (!ms->floor_valid || gain > ms->floor_gain * 2 || gain * 2 < ms->floor_gain) {
		ms->md_mean = md * 16;
		ms->md_dev = 0;
		ms->md_last = 0;
		ms->floor_gain = gain;
		ms->floor_valid = true;
		return 0;
	}
	thr = (ms->md_mean + 3 * ms->md_dev) / 16 + 4;
	excess = md > thr ? md - thr : 0;
	d = (uint32_t)abs_i32((int32_t)(md * 16) - (int32_t)ms->md_mean);
	ms->md_mean = (uint32_t)((int32_t)ms->md_mean +
				 ((int32_t)(md * 16) - (int32_t)ms->md_mean) / 64);
	ms->md_dev = (uint32_t)((int32_t)ms->md_dev + ((int32_t)d - (int32_t)ms->md_dev) / 64);
	d = excess < ms->md_last ? excess : ms->md_last;
	ms->md_last = excess;
	return d;
}

static uint32_t motion_update(struct motion_state *ms, const struct fh8626_grid *grid,
			      uint32_t total_gain, bool masked)
{
	uint64_t cur = 0, prev = 0;
	uint32_t ratio, md = 0, cells = 0, m;
	unsigned int i;

	for (i = 0; i < GRID_CELLS; i++) {
		cur += grid->g[i];
		prev += ms->prev[i];
	}
	if (ms->have_prev && !masked && prev && cur) {
		ratio = (uint32_t)((cur << 8) / prev);
		for (i = 0; i < GRID_CELLS; i++) {
			int32_t pred = (int32_t)(((uint64_t)ms->prev[i] * ratio) >> 8);
			int32_t thr = grid->g[i] / 16 + 32 + (int32_t)(total_gain / 64);

			cells += abs_i32((int32_t)grid->g[i] - pred) > thr;
		}
		if (grid->motion_valid && grid->motion_total) {
			md = grid->motion_blocks * 256 / grid->motion_total;
			if (md >= 230 && cells < 64) {
				md = 0;
			} else {
				md = md_excess(ms, md, total_gain);
			}
		}
		ms->md = md;
		ms->cells = cells;
		m = md > cells ? md : cells;
		m = m * 3 > 256 ? 256 : m * 3;
		ms->raw = m;
		if (m > ms->level)
			ms->level = m;
		else
			ms->level -= (ms->level - m + 7) / 8;
	}
	memcpy(ms->prev, grid->g, sizeof(ms->prev));
	ms->have_prev = true;
	return ms->level;
}

static int read_sensor_controls(int fd, struct ae_controller *ae)
{
	const uint32_t ids[2] = { V4L2_CID_EXPOSURE, V4L2_CID_ANALOGUE_GAIN };
	struct ae_control_range ranges[2];
	uint32_t values[2];
	unsigned int i;
	for (i = 0; i < 2; i++) {
		struct v4l2_queryctrl q = { .id = ids[i] };
		struct v4l2_control c = { .id = ids[i] };
		if (ioctl(fd, VIDIOC_QUERYCTRL, &q) < 0)
			return -1;
		if (q.type != V4L2_CTRL_TYPE_INTEGER || q.minimum < 1 ||
		    q.maximum < q.minimum || q.step < 1 ||
		    (q.flags & (V4L2_CTRL_FLAG_DISABLED | V4L2_CTRL_FLAG_READ_ONLY |
				V4L2_CTRL_FLAG_WRITE_ONLY))) {
			errno = EPROTO;
			return -1;
		}
		if (q.flags & (V4L2_CTRL_FLAG_GRABBED | V4L2_CTRL_FLAG_INACTIVE)) {
			errno = EBUSY;
			return 1;
		}
		if (ioctl(fd, VIDIOC_G_CTRL, &c) < 0)
			return -1;
		if (c.value < q.minimum || c.value > q.maximum ||
		    (c.value - q.minimum) % q.step) {
			errno = EPROTO;
			return -1;
		}
		ranges[i] = (struct ae_control_range){ q.minimum, q.maximum, q.step };
		values[i] = c.value;
	}
	ae->exposure = ranges[0];
	ae->gain = ranges[1];
	ae->current_exposure_lines = values[0];
	ae->current_analog_gain = values[1];
	return 0;
}

static int write_control(int fd, uint32_t id, uint32_t value)
{
	struct v4l2_control c = { .id = id, .value = (int32_t)value };

	return ioctl(fd, VIDIOC_S_CTRL, &c);
}

static int read_digital_gain(int isp_fd, struct ae_controller *ae)
{
	struct v4l2_control c = { .id = V4L2_CID_DIGITAL_GAIN };

	if (ae->digital_gain.max <= ae->digital_gain.min)
		return 0;
	if (ioctl(isp_fd, VIDIOC_G_CTRL, &c) < 0)
		return -1;
	ae_controller_set_current_digital_gain(ae, c.value);
	return 0;
}

/*
 * Frame length control. The nominal frame is the VBLANK found at start-up
 * or last set by another client; slow shutter lengthens the frame from it
 * to the frame that gives min_fps and gives it back when the light returns.
 */
struct frame_control {
	uint32_t min_fps_milli;		/* slow shutter floor, 0 disables it */
	bool writable, nominal_valid;
	int32_t vblank, nominal_vblank, vblank_max;
	uint32_t height;
};

/*
 * Line time from the sensor's pixel rate and horizontal blanking:
 * (width + HBLANK) / PIXEL_RATE. Frame lines are height + VBLANK. Without
 * PIXEL_RATE/HBLANK the frame interval divided by the frame lines is used.
 * The exposure limit's distance from the frame length is the margin the
 * integration keeps.
 */
static bool read_sensor_timing(int fd, struct ae_config *cfg, struct frame_control *fc)
{
	struct v4l2_subdev_format format = { .which = V4L2_SUBDEV_FORMAT_ACTIVE, .pad = 0 };
	struct v4l2_subdev_frame_interval interval = { .pad = 0 };
	struct v4l2_queryctrl qblank = { .id = V4L2_CID_VBLANK };
	struct v4l2_queryctrl qexp = { .id = V4L2_CID_EXPOSURE };
	struct v4l2_control blank = { .id = V4L2_CID_VBLANK };
	struct v4l2_ext_control rate[2] = {
		{ .id = V4L2_CID_PIXEL_RATE }, { .id = V4L2_CID_HBLANK },
	};
	struct v4l2_ext_controls rates = { .count = 2, .controls = rate };
	uint64_t lines, period, line_ns = 0, longest = 0;

	cfg->line_time_ns = cfg->frame_lines = 0;
	cfg->nominal_frame_lines = cfg->max_frame_lines = 0;
	fc->writable = false;
	if (ioctl(fd, VIDIOC_SUBDEV_G_FMT, &format) < 0 ||
	    ioctl(fd, VIDIOC_QUERYCTRL, &qblank) < 0 ||
	    ioctl(fd, VIDIOC_G_CTRL, &blank) < 0 || blank.value < 0 ||
	    !format.format.height || !format.format.width)
		return false;
	lines = (uint64_t)format.format.height + blank.value;
	if (ioctl(fd, VIDIOC_G_EXT_CTRLS, &rates) == 0 && rate[0].value64 > 0 &&
	    rate[1].value >= 0) {
		uint64_t line_px = (uint64_t)format.format.width + rate[1].value;

		line_ns = line_px * 1000000000ULL / (uint64_t)rate[0].value64;
		if (fc->min_fps_milli)
			longest = (uint64_t)rate[0].value64 * 1000 / (line_px * fc->min_fps_milli);
	} else if (ioctl(fd, VIDIOC_SUBDEV_G_FRAME_INTERVAL, &interval) == 0 &&
		   interval.interval.numerator && interval.interval.denominator) {
		period = 1000000000ULL * interval.interval.numerator / interval.interval.denominator;
		line_ns = (period + lines / 2) / lines;
		if (fc->min_fps_milli && line_ns)
			longest = 1000000000000ULL / (line_ns * fc->min_fps_milli);
	}
	if (!line_ns || line_ns > UINT32_MAX || lines > UINT32_MAX)
		return false;
	cfg->line_time_ns = line_ns;
	cfg->frame_lines = lines;

	fc->height = format.format.height;
	fc->vblank = blank.value;
	fc->vblank_max = qblank.maximum;
	if (!fc->nominal_valid) {
		fc->nominal_vblank = blank.value;
		fc->nominal_valid = true;
	}
	fc->nominal_vblank = clamp_i32(fc->nominal_vblank, qblank.minimum, qblank.maximum);
	cfg->nominal_frame_lines = fc->height + fc->nominal_vblank;
	fc->writable = qblank.type == V4L2_CTRL_TYPE_INTEGER &&
		       !(qblank.flags & (V4L2_CTRL_FLAG_DISABLED | V4L2_CTRL_FLAG_READ_ONLY |
					 V4L2_CTRL_FLAG_GRABBED | V4L2_CTRL_FLAG_INACTIVE));
	/* Slow shutter needs an exposure limit that follows the frame length. */
	if (ioctl(fd, VIDIOC_QUERYCTRL, &qexp) == 0 && qexp.maximum > 0 &&
	    lines > (uint64_t)qexp.maximum && lines - qexp.maximum < 64)
		cfg->exposure_margin = lines - qexp.maximum;
	else
		fc->writable = false;
	if (fc->writable && longest) {
		if (longest > (uint64_t)fc->height + qblank.maximum)
			longest = (uint64_t)fc->height + qblank.maximum;
		if (longest > cfg->nominal_frame_lines)
			cfg->max_frame_lines = longest;
	}
	return true;
}

static int write_vblank(int fd, struct ae_controller *ae, struct frame_control *fc,
			uint32_t frame_lines)
{
	int32_t vblank = (int32_t)(frame_lines - fc->height);
	struct v4l2_control c = { .id = V4L2_CID_VBLANK, .value = vblank };

	if (vblank == fc->vblank)
		return 0;
	if (ioctl(fd, VIDIOC_S_CTRL, &c) < 0)
		return -1;
	fc->vblank = c.value;
	ae->config.frame_lines = fc->height + c.value;
	return 0;
}

/*
 * Control events from the sensor. Writes on our own file handle are not
 * reported back, so a value change is another client's write, or the driver
 * clamping the integration into a shorter frame.
 */
struct sensor_events {
	bool timing, external_frame, controls, power_line;
};

static const uint32_t s_sensor_event_ids[] = {
	V4L2_CID_VBLANK, V4L2_CID_HBLANK, V4L2_CID_PIXEL_RATE,
	V4L2_CID_EXPOSURE, V4L2_CID_ANALOGUE_GAIN,
};

static bool subscribe_sensor_events(int fd)
{
	size_t i;

	for (i = 0; i < sizeof(s_sensor_event_ids) / sizeof(s_sensor_event_ids[0]); i++) {
		struct v4l2_event_subscription sub = {
			.type = V4L2_EVENT_CTRL, .id = s_sensor_event_ids[i],
		};

		/* PIXEL_RATE and HBLANK are optional. */
		if (ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &sub) < 0 &&
		    (sub.id == V4L2_CID_VBLANK || sub.id == V4L2_CID_EXPOSURE ||
		     sub.id == V4L2_CID_ANALOGUE_GAIN)) {
			struct v4l2_event_subscription all = { .type = V4L2_EVENT_ALL };

			ioctl(fd, VIDIOC_UNSUBSCRIBE_EVENT, &all);
			return false;
		}
	}
	return true;
}

static int read_sensor_events(int fd, struct sensor_events *ev, struct frame_control *fc,
			      struct ae_controller *ae)
{
	struct v4l2_event event;

	for (;;) {
		uint32_t changes;

		if (ioctl(fd, VIDIOC_DQEVENT, &event) < 0) {
			if (errno == EINTR)
				continue;
			return errno == ENOENT || errno == EAGAIN ? 0 : -1;
		}
		if (event.type != V4L2_EVENT_CTRL)
			continue;
		changes = event.u.ctrl.changes;
		switch (event.id) {
		case V4L2_CID_VBLANK:
			if (changes & V4L2_EVENT_CTRL_CH_VALUE) {
				fc->nominal_vblank = event.u.ctrl.value;
				ev->external_frame = true;
			}
			ev->timing = true;
			break;
		case V4L2_CID_HBLANK:
		case V4L2_CID_PIXEL_RATE:
			ev->timing = true;
			break;
		case V4L2_CID_EXPOSURE:
			/* The range follows the frame length. */
			if ((changes & V4L2_EVENT_CTRL_CH_RANGE) && event.u.ctrl.minimum >= 1 &&
			    event.u.ctrl.maximum >= event.u.ctrl.minimum && event.u.ctrl.step >= 1)
				ae->exposure = (struct ae_control_range){
					(uint32_t)event.u.ctrl.minimum,
					(uint32_t)event.u.ctrl.maximum,
					(uint32_t)event.u.ctrl.step,
				};
			/* fall through */
		case V4L2_CID_ANALOGUE_GAIN:
			if (changes & (V4L2_EVENT_CTRL_CH_VALUE | V4L2_EVENT_CTRL_CH_FLAGS))
				ev->controls = true;
			break;
		}
	}
}

/* Mains frequency is a standard ISP control so other clients can set it. */
static void read_power_line(int isp_fd, struct ae_config *cfg)
{
	struct v4l2_control c = { .id = V4L2_CID_POWER_LINE_FREQUENCY };

	if (ioctl(isp_fd, VIDIOC_G_CTRL, &c) < 0)
		return;
	cfg->flicker_mode = c.value == V4L2_CID_POWER_LINE_FREQUENCY_50HZ ? AE_ANTIFLICKER_50HZ :
			    c.value == V4L2_CID_POWER_LINE_FREQUENCY_60HZ ? AE_ANTIFLICKER_60HZ :
			    AE_ANTIFLICKER_OFF;
}

static int write_power_line(int isp_fd, ae_antiflicker_mode_t mode)
{
	struct v4l2_control c = {
		.id = V4L2_CID_POWER_LINE_FREQUENCY,
		.value = mode == AE_ANTIFLICKER_50HZ ? V4L2_CID_POWER_LINE_FREQUENCY_50HZ :
			 mode == AE_ANTIFLICKER_60HZ ? V4L2_CID_POWER_LINE_FREQUENCY_60HZ :
			 V4L2_CID_POWER_LINE_FREQUENCY_DISABLED,
	};

	return ioctl(isp_fd, VIDIOC_S_CTRL, &c);
}

static void init_isp_digital_gain(int isp_fd, struct ae_controller *ae)
{
	struct v4l2_queryctrl q = { .id = V4L2_CID_DIGITAL_GAIN };
	struct v4l2_control c = { .id = V4L2_CID_DIGITAL_GAIN };

	if (ioctl(isp_fd, VIDIOC_QUERYCTRL, &q) == 0 &&
	    q.type == V4L2_CTRL_TYPE_INTEGER && q.minimum >= 1 &&
	    q.maximum >= q.minimum &&
	    !(q.flags & (V4L2_CTRL_FLAG_DISABLED | V4L2_CTRL_FLAG_READ_ONLY))) {
		if (ioctl(isp_fd, VIDIOC_G_CTRL, &c) == 0) {
			ae_controller_set_digital_gain_range(ae, q.minimum, q.maximum,
							    q.step ? q.step : 1);
			ae_controller_set_current_digital_gain(ae, c.value);
			LOG_INFO("ISP digital gain available: [%u..%u] step %u, current %u (Q6)",
				 q.minimum, q.maximum, q.step ? q.step : 1, c.value);
			return;
		}
	}
	ae_controller_set_digital_gain_range(ae, 64, 64, 1);
	ae_controller_set_current_digital_gain(ae, 64);
}

#ifndef V4L2_EXPOSURE_AUTO
#define V4L2_EXPOSURE_AUTO 0
#endif
#ifndef V4L2_EXPOSURE_MANUAL
#define V4L2_EXPOSURE_MANUAL 1
#endif

/*
 * The command line grants loop ownership; the V4L2 auto controls can only
 * pause an owned loop. A loop disabled with -A off or -W off never starts,
 * whatever the controls say.
 */
static bool check_exposure_auto(int isp_fd, int sensor_fd, bool cli_enabled)
{
	static bool s_ae_auto = true;
	struct v4l2_control ctrl = { .id = V4L2_CID_EXPOSURE_AUTO };
	bool new_state = s_ae_auto;
	bool queried = false;

	if (!cli_enabled)
		return false;

	if (isp_fd >= 0 && ioctl(isp_fd, VIDIOC_G_CTRL, &ctrl) == 0) {
		new_state = (ctrl.value != V4L2_EXPOSURE_MANUAL);
		queried = true;
	} else if (sensor_fd >= 0 && ioctl(sensor_fd, VIDIOC_G_CTRL, &ctrl) == 0) {
		new_state = (ctrl.value != V4L2_EXPOSURE_MANUAL);
		queried = true;
	}

	if (queried && new_state != s_ae_auto) {
		LOG_INFO("AE mode switched to %s via V4L2 control", new_state ? "AUTO" : "MANUAL");
		s_ae_auto = new_state;
	}

	return s_ae_auto;
}

static bool check_awb_auto(int isp_fd, bool cli_enabled)
{
	static bool s_awb_auto = true;
	struct v4l2_control ctrl = { .id = V4L2_CID_AUTO_WHITE_BALANCE };
	bool new_state = s_awb_auto;
	bool queried = false;

	if (!cli_enabled)
		return false;

	if (isp_fd >= 0 && ioctl(isp_fd, VIDIOC_G_CTRL, &ctrl) == 0) {
		new_state = (ctrl.value != 0);
		queried = true;
	}

	if (queried && new_state != s_awb_auto) {
		LOG_INFO("AWB mode switched to %s via V4L2 control", new_state ? "AUTO" : "MANUAL");
		s_awb_auto = new_state;
	}

	return s_awb_auto;
}

/*
 * Per-frame AE state of the daemon. A new exposure is written as soon as it
 * is decided; each gain follows as many frames later as it applies sooner
 * than the integration, so all of them first show in the same statistics
 * frame.
 *
 * Frame length: a longer frame is written one frame before the integration
 * that needs it, so the sensor never sees an integration longer than its
 * frame. A shorter frame is written only once every integration commanded
 * since has shown in the statistics, so it is already in the sensor.
 */
struct ae_loop {
	bool sync_needed, gain_pending, dgain_pending, exposure_pending;
	bool sensor_events, isp_events;
	uint32_t pending_exposure, pending_again, pending_dgain;
	unsigned int gain_wait, dgain_wait, hold;
	uint32_t frames, period_us;
	uint64_t last_timestamp;	/* of the statistics frame in hand */
	struct frame_control frame;
	struct sensor_events ev;
};

static bool timing_required(const struct ae_config *cfg)
{
	return cfg->flicker_mode != AE_ANTIFLICKER_OFF || cfg->profile == AE_PROFILE_MOTION;
}

/* Frame rate in 1/100 frames per second. */
static uint32_t frame_rate_centi(const struct ae_config *cfg, uint32_t lines)
{
	uint64_t ns = (uint64_t)cfg->line_time_ns * lines;

	return ns ? (uint32_t)(100000000000ULL / ns) : 0;
}

static void log_timing(const struct ae_config *cfg)
{
	uint32_t cur = frame_rate_centi(cfg, cfg->frame_lines);
	uint32_t nom = frame_rate_centi(cfg, cfg->nominal_frame_lines);
	uint32_t min = frame_rate_centi(cfg, cfg->max_frame_lines);

	LOG_INFO("Sensor timing: line=%uns frame=%u lines (%u.%02u fps), nominal %u lines (%u.%02u fps), slow shutter %s%u lines (%u.%02u fps), margin %u",
		 cfg->line_time_ns, cfg->frame_lines, cur / 100, cur % 100,
		 cfg->nominal_frame_lines, nom / 100, nom % 100,
		 cfg->max_frame_lines > cfg->nominal_frame_lines ? "to " : "off, ",
		 cfg->max_frame_lines, min / 100, min % 100, cfg->exposure_margin);
}

static int set_frame_lines(int sensor_fd, struct ae_loop *loop, struct ae_controller *ae,
			   uint32_t lines, uint32_t sequence)
{
	uint32_t old = ae->config.frame_lines, fps;

	if (write_vblank(sensor_fd, ae, &loop->frame, lines) < 0) {
		LOG_ERR("AE frame length %u lines: %s", lines, strerror(errno));
		return -1;
	}
	if (ae->config.frame_lines != old) {
		fps = frame_rate_centi(&ae->config, ae->config.frame_lines);
		LOG_INFO("AE frame=%u frame length %u -> %u lines (%u.%02u fps)%s",
			 sequence, old, ae->config.frame_lines, fps / 100, fps % 100,
			 ae->slow ? " slow shutter" : "");
	}
	return 0;
}

static int write_again(int sensor_fd, struct ae_loop *loop)
{
	loop->gain_pending = false;
	if (write_control(sensor_fd, V4L2_CID_ANALOGUE_GAIN, loop->pending_again) < 0) {
		LOG_ERR("AE analogue gain %u: %s", loop->pending_again, strerror(errno));
		return -1;
	}
	return 0;
}

static int write_dgain(int isp_fd, struct ae_loop *loop, const struct ae_controller *ae)
{
	loop->dgain_pending = false;
	if (ae->digital_gain.max > ae->digital_gain.min &&
	    write_control(isp_fd, V4L2_CID_DIGITAL_GAIN, loop->pending_dgain) < 0) {
		LOG_ERR("AE ISP digital gain %u: %s", loop->pending_dgain, strerror(errno));
		return -1;
	}
	return 0;
}

/* Once per frame: the staggered gain writes that are due. */
static int write_pending_gains(int isp_fd, int sensor_fd, struct ae_loop *loop,
			       struct ae_controller *ae)
{
	if (loop->gain_pending && !(loop->gain_wait && --loop->gain_wait) &&
	    write_again(sensor_fd, loop) < 0)
		return -1;
	if (loop->dgain_pending && !(loop->dgain_wait && --loop->dgain_wait) &&
	    write_dgain(isp_fd, loop, ae) < 0)
		return -1;
	return 0;
}

static int resync_ae(int isp_fd, int sensor_fd, struct ae_loop *loop,
		     struct ae_controller *ae, uint32_t sequence)
{
	int ret = read_sensor_controls(sensor_fd, ae);

	if (ret)
		return ret;
	if (read_digital_gain(isp_fd, ae) < 0)
		return -1;
	read_power_line(isp_fd, &ae->config);
	if (!read_sensor_timing(sensor_fd, &ae->config, &loop->frame) &&
	    timing_required(&ae->config)) {
		errno = EOPNOTSUPP;
		return -1;
	}
	loop->gain_pending = loop->dgain_pending = loop->exposure_pending = false;
	loop->ev = (struct sensor_events){ 0 };
	ae_controller_sync(ae, sequence);
	return 0;
}

/* Write the decided targets: integration now, gains staggered. */
static int apply_ae(int isp_fd, int sensor_fd, struct ae_loop *loop, struct ae_controller *ae,
		    uint32_t sequence)
{
	uint32_t limit = ae->config.frame_lines - ae->config.exposure_margin;
	struct timespec now;
	uint64_t now_ns;

	/*
	 * The delays hold for writes early in the frame; report how long after
	 * the statistics frame the integration goes out.
	 */
	clock_gettime(CLOCK_MONOTONIC, &now);
	now_ns = (uint64_t)now.tv_sec * 1000000000ULL + (uint64_t)now.tv_nsec;

	/* Should the frame length have been refused, stay inside the frame. */
	if (ae_controller_slow_shutter(ae) && ae->target_exposure_lines > limit)
		ae->target_exposure_lines = limit;
	if (ae->target_exposure_lines != ae->current_exposure_lines &&
	    write_control(sensor_fd, V4L2_CID_EXPOSURE, ae->target_exposure_lines) < 0) {
		LOG_ERR("AE exposure %u: %s", ae->target_exposure_lines, strerror(errno));
		return -1;
	}
	/* Each gain is written so that it shows together with the integration. */
	if (ae->target_analog_gain != ae->current_analog_gain) {
		loop->gain_pending = true;
		loop->gain_wait = ae->config.exposure_delay - ae->config.gain_delay;
		loop->pending_again = ae->target_analog_gain;
		if (!loop->gain_wait && write_again(sensor_fd, loop) < 0)
			return -1;
	}
	if (ae->target_digital_gain != ae->current_digital_gain) {
		loop->dgain_pending = true;
		loop->dgain_wait = ae->config.exposure_delay - ae->config.digital_gain_delay;
		loop->pending_dgain = ae->target_digital_gain;
		if (!loop->dgain_wait && write_dgain(isp_fd, loop, ae) < 0)
			return -1;
	}
	ae_controller_commit(ae, sequence);
	LOG_INFO("AE frame=%u mean=%u target=%u err=%d -> exp=%u gain=%u dgain=%u frame=%u period=%uus lag=%uus",
		 sequence, ae->mean >> 4, ae->target >> 4, ae->error,
		 ae->current_exposure_lines, ae->current_analog_gain,
		 ae->current_digital_gain, ae->config.frame_lines, loop->period_us,
		 now_ns > loop->last_timestamp ? (uint32_t)((now_ns - loop->last_timestamp) / 1000) : 0);
	return 0;
}

/*
 * Timing and ownership changes reported by control events. Returns 1 when
 * the controller restarted from the controls and waits for this frame.
 */
static int handle_ctrl_events(int isp_fd, int sensor_fd, struct ae_loop *loop,
			      struct ae_controller *ae, uint32_t sequence)
{
	struct sensor_events ev = loop->ev;
	int ret;

	loop->ev = (struct sensor_events){ 0 };
	if (ev.power_line)
		read_power_line(isp_fd, &ae->config);
	if (ev.external_frame || ev.controls) {
		/* Another client's write breaks the history: start from the controls. */
		ret = resync_ae(isp_fd, sensor_fd, loop, ae, sequence);
		if (ret)
			return ret;
		LOG_INFO("AE frame=%u controls changed elsewhere: exp=%u gain=%u dgain=%u vblank=%d",
			 sequence, ae->current_exposure_lines, ae->current_analog_gain,
			 ae->current_digital_gain, loop->frame.vblank);
		if (ev.external_frame)
			log_timing(&ae->config);
		/* Until the new values show in the statistics. */
		loop->hold = ae->config.exposure_delay;
		return 1;
	}
	if (ev.timing) {
		if (!read_sensor_timing(sensor_fd, &ae->config, &loop->frame) &&
		    timing_required(&ae->config)) {
			errno = EOPNOTSUPP;
			return -1;
		}
		log_timing(&ae->config);
	}
	return 0;
}

static int process_ae(int isp_fd, int sensor_fd, struct ae_loop *loop,
		      struct ae_controller *ae, const struct fh8626_grid *grid,
		      const uint32_t wb[3], bool ae_cli_enabled)
{
	uint32_t old_exp = ae->current_exposure_lines;
	uint32_t old_again = ae->current_analog_gain;
	uint32_t old_dgain = ae->current_digital_gain;
	uint32_t lines;
	int ret;

	loop->frames++;
	if (!check_exposure_auto(isp_fd, sensor_fd, ae_cli_enabled)) {
		/* Manual: follow the controls for the tuning, resync on return. */
		loop->sync_needed = true;
		loop->gain_pending = loop->dgain_pending = loop->exposure_pending = false;
		if (!(loop->frames % 8)) {
			read_sensor_controls(sensor_fd, ae);
			read_digital_gain(isp_fd, ae);
		}
		return 0;
	}
	if (write_pending_gains(isp_fd, sensor_fd, loop, ae) < 0)
		return -1;
	if (loop->sync_needed) {
		ret = resync_ae(isp_fd, sensor_fd, loop, ae, grid->sequence);
		if (ret)
			return ret < 0 ? -1 : 0;
		loop->sync_needed = false;
		LOG_INFO("AE sync frame=%u exp=%u gain=%u dgain=%u",
			 grid->sequence, ae->current_exposure_lines,
			 ae->current_analog_gain, ae->current_digital_gain);
		log_timing(&ae->config);
		return 0;
	}
	ret = handle_ctrl_events(isp_fd, sensor_fd, loop, ae, grid->sequence);
	if (ret)
		return ret < 0 ? -1 : 0;
	/*
	 * Without control events, mode and timing changes are polled through
	 * the standard controls.
	 */
	if (!(loop->frames % 16)) {
		if (!loop->isp_events)
			read_power_line(isp_fd, &ae->config);
		if (!loop->sensor_events &&
		    !read_sensor_timing(sensor_fd, &ae->config, &loop->frame) &&
		    timing_required(&ae->config)) {
			errno = EOPNOTSUPP;
			return -1;
		}
	}
	/* With a stagger of more than one frame, finish the last decision. */
	if (loop->gain_pending || loop->dgain_pending)
		return 0;
	/* The longer frame went out last frame; now the integration follows. */
	if (loop->exposure_pending) {
		loop->exposure_pending = false;
		ae->target_exposure_lines = loop->pending_exposure;
		ae->target_analog_gain = loop->pending_again;
		ae->target_digital_gain = loop->pending_dgain;
		return apply_ae(isp_fd, sensor_fd, loop, ae, grid->sequence);
	}
	if (!(loop->frames % 4)) {
		if (!loop->sensor_events) {
			ret = read_sensor_controls(sensor_fd, ae);
			if (ret)
				return ret < 0 ? -1 : 0;
		}
		if (!loop->isp_events && read_digital_gain(isp_fd, ae) < 0)
			return -1;
	}
	if (ae->current_exposure_lines != old_exp || ae->current_analog_gain != old_again ||
	    ae->current_digital_gain != old_dgain) {
		LOG_INFO("AE frame=%u controls changed elsewhere: exp=%u gain=%u dgain=%u",
			 grid->sequence, ae->current_exposure_lines,
			 ae->current_analog_gain, ae->current_digital_gain);
		ae_controller_sync(ae, grid->sequence);
		/* Until the new values show in the statistics. */
		loop->hold = ae->config.exposure_delay;
		return 0;
	}
	if (loop->hold) {
		loop->hold--;
		return 0;
	}
	/* Shorten the frame once no integration in effect or on its way needs it. */
	lines = ae_controller_frame_lines(ae, grid->sequence);
	if (lines && lines < ae->config.frame_lines && loop->frame.writable &&
	    set_frame_lines(sensor_fd, loop, ae, lines, grid->sequence) < 0)
		return -1;
	if (!ae_controller_update(ae, grid, wb)) {
		LOG_DBG("AE frame=%u mean=%u target=%u p10=%u hi=%u clip=%u err=%d %s exp=%u gain=%u dgain=%u frame=%u period=%uus",
			grid->sequence, ae->mean >> 4, ae->target >> 4, ae->shadow >> 4,
			ae->highlight, ae->clipped, ae->error,
			ae->stable ? "stable" : ae->saturated ? "limit" : "hold",
			ae->current_exposure_lines, ae->current_analog_gain,
			ae->current_digital_gain, ae->config.frame_lines, loop->period_us);
		return 0;
	}
	/* A longer integration needs the longer frame first. */
	lines = ae_controller_target_frame_lines(ae);
	if (lines > ae->config.frame_lines && loop->frame.writable) {
		if (set_frame_lines(sensor_fd, loop, ae, lines, grid->sequence) < 0)
			return -1;
		loop->exposure_pending = true;
		loop->pending_exposure = ae->target_exposure_lines;
		loop->pending_again = ae->target_analog_gain;
		loop->pending_dgain = ae->target_digital_gain;
		return 0;
	}
	return apply_ae(isp_fd, sensor_fd, loop, ae, grid->sequence);
}

static void process_awb(int isp_fd, struct awb_controller *awb, const struct fh8626_grid *grid,
			bool *sync_needed, bool *changed)
{
	*changed = false;
	if (*sync_needed) {
		if (read_gains(isp_fd, awb) < 0)
			return;
		awb_controller_sync(awb);
		*sync_needed = false;
	}
	if (!awb_controller_update(awb, grid)) {
		LOG_DBG("AWB frame=%u grey=%u edges=%u conf=%u meas=%d,%d wp=%d,%d gains=%u/%u/%u",
			grid->sequence, awb->grey_cells, awb->edge_pairs, awb->confidence,
			awb->meas_x, awb->meas_y, awb->wp_x, awb->wp_y,
			awb->current_gain_r, awb->current_gain_g, awb->current_gain_b);
		return;
	}
	*changed = true;
}

int main(int argc, char **argv)
{
	char path[128] = "", sensor_path[128] = "", pidfile[256] = "";
	struct awb_config awb_cfg;
	struct awb_controller awb;
	struct ae_config ae_cfg;
	struct ae_controller ae;
	struct v4l2_event_subscription subscription = { .type = V4L2_EVENT_FRAME_SYNC };
	struct sigaction action = { .sa_handler = stop };
	static struct fh8626_isp_grid_stats raw;
	static struct fh8626_grid grid;
	static struct motion_state motion;
	struct ae_loop ae_loop = { .sync_needed = true, .frame.min_fps_milli = 12500 };
	uint32_t ae_target = DEFAULT_AE_TARGET_LUMA, last_sequence = 0, awb_sequence = 0;
	bool background = false, observe = false, have_sequence = false;
	bool ae_enabled = true, awb_enabled = true, awb_sync = true, awb_written = false;
	bool pidfile_written = false;
	bool flicker_set = false, tuning_enabled = false;
	struct isp_tuning tuning;
	int fd, lockfd = -1, sensor_fd = -1, option, result = EXIT_FAILURE;

	if (reserve_standard_fds() < 0) {
		LOG_ERR("Reserving standard descriptors: %s", strerror(errno));
		return EXIT_FAILURE;
	}

	ae_controller_set_defaults(&ae_cfg, ae_target);
	awb_controller_set_defaults(&awb_cfg);

	while ((option = getopt(argc, argv, "i:s:t:f:m:p:P:A:W:L:F:G:bovh")) != -1) {
		switch (option) {
		case 'F': {
			char *end;
			double fps;

			if (!strcmp(optarg, "off")) {
				ae_loop.frame.min_fps_milli = 0;
				break;
			}
			errno = 0;
			fps = strtod(optarg, &end);
			if (errno || end == optarg || *end || !(fps >= 1.0 && fps <= 240.0)) {
				LOG_ERR("-F requires the slowest frame rate, 1 to 240 frames/s, or off");
				return EXIT_FAILURE;
			}
			ae_loop.frame.min_fps_milli = (uint32_t)(fps * 1000.0 + 0.5);
			break;
		}
		case 'G': {
			double enter, leave = 0.0;
			char tail;
			int n = sscanf(optarg, "%lf,%lf%c", &enter, &leave, &tail);

			if (n < 1 || n > 2 || !(enter >= 1.0 && enter <= 64.0) ||
			    (n == 2 && !(leave >= 1.0 && leave <= enter))) {
				LOG_ERR("-G requires the analogue gain entering slow shutter and optionally the gain leaving it, 1 <= leave <= enter <= 64");
				return EXIT_FAILURE;
			}
			if (n == 1)
				leave = enter * 2.0 / 3.0 > 1.0 ? enter * 2.0 / 3.0 : 1.0;
			ae_cfg.slow_gain_enter = (uint32_t)(enter * 64.0 + 0.5);
			ae_cfg.slow_gain_leave = (uint32_t)(leave * 64.0 + 0.5);
			break;
		}
		case 'L': {
			unsigned int e, g, d = 0;
			char tail;
			int n = sscanf(optarg, "%u,%u,%u%c", &e, &g, &d, &tail);

			if (n < 2 || n > 3 || e < 1 || e > 8 || g < 1 || g > e ||
			    (n == 3 && (d < 1 || d > e))) {
				LOG_ERR("-L requires exposure,gain[,ISP gain] delays in frames, 1 <= gain <= exposure <= 8");
				return EXIT_FAILURE;
			}
			ae_cfg.exposure_delay = (uint8_t)e;
			ae_cfg.gain_delay = (uint8_t)g;
			ae_cfg.digital_gain_delay = (uint8_t)(n == 3 ? d : g);
			break;
		}
		case 'A':
		case 'W': {
			bool enabled;
			if (!strcmp(optarg, "auto"))
				enabled = true;
			else if (!strcmp(optarg, "off"))
				enabled = false;
			else {
				LOG_ERR("-%c requires auto or off", option);
				return EXIT_FAILURE;
			}
			if (option == 'A')
				ae_enabled = enabled;
			else
				awb_enabled = enabled;
			break;
		}
		case 'P':
			if (strlen(optarg) >= sizeof(pidfile) || optarg[0] != '/')
				return EXIT_FAILURE;
			snprintf(pidfile, sizeof(pidfile), "%s", optarg);
			break;
		case 'i':
			if (strlen(optarg) >= sizeof(path))
				return EXIT_FAILURE;
			snprintf(path, sizeof(path), "%s", optarg);
			break;
		case 's':
			if (strlen(optarg) >= sizeof(sensor_path))
				return EXIT_FAILURE;
			snprintf(sensor_path, sizeof(sensor_path), "%s", optarg);
			break;
		case 't': {
			char *end;
			unsigned long target;
			errno = 0;
			target = strtoul(optarg, &end, 10);
			if (errno || end == optarg || *end || target < 1 || target > 4096) {
				LOG_ERR("AE target must be a 12-bit level from 1 to 4096");
				return EXIT_FAILURE;
			}
			ae_target = (uint32_t)target;
			ae_cfg.target_luma = ae_target;
			break;
		}
		case 'f':
			flicker_set = true;
			if (!strcmp(optarg, "50"))
				ae_cfg.flicker_mode = AE_ANTIFLICKER_50HZ;
			else if (!strcmp(optarg, "60"))
				ae_cfg.flicker_mode = AE_ANTIFLICKER_60HZ;
			else if (!strcmp(optarg, "auto") || !strcmp(optarg, "1")) {
				LOG_ERR("Automatic flicker detection is unavailable; select 50, 60 or off");
				return EXIT_FAILURE;
			}
			else if (!strcmp(optarg, "off") || !strcmp(optarg, "0"))
				ae_cfg.flicker_mode = AE_ANTIFLICKER_OFF;
			else {
				LOG_ERR("Invalid anti-flicker mode '%s' (use 50, 60, off)", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'm':
			if (!strcmp(optarg, "bayes") || !strcmp(optarg, "2")) {
				LOG_ERR("Bayes AWB is unavailable: sensor illuminants are not calibrated; use whitepoint or grayworld");
				return EXIT_FAILURE;
			}
			else if (!strcmp(optarg, "whitepoint") || !strcmp(optarg, "wp") || !strcmp(optarg, "1"))
				awb_cfg.mode = AWB_MODE_WHITE_POINT;
			else if (!strcmp(optarg, "grayworld") || !strcmp(optarg, "gw") || !strcmp(optarg, "0"))
				awb_cfg.mode = AWB_MODE_GRAY_WORLD;
			else {
				LOG_ERR("Invalid AWB mode '%s' (use whitepoint, grayworld)", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'p':
			if (!strcmp(optarg, "motion"))
				ae_cfg.profile = AE_PROFILE_MOTION;
			else if (!strcmp(optarg, "normal"))
				ae_cfg.profile = AE_PROFILE_NORMAL;
			else {
				LOG_ERR("Invalid AE profile '%s' (use normal, motion)", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'b': background = true; break;
		case 'o': observe = true; break;
		case 'v': g_verbose = 1; break;
		case 'h':
			printf("Usage: %s [-i ISP-subdev] [-s sensor-subdev] [-t target] [-f 50|60|off] [-m wp|gw] [-p normal|motion] [-A auto|off] [-W auto|off] [-L exp,gain] [-F fps|off] [-G enter[,leave]] [-P pidfile] [-b] [-o] [-v]\n"
			       "Options:\n"
			       "  -i <dev>   ISP subdevice node (auto-detected if omitted)\n"
			       "  -s <dev>   Sensor subdevice node (auto-detected if omitted)\n"
			       "  -t <level> AE 12-bit target luminance (1..4096, default 1856)\n"
			       "  -f <mode>  Anti-flicker: 50, 60, off (default: off; 50/60 require sensor timing)\n"
			       "  -m <mode>  AWB mode: whitepoint, grayworld (default: whitepoint)\n"
			       "  -p <prof>  AE profile: normal, motion (default: normal; motion requires timing)\n"
			       "  -P <path>  Write/remove the service PID file after successful setup\n"
			       "  -A <mode>  AE ownership: auto or off (default auto)\n"
			       "  -W <mode>  AWB ownership: auto or off (default auto)\n"
			       "             Stop/restart with the respective loop off before manual controls.\n"
			       "             Auto owns its actuators; concurrent manual writes are not atomic.\n"
			       "             off is final: the auto controls cannot re-enable the loop.\n"
			       "  -b         Run as background daemon\n"
			       "  -o         Observe mode (read stats without modifying controls)\n"
			       "  -L <e,g>   Frames from a write to the statistics that show it,\n"
			       "             exposure and gain (default 3,3)\n"
			       "  -F <fps>   Slow shutter: slowest frame rate in low light, or off\n"
			       "             (default 12.5; needs a writable sensor VBLANK)\n"
			       "  -G <e[,l]> Analogue gain entering slow shutter and leaving it\n"
			       "             (default 4,2.67)\n"
			       "  -v         Verbose debug logging\n", argv[0]);
			return EXIT_SUCCESS;
		default: return EXIT_FAILURE;
		}
	}

	if (!observe && !ae_enabled && !awb_enabled) {
		LOG_ERR("Both loops are off; stop the service for manual control or use -o to observe");
		return EXIT_FAILURE;
	}
	if (optind != argc || (!path[0] && discover_subdev(path, sizeof(path), "fh8626-isp"))) {
		LOG_ERR("Specify one ISP subdevice with -i; automatic discovery was ambiguous or empty");
		return EXIT_FAILURE;
	}

	fd = open(path, O_RDWR | O_NONBLOCK | O_CLOEXEC);
	if (fd < 0) {
		LOG_ERR("Opening %s: %s", path, strerror(errno));
		return EXIT_FAILURE;
	}

	ae_controller_init(&ae, &ae_cfg);

	if (!observe || pidfile[0]) {
		lockfd = lock_isp(fd);
		if (lockfd < 0) {
			LOG_ERR("Cannot own ISP automatic controls (another daemon may be running): %s", strerror(errno));
			goto out;
		}
	}
	if (!observe) {
		init_isp_digital_gain(fd, &ae);
		char sensor_name[64] = "camera-sensor";
		bool need_sensor = ae_enabled;
		if (!sensor_path[0])
			discover_sensor_subdev(sensor_path, sizeof(sensor_path), sensor_name, sizeof(sensor_name));

		if (sensor_path[0]) {
			sensor_fd = open(sensor_path, O_RDWR | O_NONBLOCK | O_CLOEXEC);
			if (sensor_fd >= 0) {
				if (sensor_is_supported(sensor_fd, sensor_name, sizeof(sensor_name)) < 0 ||
				    read_sensor_controls(sensor_fd, &ae) != 0) {
					if (need_sensor) {
						LOG_ERR("Camera sensor (%s) control setup on %s: %s", sensor_name, sensor_path, strerror(errno));
						goto out;
					}
					close(sensor_fd);
					sensor_fd = -1;
				} else {
					/* Before the first timing read, so no change is missed. */
					ae_loop.sensor_events = ae_enabled &&
								subscribe_sensor_events(sensor_fd);
					if (ae_enabled && !ae_loop.sensor_events)
						LOG_WARN("Sensor control events unavailable; polling timing and exposure");
					if (!read_sensor_timing(sensor_fd, &ae.config, &ae_loop.frame)) {
						if (ae.config.flicker_mode != AE_ANTIFLICKER_OFF || ae.config.profile == AE_PROFILE_MOTION) {
							if (need_sensor) {
								LOG_ERR("Sensor timing is unavailable; 50/60 Hz and motion profile require frame interval and VBLANK");
								goto out;
							}
						} else {
							LOG_WARN("Sensor timing unavailable; AE uses the exposure limits only");
						}
					}
				}
			} else if (need_sensor) {
				LOG_ERR("Opening sensor %s: %s", sensor_path, strerror(errno));
				goto out;
			}
		} else if (need_sensor) {
			LOG_ERR("Specify camera sensor subdevice with -s; discovery was ambiguous or empty");
			goto out;
		}
	}

	awb_controller_init(&awb, &awb_cfg);

	if ((!observe && awb_enabled && read_gains(fd, &awb) < 0) ||
	    read_grid(fd, &raw, &grid) < 0 ||
	    ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &subscription) < 0) {
		LOG_ERR("ISP statistics/control setup: %s", strerror(errno));
		goto out;
	}
	/* Manual white balance: start the colour tuning from the set gains. */
	if (!observe && !awb_enabled && read_gains(fd, &awb) < 0)
		LOG_WARN("White-balance gains unreadable; colour tuning assumes unity");
	if (!observe) {
		if (flicker_set && write_power_line(fd, ae_cfg.flicker_mode) < 0)
			LOG_WARN("Power line frequency control: %s", strerror(errno));
		read_power_line(fd, &ae.config);
		tuning_enabled = isp_tuning_init(&tuning, fd) == 0;
	}
	if (!observe && ae_enabled) {
		struct v4l2_event_subscription sub = { .type = V4L2_EVENT_CTRL };

		sub.id = V4L2_CID_POWER_LINE_FREQUENCY;
		ae_loop.isp_events = ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &sub) == 0;
		sub.id = V4L2_CID_DIGITAL_GAIN;
		if (ae_loop.isp_events && ae.digital_gain.max > ae.digital_gain.min &&
		    ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &sub) < 0) {
			sub.id = V4L2_CID_POWER_LINE_FREQUENCY;
			ioctl(fd, VIDIOC_UNSUBSCRIBE_EVENT, &sub);
			ae_loop.isp_events = false;
		}
	}

	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	/* Report startup success only after the child has published its PID. */
	if ((background ? start_background(pidfile) : write_pidfile(pidfile)) < 0) {
		LOG_ERR("Starting daemon: %s", strerror(errno));
		goto out;
	}
	pidfile_written = pidfile[0] != '\0';

	LOG_INFO("FH8626 3A Daemon starting on %s (sensor=%s, target=%u/4096, flicker=%s, awb=%s)",
		 path, sensor_path[0] ? sensor_path : (observe ? "observe" : "disabled"), ae_target,
		 ae_cfg.flicker_mode == AE_ANTIFLICKER_50HZ ? "50Hz" :
		 ae_cfg.flicker_mode == AE_ANTIFLICKER_60HZ ? "60Hz" : "off",
		 awb_cfg.mode == AWB_MODE_WHITE_POINT ? "WhitePoint" : "GrayWorld");
	LOG_INFO("Actuator ownership: AE=%s, AWB=%s%s", ae_enabled ? "auto" : "off",
		 awb_enabled ? "auto" : "off", observe ? " (observe only; no writes)" : "");

	while (running) {
		struct pollfd pfd = { .fd = fd, .events = POLLPRI };
		struct v4l2_event event;
		uint32_t wb[3], m, gain, again;
		bool frame = false, masked, awb_changed;

		int ret = poll(&pfd, 1, 1000);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			goto io_error;
		}
		if (!ret)
			continue;

		if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
			LOG_ERR("ISP disappeared or statistics stream failed");
			goto out;
		}

		for (;;) {
			if (ioctl(fd, VIDIOC_DQEVENT, &event) == 0) {
				frame |= (event.type == V4L2_EVENT_FRAME_SYNC);
				if (event.type == V4L2_EVENT_CTRL &&
				    (event.u.ctrl.changes & V4L2_EVENT_CTRL_CH_VALUE)) {
					if (event.id == V4L2_CID_POWER_LINE_FREQUENCY)
						ae_loop.ev.power_line = true;
					else
						ae_loop.ev.controls = true;
				}
				continue;
			}
			if (errno == EINTR) {
				if (!running)
					break;
				continue;
			}
			/* VIDIOC_DQEVENT fails with ENOENT when no event is pending. */
			if (errno == ENOENT || errno == EAGAIN)
				break;
			goto io_error;
		}
		if (!running)
			break;
		if (!frame)
			continue;

		ret = read_grid(fd, &raw, &grid);
		if (ret < 0)
			goto io_error;
		if (!ret || (have_sequence && grid.sequence == last_sequence))
			continue;
		/* A new stream restarts the sequence: start from the controls. */
		if (have_sequence && (int32_t)(grid.sequence - last_sequence) < 0) {
			ae_loop.sync_needed = true;
			awb_sync = true;
			motion.have_prev = false;
			motion.floor_valid = false;
		}
		/* Delivered frame period, for the log. */
		if (have_sequence && grid.sequence - last_sequence == 1 &&
		    grid.timestamp_ns > ae_loop.last_timestamp)
			ae_loop.period_us = (uint32_t)((grid.timestamp_ns - ae_loop.last_timestamp) / 1000);
		ae_loop.last_timestamp = grid.timestamp_ns;
		have_sequence = true;
		last_sequence = grid.sequence;

		if (observe) {
			LOG_DBG("Grid frame=%u centre=%u/%u/%u motion=%u/%u",
				grid.sequence, grid.r[136] >> 4, grid.g[136] >> 4,
				grid.b[136] >> 4, grid.motion_blocks, grid.motion_total);
			continue;
		}

		/* AWB first: the AE meter weighs the channels by these gains. */
		awb_changed = false;
		if (awb_enabled && check_awb_auto(fd, awb_enabled)) {
			process_awb(fd, &awb, &grid, &awb_sync, &awb_changed);
			if (awb_changed) {
				if (write_gains(fd, &awb) < 0)
					goto io_error;
				awb_sequence = grid.sequence;
				if (!awb_written || g_verbose)
					LOG_INFO("AWB frame=%u grey=%u edges=%u wp=%d,%d gains=%u/%u/%u",
						 grid.sequence, awb.grey_cells, awb.edge_pairs,
						 awb.wp_x, awb.wp_y, awb.current_gain_r,
						 awb.current_gain_g, awb.current_gain_b);
				awb_written = true;
			}
		} else {
			/* Paused or not owned: follow the gains for the tuning. */
			awb_sync = true;
			if (!(grid.sequence % 8))
				read_gains(fd, &awb);
		}
		wb[0] = awb.current_gain_r;
		wb[1] = awb.current_gain_g;
		wb[2] = awb.current_gain_b;

		/* A frame where an exposure or balance change first shows. */
		masked = (ae_enabled && ae_controller_in_transition(&ae, grid.sequence)) ||
			 (awb_written && grid.sequence - awb_sequence <= ae.config.exposure_delay + 1U);

		if (ae_enabled && sensor_fd >= 0) {
			if (ae_loop.sensor_events &&
			    read_sensor_events(sensor_fd, &ae_loop.ev, &ae_loop.frame, &ae) < 0)
				goto io_error;
			if (process_ae(fd, sensor_fd, &ae_loop, &ae, &grid, wb, ae_enabled) < 0)
				goto io_error;
		} else if (sensor_fd >= 0 && !(grid.sequence % 8)) {
			read_sensor_controls(sensor_fd, &ae);
			read_digital_gain(fd, &ae);
		}

		/*
		 * The tuning follows the gain of the frames it will apply to.
		 * ISP parameters written now first show in the statistics
		 * digital_gain_delay frames later, like the ISP gain, so the
		 * black level and noise filters change together with the gain
		 * they belong to. Without automatic exposure, the gain read
		 * back.
		 */
		if (ae_enabled && !ae_loop.sync_needed) {
			gain = ae_controller_effective_gain(&ae, grid.sequence +
							    ae.config.digital_gain_delay, &again);
		} else {
			gain = ae_controller_total_gain(&ae);
			again = ae.current_analog_gain;
		}
		m = motion_update(&motion, &grid, gain, masked);
		if (tuning_enabled)
			isp_tuning_update(&tuning, gain, again,
					  (uint32_t)((uint64_t)ae.current_exposure_lines *
						     ae.config.line_time_ns / 1000),
					  wb[0], wb[1], wb[2], m);
		LOG_DBG("Motion frame=%u blocks=%u/%u md=%u noise=%u+%u cells=%u raw=%u level=%u%s",
			grid.sequence, grid.motion_blocks, grid.motion_total, motion.md,
			motion.md_mean >> 4, motion.md_dev >> 4, motion.cells, motion.raw, m,
			masked ? " masked" : "");
	}

	result = EXIT_SUCCESS;
	goto out;

io_error:
	LOG_ERR("3A statistics/control I/O: %s", strerror(errno));
out:
	if (pidfile_written)
		unlink(pidfile);
	/* Give the nominal frame back; the driver keeps the integration inside it. */
	if (sensor_fd >= 0 && ae_enabled && !observe && ae_loop.frame.writable &&
	    ae_loop.frame.nominal_valid && ae_loop.frame.vblank != ae_loop.frame.nominal_vblank &&
	    write_vblank(sensor_fd, &ae, &ae_loop.frame,
			 ae_loop.frame.height + ae_loop.frame.nominal_vblank) == 0)
		LOG_INFO("Restored the nominal frame, VBLANK %d", ae_loop.frame.nominal_vblank);
	if (sensor_fd >= 0)
		close(sensor_fd);
	close(fd);
	if (lockfd >= 0)
		close(lockfd);
	return result;
}
