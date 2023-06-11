################################################################################
#
# f2fs-tools-openipc
#
################################################################################

F2FS_TOOLS_OPENIPC_VERSION = 1.13.0
F2FS_TOOLS_OPENIPC_SITE = https://git.kernel.org/pub/scm/linux/kernel/git/jaegeuk/f2fs-tools.git/snapshot

F2FS_TOOLS_OPENIPC_CONF_ENV = ac_cv_file__git=no
F2FS_TOOLS_OPENIPC_DEPENDENCIES = host-pkgconf
# GIT version, shipped without configure
F2FS_TOOLS_OPENIPC_AUTORECONF = YES
F2FS_TOOLS_OPENIPC_INSTALL_STAGING = YES
F2FS_TOOLS_OPENIPC_LICENSE = GPL-2.0
F2FS_TOOLS_OPENIPC_LICENSE_FILES = COPYING

F2FS_TOOLS_OPENIPC_CONF_OPTS = \
	--without-selinux \
	--without-blkid

$(eval $(autotools-package))
$(eval $(host-autotools-package))
