################################################################################
#
# linux_kernel_ext_xiongmai_patcher
#
################################################################################

LINUX_EXTENSIONS += xiongmai_patcher

define XIONGMAI_PATCHER_PREPARE_KERNEL
	$(XIONGMAI_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_XIONGMAI_PATCHER_LIST)
endef
