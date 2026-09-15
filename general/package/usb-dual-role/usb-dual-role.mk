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

# Bring the kernel side with the package. usb-mode modprobes exactly three
# modules -- configfs, libcomposite and usb_f_uvc -- and uvc-gadget-setup
# composes the gadget under /sys/kernel/config/usb_gadget, so those symbols are
# this package's requirement rather than a property a board config happens to
# have. Stating it here means a board that selects usb-dual-role gets what its
# scripts need, and a board that does not carries none of it.
#
# This was previously neither: the goke boards shipped USB_F_ACM, USB_F_ECM,
# USB_F_RNDIS and USB_F_MASS_STORAGE -- none of which any gadget consumer in
# this tree ever composes -- while USB_CONFIGFS_F_UVC, the one function
# usb-mode actually loads, was absent from those configs entirely. So the
# modules that shipped could not serve the only package that wants a gadget,
# and the package could not have run there if it had been selected. The four
# boards that do enable F_UVC (hi3516cv6xx, hi3519dv500, rv1126, rv1106) are
# unaffected by this block -- they already have what it asks for.
#
# USB_LIBCOMPOSITE arrives by select from USB_CONFIGFS and so is not listed.
#
# USB_CONFIGFS_F_UVC is asked for but CANNOT be asserted from here, and the
# request is deliberately left in place rather than dropped: it also depends on
# VIDEO_V4L2 and VIDEO_DEV, so on a board with no V4L2 stack olddefconfig drops
# it and the UVC gadget is simply unavailable -- verified on gk7205v300_lite,
# where this block yields USB_GADGET/CONFIGFS_FS/USB_CONFIGFS/USB_LIBCOMPOSITE
# and no F_UVC. That is the honest outcome: a camera that cannot present a V4L2
# device cannot serve one over UVC either, and the media stack is far too big to
# pull in from a package fixup. The four boards that do enable F_UVC
# (hi3516cv6xx, hi3519dv500, rv1126, rv1106) have the V4L2 side already, and on
# those this block is satisfied in full.
define USB_DUAL_ROLE_LINUX_CONFIG_FIXUPS
	$(call KCONFIG_ENABLE_OPT,CONFIG_USB_GADGET)
	$(call KCONFIG_ENABLE_OPT,CONFIG_CONFIGFS_FS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_USB_CONFIGFS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_USB_CONFIGFS_F_UVC)
endef

$(eval $(generic-package))
