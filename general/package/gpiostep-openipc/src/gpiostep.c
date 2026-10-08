// SPDX-License-Identifier: GPL-2.0
/*
 * gpiostep - in-kernel GPIO half-step pan/tilt stepper driver.
 *
 * A clean reimplementation of the vendor "gpioStep"/"motor" behaviour observed
 * on Goke GK7205V510 cameras (model NC-IPTC2200_DL): two 4-wire stepper coils
 * driven over GPIO. The stepping runs entirely in kernel context with direct
 * gpio_set_value(), which avoids the syscall traffic of the userspace
 * gpio-motors tool. Timing granularity depends on the kernel: with
 * CONFIG_HIGH_RES_TIMERS (gk7205v500) every delay is a precise sleep; without
 * it a sleep rounds up to the tick, so sub-tick delays busy-wait between
 * scheduler yields (see step_delay()).
 *
 * Control is via a misc char device /dev/motorDev and a single ioctl. The pin
 * map defaults to the GK7205V510 layout and is overridable with module params:
 *
 *   insmod gpiostep.ko pan_gpios=3,4,72,73 tilt_gpios=69,59,58,57
 *
 * Step semantics (one "step" == one full 8-phase cycle) and the half-step
 * sequence are identical to general/package/gpio-motors/src/gpio-motors.c.
 *
 * A move from rest does not start at the delay it asks for. A stepper has a
 * rate it can start at from standstill and a higher one it can run at once
 * moving, and in between it misses steps: on a GK7205V510 pan/tilt head,
 * 40-step moves started cold at 700us per micro-step lost 2-22 steps where
 * 833us and slower lost none. So a move from rest starts at ramp_start_us
 * and accelerates to its own delay over ramp_microsteps (constant
 * acceleration: speed squared grows by the same amount every micro-step). A
 * delay of ramp_start_us or longer runs flat, exactly as before.
 *
 * A caller that drives continuous motion as a train of short moves (the
 * majestic-af plugin issues 2 steps per ioctl while a direction is held)
 * must not be sent back to rest by every move. Each axis therefore keeps the
 * coils energised for hold_ms after a move and remembers its direction and
 * speed: a move in the same direction that starts within two micro-step
 * periods of the last one carries on at that speed, and anything else - a
 * reversal, a pause, released coils - ramps up again. Holding the field over
 * the gap also stops the rotor where the last micro-step put it, rather than
 * letting it coast on when a fast move ends.
 */
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/workqueue.h>

#include "gpiostep.h"

/* Default pin map: pan (roll) = 3,4,72,73 ; tilt (pitch) = 69,59,58,57. */
static int pan_gpios[4] = { 3, 4, 72, 73 };
static int tilt_gpios[4] = { 69, 59, 58, 57 };
/* module_param_array() writes the count only when the parameter is given, so
 * these start at the length of the defaults: a plain insmod uses the map above
 * rather than refusing it as "0 pins". */
static int n_pan = 4, n_tilt = 4;
module_param_array(pan_gpios, int, &n_pan, 0444);
MODULE_PARM_DESC(pan_gpios, "4 GPIO numbers for the pan coil");
module_param_array(tilt_gpios, int, &n_tilt, 0444);
MODULE_PARM_DESC(tilt_gpios, "4 GPIO numbers for the tilt coil");

static int ramp_start_us = 2000;
module_param(ramp_start_us, int, 0644);
MODULE_PARM_DESC(ramp_start_us,
		 "Per-micro-step delay a move from rest starts at, us (0 = no ramp)");
static int ramp_microsteps = 64;
module_param(ramp_microsteps, int, 0644);
MODULE_PARM_DESC(ramp_microsteps,
		 "Micro-steps to accelerate from ramp_start_us to a move's delay");
static int hold_ms = 20;
module_param(hold_ms, int, 0644);
MODULE_PARM_DESC(hold_ms, "How long the coils stay energised after a move, ms");

static const int step_seq[8][4] = {
	{ 1, 0, 0, 0 }, { 1, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 1, 0 },
	{ 0, 0, 1, 0 }, { 0, 0, 1, 1 }, { 0, 0, 0, 1 }, { 1, 0, 0, 1 }
};

static const int rev_step_seq[8][4] = {
	{ 1, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 1, 1 }, { 0, 0, 1, 0 },
	{ 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 1, 1, 0, 0 }, { 1, 0, 0, 0 }
};

