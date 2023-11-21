################################################################################
#
# Ingenic Motors Driver
#
################################################################################

INGENIC_MOTORS_T31_LICENSE = GPL-2.0

define INGENIC_MOTORS_T31_EXTRACT_CMDS
	cp -av $(INGENIC_MOTORS_T31_PKGDIR)/src/* $(@D)/
endef

INGENIC_MOTORS_T31_MODULE_MAKE_OPTS = \
	INSTALL_MOD_PATH=$(TARGET_DIR) \
	INSTALL_MOD_DIR=ingenic \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
