
ROOT_DIR      := $(CURDIR)
BR_VER        := 2020.02.12
BR_DIR        := $(ROOT_DIR)/buildroot-$(BR_VER)

ifeq ($(PLATFORM),)
    ifneq ($(BOARD),)
		FULL_PATH := $(shell find br-ext-chip-* -name "$(BOARD)*_defconfig")
		ifeq ($(FULL_PATH),)
			FULL_PATH := $(error Cannot find anything for $(BOARD))
		else ifneq ($(shell echo $(FULL_PATH) | wc -w), 1)
			FULL_PATH := $(error For provided '$(BOARD)' multiple options found: $(FULL_PATH))
		endif

		PLATFORM := $(shell echo $(FULL_PATH) | cut -d '/' -f 1 | cut -d '-' -f 4 )
    endif
endif

BR_EXT_DIR    := $(ROOT_DIR)/br-ext-chip-$(PLATFORM)
SCRIPTS_DIR   := $(ROOT_DIR)/scripts
#BOARDS       := $(shell ls -1 $(BR_EXT_DIR)/configs)

.PHONY: usage help clean distclean prepare install-deps all toolchain-params run-tests overlayed-rootfs-%

usage help:
	@echo -e \
	"\n\
	BR-OpenIPC usage:\n\
	  - make help|usage - print this help\n\
	  - make install-deps - install system deps\n\
	  - make prepare - download and unpack buildroot\n\
	  - make list-configs - show available hardware configs list\n\
	  - make BOARD=<BOARD-ID> board-info - write to stdout information about selected board\n\
	  - make BOARD=<BOARD-ID> all - build all needed for a board (toolchain, kernel and rootfs images)\n\
	  - make overlayed-rootfs-<FS-TYPE> ROOTFS_OVERLAYS=... - create rootfs image that contains original\n\
	  - make clean - cleaning before reassembly\n\
	  - make distclean - switching to the factory state\n\
	  Buildroot target dir overlayed by some custom layers.\n\
	  Example:\n\
	      make overlayed-rootfs-squashfs ROOTFS_OVERLAYS=./examples/echo_server/overlay\n\
	"

distclean:
	@rm -rf output buildroot-$(BR_VER)

clean:
	@rm -rf output/target output/.config

prepare: $(BR_DIR)
$(ROOT_DIR)/buildroot-$(BR_VER).tar.gz:
	wget -O $@ --header="Host: buildroot.org" --no-check-certificate https://buildroot.org/downloads/buildroot-$(BR_VER).tar.gz

$(BR_DIR): $(ROOT_DIR)/buildroot-$(BR_VER).tar.gz
	tar -C $(ROOT_DIR) -xf buildroot-$(BR_VER).tar.gz
	rm -f buildroot-$(BR_VER).tar.gz


install-deps:
	DEBIAN_FRONTEND=noninteractive sudo apt-get update && \
		sudo apt-get -y install build-essential make libncurses-dev wget cpio rsync bc


%_info:
	@echo
	@cat $(BR_EXT_DIR)/board/$(subst _info,,$@)/config | grep RAM_LINUX_SIZE
	$(eval VENDOR 	:= $(shell echo $@ | cut -d "_" -f 1))
	$(eval FAMILY 	:= $(shell cat $(BR_EXT_DIR)/board/$(subst _info,,$@)/config | grep FAMILY | cut -d "=" -f 2))
	$(eval CHIP	:= $(shell echo $@ | cut -d "_" -f 3))
	@cat $(BR_EXT_DIR)/board/$(FAMILY)/$(CHIP).config

has-nand:
	@sed -rn "s/^BR2_TARGET_ROOTFS_UBI=(y)/\1/p" $(FULL_PATH)

toolname:
	@$(SCRIPTS_DIR)/show_toolchains.sh board $(FULL_PATH)

list-configs:
	@echo
	@ls -1 $(BR_EXT_DIR)/configs


# -------------------------------------------------------------------------------------------------
OUT_DIR ?= $(ROOT_DIR)/output

