################################################################################
#
# majestic-webui
#
################################################################################

# Pre-minified distribution built by majestic-webui CI (.github/workflows/dist.yml) and
# published as a rolling release asset. Fetching the prepared artifact keeps this build
# hermetic (no JS/CSS toolchain) and the webui source in git unminified. `dist` is a
# moving ref, so the "Refresh moving-ref package downloads" CI step keeps it from
# going stale in the dl cache.
MAJESTIC_WEBUI_VERSION = dist
MAJESTIC_WEBUI_SITE = https://github.com/openipc/majestic-webui/releases/download/dist
MAJESTIC_WEBUI_SOURCE = majestic-webui-dist.tar.gz
MAJESTIC_WEBUI_LICENSE = MIT
MAJESTIC_WEBUI_LICENSE_FILES = LICENSE

ifeq ($(OPENIPC_VARIANT),fpv)
	VERSION = FPV
else
	VERSION = STANDARD
endif

define MAJESTIC_WEBUI_INSTALL
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr
	cp -r $(@D)/sbin $(TARGET_DIR)/usr
	[ -d $(@D)/bin ] && cp -r $(@D)/bin $(TARGET_DIR)/usr || true

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -r $(@D)/www $(TARGET_DIR)/var
endef

define MAJESTIC_WEBUI_STANDARD_FIXUP
	rm $(TARGET_DIR)/var/www/cgi-bin/fpv-wfb.cgi
	rm $(TARGET_DIR)/var/www/cgi-bin/j/locale_fpv.cgi
	rm $(TARGET_DIR)/var/www/cgi-bin/p/header_fpv.cgi
	rm $(TARGET_DIR)/var/www/cgi-bin/p/fpv_common.cgi
endef

define MAJESTIC_WEBUI_FPV_FIXUP
	mv -f $(TARGET_DIR)/var/www/cgi-bin/j/locale_fpv.cgi $(TARGET_DIR)/var/www/cgi-bin/j/locale.cgi
	mv -f $(TARGET_DIR)/var/www/cgi-bin/p/header_fpv.cgi $(TARGET_DIR)/var/www/cgi-bin/p/header.cgi
	rm $(TARGET_DIR)/usr/sbin/telegram
	rm $(TARGET_DIR)/usr/sbin/openwall
endef

define MAJESTIC_WEBUI_INSTALL_TARGET_CMDS
	$(MAJESTIC_WEBUI_INSTALL)
	$(MAJESTIC_WEBUI_$(VERSION)_FIXUP)
endef

$(eval $(generic-package))
