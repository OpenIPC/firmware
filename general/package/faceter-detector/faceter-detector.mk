################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# faceter-detector  2025.12.10 — faceter-detector  2025.12.10 Buildroot package
#
################################################################################

FACETER_DETECTOR_SITE = https://github.com/OpenIPC/faceter/releases/download/faceter-detector
FACETER_DETECTOR_SOURCE = faceter-detector.$(FACETER_DETECTOR_FAMILY).tar.bz2
FACETER_DETECTOR_LICENSE = The Prosperity Public License 3.0.0
FACETER_DETECTOR_LICENSE_FILES = LICENSE

FACETER_DETECTOR_FAMILY = $(OPENIPC_SOC_FAMILY)

FACETER_DETECTOR_DEPENDENCIES += \
	majestic

define FACETER_DETECTOR_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/faceter-detector
endef

$(eval $(generic-package))
