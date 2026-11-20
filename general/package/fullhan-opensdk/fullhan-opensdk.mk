################################################################################
#
# fullhan-opensdk
#
################################################################################

FULLHAN_OPENSDK_SITE = $(call github,openipc,openfullhan,$(FULLHAN_OPENSDK_VERSION))
FULLHAN_OPENSDK_VERSION = 8f2079d88ffd60a19c176a7ad1f13ad7477f4a1b

FULLHAN_OPENSDK_LICENSE = GPL-3.0
FULLHAN_OPENSDK_LICENSE_FILES = LICENSE

# After the vendor package, so a library built here wins over a prebuilt one
# of the same name.
FULLHAN_OPENSDK_DEPENDENCIES = fullhan-osdrv-fh8852v100

# Sensors built for each family, by directory under libraries/sensor/.
FULLHAN_OPENSDK_SENSORS_fh8852v100 = galaxycore_gc4653
FULLHAN_OPENSDK_SENSORS = $(FULLHAN_OPENSDK_SENSORS_$(OPENIPC_SOC_FAMILY))
FULLHAN_OPENSDK_SENSOR_DIR = $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)

# CFLAGS through the environment: on the command line it would replace the
# sensor Makefile's own += flags.
define FULLHAN_OPENSDK_BUILD_CMDS
	$(foreach s,$(FULLHAN_OPENSDK_SENSORS),
		$(TARGET_MAKE_ENV) CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" \
			$(MAKE) -C $(FULLHAN_OPENSDK_SENSOR_DIR)/$(s) CC="$(TARGET_CC)"
	)
endef

# majestic loads /usr/lib/sensors/lib<name>.so and reads its tuning from
# /etc/sensors/<name>_attr.hex.
define FULLHAN_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors $(TARGET_DIR)/etc/sensors
	$(foreach s,$(FULLHAN_OPENSDK_SENSORS),
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(FULLHAN_OPENSDK_SENSOR_DIR)/$(s)/*.so
		$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(FULLHAN_OPENSDK_SENSOR_DIR)/$(s)/*_attr.hex
	)
endef

$(eval $(generic-package))
