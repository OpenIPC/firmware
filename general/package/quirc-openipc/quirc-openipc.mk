################################################################################
#
# quirc-openipc
#
################################################################################

QUIRC_OPENIPC_VERSION = 33251ce2005855392666fd8c7436d8c309a127d7
QUIRC_OPENIPC_SITE = $(call github,cronyx,quirc,$(QUIRC_OPENIPC_VERSION))
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
endef

$(eval $(generic-package))
