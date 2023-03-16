################################################################################
#
# sigmastar-osdrv-infinity6e
#
################################################################################

SIGMASTAR_OSDRV_INFINITY6E_VERSION =
SIGMASTAR_OSDRV_INFINITY6E_SITE =
SIGMASTAR_OSDRV_INFINITY6E_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY6E_LICENSE_FILES = LICENSE

SIGMASTAR_OSDRV_INFINITY6E_PATH = $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-infinity6e/files
SIGMASTAR_OSDRV_INFINITY6E_SENSOR = $(call qstrip,$(BR2_PACKAGE_SIGMASTAR_OSDRV_SENSOR))

define SIGMASTAR_OSDRV_INFINITY6E_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY6E_PATH)/script/bin/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(SIGMASTAR_OSDRV_INFINITY6E_PATH)/script/init.d/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(SIGMASTAR_OSDRV_INFINITY6E_PATH)/lib/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6E_PATH)/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6E_PATH)/sensor/$(SIGMASTAR_OSDRV_INFINITY6E_SENSOR)/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_INFINITY6E_PATH)/sensor/$(SIGMASTAR_OSDRV_INFINITY6E_SENSOR)/$(SIGMASTAR_OSDRV_INFINITY6E_SENSOR)_iqfile.bin
	ln -sf $(SIGMASTAR_OSDRV_INFINITY6E_SENSOR)_iqfile.bin $(TARGET_DIR)/etc/sensors/iqfile/iqfile0.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/venc_fw
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/venc_fw $(SIGMASTAR_OSDRV_INFINITY6E_PATH)/sensor/venc_fw/chagall.bin

	# Set sensor for insmod
	sed -i "s/sensor=/sensor=$(SIGMASTAR_OSDRV_INFINITY6E_SENSOR)/g" $(TARGET_DIR)/usr/bin/load_sigmastar
endef

$(eval $(generic-package))
