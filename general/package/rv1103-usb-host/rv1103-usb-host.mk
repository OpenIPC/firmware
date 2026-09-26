################################################################################
#
# rv1103-usb-host
#
################################################################################

RV1103_USB_HOST_VERSION = 1.0
RV1103_USB_HOST_SITE_METHOD = local
RV1103_USB_HOST_SITE = $(BR2_EXTERNAL_GENERAL_PATH)/package/rv1103-usb-host
RV1103_USB_HOST_LICENSE = MIT

define RV1103_USB_HOST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/files/S22usbhost \
		$(TARGET_DIR)/etc/init.d/S22usbhost
endef

$(eval $(generic-package))
