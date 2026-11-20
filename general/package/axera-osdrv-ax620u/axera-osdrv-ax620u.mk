################################################################################
#
# axera-osdrv-ax620u
#
################################################################################

AXERA_OSDRV_AX620U_VERSION =
AXERA_OSDRV_AX620U_SITE =
AXERA_OSDRV_AX620U_LICENSE = proprietary

define AXERA_OSDRV_AX620U_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.125/axera
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.125/axera $(AXERA_OSDRV_AX620U_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(AXERA_OSDRV_AX620U_PKGDIR)/files/script/load_axera

	$(AXERA_OSDRV_AX620U_INSTALL_SENSOR)
	$(AXERA_OSDRV_AX620U_INSTALL_AI_ISP)
endef

# Sensor tuning from the stock firmware, for whichever streamer drives the
# ISP: the AX_ISP_LoadBinParams bins (one per scene) in /etc/sensors and the
# sensor library in /usr/lib/sensors. With BR2_OPENIPC_SNS_MODEL set, only
# that sensor's files are installed. See README.md for the stock paths.
AXERA_OSDRV_AX620U_SNS = $(if $(OPENIPC_SNS_MODEL),$(OPENIPC_SNS_MODEL),*)

AXERA_OSDRV_AX620U_SNS_BINS = $(wildcard $(AXERA_OSDRV_AX620U_PKGDIR)/files/sensor/configs/$(AXERA_OSDRV_AX620U_SNS)_*.bin)
AXERA_OSDRV_AX620U_SNS_LIBS = $(wildcard $(AXERA_OSDRV_AX620U_PKGDIR)/files/sensor/lib/libsns_$(AXERA_OSDRV_AX620U_SNS).so)

define AXERA_OSDRV_AX620U_INSTALL_SENSOR
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors $(TARGET_DIR)/usr/lib/sensors
	$(if $(AXERA_OSDRV_AX620U_SNS_BINS),$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(AXERA_OSDRV_AX620U_SNS_BINS))
	$(if $(AXERA_OSDRV_AX620U_SNS_LIBS),$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(AXERA_OSDRV_AX620U_SNS_LIBS))
endef

# AI-ISP (NPU denoise) models. The tuning bins and the sensor library name
# them by the stock absolute path /mnt/custom/ai_isp/<model>, so that path is
# a link to where they are installed.
define AXERA_OSDRV_AX620U_INSTALL_AI_ISP
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/ai_isp $(TARGET_DIR)/mnt/custom
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/ai_isp $(AXERA_OSDRV_AX620U_PKGDIR)/files/sensor/ai_isp/*
	ln -sfn /etc/sensors/ai_isp $(TARGET_DIR)/mnt/custom/ai_isp
endef

$(eval $(generic-package))
