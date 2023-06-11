################################################################################
#
# aura-httpd | updated 2021.06.19
#
################################################################################

AURA_HTTPD_SITE_METHOD = git
AURA_HTTPD_SITE = https://github.com/nekromant/aura-httpd
AURA_HTTPD_VERSION = $(shell git ls-remote $(AURA_HTTPD_SITE) HEAD | head -1 | awk '{ print $$1 }')

AURA_HTTPD_LICENSE = MIT
AURA_HTTPD_LICENSE_FILES = LICENSE

#AURA_HTTPD_CONF_OPTS += -DCROSS_COMPILE=?

AURA_HTTPD_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
