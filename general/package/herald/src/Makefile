#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# Makefile — herald mDNS/DNS-SD build
#

CC      ?= gcc
PREFIX  ?= /usr/local

CFLAGS ?= \
	-Os \
	-std=c99 \
	-ffunction-sections \
	-fdata-sections \
	-fomit-frame-pointer \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-fno-common \
	-pipe \
	-Wall -Wextra

ARCH_FLAGS ?=

LDFLAGS ?= \
	-Wl,--gc-sections \
	-Wl,--strip-all

TARGETS := herald-client herald-server

.PHONY: all install clean

all: $(TARGETS)

herald-client: herald-client.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) -o $@ $< $(LDFLAGS)

herald-server: herald-server.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) -o $@ $< $(LDFLAGS)

install: $(TARGETS)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 herald-client $(DESTDIR)$(PREFIX)/bin/
	install -m 755 herald-server $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f $(TARGETS)
