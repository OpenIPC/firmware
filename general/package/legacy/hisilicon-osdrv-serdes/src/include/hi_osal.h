#ifndef __HI_OSAL__
#define __HI_OSAL__

#define HI_OSAL_VERSION    "1.0"

#include "osal_list.h"

#define osal_gfp_kernel    0
#define osal_gfp_atomic    1
extern void *osal_vmalloc(unsigned long size);
extern void osal_vfree(const void *addr);
extern void *osal_kmalloc(unsigned long size, unsigned int osal_gfp_flag);
extern void osal_kfree(const void *addr);

// atomic api
typedef struct {
    void *atomic;
} osal_atomic_t;

#define OSAL_ATOMIC_INIT(i)  { (i) }

extern int osal_atomic_init(osal_atomic_t *atomic);
extern void osal_atomic_destory(osal_atomic_t *atomic);
extern int osal_atomic_read(osal_atomic_t *v);
extern void osal_atomic_set(osal_atomic_t *v, int i);
extern int osal_atomic_inc_return(osal_atomic_t *v);
extern int osal_atomic_dec_return(osal_atomic_t *v);

// semaphore api
#define EINTR              4
typedef struct osal_semaphore {
    void *sem;
} osal_semaphore_t;
extern int osal_sema_init(osal_semaphore_t *sem, int val);
extern int osal_down(osal_semaphore_t *sem);
extern int osal_down_interruptible(osal_semaphore_t *sem);
extern int osal_down_trylock(osal_semaphore_t *sem);
extern void osal_up(osal_semaphore_t *sem);
// notice:must be called when kmod exit, other wise will lead to memory leak;
extern void osal_sema_destory(osal_semaphore_t *sem);

// mutex api
typedef struct osal_mutex {
    void *mutex;
} osal_mutex_t;
extern int osal_mutex_init(osal_mutex_t *mutex);
extern int osal_mutex_lock(osal_mutex_t *mutex);
extern int osal_mutex_lock_interruptible(osal_mutex_t *mutex);
extern int osal_mutex_trylock(osal_mutex_t *mutex);
extern void osal_mutex_unlock(osal_mutex_t *mutex);
// notice:must be called when kmod exit, other wise will lead to memory leak;
extern void osal_mutex_destory(osal_mutex_t *mutex);

// spin lock api
typedef struct osal_spinlock {
    void *lock;
} osal_spinlock_t;
extern int osal_spin_lock_init(osal_spinlock_t *lock);
extern void osal_spin_lock(osal_spinlock_t *lock);
extern int osal_spin_trylock(osal_spinlock_t *lock);
extern void osal_spin_unlock(osal_spinlock_t *lock);
extern void osal_spin_lock_irqsave(osal_spinlock_t *lock, unsigned long *flags);
extern void osal_spin_unlock_irqrestore(osal_spinlock_t *lock, unsigned long *flags);
// notice:must be called when kmod exit, other wise will lead to memory leak;
extern void osal_spin_lock_destory(osal_spinlock_t *lock);

// wait api
typedef int (*osal_wait_cond_func_t)(const void *param);

typedef struct osal_wait {
    void *wait;
} osal_wait_t;
#define ERESTARTSYS        512

extern unsigned long osal_msecs_to_jiffies(const unsigned int m);
extern int osal_wait_init(osal_wait_t *wait);
extern int osal_wait_interruptible(osal_wait_t *wait, osal_wait_cond_func_t func, void *param);
extern int osal_wait_uninterruptible(osal_wait_t *wait, osal_wait_cond_func_t func, void *param);
extern int osal_wait_timeout_interruptible(osal_wait_t *wait, osal_wait_cond_func_t func, void *param,
                                           unsigned long ms);
extern int osal_wait_timeout_uninterruptible(osal_wait_t *wait, osal_wait_cond_func_t func, void *param,
                                             unsigned long ms);

