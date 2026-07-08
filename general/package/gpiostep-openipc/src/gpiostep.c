// SPDX-License-Identifier: GPL-2.0
/*
 * gpiostep - in-kernel GPIO half-step pan/tilt stepper driver.
 *
 * A clean reimplementation of the vendor "gpioStep"/"motor" behaviour observed
 * on Goke GK7205V510 cameras (model NC-IPTC2200_DL): two 4-wire stepper coils
 * driven over GPIO. Unlike the userspace gpio-motors tool (which does
 * open/write/close on /sys/class/gpio per pin per microstep), the stepping here
 * runs entirely in kernel context with direct gpio_set_value(), so timing is
 * far steadier and CPU cost much lower.
 *
 * Control is via a misc char device /dev/motorDev and a single ioctl. The pin
 * map defaults to the GK7205V510 layout and is overridable with module params:
 *
 *   insmod gpiostep.ko pan_gpios=3,4,72,73 tilt_gpios=69,59,58,57
 *
 * Step semantics (one "step" == one full 8-phase cycle) and the half-step
 * sequence are identical to general/package/gpio-motors/src/gpio-motors.c.
 */
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#include "gpiostep.h"

/* Default pin map: pan (roll) = 3,4,72,73 ; tilt (pitch) = 69,59,58,57. */
static int pan_gpios[4] = { 3, 4, 72, 73 };
static int tilt_gpios[4] = { 69, 59, 58, 57 };
static int n_pan, n_tilt;
module_param_array(pan_gpios, int, &n_pan, 0444);
MODULE_PARM_DESC(pan_gpios, "4 GPIO numbers for the pan coil");
module_param_array(tilt_gpios, int, &n_tilt, 0444);
MODULE_PARM_DESC(tilt_gpios, "4 GPIO numbers for the tilt coil");

static const int step_seq[8][4] = {
	{ 1, 0, 0, 0 }, { 1, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 1, 0 },
	{ 0, 0, 1, 0 }, { 0, 0, 1, 1 }, { 0, 0, 0, 1 }, { 1, 0, 0, 1 }
};

static const int rev_step_seq[8][4] = {
	{ 1, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 1, 1 }, { 0, 0, 1, 0 },
	{ 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 1, 1, 0, 0 }, { 1, 0, 0, 0 }
};

static DEFINE_MUTEX(gpiostep_lock);

static void axis_run(const int pins[4], int steps, int delay_us)
{
	const int (*seq)[4] = (steps < 0) ? rev_step_seq : step_seq;
	int remaining = abs(steps);
	int micro, i;

	if (remaining == 0)
		return;

	micro = 0;
	while (remaining > 0) {
		for (i = 0; i < 4; i++)
			gpio_set_value(pins[i], seq[micro][i]);

		usleep_range(delay_us, delay_us + (delay_us >> 4) + 1);

		if (++micro >= 8) {
			micro = 0;
			--remaining;
		}
	}

	/* de-energise the coil when idle */
	for (i = 0; i < 4; i++)
		gpio_set_value(pins[i], 0);
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
	axis_run(pan_gpios, m.pan, m.delay_us);
	axis_run(tilt_gpios, m.tilt, m.delay_us);
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
	free_coil(pan_gpios);
	free_coil(tilt_gpios);
}

module_init(gpiostep_init);
module_exit(gpiostep_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("OpenIPC in-kernel GPIO half-step pan/tilt stepper driver");
MODULE_AUTHOR("OpenIPC");
