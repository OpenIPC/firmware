################################################################################
#
# libhv-openipc
#
################################################################################

LIBHV_OPENIPC_VERSION = 1.1.1
LIBHV_OPENIPC_SITE = $(call github,ithewei,libhv,v$(LIBHV_OPENIPC_VERSION))

LIBHV_OPENIPC_LICENSE = BSDv3-Clause
LIBHV_OPENIPC_LICENSE_FILES = LICENSE

LIBHV_OPENIPC_CONF_OPTS += \
	--prefix=/usr \
	--without-http-server \
	--without-http-client \
	--without-evpp \
	--disable-ipv6 \
	--disable-windump

LIBHV_OPENIPC_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	CXX="$(TARGET_CXX)"
#	ARCH=$(KERNEL_ARCH) \
#	CROSS_COMPILE="$(TARGET_CROSS)" \
#	CFLAGS="$(TARGET_CFLAGS)" \
#	LDFLAGS="$(TARGET_LDFLAGS)" \
#	LD="$(TARGET_CC)" \
#	STRIP=$(TARGET_STRIP) \
#	PREFIX="$(TARGET_DIR)" \

define LIBHV_OPENIPC_CONFIGURE_CMDS
	(cd $(@D); rm -rf config.cache; \
		$(TARGET_CONFIGURE_ARGS) \
		$(TARGET_CONFIGURE_OPTS) \
		./configure $(LIBHV_OPENIPC_CONF_OPTS))
endef

define LIBHV_OPENIPC_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(LIBHV_OPENIPC_MAKE_OPTS) -C $(@D)
endef

define LIBHV_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/lib/libhv.so $(TARGET_DIR)/usr/lib/libhv.so
endef

$(eval $(generic-package))
