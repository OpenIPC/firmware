################################################################################
#
# ingenic-osdrv-t21
#
################################################################################

INGENIC_OSDRV_T21_VERSION =
INGENIC_OSDRV_T21_SITE =
INGENIC_OSDRV_T21_LICENSE = MIT
INGENIC_OSDRV_T21_LICENSE_FILES = LICENSE

define INGENIC_OSDRV_T21_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/*.yaml
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/*.bin

	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/jxf23-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/jxf37-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/ov2735b-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/sc2232-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/sc2300-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/sc2335-t21.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/audio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/audioout.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/gpio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/ircut.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/motor.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/reset.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/rled.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sinfo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/wifien.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/tx-isp-t21l.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/tx-isp-t21n.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/tx-isp-t21z.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sensor_jxf23_t21n.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sensor_jxf37_t21l.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sensor_ov2735b_t21z.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sensor_sc2232_t21n.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sensor_sc2300_t21z.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sensor_sc2335_t21n.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic $(INGENIC_OSDRV_T21_PKGDIR)/files/kmod/sensor_sc2335_t21z.ko
	# ln -sf sensor_sc2335_t21z.ko $(TARGET_DIR)/lib/modules/3.10.14__isvp_turkey_1.0__/ingenic/sensor_sc2335_t21f.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T21_PKGDIR)/files/script/load*
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T21_PKGDIR)/files/script/ircut_demo

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors/params
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors/params $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/*.bin

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors/params/WDR
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors/params/WDR $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/WDR/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(INGENIC_OSDRV_T21_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
