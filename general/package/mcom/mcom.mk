################################################################################
#
# mcom
#
################################################################################

BASE_PATH = /home/pavel

define MCOM_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(BASE_PATH)/openipc-firmware/general/package/mcom/files/socat $(TARGET_DIR)/usr/bin/socat
    $(INSTALL) -D -m 0755 $(BASE_PATH)/openipc-firmware/general/package/mcom/files/microcomV2 $(TARGET_DIR)/usr/bin/mcom
endef

$(eval $(generic-package))
