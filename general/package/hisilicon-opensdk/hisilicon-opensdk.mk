################################################################################
#
# hisilicon-opensdk
#
################################################################################

HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_VERSION = d3df3bb

HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

# Ensure opensdk installs after osdrv so our modules overwrite vendor ones
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV500),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3516cv500
endif
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV200),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3516cv200
endif
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV100),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3516cv100
endif
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516AV100),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3516av100
endif
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3519V101),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3519v101
endif
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV300),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3516cv300
endif

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	DISABLE_IST=1 \
	DISABLE_PM=1 \
	CHIPARCH=$(OPENIPC_SOC_FAMILY)

# CV500, CV200, and CV100 have TDE blobs; other platforms don't support TDE yet
ifneq ($(OPENIPC_SOC_FAMILY),hi3516cv500)
ifneq ($(OPENIPC_SOC_FAMILY),hi3516cv200)
ifneq ($(OPENIPC_SOC_FAMILY),hi3516cv100)
ifneq ($(OPENIPC_SOC_FAMILY),hi3516av100)
HISILICON_OPENSDK_MODULE_MAKE_OPTS += DISABLE_TDE=1
endif
endif
endif
HISILICON_OPENSDK_MODULE_MAKE_OPTS += DISABLE_VO=1
endif

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
HISILICON_OPENSDK_SENSORS_hi3516cv500 = \
	galaxycore_gc2053/libsns_gc2053 \
	omnivision_os04b10/libsns_os04b10 \
	omnivision_os05a/libsns_os05a \
	omnivision_os05a_2L/libsns_os05a_2l \
	omnivision_os08a10/libsns_os08a10 \
	omnivision_ov12870/libsns_ov12870 \
	omnivision_ov2775/libsns_ov2775 \
	omnivision_ov9284/libsns_ov9284 \
	panasonic_mn34220/libsns_mn34220 \
	primesensor_ps5260_2L/libsns_ps5260_2l \
	smart_sc4210/libsns_sc4210 \
	sony_imx307/libsns_imx307 \
	sony_imx307_2L/libsns_imx307_2l \
	sony_imx327/libsns_imx327 \
	sony_imx327_2L/libsns_imx327_2l \
	sony_imx335/libsns_imx335 \
	sony_imx335_ForCar/libsns_imx335_forcar \
	sony_imx390/libsns_imx390 \
	sony_imx415/libsns_imx415 \
	sony_imx458/libsns_imx458
HISILICON_OPENSDK_SENSORS_hi3519v101 = \
	omnivision_os05a/libsns_os05a \
	omnivision_os08a/libsns_os08a \
	omnivision_ov4689_slave/libsns_ov4689_slave \
	sony_imx226/libsns_imx226 \
	sony_imx274/libsns_imx274 \
	sony_imx290/libsns_imx290 \
	sony_imx326/libsns_imx326 \
	sony_imx327/libsns_imx327 \
	sony_imx385/libsns_imx385
HISILICON_OPENSDK_SENSORS_hi3516cv300 = \
	aptina_ar0237/libsns_ar0237 \
	jx_f22/libsns_jxf22 \
	omnivision_ov2718/libsns_ov2718 \
	omnivision_ov2718_2a/libsns_ov2718_2a \
	smartsens_sc2235/libsns_sc2235 \
	sony_imx290/libsns_imx290 \
	sony_imx307/libsns_imx307 \
	sony_imx323/libsns_imx323 \
	sony_imx385/libsns_imx385
HISILICON_OPENSDK_SENSORS_hi3516av100 = \
	aptina_ar0230/libsns_ar0230 \
	aptina_ar0237/libsns_ar0237 \
	aptina_ar0237_dc/libsns_ar0237_dc \
	aptina_ar0330/libsns_ar0330 \
	omnivision_ov4689/libsns_ov4689 \
	omnivision_ov5658/libsns_ov5658 \
	panasonic_mn34220/libsns_mn34220 \
	panasonic_mn34220_mipi/libsns_mn34220_mipi \
	sony_imx117/libsns_imx117 \
	sony_imx123/libsns_imx123 \
	sony_imx178/libsns_imx178 \
	sony_imx178_37M/libsns_imx178_37M \
	sony_imx185/libsns_imx185
HISILICON_OPENSDK_SENSORS_hi3516cv200 = \
	aptina_9m034/libsns_9m034 \
	aptina_ar0230/libsns_ar0230 \
	ar0130/libsns_ar0130 \
	omnivision_ov2710/libsns_ov2710 \
	omnivision_ov2718/libsns_ov2718 \
	omnivision_ov9712/libsns_ov9712 \
	omnivision_ov9732/libsns_ov9732 \
	omnivision_ov9750/libsns_ov9750 \
	omnivision_ov9752/libsns_ov9752 \
	panasonic_mn34222/libsns_mn34222 \
	smartsens_sc1135/libsns_sc1135 \
	smartsens_sc2035/libsns_sc2035 \
	smartsens_sc2135/libsns_sc2135 \
	smartsens_sc2235/libsns_sc2235 \
	soi_jxf22/libsns_jxf22 \
	soi_jxf23/libsns_jxf23 \
	soi_jxh42/libsns_jxh42 \
	sony_imx222/libsns_imx222

