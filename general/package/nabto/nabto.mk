################################################################################
#
# nabto
#
################################################################################

NABTO_VERSION = 5.13.0
NABTO_SOURCE = nabto-embedded-sdk.zip
NABTO_SITE = https://github.com/nabto/nabto-embedded-sdk/releases/download/v$(NABTO_VERSION)

NABTO_DEPENDENCIES += \
	libevent-openipc \
	mbedtls-openipc

define NABTO_EXTRACT_CMDS
	$(UNZIP) -d $(@D) $(NABTO_DL_DIR)/$(NABTO_SOURCE)
	mv $(@D)/nabto-embedded-sdk/* $(@D)
endef

NABTO_CONF_OPTS += \
	-DCMAKE_BUILD_TYPE=Release \
	-DBUILD_SHARED_LIBS=OFF \
	-DDEVICE_BUILD_EXAMPLES=OFF

define NABTO_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/apps/tcp_tunnel_device/tcp_tunnel_device
endef

$(eval $(cmake-package))
