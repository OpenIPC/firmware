################################################################################
#
# playaudio
#
################################################################################

PLAYAUDIO_LICENSE = MIT
PLAYAUDIO_LICENSE_FILES = LICENSE

define PLAYAUDIO_EXTRACT_CMDS
	cp -avr ../general/package/playaudio/src/* $(@D)/
endef

PLAYAUDIO_MAKE_OPTS = \
	CC="$(TARGET_CC)"

define PLAYAUDIO_BUILD_CMDS
	$(MAKE) $(PLAYAUDIO_MAKE_OPTS) -C $(@D)
endef

define PLAYAUDIO_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/playaudio $(TARGET_DIR)/usr/bin/playaudio
endef

$(eval $(generic-package))
