################################################################################
#
# mt7601u-ap-openipc
#
################################################################################

MT7601U_AP_OPENIPC_SITE_METHOD = git
MT7601U_AP_OPENIPC_SITE = https://github.com/openipc/mt7601u
MT7601U_AP_OPENIPC_VERSION = $(call EXTERNAL_SHA,$(MT7601U_AP_OPENIPC_SITE),HEAD)

MT7601U_AP_OPENIPC_LICENSE = GPL-2.0

MT7601U_AP_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
