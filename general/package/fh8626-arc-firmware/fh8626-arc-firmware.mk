################################################################################
#
# fh8626-arc-firmware
#
################################################################################

FH8626_ARC_FIRMWARE_SITE = $(FH8626_ARC_FIRMWARE_PKGDIR)/src
FH8626_ARC_FIRMWARE_SITE_METHOD = local
FH8626_ARC_FIRMWARE_LICENSE = MIT
FH8626_ARC_FIRMWARE_LICENSE_FILES = LICENSE
FH8626_ARC_FIRMWARE_DEPENDENCIES = host-arc-binutils

define FH8626_ARC_FIRMWARE_BUILD_CMDS
	$(HOST_MAKE_ENV) $(MAKE) -C $(@D) \
		ARC_AS=$(HOST_DIR)/bin/arc-elf32-as ARC_LD=$(HOST_DIR)/bin/arc-elf32-ld
endef

define FH8626_ARC_FIRMWARE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/fh8626-arc.elf $(TARGET_DIR)/lib/firmware/fh8626-arc.elf
endef

$(eval $(generic-package))
