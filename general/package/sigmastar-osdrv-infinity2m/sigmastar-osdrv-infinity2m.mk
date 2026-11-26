################################################################################
#
# sigmastar-osdrv-infinity2m
#
################################################################################

SIGMASTAR_OSDRV_INFINITY2M_VERSION =
SIGMASTAR_OSDRV_INFINITY2M_SITE =
# The SDK's libraries, /config files and config_tool are SigmaStar's,
# redistributed as in the other sigmastar-osdrv packages; load_sigmastar
# and the logo are OpenIPC's.
SIGMASTAR_OSDRV_INFINITY2M_LICENSE = PROPRIETARY (SigmaStar SDK libraries, /config, config_tool), MIT (load_sigmastar)

# The i2m NVR SDK (2020-03), its userspace side: the MI libraries, and
# /config, where the media modules look for the panel timings, PQ tables,
# memory map and decoder firmware. The modules themselves are built for this
# kernel by sigmastar-opensdk (OpenIPC/opensigmastar).
define SIGMASTAR_OSDRV_INFINITY2M_INSTALL_TARGET_CMDS
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
