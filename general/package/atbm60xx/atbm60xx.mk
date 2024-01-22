################################################################################
#
# atbm60xx
#
################################################################################

ATBM60XX_SITE = $(call github,openipc,atbm_60xx,$(ATBM60XX_VERSION))
ATBM60XX_VERSION = master

define ATBM60XX_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/atbm60xx_conf
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/atbm60xx_conf $(ATBM60XX_PKGDIR)/files/*.txt
endef

ATBM60XX_MODULE_MAKE_OPTS = KSRC=$(LINUX_DIR)

# Set the module name based on the model and interface type
ATBM60XX_MODEL_NAME =

# Disable all models by default
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM601x=n CONFIG_ATBM602x=n CONFIG_ATBM603x=n CONFIG_ATBM6041=n

# Enable the selected model and set the interface type
ifeq ($(BR2_PACKAGE_ATBM60XX_MODEL_601X),y)
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM601x=y
ATBM60XX_MODEL_NAME = atbm601x_wifi
endif
ifeq ($(BR2_PACKAGE_ATBM60XX_MODEL_602X),y)
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM602x=y
ATBM60XX_MODEL_NAME = atbm602x_wifi
endif
ifeq ($(BR2_PACKAGE_ATBM60XX_MODEL_603X),y)
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM603x=y
ATBM60XX_MODEL_NAME = atbm603x_wifi
endif
ifeq ($(BR2_PACKAGE_ATBM60XX_MODEL_6041),y)
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM6041=y
ATBM60XX_MODEL_NAME = atbm6041_wifi
endif

# Set the interface type
ifeq ($(BR2_PACKAGE_ATBM60XX_INTERFACE_USB),y)
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM_USB_BUS=y CONFIG_ATBM_SDIO_BUS=n
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM_MODULE_NAME="$(ATBM60XX_MODEL_NAME)_usb"
endif
ifeq ($(BR2_PACKAGE_ATBM60XX_INTERFACE_SDIO),y)
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM_USB_BUS=n CONFIG_ATBM_SDIO_BUS=y
ATBM60XX_MODULE_MAKE_OPTS += CONFIG_ATBM_MODULE_NAME="$(ATBM60XX_MODEL_NAME)_sdio"
endif

ATBM60XX_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
