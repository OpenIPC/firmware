################################################################################
#
# node-exporter
#
################################################################################

NODE_EXPORTER_VERSION =
NODE_EXPORTER_SITE =
NODE_EXPORTER_LICENSE = MIT
NODE_EXPORTER_LICENSE_FILES = LICENSE
NODE_EXPORTER_INSTALL_STAGING = YES

define NODE_EXPORTER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(NODE_EXPORTER_PKGDIR)/files/script/S99node-exporter

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(NODE_EXPORTER_PKGDIR)/files/script/node-exporter

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/lua/5.1/prometheus-collectors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/lua/5.1/prometheus-collectors $(NODE_EXPORTER_PKGDIR)/files/prometheus-collectors/*.lua
endef

$(eval $(generic-package))
