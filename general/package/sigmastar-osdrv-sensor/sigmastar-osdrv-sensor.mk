################################################################################
#
# sigmastar-osdrv-sensor
#
################################################################################

SIGMASTAR_OSDRV_SENSOR_LICENSE = MIT
SIGMASTAR_OSDRV_SENSOR_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_SENSOR_EXTRACT_CMDS
	cp -r $(SIGMASTAR_OSDRV_SENSOR_PKGDIR)/src/* $(@D)
endef

SIGMASTAR_OSDRV_SENSOR_MODULE_SUBDIRS = $(OPENIPC_SOC_FAMILY)
SIGMASTAR_OSDRV_SENSOR_MODULE_MAKE_OPTS = \
	INSTALL_MOD_DIR=sigmastar \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
