################################################################################
#
# datalink
#
################################################################################
DATALINK_VERSION = 
DATALINK_SITE =
DATALINK_LICENSE = GPL-2.0

DATALINK_FPATH = air
ifeq ($(OPENIPC_SOC_FAMILY),hi3536dv100)
	DATALINK_FPATH = gs
endif

define DATALINK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp $(DATALINK_PKGDIR)/files/datalink.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc               
	cp $(DATALINK_PKGDIR)/files/$(DATALINK_FPATH)/telemetry.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(DATALINK_PKGDIR)/files/S98datalink $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(DATALINK_PKGDIR)/files/$(DATALINK_FPATH)/telemetry $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(DATALINK_PKGDIR)/files/tweaksys $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
