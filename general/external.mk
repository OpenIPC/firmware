EXTERNAL_FAMILY = $(shell grep -oE "board.\w+" $(BR2_CONFIG) | cut -d "/" -f 2)
EXTERNAL_RELEASE = $(shell grep -oE "\w+_defconfig" $(BR2_CONFIG) | cut -d "_" -f 2)
EXTERNAL_VENDOR = $(shell grep -oE "br-ext-chip-\w+" $(BR2_CONFIG))

export BR2_EXTERNAL_VENDOR_PATH = $(BR2_EXTERNAL_GENERAL_PATH)/../$(EXTERNAL_VENDOR)
export BR2_EXTERNAL_SCRIPTS_PATH = $(BR2_EXTERNAL_GENERAL_PATH)/scripts

include $(BR2_EXTERNAL_GENERAL_PATH)/linux/linux-ext-vendor-patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_GENERAL_PATH)/package/*/*.mk))