HISILICON_OPENSDK_SENSORS = $(HISILICON_OPENSDK_SENSORS_$(OPENIPC_SOC_FAMILY))

# Kernel version from the actual build — no hardcoded fallback.
# The kernel is always built before opensdk (dependency), so kernel.release exists.
HISILICON_OPENSDK_KVER = $(shell cat $(BUILD_DIR)/linux-custom/include/config/kernel.release 2>/dev/null)
HISILICON_OPENSDK_KMOD_BASE = $(TARGET_DIR)/lib/modules/$(HISILICON_OPENSDK_KVER)/hisilicon

# For hi3516cv300: install opensdk .ko to hisilicon/ with vendor names.
# V3 uses kernel 3.18.20, OSAL-based, hi3516cv300_* naming for both blobs and source drivers.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv300)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	$(INSTALL) -m 644 $(@D)/kernel/open_osal.ko          $(HISILICON_OPENSDK_KMOD_DST)/hi_osal.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sys_config.ko     $(HISILICON_OPENSDK_KMOD_DST)/sys_config.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_mipi_rx.ko        $(HISILICON_OPENSDK_KMOD_DST)/hi_mipi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_spi.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi_sensor_spi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_i2c.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_sensor.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_wdt.ko            $(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_wdt.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_ir.ko             $(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_ir.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_pwm.ko            $(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_pwm.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rtc.ko            $(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_rtc.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_acodec.ko         $(HISILICON_OPENSDK_KMOD_DST)/hi_acodec.ko
	for mod in base sys isp vpss venc vedu h264e h265e jpege rc vgs ive chnl \
		aio ai ao aenc adec; do \
		[ -f $(@D)/kernel/open_$${mod}.ko ] && \
			$(INSTALL) -m 644 $(@D)/kernel/open_$${mod}.ko \
				$(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_$${mod}.ko || true; \
	done
	$(INSTALL) -m 644 $(@D)/kernel/open_vi.ko      $(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_viu.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rgn.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi3516cv300_region.ko
	# HWRNG: install verbatim — load_hisilicon insmods it pre-sensor-probe.
	$(INSTALL) -m 644 $(@D)/kernel/open_hwrng.ko   $(HISILICON_OPENSDK_KMOD_DST)/open_hwrng.ko
endef

# For hi3519v101: install opensdk .ko to hisilicon/ with vendor names.
# V3A uses kernel 3.18.20, OSAL-based, hi3519v101_* blob naming, hi_* source naming.
else ifeq ($(OPENIPC_SOC_FAMILY),hi3519v101)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	$(INSTALL) -m 644 $(@D)/kernel/open_osal.ko          $(HISILICON_OPENSDK_KMOD_DST)/hi_osal.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sys_config.ko     $(HISILICON_OPENSDK_KMOD_DST)/sys_config.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_mipi_rx.ko        $(HISILICON_OPENSDK_KMOD_DST)/hi_mipi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_i2c.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi_sensor_i2c.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_spi.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi_sensor_spi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_hi_user.ko        $(HISILICON_OPENSDK_KMOD_DST)/hi_user.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_pwm.ko            $(HISILICON_OPENSDK_KMOD_DST)/hi_pwm.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_piris.ko          $(HISILICON_OPENSDK_KMOD_DST)/hi_piris.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_acodec.ko         $(HISILICON_OPENSDK_KMOD_DST)/hi_acodec.ko
	for mod in base sys isp vpss venc vedu h264e h265e jpege rc chnl vgs ive \
		aio ai ao aenc adec wdt ir rtc fisheye photo pciv pciv_fmw pm; do \
		[ -f $(@D)/kernel/open_$${mod}.ko ] && \
			$(INSTALL) -m 644 $(@D)/kernel/open_$${mod}.ko \
				$(HISILICON_OPENSDK_KMOD_DST)/hi3519v101_$${mod}.ko || true; \
	done
	$(INSTALL) -m 644 $(@D)/kernel/open_vi.ko      $(HISILICON_OPENSDK_KMOD_DST)/hi3519v101_viu.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rgn.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi3519v101_region.ko
	# HWRNG: install verbatim — load_hisilicon insmods it pre-sensor-probe.
	$(INSTALL) -m 644 $(@D)/kernel/open_hwrng.ko   $(HISILICON_OPENSDK_KMOD_DST)/open_hwrng.ko
endef

# For hi3516av100: install opensdk .ko to hisilicon/ with vendor names.
# AV100 (V2A) uses kernel 4.9.37 and hi3516a_* blob naming.
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516av100)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	$(INSTALL) -m 644 $(@D)/kernel/open_mmz.ko           $(HISILICON_OPENSDK_KMOD_DST)/mmz.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_himedia.ko       $(HISILICON_OPENSDK_KMOD_DST)/hi_media.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_mipi_rx.ko       $(HISILICON_OPENSDK_KMOD_DST)/hi_mipi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_i2c.ko    $(HISILICON_OPENSDK_KMOD_DST)/sensor_i2c.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_spi.ko    $(HISILICON_OPENSDK_KMOD_DST)/sensor_spi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_hi_user.ko       $(HISILICON_OPENSDK_KMOD_DST)/hiuser.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rtc.ko           $(HISILICON_OPENSDK_KMOD_DST)/hi_rtc.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_wdt.ko           $(HISILICON_OPENSDK_KMOD_DST)/wdt.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_pwm.ko           $(HISILICON_OPENSDK_KMOD_DST)/pwm.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_acodec.ko        $(HISILICON_OPENSDK_KMOD_DST)/acodec.ko
	for mod in base sys isp vpss venc h264e h265e jpege rc chnl vgs ive \
		tde aio ai ao aenc adec; do \
		[ -f $(@D)/kernel/open_$${mod}.ko ] && \
			$(INSTALL) -m 644 $(@D)/kernel/open_$${mod}.ko \
				$(HISILICON_OPENSDK_KMOD_DST)/hi3516a_$${mod}.ko || true; \
	done
	$(INSTALL) -m 644 $(@D)/kernel/open_vi.ko      $(HISILICON_OPENSDK_KMOD_DST)/hi3516a_viu.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rgn.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi3516a_region.ko
endef

# For hi3516cv100: install opensdk .ko to hisilicon/ with vendor names.
# CV100 uses kernel 3.0.8 and hi3518_* blob naming (not hi3518e_*).
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv100)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	$(INSTALL) -m 644 $(@D)/kernel/open_mmz.ko           $(HISILICON_OPENSDK_KMOD_DST)/mmz.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_i2c.ko    $(HISILICON_OPENSDK_KMOD_DST)/hi_i2c.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_hi_user.ko       $(HISILICON_OPENSDK_KMOD_DST)/hiuser.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_hidmac.ko        $(HISILICON_OPENSDK_KMOD_DST)/hidmac.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rtc.ko           $(HISILICON_OPENSDK_KMOD_DST)/hi_rtc.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_wdt.ko           $(HISILICON_OPENSDK_KMOD_DST)/wdt.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_pwm.ko           $(HISILICON_OPENSDK_KMOD_DST)/pwm.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_acodec.ko        $(HISILICON_OPENSDK_KMOD_DST)/acodec.ko
	for mod in base sys isp vpss venc h264e jpege rc chnl ive vda group sio \
		ai ao aenc adec; do \
		[ -f $(@D)/kernel/open_$${mod}.ko ] && \
			$(INSTALL) -m 644 $(@D)/kernel/open_$${mod}.ko \
				$(HISILICON_OPENSDK_KMOD_DST)/hi3518_$${mod}.ko || true; \
	done
	$(INSTALL) -m 644 $(@D)/kernel/open_vi.ko      $(HISILICON_OPENSDK_KMOD_DST)/hi3518_viu.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rgn.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi3518_region.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_vgs.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi3518_dsu.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_ssp_sony.ko    $(HISILICON_OPENSDK_KMOD_DST)/ssp_sony.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_ssp_pana.ko    $(HISILICON_OPENSDK_KMOD_DST)/ssp_pana.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_ssp_ad9020.ko  $(HISILICON_OPENSDK_KMOD_DST)/ssp_ad9020.ko
