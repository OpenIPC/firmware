/*
 * motor.c - Ingenic motor driver
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 *       http://www.ingenic.com
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/clk.h>
#include <linux/pwm.h>
#include <linux/file.h>
#include <linux/list.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/kthread.h>
#include <linux/mfd/core.h>
#include <linux/mempolicy.h>
#include <linux/interrupt.h>
#ifdef CONFIG_SOC_T40
#include <linux/mfd/ingenic-tcu.h>
#else
#include <linux/mfd/jz_tcu.h>
#endif
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#ifdef CONFIG_SOC_T40
#include <dt-bindings/interrupt-controller/t40-irq.h>
#else
#include <soc/irq.h>
#include <mach/platform.h>
#endif

#include <soc/base.h>
#include <soc/extal.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <soc/gpio.h>
#include "motor.h"
#define JZ_MOTOR_DRIVER_VERSION "H20171204a-16-jul-23-openipc"

#define HMOTOR2VMOTORRATIO 1
static unsigned int hmotor2vmotor = 1;
module_param(hmotor2vmotor, int, S_IRUGO);
MODULE_PARM_DESC(hmotor2vmotor, "The value is hmotor's speed / vmotor's");


static unsigned int hmaxstep = 4300;
module_param(hmaxstep, int, S_IRUGO);
MODULE_PARM_DESC(hmaxstep, "The max steps of horizontal motor");

static unsigned int vmaxstep = 300;
module_param(vmaxstep, int, S_IRUGO);
MODULE_PARM_DESC(vmaxstep, "The max steps of vertical motor");

/* custom definition for GPIOs (so it's generic rather than hardcoded) */
int hmin = -1;
module_param(hmin, int, S_IRUGO);
MODULE_PARM_DESC(hmin, "Horizontal motor start point GPIO");

int hmax = -1;
module_param(hmax, int, S_IRUGO);
MODULE_PARM_DESC(hmax, "Horizontal motor stop point GPIO");

int hlevel = 0;
module_param(hlevel, int, S_IRUGO);
MODULE_PARM_DESC(hlevel, "Horizontal motor IRQ style");

int hst1 = 49;
module_param(hst1, int, S_IRUGO);
MODULE_PARM_DESC(hst1, "Horizontal motor GPIO for Phase A. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");

int hst2 = 57;
module_param(hst2, int, S_IRUGO);
MODULE_PARM_DESC(hst2, "Horizontal motor GPIO for Phase B. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");

int hst3 = 54;
module_param(hst3, int, S_IRUGO);
MODULE_PARM_DESC(hst3, "Horizontal motor GPIO for Phase C. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");

int hst4 = 51;
module_param(hst4, int, S_IRUGO);
MODULE_PARM_DESC(hst4, "Horizontal motor GPIO for Phase D. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");

int vmin = -1;
module_param(vmin, int, S_IRUGO);
MODULE_PARM_DESC(vmin, "Vertical motor start point GPIO");

int vmax = -1;
module_param(vmax, int, S_IRUGO);
MODULE_PARM_DESC(vmax, "Vertical motor stop point GPIO");

int vlevel = 0;
module_param(vlevel, int, S_IRUGO);
MODULE_PARM_DESC(vlevel, "Vertical motor IRQ style");

int vst1 = 60;
module_param(vst1, int, S_IRUGO);
MODULE_PARM_DESC(vst1, "Vertical motor GPIO for Phase A. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");

int vst2 = 61;
module_param(vst2, int, S_IRUGO);
MODULE_PARM_DESC(vst2, "Vertical motor GPIO for Phase B. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");

int vst3 = 62;
module_param(vst3, int, S_IRUGO);
MODULE_PARM_DESC(vst3, "Vertical motor GPIO for Phase C. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");

int vst4 = 63;
module_param(vst4, int, S_IRUGO);
MODULE_PARM_DESC(vst4, "Vertical motor GPIO for Phase D. Use absolute GPIO index (i.e. GPIOB is 32 + gpio pin, GPIOC is 64 + gpio pin, etc)");


struct motor_platform_data motors_pdata[HAS_MOTOR_CNT] = {
	{
		.name = "Horizontal motor",
		.motor_min_gpio		= -1,
		.motor_max_gpio 	= -1,
		.motor_gpio_level	= -1,
		.motor_st1_gpio		= -1,
		.motor_st2_gpio		= -1,
		.motor_st3_gpio		= -1,
		.motor_st4_gpio		= -1,
	},
	{
		.name = "Vertical motor",
		.motor_min_gpio		= -1,
		.motor_max_gpio 	= -1,
		.motor_gpio_level	= -1,
		.motor_st1_gpio		= -1,
		.motor_st2_gpio		= -1,
		.motor_st3_gpio		= -1,
		.motor_st4_gpio		= -1,
	},
};

static void motor_set_default(struct motor_device *mdev)
{
	int index = 0;
	struct motor_driver *motor = NULL;
	mdev->dev_state = MOTOR_OPS_STOP;
	for(index = 0; index < HAS_MOTOR_CNT; index++){
		motor =  &mdev->motors[index];
		motor->state = MOTOR_OPS_STOP;
		if (motor->pdata->motor_st1_gpio)
			gpio_direction_output(motor->pdata->motor_st1_gpio, 0);
		if (motor->pdata->motor_st2_gpio)
			gpio_direction_output(motor->pdata->motor_st2_gpio, 0);
		if (motor->pdata->motor_st3_gpio)
			gpio_direction_output(motor->pdata->motor_st3_gpio, 0);
		if (motor->pdata->motor_st4_gpio)
			gpio_direction_output(motor->pdata->motor_st4_gpio, 0);
	}
	return;
}

