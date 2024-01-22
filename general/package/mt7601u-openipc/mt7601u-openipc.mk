################################################################################
#
# mt7601u-openipc
#
################################################################################

MT7601U_OPENIPC_SITE = $(call github,openipc,mt7601u,$(MT7601U_OPENIPC_VERSION))
MT7601U_OPENIPC_VERSION = master
MT7601U_OPENIPC_LICENSE = GPL-2.0

MT7601U_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

define MT7601U_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/mediatek
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/mediatek $(@D)/MT7601USTA.dat
endef

$(eval $(kernel-module))
$(eval $(generic-package))
