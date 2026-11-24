################################################################################
#
# sgsnpu-models
#
################################################################################

# Built and published by that repository's release workflow, never by hand:
# each asset traces to the commit and run that compiled it.
SGSNPU_MODELS_VERSION = v1
SGSNPU_MODELS_SITE = https://github.com/OpenIPC/sgsnpu-models/releases/download/$(SGSNPU_MODELS_VERSION)
SGSNPU_MODELS_SOURCE = yolov8n.img
# The model's <name>.json is what majestic's object-detection presets mean
# for it, measured by the same release; majestic reads it beside the model.
SGSNPU_MODELS_EXTRA_DOWNLOADS = yolov8n.json
SGSNPU_MODELS_LICENSE = AGPL-3.0

define SGSNPU_MODELS_EXTRACT_CMDS
	cp $(addprefix $(SGSNPU_MODELS_DL_DIR)/,$(SGSNPU_MODELS_SOURCE) $(SGSNPU_MODELS_EXTRA_DOWNLOADS)) $(@D)/
endef

define SGSNPU_MODELS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/majestic/npu
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/majestic/npu \
		$(@D)/yolov8n.img $(@D)/yolov8n.json
endef

$(eval $(generic-package))
