#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct ugly_fake_stat {
    unsigned long long A;
    unsigned long long B;
    unsigned int st_mode;    /* File mode.  */
};

int sTaT(const char *pathname, struct stat *statbuf)
{
    struct stat good_struct_stat;
    memset(&good_struct_stat, 0, sizeof(struct stat));
    int ret = stat(pathname, &good_struct_stat);

    struct ugly_fake_stat* ugly = (struct ugly_fake_stat*)statbuf;
    ugly->st_mode = good_struct_stat.st_mode;

    return ret;
}

