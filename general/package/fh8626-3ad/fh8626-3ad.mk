################################################################################
#
# fh8626-3ad
#
################################################################################

FH8626_3AD_SITE_METHOD = local
FH8626_3AD_SITE = $(FH8626_3AD_PKGDIR)/src
FH8626_3AD_LICENSE = Apache-2.0 or GPL-2.0
FH8626_3AD_LICENSE_FILES = fh8626-3ad.c
FH8626_3AD_DEPENDENCIES = linux

# The statistics and tuning controls are defined by the kernel's own UAPI
define FH8626_3AD_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -std=gnu99 -D_GNU_SOURCE \
		-iquote $(LINUX_DIR)/include/uapi \
		$(addprefix $(@D)/,fh8626-3ad.c ae-controller.c awb-controller.c isp-tuning.c) \
		$(TARGET_LDFLAGS) -lm -pthread -o $(@D)/fh8626-3ad
endef

define FH8626_3AD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/fh8626-3ad $(TARGET_DIR)/usr/sbin/fh8626-3ad
	$(INSTALL) -D -m 644 $(FH8626_3AD_PKGDIR)/files/fh8626-3ad.default \
		$(TARGET_DIR)/etc/default/fh8626-3ad
endef

define FH8626_3AD_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 $(FH8626_3AD_PKGDIR)/files/S80fh8626-3ad \
		$(TARGET_DIR)/etc/init.d/S80fh8626-3ad
endef

$(eval $(generic-package))
