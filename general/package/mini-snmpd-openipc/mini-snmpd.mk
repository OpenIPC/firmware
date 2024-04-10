################################################################################
#
# mini-snmpd-openipc
#
################################################################################

MINI_SNMPD_OPENIPC_VERSION = v1.4
MINI_SNMPD_OPENIPC_SITE = $(call github,troglobit,mini-snmpd,$(MINI_SNMPD_OPENIPC_VERSION))

MINI_SNMPD_OPENIPC_LICENSE = GPL-2.0
MINI_SNMPD_OPENIPC_LICENSE_FILES = COPYING
MINI_SNMPD_OPENIPC_AUTORECONF = YES

define MINI_SNMPD_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(MINI_SNMPD_OPENIPC_PKGDIR)/files/S50snmpd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/mini_snmpd
endef

$(eval $(autotools-package))
