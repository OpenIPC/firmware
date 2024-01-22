################################################################################
#
# ingenic-libimp-control
#
################################################################################

INGENIC_LIBIMP_CONTROL_SITE = $(call github,gtxaspec,libimp_control,$(INGENIC_LIBIMP_CONTROL_VERSION))
INGENIC_LIBIMP_CONTROL_VERSION = master

define INGENIC_LIBIMP_CONTROL_BUILD_CMDS
	$(MAKE) CONFIG_SOC=$(OPENIPC_SOC_MODEL) CROSS_COMPILE=$(TARGET_CROSS) -C $(@D)
endef

define INGENIC_LIBIMP_CONTROL_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libimp_control.so $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(INGENIC_LIBIMP_CONTROL_PKGDIR)/src/imp-control.sh
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(INGENIC_LIBIMP_CONTROL_PKGDIR)/src/S96impconfig
endef

$(eval $(generic-package))
