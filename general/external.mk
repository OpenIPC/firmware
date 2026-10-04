export OPENIPC_SOC_VENDOR := $(call qstrip,$(BR2_OPENIPC_SOC_VENDOR))
export OPENIPC_SOC_MODEL := $(call qstrip,$(BR2_OPENIPC_SOC_MODEL))
export OPENIPC_SOC_ALIASES := $(call qstrip,$(BR2_OPENIPC_SOC_ALIASES))
export OPENIPC_SOC_FAMILY := $(call qstrip,$(BR2_OPENIPC_SOC_FAMILY))
export OPENIPC_SNS_MODEL := $(call qstrip,$(BR2_OPENIPC_SNS_MODEL))
export OPENIPC_VARIANT := $(call qstrip,$(BR2_OPENIPC_VARIANT))
export OPENIPC_MAJESTIC := $(call qstrip,$(BR2_OPENIPC_MAJESTIC))
export WGET := wget --show-progress --passive-ftp -nd -t5 -T10

EXTERNAL_VENDOR := $(BR2_EXTERNAL)/../br-ext-chip-$(OPENIPC_SOC_VENDOR)
OPENIPC_KERNEL := $(OPENIPC_SOC_VENDOR)-$(OPENIPC_SOC_FAMILY)
OPENIPC_TOOLCHAIN := toolchain/toolchain.$(OPENIPC_KERNEL)

# Buildroot leaves upstream's wpa_supplicant defconfig defaults for TDLS and
# 802.11r in place and offers no Kconfig switch for either, so every board that
# enables wpa_supplicant carries both. Neither can fire on a camera: TDLS sets
# up direct station-to-station data links, and a camera only ever talks to its
# AP; 802.11r is fast BSS transition, and a fixed-mount camera does not roam
# between APs. Measured on gk7205v300_lite -- 28,860 and 21,940 bytes of the
# unstripped binary, 44,976 off the stripped one.
#
# Appending to the package's own list rather than patching its defconfig works
# because Buildroot includes this file (Makefile:545) after package/*/*.mk
# (Makefile:531), and expands WPA_SUPPLICANT_CONFIGURE_CMDS when the rule runs
# rather than at parse time. That leaves nothing to rebase when the package is
# bumped. If a future Buildroot ever reorders those includes this stops taking
# effect silently, and the symptom is the wpa_supplicant binary going back up
# by ~45KB in the size report rather than anything failing.
WPA_SUPPLICANT_CONFIG_DISABLE += CONFIG_TDLS CONFIG_IEEE80211R

# NAND boards with a FIT (board/<family>/nand-fit.its) carry their kernel inside
# the UBIFS rootfs rather than in a volume of its own: no flash is set aside
# for a kernel, and the rootfs volume is as big as its image. Exactly one
# kernel goes in, never two: /boot/fitImage (zImage + DTB, hashed) when the
# board builds a FIT, which these do, otherwise /boot/uImage (the NOR kernel,
# DTB appended). u-boot-xmedia boots either, trying the FIT first, so a board
# without a FIT -- or a U-Boot with UBIFS but no FIT, given a uImage build --
# needs nothing else.
#
# Copied into the UBIFS image's own copy of the target tree only, so the
# squashfs the NOR package ships stays as it was. The kernel exists by then:
# the uImage from the kernel build, the FIT from rootfs_script.sh, which runs
# before any filesystem is generated. Same include-order dependency as above:
# the hook list is expanded when the rule runs.
ifneq ($(wildcard $(EXTERNAL_VENDOR)/board/$(OPENIPC_SOC_FAMILY)/nand-fit.its),)
define OPENIPC_UBIFS_BOOT
	mkdir -p $(TARGET_DIR)/boot
	if [ -f $(BINARIES_DIR)/fitImage ]; then \
		cp $(BINARIES_DIR)/fitImage $(TARGET_DIR)/boot/; \
	else \
		cp $(BINARIES_DIR)/uImage $(TARGET_DIR)/boot/; \
	fi
endef
ROOTFS_UBIFS_PRE_GEN_HOOKS += OPENIPC_UBIFS_BOOT
endif

# linux.mk passes INSTALL_MOD_STRIP=1, which the kernel turns into
# `strip --strip-debug`: every module still ships its full .symtab/.strtab.
# Any other value is handed to strip as its options, and --strip-unneeded keeps
# exactly the symbols relocation and __ksymtab need. The last assignment on the
# make command line wins, and pkg-kernel-module.mk installs with the same
# LINUX_MAKE_FLAGS, so out-of-tree modules (wireguard, the Wi-Fi drivers) are
# covered too. Same include-order dependency as the line above. The cost is
# that an oops inside a module prints offsets instead of its static function
# names. On hi3516ev300 this took the 26 in-tree and wireguard modules from
# 1827288 to 1624584 B, and all 26 loaded and worked on the camera.
LINUX_MAKE_FLAGS += INSTALL_MOD_STRIP=--strip-unneeded

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
include $(sort $(wildcard $(BR2_EXTERNAL)/package/legacy/*/*.mk))
