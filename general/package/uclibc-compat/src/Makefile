CFLAGS ?= -O2
CFLAGS += -Wall -fPIC

all: libuclibc-compat.so libuclibc-compat-static.a

libuclibc-compat.so: uclibc-compat.c
	$(CC) $(CFLAGS) -shared -o $@ $^

libuclibc-compat-static.a: uclibc-compat-static.o
	$(AR) rcs $@ $^

uclibc-compat-static.o: uclibc-compat-static.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.so *.a
