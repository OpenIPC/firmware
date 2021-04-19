################################################################################
#
# hisi_gpio_scanner
#
################################################################################

HISI_GPIO_SCANNER_LICENSE = MIT
HISI_GPIO_SCANNER_LICENSE_FILES = LICENSE

define HISI_GPIO_SCANNER_EXTRACT_CMDS
	cp $(BR2_EXTERNAL_HISILICON_PATH)/package/hisi_gpio_scanner/src/hisi_gpio_scanner.c $(@D)/
endef

define HISI_GPIO_SCANNER_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -Os -s hisi_gpio_scanner.c -o hisi_gpio_scanner)
endef

define HISI_GPIO_SCANNER_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/hisi_gpio_scanner $(TARGET_DIR)/usr/bin/hisi_gpio_scanner
endef

$(eval $(generic-package))