# Buildroot considers relative paths relatively to its' own root directory. So we use absolute paths
# to avoid ambiguity
override OUT_DIR := $(abspath $(OUT_DIR))
BOARD_MAKE := $(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(BR_EXT_DIR) O=$(OUT_DIR)

define CREATE_TOOLCHAIN_PARAMS
    eval $$($(BOARD_MAKE) -s --no-print-directory VARS=GNU_TARGET_NAME printvars) \
    && $(SCRIPTS_DIR)/create_toolchain_binding.sh $(OUT_DIR)/host/bin $$GNU_TARGET_NAME \
    > $(OUT_DIR)/toolchain-params.mk
endef

# -------------------------------------------------------------------------------------------------
$(OUT_DIR)/.config:
ifndef BOARD
	@echo "Variable BOARD must be defined to initialize output directory" >&2 && exit 1
endif
	$(BOARD_MAKE) BR2_DEFCONFIG=$(BR_EXT_DIR)/configs/$(BOARD)_defconfig defconfig


$(OUT_DIR)/toolchain-params.mk: $(OUT_DIR)/.config $(SCRIPTS_DIR)/create_toolchain_binding.sh
	$(CREATE_TOOLCHAIN_PARAMS)


# -------------------------------------------------------------------------------------------------
# build all needed for a board
all: $(OUT_DIR)/.config $(OUT_DIR)/toolchain-params.mk
	$(BOARD_MAKE) all


# -------------------------------------------------------------------------------------------------
# re-create params file
toolchain-params:
	$(CREATE_TOOLCHAIN_PARAMS)

# -------------------------------------------------------------------------------------------------
# create rootfs image that contains original Buildroot target dir overlayed by some custom layers
# space-separated list of overlays

ROOTFS_OVERLAYS ?=
# overlayed rootfs directory
ROOTFS_OVERLAYED_DIR ?= $(OUT_DIR)/target-overlayed
# overlayed rootfs image's name (without prefix)
ROOTFS_OVERLAYED_IMAGE ?= rootfs-overlayed

overlayed-rootfs-%: $(OUT_DIR)/.config
	$(SCRIPTS_DIR)/create_overlayed_rootfs.sh $(ROOTFS_OVERLAYED_DIR) $(OUT_DIR)/target $(ROOTFS_OVERLAYS)
	$(BOARD_MAKE) $(subst overlayed-,,$@) \
	    BASE_TARGET_DIR=$(abspath $(ROOTFS_OVERLAYED_DIR)) \
	    ROOTFS_$(call UPPERCASE,$(subst overlayed-rootfs-,,$@))_FINAL_IMAGE_NAME=$(ROOTFS_OVERLAYED_IMAGE).$(subst overlayed-rootfs-,,$@)


# -------------------------------------------------------------------------------------------------
board-info:
	@cat $(BR_EXT_DIR)/board/$(BOARD)/config | grep RAM_LINUX_SIZE
	$(eval VENDOR := $(shell echo $(BOARD) | cut -d "_" -f 1))
	$(eval FAMILY := $(shell cat $(BR_EXT_DIR)/board/$(BOARD)/config | grep FAMILY | cut -d "=" -f 2))
	$(eval CHIP   := $(shell echo $(BOARD) | cut -d "_" -f 3))
	@cat $(BR_EXT_DIR)/board/$(FAMILY)/$(CHIP).config
	@cat $(BR_EXT_DIR)/board/$(BOARD)/config

# -------------------------------------------------------------------------------------------------
# such targets (with trimmed `br-` prefix) are passed to Buildroot's Makefile
br-%: $(OUT_DIR)/.config
	$(BOARD_MAKE) $(subst br-,,$@)


# -------------------------------------------------------------------------------------------------
run-tests:
	$(MAKE) -C $(ROOT_DIR)/tests


# -------------------------------------------------------------------------------------------------
# there are some extra targets of specific packages
include $(sort $(wildcard $(ROOT_DIR)/extra/*.mk))


# -------------------------------------------------------------------------------------------------
# util stuff is below
UPPERCASE = $(shell echo $(1) | tr a-z A-Z)
