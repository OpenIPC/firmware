################################################################################
#
# majestic-plugins
#
################################################################################

define MAJESTIC_PLUGINS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) TARGET=$(OPENIPC_SOC_VENDOR) -C $(BASE_DIR)/../majestic-plugins -B
endef

$(eval $(generic-package))