static unsigned char step_8[8] = {
	0x08,
	0x0c,
	0x04,
	0x06,
	0x02,
	0x03,
	0x01,
	0x09
};

static void motor_move_step(struct motor_device *mdev, int index)
{
	struct motor_driver *motor = NULL;
	int step = 0;

	motor =  &mdev->motors[index];
	if(motor->state != MOTOR_OPS_STOP){
		step = motor->cur_steps % 8;
		step = step < 0 ? step + 8 : step;
		if (motor->pdata->motor_st1_gpio)
			gpio_direction_output(motor->pdata->motor_st1_gpio, step_8[step] & 0x8);
		if (motor->pdata->motor_st2_gpio)
			gpio_direction_output(motor->pdata->motor_st2_gpio, step_8[step] & 0x4);
		if (motor->pdata->motor_st3_gpio)
			gpio_direction_output(motor->pdata->motor_st3_gpio, step_8[step] & 0x2);
		if (motor->pdata->motor_st4_gpio)
			gpio_direction_output(motor->pdata->motor_st4_gpio, step_8[step] & 0x1);
	}else{
		if (motor->pdata->motor_st1_gpio)
			gpio_direction_output(motor->pdata->motor_st1_gpio, 0);
		if (motor->pdata->motor_st2_gpio)
			gpio_direction_output(motor->pdata->motor_st2_gpio, 0);
		if (motor->pdata->motor_st3_gpio)
			gpio_direction_output(motor->pdata->motor_st3_gpio, 0);
		if (motor->pdata->motor_st4_gpio)
			gpio_direction_output(motor->pdata->motor_st4_gpio, 0);
	}
	if(motor->state == MOTOR_OPS_RESET){
		motor->total_steps++;
	}

	return;
}

static void move_to_min_pose_ops(struct motor_driver *motor)
{
	//	printk("%s min %d\n",motor->pdata->name,__LINE__);
	if(motor->state == MOTOR_OPS_RESET){
		/* motor->state = MOTOR_OPS_STOP; */
		/* //	complete(&motor->reset_completion); */
		/* motor->move_dir = MOTOR_MOVE_RIGHT_UP; */
	}else if(motor->state == MOTOR_OPS_NORMAL){
		if(motor->move_dir == MOTOR_MOVE_LEFT_DOWN){
			motor->state = MOTOR_OPS_STOP;
		}
	}else
		motor->move_dir = MOTOR_MOVE_RIGHT_UP;

	motor->cur_steps = 0;
	//printk("%s min; cur_steps = %d max_steps = %d\n", motor->pdata->name,motor->cur_steps, motor->max_steps);
}

static void move_to_max_pose_ops(struct motor_driver *motor,int index)
{
	if(motor->state == MOTOR_OPS_RESET){
		motor->state = MOTOR_OPS_STOP;
		if(index ==HORIZONTAL_MOTOR)
			motor->max_steps=hmaxstep;
		else
			motor->max_steps=vmaxstep;
		complete(&motor->reset_completion);
		motor->move_dir = MOTOR_MOVE_LEFT_DOWN;
	}else if(motor->state == MOTOR_OPS_NORMAL){
		if(motor->move_dir == MOTOR_MOVE_RIGHT_UP){
			motor->state = MOTOR_OPS_STOP;
		}
	}else
		motor->move_dir = MOTOR_MOVE_LEFT_DOWN;
	motor->cur_steps = motor->max_steps;
	//printk("%s max; cur_steps = %d max_steps = %d\n", motor->pdata->name,motor->cur_steps, motor->max_steps);
}


static char skip_move_mode[4][4] = {{2,0,0,0},
				    {3,2,0,0},
				    {4,3,2,0},
				    {4,3,2,1}};

static inline void calc_slow_mode(struct motor_device *mdev, unsigned int steps)
{
	int index = steps / 10;
	index = index > 3 ? 3 : index;
	mdev->skip_mode = skip_move_mode[index];
}

/* return: 1 --> move, 0 --> don't move */
static inline int whether_move_func(struct motor_device *mdev, unsigned int remainder)
{
	if(remainder == 0)
		return 0;

	remainder = remainder / 10;
	remainder = remainder > 3 ? 3: remainder;
	if(mdev->counter % mdev->skip_mode[remainder] == 0)
		return 1;
	else
		return 0;
}

