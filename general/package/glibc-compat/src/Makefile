CFLAGS ?= -O2
CFLAGS += -Wall -fPIC

all: libglibc-compat.so

libglibc-compat.so: glibc-compat.c
	$(CC) $(CFLAGS) -shared -o $@ $^

clean:
	rm -f *.o *.so
