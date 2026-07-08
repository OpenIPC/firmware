CFLAGS ?= -O2
CFLAGS += -Wall -fPIC

all: libglibc-compat.so libglibc-compat-static.a

libglibc-compat.so: glibc-compat.c
	$(CC) $(CFLAGS) -shared -o $@ $^

libglibc-compat-static.a: glibc-compat-static.o
	$(AR) rcs $@ $^

glibc-compat-static.o: glibc-compat-static.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.so *.a
