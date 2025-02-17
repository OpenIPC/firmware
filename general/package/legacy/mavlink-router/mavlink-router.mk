################################################################################
#
# mavlink-router
#
################################################################################

MAVLINK_ROUTER_SITE_METHOD = git
MAVLINK_ROUTER_SITE = https://github.com/mavlink-router/mavlink-router
MAVLINK_ROUTER_VERSION = 18c06da77563db9ba1b7c4173ff5cae1646b383e
MAVLINK_ROUTER_LICENSE = ASF-2.0

MAVLINK_ROUTER_DEPENDENCIES += host-meson host-pkgconf
MAVLINK_ROUTER_GIT_SUBMODULES = YES

MAVLINK_ROUTER_UNIT = drone
ifeq ($(OPENIPC_SOC_FAMILY),hi3536dv100)
	MAVLINK_ROUTER_UNIT = gs
endif

ifeq ($(BR2_TOOLCHAIN_USES_GLIBC),y)
MAVLINK_ROUTER_LIBC = glibc
else
MAVLINK_ROUTER_LIBC = musl
endif

ifeq ($(BR2_ARM_EABIHF),y)
MAVLINK_ROUTER_ABI = -hf
endif

MAVLINK_ROUTER_CONF_OPTS += \
	--buildtype=release \
	--cross-file $(MAVLINK_ROUTER_PKGDIR)/files/meson-cross-$(ARCH)-$(MAVLINK_ROUTER_LIBC)$(MAVLINK_ROUTER_ABI)-openipc.ini

define MAVLINK_ROUTER_CONFIGURE_CMDS
	$(TARGET_MAKE_ENV) meson setup $(MAVLINK_ROUTER_CONF_OPTS) $(@D) $(@D)/build
endef

define MAVLINK_ROUTER_BUILD_CMDS
	$(TARGET_MAKE_ENV) ninja -C $(@D)/build
endef

define MAVLINK_ROUTER_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) DESTDIR=$(TARGET_DIR) ninja -C $(@D)/build install

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp $(MAVLINK_ROUTER_PKGDIR)/files/mavlink_$(MAVLINK_ROUTER_UNIT).conf $(TARGET_DIR)/etc/mavlink.conf

#	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
#	cp $(MAVLINK_ROUTER_PKGDIR)/files/S97mavlink $(TARGET_DIR)/etc/init.d
endef

$(eval $(generic-package))
