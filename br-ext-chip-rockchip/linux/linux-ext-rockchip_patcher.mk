################################################################################
#
# linux_kernel_ext_rockchip_patcher
#
################################################################################

LINUX_EXTENSIONS += rockchip_patcher

define ROCKCHIP_PATCHER_PREPARE_KERNEL
	$(ROCKCHIP_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_ROCKCHIP_PATCHER_LIST)
endef
