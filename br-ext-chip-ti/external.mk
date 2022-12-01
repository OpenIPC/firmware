include $(BR2_EXTERNAL_TI_PATH)/linux/linux-ext-ti_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_TI_PATH)/package/*/*.mk))
