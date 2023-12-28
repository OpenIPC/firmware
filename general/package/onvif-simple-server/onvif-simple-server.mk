################################################################################
#
# onvif-simple-server
#
################################################################################

ONVIF_SIMPLE_SERVER_SITE_METHOD = git
ONVIF_SIMPLE_SERVER_SITE = https://github.com/roleoroleo/onvif_simple_server
#ONVIF_SIMPLE_SERVER_VERSION = de6d585a245e4873c49b113360f98907f2b4cf49
ONVIF_SIMPLE_SERVER_VERSION = $(shell git ls-remote $(ONVIF_SIMPLE_SERVER_SITE) HEAD | head -1 | awk '{ print $$1 }')

ONVIF_SIMPLE_SERVER_DEPENDENCIES = mbedtls
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
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/var/www/onvif/device_service_files
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/var/www/onvif/device_service_files $(@D)/device_service_files/*
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/var/www/onvif/generic_files
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/var/www/onvif/generic_files $(@D)/generic_files/*
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/var/www/onvif/media_service_files
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/var/www/onvif/media_service_files $(@D)/media_service_files/*
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/var/www/onvif/ptz_service_files
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/var/www/onvif/ptz_service_files $(@D)/ptz_service_files/*
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/var/www/onvif/wsd_files
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/var/www/onvif/wsd_files $(@D)/wsd_files/*

	ln -s /usr/sbin/onvif_simple_server $(TARGET_DIR)/var/www/onvif/device_service
	ln -s /usr/sbin/onvif_simple_server $(TARGET_DIR)/var/www/onvif/events_service
	ln -s /usr/sbin/onvif_simple_server $(TARGET_DIR)/var/www/onvif/media_service
	ln -s /usr/sbin/onvif_simple_server $(TARGET_DIR)/var/www/onvif/ptz_service
endef

$(eval $(generic-package))
