################################################################################
#
# AltoBeam INGENIC_MOTORS_T31 wifi driver
#
################################################################################

INGENIC_MOTORS_T31_LICENSE = GPL-2.0

define INGENIC_MOTORS_T31_EXTRACT_CMDS
	cp -av $(INGENIC_MOTORS_T31_PKGDIR)/src/* $(@D)/
endef

INGENIC_MOTORS_T31_MODULE_MAKE_OPTS = \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
