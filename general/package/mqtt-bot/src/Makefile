#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# Makefile — mqtt-bot build
#

CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -std=c99 -pedantic -D_POSIX_C_SOURCE=200809L
LDFLAGS ?= -lmosquitto
TARGET  = mqtt-bot

.PHONY: all clean

all: $(TARGET)

$(TARGET): mqtt-bot.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)
