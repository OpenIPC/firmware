################################################################################
#
# apfpv-greg
#
################################################################################

APFPV_GREG_SITE = $(patsubst %/,%,$(APFPV_GREG_PKGDIR))
APFPV_GREG_SITE_METHOD = local
APFPV_GREG_LICENSE = Mixed

define APFPV_GREG_INSTALL_TARGET_CMDS
	true
endef

$(eval $(generic-package))
