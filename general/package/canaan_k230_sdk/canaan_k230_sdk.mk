################################################################################
#
# canaan k230 sdk
#
################################################################################
CANAAN_K230_SDK_SITE =
CANAAN_K230_SDK_SOURCE =

k230_download_url := $(shell \
	if curl --output /dev/null --silent --head --fail https://ai.b-bug.org/k230/downloads/dl/openipc ;then  \
	echo "https://ai.b-bug.org/k230/downloads/dl/openipc"; \
	else \
	echo "https://kendryte-download.canaan-creative.com/k230/dl/openipc";\
	fi ; )


CANAAN_K230_SDK_EXTRA_DOWNLOADS := $(k230_download_url)/$(call qstrip ,$(BR2_PACKAGE_CANAAN_K230_SDK_CONF))/sysimage-sdcard.img.gz
CANAAN_K230_SDK_EXTRA_DOWNLOADS += $(k230_download_url)/$(call qstrip ,$(BR2_PACKAGE_CANAAN_K230_SDK_CONF))/rootfs.tar.xz

define CANAAN_K230_SDK_INSTALL_TARGET_CMDS
	tar -xf $(CANAAN_K230_SDK_DL_DIR)/rootfs.tar.xz -C $(TARGET_DIR)
	rm -rf $(TARGET_DIR)/var/run;
	rm -rf $(TARGET_DIR)/etc/init.d/S30dbus;
	rm -rf $(TARGET_DIR)/etc/init.d/S40bluetoothd;
	rm -rf $(TARGET_DIR)/etc/init.d/S48sntp;
	rm -rf $(TARGET_DIR)/etc/init.d/S49ntp;
	rm -rf $(TARGET_DIR)/etc/init.d/S50crond;
	rm -rf $(TARGET_DIR)/etc/init.d/S50sshd;
	rm -rf $(TARGET_DIR)/etc/init.d/S50telnet;
	gunzip -f -k  -c $(CANAAN_K230_SDK_DL_DIR)/sysimage-sdcard.img.gz > $(BINARIES_DIR)/sysimage-sdcard.img
endef

$(eval $(generic-package))
