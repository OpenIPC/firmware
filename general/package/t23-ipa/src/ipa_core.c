/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
/*
 * t23-ipa: 3A for one T23 ISP input. Reads hardware statistics from the
 * input's T23S capture node, discovers and drives the bound sensor subdev's
 * exposure/gain/vblank controls via Media Controller topology (falling back
 * to the capture node on legacy kernels), and queues ISP parameters on the
 * T23E output node. Input 0 also switches the shared IR-cut filter
 * and the infrared illuminator between day and night; input 1 follows the
 * filter. Video capture itself is left to the application.
 */
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <poll.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/media.h>
#include <linux/videodev2.h>
#include "t23_ipa.h"

#ifndef V4L2_META_FMT_T23_ISP_EXT_PARAMS
#define V4L2_META_FMT_T23_ISP_EXT_PARAMS v4l2_fourcc('T', '2', '3', 'E')
#endif

#define STATS_BUFFERS		4
#define PARAMS_BUFFERS		3
#define MAX_META_BUFFERS	(PARAMS_BUFFERS > STATS_BUFFERS ? PARAMS_BUFFERS : STATS_BUFFERS)
#define LSC_CT_STEP		50.0	/* kelvin */
#define DNS_GAIN_STEP		0x1000	/* log2 Q16, 1/16 stop */

struct meta_queue {
	int fd;
	enum v4l2_buf_type type;
	unsigned int count;
	void *map[MAX_META_BUFFERS];
	size_t map_len[MAX_META_BUFFERS];
	bool queued[MAX_META_BUFFERS];
	unsigned int rejected;
};

static void queue_cleanup(struct meta_queue *q)
{
	unsigned int i;

	for (i = 0; i < q->count; i++) {
		if (q->map[i] && q->map[i] != MAP_FAILED) {
			munmap(q->map[i], q->map_len[i]);
			q->map[i] = NULL;
		}
	}
	q->count = 0;
}

static volatile sig_atomic_t stop;

static void on_signal(int sig)
{
	(void)sig;
	stop = 1;
}

static int find_node(const char *name)
{
	char path[64], text[64];
	unsigned int i;

	for (i = 0; i < 64; i++) {
		FILE *f;

		snprintf(path, sizeof(path), "/sys/class/video4linux/video%u/name", i);
		f = fopen(path, "r");
		if (!f)
			continue;
		if (fgets(text, sizeof(text), f)) {
			text[strcspn(text, "\n")] = '\0';
			if (!strcmp(text, name)) {
				fclose(f);
				snprintf(path, sizeof(path), "/dev/video%u", i);
				return open(path, O_RDWR | O_NONBLOCK | O_CLOEXEC);
			}
		}
		fclose(f);
	}
	errno = ENODEV;
	return -1;
}

static int open_char_dev(unsigned int major, unsigned int minor)
{
	char sys_path[64], link_buf[128], dev_path[144];
	const char *base;
	ssize_t len;

	snprintf(sys_path, sizeof(sys_path), "/sys/dev/char/%u:%u", major, minor);
	len = readlink(sys_path, link_buf, sizeof(link_buf) - 1);
	if (len <= 0)
		return -1;
	link_buf[len] = '\0';
	base = strrchr(link_buf, '/');
	base = base ? base + 1 : link_buf;
	snprintf(dev_path, sizeof(dev_path), "/dev/%s", base);
	return open(dev_path, O_RDWR | O_CLOEXEC);
}

/*
 * Walk /dev/mediaN topology from "t23-isp-core<input>" sink pad 0 back to its
 * enabled upstream sensor entity and open the corresponding /dev/v4l-subdevX.
 */
