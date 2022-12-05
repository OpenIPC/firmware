################################################################################
#
# linux_kernel_ext_ti_patcher
#
################################################################################

LINUX_EXTENSIONS += ti_patcher

define TI_PATCHER_PREPARE_KERNEL
	$(TI_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_TI_PATCHER_LIST)
endef
