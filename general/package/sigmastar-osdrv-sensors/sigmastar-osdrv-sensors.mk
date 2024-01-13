################################################################################
#
# sigmastar-osdrv-sensors
#
################################################################################

SIGMASTAR_OSDRV_SENSORS_SITE = https://github.com/openipc/sensors/archive
SIGMASTAR_OSDRV_SENSORS_SOURCE = master.tar.gz

SIGMASTAR_OSDRV_SENSORS_LICENSE = MIT
SIGMASTAR_OSDRV_SENSORS_LICENSE_FILES = LICENSE

SIGMASTAR_OSDRV_SENSORS_MODULE_SUBDIRS = $(OPENIPC_SOC_VENDOR)/sources/$(OPENIPC_SOC_FAMILY)
SIGMASTAR_OSDRV_SENSORS_MODULE_MAKE_OPTS = \
	SENSOR_VERSION=$(OPENIPC_SOC_FAMILY) \
	INSTALL_MOD_DIR=$(OPENIPC_SOC_VENDOR) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
