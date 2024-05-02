#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define LOGGER_ENTRY_MAX_LEN (4 * 1024)
#define LOGGER_FLUSH_LOG _IO(__LOGGERIO, 4)
#define __LOGGERIO 0xAE

struct logger_entry {
	uint16_t len;
	uint16_t __pad;
	int32_t pid;
	int32_t tid;
	int32_t sec;
	int32_t nsec;
	char msg[4096];
} entry;

enum android_LogPriority {
	ANDROID_LOG_UNKNOWN = 0,
	ANDROID_LOG_DEFAULT,
	ANDROID_LOG_VERBOSE,
	ANDROID_LOG_DEBUG,
	ANDROID_LOG_INFO,
	ANDROID_LOG_WARN,
	ANDROID_LOG_ERROR,
	ANDROID_LOG_FATAL,
	ANDROID_LOG_SILENT,
};

int main(int argc, char *argv[]) {
	int fd;
	int msize;
	int readsize;
	int numread;

	if (argc > 1) {
		if (strcmp(argv[1], "-c") == 0) {
			fd = open("/dev/log_main", O_WRONLY);
			if (fd < 0) {
				perror("Error opening /dev/log_main\n");
				return -1;
			}
			ioctl(fd, LOGGER_FLUSH_LOG);
			close(fd);
			return 0;
		} else {
			printf("Usage: %s [-c] [-h]\n", argv[0]);
			printf("Options:\n");
			printf("	-c	Clear the log\n");
			printf("	-h	Show this help\n");
			return 0;
		}
	}

	fd = open("/dev/log_main", O_RDONLY);
	if (fd < 0) {
		perror("Error opening /dev/log_main\n");
		return -1;
	}

	int pos;
	while (1) {
		numread = read(fd, &entry, LOGGER_ENTRY_MAX_LEN);
		if (numread < 1) {
			perror("Error: ");
			strerror(errno);
			return -1;
		}

		char *tag = entry.msg;
		char *prog = entry.msg + 1;
		char *msg = entry.msg + strlen(entry.msg) + 1;
		char tagbyte;

		switch (*tag) {
			case ANDROID_LOG_UNKNOWN:
				tagbyte = 'U';
				break;

			case ANDROID_LOG_DEFAULT:
				tagbyte = '*';
				break;

			case ANDROID_LOG_VERBOSE:
				tagbyte = 'V';
				break;

			case ANDROID_LOG_DEBUG:
				tagbyte = 'D';
				break;

			case ANDROID_LOG_INFO:
				tagbyte = 'I';
				break;

			case ANDROID_LOG_WARN:
				tagbyte = 'W';
				break;

			case ANDROID_LOG_ERROR:
				tagbyte = 'E';
				break;

			case ANDROID_LOG_FATAL:
				tagbyte = 'F';
				break;

			case ANDROID_LOG_SILENT:
				tagbyte = 'S';
				break;

			default:
				tagbyte = '?';
				break;
		}

		printf("%c/%s(%5d): %s", tagbyte, prog, entry.pid, msg);
	}
}
