################################################################################
#
# libjpeg-openipc
#
################################################################################

# 9f, not 9e, because 9f is what this package has actually been building. The
# SOURCE line below used to interpolate $(LIBJPEG_VERSION) -- buildroot's OWN
# variable, from package/libjpeg, which is 9f -- rather than this package's.
# general/external.mk includes every package .mk into one make invocation, so
# that name resolved to a value this package does not own, and the tree fetched
# jpegsrc.v9f.tar.gz into a directory called libjpeg-openipc-9e. Correcting the
# variable while leaving 9e here would have been a silent downgrade of shipped
# code, so the label is moved to the truth instead. Verified by sha256: what CI
# downloads matches buildroot's recorded hash for 9f exactly.
LIBJPEG_OPENIPC_VERSION = 9f
LIBJPEG_OPENIPC_SITE = http://www.ijg.org/files
LIBJPEG_OPENIPC_SOURCE = jpegsrc.v$(LIBJPEG_OPENIPC_VERSION).tar.gz

# www.ijg.org is one small host, and until this line the package had no second
# place to look: buildroot builds the BR2_BACKUP_SITE URL as
# <backup>/$(PKG)_DL_SUBDIR/<file>, DL_SUBDIR defaults to the package name, and
# sources.buildroot.net has no libjpeg-openipc/ directory -- only libjpeg/,
# named after the upstream package this one is a fork of. Both fallbacks
# therefore 404'd, so a single ijg.org outage was fatal: hi3516av100_ultimate
# burned all seven build attempts on it, 42m52s, on 2026-08-18.
#
# Pointing DL_SUBDIR at the directory that does hold this exact tarball gives
# the mirror back. Same idiom buildroot uses for its own forks and split
# packages -- LINUX_HEADERS_DL_SUBDIR = linux, HOST_LIBIBERTY_DL_SUBDIR =
# binutils. It also shares output/dl/libjpeg/ with buildroot's libjpeg, which is
# correct: it is the same tarball under the same name, and the .hash beside this
# file is checked either way (HASH_FILES is keyed on the package, not on
# DL_SUBDIR), so a mirror serving anything else still fails the download.
LIBJPEG_OPENIPC_DL_SUBDIR = libjpeg

LIBJPEG_OPENIPC_INSTALL_STAGING = YES
LIBJPEG_OPENIPC_LICENSE = IJG
LIBJPEG_OPENIPC_LICENSE_FILES = README

LIBJPEG_OPENIPC_CONF_OPTS = --disable-shared

define LIBJPEG_OPENIPC_REMOVE_BINARIES
	rm -f $(addprefix $(TARGET_DIR)/usr/bin/,cjpeg djpeg jpegtran rdjpgcom wrjpgcom)
endef

LIBJPEG_OPENIPC_POST_INSTALL_TARGET_HOOKS += LIBJPEG_OPENIPC_REMOVE_BINARIES

$(eval $(autotools-package))
$(eval $(host-autotools-package))
