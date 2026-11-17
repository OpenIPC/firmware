################################################################################
#
# ingenic-osdrv-t41
#
################################################################################

INGENIC_OSDRV_T41_VERSION =
INGENIC_OSDRV_T41_SITE =
INGENIC_OSDRV_T41_LICENSE = MIT
INGENIC_OSDRV_T41_LICENSE_FILES = LICENSE

# files/lib: ISVP-T41 1.2.0 (20230713) sdk/lib/uclibc, with DT_NEEDED
# dropped and stripped -- the treatment that reproduces the T40 libs byte
# for byte from their SDK. files/sensor/params: that SDK's
# resource/sensor_settings_t41, every sensor it tunes (sc301IoT renamed to
# sc301iot, the name its driver and yaml use).
ifeq ($(OPENIPC_SNS_MODEL),)
INGENIC_OSDRV_T41_SENSORS = $(notdir $(basename $(wildcard $(INGENIC_OSDRV_T41_PKGDIR)/files/sensor/*.yaml)))
else
INGENIC_OSDRV_T41_SENSORS = $(OPENIPC_SNS_MODEL)
endif

define INGENIC_OSDRV_T41_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(foreach s,$(INGENIC_OSDRV_T41_SENSORS),
		$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T41_PKGDIR)/files/sensor/$(s).yaml
		$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T41_PKGDIR)/files/sensor/params/$(s)-t41.bin)

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T41_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(INGENIC_OSDRV_T41_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
