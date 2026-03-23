################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# webface — cookie-based session authentication web UI
#
################################################################################

WEBFACE_LICENSE = MIT
WEBFACE_LICENSE_FILES = LICENSE

define WEBFACE_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/var/www
	cp -rv $(WEBFACE_PKGDIR)/files/www/* $(TARGET_DIR)/var/www
endef

$(eval $(generic-package))

