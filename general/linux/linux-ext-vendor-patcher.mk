################################################################################
#
# linux-ext-vendor-patcher
#
################################################################################

LINUX_EXTENSIONS += vendor-patcher

define VENDOR_PATCHER_PREPARE_KERNEL
	$(VENDOR_PATCHER_PKGDIR)/apply.sh $(@D) $(BR2_LINUX_KERNEL_EXT_VENDOR_PATCHER_LIST)
endef
