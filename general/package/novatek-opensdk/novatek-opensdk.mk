################################################################################
#
# novatek-opensdk
#
################################################################################

NOVATEK_OPENSDK_SITE = $(call github,openipc,opennovatek,$(NOVATEK_OPENSDK_VERSION))
NOVATEK_OPENSDK_VERSION = a289f2db325437c99b1eb9e9805005f86f84ec24

NOVATEK_OPENSDK_LICENSE = GPL-3.0
NOVATEK_OPENSDK_LICENSE_FILES = LICENSE

# The sensor drivers are kernel modules, built against this kernel. After the
# vendor package too, so a module installed here wins over a prebuilt one.
NOVATEK_OPENSDK_DEPENDENCIES = linux
ifeq ($(BR2_PACKAGE_NOVATEK_OSDRV_NT9856X),y)
NOVATEK_OPENSDK_DEPENDENCIES += novatek-osdrv-nt9856x
endif

# opennovatek names the family by its die, na51089 (NT98560/1/2/6).
NOVATEK_OPENSDK_CHIPARCH_nt9856x = na51089
NOVATEK_OPENSDK_CHIPARCH = $(NOVATEK_OPENSDK_CHIPARCH_$(OPENIPC_SOC_FAMILY))
NOVATEK_OPENSDK_SENSOR_DIR = $(@D)/libraries/sensor/$(NOVATEK_OPENSDK_CHIPARCH)

# Sensors built for each family, by directory under libraries/sensor/: the
# ones load_novatek can start. Each builds nvt_sen_<model>.ko.
NOVATEK_OPENSDK_SENSORS_nt9856x = sony_imx415 smart_sc401ai smart_sc500ai smart_sc501ai
NOVATEK_OPENSDK_SENSORS = $(NOVATEK_OPENSDK_SENSORS_$(OPENIPC_SOC_FAMILY))

# Plain kbuild calls rather than the kernel-module infrastructure: that would
# also install every module under lib/modules/<ver>/extra, a second copy of
# each on flash, while load_novatek insmods them by name from novatek/.
define NOVATEK_OPENSDK_BUILD_CMDS
	$(foreach s,$(NOVATEK_OPENSDK_SENSORS),
		$(LINUX_MAKE_ENV) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) \
			M=$(NOVATEK_OPENSDK_SENSOR_DIR)/$(s) modules
	)
endef

# Where load_novatek looks. Stripped as on hisilicon: target-finalize leaves
# *.ko alone.
#
# Unlike the open_* modules of the other opensdk packages, these keep the
# vendor's nvt_sen_<model>.ko names: load_novatek insmods nvt_sen_$SENSOR.ko,
# and each driver registers with HDAL as "nvt_sen_<model>" whatever its file
# is called, so the same names let it drop in without touching the loader.
# They are built from source all the same (OpenIPC/opennovatek).
NOVATEK_OPENSDK_KMOD_DST = $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/novatek

define NOVATEK_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(NOVATEK_OPENSDK_KMOD_DST)
	$(foreach s,$(NOVATEK_OPENSDK_SENSORS),
		for ko in $(NOVATEK_OPENSDK_SENSOR_DIR)/$(s)/nvt_sen_*.ko; do \
			$(TARGET_STRIP) --strip-unneeded --remove-section=.comment --remove-section=.note \
				-o $(NOVATEK_OPENSDK_KMOD_DST)/$$(basename $$ko) $$ko || exit 1; \
			chmod 644 $(NOVATEK_OPENSDK_KMOD_DST)/$$(basename $$ko); \
		done
	)
endef

$(eval $(generic-package))
