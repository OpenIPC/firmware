################################################################################
#
# aura-httpd | updated 2021.06.19
#
################################################################################

AURA_HTTPD_SITE_METHOD = git
AURA_HTTPD_SITE = https://github.com/nekromant/aura-httpd
AURA_HTTPD_VERSION = $(call EXTERNAL_SHA,$(AURA_HTTPD_SITE),HEAD)

AURA_HTTPD_LICENSE = MIT
AURA_HTTPD_LICENSE_FILES = LICENSE

#AURA_HTTPD_CONF_OPTS += -DCROSS_COMPILE=?

AURA_HTTPD_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