endef

# For hi3516cv200: install opensdk .ko to hisilicon/ with vendor names,
# replacing the osdrv vendor modules.
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv200)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
	# OV2710 self-detects MIPI vs DVP from its own .so basename via dladdr,
	# so one binary covers both PCB wirings (see openhisilicon
	# libraries/sensor/hi3516cv200/omnivision_ov2710/ov2710_sensor_ctl.c).
	ln -sf libsns_ov2710.so $(TARGET_DIR)/usr/lib/sensors/libsns_ov2710_mipi.so
	ln -sf libsns_ov2710.so $(TARGET_DIR)/usr/lib/sensors/libsns_ov2710_dc.so
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	$(INSTALL) -m 644 $(@D)/kernel/open_mmz.ko          $(HISILICON_OPENSDK_KMOD_DST)/mmz.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_himedia.ko       $(HISILICON_OPENSDK_KMOD_DST)/hi_media.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sys_config.ko    $(HISILICON_OPENSDK_KMOD_DST)/sys_config.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_mipi_rx.ko       $(HISILICON_OPENSDK_KMOD_DST)/hi_mipi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_pwm.ko           $(HISILICON_OPENSDK_KMOD_DST)/pwm.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_piris.ko         $(HISILICON_OPENSDK_KMOD_DST)/piris.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_i2c.ko    $(HISILICON_OPENSDK_KMOD_DST)/sensor_i2c.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_sensor_spi.ko    $(HISILICON_OPENSDK_KMOD_DST)/sensor_spi.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_hi_user.ko       $(HISILICON_OPENSDK_KMOD_DST)/hiuser.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_ir.ko            $(HISILICON_OPENSDK_KMOD_DST)/ir.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rtc.ko           $(HISILICON_OPENSDK_KMOD_DST)/hi_rtc.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_wdt.ko           $(HISILICON_OPENSDK_KMOD_DST)/wdt.ko
	for mod in base sys isp vpss venc h264e jpege rc chnl vgs ive \
		tde aio ai ao aenc adec; do \
		[ -f $(@D)/kernel/open_$${mod}.ko ] && \
			$(INSTALL) -m 644 $(@D)/kernel/open_$${mod}.ko \
				$(HISILICON_OPENSDK_KMOD_DST)/hi3518e_$${mod}.ko || true; \
	done
	$(INSTALL) -m 644 $(@D)/kernel/open_vi.ko      $(HISILICON_OPENSDK_KMOD_DST)/hi3518e_viu.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_rgn.ko     $(HISILICON_OPENSDK_KMOD_DST)/hi3518e_region.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_acodec.ko  $(HISILICON_OPENSDK_KMOD_DST)/acodec.ko
	# HWRNG: install verbatim — load_hisilicon insmods it pre-sensor-probe.
	$(INSTALL) -m 644 $(@D)/kernel/open_hwrng.ko   $(HISILICON_OPENSDK_KMOD_DST)/open_hwrng.ko
