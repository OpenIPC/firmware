################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# siproxd-openipc — siproxd-openipc Buildroot package
#
################################################################################

SIPROXD_OPENIPC_VERSION = 0.8.3
SIPROXD_OPENIPC_SOURCE = siproxd-$(SIPROXD_OPENIPC_VERSION).tar.gz
SIPROXD_OPENIPC_SITE = http://prdownloads.sourceforge.net/siproxd

SIPROXD_OPENIPC_LICENSE = GPL-2.0+
SIPROXD_OPENIPC_LICENSE_FILES = COPYING

SIPROXD_OPENIPC_DEPENDENCIES = libosip2
SIPROXD_OPENIPC_CONF_OPTS += --without-included-ltdl LDFLAGS="-no-pie"
SIPROXD_OPENIPC_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -fno-PIE"

define SIPROXD_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(SIPROXD_OPENIPC_PKGDIR)/files/siproxd.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(SIPROXD_OPENIPC_PKGDIR)/files/siproxd_passwd.cfg

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(SIPROXD_OPENIPC_PKGDIR)/files/S60siproxd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/src/siproxd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/siproxd
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/lib/siproxd $(@D)/src/plugin_*.la
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/lib/siproxd $(@D)/src/.libs/plugin_*.so
endef

$(eval $(autotools-package))


