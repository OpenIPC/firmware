################################################################################
#
# mavlink-router
#
################################################################################

MAVLINK_ROUTER_VERSION = 18c06da77563db9ba1b7c4173ff5cae1646b383e
MAVLINK_ROUTER_SITE = $(call github,mavlink-router,mavlink-router,$(MAVLINK_ROUTER_VERSION))
MAVLINK_ROUTER_LICENSE = ASF-2.0

MAVLINK_ROUTER_DEPENDENCIES += host-meson host-pkgconf

MAVLINK_ROUTER_CONF_OPTS += \
	--buildtype=release \
	--cross-file $(MAVLINK_ROUTER_PKGDIR)/files/meson-cross-$(ARCH)-openipc.ini

define MAVLINK_ROUTER_CONFIGURE_CMDS
	rm -rf $(@D)/build
	mkdir -p $(@D)/build
	cp -f $(MAVLINK_ROUTER_PKGDIR)/files/prepare.sh $(@D)/prepare.sh
	cd $(@D) ; sh prepare.sh
	$(TARGET_MAKE_ENV) meson setup $(MAVLINK_ROUTER_CONF_OPTS) $(@D) $(@D)/build
endef

define MAVLINK_ROUTER_BUILD_CMDS
	$(TARGET_MAKE_ENV) ninja -C $(@D)/build
endef

define MAVLINK_ROUTER_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) DESTDIR=$(TARGET_DIR) ninja -C $(@D)/build install

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp $(MAVLINK_ROUTER_PKGDIR)/files/mavlink.conf $(TARGET_DIR)/etc/mavlink.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(MAVLINK_ROUTER_PKGDIR)/files/S97mavlink $(TARGET_DIR)/etc/init.d
endef

$(eval $(generic-package))
