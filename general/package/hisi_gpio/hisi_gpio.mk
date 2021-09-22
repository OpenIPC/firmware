################################################################################
#
# hisi_gpio
#
################################################################################

HISI_GPIO_LICENSE = MIT
HISI_GPIO_LICENSE_FILES = LICENSE

define HISI_GPIO_EXTRACT_CMDS
	cp $(BR2_EXTERNAL_HISILICON_PATH)/package/hisi_gpio/src/hisi_gpio_scanner.c $(@D)/
	cp $(BR2_EXTERNAL_HISILICON_PATH)/package/hisi_gpio/src/hisi_gpio_watcher.c $(@D)/
endef

define HISI_GPIO_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -Os -s hisi_gpio_scanner.c -o hisi_gpio_scanner)
	(cd $(@D); $(TARGET_CC) -Os -s hisi_gpio_watcher.c -o hisi_gpio_watcher)
endef

define HISI_GPIO_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/hisi_gpio_scanner $(TARGET_DIR)/usr/bin/hisi_gpio_scanner
	install -m 0755 -D $(@D)/hisi_gpio_watcher $(TARGET_DIR)/usr/bin/hisi_gpio_watcher
endef

$(eval $(generic-package))
