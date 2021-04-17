################################################################################
#
# linux_kernel_ext_hisi_patcher
#
################################################################################

LINUX_EXTENSIONS += hisi_patcher

define HISI_PATCHER_PREPARE_KERNEL
	$(HISI_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_HISI_PATCHER_LIST)
endef
