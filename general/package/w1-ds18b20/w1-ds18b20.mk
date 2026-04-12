################################################################################
#
# w1-ds18b20
#
################################################################################

W1_DS18B20_SITE_METHOD = local
W1_DS18B20_SITE = $(W1_DS18B20_PKGDIR)/src

W1_DS18B20_LICENSE = MIT
W1_DS18B20_LICENSE_FILES = LICENSE

define W1_DS18B20_BUILD_CMDS
	$(TARGET_CC) $(@D)/w1-ds18b20.c -o $(@D)/w1-ds18b20 -Os -s
endef

define W1_DS18B20_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -t $(TARGET_DIR)/usr/bin $(@D)/w1-ds18b20
endef

$(eval $(generic-package))
