################################################################################
#
# THERMAL_THROTTLE_DB
#
################################################################################

THERMAL_THROTTLE_DB_SITE = $(call github,danielbanar,openipc-thermal,$(THERMAL_THROTTLE_DB_VERSION))
THERMAL_THROTTLE_DB_VERSION = HEAD

define THERMAL_THROTTLE_DB_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/temp_profile.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(@D)/S96thermal

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/thermal-throttle
endef

$(eval $(generic-package))