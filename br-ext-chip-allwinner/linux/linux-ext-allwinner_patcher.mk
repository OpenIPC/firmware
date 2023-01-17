################################################################################
#
# linux_kernel_ext_allwinner_patcher
#
################################################################################

LINUX_EXTENSIONS += allwinner_patcher

define ALLWINNER_PATCHER_PREPARE_KERNEL
	$(ALLWINNER_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_ALLWINNER_PATCHER_LIST)
endef
