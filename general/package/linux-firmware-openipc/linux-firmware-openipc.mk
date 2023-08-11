################################################################################
#
# linux-firmware-openipc
#
################################################################################

ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_ATHEROS_9271),y)
define LINUX_FIRMWARE_OPENIPC_INSTALL_ATHEROS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/ath9k_htc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware/ath9k_htc $(LINUX_FIRMWARE_OPENIPC_PKGDIR)/files/ath9k_htc/htc_9271-1.4.0.fw
endef
endif

ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_MEDIATEK_MT7601U),y)
define LINUX_FIRMWARE_OPENIPC_INSTALL_MEDIATEK
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/mediatek
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware/mediatek $(LINUX_FIRMWARE_OPENIPC_PKGDIR)/files/mediatek/mt7601u.bin
	ln -s mediatek/mt7601u.bin $(TARGET_DIR)/lib/firmware/mt7601u.bin
endef
endif

ifeq ($(BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_RTL_8188EU),y)
define LINUX_FIRMWARE_OPENIPC_INSTALL_REALTEK
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/rtlwifi
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware/rtlwifi $(LINUX_FIRMWARE_OPENIPC_PKGDIR)/files/rtlwifi/rtl8188eufw.bin
endef
endif

define LINUX_FIRMWARE_OPENIPC_INSTALL_TARGET_CMDS
	$(LINUX_FIRMWARE_OPENIPC_INSTALL_ATHEROS)
	$(LINUX_FIRMWARE_OPENIPC_INSTALL_MEDIATEK)
	$(LINUX_FIRMWARE_OPENIPC_INSTALL_REALTEK)
endef

$(eval $(generic-package))
