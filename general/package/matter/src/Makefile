#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# Makefile — matter-server build
#

CC     ?= $(CROSS_COMPILE)gcc
STRIP  ?= $(CROSS_COMPILE)strip

PREFIX  ?= /usr
SBINDIR ?= $(PREFIX)/sbin

# ---------------------------------------------------------------------------
# Compiler flags tuned for embedded / BusyBox targets:
#   -Os            minimise binary size (preferred over -O2 on flash storage)
#   -std=c99       strict C99 — avoids implicit POSIX extensions
#   -D_POSIX_C_SOURCE=200809L  expose POSIX.1-2008 API
#   -D_DEFAULT_SOURCE          expose BSD/SVID extensions (usleep, etc.)
#   -ffunction-sections / -fdata-sections  dead-code elimination by linker
#   -fomit-frame-pointer       save a register on every call frame
#   -fno-unwind-tables /       no DWARF unwinding tables — smaller .so/.elf
#   -fno-asynchronous-unwind-tables
#   -fno-common                no tentative definitions — catches duplicates
#   -pipe                      avoid temporary files, speed up compilation
# ---------------------------------------------------------------------------
CFLAGS  ?= \
	-Os \
	-std=c99 \
	-ffunction-sections \
	-fdata-sections \
	-fomit-frame-pointer \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-fno-common \
	-pipe \
	-Wall \
	-Wextra \
	-Wno-unused-result

ARCH_FLAGS ?=

# ---------------------------------------------------------------------------
# Linker flags:
#   -pthread                   link pthreads and define _REENTRANT
#   -Wl,--gc-sections          discard unreferenced sections (pairs with -f*-sections)
#   -Wl,--strip-all            strip all symbols from the final binary
# ---------------------------------------------------------------------------
LDFLAGS ?= \
	-pthread \
	-Wl,--gc-sections \
	-Wl,--strip-all

TARGET := matter-server

.PHONY: all install strip clean

all: $(TARGET)

$(TARGET): matter-server.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) -o $@ $< $(LDFLAGS)

# Run strip explicitly (useful when LDFLAGS cannot carry --strip-all,
# e.g. when building with musl-static where the linker ignores it).
strip: $(TARGET)
	$(STRIP) --strip-all $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(SBINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(SBINDIR)/$(TARGET)

clean:
	rm -f $(TARGET)
