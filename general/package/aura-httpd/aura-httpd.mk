################################################################################
#
# aura-httpd
#
################################################################################

AURA_HTTPD_SITE = $(call github,nekromant,aura-httpd,$(AURA_HTTPD_VERSION))
AURA_HTTPD_VERSION = master

AURA_HTTPD_LICENSE = MIT
AURA_HTTPD_LICENSE_FILES = LICENSE
AURA_HTTPD_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
