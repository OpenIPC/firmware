/* SPDX-License-Identifier: GPL-2.0 */
/*
 * gpiostep - shared ABI between the kernel module and the userspace CLI.
 *
 * A "step" is one full 8-phase half-step cycle, matching the semantics of the
 * userspace gpio-motors tool so the two drivers can be compared 1:1.
 */
#ifndef _GPIOSTEP_H
#define _GPIOSTEP_H

#include <linux/ioctl.h>

struct gpiostep_move {
	int pan;	/* pan steps  (sign = direction) */
	int tilt;	/* tilt steps (sign = direction) */
	int delay_us;	/* per-microstep delay in microseconds */
};

#define GPIOSTEP_IOC_MAGIC 'g'
#define GPIOSTEP_MOVE	_IOW(GPIOSTEP_IOC_MAGIC, 1, struct gpiostep_move)

#define GPIOSTEP_DEV_NAME "motorDev"

#endif /* _GPIOSTEP_H */
