BR_VER = 2024.02.10
BR_MAKE = $(MAKE) -C $(TARGET)/buildroot-$(BR_VER) BR2_EXTERNAL=$(PWD)/general O=$(TARGET)
BR_LINK = https://github.com/buildroot/buildroot/archive
BR_FILE = /tmp/buildroot-$(BR_VER).tar.gz
BR_CONF = $(TARGET)/openipc_defconfig
TARGET ?= $(PWD)/output
export CMAKE_POLICY_VERSION_MINIMUM := 3.5

# GCC 15 defaults to -std=gnu23, where an empty parameter list means "takes no
# arguments" rather than "unspecified". Several host packages Buildroot pins
# here predate that and their configure probes stop compiling: gmp 6.3.0 fails
# its compiler test with "too many arguments to function 'g'", which surfaces as
# the far less helpful "could not find a working compiler" and halts any
# `make toolchain` on a current distro. Pin the dialect rather than carry a
# version bump for every affected host package. Applies only to host C builds,
# is overridable from the environment, and is a no-op on hosts whose GCC still
# defaults to gnu17.
#
# "Applies only to host C builds" is what the prepare: rule below has to make
# true -- Buildroot appends HOST_CFLAGS to HOST_CXXFLAGS wholesale.
HOST_CFLAGS ?= -O2 -std=gnu17
export HOST_CFLAGS

CONFIG = $(error variable BOARD not defined)
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

ifneq ($(filter repack,$(MAKECMDGOALS)),)
-include $(BR_CONF)
endif

all: repack-final timer

build: defconfig
	@$(BR_MAKE) all -j$(shell nproc)

br-%: defconfig
	@$(BR_MAKE) $(subst br-,,$@) -j$(shell nproc)

defconfig: prepare
	@echo --- $(or $(CONFIG),$(error variable BOARD not found))
	@cat $(CONFIG) $(PWD)/general/openipc.fragment > $(BR_CONF)
	@grep -s '^BR2_GLOBAL_PATCH_DIR=' $(CONFIG) >> $(BR_CONF) || true
	@$(BR_MAKE) BR2_DEFCONFIG=$(BR_CONF) defconfig

