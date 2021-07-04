################################################################################
#
# libsrt
#
################################################################################

LIBSRT_VERSION = 1.4.3
LIBSRT_SITE = $(call github,Haivision,srt,v$(LIBSRT_VERSION))
LIBSRT_LICENSE = MPLv2.0
LIBSRT_LICENSE_FILES = LICENSE


LIBSRT_CONF_OPTS += \
	--prefix=/usr \
	--disable-static \
	--disable-logging \
	--disable-encryption \
	--disable-apps


define LIBSRT_CONFIGURE_CMDS
	(cd $(@D); rm -rf config.cache; \
		$(TARGET_CONFIGURE_ARGS) \
		$(TARGET_CONFIGURE_OPTS) \
		./configure $(LIBSRT_CONF_OPTS))
endef


define LIBSRT_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBSRT_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libsrt.so.$(LIBSRT_VERSION) $(TARGET_DIR)/usr/lib/libsrt.so.$(LIBSRT_VERSION)
	ln -sf $(TARGET_DIR)/usr/lib/libsrt.so.$(LIBSRT_VERSION) $(TARGET_DIR)/usr/lib/libsrt.so
	ln -sf $(TARGET_DIR)/usr/lib/libsrt.so.$(LIBSRT_VERSION) $(TARGET_DIR)/usr/lib/libsrt.so.1.4
endef


$(eval $(generic-package))
