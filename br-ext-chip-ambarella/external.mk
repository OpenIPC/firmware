include $(BR2_EXTERNAL_AMBARELLA_PATH)/linux/linux-ext-ambarella_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_AMBARELLA_PATH)/package/*/*.mk))
