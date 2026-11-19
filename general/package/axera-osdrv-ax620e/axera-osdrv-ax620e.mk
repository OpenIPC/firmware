################################################################################
#
# axera-osdrv-ax620e
#
################################################################################

AXERA_OSDRV_AX620E_VERSION =
AXERA_OSDRV_AX620E_SITE =
AXERA_OSDRV_AX620E_LICENSE = GPL-2.0 (open drivers), proprietary (ax_proton/ax_npu)

define AXERA_OSDRV_AX620E_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/5.15.73/axera
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.15.73/axera $(AXERA_OSDRV_AX620E_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(AXERA_OSDRV_AX620E_PKGDIR)/files/script/load_axera

	$(AXERA_OSDRV_AX620E_INSTALL_SENSOR)
	$(AXERA_OSDRV_AX620E_INSTALL_AI_ISP)
endef

# Sensor tuning from the stock firmware, for whichever streamer drives the
# ISP: the AX_ISP_LoadBinParams bins (one per scene) in /etc/sensors and the
# sensor library in /usr/lib/sensors. With BR2_OPENIPC_SNS_MODEL set, only
# that sensor's files are installed. See README.md for the stock paths.
AXERA_OSDRV_AX620E_SNS = $(if $(OPENIPC_SNS_MODEL),$(OPENIPC_SNS_MODEL),*)

AXERA_OSDRV_AX620E_SNS_BINS = $(wildcard $(AXERA_OSDRV_AX620E_PKGDIR)/files/sensor/configs/$(AXERA_OSDRV_AX620E_SNS)_*.bin)
AXERA_OSDRV_AX620E_SNS_LIBS = $(wildcard $(AXERA_OSDRV_AX620E_PKGDIR)/files/sensor/lib/libsns_$(AXERA_OSDRV_AX620E_SNS).so)

define AXERA_OSDRV_AX620E_INSTALL_SENSOR
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors $(TARGET_DIR)/usr/lib/sensors
	$(if $(AXERA_OSDRV_AX620E_SNS_BINS),$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(AXERA_OSDRV_AX620E_SNS_BINS))
	$(if $(AXERA_OSDRV_AX620E_SNS_LIBS),$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(AXERA_OSDRV_AX620E_SNS_LIBS))
endef

# AI-ISP (NPU denoise) models. The tuning bins and the sensor library name
# them by the stock absolute path /mnt/custom/ai_isp/<model>, so that path is
# a link to where they are installed.
# Off by default on this board: see Config.in.
ifeq ($(BR2_PACKAGE_AXERA_OSDRV_AX620E_AI_ISP),y)
define AXERA_OSDRV_AX620E_INSTALL_AI_ISP
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/ai_isp $(TARGET_DIR)/mnt/custom
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/ai_isp $(AXERA_OSDRV_AX620E_PKGDIR)/files/sensor/ai_isp/*
	ln -sfn /etc/sensors/ai_isp $(TARGET_DIR)/mnt/custom/ai_isp
endef
else
# Turning the option off in an existing build removes what it installed. At
# target-finalize, which re-runs on every build and works on the final target
# directory: this package's install step does not re-run on a config change.
define AXERA_OSDRV_AX620E_REMOVE_AI_ISP
	rm -rf $(TARGET_DIR)/etc/sensors/ai_isp $(TARGET_DIR)/mnt/custom
endef
AXERA_OSDRV_AX620E_TARGET_FINALIZE_HOOKS += AXERA_OSDRV_AX620E_REMOVE_AI_ISP
endif

$(eval $(generic-package))
