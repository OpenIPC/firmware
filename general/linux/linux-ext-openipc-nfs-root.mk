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

define OPENIPC_NFS_ROOT_CHECK_KERNEL_SUPPORT
	grep -qr 'eth_platform_get_mac_address' $(@D)/net $(@D)/include 2>/dev/null || { \
		echo "openipc-nfs-root requires Linux >= 4.5 or a vendor kernel with eth_platform_get_mac_address() support" >&2; \
		exit 1; \
	}
endef

define OPENIPC_NFS_ROOT_PREPARE_KERNEL
	$(OPENIPC_NFS_ROOT_CHECK_KERNEL_SUPPORT)
	$(APPLY_PATCHES) $(@D) $(BR2_EXTERNAL_GENERAL_PATH)/package/openipc-nfs-root/kernel-patches \*.patch
	$(OPENIPC_NFS_ROOT_SYNC_ETHADDR_SOURCE)
endef