static int find_sensor_subdev(unsigned int input, char *model, size_t size)
{
	char isp_name[32], mpath[32];
	unsigned int m;

	snprintf(isp_name, sizeof(isp_name), "t23-isp-core%u", input);
	for (m = 0; m < 8; m++) {
		struct media_v2_topology topo = {0};
		struct media_v2_entity *ents = NULL;
		struct media_v2_interface *intfs = NULL;
		struct media_v2_pad *pads = NULL;
		struct media_v2_link *links = NULL;
		uint32_t isp_ent_id = 0, sink_pad_id = 0;
		uint32_t src_pad_id = 0, sensor_ent_id = 0, intf_id = 0;
		unsigned int i;
		int mfd, sfd = -1;

		snprintf(mpath, sizeof(mpath), "/dev/media%u", m);
		mfd = open(mpath, O_RDONLY | O_CLOEXEC);
		if (mfd < 0)
			continue;

		if (ioctl(mfd, MEDIA_IOC_G_TOPOLOGY, &topo) < 0 ||
		    !topo.num_entities || !topo.num_pads || !topo.num_links) {
			close(mfd);
			continue;
		}

		ents = calloc(topo.num_entities, sizeof(*ents));
		if (topo.num_interfaces)
			intfs = calloc(topo.num_interfaces, sizeof(*intfs));
		pads = calloc(topo.num_pads, sizeof(*pads));
		links = calloc(topo.num_links, sizeof(*links));
		if (!ents || (topo.num_interfaces && !intfs) || !pads || !links)
			goto next_media;

		topo.ptr_entities = (uintptr_t)ents;
		topo.ptr_interfaces = (uintptr_t)intfs;
		topo.ptr_pads = (uintptr_t)pads;
		topo.ptr_links = (uintptr_t)links;
		if (ioctl(mfd, MEDIA_IOC_G_TOPOLOGY, &topo) < 0)
			goto next_media;

		for (i = 0; i < topo.num_entities; i++) {
			if (!strcmp(ents[i].name, isp_name)) {
				isp_ent_id = ents[i].id;
				break;
			}
		}
		if (!isp_ent_id)
			goto next_media;

		for (i = 0; i < topo.num_pads; i++) {
			if (pads[i].entity_id == isp_ent_id &&
			    (pads[i].flags & MEDIA_PAD_FL_SINK)) {
				sink_pad_id = pads[i].id;
				break;
			}
		}
		if (!sink_pad_id)
			goto next_media;

		for (i = 0; i < topo.num_links; i++) {
			uint32_t ltype = links[i].flags & MEDIA_LNK_FL_LINK_TYPE;

			if (ltype == MEDIA_LNK_FL_DATA_LINK &&
			    (links[i].flags & MEDIA_LNK_FL_ENABLED) &&
			    links[i].sink_id == sink_pad_id) {
				src_pad_id = links[i].source_id;
				break;
			}
		}
		if (!src_pad_id)
			goto next_media;

		for (i = 0; i < topo.num_pads; i++) {
			if (pads[i].id == src_pad_id) {
				sensor_ent_id = pads[i].entity_id;
				break;
			}
		}
		if (!sensor_ent_id)
			goto next_media;

		for (i = 0; i < topo.num_links; i++) {
			uint32_t ltype = links[i].flags & MEDIA_LNK_FL_LINK_TYPE;

			if (ltype == MEDIA_LNK_FL_INTERFACE_LINK &&
			    links[i].sink_id == sensor_ent_id) {
				intf_id = links[i].source_id;
				break;
			}
		}
		if (!intf_id)
			goto next_media;

		for (i = 0; i < topo.num_interfaces; i++) {
			if (intfs[i].id == intf_id) {
				sfd = open_char_dev(intfs[i].devnode.major,
						    intfs[i].devnode.minor);
				break;
			}
		}

		if (sfd >= 0 && model) {
			for (i = 0; i < topo.num_entities; i++) {
				if (ents[i].id == sensor_ent_id) {
					snprintf(model, size, "%.*s",
						 (int)strcspn(ents[i].name, " "), ents[i].name);
					break;
				}
			}
		}
next_media:
		free(ents);
		free(intfs);
		free(pads);
		free(links);
		close(mfd);
		if (sfd >= 0)
			return sfd;
	}
	return -1;
}

