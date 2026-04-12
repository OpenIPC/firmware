################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# mqtt-bot — lightweight MQTT command executor
#
################################################################################

MQTT_BOT_LICENSE       = MIT
MQTT_BOT_LICENSE_FILES = README.md
MQTT_BOT_DEPENDENCIES  = mosquitto mbedtls-openipc

define MQTT_BOT_EXTRACT_CMDS
	cp -avr $(MQTT_BOT_PKGDIR)/src/* $(@D)/
endef

define MQTT_BOT_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall -Wextra -std=c99 -pedantic -D_POSIX_C_SOURCE=200809L" \
		LDFLAGS="$(TARGET_LDFLAGS) -lmosquitto" \
		-C $(@D)
endef

define MQTT_BOT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	install -m 0755 -D $(@D)/mqtt-bot $(TARGET_DIR)/usr/sbin/mqtt-bot
endef

$(eval $(generic-package))
