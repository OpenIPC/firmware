################################################################################
#
# vtund-lite
#
################################################################################

VTUND_LITE_VERSION = 3.0.2
VTUND_LITE_SOURCE = vtun-$(VTUND_LITE_VERSION).tar.gz
VTUND_LITE_SITE = http://prdownloads.sourceforge.net/vtun
VTUND_LITE_LICENSE = GPL-2.0+
VTUND_LITE_LICENSE_FILES = LICENSE
VTUND_LITE_DEPENDENCIES = host-bison host-flex host-pkgconf

VTUND_LITE_CONF_OPTS += --disable-lzo --disable-ssl --disable-zlib

# Assumes old-style gcc inline symbol visibility rules
VTUND_LITE_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -std=gnu89"

define VTUND_LITE_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(TOPDIR)/../general/package/vtund-lite/files/tapip
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(TOPDIR)/../general/package/vtund-lite/files/tunnel
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/vtund
endef

$(eval $(autotools-package))