#define osal_wait_event_interruptible(wait, func, param)                       \
    ({                                                                         \
        int __ret = 0;                                                         \
                                                                               \
        for (;;) {                                                             \
            if (func(param)) {                                                 \
                __ret = 0;                                                     \
                break;                                                         \
            }                                                                  \
            __ret = osal_wait_timeout_interruptible(wait, (func), param, 100); \
            if (__ret < 0)                                                     \
                break;                                                         \
        }                                                                      \
        __ret;                                                                 \
    })

#define osal_wait_event_uninterruptible(wait, func, param)          \
    ({                                                              \
        int __ret = 0;                                              \
                                                                    \
        for (;;) {                                                  \
            if (func(param)) {                                      \
                __ret = 0;                                          \
                break;                                              \
            }                                                       \
            __ret = osal_wait_uninterruptible(wait, (func), param); \
            if (__ret < 0)                                          \
                break;                                              \
        }                                                           \
        __ret;                                                      \
    })

#define osal_wait_event_timeout_interruptible(wait, func, param, timeout)        \
    ({                                                                           \
        int __ret = timeout;                                                     \
                                                                                 \
        if ((func(param)) && !timeout) {                                         \
            __ret = 1;                                                           \
        }                                                                        \
                                                                                 \
        for (;;) {                                                               \
            if (func(param)) {                                                   \
                __ret = osal_msecs_to_jiffies(__ret);                            \
                break;                                                           \
            }                                                                    \
            __ret = osal_wait_timeout_interruptible(wait, (func), param, __ret); \
            if (!__ret || __ret == -ERESTARTSYS)                                 \
                break;                                                           \
        }                                                                        \
        __ret;                                                                   \
    })

#define osal_wait_event_timeout_uninterruptible(wait, func, param, timeout)        \
    ({                                                                             \
        int __ret = timeout;                                                       \
                                                                                   \
        if ((func(param)) && !timeout) {                                           \
            __ret = 1;                                                             \
        }                                                                          \
                                                                                   \
        for (;;) {                                                                 \
            if (func(param)) {                                                     \
                __ret = osal_msecs_to_jiffies(__ret);                              \
                break;                                                             \
            }                                                                      \
            __ret = osal_wait_timeout_uninterruptible(wait, (func), param, __ret); \
            if (!__ret || __ret == -ERESTARTSYS)                                   \
                break;                                                             \
        }                                                                          \
        __ret;                                                                     \
    })

extern void osal_wakeup(osal_wait_t *wait);  // same as wake_up_all
extern void osal_wait_destory(osal_wait_t *wait);

// workqueue api
typedef struct osal_work_struct {
    void *work;
    void (*func)(struct osal_work_struct *work);
} osal_work_struct_t;
typedef void (*osal_work_func_t)(struct osal_work_struct *work);

extern int osal_init_work(struct osal_work_struct *work, osal_work_func_t func);

#define OSAL_INIT_WORK(_work, _func)      \
    do {                                  \
        osal_init_work((_work), (_func)); \
    } while (0)

extern int osal_schedule_work(struct osal_work_struct *work);
extern void osal_destroy_work(struct osal_work_struct *work);

// shedule
extern void osal_yield(void);

// interrupt api
enum osal_irqreturn {
    OSAL_IRQ_NONE = (0 << 0),
    OSAL_IRQ_HANDLED = (1 << 0),
    OSAL_IRQ_WAKE_THREAD = (1 << 1),
};

typedef int (*osal_irq_handler_t)(int, void *);
extern int osal_request_irq(unsigned int irq, osal_irq_handler_t handler, osal_irq_handler_t thread_fn,
                            const char *name, void *dev);
extern void osal_free_irq(unsigned int irq, void *dev);
extern int osal_in_interrupt(void);

#define OSAL_DIS_IRQ_CNT   2
typedef void (*osal_gic_handle_t)(unsigned int, unsigned int, void *);
extern int osal_register_gic_handle(unsigned int index, unsigned int irq, osal_gic_handle_t handle, const char *name,
                                    void *dev);
