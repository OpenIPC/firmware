################################################################################
#
# quirc-openipc
#
################################################################################

QUIRC_OPENIPC_SITE_METHOD = git
QUIRC_OPENIPC_SITE = https://github.com/openipc/quirc
QUIRC_OPENIPC_VERSION = $(call EXTERNAL_SHA,$(QUIRC_OPENIPC_SITE),HEAD)

QUIRC_OPENIPC_DEPENDENCIES += libjpeg
QUIRC_OPENIPC_LICENSE = ISC
QUIRC_OPENIPC_LICENSE_FILES = LICENSE.txt

QUIRC_OPENIPC_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	AR="$(TARGET_AR)"

define QUIRC_OPENIPC_BUILD_CMDS
	$(MAKE) $(QUIRC_OPENIPC_MAKE_OPTS) -C $(@D) all
endef

define QUIRC_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/qrscan $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(QUIRC_OPENIPC_PKGDIR)files/qrparse
endef

$(eval $(generic-package))
