################################################################################
#
# openipc-nfs-root
#
# NFS-root-specific files and scripts for OpenIPC builds.
#
################################################################################

OPENIPC_NFS_ROOT_VERSION = 1.0
OPENIPC_NFS_ROOT_SITE_METHOD = local
OPENIPC_NFS_ROOT_SITE = $(BR2_EXTERNAL_GENERAL_PATH)/package/openipc-nfs-root
OPENIPC_NFS_ROOT_LICENSE = MIT

define OPENIPC_NFS_ROOT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/files/S45setupenv $(TARGET_DIR)/etc/init.d/S45setupenv
	$(INSTALL) -D -m 0755 $(@D)/files/S39netprofiles $(TARGET_DIR)/etc/init.d/S39netprofiles
	$(INSTALL) -D -m 0444 $(@D)/files/interfaces $(TARGET_DIR)/etc/network/profiles/interfaces-nfs-root
	$(INSTALL) -D -m 0444 $(@D)/files/eth0 $(TARGET_DIR)/etc/network/profiles/eth0-nfs-root
endef

$(eval $(generic-package))
