################################################################################
#
# mt7601u-ap-openipc
#
################################################################################

MT7601U_AP_OPENIPC_VERSION = HEAD
MT7601U_AP_OPENIPC_SITE = $(call github,openipc,mt7601u,$(MT7601U_AP_OPENIPC_VERSION))
MT7601U_AP_OPENIPC_LICENSE = GPL-2.0

MT7601U_AP_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