extern int osal_unregister_gic_handle(unsigned int index, unsigned int irq, void *dev);

// task api
typedef struct osal_task {
    void *task_struct;
} osal_task_t;
typedef int (*threadfn_t)(void *data);
extern osal_task_t *osal_kthread_create(threadfn_t thread, void *data, const char *name);
extern void osal_kthread_destory(osal_task_t *task, unsigned int stop_flag);

// string api
extern char *osal_strcpy(char *s1, const char *s2);
extern char *osal_strncpy(char *s1, const char *s2, int size);
extern int osal_strlcpy(char *s1, const char *s2, int size);
extern char *osal_strcat(char *s1, const char *s2);
extern char *osal_strncat(char *s1, const char *s2, int size);
extern int osal_strlcat(char *s1, const char *s2, int size);
extern int osal_strcmp(const char *s1, const char *s2);
extern int osal_strncmp(const char *s1, const char *s2, int size);
extern int osal_strnicmp(const char *s1, const char *s2, int size);
extern int osal_strcasecmp(const char *s1, const char *s2);
extern int osal_strncasecmp(const char *s1, const char *s2, int n);
extern char *osal_strchr(const char *s, int n);
extern char *osal_strnchr(const char *s, int count, int c);
extern char *osal_strrchr(const char *s, int c);
extern char *osal_strstr(const char *s1, const char *s2);
extern char *osal_strnstr(const char *s1, const char *s2, int n);
extern int osal_strlen(const char *s);
extern int osal_strnlen(const char *s, int size);
extern char *osal_strpbrk(const char *s1, const char *s2);
extern char *osal_strsep(char **s, const char *ct);
extern int osal_strspn(const char *s, const char *accept);
extern int osal_strcspn(const char *s, const char *reject);
extern void *osal_memset(void *str, int c, int count);
extern void *osal_memcpy(void *s1, const void *s2, int count);
extern void *osal_memmove(void *s1, const void *s2, int count);
extern void *osal_memscan(void *addr, int c, int size);
extern int osal_memcmp(const void *cs, const void *ct, int count);
extern void *osal_memchr(const void *s, int c, int n);
extern void *osal_memchr_inv(const void *s, int c, int n);

extern unsigned long long osal_strtoull(const char *cp, char **endp, unsigned int base);
extern unsigned long osal_strtoul(const char *cp, char **endp, unsigned int base);
extern long osal_strtol(const char *cp, char **endp, unsigned int base);
extern long long osal_strtoll(const char *cp, char **endp, unsigned int base);
extern int osal_snprintf(char *buf, int size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
extern int osal_scnprintf(char *buf, int size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
extern int osal_sprintf(char *buf, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
extern int osal_sscanf(const char *buf, const char *fmt, ...);

// addr translate
extern void *osal_ioremap(unsigned long phys_addr, unsigned long size);
extern void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size);
extern void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size);
extern void osal_iounmap(void *addr);

#define osal_readl(x) (*((volatile int *)(x)))
#define osal_writel(v, x) (*((volatile int *)(x)) = (v))

extern unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n);
extern unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n);

#define OSAL_VERIFY_READ   0
#define OSAL_VERIFY_WRITE  1
extern int osal_access_ok(int type, const void *addr, unsigned long size);

// cache api
extern void osal_flush_cache_all(void);
extern void osal_cpuc_flush_dcache_area(void *addr, int size);

extern void osal_flush_dcache_area(void *kvirt, unsigned long phys_addr, unsigned long length);
extern int osal_flush_dcache_all(void);

// math
extern unsigned long long osal_div_u64(unsigned long long dividend, unsigned int divisor);
extern long long osal_div_s64(long long dividend, int divisor);
extern unsigned long long osal_div64_u64(unsigned long long dividend, unsigned long long divisor);
extern long long osal_div64_s64(long long dividend, long long divisor);
extern unsigned long long osal_div_u64_rem(unsigned long long dividend, unsigned int divisor);
extern long long osal_div_s64_rem(long long dividend, int divisor);
extern unsigned long long osal_div64_u64_rem(unsigned long long dividend, unsigned long long divisor);
extern unsigned int osal_random(void);

