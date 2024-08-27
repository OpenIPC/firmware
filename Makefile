BR_VER = 2024.02.1
BR_MAKE = $(MAKE) -C $(TARGET)/buildroot-$(BR_VER) BR2_EXTERNAL=$(PWD)/general O=$(TARGET)
BR_LINK = https://github.com/buildroot/buildroot/archive
BR_FILE = /tmp/buildroot-$(BR_VER).tar.gz
TARGET ?= $(PWD)/output

CONFIG = $(error variable BOARD is not defined)
TIMER := $(shell date +%s)

ifeq ($(or $(MAKECMDGOALS), $(BOARD)),)
LIST := $(shell find ./br-ext-*/configs/*_defconfig | sort | \
	sed -E "s/br-ext-chip-(.+).configs.(.+)_defconfig/'\2' '\1 \2'/")
BOARD := $(or $(shell whiptail --title "Available boards" --menu "Select a config:" 20 70 12 \
	--notags $(LIST) 3>&1 1>&2 2>&3),$(CONFIG))
endif

ifneq ($(BOARD),)
CONFIG := $(shell find br-ext-*/configs/*_defconfig | grep -m1 $(BOARD))
include $(CONFIG)
endif

all: build repack timer

build: defconfig
	@$(BR_MAKE) all

br-%: defconfig
	@$(BR_MAKE) $(subst br-,,$@)

defconfig: prepare
	@echo --- $(or $(CONFIG),$(error variable BOARD is not found))
	@$(BR_MAKE) BR2_DEFCONFIG=$(PWD)/$(CONFIG) defconfig

prepare:
	@if test ! -e $(TARGET)/buildroot-$(BR_VER); then \
		wget -c -q $(BR_LINK)/$(BR_VER).tar.gz -O $(BR_FILE); \
		mkdir -p $(TARGET); tar -xf $(BR_FILE) -C $(TARGET); fi

help:
	@printf "BR-OpenIPC usage:\n \
	- make list - show available device configurations\n \
	- make deps - install build dependencies\n \
	- make clean - remove defconfig and target folder\n \
	- make package - list available packages\n \
	- make distclean - remove buildroot and output folder\n \
	- make br-linux - build linux kernel only\n\n"

list:
	@ls -1 br-ext-chip-*/configs

package:
	@find general/package/* -maxdepth 0 -type d -printf "br-%f\n" | grep -v patch

toolname:
	@general/scripts/show_toolchains.sh $(CONFIG)

clean:
	@rm -rf $(TARGET)/build $(TARGET)/images $(TARGET)/per-package $(TARGET)/target

distclean:
	@rm -rf $(BR_FILE) $(TARGET)

deps:
	sudo apt-get install -y automake autotools-dev bc build-essential cpio \
		curl file fzf git libncurses-dev libtool lzop make rsync unzip wget libssl-dev

timer:
	@echo - Build time: $(shell date -d @$(shell expr $(shell date +%s) - $(TIMER)) -u +%M:%S)

repack:
ifeq ($(BR2_TARGET_ROOTFS_SQUASHFS),y)
ifeq ($(BR2_OPENIPC_FLASH_SIZE),"8")
	@$(call PREPARE_REPACK,uImage,2048,rootfs.squashfs,5120,nor)
else
	@$(call PREPARE_REPACK,uImage,2048,rootfs.squashfs,8192,nor)
endif
endif
ifeq ($(BR2_TARGET_ROOTFS_UBI),y)
ifeq ($(BR2_OPENIPC_SOC_VENDOR),"rockchip")
	@$(call PREPARE_REPACK,zboot.img,4096,rootfs.ubi,16384,nand)
else ifeq ($(BR2_OPENIPC_SOC_VENDOR),"sigmastar")
	@$(call PREPARE_REPACK,,,rootfs.ubi,16384,nand)
else
	@$(call PREPARE_REPACK,uImage,4096,rootfs.ubi,16384,nand)
endif
endif
ifeq ($(BR2_TARGET_ROOTFS_INITRAMFS),y)
	@$(call PREPARE_REPACK,uImage,16384,,,initramfs)
endif

define PREPARE_REPACK
	$(if $(1),$(call CHECK_SIZE,$(1),$(2)))
	$(if $(3),$(call CHECK_SIZE,$(3),$(4)))
	$(call REPACK_FIRMWARE,$(1),$(3),$(5))
endef

define CHECK_SIZE
	$(eval FILE_SIZE = $(shell expr $(shell stat -c %s $(TARGET)/images/$(1) || echo 0) / 1024))
	if test $(FILE_SIZE) -eq 0; then exit 1; fi
	echo - $(1): [$(FILE_SIZE)KB/$(2)KB]
	if test $(FILE_SIZE) -gt $(2); then \
		echo -- size exceeded by: $(shell expr $(FILE_SIZE) - $(2))KB; exit 1; fi
endef

define REPACK_FIRMWARE
	cd $(TARGET)/images && if test -e rootfs.tar; then mv -f rootfs.tar rootfs.$(BR2_OPENIPC_SOC_MODEL).tar; fi
	$(if $(1),cd $(TARGET)/images && if test -e $(1); then mv -f $(1) $(1).$(BR2_OPENIPC_SOC_MODEL); fi)
	$(if $(2),cd $(TARGET)/images && if test -e $(2); then mv -f $(2) $(2).$(BR2_OPENIPC_SOC_MODEL); fi)
	$(if $(1),cd $(TARGET)/images && md5sum $(1).$(BR2_OPENIPC_SOC_MODEL) > $(1).$(BR2_OPENIPC_SOC_MODEL).md5sum)
	$(if $(2),cd $(TARGET)/images && md5sum $(2).$(BR2_OPENIPC_SOC_MODEL) > $(2).$(BR2_OPENIPC_SOC_MODEL).md5sum)
	$(if $(1),$(eval KERNEL = $(1).$(BR2_OPENIPC_SOC_MODEL) $(1).$(BR2_OPENIPC_SOC_MODEL).md5sum),$(eval KERNEL =))
	$(if $(2),$(eval ROOTFS = $(2).$(BR2_OPENIPC_SOC_MODEL) $(2).$(BR2_OPENIPC_SOC_MODEL).md5sum),$(eval ROOTFS =))
	$(eval ARCHIVE = openipc.$(BR2_OPENIPC_SOC_MODEL)-$(3)-$(BR2_OPENIPC_VARIANT).tgz)
	cd $(TARGET)/images && tar -czf $(ARCHIVE) $(KERNEL) $(ROOTFS)
	rm -f $(TARGET)/images/*.md5sum
endef