static DEFINE_MUTEX(gpiostep_lock);

/* One coil and what it was last doing. All under gpiostep_lock. */
struct axis {
	const int *pins;
	bool energised;		/* field on since the last move */
	int dir;		/* direction of the last move */
	u64 speed2;		/* (micro-steps/s)^2 reached at its end */
	int last_us;		/* its last micro-step delay */
	ktime_t end;		/* when it ended */
	struct delayed_work release;
};

static struct axis pan_axis = { .pins = pan_gpios };
static struct axis tilt_axis = { .pins = tilt_gpios };

static void coil_off(struct axis *ax)
{
	int i;

	for (i = 0; i < 4; i++)
		gpio_set_value(ax->pins[i], 0);
	ax->energised = false;
	ax->dir = 0;
}

static void release_work(struct work_struct *w)
{
	struct axis *ax = container_of(to_delayed_work(w), struct axis, release);
	s64 idle_ms;

	mutex_lock(&gpiostep_lock);
	idle_ms = ktime_ms_delta(ktime_get(), ax->end);
	if (ax->energised && idle_ms < hold_ms)
		/* a move ran while this waited for the lock: hold on for it */
		schedule_delayed_work(&ax->release,
				      msecs_to_jiffies(hold_ms - idle_ms));
	else if (ax->energised)
		coil_off(ax);
	mutex_unlock(&gpiostep_lock);
}

/*
 * usleep_range() runs on hrtimers, but without CONFIG_HIGH_RES_TIMERS those
 * expire with jiffy granularity, so a sub-tick sleep rounds up to the next
 * tick (10ms at HZ=100) exactly like a userspace usleep. On such a kernel,
 * busy-wait instead while the requested delay is under a quarter tick, where
 * that rounding would at least quadruple the step period; from a quarter tick
 * up, sleep and accept the
 * rounding, since the busy-wait cost grows with the delay while its benefit
 * shrinks. The cond_resched() keeps a move from monopolising the core: these
 * kernels are !SMP and !PREEMPT, so without it the encoder would not run at
 * all until the whole move finished. It also means the sub-tick pacing only
 * holds on an idle core - under load the yield can hand the core away for
 * several ticks between two micro-steps.
 *
 * A zero delay keeps its usleep_range(0, 1). That is an already-expired
 * hrtimer and returns at once - measured on a Hi3518EV200, 320 micro-steps at
 * delay 0 finish in under 10ms with either version of this module - so zero
 * has never had a floor; this just leaves that unchanged rather than sending
 * it down the busy-wait path, where the guard would be the only thing between
 * a negative and udelay().
 */
static void step_delay(int delay_us)
{
	if (delay_us > 0 && !IS_ENABLED(CONFIG_HIGH_RES_TIMERS) &&
	    delay_us < (int)(jiffies_to_usecs(1) / 4)) {
		/* udelay() on ARM is bounded at ~2ms per call; chunk it */
		while (delay_us > 1000) {
			udelay(1000);
			delay_us -= 1000;
		}
		udelay(delay_us);
		cond_resched();
		return;
	}

	usleep_range(delay_us, delay_us + (delay_us >> 4) + 1);
}

/* Micro-steps per second at a delay, and back; a zero delay is "as fast as
 * the loop goes", which no ramp can be computed towards. */
static u64 speed_of(int delay_us)
{
	return delay_us > 0 ? 1000000 / delay_us : 0;
}

