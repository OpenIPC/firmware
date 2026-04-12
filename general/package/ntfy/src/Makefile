#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# Makefile — ntfy notification client build
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

LDFLAGS ?= \
	-Wl,--gc-sections \
	-Wl,--strip-all

LIBS ?= -lcurl

TARGET := ntfy

.PHONY: all install clean

all: $(TARGET)

$(TARGET): ntfy.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f $(TARGET)
