################################################################################
#
# goke-osdrv-gk710x
#
################################################################################

GOKE_OSDRV_GK710X_VERSION =
GOKE_OSDRV_GK710X_SITE =
GOKE_OSDRV_GK710X_LICENSE = MIT
GOKE_OSDRV_GK710X_LICENSE_FILES = LICENSE

CHIP := $(shell echo $(BOARD) | cut -d "_" -f 1)

ifeq ($(CHIP),gk7102)
	FIRMWARE=gk_fw_710x.bin
else ifeq ($(CHIP),gk7102s)
	FIRMWARE=gk_fw_710xs.bin
endif

define GOKE_OSDRV_GK710X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/sensor/config/*.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/sensor/config/sc1135.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/sensor/config/gc1034.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/sensor/fw/$(FIRMWARE)

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.4.43-Goke/goke
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.4.43-Goke/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.4.43-Goke/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/sensor/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/script/ircut_demo

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk710x/files/lib/*.so
endef

$(eval $(generic-package))
