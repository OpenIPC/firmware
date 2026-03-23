################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# mdnsd-openipc — mdnsd-openipc Buildroot package
#
################################################################################

MDNSD_OPENIPC_SITE = $(call github,troglobit,mdnsd,$(MDNSD_OPENIPC_VERSION))
MDNSD_OPENIPC_VERSION = v0.12

MDNSD_OPENIPC_LICENSE = BSD-3-Clause
MDNSD_OPENIPC_LICENSE_FILES = LICENSE
MDNSD_OPENIPC_DEPENDENCIES = host-pkgconf
MDNSD_OPENIPC_AUTORECONF = YES

ifeq ($(BR2_PACKAGE_MDNSD_MQUERY_OPENIPC),y)
MDNSD_OPENIPC_CONF_OPTS += --with-mquery --without-systemd
else
MDNSD_OPENIPC_CONF_OPTS += --without-mquery --without-systemd
endif

define MDNSD_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(MDNSD_OPENIPC_PKGDIR)/files/S50mdnsd
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/mdns.d
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/mdns.d $(MDNSD_OPENIPC_PKGDIR)/files/rtsp.service
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/src/mquery
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(@D)/libmdnsd/.libs/libmdnsd.so.1.0.0
	ln -sf libmdnsd.so.1.0.0 $(TARGET_DIR)/usr/lib/libmdnsd.so
	ln -sf libmdnsd.so.1.0.0 $(TARGET_DIR)/usr/lib/libmdnsd.so.1
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/src/mdnsd
endef

$(eval $(autotools-package))
