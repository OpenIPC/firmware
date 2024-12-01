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

define NABTO_REMOVE_EXTRA
	rm -f $(TARGET_DIR)/usr/lib/libnabto_device.so
endef

NABTO_POST_INSTALL_TARGET_HOOKS += NABTO_REMOVE_EXTRA

$(eval $(cmake-package))