prepare:
	@if test ! -e $(TARGET)/buildroot-$(BR_VER); then \
		wget -c -q $(BR_LINK)/$(BR_VER).tar.gz -O $(BR_FILE); \
		mkdir -p $(TARGET); tar -xf $(BR_FILE) -C $(TARGET); fi
	@# The majestic and majestic-webui tarballs are rolling release assets:
	@# a fixed filename, no hash, refreshed upstream whenever those repos
	@# publish. Buildroot's dl cache keeps the first copy forever, so a
	@# from-source build with an old cache pairs a majestic that expects the
	@# setup page with a webui from before the page existed -- the browser
	@# door then 404s while SSH works. Expire cached copies after a day;
	@# fresh ones are kept, offline rebuilds inside that window still work,
	@# and CI downloads into an empty cache every run and never gets here.
	@find $(or $(BR2_DL_DIR),$(TARGET)/buildroot-$(BR_VER)/dl) -maxdepth 2 \
		\( -name 'majestic.*.master.tar.bz2' -o -name 'majestic-webui-dist.tar.gz' \) \
		-mmin +1440 -delete 2>/dev/null || true
	@if test -f $(TARGET)/buildroot-$(BR_VER)/linux/Config.in; then \
		sed -i '/source "$$(BR2_EXTERNAL_GENERAL_PATH)\/linux\/Config.ext.in"/d' \
			$(TARGET)/buildroot-$(BR_VER)/linux/Config.in; \
		grep -qF 'source "$$BR2_EXTERNAL_GENERAL_PATH/linux/Config.ext.in"' \
			$(TARGET)/buildroot-$(BR_VER)/linux/Config.in || \
		sed -i '/source "linux\/Config.ext.in"/a source "$$BR2_EXTERNAL_GENERAL_PATH/linux/Config.ext.in"' \
			$(TARGET)/buildroot-$(BR_VER)/linux/Config.in; \
	fi
	@# Keep the C dialect pinned at the top of this file out of host C++ builds.
	@# package/Makefile.in does `HOST_CXXFLAGS += $$(HOST_CFLAGS)`, so -std=gnu17
	@# reaches every host C++ compile, where it is not a C++ dialect at all: g++
	@# ignores it and prints "command-line option '-std=gnu17' is valid for
	@# C/ObjC but not for C++". Compilation still succeeds -- what does not is
	@# CMake, whose cm_check_cxx_feature discards any feature whose try_compile
	@# output contains the word "warning" (Source/Checks/cm_cxx_features.cmake).
	@# host-cmake therefore decides the compiler has no std::unique_ptr and
	@# aborts its own configure, taking every `make BOARD=...` with it.
	@# Filtering -std= rather than that one value so a C dialect set from the
	@# environment does not reintroduce this.
	@if test -f $(TARGET)/buildroot-$(BR_VER)/package/Makefile.in; then \
		grep -qF 'filter-out -std=%' \
			$(TARGET)/buildroot-$(BR_VER)/package/Makefile.in || \
		sed -i 's|^HOST_CXXFLAGS += \$$(HOST_CFLAGS)$$|HOST_CXXFLAGS += $$(filter-out -std=%,$$(HOST_CFLAGS))|' \
			$(TARGET)/buildroot-$(BR_VER)/package/Makefile.in; \
	fi

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
	@find $(PWD)/general/package/* -maxdepth 0 -type d -printf "br-%f\n" | grep -v patch

toolname:
	@echo toolchain.$(BR2_OPENIPC_SOC_VENDOR)-$(BR2_OPENIPC_SOC_FAMILY)

clean:
	@rm -rf $(TARGET)/build $(TARGET)/images $(TARGET)/per-package $(TARGET)/target

distclean:
	@rm -rf $(BR_FILE) $(TARGET)

audit-abi:
	@python3 $(PWD)/general/scripts/audit-vendor-abi.py

deps:
	sudo apt-get install -y automake autotools-dev bc build-essential cpio \
		curl file fzf git libncurses-dev libtool lzop make rsync unzip wget libssl-dev \
		python3 python3-pip
	# kconfiglib is the only non-stdlib dep added by general/scripts/kconfig_graph.py;
	# install with --break-system-packages on PEP 668 distros (Ubuntu 24.04+, Debian 12+).
	python3 -m pip install --user --break-system-packages kconfiglib

timer:
	@echo - Build time: $(shell date -d @$(shell expr $(shell date +%s) - $(TIMER)) -u +%M:%S)

toolchain: defconfig
ifeq ($(BR2_TOOLCHAIN_EXTERNAL),y)
	@cp -rf $(PWD)/general/package/gcc $(TARGET)/buildroot-$(BR_VER)/package
	@$(MAKE) -f $(PWD)/general/toolchain.mk BR_CONF=$(BR_CONF) CONFIG=$(PWD)/$(CONFIG)
	@$(BR_MAKE) BR2_DEFCONFIG=$(BR_CONF) defconfig
endif
	@$(BR_MAKE) sdk -j$(shell nproc)
	@$(call BUNDLE_SDK)

toolchain-asan: defconfig
ifeq ($(BR2_TOOLCHAIN_EXTERNAL),y)
	@cp -rf $(PWD)/general/package/gcc $(TARGET)/buildroot-$(BR_VER)/package
	@$(MAKE) -f $(PWD)/general/toolchain.mk BR_CONF=$(BR_CONF) CONFIG=$(PWD)/$(CONFIG)
	@$(BR_MAKE) BR2_DEFCONFIG=$(BR_CONF) defconfig
endif
	@echo 'BR2_EXTRA_GCC_CONFIG_OPTIONS="--enable-libsanitizer"' >> $(BR_CONF)
	@$(BR_MAKE) BR2_DEFCONFIG=$(BR_CONF) defconfig
	@$(BR_MAKE) sdk -j$(shell nproc)
	@$(call BUNDLE_SDK)

repack-final: build
	@$(MAKE) --no-print-directory BOARD=$(BOARD) TARGET=$(TARGET) repack

repack:
ifeq ($(BR2_PACKAGE_OPENIPC_NFS_ROOT),y)
ifeq ($(BR2_OPENIPC_SOC_VENDOR),"rockchip")
	@$(call PREPARE_REPACK,zboot.img,16384,,,nfs-root)
else
	@$(call PREPARE_REPACK,uImage,16384,,,nfs-root)
endif
else
ifneq ($(wildcard $(TARGET)/images/firmware.bin),)
# The NOR split layout (general/scripts/post-image-split.sh): firmware.bin is
# the uImage and the squashfs back to back in one "firmware" partition the
# kernel splits at boot, whatever bootloader loaded it, shipped as the -split
# package. Only its whole is bounded: the chip, less what comes before the
# firmware partition (BR2_OPENIPC_NOR_FIRMWARE_OFFSET) and the smallest overlay
# worth keeping (256K).
#
# A board whose older cameras upgrade from fixed slots also ships those, the
# plain package of uImage and rootfs. The uImage stays a 64 KiB block short of
# its 2 MiB slot: under 1984K, which CHECK_SIZE, counting whole KB, enforces as
# 1983K. The kernel, U-Boot and sysupgrade tell the layouts apart by where the
# squashfs is: at the block right after the uImage is the split layout, at the
# slot boundary fixed slots. A fixed-slot uImage ending in the slot's last block
# would be both.
#
# The fixed-slot package is the extra: a build whose uImage or rootfs has
# outgrown its slot ships the split package alone rather than failing, and
# says so. Cameras on fixed slots then upgrade to the split layout, which
# sysupgrade and defib install from the -split package.
ifeq ($(BR2_OPENIPC_NOR_FIXED_PACKAGE),y)
ifeq ($(BR2_OPENIPC_FLASH_SIZE),"8")
	@$(if $(call FIXED_SLOTS_FIT,5120),$(call PREPARE_REPACK,uImage,1983,rootfs.squashfs,5120,nor),$(call FIXED_SLOTS_SKIP,5120))
else
	@$(if $(call FIXED_SLOTS_FIT,8192),$(call PREPARE_REPACK,uImage,1983,rootfs.squashfs,8192,nor),$(call FIXED_SLOTS_SKIP,8192))
endif
endif
	@$(call CHECK_SIZE,firmware.bin,$(shell expr $(subst ",,$(BR2_OPENIPC_FLASH_SIZE)) \* 1024 - $$(( $(or $(BR2_OPENIPC_NOR_FIRMWARE_OFFSET),0x50000) / 1024 )) - 256))
	@$(call CHECK_KERNEL_READ)
	@$(call REPACK_NOR_SPLIT)
else
ifeq ($(BR2_TARGET_ROOTFS_SQUASHFS),y)
ifeq ($(BR2_OPENIPC_SOC_VENDOR),"rockchip")
	@$(call PREPARE_REPACK,zboot.img,4096,rootfs.squashfs,8192,nor)
else ifeq ($(BR2_OPENIPC_FLASH_SIZE),"8")
	@$(call PREPARE_REPACK,uImage,2048,rootfs.squashfs,5120,nor)
else
	@$(call PREPARE_REPACK,uImage,2048,rootfs.squashfs,8192,nor)
endif
endif
endif
# A NOR+NAND build (ultimate with BR2_TARGET_ROOTFS_UBI) packages its NAND
# image whichever NOR layout it ships.
ifeq ($(BR2_TARGET_ROOTFS_UBI),y)
ifneq ($(filter $(BR2_OPENIPC_SOC_VENDOR),"rockchip" "sigmastar"),)
	@$(call PREPARE_REPACK,,,rootfs.ubi,16384,nand)
else ifneq ($(wildcard $(PWD)/br-ext-chip-$(subst ",,$(BR2_OPENIPC_SOC_VENDOR))/board/$(subst ",,$(BR2_OPENIPC_SOC_FAMILY))/nand-fit.its),)
# FIT NAND (board/<family>/nand-fit.its): the kernel lives inside the UBIFS
# rootfs (/boot, see external.mk), so the package carries what sysupgrade
# writes -- rootfs.ubifs -- plus rootfs.ubi for a fresh install, and fitImage
# as the SoC witness sysupgrade reads beside a UBIFS rootfs. No volume bounds
# either image: sysupgrade sizes the volumes to them. rootfs.ubi is the whole
# UBI image a fresh install loads into RAM at 0x42000000 and writes from there,
# so it is held to the 24M the installer stages (openipc.org's 0x1800000),
# which still clears the relocated U-Boot at the top of a 64M part
# (hi3516ev200).
#
# gk7205v500 is held to 32M instead: its ultimate build carries the NPU's
# object detectors (xmnpu-models, 22 MB, NAND only), which take rootfs.ubi to
# about 29.5M. Its parts have 128M of DDR, so the image still clears U-Boot,
# but a fresh install has to stage at least 0x1E00000 for it -- the 0x1800000
# above would truncate it. sysupgrade writes rootfs.ubifs and is unaffected.
ifeq ($(BR2_OPENIPC_SOC_FAMILY),"gk7205v500")
	@$(call CHECK_SIZE,rootfs.ubi,32768)
else
	@$(call CHECK_SIZE,rootfs.ubi,24576)
endif
	@$(call REPACK_NAND_FIT)
else
	@$(call PREPARE_REPACK,uImage,4096,rootfs.ubi,16384,nand)
endif
endif
ifeq ($(BR2_TARGET_ROOTFS_INITRAMFS),y)
	@$(call PREPARE_REPACK,uImage,16384,,,initramfs)
endif
endif

size-report:
	@TARGET_DIR=$(TARGET)/target \
	BR2_OUTPUT_DIR=$(TARGET) \
	IMAGES_DIR=$(TARGET)/images \
	OPENIPC_SOC_MODEL=$(BR2_OPENIPC_SOC_MODEL) \
	OPENIPC_VARIANT=$(BR2_OPENIPC_VARIANT) \
	BR2_OPENIPC_FLASH_SIZE=$(BR2_OPENIPC_FLASH_SIZE) \
	BR2_OPENIPC_SOC_VENDOR=$(BR2_OPENIPC_SOC_VENDOR) \
	BR2_TARGET_ROOTFS_SQUASHFS=$(BR2_TARGET_ROOTFS_SQUASHFS) \
	BR2_TARGET_ROOTFS_UBI=$(BR2_TARGET_ROOTFS_UBI) \
	python3 $(PWD)/general/scripts/size_report.py

kconfig-graph:
	@TARGET_DIR=$(TARGET)/target \
	BR2_OUTPUT_DIR=$(TARGET) \
	IMAGES_DIR=$(TARGET)/images \
	OPENIPC_SOC_MODEL=$(BR2_OPENIPC_SOC_MODEL) \
	OPENIPC_VARIANT=$(BR2_OPENIPC_VARIANT) \
	BR_VER=$(BR_VER) \
	PWD=$(PWD) \
	python3 $(PWD)/general/scripts/kconfig_graph.py

define BUNDLE_SDK
	OSDRV_DIR=$(PWD)/general/package/$(BR2_OPENIPC_SOC_VENDOR)-osdrv-$(BR2_OPENIPC_SOC_FAMILY)/files; \
	MPP_HEADERS=$(PWD)/general/package/hisilicon-osdrv-hi3516cv100/files/include; \
	SDK_TGZ=$$(find $(TARGET)/images -name '*_sdk-buildroot.tar.gz' | head -1); \
	UCLIBC_COMPAT_SRC=$(PWD)/general/package/uclibc-compat/src/uclibc-compat.c; \
	UCLIBC_COMPAT_STATIC=$(PWD)/general/package/uclibc-compat/src/uclibc-compat-static.c; \
	GLIBC_COMPAT_SRC=$(PWD)/general/package/glibc-compat/src/glibc-compat.c; \
	GLIBC_COMPAT_STATIC=$(PWD)/general/package/glibc-compat/src/glibc-compat-static.c; \
	SDK_CC=$$(ls $(TARGET)/host/bin/*-gcc 2>/dev/null | head -1); \
	if [ -d "$$OSDRV_DIR" ] && [ -n "$$SDK_TGZ" ]; then \
		SDK_TOP=$$(tar tzf $$SDK_TGZ | head -1 | cut -d/ -f1); \
		rm -rf /tmp/sdk-overlay && mkdir -p /tmp/sdk-overlay/$$SDK_TOP/sdk; \
		cp -a $$OSDRV_DIR/* /tmp/sdk-overlay/$$SDK_TOP/sdk/; \
		if [ "$(BR2_OPENIPC_SOC_VENDOR)" = "hisilicon" ] && [ ! -d "$$OSDRV_DIR/include" ] && [ -d "$$MPP_HEADERS" ]; then \
			mkdir -p /tmp/sdk-overlay/$$SDK_TOP/sdk/include; \
			cp -a $$MPP_HEADERS/. /tmp/sdk-overlay/$$SDK_TOP/sdk/include/; \
		fi; \
		if [ -n "$$SDK_CC" ]; then \
			SDK_AR=$$(echo $$SDK_CC | sed 's/-gcc$$/-ar/'); \
			if [ -f "$$UCLIBC_COMPAT_SRC" ]; then \
				$$SDK_CC -shared -Wall -O2 -fPIC \
					-o /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/libuclibc-compat.so \
					$$UCLIBC_COMPAT_SRC; \
			fi; \
			if [ -f "$$UCLIBC_COMPAT_STATIC" ]; then \
				$$SDK_CC -Wall -O2 -fPIC -c \
					-o /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/uclibc-compat-static.o \
					$$UCLIBC_COMPAT_STATIC; \
				$$SDK_AR rcs /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/libuclibc-compat-static.a \
					/tmp/sdk-overlay/$$SDK_TOP/sdk/lib/uclibc-compat-static.o; \
				rm -f /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/uclibc-compat-static.o; \
			fi; \
			if [ -f "$$GLIBC_COMPAT_SRC" ]; then \
				$$SDK_CC -shared -Wall -O2 -fPIC \
					-o /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/libglibc-compat.so \
					$$GLIBC_COMPAT_SRC; \
			fi; \
			if [ -f "$$GLIBC_COMPAT_STATIC" ]; then \
				$$SDK_CC -Wall -O2 -fPIC -c \
					-o /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/glibc-compat-static.o \
					$$GLIBC_COMPAT_STATIC; \
				$$SDK_AR rcs /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/libglibc-compat-static.a \
					/tmp/sdk-overlay/$$SDK_TOP/sdk/lib/glibc-compat-static.o; \
				rm -f /tmp/sdk-overlay/$$SDK_TOP/sdk/lib/glibc-compat-static.o; \
			fi; \
		fi; \
		gunzip $$SDK_TGZ && \
		tar rf $${SDK_TGZ%.tar.gz}.tar -C /tmp/sdk-overlay $$SDK_TOP && \
		gzip $${SDK_TGZ%.tar.gz}.tar; \
		rm -rf /tmp/sdk-overlay; \
	fi
endef

define PREPARE_REPACK
	$(if $(1),$(call CHECK_SIZE,$(1),$(2)))
	$(if $(3),$(call CHECK_SIZE,$(3),$(4)))
	$(call REPACK_FIRMWARE,$(1),$(3),$(5))
endef

# The headroom line exists because "fits" and "only just fits" read the same in
# a green build. hi3519v101_lite sat at exactly 5120KB of a 5120KB cap for weeks
# -- reported, passing, and one 34-line edit from the overflow it hit on
# 2026-08-18. 32KB is the threshold because what tips these boards is a change
# to the shared overlay, which is single-digit KB at a time; a board under that
# is a couple of ordinary commits from red, and a board over it is not.
# The uImage at the start of firmware.bin against what a bootloader in the field
# reads from there to boot it (BR2_OPENIPC_NOR_KERNEL_READ_SIZE), in bytes: one
# byte over is a kernel read short and a failed CRC. Measured as a bootloader
# sees it, from firmware.bin's own uImage header (64 bytes, then the data size
# big-endian at byte 12): the fixed-slot package's repack has already renamed
# images/uImage on the boards that ship one.
define CHECK_KERNEL_READ
	hex=$$(od -An -tx1 -j12 -N4 $(TARGET)/images/firmware.bin | tr -d ' \n'); \
	test -n "$$hex" || exit 1; \
	size=$$((64 + 0x$$hex)); \
	limit=$$(( $(or $(BR2_OPENIPC_NOR_KERNEL_READ_SIZE),0x200000) )); \
	echo "- uImage: [$$((size / 1024))KB/$$((limit / 1024))KB read by the bootloader]"; \
	if test $$size -gt $$limit; then \
		echo "-- size exceeded by: $$((size - limit)) bytes; bootloaders reading $$((limit / 1024))KB would cut it short"; exit 1; fi
endef

define CHECK_SIZE
	$(eval FILE_SIZE = $(shell expr $(shell stat -c %s $(TARGET)/images/$(1) || echo 0) / 1024))
	if test $(FILE_SIZE) -eq 0; then exit 1; fi
	echo - $(1): [$(FILE_SIZE)KB/$(2)KB]
	if test $(FILE_SIZE) -gt $(2); then \
		echo -- size exceeded by: $(shell expr $(FILE_SIZE) - $(2))KB; exit 1; fi
	if test $(shell expr $(2) - $(FILE_SIZE)) -lt 32; then \
		echo -- headroom warning: $(1) has $(shell expr $(2) - $(FILE_SIZE))KB left of $(2)KB; fi
endef

define REPACK_FIRMWARE
	cd $(TARGET)/images && if test -e rootfs.tar; then mv -f rootfs.tar rootfs.$(BR2_OPENIPC_SOC_MODEL).tar; fi
	$(if $(1),cd $(TARGET)/images && if test -e $(1); then mv -f $(1) $(1).$(BR2_OPENIPC_SOC_MODEL); fi)
	$(if $(2),cd $(TARGET)/images && if test -e $(2); then mv -f $(2) $(2).$(BR2_OPENIPC_SOC_MODEL); fi)
	$(if $(1),cd $(TARGET)/images && md5sum $(1).$(BR2_OPENIPC_SOC_MODEL) > $(1).$(BR2_OPENIPC_SOC_MODEL).md5sum)
	$(if $(2),cd $(TARGET)/images && md5sum $(2).$(BR2_OPENIPC_SOC_MODEL) > $(2).$(BR2_OPENIPC_SOC_MODEL).md5sum)
	$(if $(1),$(eval KERNEL = $(1).$(BR2_OPENIPC_SOC_MODEL)),$(eval KERNEL =))
	$(if $(2),$(eval ROOTFS = $(2).$(BR2_OPENIPC_SOC_MODEL)),$(eval ROOTFS =))
	$(if $(1),$(eval KERNEL_MD5 = $(1).$(BR2_OPENIPC_SOC_MODEL).md5sum),$(eval KERNEL_MD5 =))
	$(if $(2),$(eval ROOTFS_MD5 = $(2).$(BR2_OPENIPC_SOC_MODEL).md5sum),$(eval ROOTFS_MD5 =))
	$(eval ARCHIVE = openipc.$(BR2_OPENIPC_SOC_MODEL)-$(3)-$(BR2_OPENIPC_VARIANT).tgz)
	# Checksums first, so an unpack that runs out of room in /tmp on a 32 MB
	# camera loses the IMAGE and keeps the .md5sum that convicts it. The other
	# order loses the checksum and leaves a short image that sysupgrade's
	# `md5sum -c *.md5sum` then cannot see at all.
	cd $(TARGET)/images && tar -czf $(ARCHIVE) $(KERNEL_MD5) $(ROOTFS_MD5) $(KERNEL) $(ROOTFS)
	rm -f $(TARGET)/images/*.md5sum
endef

# Whether uImage and rootfs.squashfs fit the fixed slots beside a split image:
# the uImage under 1984K (see the repack target), the rootfs within $(1) KB,
# both counted in whole KB as CHECK_SIZE counts them. "y" or empty.
FIXED_SLOTS_FIT = $(shell k=$$(( $$(stat -c %s $(TARGET)/images/uImage 2>/dev/null || echo 0) / 1024 )); \
	r=$$(( $$(stat -c %s $(TARGET)/images/rootfs.squashfs 2>/dev/null || echo 0) / 1024 )); \
	[ $$k -gt 0 ] && [ $$k -le 1983 ] && [ $$r -gt 0 ] && [ $$r -le $(1) ] && echo y)

define FIXED_SLOTS_SKIP
	k=$$(( $$(stat -c %s $(TARGET)/images/uImage) / 1024 )); \
	r=$$(( $$(stat -c %s $(TARGET)/images/rootfs.squashfs) / 1024 )); \
	over=; \
	[ $$k -le 1983 ] || over="uImage $${k}KB/1983KB"; \
	[ $$r -le $(1) ] || over="$${over:+$$over, }rootfs.squashfs $${r}KB/$(1)KB"; \
	echo "::warning::$(BR2_OPENIPC_SOC_MODEL) $(BR2_OPENIPC_VARIANT): $$over outgrew the fixed slot;" \
		"shipping the split package only"
endef

# The split NOR package: firmware.bin alone, under its own name so a sysupgrade
# that predates the layout never sees it. Copies, as REPACK_NAND_FIT does.
define REPACK_NOR_SPLIT
	cd $(TARGET)/images && cp -f firmware.bin firmware.bin.$(BR2_OPENIPC_SOC_MODEL) && \
		md5sum firmware.bin.$(BR2_OPENIPC_SOC_MODEL) > firmware.bin.$(BR2_OPENIPC_SOC_MODEL).md5sum
	# Checksums first, as in REPACK_FIRMWARE.
	cd $(TARGET)/images && tar -czf openipc.$(BR2_OPENIPC_SOC_MODEL)-nor-$(BR2_OPENIPC_VARIANT)-split.tgz \
		firmware.bin.$(BR2_OPENIPC_SOC_MODEL).md5sum firmware.bin.$(BR2_OPENIPC_SOC_MODEL)
	rm -f $(TARGET)/images/*.md5sum
endef

# The FIT NAND package: three images, so not REPACK_FIRMWARE's two. Copies
# rather than renames -- rootfs.ubifs stays where buildroot left it, and the
# NOR package built from the same tree does not share any of these names.
NAND_FIT_IMAGES = fitImage rootfs.ubifs rootfs.ubi
define REPACK_NAND_FIT
	cd $(TARGET)/images && for f in $(NAND_FIT_IMAGES); do \
		cp -f $$f $$f.$(BR2_OPENIPC_SOC_MODEL) && \
		md5sum $$f.$(BR2_OPENIPC_SOC_MODEL) > $$f.$(BR2_OPENIPC_SOC_MODEL).md5sum || exit 1; done
	# Checksums first, as in REPACK_FIRMWARE.
	cd $(TARGET)/images && tar -czf openipc.$(BR2_OPENIPC_SOC_MODEL)-nand-$(BR2_OPENIPC_VARIANT).tgz \
		$(foreach f,$(NAND_FIT_IMAGES),$(f).$(BR2_OPENIPC_SOC_MODEL).md5sum) \
		$(foreach f,$(NAND_FIT_IMAGES),$(f).$(BR2_OPENIPC_SOC_MODEL))
	rm -f $(TARGET)/images/*.md5sum
endef
