#include <jz_proc.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenIPC");
MODULE_DESCRIPTION("Ingenic GPIO Claimer");

static struct proc_dir_entry *claim_proc;

int claim_gpio(int gpio) {
  int ret = 0;

  pr_debug("GPIO[%i] Requesting...\n", gpio);

  if (!gpio_is_valid(gpio)) {
    pr_err("GPIO[%i] is not valid\n", gpio);
    return -1;
  }

  if (gpio_request(gpio, 0) > 0)
    return -1;

  pr_debug("GPIO[%i] Setting direction...\n", gpio);
  gpio_direction_output(gpio, 0);
  pr_debug("GPIO[%i] Exporting...\n", gpio);
  gpio_export(gpio, true);

  return 0;
}

ssize_t claim_proc_write(struct file *filp, const char *buf, size_t len,
                         loff_t *off) {
  int ret = 0;
  char cmd[4] = {0};
  uint32_t addr, data, datalen;

  if (len > 4) {
    return -EFAULT;
  }
  if (copy_from_user(cmd, buf, len)) {
    return -EFAULT;
  }
  int gpio = simple_strtoul(cmd, NULL, 0);
  ret = claim_gpio(gpio);
  if (ret) {
    pr_err("GPIO[%i] Error %i \n", gpio, ret);
    return -EFAULT;
  } else {
    pr_debug("GPIO[%i] Claiming...\n", gpio);
  }

  return len;
}

static const struct file_operations claim_proc_fops = {
    .owner = THIS_MODULE,
    .write = claim_proc_write,
};

static __init int init_claim(void) {
  claim_proc = proc_mkdir("jz/claim", 0);
  if (!claim_proc) {
    printk("err: jz_proc_mkdir failed\n");
  }
  proc_create_data("gpio", S_IRUGO, claim_proc, &claim_proc_fops, NULL);
  printk("Ingenic GPIO claim module (c) OpenIPC.org\n");
  return 0;
}

static __exit void exit_claim(void) { proc_remove(claim_proc); }

module_init(init_claim);
module_exit(exit_claim);
