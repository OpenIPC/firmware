################################################################################
#
# linux-ext-vendor-patcher
#
################################################################################

LINUX_EXTENSIONS += vendor-patcher
VENDOR_PATCHER_PATH = $(BR2_EXTERNAL_VENDOR)/board/$(EXTERNAL_FAMILY)/kernel

define VENDOR_PATCHER_PREPARE_KERNEL
	$(VENDOR_PATCHER_PKGDIR)/apply.sh $(@D) "$(VENDOR_PATCHER_PATH)/patches/ $(VENDOR_PATCHER_PATH)/overlay"
endef
