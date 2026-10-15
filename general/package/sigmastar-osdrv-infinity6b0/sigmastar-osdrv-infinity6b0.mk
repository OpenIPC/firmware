################################################################################
#
# sigmastar-osdrv-infinity6b0
#
################################################################################

SIGMASTAR_OSDRV_INFINITY6B0_VERSION =
SIGMASTAR_OSDRV_INFINITY6B0_SITE =
SIGMASTAR_OSDRV_INFINITY6B0_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY6B0_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_INFINITY6B0_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/firmware $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/sensor/firmware/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/sensor/configs/$(if $(OPENIPC_SNS_MODEL),$(OPENIPC_SNS_MODEL),*).bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/script/*
endef

define SIGMASTAR_OSDRV_INFINITY6B0_LIBRARIES
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/lib/*
endef

# majestic links none of these, so its images skip them; another streamer
# needs them. Installed at target-finalize rather than in this package's
# install step, because finalize re-runs on every build: turning majestic off
# in an existing build reinstalls nothing here, and an install-step gate left
# that image with no SDK libraries at all (#2607 fixed the same kind of
# gate in goke-osdrv-gk7205v500).
ifneq ($(BR2_PACKAGE_MAJESTIC),y)
SIGMASTAR_OSDRV_INFINITY6B0_TARGET_FINALIZE_HOOKS += SIGMASTAR_OSDRV_INFINITY6B0_LIBRARIES
endif

$(eval $(generic-package))
