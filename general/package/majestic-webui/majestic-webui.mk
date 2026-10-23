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

# wfbng is the wfb-ng variant's name since OpenIPC/builder renamed fpv, which
# says what the build carries; fpv stays for builds from before the rename.
# Only FPV has a fixup of its own now; the FPV page that STANDARD used to strip
# is BR2_PACKAGE_MAJESTIC_WEBUI_FPV, below.
ifneq ($(filter fpv wfbng,$(OPENIPC_VARIANT)),)
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

# -f throughout: which files the WebUI ships is decided in another repo, so a
# fixup that hard-fails on a missing one turns any deletion there into a broken
# build here -- for every platform at once, and only visible on the next PR,
# because master has no push trigger.
#
# The WebUI renamed every page after the word its menu uses, so the FPV page is
# wfb.cgi now. Both spellings are listed and both must stay for a while: the
# dist tarball is a rolling release off that repo's master, so an image built
# today gets the new name and one rebuilt from an older tag gets the old one,
# and a build without the page must not ship it either way. j/locale_fpv.cgi
# and p/header_fpv.cgi went in OpenIPC/majestic-webui#141 and are dropped here.
#
# The page is BR2_PACKAGE_MAJESTIC_WEBUI_FPV, on by default for the fpv and
# wfbng variants -- what it was keyed on before it was an option -- and a
# profile can still set it either way. a/wfb.js is the page's own script; it
# used to ship on every build. The WebUI's sbin/updatewebui lists the same
# files and installs them only where the image carries p/fpv_common.cgi.
define MAJESTIC_WEBUI_FPV_PAGE_FIXUP
	rm -f $(TARGET_DIR)/var/www/cgi-bin/wfb.cgi
	rm -f $(TARGET_DIR)/var/www/cgi-bin/fpv-wfb.cgi
	rm -f $(TARGET_DIR)/var/www/cgi-bin/p/fpv_common.cgi
	rm -f $(TARGET_DIR)/var/www/a/wfb.js
endef

# The two variant overrides this used to install are gone upstream: header_fpv
# was never included by any page and locale_fpv was never read, so an FPV build
# already renders the standard header and labels. Moving them is not just
# unnecessary now, it fails -- mv errors on a missing source even with -f.
#
# max is the third notification sender, added in OpenIPC/majestic-webui#516,
# and it belongs on this list for the same reason telegram does. The list it
# has to agree with is the one in that repo's sbin/updatewebui, which skips
# sbin/telegram, sbin/max and sbin/openwall on an fpv build: what an image
# ships and what an updatewebui run installs over it must be the same set, or
# a deploy puts back a file the image deliberately left out. ntfy is on
# neither list and is deliberately not added here -- changing that is a
# decision for the WebUI's list first.
define MAJESTIC_WEBUI_FPV_FIXUP
	rm -f $(TARGET_DIR)/usr/sbin/telegram
	rm -f $(TARGET_DIR)/usr/sbin/max
	rm -f $(TARGET_DIR)/usr/sbin/openwall
endef

# The SMS page and the two scripts behind it are for a board with a cellular
# modem, so they ship only where a device profile sets
# BR2_PACKAGE_MAJESTIC_WEBUI_SMS. The same six paths are listed in the WebUI's
# sbin/updatewebui, which installs them only where the image itself carries
# /usr/sbin/modem-at -- so this option is the one place the choice is made,
# for an image and for every WebUI update laid over it.
define MAJESTIC_WEBUI_SMS_FIXUP
	rm -f $(TARGET_DIR)/var/www/cgi-bin/sms.cgi
	rm -f $(TARGET_DIR)/var/www/cgi-bin/j/sms.cgi
	rm -f $(TARGET_DIR)/var/www/a/sms.js
	rm -f $(TARGET_DIR)/var/www/a/sms-pdu.js
	rm -f $(TARGET_DIR)/usr/sbin/sms-forward
	rm -f $(TARGET_DIR)/usr/sbin/modem-at
endef

# The PTZ controls are code for moving motors, and only an image with a motor
# driver can move one: gpio-motors, or the majestic-af plugin for the Pelco,
# MS41908 and gpiostep backends. BR2_PACKAGE_MAJESTIC_WEBUI_PTZ follows those
# two by default. The same eight paths are listed in the WebUI's
# sbin/updatewebui, which installs them only where the image carries
# /usr/bin/gpio-motors or /usr/lib/majestic-af.so.
#
# p/motor.cgi is emptied, never removed: live.cgi pulls it in with <%in %>,
# which haserl opens while parsing the page, before any shell around it runs,
# so a missing file is an error on the Live page whatever the camera's PTZ
# settings say. The WebUI reads an empty one as "no PTZ".
define MAJESTIC_WEBUI_PTZ_FIXUP
	rm -f $(TARGET_DIR)/var/www/a/preview-ptz.js
	rm -f $(TARGET_DIR)/var/www/a/ptz-stick.js
	rm -f $(TARGET_DIR)/var/www/a/ptz-centre.js
	rm -f $(TARGET_DIR)/var/www/a/ptz-follow.js
	rm -f $(TARGET_DIR)/var/www/a/ptz-speed.js
	rm -f $(TARGET_DIR)/var/www/a/af-state.js
	[ ! -e $(TARGET_DIR)/var/www/cgi-bin/p/motor.cgi ] || : >$(TARGET_DIR)/var/www/cgi-bin/p/motor.cgi
	rm -f $(TARGET_DIR)/var/www/cgi-bin/j/ptz.cgi
endef

# The Counting page reads what majestic's object detection counts, so it is of
# use only on an image with a detection model: BR2_PACKAGE_MAJESTIC_WEBUI_COUNTING
# follows xmnpu-models and cpu-models' person detector by default (the drum
# reader alone is no detection model). The same three paths are
# listed in the WebUI's sbin/updatewebui, which installs them only where the
# image carries the page itself (/var/www/cgi-bin/counting.cgi). The Settings
# page's line and zone editor is not part of it: it is drawn only where
# majestic has a counting section.
define MAJESTIC_WEBUI_COUNTING_FIXUP
	rm -f $(TARGET_DIR)/var/www/cgi-bin/counting.cgi
	rm -f $(TARGET_DIR)/var/www/a/counting.js
	rm -f $(TARGET_DIR)/var/www/a/counting-data.js
endef

define MAJESTIC_WEBUI_INSTALL_TARGET_CMDS
	$(MAJESTIC_WEBUI_INSTALL)
	$(MAJESTIC_WEBUI_$(VERSION)_FIXUP)
	$(if $(BR2_PACKAGE_MAJESTIC_WEBUI_SMS),,$(MAJESTIC_WEBUI_SMS_FIXUP))
	$(if $(BR2_PACKAGE_MAJESTIC_WEBUI_PTZ),,$(MAJESTIC_WEBUI_PTZ_FIXUP))
	$(if $(BR2_PACKAGE_MAJESTIC_WEBUI_COUNTING),,$(MAJESTIC_WEBUI_COUNTING_FIXUP))
	$(if $(BR2_PACKAGE_MAJESTIC_WEBUI_FPV),,$(MAJESTIC_WEBUI_FPV_PAGE_FIXUP))
endef

$(eval $(generic-package))
