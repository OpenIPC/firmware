################################################################################
#
# libsrt-openipc
#
################################################################################

LIBSRT_OPENIPC_VERSION = 1.4.4
LIBSRT_OPENIPC_SITE = $(call github,Haivision,srt,v$(LIBSRT_OPENIPC_VERSION))

LIBSRT_OPENIPC_LICENSE = MPLv2.0
LIBSRT_OPENIPC_LICENSE_FILES = LICENSE

LIBSRT_OPENIPC_CONF_OPTS += \
	--prefix=/usr \
	--disable-static \
	--disable-logging \
	--disable-encryption \
	--disable-apps

define LIBSRT_OPENIPC_CONFIGURE_CMDS
	(cd $(@D); rm -rf config.cache; \
		$(TARGET_CONFIGURE_ARGS) \
		$(TARGET_CONFIGURE_OPTS) \
		./configure $(LIBSRT_OPENIPC_CONF_OPTS))
endef

define LIBSRT_OPENIPC_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBSRT_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libsrt.so.$(LIBSRT_OPENIPC_VERSION) $(TARGET_DIR)/usr/lib/libsrt.so.$(LIBSRT_OPENIPC_VERSION)
	ln -sf libsrt.so.$(LIBSRT_OPENIPC_VERSION) $(TARGET_DIR)/usr/lib/libsrt.so
	ln -sf libsrt.so.$(LIBSRT_OPENIPC_VERSION) $(TARGET_DIR)/usr/lib/libsrt.so.1.4
endef

$(eval $(generic-package))
