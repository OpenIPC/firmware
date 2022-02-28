################################################################################
#
# linux-firmware-openipc
#
################################################################################

LINUX_FIRMWARE_OPENIPC_VERSION = 20190717
LINUX_FIRMWARE_OPENIPC_SOURCE = linux-firmware-$(LINUX_FIRMWARE_OPENIPC_VERSION).tar.gz
LINUX_FIRMWARE_OPENIPC_SITE = https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git/snapshot
#LINUX_FIRMWARE_OPENIPC_SITE = http://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
#LINUX_FIRMWARE_OPENIPC_SITE_METHOD = git

# WiFi RTL8188EU
ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_RTL8188EU),y)
LINUX_FIRMWARE_OPENIPC_FILES += rtlwifi/rtl8188eufw.bin
LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES += LICENCE.rtlwifi_firmware.txt
endif

# WiFi MT7601U
ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_MT7601U),y)
LINUX_FIRMWARE_OPENIPC_FILES += mt7601u.bin
LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES += LICENCE.ralink_a_mediatek_company_firmware
endif

# WiFi ATH9K_HTC
ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_ATHEROS_9271),y)
LINUX_FIRMWARE_OPENIPC_FILES += ath9k_htc/htc_9271-1.4.0.fw
LINUX_FIRMWARE_OPENIPC_ALL_LICENSE_FILES += LICENCE.open-ath9k-htc-firmware
endif

ifneq ($(LINUX_FIRMWARE_OPENIPC_FILES),)
define LINUX_FIRMWARE_OPENIPC_INSTALL_FILES
	cd $(@D) && \
		$(TAR) cf install.tar $(sort $(LINUX_FIRMWARE_OPENIPC_FILES)) && \
		$(TAR) xf install.tar -C $(TARGET_DIR)/lib/firmware
endef
endif

ifneq ($(LINUX_FIRMWARE_OPENIPC_DIRS),)
# We need to rm-rf the destination directory to avoid copying
# into it in itself, should we re-install the package.
define LINUX_FIRMWARE_OPENIPC_INSTALL_DIRS
	$(foreach d,$(LINUX_FIRMWARE_OPENIPC_DIRS), \
		rm -rf $(TARGET_DIR)/lib/firmware/$(d); \
		mkdir -p $(dir $(TARGET_DIR)/lib/firmware/$(d)); \
		cp -a $(@D)/$(d) $(TARGET_DIR)/lib/firmware/$(d)$(sep))
endef
endif

ifneq ($(LINUX_FIRMWARE_OPENIPC_FILES)$(LINUX_FIRMWARE_OPENIPC_DIRS),)

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

endif

define LINUX_FIRMWARE_OPENIPC_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/lib/firmware
	$(LINUX_FIRMWARE_OPENIPC_INSTALL_FILES)
	$(LINUX_FIRMWARE_OPENIPC_INSTALL_DIRS)
endef

$(eval $(generic-package))
