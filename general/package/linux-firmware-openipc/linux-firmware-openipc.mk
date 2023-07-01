################################################################################
#
# linux-firmware-openipc
#
################################################################################

LINUX_FIRMWARE_OPENIPC_SITE_METHOD = git
LINUX_FIRMWARE_OPENIPC_SITE = https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware
LINUX_FIRMWARE_OPENIPC_VERSION = $(call EXTERNAL_SHA,$(LINUX_FIRMWARE_OPENIPC_SITE),HEAD)

# WiFi RTL8188EU
ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_RTL_8188EU),y)
LINUX_FIRMWARE_OPENIPC_FILES += rtlwifi/rtl8188eufw.bin
LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES += LICENCE.rtlwifi_firmware.txt
endif

# ar9271
ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_ATHEROS_9271),y)
LINUX_FIRMWARE_OPENIPC_FILES += ath9k_htc/htc_9271-1.4.0.fw
LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES += LICENCE.atheros_firmware
endif

# MT7601
ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_MEDIATEK_MT7601U),y)
LINUX_FIRMWARE_OPENIPC_FILES += mediatek/mt7601u.bin
LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES += LICENCE.ralink_a_mediatek_company_firmware
endif

ifneq ($(LINUX_FIRMWARE_OPENIPC_FILES)$(LINUX_FIRMWARE_OPENIPC_DIRS),)

define LINUX_FIRMWARE_OPENIPC_BUILD_CMDS
	cd $(@D) && \
	$(TAR) cf br-firmware.tar $(sort $(LINUX_FIRMWARE_OPENIPC_FILES) $(LINUX_FIRMWARE_OPENIPC_DIRS))
endef

# Most firmware files are under a proprietary license, so no need to
# repeat it for every selections above. Those firmwares that have more
# lax licensing terms may still add them on a per-case basis.
LINUX_FIRMWARE_OPENIPC_LICENSE += Proprietary

# This file contains some licensing information about all the firmware
# files found in the linux-firmware package, so we always add it, even
# for firmwares that have their own licensing terms.
LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES += WHENCE

# Some license files may be listed more than once, so we have to remove
# duplicates
LINUX_FIRMWARE_OPENIPC_LICENSE_FILES = $(sort $(LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES))

# Some firmware are distributed as a symlink, for drivers to load them using a
# defined name other than the real one. Since 9cfefbd7fbda ("Remove duplicate
# symlinks") those symlink aren't distributed in linux-firmware but are created
# automatically by its copy-firmware.sh script during the installation, which
# parses the WHENCE file where symlinks are described. We follow the same logic
# here, adding symlink only for firmwares installed in the target directory.
#
# For testing the presence of firmwares in the target directory we first make
# sure we canonicalize the pointed-to file, to cover the symlinks of the form
# a/foo -> ../b/foo  where a/ (the directory where to put the symlink) does
# not yet exist.
define LINUX_FIRMWARE_OPENIPC_INSTALL_FW
	mkdir -p $(1)
	$(TAR) xf $(@D)/br-firmware.tar -C $(1)
	cd $(1) ; \
	sed -r -e '/^Link: (.+) -> (.+)$$/!d; s//\1 \2/' $(@D)/WHENCE | \
	while read f d; do \
		if test -f $$(readlink -m $$(dirname "$$f")/$$d); then \
			mkdir -p $$(dirname "$$f") || exit 1; \
			ln -sf $$d "$$f" || exit 1; \
		fi ; \
	done
endef

endif  # LINUX_FIRMWARE_OPENIPC_FILES || LINUX_FIRMWARE_OPENIPC_DIRS

define LINUX_FIRMWARE_OPENIPC_INSTALL_TARGET_CMDS
	$(call LINUX_FIRMWARE_OPENIPC_INSTALL_FW, $(TARGET_DIR)/lib/firmware)
endef

define LINUX_FIRMWARE_INSTALL_OPENIPC_IMAGES_CMDS
	$(call LINUX_FIRMWARE_OPENIPC_INSTALL_FW, $(BINARIES_DIR))
endef

$(eval $(generic-package))
