################################################################################
#
# hisilicon-opensdk
#
################################################################################

HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_VERSION = d8b0941

HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

# Ensure opensdk installs after osdrv so our modules overwrite vendor ones
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV500),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3516cv500
endif

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	DISABLE_IST=1 \
	DISABLE_PM=1 \
	DISABLE_TDE=1 \
	DISABLE_VO=1 \
	CHIPARCH=$(OPENIPC_SOC_FAMILY)

ifeq ($(OPENIPC_SOC_FAMILY),hi3516ev200)
	HISILICON_OPENSDK_SDK_CODE = 0x3516E200
else ifeq ($(OPENIPC_SOC_FAMILY),gk7205v200)
	HISILICON_OPENSDK_SDK_CODE = 0x7205200
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv500)
	HISILICON_OPENSDK_SDK_CODE = 0x3516C500
endif

# for userspace libraries
define HISILICON_OPENSDK_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) CHIPARCH=$(OPENIPC_SOC_FAMILY) SDK_CODE=$(HISILICON_OPENSDK_SDK_CODE) -C $(@D)/libraries all
endef

# Sensor install list per SoC family
HISILICON_OPENSDK_SENSORS_hi3516ev200 = sony_imx335/libsns_imx335 sony_imx307/libsns_imx307 soi_h63/libsns_h63
HISILICON_OPENSDK_SENSORS_gk7205v200 = sony_imx335/libsns_imx335 sony_imx307/libsns_imx307 soi_h63/libsns_h63
HISILICON_OPENSDK_SENSORS_hi3516cv500 = sony_imx335/libsns_imx335 sony_imx307/libsns_imx307 sony_imx415/libsns_imx415

HISILICON_OPENSDK_SENSORS = $(HISILICON_OPENSDK_SENSORS_$(OPENIPC_SOC_FAMILY))

# For hi3516cv500: install opensdk .ko directly to hisilicon/ with vendor names,
# replacing the osdrv vendor modules. This avoids doubling disk usage.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv500)
HISILICON_OPENSDK_CHIP = hi3516cv500
HISILICON_OPENSDK_KMOD_DST = $(TARGET_DIR)/lib/modules/4.9.37/hisilicon
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
	$(INSTALL) -m 644 $(@D)/libraries/isp/libisp.so $(TARGET_DIR)/usr/lib/libisp.so
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	$(INSTALL) -m 644 $(@D)/kernel/open_osal.ko       $(HISILICON_OPENSDK_KMOD_DST)/hi_osal.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sys_config.ko  $(HISILICON_OPENSDK_KMOD_DST)/sys_config.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_mipi_rx.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi_mipi_rx.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_mipi_tx.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi_mipi_tx.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_cipher.ko      $(HISILICON_OPENSDK_KMOD_DST)/hi_cipher.ko
	for mod in base sys isp vi vpss venc vedu h264e h265e jpege jpegd rc rgn vgs ive chnl \
		tde vo hdmi vdec vfmw aio ai ao aenc adec acodec gdc dis nnie wdt; do \
		[ -f $(@D)/kernel/open_$${mod}.ko ] && \
			$(INSTALL) -m 644 $(@D)/kernel/open_$${mod}.ko \
				$(HISILICON_OPENSDK_KMOD_DST)/$(HISILICON_OPENSDK_CHIP)_$${mod}.ko || true; \
	done
endef

else
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
endef
endif

$(eval $(kernel-module))
$(eval $(generic-package))

# Must be registered AFTER $(eval $(kernel-module)) so our cleanup runs
# after the kernel-module hook that populates extra/.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv500)
define HISILICON_OPENSDK_CLEANUP_EXTRA
	rm -rf $(TARGET_DIR)/lib/modules/*/extra/open_*.ko
endef
HISILICON_OPENSDK_POST_INSTALL_TARGET_HOOKS += HISILICON_OPENSDK_CLEANUP_EXTRA
endif