static void axis_run(struct axis *ax, int steps, int delay_us)
{
	const int (*seq)[4] = (steps < 0) ? rev_step_seq : step_seq;
	int remaining = abs(steps);
	int dir = steps < 0 ? -1 : 1;
	u64 target2, start2, accel2, speed2;
	int micro, i, us;

	if (remaining == 0)
		return;

	target2 = speed_of(delay_us) * speed_of(delay_us);
	start2 = speed_of(ramp_start_us) * speed_of(ramp_start_us);
	/* No ramp: none configured, a delay at or under it, or a zero delay. */
	if (ramp_start_us <= 0 || ramp_microsteps <= 0 || delay_us <= 0 ||
	    target2 <= start2) {
		accel2 = 0;
		speed2 = target2;
	} else {
		accel2 = div_u64(target2 - start2, ramp_microsteps);
		speed2 = start2;
		/* Still moving the same way: carry on at the speed it had. */
		if (ax->energised && ax->dir == dir &&
		    ktime_us_delta(ktime_get(), ax->end) <= 2 * ax->last_us)
			speed2 = clamp(ax->speed2, start2, target2);
	}

	cancel_delayed_work(&ax->release);
	us = delay_us;
	micro = 0;
	while (remaining > 0) {
		for (i = 0; i < 4; i++)
			gpio_set_value(ax->pins[i], seq[micro][i]);

		if (accel2) {
			us = (int)div_u64(1000000, int_sqrt((unsigned long)speed2));
			if (us < delay_us)
				us = delay_us;
			speed2 = min(speed2 + accel2, target2);
		}
		step_delay(us);

		if (++micro >= 8) {
			micro = 0;
			--remaining;
		}
	}

	ax->energised = true;
	ax->dir = dir;
	ax->speed2 = speed2;
	ax->last_us = us;
	ax->end = ktime_get();
	/* de-energise the coil once the axis has been idle for hold_ms */
	schedule_delayed_work(&ax->release, msecs_to_jiffies(max(hold_ms, 0)));
}

static long gpiostep_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	struct gpiostep_move m;

	if (cmd != GPIOSTEP_MOVE)
		return -ENOTTY;

	if (copy_from_user(&m, (void __user *)arg, sizeof(m)))
		return -EFAULT;

	if (m.delay_us < 0)
		return -EINVAL;

	mutex_lock(&gpiostep_lock);
	axis_run(&pan_axis, m.pan, m.delay_us);
	axis_run(&tilt_axis, m.tilt, m.delay_us);
	mutex_unlock(&gpiostep_lock);

	return 0;
}

static const struct file_operations gpiostep_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = gpiostep_ioctl,
};

static struct miscdevice gpiostep_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = GPIOSTEP_DEV_NAME,
	.fops = &gpiostep_fops,
};

static int request_coil(const int pins[4], const char *label)
{
	int i, ret;

	for (i = 0; i < 4; i++) {
		ret = gpio_request(pins[i], label);
		if (ret) {
			pr_err("gpiostep: %s gpio %d request failed (%d)\n",
			       label, pins[i], ret);
			while (--i >= 0)
				gpio_free(pins[i]);
			return ret;
		}
		gpio_direction_output(pins[i], 0);
	}
	return 0;
}

static void free_coil(const int pins[4])
{
	int i;

	for (i = 0; i < 4; i++)
		gpio_free(pins[i]);
}

static int __init gpiostep_init(void)
{
	int ret;

	if (n_pan != 4 || n_tilt != 4) {
		pr_err("gpiostep: need exactly 4 pan_gpios and 4 tilt_gpios\n");
		return -EINVAL;
	}

	ret = request_coil(pan_gpios, "gpiostep-pan");
	if (ret)
		return ret;

	ret = request_coil(tilt_gpios, "gpiostep-tilt");
	if (ret) {
		free_coil(pan_gpios);
		return ret;
	}

	INIT_DELAYED_WORK(&pan_axis.release, release_work);
	INIT_DELAYED_WORK(&tilt_axis.release, release_work);

	ret = misc_register(&gpiostep_misc);
	if (ret) {
		free_coil(pan_gpios);
		free_coil(tilt_gpios);
		return ret;
	}

	pr_info("gpiostep: ready, pan=%d,%d,%d,%d tilt=%d,%d,%d,%d via /dev/%s\n",
		pan_gpios[0], pan_gpios[1], pan_gpios[2], pan_gpios[3],
		tilt_gpios[0], tilt_gpios[1], tilt_gpios[2], tilt_gpios[3],
		GPIOSTEP_DEV_NAME);
	return 0;
}

static void __exit gpiostep_exit(void)
{
	misc_deregister(&gpiostep_misc);
	cancel_delayed_work_sync(&pan_axis.release);
	cancel_delayed_work_sync(&tilt_axis.release);
	coil_off(&pan_axis);
	coil_off(&tilt_axis);
	free_coil(pan_gpios);
	free_coil(tilt_gpios);
}

module_init(gpiostep_init);
module_exit(gpiostep_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("OpenIPC in-kernel GPIO half-step pan/tilt stepper driver");
MODULE_AUTHOR("OpenIPC");
