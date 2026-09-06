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

# Ordering, not a feature requirement: the two web UI files below land inside
# /var/www, which majestic-webui unpacks wholesale from its dist tarball. Named
# as a dependency so ours are written after that has happened rather than
# whenever the scheduler feels like it.
USB_DUAL_ROLE_DEPENDENCIES = majestic-webui

define USB_DUAL_ROLE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/files/usb-mode $(TARGET_DIR)/usr/sbin/usb-mode
	$(INSTALL) -D -m 0755 $(@D)/files/uvc-gadget-setup $(TARGET_DIR)/usr/sbin/uvc-gadget-setup
	$(INSTALL) -D -m 0755 $(@D)/files/S72usbmode $(TARGET_DIR)/etc/init.d/S72usbmode
	# The USB page's working parts. They live here rather than in
	# majestic-webui because that ships one tarball to every camera, and a
	# board with no switchable port would carry a role switch it can never
	# use -- on gk7205v300_lite, which is already over its partition, that is
	# 4 KB of the 5.4 KB this page costs. Only the page shell and its guarded
	# nav entry stay in the web UI, because its link checker needs the .cgi a
	# menu entry names to exist in its own tree.
	$(INSTALL) -D -m 0755 $(@D)/files/www/cgi-bin/j/usb.cgi $(TARGET_DIR)/var/www/cgi-bin/j/usb.cgi
	$(INSTALL) -D -m 0644 $(@D)/files/www/a/usb.js $(TARGET_DIR)/var/www/a/usb.js
endef

$(eval $(generic-package))
