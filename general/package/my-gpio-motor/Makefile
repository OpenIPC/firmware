CROSS_COMPILE = /data/openipc/output/host/bin/arm-openipc-linux-musleabihf-
CC = $(CROSS_COMPILE)gcc

TARGET = my-gpio-motors
SRC = gpio-motors.c

CFLAGS = -Wall -O2 -I/data/openipc/output/per-package/libgpiod/target/usr/include
LDFLAGS = -L/data/openipc/output/target/usr/lib -lgpiod

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)

.PHONY: rebuild
rebuild: clean all
