/*
 * copyright (c) hisilicon technologies co., ltd. 2016-2019. all rights reserved.
 * description: serdes_comm.c
 * author: hisilicon multimedia software group
 * create: 2019-05-17
 */

#include <linux/i2c.h>
#include <asm/uaccess.h>
#ifndef __HuaweiLite__
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/miscdevice.h>
#else
#include "i2c.h"
#endif

#include "hi_serdes.h"
#include "serdes_comm.h"
#include "type.h"

#define SERDES_DEV_NAME0            "serdes0"
#define SERDES_DEV_NAME1            "serdes1"
#define SERDES_I2C_BUF_MAX_NUM      8
#define ONT_BYTE_EQUAL_BIT_NUM      8

unsigned char sensor_dev_addr[SERDES_MAX_NUM] = {0};
unsigned char serdes_init_flag[SERDES_MAX_NUM] = {0};
static struct i2c_client *g_serdes_client[SERDES_MAX_NUM];

#ifndef __HuaweiLite__
static struct i2c_board_info g_thine_info =
{
    I2C_BOARD_INFO(THINE_DEVICE_TYPE, (THINE_DEVICE_ADDR >> 1)),
};
#endif

int serdes_i2c_write(unsigned char i2c_dev, unsigned char dev_addr,
                     unsigned int reg_addr, unsigned int reg_addr_num,
                     unsigned int data, unsigned int data_byte_num)
{
    unsigned char tmp_buf[SERDES_I2C_BUF_MAX_NUM];
    int ret = 0;
    int idx = 0;
    unsigned int try_count = 0;
    struct i2c_client client;

    if (i2c_dev >= SERDES_MAX_NUM) {
        return HI_FAILURE;
    }

    if (g_serdes_client[i2c_dev] == NULL) {
        return HI_FAILURE;
    }

    osal_memcpy(&client, g_serdes_client[i2c_dev], sizeof(struct i2c_client));

#ifdef __HuaweiLite__
    client.addr = ((dev_addr >> 1) & 0xff);

    /* reg_addr config */
    if (reg_addr_num == 1) {
        client.flags &= ~I2C_M_16BIT_REG;
        tmp_buf[idx++] = reg_addr & 0xff;
    } else {
        client.flags |= I2C_M_16BIT_REG;
        tmp_buf[idx++] = (reg_addr >> ONT_BYTE_EQUAL_BIT_NUM) & 0xff;
        tmp_buf[idx++] = reg_addr & 0xff;
    }

    /* data config */
    if (data_byte_num == 1) {
        client.flags &= ~I2C_M_16BIT_DATA;
        tmp_buf[idx++] = data & 0xff;
    } else {
        client.flags |= I2C_M_16BIT_DATA;
        tmp_buf[idx++] = (data >> ONT_BYTE_EQUAL_BIT_NUM) & 0xff;
        tmp_buf[idx++] = data & 0xff;
    }
#else
    client.addr = (dev_addr >> 1);

    /* reg_addr config */
    if (reg_addr_num == 1) {
        tmp_buf[idx++] = reg_addr & 0xff;
    } else {
        tmp_buf[idx++] = (reg_addr >> ONT_BYTE_EQUAL_BIT_NUM) & 0xff;
        tmp_buf[idx++] = reg_addr & 0xff;
    }

    /* data config */
    if (data_byte_num == 1) {
        tmp_buf[idx++] = data & 0xff;
    } else {
        tmp_buf[idx++] = (data >> ONT_BYTE_EQUAL_BIT_NUM) & 0xff;
        tmp_buf[idx++] = data & 0xff;
    }
#endif

    while (1) {
        ret = hi_i2c_master_send(&client, (const char *)tmp_buf, idx);
        if (ret == idx) {
            break;
        }
#ifdef __HuaweiLite__
        else if ((ret == -EAGAIN))
#else
        else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
#endif
        {
            try_count++;
            if (try_count > 5) {
                return HI_FAILURE;
            }
        } else {
            osal_printk("[%s %d] hi_i2c_master_send error, reg_addr=0x%x, ret=%d.\n", __func__, __LINE__, reg_addr, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

int serdes_i2c_read(unsigned char i2c_dev, const unsigned char dev_addr,
                    const unsigned int reg_addr, unsigned int *data)
{
    int ret;
    static struct i2c_msg msg[2];
    unsigned char buffer[2];
    struct i2c_client client;

    if (i2c_dev >= SERDES_MAX_NUM) {
        return HI_FAILURE;
    }

    if (g_serdes_client[i2c_dev] == NULL) {
        return HI_FAILURE;
    }

    osal_memcpy(&client, g_serdes_client[i2c_dev], sizeof(struct i2c_client));

    buffer[0] = reg_addr >> ONT_BYTE_EQUAL_BIT_NUM & 0xFF;
    buffer[1] = reg_addr & 0xFF;

    msg[0].addr = client.addr;
    msg[0].flags = 0;
    msg[0].len = 2;
    msg[0].buf = buffer;

    msg[1].addr = client.addr;
    msg[1].flags = client.flags | I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = buffer;

    ret = hi_i2c_transfer(client.adapter, msg, 2);
    if (ret != 2) {
        osal_printk("[%s %d] hi_i2c_transfer error, ret=%d.\n", __FUNCTION__, __LINE__, ret);
        return HI_FAILURE;
    }

    *data = buffer[0];

    return HI_SUCCESS;
}

static int check_serdes_dev_attr(serdes_dev_attr_t* p_attr)
{
    if (p_attr->devno >= SERDES_MAX_NUM) {
        osal_printk("invalid serdes_dev number(%d)!\n", p_attr->devno);
        return HI_FAILURE;
    }

    if (p_attr->sendes_en != HI_TRUE && p_attr->sendes_en != HI_FALSE) {
        osal_printk("invalid serdes_en(%d)!\n", p_attr->sendes_en);
        return HI_FAILURE;
    }

    if (p_attr->serdes_mode < SERDES_MODE_4LANE_LINEAR ||
        p_attr->serdes_mode >= SERDES_MODE_BUTT) {
        osal_printk("invalid serdes_mode(%d)!\n", p_attr->serdes_mode);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef __HuaweiLite__
static int serdes_ioctl(struct file *file, int cmd, unsigned long arg)
#else
static long serdes_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    int ret;
    serdes_dev_attr_t __user *argp = (serdes_dev_attr_t __user *)arg;
    serdes_dev_attr_t serdes_dev_attr;

    if (copy_from_user(&serdes_dev_attr, argp, sizeof(serdes_dev_attr_t))) {
        return -EFAULT;
    }

    switch (cmd) {
        case HI_SERDES_START:
            ret = check_serdes_dev_attr(&serdes_dev_attr);
            if (ret < 0) {
                return ret;
            }

            sensor_dev_addr[serdes_dev_attr.devno] = serdes_dev_attr.sensor_i2c_addr;
            if ((serdes_dev_attr.sendes_en == HI_TRUE) &&
                (serdes_init_flag[serdes_dev_attr.devno] == HI_FALSE)) {
                serdes_thine_init(serdes_dev_attr.devno, serdes_dev_attr.serdes_mode);
                serdes_init_flag[serdes_dev_attr.devno] = HI_TRUE;
            } else if ((serdes_dev_attr.sendes_en == HI_FALSE) &&
                       (serdes_init_flag[serdes_dev_attr.devno] == HI_TRUE)){
                serdes_thine_exit(serdes_dev_attr.devno);
                serdes_init_flag[serdes_dev_attr.devno] = HI_FALSE;
            } else {
            }

            break;

        default: {
            osal_printk("kernel: no such serdes command %#x!\n", cmd);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}


#ifdef __HuaweiLite__
static int serdes_open(struct file *file)
{
    return 0;
}
static int serdes_close(struct file *file)
{
    return 0;
}
const static struct file_operations_vfs serdes_fops = {
    .open = serdes_open,
    .close = serdes_close,
    .ioctl = serdes_ioctl
};

static struct i2c_client g_serdes_client_obj[SERDES_MAX_NUM];
static struct i2c_client *hi_serdes_i2c_client_init(int i2c_dev)
{
    int ret;
    struct i2c_client *i2c_client0 = &g_serdes_client_obj[i2c_dev];
    i2c_client0->addr = THINE_DEVICE_ADDR >> 1;
    i2c_client0->flags = 0;
    ret = client_attach(i2c_client0, i2c_dev);
    if (ret) {
        dprintf("fail to attach g_serdes_client!\n");
        return NULL;
    }
    return &g_serdes_client_obj[i2c_dev];
}

int serdes_mod_init(void)
{
    int i  = 0;
    int ret;

    for (i = 0; i < SERDES_MAX_NUM; i++) {
        g_serdes_client[i] = hi_serdes_i2c_client_init(i);
    }

    ret = register_driver("/dev/serdes0", &serdes_fops, 0666, 0);
    if (ret) {
        osal_printk("register serdes0 device failed with %#x!\n", ret);
        return -1;
    }

    ret = register_driver("/dev/serdes1", &serdes_fops, 0666, 0);
    if (ret) {
        osal_printk("register serdes1 device failed with %#x!\n", ret);
        return -1;
    }

    osal_printk("\n====> register serdes0_1 success.\n");

    return HI_SUCCESS;
}

void serdes_mod_exit(void)
{
    unregister_driver("/dev/serdes1");
    unregister_driver("/dev/serdes0");
    return;
}

#else

static int serdes_open(struct inode *inode, struct file *file)
{
    return 0;
}
static int serdes_close(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations serdes_fops = {
    .owner          = THIS_MODULE,
    .open           = serdes_open,
    .release        = serdes_close,
    .unlocked_ioctl = serdes_ioctl,
};

static struct miscdevice serdes_dev0 = {
    .minor       = MISC_DYNAMIC_MINOR,
    .name        = SERDES_DEV_NAME0,
    .fops        = &serdes_fops,
};

static struct miscdevice serdes_dev1 = {
    .minor       = MISC_DYNAMIC_MINOR,
    .name        = SERDES_DEV_NAME1,
    .fops        = &serdes_fops,
};

int serdes_mod_init(void)
{
    int i;
    int ret;
    struct i2c_adapter *i2c_adapter;

    for (i = 0; i < SERDES_MAX_NUM; i++) {
        i2c_adapter = i2c_get_adapter(i);
        if (i2c_adapter == NULL) {
            osal_printk("i2c_get_adapter error!\n");
            return HI_FAILURE;
        }

        g_serdes_client[i] = i2c_new_device(i2c_adapter, &g_thine_info);

        i2c_put_adapter(i2c_adapter);
    }

    ret = misc_register(&serdes_dev0);
    if (ret != HI_SUCCESS) {
        osal_printk("kernel: register serdes_0 device failed!\n");
        return HI_FAILURE;
    }

    ret = misc_register(&serdes_dev1);
    if (ret != HI_SUCCESS) {
        osal_printk("kernel: register serdes_1 device failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

void serdes_mod_exit(void)
{
    int i;

    for (i = 0; i < SERDES_MAX_NUM; i++) {
        i2c_unregister_device(g_serdes_client[i]);
    }

    misc_deregister(&serdes_dev1);
    misc_deregister(&serdes_dev0);

    return;
}

module_init(serdes_mod_init);
module_exit(serdes_mod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hisilicon");

#endif

