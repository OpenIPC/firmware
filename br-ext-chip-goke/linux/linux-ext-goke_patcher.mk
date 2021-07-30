################################################################################
#
# linux_kernel_ext_goke_patcher
#
################################################################################

LINUX_EXTENSIONS += goke_patcher

define GOKE_PATCHER_PREPARE_KERNEL
	$(GOKE_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_GOKE_PATCHER_LIST)
endef