static irqreturn_t jz_timer_interrupt(int irq, void *dev_id)
{
	struct motor_device *mdev = dev_id;
	struct motor_move *dst = &mdev->dst_move;
	struct motor_move *cur = &mdev->cur_move;
	struct motor_driver *motors = mdev->motors;

	if(motors[HORIZONTAL_MOTOR].state == MOTOR_OPS_STOP
			&& motors[VERTICAL_MOTOR].state == MOTOR_OPS_STOP){
		mdev->dev_state = MOTOR_OPS_STOP;
		motor_move_step(mdev, HORIZONTAL_MOTOR);
		motor_move_step(mdev, VERTICAL_MOTOR);
		if(mdev->wait_stop){
			mdev->wait_stop = 0;
			complete(&mdev->stop_completion);
		}
		return IRQ_HANDLED;
	}

	if(motors[HORIZONTAL_MOTOR].cur_steps <= 0)
		move_to_min_pose_ops(&motors[HORIZONTAL_MOTOR]);

	if(motors[HORIZONTAL_MOTOR].cur_steps >= motors[HORIZONTAL_MOTOR].max_steps)
		move_to_max_pose_ops(&motors[HORIZONTAL_MOTOR],HORIZONTAL_MOTOR);

	if(motors[VERTICAL_MOTOR].cur_steps <= 0)
		move_to_min_pose_ops(&motors[VERTICAL_MOTOR]);

	if(motors[VERTICAL_MOTOR].cur_steps >= motors[VERTICAL_MOTOR].max_steps)
		move_to_max_pose_ops(&motors[VERTICAL_MOTOR],VERTICAL_MOTOR);

	if(mdev->dev_state == MOTOR_OPS_CRUISE){
		mdev->counter++;
		motors[HORIZONTAL_MOTOR].cur_steps += motors[HORIZONTAL_MOTOR].move_dir;
		if(mdev->counter % hmotor2vmotor == 0)
			motors[VERTICAL_MOTOR].cur_steps += motors[VERTICAL_MOTOR].move_dir;
		motor_move_step(mdev, HORIZONTAL_MOTOR);
		motor_move_step(mdev, VERTICAL_MOTOR);
	}else if(mdev->dev_state == MOTOR_OPS_RESET){
		if(motors[HORIZONTAL_MOTOR].state != MOTOR_OPS_STOP){
			motors[HORIZONTAL_MOTOR].cur_steps += motors[HORIZONTAL_MOTOR].move_dir;
			motor_move_step(mdev, HORIZONTAL_MOTOR);
			cur->one.x++;
		}
		if(motors[VERTICAL_MOTOR].state != MOTOR_OPS_STOP){
			motors[VERTICAL_MOTOR].cur_steps += motors[VERTICAL_MOTOR].move_dir;
			motor_move_step(mdev, VERTICAL_MOTOR);
			cur->one.y++;
		}
	}else{
		mdev->counter++;

		if(cur->one.x < dst->one.x && motors[HORIZONTAL_MOTOR].state != MOTOR_OPS_STOP){
			if(whether_move_func(mdev, dst->one.x - cur->one.x)){
				motors[HORIZONTAL_MOTOR].cur_steps += motors[HORIZONTAL_MOTOR].move_dir;
				motor_move_step(mdev, HORIZONTAL_MOTOR);
				cur->one.x++;
			}
		}else{
			motors[HORIZONTAL_MOTOR].state = MOTOR_OPS_STOP;
		}

		if(cur->one.y < dst->one.y && motors[VERTICAL_MOTOR].state != MOTOR_OPS_STOP){
			if(mdev->counter % hmotor2vmotor == 0){
				motors[VERTICAL_MOTOR].cur_steps += motors[VERTICAL_MOTOR].move_dir;
				cur->one.y++;
				motor_move_step(mdev, VERTICAL_MOTOR);
			}
		}else{
			motors[VERTICAL_MOTOR].state = MOTOR_OPS_STOP;
		}
	}
	return IRQ_HANDLED;
}

static long motor_ops_move(struct motor_device *mdev, int x, int y)
{
	struct motor_driver *motors = mdev->motors;
	unsigned long flags;
	int x_dir = MOTOR_MOVE_STOP;
	int y_dir = MOTOR_MOVE_STOP;
	int x1 = 0;
	int y1 = 0;
	/* check x value */
	if(x > 0){
		if(motors[HORIZONTAL_MOTOR].cur_steps >= motors[HORIZONTAL_MOTOR].max_steps)
			x = 0;
	}else{
		if(motors[HORIZONTAL_MOTOR].cur_steps <= 0)
			x = 0;
	}
	/* check y value */
	if(y > 0){
		if(motors[VERTICAL_MOTOR].cur_steps >= motors[VERTICAL_MOTOR].max_steps)
			y = 0;
	}else{
		if(motors[VERTICAL_MOTOR].cur_steps <= 0)
			y = 0;
	}

	/*x_dir = x > 0 ? MOTOR_MOVE_RIGHT_UP : (x < 0 ? MOTOR_MOVE_LEFT_DOWN: MOTOR_MOVE_STOP);*/
	/*y_dir = y > 0 ? MOTOR_MOVE_RIGHT_UP : (y < 0 ? MOTOR_MOVE_LEFT_DOWN: MOTOR_MOVE_STOP);*/
	x_dir = x > 0 ? MOTOR_MOVE_RIGHT_UP : MOTOR_MOVE_LEFT_DOWN;
	y_dir = y > 0 ? MOTOR_MOVE_RIGHT_UP : MOTOR_MOVE_LEFT_DOWN;
	x1 = x < 0 ? 0 - x : x;
	y1 = y < 0 ? 0 - y : y;

	if(x1 + y1 == 0)
		return 0;

	mutex_lock(&mdev->dev_mutex);
	spin_lock_irqsave(&mdev->slock, flags);

	calc_slow_mode(mdev, x1);
	mdev->counter = 0;
	mdev->dev_state = MOTOR_OPS_NORMAL;
	mdev->dst_move.one.x = x1;
	mdev->dst_move.one.y = y1;
	mdev->cur_move.one.x = 0;
	mdev->cur_move.one.y = 0;
	motors[HORIZONTAL_MOTOR].state = MOTOR_OPS_NORMAL;
	motors[HORIZONTAL_MOTOR].move_dir = x_dir;
	motors[VERTICAL_MOTOR].state = MOTOR_OPS_NORMAL;
	motors[VERTICAL_MOTOR].move_dir = y_dir;
	spin_unlock_irqrestore(&mdev->slock, flags);
	mutex_unlock(&mdev->dev_mutex);
	/* printk("%s%d x=%d y=%d t=%d\n",__func__,__LINE__,mdev->dst_move.one.x,mdev->dst_move.one.y,mdev->dst_move.times); */
	/* printk("x_dir=%d,y_dir=%d\n",x_dir,y_dir); */
#ifdef CONFIG_SOC_T40
	ingenic_tcu_counter_begin(mdev->tcu);
#else
	jz_tcu_enable_counter(mdev->tcu);
#endif
	return 0;
}

