################################################################################
#
# mcom
#
################################################################################


define MCOM_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 /home/pavel/openipc-firmware/general/package/mcom/microcomV2 $(TARGET_DIR)/usr/bin/mcom
endef

$(eval $(generic-package))
