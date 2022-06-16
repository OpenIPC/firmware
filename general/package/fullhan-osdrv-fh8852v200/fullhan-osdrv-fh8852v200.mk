################################################################################
#
# fullhan-osdrv-fh8852v200
#
################################################################################

FULLHAN_OSDRV_FH8852V200_VERSION =
FULLHAN_OSDRV_FH8852V200_SITE =
FULLHAN_OSDRV_FH8852V200_LICENSE = MIT
FULLHAN_OSDRV_FH8852V200_LICENSE_FILES = LICENSE

define FULLHAN_OSDRV_FH8852V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_FULLHAN_PATH)/package/fullhan-osdrv-fh8852v200/files/script/S95fullhan

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(BR2_EXTERNAL_FULLHAN_PATH)/package/fullhan-osdrv-fh8852v200/files/firmware/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.129/fullhan
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.129/fullhan $(BR2_EXTERNAL_FULLHAN_PATH)/package/fullhan-osdrv-fh8852v200/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_FULLHAN_PATH)/package/fullhan-osdrv-fh8852v200/files/script/load_fullhan

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_FULLHAN_PATH)/package/fullhan-osdrv-fh8852v200/files/lib/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_FULLHAN_PATH)/package/fullhan-osdrv-fh8852v200/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_FULLHAN_PATH)/package/fullhan-osdrv-fh8852v200/files/sensor/params/*
endef

$(eval $(generic-package))