static void motor_ops_stop(struct motor_device *mdev)
{
	unsigned long flags;
	long ret = 0;
	unsigned int remainder = 0;
	struct motor_driver *motors = mdev->motors;
	struct motor_move *dst = &mdev->dst_move;
	struct motor_move *cur = &mdev->cur_move;


	if(mdev->dev_state == MOTOR_OPS_STOP)
		return;

	mutex_lock(&mdev->dev_mutex);
	spin_lock_irqsave(&mdev->slock, flags);

	if(mdev->dev_state == MOTOR_OPS_NORMAL){
		remainder = dst->one.x - cur->one.x;
		if(remainder > 30){
			dst->one.x = 29;
			cur->one.x = 0;
		}
		remainder = dst->one.y - cur->one.y;
		if(remainder > 8){
			dst->one.y = 6;
			cur->one.y = 0;
		}
	}

	if(mdev->dev_state == MOTOR_OPS_CRUISE){
		mdev->dev_state = MOTOR_OPS_NORMAL;
		motors[HORIZONTAL_MOTOR].state = MOTOR_OPS_NORMAL;
		motors[VERTICAL_MOTOR].state = MOTOR_OPS_NORMAL;
		dst->one.x = 0;
		cur->one.x = 0;
		dst->one.y = 0;
		cur->one.y = 0;
	}

	mdev->counter = 0;
	mdev->wait_stop = 1;
	spin_unlock_irqrestore(&mdev->slock, flags);
	mutex_unlock(&mdev->dev_mutex);
	do{
		ret = wait_for_completion_interruptible_timeout(&mdev->stop_completion, msecs_to_jiffies(15000));
		if(ret == 0){
			ret = -ETIMEDOUT;
			break;
		}
	}while(ret == -ERESTARTSYS);
#ifdef CONFIG_SOC_T40
	ingenic_tcu_counter_stop(mdev->tcu);
#else
	jz_tcu_disable_counter(mdev->tcu);
#endif
	/*mdev->dev_state = MOTOR_OPS_STOP;*/
	/*motors[HORIZONTAL_MOTOR].state = MOTOR_OPS_STOP;*/
	/*motors[VERTICAL_MOTOR].state = MOTOR_OPS_STOP;*/
	motor_set_default(mdev);
	return;
}

static long motor_ops_goback(struct motor_device *mdev)
{
	struct motor_driver *motors = mdev->motors;
	int sx, sy;
	int cx, cy;
	sx = motors[HORIZONTAL_MOTOR].max_steps >> 1;
	sy = motors[VERTICAL_MOTOR].max_steps >> 1;
	cx = motors[HORIZONTAL_MOTOR].cur_steps;
	cy = motors[VERTICAL_MOTOR].cur_steps;
	//printk("sx=%d,sy=%d,cx=%d,cy=%d\n",sx,sy,cx,cy);
	return motor_ops_move(mdev, sx-cx, sy-cy);
}

static long motor_ops_cruise(struct motor_device *mdev)
{
	unsigned long flags;
	struct motor_driver *motors = mdev->motors;
	motor_ops_goback(mdev);
	mutex_lock(&mdev->dev_mutex);
	spin_lock_irqsave(&mdev->slock, flags);
	mdev->dev_state = MOTOR_OPS_CRUISE;
	motors[HORIZONTAL_MOTOR].state = MOTOR_OPS_CRUISE;
	motors[VERTICAL_MOTOR].state = MOTOR_OPS_CRUISE;
	spin_unlock_irqrestore(&mdev->slock, flags);
	mutex_unlock(&mdev->dev_mutex);
#ifdef CONFIG_SOC_T40
	ingenic_tcu_counter_begin(mdev->tcu);
#else
	jz_tcu_enable_counter(mdev->tcu);
#endif
	return 0;
}

static void motor_get_message(struct motor_device *mdev, struct motor_message *msg)
{
	struct motor_driver *motors = mdev->motors;
	msg->x = motors[HORIZONTAL_MOTOR].cur_steps;
	msg->y = motors[VERTICAL_MOTOR].cur_steps;
	msg->speed = mdev->tcu_speed;
	if(mdev->dev_state == MOTOR_OPS_STOP)
		msg->status = MOTOR_IS_STOP;
	else
		msg->status = MOTOR_IS_RUNNING;
	/* This is not standard, return the max_steps of each motor */
	msg->x_max_steps = motors[HORIZONTAL_MOTOR].max_steps;
	msg->y_max_steps = motors[VERTICAL_MOTOR].max_steps;
	return;
}

static inline int motor_ops_reset_check_params(struct motor_reset_data *rdata)
{
	if(rdata->x_max_steps == 0 || rdata->y_max_steps == 0){
		return -1;
	}
	if(rdata->x_max_steps < rdata->x_cur_step || rdata->x_max_steps < rdata->x_cur_step)
		return -1;
	return 0;
}

