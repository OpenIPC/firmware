################################################################################
#
# sigmastar-osdrv-infinity2m
#
################################################################################

SIGMASTAR_OSDRV_INFINITY2M_VERSION =
SIGMASTAR_OSDRV_INFINITY2M_SITE =
SIGMASTAR_OSDRV_INFINITY2M_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY2M_LICENSE_FILES = LICENSE

# The i2m NVR SDK (2020-03): MHAL and the MI modules for decoding to HDMI
# and VGA, the framebuffer, and /config, where the modules look for the
# panel timings, PQ tables, memory map and decoder firmware.
define SIGMASTAR_OSDRV_INFINITY2M_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/config
	cp -a $(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/config/. $(TARGET_DIR)/config/
	ln -sf config_tool $(TARGET_DIR)/config/dump_config
	ln -sf config_tool $(TARGET_DIR)/config/dump_mmap

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/script/*

	# the JPEG a user's U-Boot environment can load and show at power-on
	# instead of U-Boot's own drawn logo
	$(INSTALL) -m 644 -D $(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/logo/bootlogo.jpg \
		$(TARGET_DIR)/usr/share/bootlogo.jpg

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/lib/*
endef

$(eval $(generic-package))
