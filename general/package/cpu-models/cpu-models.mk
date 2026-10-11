################################################################################
#
# cpu-models
#
################################################################################

# Built and published by that repository's release workflow, never by hand:
# each asset traces to the commit and run that built it, and the release
# checked the engine against the model's reference arithmetic first.
CPU_MODELS_VERSION = v1
CPU_MODELS_SITE = https://github.com/OpenIPC/cpu-models/releases/download/$(CPU_MODELS_VERSION)
CPU_MODELS_SOURCE = person.pdnet
# The thresholds behind majestic's object-detection presets for this model,
# read beside it.
CPU_MODELS_EXTRA_DOWNLOADS = person.json
CPU_MODELS_LICENSE = MIT

define CPU_MODELS_EXTRACT_CMDS
	cp $(addprefix $(CPU_MODELS_DL_DIR)/,$(CPU_MODELS_SOURCE) $(CPU_MODELS_EXTRA_DOWNLOADS)) $(@D)/
endef

define CPU_MODELS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/majestic
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/majestic \
		$(@D)/person.pdnet $(@D)/person.json
endef

$(eval $(generic-package))
