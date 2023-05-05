include $(BR2_EXTERNAL_GRAINMEDIA_PATH)/linux/linux-ext-grainmedia_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_GRAINMEDIA_PATH)/../general/package/*/*.mk))
