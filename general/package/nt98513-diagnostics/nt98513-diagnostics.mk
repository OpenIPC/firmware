################################################################################
#
# nt98513-diagnostics
#
################################################################################

NT98513_DIAGNOSTICS_VERSION = c6fba9743e6b3924a3652d72fbcbdf564cbd33bb
NT98513_DIAGNOSTICS_SITE = $(call github,OpenIPC,ipctool,$(NT98513_DIAGNOSTICS_VERSION))
NT98513_DIAGNOSTICS_SUBDIR = tools/nt98513
NT98513_DIAGNOSTICS_LICENSE = MIT
NT98513_DIAGNOSTICS_LICENSE_FILES = tools/nt98513/LICENSE
NT98513_DIAGNOSTICS_SUPPORTS_IN_SOURCE_BUILD = NO

# Keep Buildroot's compiler wrapper/sysroot while satisfying the standalone
# project's Linux target check. Its fixed flags and compiler probe enforce
# the freestanding output ABI independently of the rootfs libc float ABI.
NT98513_DIAGNOSTICS_CONF_OPTS = -DCMAKE_SYSTEM_NAME=Linux
NT98513_DIAGNOSTICS_INSTALL_TARGET_OPTS = --component NT98513Diagnostics

$(eval $(cmake-package))
