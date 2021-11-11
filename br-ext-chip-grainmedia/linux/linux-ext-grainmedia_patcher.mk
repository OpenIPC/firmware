################################################################################
#
# linux_kernel_ext_grainmedia_patcher
#
################################################################################

LINUX_EXTENSIONS += grainmedia_patcher

define GRAINMEDIA_PATCHER_PREPARE_KERNEL
	$(GRAINMEDIA_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_GRAINMEDIA_PATCHER_LIST)
endef
