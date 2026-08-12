################################################################################
#
# mt7601u-openipc
#
################################################################################

MT7601U_OPENIPC_SITE = $(call github,openipc,mt7601u,$(MT7601U_OPENIPC_VERSION))
MT7601U_OPENIPC_VERSION = 0ac46553f3190d788b01c15cbeaa14f2951c55a3
MT7601U_OPENIPC_LICENSE = GPL-2.0

MT7601U_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

define MT7601U_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/mediatek
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/mediatek $(@D)/MT7601USTA.dat
endef

$(eval $(kernel-module))
$(eval $(generic-package))