#define osal_max(x, y) ({                            \
        __typeof__(x) _max1 = (x);                  \
        __typeof__(y) _max2 = (y);                  \
        (void) (&_max1 == &_max2);              \
        _max1 > _max2 ? _max1 : _max2; })

#define osal_min(x, y) ({                \
    __typeof__(x) _min1 = (x);          \
     __typeof__(y) _min2 = (y);          \
     (void) (&_min1 == &_min2);      \
     _min1 < _min2 ? _min1 : _min2; })

#define osal_abs(x) ({                \
    long ret;                         \
    if (sizeof(x) == sizeof(long)) {  \
        long __x = (x);               \
        ret = (__x < 0) ? -__x : __x; \
    } else {                          \
        int __x = (x);                \
        ret = (__x < 0) ? -__x : __x; \
    }                                 \
    ret;                              \
})

// barrier
extern void osal_mb(void);
extern void osal_rmb(void);
extern void osal_wmb(void);
extern void osal_smp_mb(void);
extern void osal_smp_rmb(void);
extern void osal_smp_wmb(void);
extern void osal_isb(void);
extern void osal_dsb(void);
extern void osal_dmb(void);

// debug
extern int osal_printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
extern void osal_panic(const char *fmt, const char *fun, int line, const char *);
#define OSAL_BUG() \
    do {           \
    } while (1)

