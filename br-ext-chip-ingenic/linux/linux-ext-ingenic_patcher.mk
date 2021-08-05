################################################################################
#
# linux_kernel_ext_ingenic_patcher
#
################################################################################

LINUX_EXTENSIONS += ingenic_patcher

define INGENIC_PATCHER_PREPARE_KERNEL
	$(INGENIC_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_INGENIC_PATCHER_LIST)
endef
