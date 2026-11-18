################################################################################
#
# reset-button
#
################################################################################

RESET_BUTTON_VERSION = 1.0
RESET_BUTTON_SITE_METHOD = local
RESET_BUTTON_SITE = $(RESET_BUTTON_PKGDIR)/src
RESET_BUTTON_LICENSE = MIT

define RESET_BUTTON_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -Os -s \
		$(@D)/reset-button.c -o $(@D)/reset-button -lm
endef

define RESET_BUTTON_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/reset-button $(TARGET_DIR)/usr/sbin/reset-button
	$(INSTALL) -m 0755 -D $(RESET_BUTTON_PKGDIR)/files/S96reset-button \
		$(TARGET_DIR)/etc/init.d/S96reset-button
endef

$(eval $(generic-package))
