################################################################################
#
# mini-snmpd-openipc
#
################################################################################

MINI_SNMPD_OPENIPC_VERSION = 1.4
MINI_SNMPD_OPENIPC_SITE = $(call github,troglobit,mini-snmpd,v$(MINI_SNMPD_OPENIPC_VERSION))

MINI_SNMPD_OPENIPC_LICENSE = GPL-2.0
MINI_SNMPD_OPENIPC_LICENSE_FILES = COPYING
MINI_SNMPD_OPENIPC_AUTORECONF = YES

define MINI_SNMPD_OPENIPC_INSTALL_TARGET_CMDS

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(MINI_SNMPD_OPENIPC_PKGDIR)/files/S50snmpd $(TARGET_DIR)/etc/init.d

endef

$(eval $(autotools-package))
