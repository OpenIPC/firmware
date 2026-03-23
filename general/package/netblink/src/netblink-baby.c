/*
 *
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 *
 * netblink-baby.c — network activity LED indicator via GPIO sysfs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define TIMER 1000 * 100

volatile sig_atomic_t stop_loop = 0;

void write_val(const char *path, const char *val) {
	int fd = open(path, O_WRONLY);
	if (fd >= 0) {
		write(fd, val, strlen(val));
		close(fd);
	} else {
		printf("Cannot write to: %s\n", path);
	}
}

size_t read_bytes(const char *ifc, const char *dir) {
	char path[128];
	sprintf(path, "/sys/class/net/%s/statistics/%s_bytes", ifc, dir);

	FILE *f = fopen(path, "r");
	if (!f) {
		printf("Cannot open path: %s\n", path);
		return 0;
	}

	size_t b;
	fscanf(f, "%lu", &b);
	fclose(f);

	return b;
}

void handle_sigint(int sig) {
	stop_loop = 1;
}

int main(int argc, char **argv) {
	if (argc == 1 || argc > 3) {
		printf("Usage: %s [gpio] [interface]\n", argv[0]);
		return 1;
	}

	const char *gpio = argv[1];
	const char *ifc = argc > 2 ? argv[2] : "eth0";

	printf("Interface: %s, gpio: %s\n", ifc, gpio);
	signal(SIGINT, handle_sigint);

	char gp_val[64];
	char gp_dir[64];

	sprintf(gp_val, "/sys/class/gpio/gpio%s/value", gpio);
	sprintf(gp_dir, "/sys/class/gpio/gpio%s/direction", gpio);

	write_val("/sys/class/gpio/export", gpio);
	usleep(TIMER);
	write_val(gp_dir, "out");

	size_t value = 0;
	while (1) {
		size_t total = read_bytes(ifc, "rx") + read_bytes(ifc, "tx");
		if (total != value) {
			write_val(gp_val, "1");
			usleep(TIMER);
			write_val(gp_val, "0");
		}

		if (stop_loop) {
			break;
		}

		value = total;
		usleep(TIMER);
	}

	write_val("/sys/class/gpio/unexport", gpio);

	return 0;
}
