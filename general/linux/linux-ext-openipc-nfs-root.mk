################################################################################
#
# openipc-nfs-root kernel extension
#
################################################################################

LINUX_EXTENSIONS += openipc-nfs-root

define OPENIPC_NFS_ROOT_SYNC_ETHADDR_SOURCE
	cp $(BR2_EXTERNAL_GENERAL_PATH)/package/openipc-nfs-root/files/openipc_ethaddr.c \
		$(@D)/drivers/net/ethernet/openipc_ethaddr.c
	grep -q '^obj-y += openipc_ethaddr.o$$' $(@D)/drivers/net/ethernet/Makefile || \
		echo 'obj-y += openipc_ethaddr.o' >> $(@D)/drivers/net/ethernet/Makefile
endef

define OPENIPC_NFS_ROOT_PREPARE_KERNEL
	$(APPLY_PATCHES) $(@D) $(BR2_EXTERNAL_GENERAL_PATH)/package/openipc-nfs-root/kernel-patches \*.patch
	$(OPENIPC_NFS_ROOT_SYNC_ETHADDR_SOURCE)
endef

LINUX_PRE_BUILD_HOOKS += OPENIPC_NFS_ROOT_SYNC_ETHADDR_SOURCE
