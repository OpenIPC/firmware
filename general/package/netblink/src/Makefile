#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# Makefile — netblink build
#

CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -Os -std=c99 -D_XOPEN_SOURCE=600
LDFLAGS ?= -s

TARGET  = netblink
SRCS    = netblink.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
