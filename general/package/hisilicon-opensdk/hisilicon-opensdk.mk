################################################################################
#
# hisilicon-opensdk
#
################################################################################

HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_VERSION = ff20187b

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
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3520DV200),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3520dv200
endif
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV6XX),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3516cv6xx
endif
ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3519DV500),y)
HISILICON_OPENSDK_DEPENDENCIES += hisilicon-osdrv-hi3519dv500
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

# V5 (hi3516cv6xx) is encode-only — no VO, no HDMI, no TDE, no NNIE, no
# VDEC, no VEDU, no JPEGD, no GDC, no DIS, no MIPI_TX, no HIFB. These
# blobs simply don't exist in the CV610 SDK. The per-SoC .kbuild already
# omits the obj-m lines, but the shared Kbuild fallback path
# (kernel/Kbuild's `else` branch) gates on DISABLE_* macros — match those
# guards so any shared per-block Kbuild fragments that get pulled in via
# include() stay disabled.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv6xx)
HISILICON_OPENSDK_MODULE_MAKE_OPTS += DISABLE_VO=1 DISABLE_TDE=1
endif

ifeq ($(OPENIPC_SOC_FAMILY),hi3516ev200)
	HISILICON_OPENSDK_SDK_CODE = 0x3516E200
else ifeq ($(OPENIPC_SOC_FAMILY),gk7205v200)
	HISILICON_OPENSDK_SDK_CODE = 0x7205200
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv500)
	HISILICON_OPENSDK_SDK_CODE = 0x3516C500
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv6xx)
	HISILICON_OPENSDK_SDK_CODE = 0x3516C610
else ifeq ($(OPENIPC_SOC_FAMILY),hi3519dv500)
	HISILICON_OPENSDK_SDK_CODE = 0x3519D500
endif

# for userspace libraries
define HISILICON_OPENSDK_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) CHIPARCH=$(OPENIPC_SOC_FAMILY) SDK_CODE=$(HISILICON_OPENSDK_SDK_CODE) -C $(@D)/libraries all
endef

# hi3520dv200 mirror is kernel-only — no isp/sensor/mpi_neo userspace
# to compile yet. libraries/Makefile has no SUBDIRS filter for this
# CHIPARCH, so the default `make all` would try every subdir and fail.
# Override the build step to a no-op until the userspace mirror lands.
ifeq ($(OPENIPC_SOC_FAMILY),hi3520dv200)
define HISILICON_OPENSDK_BUILD_CMDS
	@:
endef
endif

# hi3516cv6xx (V5) builds the 6 sensor drivers from source. The MPP
# userspace libraries (libss_mpi_*.so, libvqe_*.so, libsvp_*.so etc.)
# don't have a source mirror yet and continue to ship prebuilt from
# hisilicon-osdrv-hi3516cv6xx. The default BUILD_CMDS runs
# `make -C libraries all` which the libraries/Makefile filters down
# to ./sensor/hi3516cv6xx/% for this CHIPARCH.

# Sensor install list per SoC family
HISILICON_OPENSDK_SENSORS_hi3516ev200 = \
	aptina_ar0237/libsns_ar0237 \
	bt656/libsns_bt656 \
	galaxycore_gc2053/libsns_gc2053 \
	galaxycore_gc2053_ForCar/libsns_gc2053_forcar \
	galaxycore_gc4023/libsns_gc4023 \
	galaxycore_gc4653_2L/libsns_gc4653_2l \
	galaxycore_gc5603/libsns_gc5603 \
	imagedesign_mis2008/libsns_mis2008 \
	omnivision_os02g10/libsns_os02g10 \
	omnivision_os05a/libsns_os05a \
	omnivision_ov2718/libsns_ov2718 \
	smart_sc2231/libsns_sc2231 \
	smart_sc2235/libsns_sc2235 \
	smart_sc223a/libsns_sc223a \
	smart_sc3235/libsns_sc3235 \
	smart_sc3335/libsns_sc3335 \
	smart_sc3336/libsns_sc3336 \
	smart_sc4236/libsns_sc4236 \
	smart_sc500ai/libsns_sc500ai \
	soi_f37/libsns_f37 \
	sony_imx290/libsns_imx290 \
	sony_imx307/libsns_imx307 \
	sony_imx307_2L/libsns_imx307_2l \
	sony_imx327/libsns_imx327 \
	sony_imx327_2L/libsns_imx327_2l \
	sony_imx335/libsns_imx335 \
	sony_imx335_2L/libsns_imx335_2l \
	superpix_sp2305/libsns_sp2305 \
	superpix_sp2308/libsns_sp2308

