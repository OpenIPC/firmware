################################################################################
#
# vtund-openipc
#
################################################################################

VTUND_OPENIPC_VERSION = 3.0.2
VTUND_OPENIPC_SOURCE = vtun-$(VTUND_OPENIPC_VERSION).tar.gz
VTUND_OPENIPC_SITE = http://prdownloads.sourceforge.net/vtun

VTUND_OPENIPC_LICENSE = GPL-2.0+
VTUND_OPENIPC_LICENSE_FILES = LICENSE
VTUND_OPENIPC_DEPENDENCIES = host-bison host-flex host-pkgconf

VTUND_OPENIPC_CONF_OPTS += --disable-lzo --disable-ssl --disable-zlib

# Assumes old-style gcc inline symbol visibility rules
VTUND_OPENIPC_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -std=gnu89"

define VTUND_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(VTUND_OPENIPC_PKGDIR)/files/S98vtun

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(VTUND_OPENIPC_PKGDIR)/files/tapip
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(VTUND_OPENIPC_PKGDIR)/files/tunnel
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/vtund
endef

$(eval $(autotools-package))
