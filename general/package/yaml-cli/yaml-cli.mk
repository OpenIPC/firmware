################################################################################
#
# yaml-cli
#
################################################################################

YAML_CLI_SITE_METHOD = git
YAML_CLI_SITE = https://github.com/openipc/yaml-cli
YAML_CLI_VERSION = $(shell git ls-remote $(YAML_CLI_SITE) HEAD | head -1 | awk '{ print $$1 }')

YAML_CLI_LICENSE = MIT
YAML_CLI_LICENSE_FILES = LICENSE

YAML_CLI_DEPENDENCIES = libyaml
YAML_CLI_CONF_OPTS += -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_BINDIR="/usr/bin"

YAML_CLI_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
