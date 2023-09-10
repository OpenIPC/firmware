#if !defined(__ALE_H__)
#define __ALE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define LOGGER_ALE_MASK         0xf0

#define LOGGER_ALE_MSG_RAW      0x00
#define LOGGER_ALE_MSG_K        0x10
#define LOGGER_ALE_MSG_N        0x20
#define LOGGER_ALE_MSG_J        0x30
#define LOGGER_ALE_XMSG_K       0x40
#define LOGGER_ALE_XMSG_N       0x50
#define LOGGER_ALE_XMSG_J       0x60

#define LOGGER_ALE_MSG_SIZE     768

#define LOGGER_ALE_ARGS_MAX 16
struct ale_convert {
    const char *tag_str;
    const char *fmt_ptr;
    const char *file;
    int lineno;
    unsigned int hash;
    char params[LOGGER_ALE_ARGS_MAX];
};

int ale_log_output_binary(int prio, const char *tag, const void *payload, size_t len);

int ale_log_output(int bufid, int prio, const struct ale_convert *convert, ...);

#define ale_log_print(bufid, prio, tag, fmt, ...)			\
  ({									\
      if (__builtin_constant_p(tag) && __builtin_constant_p(fmt)) {     \
          static const struct ale_convert ____ale_rec____ =		\
	    {								\
                __builtin_constant_p(tag) ? tag : NULL,			\
                __builtin_constant_p(fmt) ? fmt : NULL,			\
                __FILE__,                                               \
                __builtin_constant_p(prio) ? prio : 0,                  \
                0,                                                      \
                "" };                                                   \
          ale_log_output(bufid, prio, &____ale_rec____, ##__VA_ARGS__);	\
      }                                                                 \
      else {                                                            \
          struct ale_convert ale =                                      \
              {tag, fmt, NULL,                                          \
               0, 0, ""};                                               \
          ale_log_output(bufid, prio, &ale, ##__VA_ARGS__);		\
      }                                                                 \
  })

#if defined(__cplusplus)
};
#endif

#endif
