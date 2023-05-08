export EXTERNAL_FAMILY = $(shell cat $(BR2_CONFIG) | grep -oE -m1 "board.\w+" | cut -d "/" -f 2)
export EXTERNAL_RELEASE = $(shell cat $(BR2_CONFIG) | grep -oE "\w+_defconfig" | cut -d "_" -f 2)
export EXTERNAL_VENDOR = $(shell cat $(BR2_CONFIG) | grep -oE "br-ext-chip-\w+" | cut -d "-" -f 4)

BR2_EXTERNAL_VENDOR_PATH = $(BR2_EXTERNAL_GENERAL_PATH)/../br-ext-chip-$(EXTERNAL_VENDOR)
BR2_EXTERNAL_SCRIPTS_PATH = $(BR2_EXTERNAL_GENERAL_PATH)/../scripts

include $(BR2_EXTERNAL_GENERAL_PATH)/linux/linux-ext-vendor-patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_GENERAL_PATH)/package/*/*.mk))