HISILICON_OPENSDK_SENSORS_gk7205v200 = $(HISILICON_OPENSDK_SENSORS_hi3516ev200)
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
HISILICON_OPENSDK_SENSORS_hi3516cv100 = \
	aptina_9m034/libsns_9m034 \
	aptina_ar0130/libsns_ar0130 \
	aptina_ar0330/libsns_ar0330 \
	aptina_mt9p006/libsns_mt9p006 \
	himax_1375/libsns_hm1375 \
	ov_9712/libsns_ov9712 \
	ov_9712+/libsns_ov9712_plus \
	pana34031/libsns_mn34031 \
	pana34041/libsns_mn34041 \
	pixelplus_3100k/libsns_po3100k \
	soi_h22/libsns_h22 \
	soi_jxh42/libsns_jxh42 \
	sony_icx692/libsns_icx692 \
	sony_imx104/libsns_imx104 \
	sony_imx122/libsns_imx122 \
	sony_imx138/libsns_imx138 \
	sony_imx236/libsns_imx236
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
HISILICON_OPENSDK_SENSORS_hi3516cv6xx = \
	galaxycore_gc4023/libsns_gc4023 \
	omnivision_os04d10/libsns_os04d10 \
	smart_sc431hai/libsns_sc431hai \
	smart_sc4336p/libsns_sc4336p \
	smart_sc450ai/libsns_sc450ai \
	smart_sc500ai/libsns_sc500ai
HISILICON_OPENSDK_SENSORS_hi3519dv500 = \
	gst_412c/libsns_gst412c \
	omnivision_os04a10/libsns_os04a10 \
	omnivision_os04a10_slave/libsns_os04a10_slave \
	omnivision_os08a20/libsns_os08a20 \
	smart_sc450ai/libsns_sc450ai \
	smart_sc850sl/libsns_sc850sl \
	sony_imx347_slave/libsns_imx347_slave \
	sony_imx515/libsns_imx515

HISILICON_OPENSDK_SENSORS = $(HISILICON_OPENSDK_SENSORS_$(OPENIPC_SOC_FAMILY))

# fpv variant on V4 (hi3516ev200 + gk7205v200): restrict to the high-fps
# Sony sensors only. FPV cameras are permanently paired with one sensor;
# the other 26 entries in HISILICON_OPENSDK_SENSORS_$(family) are dead
# weight on the tight 5120 KB NOR rootfs that fpv variants ship on.
#
# PR #2054 (2026-05-08) switched 30 sensor .so files from pre-built
# vendor blobs to source-built, adding ~+160 KB compressed to the rootfs
# squashfs and pushing hi3516ev200_fpv + hi3516ev300_fpv past the 5120
# KB cap. Bisect via OpenIPC/builder#94's firmware_ref input isolated
# #2054 as the cause — see kaeru
# hi3516ev200-ev300-fpv-rootfs-size-overflow-2026-05-24.
#
# Sony IMX307 and IMX335 are the only sensors with high-fps presets
# validated for fpv so far (PRs #2090, #2091, #2093, #2094). Other
# sensors can be re-enabled per user request once their high-fps modes
# are checked and the rootfs has room.
ifeq ($(OPENIPC_VARIANT),fpv)
ifneq ($(filter $(OPENIPC_SOC_FAMILY),hi3516ev200 gk7205v200),)
HISILICON_OPENSDK_SENSORS = \
	sony_imx307/libsns_imx307 \
	sony_imx307_2L/libsns_imx307_2l \
	sony_imx335/libsns_imx335 \
	sony_imx335_2L/libsns_imx335_2l
