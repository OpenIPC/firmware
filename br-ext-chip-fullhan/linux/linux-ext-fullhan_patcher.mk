################################################################################
#
# linux_kernel_ext_fullhan_patcher
#
################################################################################

LINUX_EXTENSIONS += fullhan_patcher

define FULLHAN_PATCHER_PREPARE_KERNEL
	$(FULLHAN_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_FULLHAN_PATCHER_LIST)
endef
