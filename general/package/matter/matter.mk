################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# matter — Matter 1.2-compatible IP camera server
#
################################################################################

MATTER_LICENSE       = MIT
MATTER_LICENSE_FILES = README.md

MATTER_RTSP_URL    = $(call qstrip,$(BR2_PACKAGE_MATTER_RTSP_URL))
MATTER_PORT        = $(call qstrip,$(BR2_PACKAGE_MATTER_PORT))
MATTER_STARTLEVEL  = $(call qstrip,$(BR2_PACKAGE_MATTER_INIT_STARTLEVEL))

define MATTER_EXTRACT_CMDS
	cp -avr $(MATTER_PKGDIR)/src/* $(@D)/
endef

define MATTER_BUILD_CMDS
	$(MAKE) \
		CC="$(TARGET_CC)" \
		STRIP="$(TARGET_STRIP)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) \
			-pthread \
			-Wl,--gc-sections \
			-Wl,--strip-all" \
		ARCH_FLAGS="" \
		-C $(@D)
endef

define MATTER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 $(@D)/matter-server \
		$(TARGET_DIR)/usr/sbin/matter-server

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 $(MATTER_PKGDIR)/files/S90matter \
		$(TARGET_DIR)/etc/init.d/S$(MATTER_STARTLEVEL)matter

	$(INSTALL) -m 644 -D $(MATTER_PKGDIR)/files/matter.conf \
		$(TARGET_DIR)/etc/matter.conf
	$(SED) 's|rtsp://127.0.0.1:554/stream|$(MATTER_RTSP_URL)|' \
		$(TARGET_DIR)/etc/matter.conf
	$(SED) 's|^MATTER_PORT=5540|MATTER_PORT=$(MATTER_PORT)|' \
		$(TARGET_DIR)/etc/matter.conf
endef

$(eval $(generic-package))