static long motor_ops_reset(struct motor_device *mdev, struct motor_reset_data *rdata)
{
	unsigned long flags;
	int index = 0;
	long ret = 0;
	int times = 0;
	struct motor_message msg;
	printk("%s%d\n",__func__,__LINE__);

	if(mdev == NULL || rdata == NULL){
		printk("ERROR: the parameters of %s is wrong!!\n",__func__);
		return -EPERM;
	}

	if(motor_ops_reset_check_params(rdata) == 0){
		/* app set max steps and current pos */
		mutex_lock(&mdev->dev_mutex);
		spin_lock_irqsave(&mdev->slock, flags);
		mdev->motors[HORIZONTAL_MOTOR].max_steps = rdata->x_max_steps;
		mdev->motors[HORIZONTAL_MOTOR].cur_steps = rdata->x_cur_step;
		mdev->motors[VERTICAL_MOTOR].max_steps = rdata->y_max_steps;
		mdev->motors[VERTICAL_MOTOR].cur_steps = rdata->y_cur_step;
		spin_unlock_irqrestore(&mdev->slock, flags);
		mutex_unlock(&mdev->dev_mutex);
	}else{
		/* driver calculate max steps. */
		mutex_lock(&mdev->dev_mutex);
		spin_lock_irqsave(&mdev->slock, flags);
		for(index = 0; index < HAS_MOTOR_CNT; index++){
			mdev->motors[index].move_dir = MOTOR_MOVE_RIGHT_UP;
			mdev->motors[index].state = MOTOR_OPS_RESET;
			mdev->motors[index].cur_steps = 0x0;
		}
		mdev->dst_move.one.x = mdev->motors[HORIZONTAL_MOTOR].max_steps;
		mdev->dst_move.one.y = mdev->motors[VERTICAL_MOTOR].max_steps;
		mdev->dst_move.times = 1;
		mdev->cur_move.one.x = 0;
		mdev->cur_move.one.y = 0;
		mdev->cur_move.times = 0;
		mdev->dev_state = MOTOR_OPS_RESET;
		spin_unlock_irqrestore(&mdev->slock, flags);
		mutex_unlock(&mdev->dev_mutex);
#ifdef CONFIG_SOC_T40
		ingenic_tcu_counter_begin(mdev->tcu);
#else
		jz_tcu_enable_counter(mdev->tcu);
#endif

		for(index = 0; index < HAS_MOTOR_CNT; index++){
			do{
				ret = wait_for_completion_interruptible_timeout(&mdev->motors[index].reset_completion, msecs_to_jiffies(150000));
				if(ret == 0){
					ret = -ETIMEDOUT;
					goto exit;
				}
			}while(ret == -ERESTARTSYS);
		}
	}
	//printk("x_max = %d, y_max = %d\n", mdev->motors[HORIZONTAL_MOTOR].max_steps,
			//mdev->motors[VERTICAL_MOTOR].max_steps);
	ret = motor_ops_goback(mdev);
	/*ret =  motor_ops_move(mdev, (mdev->motors[HORIZONTAL_MOTOR].max_steps) >> 1, */
			/*(mdev->motors[VERTICAL_MOTOR].max_steps) >> 1);*/

	do{
		msleep(10);
		motor_get_message(mdev, &msg);
		times++;
		if(times > 1000){
			printk("ERROR:wait motor timeout %s%d\n",__func__,__LINE__);
			ret = -ETIMEDOUT;
			goto exit;
		}
	}while(msg.status == MOTOR_IS_RUNNING);
	ret = 0;

	/* sync data */
	 rdata->x_max_steps	= mdev->motors[HORIZONTAL_MOTOR].max_steps;
	 rdata->x_cur_step	= mdev->motors[HORIZONTAL_MOTOR].cur_steps;
	 rdata->y_max_steps	= mdev->motors[VERTICAL_MOTOR].max_steps;
	 rdata->y_cur_step	= mdev->motors[VERTICAL_MOTOR].cur_steps;

exit:
#ifdef CONFIG_SOC_T40
	ingenic_tcu_counter_stop(mdev->tcu);
#else
	jz_tcu_disable_counter(mdev->tcu);
#endif
	msleep(10);
	motor_set_default(mdev);
	return ret;
}

static int motor_speed(struct motor_device *mdev, int speed)
{
	__asm__("ssnop");
	if ((speed < MOTOR_MIN_SPEED) || (speed > MOTOR_MAX_SPEED)) {
		dev_err(mdev->dev, "speed(%d) set error\n", speed);
		return -1;
	}
	__asm__("ssnop");

	mdev->tcu_speed = speed;
#ifdef CONFIG_SOC_T40
	ingenic_tcu_set_period(mdev->tcu->cib.id,(24000000 / 64 / mdev->tcu_speed));
#else
	jz_tcu_set_period(mdev->tcu, (24000000 / 64 / mdev->tcu_speed));
#endif
	return 0;
}

static int motor_open(struct inode *inode, struct file *file)
{
	struct miscdevice *dev = file->private_data;
	struct motor_device *mdev = container_of(dev, struct motor_device, misc_dev);
	int ret = 0;
	if(mdev->flag){
		ret = -EBUSY;
		dev_err(mdev->dev, "Motor driver busy now!\n");
	}else{
		mdev->flag = 1;
	}

	return ret;
}

static int motor_release(struct inode *inode, struct file *file)
{
	struct miscdevice *dev = file->private_data;
	struct motor_device *mdev = container_of(dev, struct motor_device, misc_dev);
	motor_ops_stop(mdev);
	mdev->flag = 0;
	return 0;
}

