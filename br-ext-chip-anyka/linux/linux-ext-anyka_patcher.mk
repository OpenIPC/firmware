################################################################################
#
# linux_kernel_ext_anyka_patcher
#
################################################################################

LINUX_EXTENSIONS += anyka_patcher

define ANYKA_PATCHER_PREPARE_KERNEL
	$(ANYKA_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_ANYKA_PATCHER_LIST)
endef
