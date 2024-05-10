################################################################################
#
# sigmastar-osdrv-sensors
#
################################################################################

SIGMASTAR_OSDRV_SENSORS_SITE = $(call github,openipc,sensors,$(SIGMASTAR_OSDRV_SENSORS_VERSION))
SIGMASTAR_OSDRV_SENSORS_VERSION = HEAD

define SIGMASTAR_OSDRV_SENSORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar $(@D)/sigmastar/$(OPENIPC_SOC_FAMILY)/*
endef

$(eval $(generic-package))
