################################################################################
#
# sigmastar-osdrv-infinity2m
#
################################################################################

SIGMASTAR_OSDRV_INFINITY2M_VERSION =
SIGMASTAR_OSDRV_INFINITY2M_SITE =
# The SDK's libraries and decoder firmware are SigmaStar's, redistributed as
# in the other sigmastar-osdrv packages; config_tool, load_sigmastar and the
# logo are OpenIPC's.
SIGMASTAR_OSDRV_INFINITY2M_LICENSE = PROPRIETARY (SigmaStar SDK libraries, decoder firmware), MIT (config_tool, load_sigmastar)

# The i2m NVR SDK (2020-03), its userspace side: the MI libraries and the
# decoder firmware. The modules themselves are built for this kernel by
# sigmastar-opensdk (OpenIPC/opensigmastar).
#
# config_tool is OpenIPC's: mi_sys runs it from its default_config_path
# (/usr/lib/sigmastar, set by load_sigmastar) to load the memory map, panel
# timings and PQ defaults, which it carries itself (src/config_tool.c).
define SIGMASTAR_OSDRV_INFINITY2M_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		$(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/src/config_tool.c -o $(@D)/config_tool
endef

define SIGMASTAR_OSDRV_INFINITY2M_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -D $(@D)/config_tool $(TARGET_DIR)/usr/lib/sigmastar/config_tool

	# open_fbdev's screen (default_config_path_file, set by load_sigmastar)
	$(INSTALL) -m 644 -D $(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/fbdev.ini \
		$(TARGET_DIR)/usr/share/sigmastar/fbdev.ini

	# open_mi_vdec's firmware, by the paths opensigmastar wrote into it
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware \
		$(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/firmware/chagall-*.bin

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
