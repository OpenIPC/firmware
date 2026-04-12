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
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(FULLHAN_OSDRV_FH8852V200_PKGDIR)/files/firmware/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.129/fullhan
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.129/fullhan $(FULLHAN_OSDRV_FH8852V200_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(FULLHAN_OSDRV_FH8852V200_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(FULLHAN_OSDRV_FH8852V200_PKGDIR)/files/lib/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(FULLHAN_OSDRV_FH8852V200_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(FULLHAN_OSDRV_FH8852V200_PKGDIR)/files/sensor/params/*
endef

$(eval $(generic-package))
