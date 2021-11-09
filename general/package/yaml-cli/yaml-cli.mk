################################################################################
#
# yaml-cli | updated 2021.11.09
#
################################################################################

YAML_CLI_VERSION = 0.0.4
YAML_CLI_SITE = $(call github,openipc,yaml-cli,v$(YAML_CLI_VERSION))
YAML_CLI_LICENSE = MIT
YAML_CLI_LICENSE_FILES = LICENSE

YAML_CLI_DEPENDENCIES = libyaml
YAML_CLI_CONF_OPTS += -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_BINDIR="/usr/bin"

YAML_CLI_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
