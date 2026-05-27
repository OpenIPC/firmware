CC ?= gcc
CFLAGS ?= -O2

all: ptz

ptz: ptz.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f ptz

.PHONY: all clean
