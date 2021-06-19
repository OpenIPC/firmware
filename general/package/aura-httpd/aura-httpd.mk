################################################################################
#
# aura-httpd | updated 2021.06.19
#
################################################################################

AURA_HTTPD_VERSION = 251c0e110717cfbd25346161a41fa9aef7c801dd
AURA_HTTPD_SITE = $(call github,nekromant,aura-httpd,$(AURA_HTTPD_VERSION))
AURA_HTTPD_LICENSE = MIT
AURA_HTTPD_LICENSE_FILES = LICENSE

#AURA_HTTPD_CONF_OPTS += -DCROSS_COMPILE=?

AURA_HTTPD_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
