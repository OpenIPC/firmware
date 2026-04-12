LIB := libuclibc-compat

CFLAGS ?= -O2
CFLAGS += -Wall -fPIC

all: $(LIB).so

$(LIB).so: uclibc-compat.c
	$(CC) $(CFLAGS) -shared -o $@ $^

clean:
	rm -f $(LIB).so
