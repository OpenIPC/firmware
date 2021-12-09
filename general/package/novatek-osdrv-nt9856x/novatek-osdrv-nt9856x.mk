################################################################################
#
# novatek-osdrv-nt9856x
#
################################################################################

NOVATEK_OSDRV_NT9856X_VERSION =
NOVATEK_OSDRV_NT9856X_SITE =
NOVATEK_OSDRV_NT9856X_LICENSE = MIT
NOVATEK_OSDRV_NT9856X_LICENSE_FILES = LICENSE


define NOVATEK_OSDRV_NT9856X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/script/S99novatek

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/*.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/mhal.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/script/load*
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/script/ircut_demo

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# Minimalism for sample_venc_jpeg
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/lib/libmi_sys.so
	# Maximum for demo software
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/lib/libmi_common.so
endef

$(eval $(generic-package))