static long motor_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct miscdevice *dev = filp->private_data;
	struct motor_device *mdev = container_of(dev, struct motor_device, misc_dev);
	long ret = 0;

	if(mdev->flag == 0){
		printk("Please Open /dev/motor Firstly\n");
		return -EPERM;
	}

	switch (cmd) {
		case MOTOR_STOP:
			motor_ops_stop(mdev);
			/*printk("MOTOR_STOP!!!!!!!!!!!!!!!!!!!\n");*/
			break;
		case MOTOR_RESET:
			{
				struct motor_reset_data rdata;
				if(arg == 0){
					ret = -EPERM;
					break;
				}
				if (copy_from_user(&rdata, (void __user *)arg,
							sizeof(rdata))) {
					dev_err(mdev->dev, "[%s][%d] copy from user error\n",
							__func__, __LINE__);
					return -EFAULT;
				}
				ret = motor_ops_reset(mdev, &rdata);
				if(!ret){
					if (copy_to_user((void __user *)arg, &rdata,
								sizeof(rdata))) {
						dev_err(mdev->dev, "[%s][%d] copy to user error\n",
								__func__, __LINE__);
						return -EFAULT;
					}
				}
				/*printk("MOTOR_RESET!!!!!!!!!!!!!!!!!!!\n");*/
				break;
			}
		case MOTOR_MOVE:
			{
				struct motors_steps dst;
				if (copy_from_user(&dst, (void __user *)arg,
							sizeof(struct motors_steps))) {
					dev_err(mdev->dev, "[%s][%d] copy from user error\n",
							__func__, __LINE__);
					return -EFAULT;
				}
				ret = motor_ops_move(mdev, dst.x, dst.y);
				/*printk("MOTOR_MOVE!!!!!!!!!!!!!!!!!!!\n");*/
			}
			break;
		case MOTOR_GET_STATUS:
			{
				struct motor_message msg;

				motor_get_message(mdev, &msg);
				if (copy_to_user((void __user *)arg, &msg,
							sizeof(struct motor_message))) {
					dev_err(mdev->dev, "[%s][%d] copy to user error\n",
							__func__, __LINE__);
					return -EFAULT;
				}
			}
			/*printk("MOTOR_GET_STATUS!!!!!!!!!!!!!!!!!!\n");*/
			break;
		case MOTOR_SPEED:
			{
				int speed;

				if (copy_from_user(&speed, (void __user *)arg, sizeof(int))) {
					dev_err(mdev->dev, "[%s][%d] copy to user error\n", __func__, __LINE__);
					return -EFAULT;
				}

				motor_speed(mdev, speed);
			}
			/*printk("MOTOR_SPEED!!!!!!!!!!!!!!!!!!!!!!!\n");*/
			break;
		case MOTOR_GOBACK:
			/*printk("MOTOR_GOBACK!!!!!!!!!!!!!!!!!!!!!!!\n");*/
			ret = motor_ops_goback(mdev);
			break;
		case MOTOR_CRUISE:
			/*printk("MOTOR_CRUISE!!!!!!!!!!!!!!!!!!!!!!!\n");*/
			ret = motor_ops_cruise(mdev);
			break;
		default:
			return -EINVAL;
	}

	return ret;
}

static struct file_operations motor_fops = {
	.open = motor_open,
	.release = motor_release,
	.unlocked_ioctl = motor_ioctl,
};

static int motor_info_show(struct seq_file *m, void *v)
{
	int len = 0;
	struct motor_device *mdev = (struct motor_device *)(m->private);
	struct motor_message msg;
	int index = 0;

#ifdef CONFIG_SOC_T40
	seq_printf(m ,"The version of Motor driver is %s. SoC is T40\n",JZ_MOTOR_DRIVER_VERSION);
	seq_printf(m ,"Motor driver is %s\n", mdev->flag?"opened":"closed");
	seq_printf(m ,"The max speed is %d and the min speed is %d\n", MOTOR_MAX_SPEED, MOTOR_MIN_SPEED);
	motor_get_message(mdev, &msg);
	seq_printf(m ,"The status of motor is %s\n", msg.status?"running":"stop");
	seq_printf(m ,"The pos of motor is (%d, %d)\n", msg.x, msg.y);
	seq_printf(m ,"The speed of motor is %d\n", msg.speed);

	for(index = 0; index < HAS_MOTOR_CNT; index++){
		seq_printf(m ,"## motor is %s ##\n", mdev->motors[index].pdata->name);
		seq_printf(m ,"max steps %d\n", mdev->motors[index].max_steps);
		seq_printf(m ,"motor direction %d\n", mdev->motors[index].move_dir);
		seq_printf(m ,"motor state %d (normal; cruise; reset)\n", mdev->motors[index].state);
		seq_printf(m ,"the irq's counter of max pos is %d\n", mdev->motors[index].max_pos_irq_cnt);
		seq_printf(m ,"the irq's counter of min pos is %d\n", mdev->motors[index].min_pos_irq_cnt);
	}
#else
	len += seq_printf(m ,"The version of Motor driver is %s. SoC is not T40\n",JZ_MOTOR_DRIVER_VERSION);
	len += seq_printf(m ,"Motor driver is %s\n", mdev->flag?"opened":"closed");
	len += seq_printf(m ,"The max speed is %d and the min speed is %d\n", MOTOR_MAX_SPEED, MOTOR_MIN_SPEED);
	motor_get_message(mdev, &msg);
	len += seq_printf(m ,"The status of motor is %s\n", msg.status?"running":"stop");
	len += seq_printf(m ,"The pos of motor is (%d, %d)\n", msg.x, msg.y);
	len += seq_printf(m ,"The speed of motor is %d\n", msg.speed);

	for(index = 0; index < HAS_MOTOR_CNT; index++){
		len += seq_printf(m ,"## motor is %s ##\n", mdev->motors[index].pdata->name);
		len += seq_printf(m ,"GPIOs: Min %d, Max %d, Level %d, ST1 %d, ST2 %d, ST3 %d, ST4 %d\n",
			mdev->motors[index].pdata->motor_min_gpio,
			mdev->motors[index].pdata->motor_max_gpio,
			mdev->motors[index].pdata->motor_gpio_level,
			mdev->motors[index].pdata->motor_st1_gpio,
			mdev->motors[index].pdata->motor_st2_gpio,
			mdev->motors[index].pdata->motor_st3_gpio,
			mdev->motors[index].pdata->motor_st4_gpio);
		len += seq_printf(m ,"max steps %d\n", mdev->motors[index].max_steps);
		len += seq_printf(m ,"motor direction %d\n", mdev->motors[index].move_dir);
		len += seq_printf(m ,"motor state %d (normal; cruise; reset)\n", mdev->motors[index].state);
		len += seq_printf(m ,"the irq's counter of max pos is %d\n", mdev->motors[index].max_pos_irq_cnt);
		len += seq_printf(m ,"the irq's counter of min pos is %d\n", mdev->motors[index].min_pos_irq_cnt);
	}
#endif

	return len;
}

