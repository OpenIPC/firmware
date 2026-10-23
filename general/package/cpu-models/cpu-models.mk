################################################################################
#
# cpu-models
#
################################################################################

# Built and published by that repository's release workflow, never by hand:
# each asset traces to the commit and run that built it, and the release
# checked the engine against the model's reference arithmetic first.
CPU_MODELS_VERSION = v2
CPU_MODELS_SITE = https://github.com/OpenIPC/cpu-models/releases/download/$(CPU_MODELS_VERSION)
CPU_MODELS_SOURCE = person.qnet
# The thresholds behind majestic's object-detection presets for this model,
# read beside it; and the drum model majestic's meter reading classifies
# each drum of a utility meter with.
CPU_MODELS_EXTRA_DOWNLOADS = person.json drum.qnet
CPU_MODELS_LICENSE = MIT

define CPU_MODELS_EXTRACT_CMDS
	cp $(addprefix $(CPU_MODELS_DL_DIR)/,$(CPU_MODELS_SOURCE) $(CPU_MODELS_EXTRA_DOWNLOADS)) $(@D)/
endef

# All three are fetched and hash-checked whichever are chosen: one release,
# one hash file.
CPU_MODELS_FILES = \
	$(if $(BR2_PACKAGE_CPU_MODELS_PERSON),person.qnet person.json) \
	$(if $(BR2_PACKAGE_CPU_MODELS_DRUM),drum.qnet)

define CPU_MODELS_INSTALL_TARGET_CMDS
	# v1's blob, which the engine no longer reads, and any model no longer
	# chosen, out of a reused target
	rm -f $(addprefix $(TARGET_DIR)/usr/share/majestic/,person.pdnet person.qnet person.json drum.qnet)
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/majestic
	$(if $(strip $(CPU_MODELS_FILES)),$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/majestic \
		$(addprefix $(@D)/,$(CPU_MODELS_FILES)))
endef

$(eval $(generic-package))
