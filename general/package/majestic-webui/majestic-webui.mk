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

# -f throughout: which files the WebUI ships is decided in another repo, so a
# fixup that hard-fails on a missing one turns any deletion there into a broken
# build here -- for every platform at once, and only visible on the next PR,
# because master has no push trigger.
#
# The WebUI renamed every page after the word its menu uses, so the FPV page is
# wfb.cgi now. Both spellings are listed and both must stay for a while: the
# dist tarball is a rolling release off that repo's master, so an image built
# today gets the new name and one rebuilt from an older tag gets the old one,
# and a standard build must not ship the page either way. j/locale_fpv.cgi and
# p/header_fpv.cgi went in OpenIPC/majestic-webui#141 and are dropped here.
define MAJESTIC_WEBUI_STANDARD_FIXUP
	rm -f $(TARGET_DIR)/var/www/cgi-bin/wfb.cgi
	rm -f $(TARGET_DIR)/var/www/cgi-bin/fpv-wfb.cgi
	rm -f $(TARGET_DIR)/var/www/cgi-bin/p/fpv_common.cgi
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

define MAJESTIC_WEBUI_INSTALL_TARGET_CMDS
	$(MAJESTIC_WEBUI_INSTALL)
	$(MAJESTIC_WEBUI_$(VERSION)_FIXUP)
endef

$(eval $(generic-package))
