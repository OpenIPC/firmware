################################################################################
#
# atbm-wifi
#
################################################################################

ATBM_WIFI_SITE = $(call github,gtxaspec,atbm-wifi,$(ATBM_WIFI_VERSION))
ATBM_WIFI_VERSION = HEAD

define ATBM_WIFI_INSTALL_TARGET_CMDS
#	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/ATBM-WIFI_conf
#	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/ATBM-WIFI_conf $(ATBM-WIFI_PKGDIR)/files/*.txt
endef

ATBM_WIFI_MODULE_MAKE_OPTS = KSRC=$(LINUX_DIR)

# Set the module name based on the model and interface type
ATBM_WIFI_MODEL_NAME =

# Disable all models by default
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM6012B_y=n CONFIG_ATBM603x=n CONFIG_ATBM6132=n

# Enable the selected model and set the interface type
ifeq ($(BR2_PACKAGE_ATBM_WIFI_MODEL_6012B),y)
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM6012B_y=y
ATBM_WIFI_MODEL_NAME = atbm6012b_wifi
endif
ifeq ($(BR2_PACKAGE_ATBM_WIFI_MODEL_603X),y)
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM603x=y
ATBM_WIFI_MODEL_NAME = atbm603x_wifi
endif
ifeq ($(BR2_PACKAGE_ATBM_WIFI_MODEL_6132),y)
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM6132=y
ATBM_WIFI_MODEL_NAME = atbm6132_wifi
endif

# Set the interface type
ifeq ($(BR2_PACKAGE_ATBM_WIFI_INTERFACE_USB),y)
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM_USB_BUS=y CONFIG_ATBM_SDIO_BUS=n
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM_MODULE_NAME="$(ATBM_WIFI_MODEL_NAME)_usb"
ifneq ($(BR2_PACKAGE_ATBM_WIFI_USB_PID),)
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM_USB_PID="$(BR2_PACKAGE_ATBM_WIFI_USB_PID)"
endif
endif

ifeq ($(BR2_PACKAGE_ATBM_WIFI_INTERFACE_SDIO),y)
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM_USB_BUS=n CONFIG_ATBM_SDIO_BUS=y
ATBM_WIFI_MODULE_MAKE_OPTS += CONFIG_ATBM_MODULE_NAME="$(ATBM_WIFI_MODEL_NAME)_sdio"
endif

ATBM_WIFI_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
