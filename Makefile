BR_VER = 2021.02.12
BR_DIR = $(PWD)/buildroot-$(BR_VER)
BR_MAKE = $(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(PWD)/general O=$(PWD)/output
CHECK = $(wildcard $(BR_DIR))

ifdef BOARD
	CONFIG = $(shell find br-ext-chip-*/configs -type f | grep -m1 $(BOARD))
endif

ifeq ($(CONFIG),)
	CONFIG = $(error variable BOARD must be defined to initialize build)
else
	ifneq ($(shell grep GCC_VERSION_12 $(CONFIG)),)
		BR_VER = 2023.02
	endif
endif

.PHONY: all clean defconfig distclean help prepare toolname

help:
	@printf "BR-OpenIPC usage:\n \
	- make clean - remove defconfig and target folder\n \
	- make distclean - remove buildroot and output folder\n \
	- make list-configs - show available board configurations\n \
	- make all BOARD=<device> - builds the selected board\n\n"

all: defconfig
	@$(BR_MAKE) all

br-%: defconfig
	@$(BR_MAKE) $(subst br-,,$@)

defconfig: prepare
	@$(BR_MAKE) BR2_DEFCONFIG=$(PWD)/$(CONFIG) defconfig

toolname: prepare
	@general/scripts/show_toolchains.sh $(CONFIG) $(BR_VER)

prepare:
ifeq ($(CHECK),)
	@wget -O $(BR_DIR).tar.gz -nv --retry-connrefused --timeout=3 \
		https://github.com/buildroot/buildroot/archive/refs/tags/$(BR_VER).tar.gz
	@tar -C $(PWD) -xf $(BR_DIR).tar.gz
	@rm -f $(BR_DIR).tar.gz
endif

buildroot-version:
	@echo $(BR_VER)

clean:
	@rm -rf output/target output/.config

distclean:
	@rm -rf output buildroot-*.*

list-configs:
	@ls -1 br-ext-chip-*/configs
