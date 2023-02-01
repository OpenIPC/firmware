################################################################################
#
# nabto embedded sdk
#
################################################################################

NABTO_VERSION = 5.12.0
NABTO_SOURCE = nabto-embedded-sdk.zip
NABTO_SITE = https://github.com/nabto/nabto-embedded-sdk/releases/download/v$(NABTO_VERSION)

define NABTO_EXTRACT_CMDS
	$(UNZIP) -d $(@D) $(NABTO_DL_DIR)/$(NABTO_SOURCE)
	mv $(@D)/nabto-embedded-sdk/* $(@D)/
	rm -rf $(@D)/nabto-embedded-sdk
endef

NABTO_CONF_OPTS += \
	-DCMAKE_BUILD_TYPE=Release \
	-DBUILD_SHARED_LIBS=OFF \
	-DDEVICE_BUILD_TESTS=OFF \
	-DDEVICE_BUILD_EXAMPLES=OFF

define NABTO_REMOVE_EXTRA
	rm -f $(TARGET_DIR)/usr/lib/libnabto_device.so
endef

NABTO_POST_INSTALL_TARGET_HOOKS += NABTO_REMOVE_EXTRA

$(eval $(cmake-package))
