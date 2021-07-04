################################################################################
#
# libhv
#
################################################################################

LIBHV_VERSION = 1.1.1
LIBHV_SITE = $(call github,ithewei,libhv,v$(LIBHV_VERSION))
LIBHV_LICENSE = BSDv3-Clause
LIBHV_LICENSE_FILES = LICENSE


LIBHV_CONF_OPTS += \
	--prefix=/usr \
	--without-http-server \
	--without-http-client \
	--without-evpp \
	--disable-ipv6 \
	--disable-windump


define LIBHV_CONFIGURE_CMDS
	(cd $(@D); rm -rf config.cache; \
		$(TARGET_CONFIGURE_ARGS) \
		$(TARGET_CONFIGURE_OPTS) \
		./configure $(LIBHV_CONF_OPTS))
endef


define LIBHV_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBHV_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/lib/libhv.so $(TARGET_DIR)/usr/lib/libhv.so
endef


$(eval $(generic-package))
