################################################################################
#
# i2c-telemetry | updated 2022.08.10
#
################################################################################

I2C_TELEMETRY_LICENSE = MIT
I2C_TELEMETRY_LICENSE_FILES = LICENSE

define I2C_TELEMETRY_EXTRACT_CMDS
	cp -av $(I2C_TELEMETRY_PKGDIR)/src/* $(@D)/
endef

define I2C_TELEMETRY_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -s ina219.c -o ina219)
endef

define I2C_TELEMETRY_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/ina219 $(TARGET_DIR)/usr/sbin/ina219
endef

$(eval $(generic-package))