static int queue_init(struct meta_queue *q, int fd, enum v4l2_buf_type type,
		      unsigned int count)
{
	struct v4l2_requestbuffers req = { .count = count, .type = type,
					   .memory = V4L2_MEMORY_MMAP };
	unsigned int i;

	memset(q, 0, sizeof(*q));
	q->fd = fd;
	q->type = type;
	if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0 || req.count < 2)
		return -1;
	q->count = req.count > count ? count : req.count;
	for (i = 0; i < q->count; i++) {
		struct v4l2_buffer b = { .type = type, .memory = V4L2_MEMORY_MMAP,
					 .index = i };

		if (ioctl(fd, VIDIOC_QUERYBUF, &b) < 0)
			return -1;
		q->map_len[i] = b.length;
		q->map[i] = mmap(NULL, b.length, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, b.m.offset);
		if (q->map[i] == MAP_FAILED)
			return -1;
		if (type == V4L2_BUF_TYPE_META_CAPTURE &&
		    ioctl(fd, VIDIOC_QBUF, &b) < 0)
			return -1;
	}
	return 0;
}

static void *params_get(struct meta_queue *q, unsigned int *index)
{
	struct v4l2_buffer b = { .type = q->type, .memory = V4L2_MEMORY_MMAP };
	unsigned int i;

	while (ioctl(q->fd, VIDIOC_DQBUF, &b) == 0) {
		q->queued[b.index] = false;
		if (b.flags & V4L2_BUF_FLAG_ERROR)
			q->rejected++;
	}
	for (i = 0; i < q->count; i++) {
		if (!q->queued[i]) {
			*index = i;
			return q->map[i];
		}
	}
	return NULL;
}

static int params_queue(struct meta_queue *q, unsigned int index,
			size_t bytesused)
{
	struct v4l2_buffer b = { .type = q->type, .memory = V4L2_MEMORY_MMAP,
				 .index = index,
				 .bytesused = bytesused };

	if (ioctl(q->fd, VIDIOC_QBUF, &b) < 0)
		return -1;
	q->queued[index] = true;
	return 0;
}

static bool gain_moved(uint32_t gain, uint32_t last)
{
	return last == UINT32_MAX ||
	       (gain > last ? gain - last : last - gain) >= DNS_GAIN_STEP;
}

static int load_mode(struct ipa_tuning *t, bool night, unsigned int target,
		     unsigned int mains_hz, double tolerance, struct ipa_ae *ae,
		     struct ipa_awb *awb, struct ipa_color *color)
{
	t->night = night;
	if (ipa_ae_init(ae, t, target, mains_hz) || ipa_awb_init(awb, t, tolerance) ||
	    ipa_color_init(color, t))
		return -EINVAL;
	color->mono = night;
	return 0;
}