#define OSAL_ASSERT(expr)                       \
    do {                                        \
        if (!(expr)) {                          \
            osal_printk("\nASSERT failed at:\n" \
                        "  >Condition: %s\n",   \
                #expr);                         \
            OSAL_BUG();                         \
        }                                       \
    } while (0)

#define OSAL_BUG_ON(expr)                                                               \
    do {                                                                                \
        if (expr) {                                                                     \
            osal_printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
            OSAL_BUG();                                                                 \
        }                                                                               \
    } while (0)

// proc
typedef struct osal_proc_dir_entry {
    char name[50];
    void *proc_dir_entry;
    int (*open)(struct osal_proc_dir_entry *entry);
    int (*read)(struct osal_proc_dir_entry *entry);
    int (*write)(struct osal_proc_dir_entry *entry, const char *buf, int count, long long *);
    void *private;
    void *seqfile;
    struct osal_list_head node;
} osal_proc_entry_t;
extern osal_proc_entry_t *osal_create_proc_entry(const char *name, osal_proc_entry_t *parent);
extern osal_proc_entry_t *osal_proc_mkdir(const char *name, osal_proc_entry_t *parent);
extern void osal_remove_proc_entry(const char *name, osal_proc_entry_t *parent);
extern int osal_seq_printf(osal_proc_entry_t *entry, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

// device api
#ifndef _IOC_TYPECHECK
#include "osal_ioctl.h"
#endif
typedef struct osal_dev {
    char name[48];
    void *dev;
    int minor;
    struct osal_fileops *fops;
    struct osal_pmops *osal_pmops;
} osal_dev_t;

typedef struct osal_vm {
    void *vm;
} osal_vm_t;

#define OSAL_POLLIN        0x0001
#define OSAL_POLLPRI       0x0002
#define OSAL_POLLOUT       0x0004
#define OSAL_POLLERR       0x0008
#define OSAL_POLLHUP       0x0010
#define OSAL_POLLNVAL      0x0020
#define OSAL_POLLRDNORM    0x0040
#define OSAL_POLLRDBAND    0x0080
#define OSAL_POLLWRNORM    0x0100

typedef struct osal_poll {
    void *poll_table;
    void *data;
} osal_poll_t;

typedef struct osal_fileops {
    int (*open)(void *private_data);
    int (*read)(char *buf, int size, long *offset, void *private_data);
    int (*write)(const char *buf, int size, long *offset, void *private_data);
    long (*llseek)(long offset, int whence, void *private_data);
    int (*release)(void *private_data);
    long (*unlocked_ioctl)(unsigned int cmd, unsigned long arg, void *private_data);
    unsigned int (*poll)(osal_poll_t *osal_poll, void *private_data);
    int (*mmap)(osal_vm_t *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff, void *private_data);
#ifdef CONFIG_COMPAT
    long (*compat_ioctl)(unsigned int cmd, unsigned long arg, void *private_data);
#endif
} osal_fileops_t;

typedef struct osal_pmops {
    int (*pm_prepare)(osal_dev_t *dev);
    void (*pm_complete)(osal_dev_t *dev);
    int (*pm_suspend)(osal_dev_t *dev);
    int (*pm_resume)(osal_dev_t *dev);
    int (*pm_freeze)(osal_dev_t *dev);
    int (*pm_thaw)(osal_dev_t *dev);
    int (*pm_poweroff)(osal_dev_t *dev);
    int (*pm_restore)(osal_dev_t *dev);
    int (*pm_suspend_late)(osal_dev_t *dev);
    int (*pm_resume_early)(osal_dev_t *dev);
    int (*pm_freeze_late)(osal_dev_t *dev);
    int (*pm_thaw_early)(osal_dev_t *dev);
    int (*pm_poweroff_late)(osal_dev_t *dev);
    int (*pm_restore_early)(osal_dev_t *dev);
    int (*pm_suspend_noirq)(osal_dev_t *dev);
    int (*pm_resume_noirq)(osal_dev_t *dev);
    int (*pm_freeze_noirq)(osal_dev_t *dev);
    int (*pm_thaw_noirq)(osal_dev_t *dev);
    int (*pm_poweroff_noirq)(osal_dev_t *dev);
    int (*pm_restore_noirq)(osal_dev_t *dev);
} osal_pmops_t;

#define OSAL_SEEK_SET      0
#define OSAL_SEEK_CUR      1
#define OSAL_SEEK_END      2

// #define PAGE_SHIFT         12

extern osal_dev_t *osal_createdev(const char *name);
extern int osal_destroydev(osal_dev_t *pdev);
extern int osal_registerdevice(osal_dev_t *pdev);
extern void osal_deregisterdevice(osal_dev_t *pdev);
extern void osal_poll_wait(osal_poll_t *table, osal_wait_t *wait);
extern void osal_pgprot_noncached(osal_vm_t *vm);
extern void osal_pgprot_cached(osal_vm_t *vm);
extern void osal_pgprot_writecombine(osal_vm_t *vm);
extern void osal_pgprot_stronglyordered(osal_vm_t *vm);
extern int osal_remap_pfn_range(osal_vm_t *vm, unsigned long addr, unsigned long pfn, unsigned long size);
extern int osal_io_remap_pfn_range(osal_vm_t *vm, unsigned long addr, unsigned long pfn, unsigned long size);

// timer
typedef struct osal_timer {
    void *timer;
    void (*function)(unsigned long);
    unsigned long data;
} osal_timer_t;

typedef struct osal_timeval {
    long tv_sec;
    long tv_usec;
} osal_timeval_t;

typedef struct osal_rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} osal_rtc_time_t;

/* Return values for the timer callback function */
typedef enum hiOSAL_HRTIMER_RESTART_E {
    OSAL_HRTIMER_NORESTART, /* < The timer will not be restarted. */
    OSAL_HRTIMER_RESTART /* < The timer must be restarted. */
} OSAL_HRTIMER_RESTART_E;

/* hrtimer struct */
typedef struct osal_hrtimer {
    void *timer;
    OSAL_HRTIMER_RESTART_E (*function)(void *timer);
    unsigned long interval; /* Unit ms */
} osal_hrtimer_t;

extern int osal_hrtimer_create(osal_hrtimer_t *phrtimer);
extern int osal_hrtimer_start(osal_hrtimer_t *phrtimer);
extern int osal_hrtimer_destory(osal_hrtimer_t *phrtimer);

extern int osal_timer_init(osal_timer_t *timer);
extern int osal_set_timer(osal_timer_t *timer, unsigned long interval);  // ms
extern int osal_del_timer(osal_timer_t *timer);
extern int osal_timer_destory(osal_timer_t *timer);

extern unsigned long osal_msleep(unsigned int msecs);
extern void osal_udelay(unsigned int usecs);
extern void osal_mdelay(unsigned int msecs);

extern unsigned int osal_get_tickcount(void);
extern unsigned long long osal_sched_clock(void);
extern void osal_gettimeofday(osal_timeval_t *tv);
extern void osal_rtc_time_to_tm(unsigned long time, osal_rtc_time_t *tm);
extern void osal_rtc_tm_to_time(osal_rtc_time_t *tm, unsigned long *time);
extern int osal_rtc_valid_tm(struct osal_rtc_time *tm);
extern void osal_getjiffies(unsigned long long *pjiffies);

#define OSAL_O_ACCMODE     00000003
#define OSAL_O_RDONLY      00000000
#define OSAL_O_WRONLY      00000001
#define OSAL_O_RDWR        00000002
#define OSAL_O_CREAT       00000100

extern void *osal_klib_fopen(const char *filename, int flags, int mode);
extern void osal_klib_fclose(void *filp);
extern int osal_klib_fwrite(const char *buf, int len, void *filp);
extern int osal_klib_fread(char *buf, unsigned int len, void *filp);

// reboot
struct osal_notifier_block {
    int (*notifier_call)(struct osal_notifier_block *nb, unsigned long action, void *data);
    void *notifier_block;
};
typedef int (*osal_notifier_fn_t)(struct osal_notifier_block *nb, unsigned long action, void *data);

extern int osal_register_reboot_notifier(struct osal_notifier_block *nb);
extern int osal_unregister_reboot_notifier(struct osal_notifier_block *nb);

#include <stdarg.h>

#ifndef _OSAL_VA_LIST

#define _OSAL_VA_LIST
#define osal_va_list       va_list
#define osal_va_arg(ap, T) va_arg(ap, T)
#define osal_va_end(ap) va_end(ap)
#define osal_va_start(ap, A) va_start(ap, A)

#endif /* va_arg */

#define NULL_STRING        "NULL"

extern void osal_vprintk(const char *fmt, osal_va_list args);
extern int osal_vsnprintf(char *str, int size, const char *fmt, osal_va_list args);

#ifdef CONFIG_HISI_SNAPSHOT_BOOT

#ifndef OSAL_UMH_WAIT_PROC
#define OSAL_UMH_WAIT_PROC 2 /* wait for the process to complete */
#endif

extern int osal_call_usermodehelper_force(char *path, char **argv, char **envp, int wait);
#endif

int osal_platform_driver_register(void *drv);
void osal_platform_driver_unregister(void *drv);
void *osal_platform_get_resource_byname(void *dev, unsigned int type,
                                        const char *name);
void *osal_platform_get_resource(void *dev, unsigned int type,
                                 unsigned int num);
int osal_platform_get_irq(void *dev, unsigned int num);
int osal_platform_get_irq_byname(void *dev, const char *name);

#define osal_module_driver(osal_driver, osal_register, osal_unregister, ...) \
    static int __init osal_driver##_init(void)                               \
    {                                                                        \
        return osal_register(&(osal_driver));                                \
    }                                                                        \
    module_init(osal_driver##_init);                                         \
    static void __exit osal_driver##_exit(void)                              \
    {                                                                        \
        osal_unregister(&(osal_driver));                                     \
    }                                                                        \
    module_exit(osal_driver##_exit);

#define osal_module_platform_driver(platform_driver)                   \
    osal_module_driver(platform_driver, osal_platform_driver_register, \
        osal_platform_driver_unregister)

#endif
