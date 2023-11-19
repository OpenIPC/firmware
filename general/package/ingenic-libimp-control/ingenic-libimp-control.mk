################################################################################
#
# ingenic-libimp-control
#
################################################################################

INGENIC_LIBIMP_CONTROL_VERSION = master
INGENIC_LIBIMP_CONTROL_SITE = $(call github,gtxaspec,libimp_control,$(INGENIC_LIBIMP_CONTROL_VERSION))
INGENIC_LIBIMP_CONTROL_LICENSE = GPL-2.0
INGENIC_LIBIMP_CONTROL_LICENSE_FILES = COPYING

define INGENIC_LIBIMP_CONTROL_BUILD_CMDS
    $(MAKE) CROSS_COMPILE=$(TARGET_CROSS) -C $(@D)
endef

define INGENIC_LIBIMP_CONTROL_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/libimp_control.so $(TARGET_DIR)/usr/lib
    $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/sbin $(INGENIC_LIBIMP_CONTROL_PKGDIR)/src/imp-control.sh

endef

$(eval $(generic-package))