static void usage(const char *name)
{
	printf("Usage: %s [options]\n"
	       "  -i, --input <n>        ISP input (default 0)\n"
	       "  -c, --tuning <file>    tuning YAML from tuning/bin2yaml.py\n"
	       "                         (default from the selected sensor)\n"
	       "  -t, --target <luma>    fixed AE target, pre-gamma 8-bit zone mean\n"
	       "                         (default: the tuning's curve by exposure)\n"
	       "  -l, --tolerance <d>    AWB distance allowed off the locus,\n"
	       "                         grid units (default 20)\n"
	       "  -F, --min-fps <fps>    slowest frame rate AE may use (default 15);\n"
	       "                         input 1 runs the frame length of input 0,\n"
	       "                         its frame-sync master\n"
	       "  -f, --flicker <hz>     mains frequency, 50 or 60; AE keeps exposure\n"
	       "                         to whole lamp flicker periods (default 0, off)\n"
	       "  -A, --no-ae            leave exposure and gain alone\n"
	       "  -W, --no-awb           keep unity white balance\n"
	       "  -d, --daynight <mode>  auto, day, night or follow\n"
	       "                         (default auto on input 0, follow on input 1)\n"
	       "  -N, --night-ev <us>    exposure us x gain that starts night (default 1000000)\n"
	       "  -D, --day-ev <us>      visible-light exposure us x gain that ends night\n"
	       "                         (default 330000)\n"
	       "  -L, --illuminator <led> LED class name, empty for none\n"
	       "                         (default ir:illumination)\n"
	       "  -n, --frames <count>   stop after count statistics buffers\n"
	       "  -b, --blocks <mask>    IPA_PARAM_* blocks to program (default all)\n"
	       "  -E, --ext-params       use extensible V4L2_META_FMT_T23_ISP_EXT_PARAMS (default)\n"
	       "  -v, --verbose          print the state every 15 frames\n",
	       name);
}

