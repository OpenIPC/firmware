################################################################################
#
# linux_kernel_ext_sigmastar_patcher
#
################################################################################

LINUX_EXTENSIONS += sigmastar_patcher

define SIGMASTAR_PATCHER_PREPARE_KERNEL
	$(SIGMASTAR_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_SIGMASTAR_PATCHER_LIST)
endef