endif
endif

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
	# V3A OSAL shim — only meaningful on neo (kernel 7.0). On lite (3.18)
	# the module compiles as a no-op so it could ship there too, but
	# load_hisilicon would still try to insmod it; gate to neo only.
	[ "$(OPENIPC_VARIANT)" = "neo" ] && \
		$(INSTALL) -m 644 $(@D)/kernel/open_v3a_shim.ko \
			$(HISILICON_OPENSDK_KMOD_DST)/v3a_shim.ko || true
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
	# V2A OSAL shim — only meaningful on neo (kernel 7.0). Gate to neo.
	[ "$(OPENIPC_VARIANT)" = "neo" ] && \
		$(INSTALL) -m 644 $(@D)/kernel/open_v2a_shim.ko \
			$(HISILICON_OPENSDK_KMOD_DST)/v2a_shim.ko || true
endef

# For hi3516cv100: install opensdk .ko to hisilicon/ with vendor names.
# CV100 uses kernel 3.0.8 and hi3518_* blob naming (not hi3518e_*).
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv100)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
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
	# V1 OSAL shim — only meaningful on neo (kernel 7.0). On lite (3.0.8)
	# the module compiles as a no-op so it could ship there too, but
	# load_hisilicon would still try to insmod it; gate to neo only.
	[ "$(OPENIPC_VARIANT)" = "neo" ] && \
		$(INSTALL) -m 644 $(@D)/kernel/open_v1_shim.ko \
			$(HISILICON_OPENSDK_KMOD_DST)/v1_shim.ko || true
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
	# Frame-timestamp chrdev — open_isp.ko (renamed to hi3518e_isp.ko above)
	# imports openipc_frame_ts_push from this module since the openhisilicon
	# bump that wired cv200 ISP_ISR into the openipc_frame_ts chrdev path
	# (kernel/isp/arch/hi3516cv200/firmware/drv/isp.c). Without it,
	# `insmod hi3518e_isp.ko` fails with "unknown symbol" and the dependent
	# sensor_i2c / sensor_spi insmods cascade.
	$(INSTALL) -m 644 $(@D)/kernel/open_openipc_frame_ts.ko $(HISILICON_OPENSDK_KMOD_DST)/open_openipc_frame_ts.ko
	# V2 OSAL shim — only meaningful on neo (kernel 7.0). On lite (4.9)
	# the module compiles as a no-op so it could ship there too, but
	# load_hisilicon would still try to insmod it; gate to neo only.
	[ "$(OPENIPC_VARIANT)" = "neo" ] && \
		$(INSTALL) -m 644 $(@D)/kernel/open_v2_shim.ko \
			$(HISILICON_OPENSDK_KMOD_DST)/v2_shim.ko || true
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

# For gk7205v200: install opensdk .ko under /lib/modules/<kver>/goke/ with
# vendor names so the existing load_goke `insmod gk7205v200_<mod>.ko`
# calls pick up our open_*.ko build instead of the vendor blobs that
# goke-osdrv-gk7205v200 used to ship. Peripherals (open_osal, open_isp,
# open_mipi_rx, open_sys_config, open_sensor_i2c, open_sensor_spi,
# open_pwm, open_wdt, open_piris, open_hwrng, open_adc) are loaded via
# modprobe and live in /lib/modules/<kver>/extra/ via the default
# kernel-module install — they don't need a rename, modprobe finds them
# there.
else ifeq ($(OPENIPC_SOC_FAMILY),gk7205v200)
HISILICON_OPENSDK_KMOD_DST = $(TARGET_DIR)/lib/modules/$(HISILICON_OPENSDK_KVER)/goke
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	for mod in acodec adec aenc ai aio ao base chnl h264e h265e ive jpege \
		rc rgn sys vedu venc vgs vi vpss; do \
		$(INSTALL) -m 644 $(@D)/kernel/open_$${mod}.ko \
			$(HISILICON_OPENSDK_KMOD_DST)/gk7205v200_$${mod}.ko || exit 1; \
	done
endef

# For hi3516cv6xx: V5 — install opensdk .ko directly to hisilicon/ keeping
# the open_* names. load_hisilicon (rewritten) drives `modprobe open_*`.
# Sensor .so files built from source under libraries/sensor/hi3516cv6xx/
# get installed to /usr/lib/sensors/, overwriting any prebuilt vendor
# copies that hisilicon-osdrv-hi3516cv6xx may also have installed.
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv6xx)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	for ko in $(@D)/kernel/open_*.ko; do \
		[ -f $${ko} ] && $(INSTALL) -m 644 -t $(HISILICON_OPENSDK_KMOD_DST) $${ko} || true; \
	done
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
endef

