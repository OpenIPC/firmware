################################################################################
#
# openipc-failsafe
#
# Failsafe rescue mode, DRAM boot-counter cleanup and pstore crash capture for
# SoC families that reserve the boot counter (Goke gk7205v200/gk7205v300). Kept
# out of the shared overlay so it does not ship to -- or write the memory of --
# families that do not reserve it. See PR #2396.
#
################################################################################

OPENIPC_FAILSAFE_VERSION = 1.0
OPENIPC_FAILSAFE_SITE_METHOD = local
OPENIPC_FAILSAFE_SITE = $(BR2_EXTERNAL_GENERAL_PATH)/package/openipc-failsafe
OPENIPC_FAILSAFE_LICENSE = MIT

define OPENIPC_FAILSAFE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/files/bootcount-clear $(TARGET_DIR)/usr/sbin/bootcount-clear
	$(INSTALL) -D -m 0755 $(@D)/files/failsafe-rescue $(TARGET_DIR)/usr/sbin/failsafe-rescue
	$(INSTALL) -D -m 0755 $(@D)/files/S98crashlog $(TARGET_DIR)/etc/init.d/S98crashlog
	$(INSTALL) -D -m 0755 $(@D)/files/S99bootok $(TARGET_DIR)/etc/init.d/S99bootok
endef

$(eval $(generic-package))
