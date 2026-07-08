// SPDX-License-Identifier: GPL-2.0
/*
 * gpiostep-ctl - userspace front-end for the gpiostep kernel module.
 *
 * Argument signature mirrors the userspace gpio-motors tool so the two
 * implementations can be compared 1:1:
 *
 *   gpiostep-ctl <pan steps> <tilt steps> <delay (ms)>
 *
 * e.g.  gpiostep-ctl 20 0 30   # pan +20 steps, 30ms per microstep
 *       gpiostep-ctl 0 -20 30  # tilt -20 steps
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "gpiostep.h"

#define DEV "/dev/" GPIOSTEP_DEV_NAME

int main(int argc, char *argv[])
{
	struct gpiostep_move m;
	int fd, ret;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s <pan steps> <tilt steps> <delay (ms)>\n",
			argv[0]);
		return 1;
	}

	memset(&m, 0, sizeof(m));
	m.pan = atoi(argv[1]);
	m.tilt = atoi(argv[2]);
	m.delay_us = atoi(argv[3]) * 1000;

	fd = open(DEV, O_RDWR);
	if (fd < 0) {
		perror("open " DEV);
		return 1;
	}

	ret = ioctl(fd, GPIOSTEP_MOVE, &m);
	if (ret < 0)
		perror("ioctl GPIOSTEP_MOVE");

	close(fd);
	return ret < 0 ? 1 : 0;
}