# hi3519dv500: V5 aarch64 — same as cv6xx, install open_*.ko verbatim
# (load_hisilicon drives `modprobe open_*`) + the source-built sensor .so.
else ifeq ($(OPENIPC_SOC_FAMILY),hi3519dv500)
HISILICON_OPENSDK_KMOD_DST = $(HISILICON_OPENSDK_KMOD_BASE)
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	for ko in $(@D)/kernel/open_*.ko; do \
		[ -f $${ko} ] && $(INSTALL) -m 644 -t $(HISILICON_OPENSDK_KMOD_DST) $${ko} || true; \
	done
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach s,$(HISILICON_OPENSDK_SENSORS), \
		$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/$(s).so $(TARGET_DIR)/usr/lib/sensors ; \
	)
endef

else ifeq ($(OPENIPC_SOC_FAMILY),hi3520dv200)
# hi3520dv200: V2-era 4-channel analog DVR SoC. Kernel 3.0.8. No
# sensor blobs (NVP6114 analog video decoder kernel module is built
# from source instead — see open_nvp6134_ex.ko below).
#
# Vendor osdrv ships ~30 blobs in /lib/modules/3.0.8/hisilicon/. The
# initial source mirror only replaces the four that load_hisilicon
# actually insmods today — minimal blast radius for the first smoke
# test. The MPP video pipeline (hi3520D_base/sys/viu/vpss/vou/venc/…)
# stays as vendor blobs because vendor never released V2 MPP source.
HISILICON_OPENSDK_KMOD_DST = $(TARGET_DIR)/lib/modules/3.0.8/hisilicon
define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(HISILICON_OPENSDK_KMOD_DST)
	$(INSTALL) -m 644 $(@D)/kernel/open_mmz.ko          $(HISILICON_OPENSDK_KMOD_DST)/mmz.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_hi_user.ko       $(HISILICON_OPENSDK_KMOD_DST)/hiuser.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_gpioi2c.ko       $(HISILICON_OPENSDK_KMOD_DST)/gpioi2c.ko
	$(INSTALL) -m 644 $(@D)/kernel/open_nvp6134_ex.ko    $(HISILICON_OPENSDK_KMOD_DST)/nvp6134_ex.ko
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
ifneq ($(filter hi3516cv500 hi3516cv200 hi3516cv100 hi3516av100 hi3519v101 hi3516cv300 hi3520dv200 hi3516cv6xx hi3519dv500,$(OPENIPC_SOC_FAMILY)),)
define HISILICON_OPENSDK_FINALIZE_MODULES
	$(if $(BR2_PER_PACKAGE_DIRECTORIES),rsync -a $(PER_PACKAGE_DIR)/hisilicon-opensdk/target/lib/modules/ $(TARGET_DIR)/lib/modules/)
	rm -rf $(TARGET_DIR)/lib/modules/*/extra/open_*.ko
	$(LINUX_RUN_DEPMOD)
endef
HISILICON_OPENSDK_TARGET_FINALIZE_HOOKS += HISILICON_OPENSDK_FINALIZE_MODULES
endif

# For gk7205v200: peripherals (osal, isp, mipi_rx, sys_config, sensor_*,
# pwm, wdt, piris, hwrng, adc) stay in /lib/modules/<kver>/extra/ as
# open_*.ko — load_goke `modprobe open_<mod>` finds them there. The V4
# heavy modules are renamed-installed into /lib/modules/<kver>/goke/
# under their gk7205v200_*.ko names; the source-named copies the
# kernel-module default install drops in extra/ would otherwise double
# up disk usage and push the rootfs over the NOR partition. Wipe those.
ifeq ($(OPENIPC_SOC_FAMILY),gk7205v200)
define HISILICON_OPENSDK_FINALIZE_MODULES_GK7205V200
	$(if $(BR2_PER_PACKAGE_DIRECTORIES),rsync -a $(PER_PACKAGE_DIR)/hisilicon-opensdk/target/lib/modules/ $(TARGET_DIR)/lib/modules/)
	for mod in acodec adec aenc ai aio ao base chnl h264e h265e ive jpege \
		rc rgn sys vedu venc vgs vi vpss; do \
		rm -f $(TARGET_DIR)/lib/modules/*/extra/open_$${mod}.ko; \
	done
	rm -f $(TARGET_DIR)/lib/modules/*/extra/open_ive_neo.ko
	rm -f $(TARGET_DIR)/lib/modules/*/extra/open_adc.ko
	$(LINUX_RUN_DEPMOD)
