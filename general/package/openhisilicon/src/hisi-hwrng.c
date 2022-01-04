#include <asm/io.h>
#include <linux/delay.h>
#include <linux/hw_random.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>

struct rng_state *st;

struct rng_state {
  struct task_struct *rng_task;
  void *regs;
};

#define RNG_BUF_SIZE 320
#define RNG_ENTROPY(x) (((x)*8 * 320) >> 10) /* quality: 320/1024 */

static int rng_data_read(struct rng_state *st, u32 *buf, u32 buf_size) {
  int i;

  for (i = 0; i < buf_size; i += 4) {
    buf[i] = readl(st->regs);
  }

  return i;
}

static int rng_kthread(void *data) {
  struct rng_state *st = data;
  u32 *rng_buf;
  int bytes_read;

  rng_buf = kmalloc_array(RNG_BUF_SIZE, sizeof(u32), GFP_KERNEL);
  if (!rng_buf)
    goto out;

  while (!kthread_should_stop()) {
    bytes_read = rng_data_read(st, rng_buf, RNG_BUF_SIZE);

    /* sleep until entropy bits under write_wakeup_threshold */
    add_hwgenerator_randomness((void *)rng_buf, bytes_read,
                               RNG_ENTROPY(bytes_read));

    msleep_interruptible(1000);
  }
out:
  kfree(rng_buf);

  st->rng_task = NULL;

  return 0;
}

static void rng_start(struct rng_state *st) {
  if (st->rng_task)
    return;

  st->rng_task = kthread_run(rng_kthread, st, "hisi-hwrng");
  if (IS_ERR(st->rng_task))
    st->rng_task = NULL;
}

static void rng_stop(struct rng_state *st) {
  if (st->rng_task) {
    kthread_stop(st->rng_task);
  }
}

static int mod_init(void) {
  st = kmalloc(sizeof(struct rng_state), GFP_KERNEL);
  memset(st, 0, sizeof(struct rng_state));
  st->regs = ioremap_nocache(0x10080204, 4);

  rng_start(st);

  return 0;
}

static void mod_exit(void) {
  rng_stop(st);
  iounmap(st->regs);
  kfree(st);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Ilyin <d.ilyin@openipc.org>");
