################################################################################
#
# mavlink-router
#
################################################################################

MAVLINK_ROUTER_LICENSE = ASF-2.0

define MAVLINK_ROUTER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/usr/bin
	cp ../general/package/mavlink-router/files/mavlink-routerd $(TARGET_DIR)/usr/bin/mavlink-routerd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp ../general/package/mavlink-router/files/mavlink.conf $(TARGET_DIR)/etc/mavlink.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp ../general/package/mavlink-router/files/S97mavlink $(TARGET_DIR)/etc/init.d
endef

$(eval $(generic-package))
