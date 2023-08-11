################################################################################
#
# mt7601u-openipc
#
################################################################################

MT7601U_OPENIPC_SITE_METHOD = git
MT7601U_OPENIPC_SITE = https://github.com/openipc/mt7601u
MT7601U_OPENIPC_VERSION = $(shell git ls-remote $(MT7601U_OPENIPC_SITE) HEAD | head -1 | cut -f1)
MT7601U_OPENIPC_LICENSE = GPL-2.0

MT7601U_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

define MT7601U_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/mediatek
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/mediatek $(@D)/MT7601USTA.dat
endef

$(eval $(kernel-module))
$(eval $(generic-package))
