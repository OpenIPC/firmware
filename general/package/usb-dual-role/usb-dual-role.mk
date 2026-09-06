################################################################################
#
# usb-dual-role
#
# Runtime host/device switching for a dual-role DWC3 USB port.
#
################################################################################

USB_DUAL_ROLE_VERSION = 1.0
USB_DUAL_ROLE_SITE_METHOD = local
USB_DUAL_ROLE_SITE = $(BR2_EXTERNAL_GENERAL_PATH)/package/usb-dual-role
USB_DUAL_ROLE_LICENSE = MIT

define USB_DUAL_ROLE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/files/usb-mode $(TARGET_DIR)/usr/sbin/usb-mode
	$(INSTALL) -D -m 0755 $(@D)/files/uvc-gadget-setup $(TARGET_DIR)/usr/sbin/uvc-gadget-setup
	$(INSTALL) -D -m 0755 $(@D)/files/S72usbmode $(TARGET_DIR)/etc/init.d/S72usbmode
endef

$(eval $(generic-package))
