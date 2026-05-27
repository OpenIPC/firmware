CC ?= gcc
CFLAGS ?= -O2

# Shared lib build flags required regardless of base CFLAGS from buildroot.
SHIM_CFLAGS = $(CFLAGS) -fPIC

all: libfixmipi.so

libfixmipi.so: libfixmipi.c
	$(CC) $(SHIM_CFLAGS) -shared -o $@ $< -ldl

clean:
	rm -f libfixmipi.so

.PHONY: all clean
