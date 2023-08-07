################################################################################
#
# sigmastar-osdrv-sensor
#
################################################################################

SIGMASTAR_OSDRV_SENSOR_SITE = https://github.com/openipc/sensors/archive
SIGMASTAR_OSDRV_SENSOR_SOURCE = master.tar.gz

SIGMASTAR_OSDRV_SENSOR_LICENSE = MIT
SIGMASTAR_OSDRV_SENSOR_LICENSE_FILES = LICENSE

SIGMASTAR_OSDRV_SENSOR_MODULE_SUBDIRS = sigmastar/sources/$(OPENIPC_SOC_FAMILY)
SIGMASTAR_OSDRV_SENSOR_MODULE_MAKE_OPTS = \
	SENSOR_VERSION=$(OPENIPC_SOC_MODEL)-$(OPENIPC_SOC_FAMILY) \
	INSTALL_MOD_DIR=sigmastar \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
