################################################################################
#
# majestic-af
#
################################################################################

MAJESTIC_AF_SITE = $(call github,openipc,majestic-af,$(MAJESTIC_AF_VERSION))
MAJESTIC_AF_VERSION = 7caebdc6289347a04cadbf48894600858f45ae69

MAJESTIC_AF_LICENSE = MIT
MAJESTIC_AF_LICENSE_FILES = LICENSE

# Out-of-core autofocus / PTZ engine for majestic: one shared object majestic
# dlopen()s at runtime. The HAL seams it calls (sdk_get_focus_value, ...) are
# left UNDEFINED here and resolve against the majestic executable at dlopen, so
# this links nothing from majestic — only its own vendored headers. majestic must
# be built with plugin-symbol export (WITH_PLUGINS_SUPPORT) for those to resolve;
# the plugin otherwise fails to load and majestic keeps working without it.
define MAJESTIC_AF_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -std=gnu99 -fPIC -shared \
		-DFILE_BASENAME=\"majestic-af\" \
		-I$(@D)/include \
		$(@D)/src/plugin.c $(@D)/src/engine.c $(@D)/src/af2.c \
		-o $(@D)/majestic-af.so
endef

define MAJESTIC_AF_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0644 -D $(@D)/majestic-af.so $(TARGET_DIR)/usr/lib/majestic-af.so
endef

$(eval $(generic-package))
