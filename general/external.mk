include $(BR2_EXTERNAL_GENERAL_PATH)/linux/linux-ext-vendor-patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_GENERAL_PATH)/package/*/*.mk))

EXTERNAL_VENDOR = $(shell echo $(BR2_DEFCONFIG) | grep -o "br-ext-chip-\w*")
BR2_EXTERNAL_VENDOR_PATH = $(BR2_EXTERNAL_GENERAL_PATH)/../$(EXTERNAL_VENDOR)
BR2_EXTERNAL_SCRIPTS_PATH = $(BR2_EXTERNAL_GENERAL_PATH)/../scripts
