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
	$(INSTALL) -D -m 0755 $(@D)/files/rootfs-setup $(TARGET_DIR)/usr/sbin/rootfs-setup
	$(INSTALL) -D -m 0755 $(@D)/files/set-hostname-from-cmdline $(TARGET_DIR)/usr/sbin/set-hostname-from-cmdline
	$(INSTALL) -D -m 0755 $(@D)/files/S45setupenv $(TARGET_DIR)/etc/init.d/S45setupenv
endef

$(eval $(generic-package))
