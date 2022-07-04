################################################################################
#
# linux_kernel_ext_ambarella_patcher
#
################################################################################

LINUX_EXTENSIONS += ambarella_patcher

define AMBARELLA_PATCHER_PREPARE_KERNEL
	$(AMBARELLA_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_AMBARELLA_PATCHER_LIST)
endef
