PKG_CONFIG ?= pkg-config
TARGET = gpiod-motors

CPPFLAGS += $(shell $(PKG_CONFIG) --cflags libgpiod)
CFLAGS += -Wall
LDLIBS += $(shell $(PKG_CONFIG) --libs libgpiod)

.PHONY: all clean

all: output/$(TARGET)

output/$(TARGET): $(TARGET).c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@ -s

clean:
	rm -rf output
