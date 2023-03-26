################################################################################
#
# linux-ext-sigmastar_initramfs
#
################################################################################

ifeq ($(BR2_LINUX_KERNEL_EXT_SIGMASTAR_INITRAMFS),y)
LINUX_EXTENSIONS += sigmastar-initramfs
LINUX_DEPENDENCIES += sigmastar-initramfs
endif

define SIGMASTAR_INITRAMFS_PREPARE_KERNEL
	cp -rf $(SIGMASTAR_INITRAMFS_PKGDIR)/files/initramfs $(@D)
	$(SIGMASTAR_INITRAMFS_PKGDIR)/sigmastar-initramfs.sh "$(@D)" "$(MAKE)"
endef