endef

# For hi3516cv500: install opensdk .ko directly to hisilicon/ keeping the
# upstream open_* names. Aligns module naming with hi3516ev200/ev300 so
# load_hisilicon can drive everything through `modprobe open_*` instead of
# `insmod` with vendor-renamed filenames. The legacy hi_*/hi3516cv500_*
# rename was a holdover from when osdrv shipped prebuilt vendor blobs.
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv500)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
	$(INSTALL) -m 644 $(@D)/libraries/isp/libisp.so $(TARGET_DIR)/usr/lib/libisp.so
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	for ko in $(@D)/kernel/open_*.ko; do \
		$(INSTALL) -m 644 -t $(HISILICON_OPENSDK_KMOD_DST) $${ko}; \
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

# Run as a target-finalize hook so it operates on the *merged* $(TARGET_DIR),
# which is the authoritative tree in both per-package and non-per-package modes.
#
# Three things happen here:
#
# 1. (per-package mode only) Re-apply our lib/modules/ onto the merged target.
#    target-finalize merges per-package/*/target/ in $(sort $(PACKAGES)) order,
#    which is alphabetical, not dependency order. That lets hisilicon-osdrv-*
#    clobber the hisilicon/<vendor>.ko files we install here, even though
#    HISILICON_OPENSDK_DEPENDENCIES sequences us after osdrv during build.
#
# 2. Wipe the source-named open_*.ko copies the kernel-module install leaves
#    in extra/. We rename them to vendor names in INSTALL_TARGET_CMDS; the
#    originals would otherwise be registered in modules.dep by depmod.
#
# 3. Re-run depmod. LINUX_RUN_DEPMOD is registered earlier in
#    TARGET_FINALIZE_HOOKS (linux package is processed before this one), so
#    it has already executed by the time we get here — we need a second pass
#    so modules.dep reflects the post-cleanup state.
ifneq ($(filter hi3516cv500 hi3516cv200 hi3516cv100 hi3516av100 hi3519v101 hi3516cv300,$(OPENIPC_SOC_FAMILY)),)
define HISILICON_OPENSDK_FINALIZE_MODULES
	$(if $(BR2_PER_PACKAGE_DIRECTORIES),rsync -a $(PER_PACKAGE_DIR)/hisilicon-opensdk/target/lib/modules/ $(TARGET_DIR)/lib/modules/)
	rm -rf $(TARGET_DIR)/lib/modules/*/extra/open_*.ko
	$(LINUX_RUN_DEPMOD)
endef
HISILICON_OPENSDK_TARGET_FINALIZE_HOOKS += HISILICON_OPENSDK_FINALIZE_MODULES
endif

$(eval $(generic-package))
