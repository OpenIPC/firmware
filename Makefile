BR_VER = 2023.02.1
BR_MAKE = $(MAKE) -C buildroot-$(BR_VER) BR2_EXTERNAL=$(PWD)/general O=$(TARGET)
BR_LINK = https://github.com/buildroot/buildroot/archive/refs/tags
BR_FILE = /tmp/buildroot-$(BR_VER).tar.gz
TARGET ?= $(PWD)/output

ifdef BOARD
	CONFIG = $(shell find br-ext-chip-*/configs | grep -m1 $(BOARD))
endif

ifeq ($(CONFIG),)
	CONFIG = $(error variable BOARD must be defined to initialize build)
endif

.PHONY: all clean defconfig deps distclean help prepare toolname

help:
	@printf "BR-OpenIPC usage:\n \
	- make deps - install build dependencies\n \
	- make clean - remove defconfig and target folder\n \
	- make distclean - remove buildroot and output folder\n \
	- make list - show available device configurations\n \
	- make all BOARD=<config> - builds the selected device\n\n"

all: defconfig
	@$(BR_MAKE) all

br-%: defconfig
	@$(BR_MAKE) $(subst br-,,$@)

defconfig: prepare
	@$(BR_MAKE) BR2_DEFCONFIG=$(PWD)/$(CONFIG) defconfig

prepare:
	@test -e $(BR_FILE) || wget -c -q $(BR_LINK)/$(BR_VER).tar.gz -O $(BR_FILE)
	@test -e buildroot-$(BR_VER) || tar -xf $(BR_FILE) -C $(PWD)

toolname:
	@general/scripts/show_toolchains.sh $(CONFIG)

clean:
	@rm -rf $(TARGET)/target $(TARGET)/.config

distclean:
	@rm -rf buildroot-$(BR_VER) $(BR_FILE) $(TARGET)

list:
	@ls -1 br-ext-chip-*/configs

deps:
	sudo apt-get install -y automake autotools-dev bc build-essential cpio curl file fzf git libncurses-dev libtool lzop make rsync unzip wget
