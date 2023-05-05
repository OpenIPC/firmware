################################################################################
#
# datalink
#
################################################################################
DATALINK_VERSION = 
DATALINK_SITE =
DATALINK_LICENSE = GPL-2.0
FPATH = air

FAMILY := $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)

RELEASE := $(shell grep "BR2_DEFCONFIG" $(BR2_CONFIG) | head -1 | cut -d "_" -f 3)

ifeq ($(FAMILY),hi3536dv100)
ifeq ($(RELEASE),fpv)
FPATH = gs
endif
endif

define DATALINK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp $(DATALINK_PKGDIR)/files/datalink.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc               
	cp $(DATALINK_PKGDIR)/files/$(FPATH)/telemetry.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(DATALINK_PKGDIR)/files/S98datalink $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(DATALINK_PKGDIR)/files/$(FPATH)/telemetry $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(DATALINK_PKGDIR)/files/tweaksys $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
