################################################################################
#
# hisi-gpio | updated 2022.08.10

#
################################################################################

HISI_GPIO_LICENSE = MIT
HISI_GPIO_LICENSE_FILES = LICENSE

define HISI_GPIO_EXTRACT_CMDS
	cp ../general/package/hisi-gpio/src/hisi-gpio-scanner.c $(@D)/
	cp ../general/package/hisi-gpio/src/hisi-gpio-watcher.c $(@D)/
endef

define HISI_GPIO_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -Os -s hisi-gpio-scanner.c -o hisi-gpio-scanner)
	(cd $(@D); $(TARGET_CC) -Os -s hisi-gpio-watcher.c -o hisi-gpio-watcher)
endef

define HISI_GPIO_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/hisi-gpio-scanner $(TARGET_DIR)/usr/bin/hisi-gpio-scanner
	install -m 0755 -D $(@D)/hisi-gpio-watcher $(TARGET_DIR)/usr/bin/hisi-gpio-watcher
endef

$(eval $(generic-package))
