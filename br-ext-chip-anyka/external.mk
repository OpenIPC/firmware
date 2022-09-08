include $(BR2_EXTERNAL_ANYKA_PATH)/linux/linux-ext-anyka_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_ANYKA_PATH)/package/*/*.mk))
