#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int __uClibc_main(int (*main)(int, char **, char **), int argc, char **argv,
                  int (*init)(int, char **, char **), void (*fini)(void),
                  void (*rtld_fini)(void), void *stack_end) {
  exit(main(argc, argv, environ));
}

void *mmap64(void *start, size_t len, int prot, int flags, int fd,
             uint64_t off);

void *xmap(void *start, size_t len, int prot, int flags, int fd, uint32_t off) {
  return mmap64(start, len, prot, flags, fd, off);
}
