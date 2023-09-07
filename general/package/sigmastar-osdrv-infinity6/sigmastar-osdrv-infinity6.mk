################################################################################
#
# sigmastar-osdrv-infinity6
#
################################################################################

SIGMASTAR_OSDRV_INFINITY6_VERSION =
SIGMASTAR_OSDRV_INFINITY6_SITE =
SIGMASTAR_OSDRV_INFINITY6_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY6_LICENSE_FILES = LICENSE

SIGMASTAR_OSDRV_INFINITY6_DEPENDENCIES = ipctool

define SIGMASTAR_OSDRV_INFINITY6_BUILD_CMDS
	cp -rf $(SIGMASTAR_OSDRV_INFINITY6_PKGDIR)/files/src $(@D)
	$(TARGET_CC) $(@D)/src/config_tool.c -o $(@D)/config_tool -s \
		-I$(BUILD_DIR)/ipctool -L$(BUILD_DIR)/ipctool -lipchw
endef

define SIGMASTAR_OSDRV_INFINITY6_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/firmware $(SIGMASTAR_OSDRV_INFINITY6_PKGDIR)/files/sensor/firmware/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(SIGMASTAR_OSDRV_INFINITY6_PKGDIR)/files/sensor/configs/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(SIGMASTAR_OSDRV_INFINITY6_PKGDIR)/files/lib/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/config_tool
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY6_PKGDIR)/files/script/*
endef

$(eval $(generic-package))