int main(int argc, char **argv)
{
	static const struct option options[] = {
		{ "input", required_argument, NULL, 'i' },
		{ "tuning", required_argument, NULL, 'c' },
		{ "target", required_argument, NULL, 't' },
		{ "tolerance", required_argument, NULL, 'l' },
		{ "min-fps", required_argument, NULL, 'F' },
		{ "flicker", required_argument, NULL, 'f' },
		{ "no-ae", no_argument, NULL, 'A' },
		{ "no-awb", no_argument, NULL, 'W' },
		{ "daynight", required_argument, NULL, 'd' },
		{ "night-ev", required_argument, NULL, 'N' },
		{ "day-ev", required_argument, NULL, 'D' },
		{ "illuminator", required_argument, NULL, 'L' },
		{ "frames", required_argument, NULL, 'n' },
		{ "blocks", required_argument, NULL, 'b' },
		{ "ext-params", no_argument, NULL, 'E' },
		{ "verbose", no_argument, NULL, 'v' },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};
	static const char *const dn_names[] = { "auto", "day", "night", "follow" };
	const char *tuning_path = NULL;
	const char *illuminator = "ir:illumination";
	double to_night = 1e6, to_day = 330000;
	int dn_mode = -1;
	unsigned int input = 0, target = 0, frames = 0, count = 0, mains_hz = 0;
	double tolerance = 20, lsc_ct = -1e9, min_fps = 15;
	uint32_t dns_gain = UINT32_MAX, lsc_gain = UINT32_MAX;
	uint32_t blocks = IPA_PARAM_ALL;
	bool do_ae = true, do_awb = true, verbose = false, idle = true;
	struct sigaction action = { .sa_handler = on_signal };
	struct meta_queue stats_q = {0}, params_q = {0};
	struct ipa_params_staging *scratch_cfg = NULL;
	struct ipa_tuning tuning = {0};
	struct ipa_sensor sensor;
	struct ipa_ae ae;
	struct ipa_awb awb;
	struct ipa_color color;
	struct ipa_daynight dn = { .mode = IPA_DN_FOLLOW, .ircut_fd = -1,
				   .state_fd = -1, .led_fd = -1 };
	enum v4l2_buf_type type;
	char name[32];
	char sensor_model[64] = "", default_tuning[96];
	int sensor_fd = -1, video = -1, stats_fd = -1, params_fd = -1, master = -1;
	int opt, ret = EXIT_FAILURE;

	while ((opt = getopt_long(argc, argv, "i:c:t:l:F:f:AWd:N:D:L:n:b:Evh", options, NULL)) != -1) {
		switch (opt) {
		case 'i': input = strtoul(optarg, NULL, 0); break;
		case 'c': tuning_path = optarg; break;
		case 't': target = strtoul(optarg, NULL, 0); break;
		case 'l': tolerance = strtod(optarg, NULL); break;
		case 'F': min_fps = strtod(optarg, NULL); break;
		case 'f': mains_hz = strtoul(optarg, NULL, 0); break;
		case 'A': do_ae = false; break;
		case 'W': do_awb = false; break;
		case 'd':
			for (dn_mode = 3; dn_mode >= 0; dn_mode--)
				if (!strcmp(optarg, dn_names[dn_mode]))
					break;
			if (dn_mode < 0) {
				usage(argv[0]);
				return EXIT_FAILURE;
			}
			break;
		case 'N': to_night = strtod(optarg, NULL); break;
		case 'D': to_day = strtod(optarg, NULL); break;
		case 'L': illuminator = optarg; break;
		case 'n': frames = strtoul(optarg, NULL, 0); break;
		case 'b': blocks = strtoul(optarg, NULL, 0); break;
		case 'E': break;
		case 'v': verbose = true; break;
		case 'h': usage(argv[0]); return EXIT_SUCCESS;
		default: usage(argv[0]); return EXIT_FAILURE;
		}
	}
	if (dn_mode < 0)
		dn_mode = input ? IPA_DN_FOLLOW : IPA_DN_AUTO;
	if (input > 1 || target > 254 || tolerance < 0 || min_fps < 0 ||
	    (mains_hz && mains_hz != 50 && mains_hz != 60) ||
	    to_day <= 0 || to_night <= to_day) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	snprintf(name, sizeof(name), "T23 stats%u", input);
	stats_fd = find_node(name);
	snprintf(name, sizeof(name), "T23 params%u", input);
	params_fd = find_node(name);
	if (stats_fd < 0 || params_fd < 0) {
		ipa_err("T23 ISP metadata nodes: %s\n", strerror(errno));
		goto out;
	}

	/*
	 * Discover the sensor subdev via Media Controller (Linux 6.12 IO_MC);
	 * fall back to the legacy capture node when MC is unavailable.
	 */
	sensor_fd = find_sensor_subdev(input, sensor_model, sizeof(sensor_model));
	if (sensor_fd < 0) {
		snprintf(name, sizeof(name), "T23 video%u", input);
		video = find_node(name);
		sensor_fd = video;
	}
	if (sensor_fd >= 0 && !strcmp(sensor_model, "imx290"))
		snprintf(sensor_model, sizeof(sensor_model), "imx327");
	if (sensor_fd < 0 || ipa_sensor_init(&sensor, sensor_fd, sensor_model)) {
		ipa_err("input %u: unsupported sensor controls\n", input);
		goto out;
	}

	if (!tuning_path) {
		static const char fallback_tuning[] = "/etc/sensor/sc1a4t-t23.yaml";

		snprintf(default_tuning, sizeof(default_tuning), "/etc/sensor/%s-t23.yaml",
			 *sensor_model ? sensor_model : "sc1a4t");
		if (strcmp(default_tuning, fallback_tuning) &&
		    access(default_tuning, R_OK) != 0) {
			ipa_warn("%s: %s, falling back to %s\n",
				 default_tuning, strerror(errno), fallback_tuning);
			tuning_path = fallback_tuning;
		} else {
			tuning_path = default_tuning;
		}
	}
	if (ipa_tuning_load(&tuning, tuning_path)) {
		ipa_err("%s: cannot load tuning\n", tuning_path);
		goto out;
	}
	if (load_mode(&tuning, true, target, mains_hz, tolerance, &ae, &awb, &color) ||
	    load_mode(&tuning, false, target, mains_hz, tolerance, &ae, &awb, &color)) {
		ipa_err("%s: incomplete tuning\n", tuning_path);
		goto out;
	}

	if (input) {
		master = find_sensor_subdev(0, NULL, 0);
		if (master < 0)
			master = find_node("T23 video0");
		if (master < 0)
			ipa_warn("input 0 not found, frame length fixed\n");
	} else {
		ipa_sensor_frame_limits(&sensor, min_fps);
	}

	{
		uint32_t want_fmt = V4L2_META_FMT_T23_ISP_EXT_PARAMS;
		struct v4l2_format pfmt = {
			.type = V4L2_BUF_TYPE_META_OUTPUT,
			.fmt.meta.dataformat = want_fmt,
		};

		if (ioctl(params_fd, VIDIOC_S_FMT, &pfmt) < 0 ||
		    pfmt.fmt.meta.dataformat != want_fmt) {
			ipa_err("ISP params format not supported\n");
			goto out;
		}
		scratch_cfg = calloc(1, sizeof(*scratch_cfg));
		if (!scratch_cfg)
			goto out;
	}

	if (dn_mode == IPA_DN_AUTO && !sensor.line_us)
		ipa_warn("input %u: unknown line time, day/night fixed\n",
			 input);
	sigemptyset(&action.sa_mask);
	/* Any of these must leave the filter in and the illuminator off. */
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGHUP, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	{
		int err = ipa_daynight_init(&dn, dn_mode, illuminator, to_night, to_day);

		if (err) {
			ipa_err("day/night state: %s\n", strerror(-err));
			goto out;
		}
	}
	if (dn.night)
		load_mode(&tuning, true, target, mains_hz, tolerance, &ae, &awb, &color);
	if (queue_init(&stats_q, stats_fd, V4L2_BUF_TYPE_META_CAPTURE, STATS_BUFFERS) ||
	    queue_init(&params_q, params_fd, V4L2_BUF_TYPE_META_OUTPUT, PARAMS_BUFFERS)) {
		ipa_err("metadata buffers: %s\n", strerror(errno));
		goto out;
	}
	type = params_q.type;
	if (ioctl(params_fd, VIDIOC_STREAMON, &type) < 0)
		goto out;
	type = stats_q.type;
	if (ioctl(stats_fd, VIDIOC_STREAMON, &type) < 0)
		goto out;

	setvbuf(stdout, NULL, _IOLBF, 0);

	ret = EXIT_SUCCESS;
	while (!stop && (!frames || count < frames)) {
		struct pollfd p = { .fd = stats_fd, .events = POLLIN };
		struct v4l2_buffer b = { .type = stats_q.type,
					 .memory = V4L2_MEMORY_MMAP };
		const struct t23_isp_stat_buffer *stats;
		void *slot;
		unsigned int index;
		uint32_t gain;
		double ev_us;
		int n;

		n = poll(&p, 1, 1000);
		if (n < 0 && errno != EINTR) {
			ipa_err("poll: %s\n", strerror(errno));
			ret = EXIT_FAILURE;
			break;
		}
		if (n <= 0) {
			/* No statistics for a poll period: the stream stopped. */
			idle |= n == 0;
			continue;
		}
		if (ioctl(stats_fd, VIDIOC_DQBUF, &b) < 0) {
			if (errno == ENODEV) {
				ipa_err("stats DQBUF: %s\n", strerror(errno));
				ret = EXIT_FAILURE;
				break;
			}
			/* An error event with no buffer to dequeue would spin. */
			if (p.revents & POLLERR)
				usleep(100000);
			continue;
		}
		stats = stats_q.map[b.index];
		count++;
		if (idle) {
			ipa_daynight_restart(&dn);
			idle = false;
		}

		if (master >= 0)
			ipa_sensor_follow(&sensor, master);
		if (do_ae)
			ipa_ae_run(&ae, &sensor, stats);
		else
			ae.ev = sensor.exposure *
				exp2(ipa_sensor_gain_log2(&sensor, sensor.gain_index) / Q16_ONE);
		gain = ipa_sensor_gain_log2(&sensor, sensor.gain_index);
		ev_us = ae.ev * sensor.line_us;
		if (ipa_daynight_run(&dn, ev_us, ae.converged)) {
			load_mode(&tuning, dn.night, target, mains_hz, tolerance, &ae,
				  &awb, &color);
			/* The illuminator and filter change the light at once. */
			if (do_ae) {
				ipa_ae_scale(&ae, &sensor,
					     ipa_daynight_ev_factor(&dn, ev_us));
				gain = ipa_sensor_gain_log2(&sensor, sensor.gain_index);
			}
			lsc_ct = -1e9;
			lsc_gain = dns_gain = UINT32_MAX;
			ipa_info("seq %u: %s\n", stats->sequence,
				 dn.night ? "night" : "day");
		}
		if (do_awb && dn.night)
			ipa_awb_grey(&awb, stats);
		else if (do_awb)
			ipa_awb_run(&awb, stats, ae.ev);

		slot = params_get(&params_q, &index);
		if (slot) {
			struct ipa_params_staging *cfg = scratch_cfg;
			size_t bytesused;

			memset(cfg, 0, sizeof(*cfg));
			cfg->update = IPA_PARAM_WB | IPA_PARAM_CCM |
				      IPA_PARAM_GAMMA;
			cfg->wb.gain_r = do_awb ? awb.gain_r : (uint16_t)Q10_ONE;
			cfg->wb.gain_b = do_awb ? awb.gain_b : (uint16_t)Q10_ONE;
			ipa_color_ccm(&color, awb.ct, ae.ev, &cfg->ccm);
			ipa_color_gamma(&color, &cfg->gamma);
			if (fabs(awb.ct - lsc_ct) >= LSC_CT_STEP ||
			    gain_moved(gain, lsc_gain)) {
				ipa_color_lsc(&color, awb.ct, gain, &cfg->lsc);
				cfg->update |= IPA_PARAM_LSC;
				lsc_ct = awb.ct;
				lsc_gain = gain;
			}
			if (gain_moved(gain, dns_gain)) {
				if (ipa_dns_pack(&tuning, gain, cfg) == 0)
					dns_gain = gain;
			}
			cfg->update &= blocks;

			bytesused = ipa_pack_ext_params(slot, cfg);

			if (params_queue(&params_q, index, bytesused))
				ipa_warn("params QBUF: %s\n", strerror(errno));
		}

		if (verbose && count % 15 == 1)
			ipa_info("seq %u luma %.1f/%.0f hi %.0f exp %d vts %d gain %.2fx ev %.0fus ct %.0fK wb %u/%u rejected %u %s%s\n",
				 stats->sequence, ae.luma, ae.target, ae.highlight,
				 sensor.exposure, sensor.height + sensor.vblank,
				 exp2(gain / Q16_ONE), ae.ev * sensor.line_us,
				 awb.ct, awb.gain_r, awb.gain_b, params_q.rejected,
				 dn.night ? "night" : "day",
				 ae.converged ? " ae-ok" : "");
		if (verbose && count % 15 == 1)
			ipa_info("  awb grey %.0f,%.0f white %.0f,%.0f filtered %.0f,%.0f\n",
				 awb.dbg_grey_rg, awb.dbg_grey_bg, awb.dbg_mean_rg,
				 awb.dbg_mean_bg, awb.rg, awb.bg);
		ioctl(stats_fd, VIDIOC_QBUF, &b);
	}
	type = stats_q.type;
	ioctl(stats_fd, VIDIOC_STREAMOFF, &type);
	type = params_q.type;
	ioctl(params_fd, VIDIOC_STREAMOFF, &type);
out:
	free(scratch_cfg);
	queue_cleanup(&stats_q);
	queue_cleanup(&params_q);
	if (stats_fd >= 0)
		close(stats_fd);
	if (params_fd >= 0)
		close(params_fd);
	if (sensor_fd >= 0 && sensor_fd != video)
		close(sensor_fd);
	if (video >= 0)
		close(video);
	if (master >= 0)
		close(master);
	ipa_daynight_close(&dn);
	ipa_tuning_free(&tuning);
	return ret;
}
