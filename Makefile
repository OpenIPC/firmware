BR_VER = 2023.02.1
BR_MAKE = $(MAKE) -C $(TARGET)/buildroot-$(BR_VER) BR2_EXTERNAL=$(PWD)/general O=$(TARGET)
BR_LINK = https://github.com/buildroot/buildroot/archive/refs/tags
BR_FILE = /tmp/buildroot-$(BR_VER).tar.gz
TARGET ?= $(PWD)/output

MAX_KERNEL_SIZE_LITE = 2048
MAX_ROOTFS_SIZE_LITE = 5120
MAX_KERNEL_SIZE_ULTIMATE = 3072 
MAX_ROOTFS_SIZE_ULTIMATE = 10240
MAX_KERNEL_SIZE_NAND = 4096
MAX_ROOTFS_SIZE_NAND = 16384

ifdef BOARD
	CONFIG = $(shell find br-ext-*/configs/*_defconfig | grep -m1 $(BOARD))
endif

ifeq ($(CONFIG),)
	CONFIG = $(error variable BOARD must be defined to initialize build)
else
	include $(CONFIG)
endif

.PHONY: all clean defconfig deps distclean help prepare toolname

help:
	@printf "BR-OpenIPC usage:\n \
	- make deps - install build dependencies\n \
	- make clean - remove defconfig and target folder\n \
	- make distclean - remove buildroot and output folder\n \
	- make list - show available device configurations\n \
	- make select - show interactive device list\n \
	- make all BOARD=<config> - build the selected device\n\n"

all: build repack

build: defconfig
	@$(BR_MAKE) all

br-%: defconfig
	@$(BR_MAKE) $(subst br-,,$@)

defconfig: prepare
	@$(BR_MAKE) BR2_DEFCONFIG=$(PWD)/$(CONFIG) defconfig

prepare:
	@if test ! -e $(TARGET)/buildroot-$(BR_VER); then \
		wget -c -q $(BR_LINK)/$(BR_VER).tar.gz -O $(BR_FILE); \
		mkdir -p $(TARGET); tar -xf $(BR_FILE) -C $(TARGET); fi

toolname:
	@general/scripts/show_toolchains.sh $(CONFIG)

clean:
	@rm -rf $(TARGET)/images $(TARGET)/target

distclean:
	@rm -rf $(BR_FILE) $(TARGET)

list:
	@ls -1 br-ext-chip-*/configs

deps:
	sudo apt-get install -y automake autotools-dev bc build-essential cpio \
		curl file fzf git libncurses-dev libtool lzop make rsync unzip wget

select:
	$(eval MENU_LIST = $(shell find ./br-ext-*/configs/*_defconfig | sort | \
		sed -E "s|br-ext-chip-(.+).configs.(.+)_defconfig|'\2' '\1 \2'|"))
	@$(MAKE) BOARD=$(shell whiptail --title "Available boards" --menu "Please select a board:" \
		20 76 12 --notags $(MENU_LIST) 3>&1 1>&2 2>&3) all

repack:
ifeq ($(BR2_TARGET_ROOTFS_SQUASHFS),y)
ifeq ($(BR2_OPENIPC_FLASH_SIZE),"8")
	@$(call CHECK_SIZE,uImage,$(MAX_KERNEL_SIZE_LITE))
	@$(call CHECK_SIZE,rootfs.squashfs,$(MAX_ROOTFS_SIZE_LITE))
else
	@$(call CHECK_SIZE,uImage,$(MAX_KERNEL_SIZE_ULTIMATE))
	@$(call CHECK_SIZE,rootfs.squashfs,$(MAX_ROOTFS_SIZE_ULTIMATE))
endif
	@$(call REPACK_FIRMWARE,uImage,rootfs.squashfs,nor)
endif
ifeq ($(BR2_TARGET_ROOTFS_UBI),y)
	$(eval KERNEL_BIN = $(or $(BR2_LINUX_KERNEL_IMAGE_NAME),uImage))
	@$(call CHECK_SIZE,$(KERNEL_BIN),$(MAX_KERNEL_SIZE_NAND))
	@$(call CHECK_SIZE,rootfs.ubi,$(MAX_ROOTFS_SIZE_NAND))
	@$(call REPACK_FIRMWARE,$(KERNEL_BIN),rootfs.ubi,nand)
endif

define CHECK_SIZE
	$(eval FILE_SIZE = $(shell expr $(shell stat -c %s $(TARGET)/images/$(1) || echo 0) / 1024))
	if test $(FILE_SIZE) -eq 0; then exit 1; fi
	echo - $(1): [$(FILE_SIZE)KB/$(2)KB]
	if test $(FILE_SIZE) -gt $(2); then \
		echo -- size exceeded by: $(shell expr $(FILE_SIZE) - $(2))KB; exit 1; fi
endef

define REPACK_FIRMWARE
	mkdir -p $(TARGET)/images/$(3)
	cd $(TARGET)/images/$(3) && cp -f ../$(1) $(1).$(BR2_OPENIPC_SOC_MODEL)
	cd $(TARGET)/images/$(3) && cp -f ../$(2) $(2).$(BR2_OPENIPC_SOC_MODEL)
	cd $(TARGET)/images/$(3) && md5sum $(1).$(BR2_OPENIPC_SOC_MODEL) > $(1).$(BR2_OPENIPC_SOC_MODEL).md5sum
	cd $(TARGET)/images/$(3) && md5sum $(2).$(BR2_OPENIPC_SOC_MODEL) > $(2).$(BR2_OPENIPC_SOC_MODEL).md5sum
	cd $(TARGET)/images/$(3) && tar -czf $(TARGET)/openipc.$(BR2_OPENIPC_SOC_MODEL)-$(3)-$(BR2_OPENIPC_FLAVOR).tgz \
		$(1).$(BR2_OPENIPC_SOC_MODEL) $(1).$(BR2_OPENIPC_SOC_MODEL).md5sum \
		$(2).$(BR2_OPENIPC_SOC_MODEL) $(2).$(BR2_OPENIPC_SOC_MODEL).md5sum
endef