endef
HISILICON_OPENSDK_TARGET_FINALIZE_HOOKS += HISILICON_OPENSDK_FINALIZE_MODULES_GK7205V200
endif

# Modules built for V4 ev200 by openhisilicon's Kbuild but never insmod'd
# on V4 — load_hisilicon doesn't reference them, majestic / userspace MPI
# doesn't call in, no init script loads them. Shipped as dead weight in
# /lib/modules/<kver>/extra/ on every ev200 lite rootfs. Skip the install
# so the bytes don't compete with majestic for the 5 MB NOR partition cap
# on boards like hi3518ev300_lite.
#
#   - open_ive_neo.ko (~12 KB)  — cv500-targeted experimental, also
#                                  compiles for V4; the .c grew from
#                                  66 KB → 130 KB source in recent
#                                  cv500 dispatch work.
#   - open_adc.ko (~6 KB)       — generic ADC driver, only relevant for
#                                  boards exposing GPIO/temp via sysfs;
#                                  not referenced anywhere on
#                                  ev300_lite / gk7205v200_lite nightly.
#
# Both still compile in CI (Kbuild unchanged) and cv500 keeps whatever
# subset its own kbuild includes. gk7205v200 handles the same cleanup
# inline in HISILICON_OPENSDK_FINALIZE_MODULES_GK7205V200 above — putting
# it in a separate post-hook would re-run the rsync from per-package and
# undo the goke/ rename cleanup, re-introducing 1.9 MB of heavy modules
# in extra/ on gk7205v200 lite builds.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516ev200)
define HISILICON_OPENSDK_SKIP_UNUSED_EV200
	$(if $(BR2_PER_PACKAGE_DIRECTORIES),rsync -a $(PER_PACKAGE_DIR)/hisilicon-opensdk/target/lib/modules/ $(TARGET_DIR)/lib/modules/)
	rm -f $(TARGET_DIR)/lib/modules/*/extra/open_ive_neo.ko
	rm -f $(TARGET_DIR)/lib/modules/*/extra/open_adc.ko
	$(LINUX_RUN_DEPMOD)
endef
HISILICON_OPENSDK_TARGET_FINALIZE_HOOKS += HISILICON_OPENSDK_SKIP_UNUSED_EV200
endif

# hi3518ev300_lite ships with a 5 MB rootfs partition that is already at
# 5120/5120 KB on master — no margin for new sensors. Trim SP2308 (.so + .ini)
# so this board variant keeps building; other ev200/gk7205v200 variants have
# room and keep the sensor.
#
# TODO: SC500AI and GC5603 are temporarily trimmed too to absorb a recent
# majestic binary growth (+41 KB) that pushed ev300_lite past the cap.
# This breaks sysupgrade for any deployed camera using those sensors —
# the openipc.org-hosted sysupgrade payload will no longer ship their
# .so files. Address via compaction in a follow-up PR (strip flags,
# unused-symbol removal, partition rebalance) and restore the sensors.
ifeq ($(OPENIPC_SOC_MODEL)/$(OPENIPC_VARIANT),hi3518ev300/lite)
define HISILICON_OPENSDK_TRIM_SP2308
	rm -f $(TARGET_DIR)/usr/lib/sensors/libsns_sp2308.so
	rm -f $(TARGET_DIR)/etc/sensors/sp2308_i2c_1080p.ini
	rm -f $(TARGET_DIR)/usr/lib/sensors/libsns_sc500ai.so
	rm -f $(TARGET_DIR)/usr/lib/sensors/libsns_gc5603.so
endef
HISILICON_OPENSDK_TARGET_FINALIZE_HOOKS += HISILICON_OPENSDK_TRIM_SP2308
endif

$(eval $(generic-package))
