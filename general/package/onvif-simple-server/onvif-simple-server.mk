################################################################################
#
# onvif-simple-server
#
################################################################################

ONVIF_SIMPLE_SERVER_SITE_METHOD = git
ONVIF_SIMPLE_SERVER_SITE = https://github.com/roleoroleo/onvif_simple_server
ONVIF_SIMPLE_SERVER_VERSION = 8a19b6662af3ab123bb000a09710728cc355359c
ONVIF_SIMPLE_SERVER_DEPENDENCIES = libtomcrypt

ONVIF_SIMPLE_SERVER_LICENSE = MIT
ONVIF_SIMPLE_SERVER_LICENSE_FILES = LICENSE


define ONVIF_SIMPLE_SERVER_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)
endef

define ONVIF_SIMPLE_SERVER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 0755 -t $(TARGET_DIR)/usr/sbin $(@D)/onvif_simple_server
	$(INSTALL) -m 0755 -t $(TARGET_DIR)/usr/sbin $(@D)/wsd_simple_server
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/etc $(ONVIF_SIMPLE_SERVER_PKGDIR)/files/onvif.conf
endef

$(eval $(generic-package))
