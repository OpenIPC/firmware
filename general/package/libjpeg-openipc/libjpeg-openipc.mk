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