static int motor_info_open(struct inode *inode, struct file *file)
{
	return single_open_size(file, motor_info_show, PDE_DATA(inode),1024);
}

static const struct file_operations motor_info_fops ={
	.read = seq_read,
	.open = motor_info_open,
	.llseek = seq_lseek,
	.release = single_release,
};

static int motor_probe(struct platform_device *pdev)
{
	int i, ret = 0;
	struct motor_device *mdev;
	struct motor_driver *motor = NULL;
	struct proc_dir_entry *proc;
	mdev = devm_kzalloc(&pdev->dev, sizeof(struct motor_device), GFP_KERNEL);
	if (!mdev) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "kzalloc motor device memery error\n");
		goto error_devm_kzalloc;
	}

	mdev->cell = mfd_get_cell(pdev);
	if (!mdev->cell) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get mfd cell for motor_probe!\n");
		goto error_devm_kzalloc;
	}

	mdev->dev = &pdev->dev;

#ifdef CONFIG_SOC_T40
	mdev->tcu = (struct ingenic_tcu_chn *)mdev->cell->platform_data;
#else
	mdev->tcu = (struct jz_tcu_chn *)mdev->cell->platform_data;
#endif
	mdev->tcu->irq_type = FULL_IRQ_MODE;
	mdev->tcu->clk_src = TCU_CLKSRC_EXT;
	mdev->tcu_speed = MOTOR_MAX_SPEED;
#ifdef CONFIG_SOC_T40
	mdev->tcu->is_pwm = 0;
	mdev->tcu->cib.func = TRACKBALL_FUNC;
	mdev->tcu->clk_div = TCU_PRESCALE_64;
	ingenic_tcu_config(mdev->tcu);
	ingenic_tcu_set_period(mdev->tcu->cib.id,(24000000 / 64 / mdev->tcu_speed));
//	ingenic_tcu_counter_begin(mdev->tcu);
#else
	mdev->tcu->prescale = TCU_PRESCALE_64;
	jz_tcu_config_chn(mdev->tcu);
	jz_tcu_set_period(mdev->tcu, (24000000 / 64 / mdev->tcu_speed));
	jz_tcu_start_counter(mdev->tcu);
#endif
	mutex_init(&mdev->dev_mutex);
	spin_lock_init(&mdev->slock);

	platform_set_drvdata(pdev, mdev);

	/* copy module parameters to the motors struct */
	motors_pdata[0].motor_min_gpio = hmin;
	motors_pdata[0].motor_max_gpio = hmax;
	motors_pdata[0].motor_gpio_level = hlevel;
	motors_pdata[0].motor_st1_gpio = hst1;
	motors_pdata[0].motor_st2_gpio = hst2;
	motors_pdata[0].motor_st3_gpio = hst3;
	motors_pdata[0].motor_st4_gpio = hst4;

	motors_pdata[1].motor_min_gpio = vmin;
	motors_pdata[1].motor_max_gpio = vmax;
	motors_pdata[1].motor_gpio_level = vlevel;
	motors_pdata[1].motor_st1_gpio = vst1;
	motors_pdata[1].motor_st2_gpio = vst2;
	motors_pdata[1].motor_st3_gpio = vst3;
	motors_pdata[1].motor_st4_gpio = vst4;

	for(i = 0; i < HAS_MOTOR_CNT; i++) {
		motor = &(mdev->motors[i]);
		motor->pdata = &motors_pdata[i];
		motor->move_dir	= MOTOR_MOVE_STOP;
		dev_info(&pdev->dev, "'%s' GPIOs: Min %d, Max %d, Level %d, ST1 %d, ST2 %d, ST3 %d, ST4 %d\n",
			motor->pdata->name,
			motor->pdata->motor_min_gpio,
			motor->pdata->motor_max_gpio,
			motor->pdata->motor_gpio_level,
			motor->pdata->motor_st1_gpio,
			motor->pdata->motor_st2_gpio,
			motor->pdata->motor_st3_gpio,
			motor->pdata->motor_st4_gpio
		);

		init_completion(&motor->reset_completion);

		if (motor->pdata->motor_st1_gpio != -1) {
			gpio_request(motor->pdata->motor_st1_gpio, "motor_st1_gpio");
		}
		if (motor->pdata->motor_st2_gpio != -1) {
			gpio_request(motor->pdata->motor_st2_gpio, "motor_st2_gpio");
		}
		if (motor->pdata->motor_st3_gpio != -1) {
			gpio_request(motor->pdata->motor_st3_gpio, "motor_st3_gpio");
		}
		if (motor->pdata->motor_st4_gpio != -1) {
			gpio_request(motor->pdata->motor_st4_gpio, "motor_st4_gpio");
		}
	}

	mdev->motors[HORIZONTAL_MOTOR].max_steps = hmaxstep+100;
	mdev->motors[VERTICAL_MOTOR].max_steps = vmaxstep+30;

