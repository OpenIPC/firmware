################################################################################
#
# linux_kernel_ext_novatek_patcher
#
################################################################################

LINUX_EXTENSIONS += novatek_patcher

define NOVATEK_PATCHER_PREPARE_KERNEL
	$(NOVATEK_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_NOVATEK_PATCHER_LIST)
endef
