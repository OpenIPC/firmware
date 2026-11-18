# SPDX-License-Identifier: MIT
ARC_PREFIX ?= arc-linux-gnu-
ARC_AS ?= $(ARC_PREFIX)as
ARC_LD ?= $(ARC_PREFIX)ld
O ?= .

.PHONY: all
all: $(O)/fh8626-arc.elf

$(O):
	mkdir -p $@

$(O)/firmware.o: firmware.S | $(O)
	$(ARC_AS) -mcpu=arc600 -EL -o $@ $<

$(O)/fh8626-arc.elf: $(O)/firmware.o firmware.ld
	$(ARC_LD) -EL --build-id=none -T firmware.ld -o $@ $(O)/firmware.o
