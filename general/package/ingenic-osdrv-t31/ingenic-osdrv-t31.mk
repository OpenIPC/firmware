################################################################################
#
# ingenic-osdrv-t31
#
################################################################################

INGENIC_OSDRV_T31_VERSION =
INGENIC_OSDRV_T31_SITE =
INGENIC_OSDRV_T31_LICENSE = MIT
INGENIC_OSDRV_T31_LICENSE_FILES = LICENSE

define INGENIC_OSDRV_T31_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/*.yaml
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/*.bin

	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/gc2053-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/gc2083-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/gc4653-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/imx307-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/jxf37-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/jxh62-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/jxq03-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/jxq03p-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/sc200ai-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/sc2232h-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/sc2335-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/sc3338-t31.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/sc5235-t31.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/audio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/avpu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/gpio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sinfo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/tx-isp-t31.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_gc2053_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_gc2083_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_gc4653_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_imx307_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_jxf37_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_jxh62_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_jxq03_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_jxq03p_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_sc200ai_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_sc2232h_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_sc2335_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_sc3338_t31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/ingenic $(INGENIC_OSDRV_T31_PKGDIR)/files/kmod/sensor_sc5235_t31.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T31_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T31_PKGDIR)/files/script/ircut_demo

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors/params
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors/params $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/*.bin

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors/params/WDR
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors/params/WDR $(INGENIC_OSDRV_T31_PKGDIR)/files/sensor/params/WDR/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(INGENIC_OSDRV_T31_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