#ifdef CONFIG_SOC_T40
	ingenic_tcu_channel_to_virq(mdev->tcu);
	mdev->run_step_irq = mdev->tcu->virq[0];
#else
	mdev->run_step_irq = platform_get_irq(pdev,0);
#endif
	if (mdev->run_step_irq < 0) {
		ret = mdev->run_step_irq;
		dev_err(&pdev->dev, "Failed to get platform irq: %d\n", ret);
		goto error_get_irq;
	}

	ret = request_irq(mdev->run_step_irq, jz_timer_interrupt, 0,
				"jz_timer_interrupt", mdev);
	if (ret) {
		dev_err(&pdev->dev, "Failed to run request_irq() !\n");
		goto error_request_irq;
	}

	init_completion(&mdev->stop_completion);
	mdev->wait_stop = 0;
	mdev->misc_dev.minor = MISC_DYNAMIC_MINOR;
	mdev->misc_dev.name = "motor";
	mdev->misc_dev.fops = &motor_fops;
	ret = misc_register(&mdev->misc_dev);
	if (ret < 0) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "misc_register failed\n");
		goto error_misc_register;
	}

	/* debug info */
	proc = jz_proc_mkdir("motor");
	if (!proc) {
		mdev->proc = NULL;
		printk("create motor_info failed!\n");
	} else {
		mdev->proc = proc;
	}
	proc_create_data("motor_info", S_IRUGO, proc, &motor_info_fops, (void *)mdev);

	motor_set_default(mdev);
	mdev->flag = 0;
	//printk("%s%d\n",__func__,__LINE__);
#ifdef CONFIG_SOC_T40
	ingenic_tcu_counter_begin(mdev->tcu);
#endif
	return 0;

error_misc_register:
	free_irq(mdev->run_step_irq, mdev);
error_request_irq:
error_get_irq:
	for(i = 0; i < HAS_MOTOR_CNT; i++) {
		motor = &(mdev->motors[i]);
		if(motor->pdata == NULL)
			continue;

		if (motor->pdata->motor_st1_gpio != -1)
			gpio_free(motor->pdata->motor_st1_gpio);

		if (motor->pdata->motor_st2_gpio != -1)
			gpio_free(motor->pdata->motor_st2_gpio);

		if (motor->pdata->motor_st3_gpio != -1)
			gpio_free(motor->pdata->motor_st3_gpio);

		if (motor->pdata->motor_st4_gpio != -1)
			gpio_free(motor->pdata->motor_st4_gpio);
		motor->pdata = 0;
		motor->min_pos_irq = 0;
		motor->max_pos_irq = 0;
	}
	kfree(mdev);
error_devm_kzalloc:
	return ret;
}

static int motor_remove(struct platform_device *pdev)
{
	int i;
	struct motor_device *mdev = platform_get_drvdata(pdev);
	struct motor_driver *motor = NULL;

#ifdef CONFIG_SOC_T40
	ingenic_tcu_counter_stop(mdev->tcu);
#else
	jz_tcu_disable_counter(mdev->tcu);
	jz_tcu_stop_counter(mdev->tcu);
#endif
	mutex_destroy(&mdev->dev_mutex);

	free_irq(mdev->run_step_irq, mdev);
	for(i = 0; i < HAS_MOTOR_CNT; i++) {
		motor = &(mdev->motors[i]);
		if(motor->pdata == NULL)
			continue;

		if (motor->pdata->motor_st1_gpio != -1)
			gpio_free(motor->pdata->motor_st1_gpio);

		if (motor->pdata->motor_st2_gpio != -1)
			gpio_free(motor->pdata->motor_st2_gpio);

		if (motor->pdata->motor_st3_gpio != -1)
			gpio_free(motor->pdata->motor_st3_gpio);

		if (motor->pdata->motor_st4_gpio != -1)
			gpio_free(motor->pdata->motor_st4_gpio);
		motor->pdata = 0;
		motor->min_pos_irq = 0;
		motor->max_pos_irq = 0;
		motor->min_pos_irq_cnt = 0;
		motor->max_pos_irq_cnt = 0;
	}

	if (mdev->proc)
		proc_remove(mdev->proc);
	misc_deregister(&mdev->misc_dev);

	kfree(mdev);
	return 0;
}

#ifdef CONFIG_SOC_T40
static struct of_device_id motor_match[]={
	{.compatible = "ingenic,tcu_chn2",},
	{}
};
#endif

static struct platform_driver motor_driver = {
	.probe = motor_probe,
	.remove = motor_remove,
	.driver = {
		.name	= "tcu_chn2",
#ifdef CONFIG_SOC_T40
		.of_match_table = motor_match,
#endif
		.owner	= THIS_MODULE,
	}
};

static int __init motor_init(void)
{
	return platform_driver_register(&motor_driver);
}

static void __exit motor_exit(void)
{
	platform_driver_unregister(&motor_driver);
}

module_init(motor_init);
module_exit(motor_exit);

MODULE_LICENSE("GPL");
