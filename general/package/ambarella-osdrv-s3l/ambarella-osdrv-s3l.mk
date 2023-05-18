################################################################################
#
# ambarella-osdrv-s3l
#
################################################################################

AMBARELLA_OSDRV_S3L_VERSION =
AMBARELLA_OSDRV_S3L_SITE =
AMBARELLA_OSDRV_S3L_LICENSE = MIT
AMBARELLA_OSDRV_S3L_LICENSE_FILES = LICENSE


define AMBARELLA_OSDRV_S3L_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/sensor/config/*.cfg

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.104/ambarella
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.104/ambarella $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/lib/xxx.so
endef

$(eval $(generic-package))
