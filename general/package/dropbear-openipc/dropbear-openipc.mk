################################################################################
#
# dropbear
#
################################################################################

DROPBEAR_OPENIPC_VERSION = 2022.82
DROPBEAR_OPENIPC_SITE = http://sources.buildroot.net/dropbear
DROPBEAR_OPENIPC_SOURCE = dropbear-$(DROPBEAR_OPENIPC_VERSION).tar.bz2

DROPBEAR_OPENIPC_LICENSE = MIT, BSD-2-Clause, Public domain
DROPBEAR_OPENIPC_LICENSE_FILES = LICENSE
DROPBEAR_OPENIPC_TARGET_BINS = dropbearkey dropbearconvert scp
DROPBEAR_OPENIPC_PROGRAMS = dropbear $(DROPBEAR_OPENIPC_TARGET_BINS)
DROPBEAR_OPENIPC_CPE_ID_VENDOR = dropbear_ssh_project
DROPBEAR_OPENIPC_CPE_ID_PRODUCT = dropbear_ssh

# Disable hardening flags added by dropbear configure.ac, and let
# Buildroot add them when the relevant options are enabled. This
# prevents dropbear from using SSP support when not available.
DROPBEAR_OPENIPC_CONF_OPTS = --disable-harden

ifeq ($(BR2_PACKAGE_DROPBEAR_OPENIPC_CLIENT),y)
# Build dbclient, and create a convenience symlink named ssh
DROPBEAR_OPENIPC_PROGRAMS += dbclient
DROPBEAR_OPENIPC_TARGET_BINS += dbclient ssh
endif

DROPBEAR_OPENIPC_MAKE = \
	$(MAKE) MULTI=1 SCPPROGRESS=1 \
	PROGRAMS="$(DROPBEAR_OPENIPC_PROGRAMS)"

# With BR2_SHARED_STATIC_LIBS=y the generic infrastructure adds a
# --enable-static flags causing dropbear to be built as a static
# binary. Adding a --disable-static reverts this
ifeq ($(BR2_SHARED_STATIC_LIBS),y)
DROPBEAR_OPENIPC_CONF_OPTS += --disable-static
endif

ifeq ($(BR2_PACKAGE_LINUX_PAM),y)
define DROPBEAR_OPENIPC_SVR_PAM_AUTH
	echo '#define DROPBEAR_SVR_PASSWORD_AUTH 0'     >> $(@D)/localoptions.h
	echo '#define DROPBEAR_SVR_PAM_AUTH 1'          >> $(@D)/localoptions.h
endef
define DROPBEAR_OPENIPC_INSTALL_PAM_CONF
	$(INSTALL) -D -m 644 package/dropbear/etc-pam.d-sshd $(TARGET_DIR)/etc/pam.d/sshd
endef
DROPBEAR_OPENIPC_DEPENDENCIES += linux-pam
DROPBEAR_OPENIPC_CONF_OPTS += --enable-pam
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_SVR_PAM_AUTH
DROPBEAR_OPENIPC_POST_INSTALL_TARGET_HOOKS += DROPBEAR_OPENIPC_INSTALL_PAM_CONF
else
# Ensure that dropbear doesn't use crypt() when it's not available
define DROPBEAR_OPENIPC_SVR_PASSWORD_AUTH
	echo '#if !HAVE_CRYPT'                          >> $(@D)/localoptions.h
	echo '#define DROPBEAR_SVR_PASSWORD_AUTH 0'     >> $(@D)/localoptions.h
	echo '#endif'                                   >> $(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_SVR_PASSWORD_AUTH
endif

ifeq ($(BR2_PACKAGE_DROPBEAR_OPENIPC_LEGACY_CRYPTO),y)
define DROPBEAR_OPENIPC_ENABLE_LEGACY_CRYPTO
	echo '#define DROPBEAR_3DES 1'                  >> $(@D)/localoptions.h
	echo '#define DROPBEAR_ENABLE_CBC_MODE 1'       >> $(@D)/localoptions.h
	echo '#define DROPBEAR_SHA1_96_HMAC 1'          >> $(@D)/localoptions.h
	echo '#define DROPBEAR_DH_GROUP1 1'             >> $(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_ENABLE_LEGACY_CRYPTO
else
define DROPBEAR_OPENIPC_DISABLE_LEGACY_CRYPTO
	echo '#define DROPBEAR_DSS 0'                   >> $(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_DISABLE_LEGACY_CRYPTO
endif

ifeq ($(BR2_PACKAGE_DROPBEAR_OPENIPC_DISABLE_REVERSEDNS),)
define DROPBEAR_OPENIPC_ENABLE_REVERSE_DNS
	echo '#define DO_HOST_LOOKUP 1'                 >> $(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_ENABLE_REVERSE_DNS
endif

ifeq ($(BR2_PACKAGE_DROPBEAR_OPENIPC_SMALL),y)
DROPBEAR_OPENIPC_LICENSE += , Unlicense, WTFPL
DROPBEAR_OPENIPC_LICENSE_FILES += libtommath/LICENSE libtomcrypt/LICENSE
DROPBEAR_OPENIPC_CONF_OPTS += --disable-zlib --enable-bundled-libtom
else
define DROPBEAR_OPENIPC_BUILD_FEATURED
	echo '#define DROPBEAR_SMALL_CODE 0'            >> $(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_BUILD_FEATURED
DROPBEAR_OPENIPC_DEPENDENCIES += zlib libtomcrypt
DROPBEAR_OPENIPC_CONF_OPTS += --disable-bundled-libtom
endif

define DROPBEAR_OPENIPC_CUSTOM_PATH
	echo '#define DEFAULT_PATH $(BR2_SYSTEM_DEFAULT_PATH)' >>$(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_CUSTOM_PATH

define DROPBEAR_OPENIPC_INSTALL_INIT_SYSTEMD
	$(INSTALL) -D -m 644 package/dropbear/dropbear.service \
		$(TARGET_DIR)/usr/lib/systemd/system/dropbear.service
endef

ifeq ($(BR2_USE_MMU),y)
define DROPBEAR_OPENIPC_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 package/dropbear/S50dropbear \
		$(TARGET_DIR)/etc/init.d/S50dropbear
endef
else
define DROPBEAR_OPENIPC_DISABLE_STANDALONE
	echo '#define NON_INETD_MODE 0'                 >> $(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_DISABLE_STANDALONE
endif

ifneq ($(BR2_PACKAGE_DROPBEAR_OPENIPC_WTMP),y)
DROPBEAR_OPENIPC_CONF_OPTS += --disable-wtmp
endif

ifneq ($(BR2_PACKAGE_DROPBEAR_OPENIPC_LASTLOG),y)
DROPBEAR_OPENIPC_CONF_OPTS += --disable-lastlog
endif

DROPBEAR_OPENIPC_LOCALOPTIONS_FILE = $(call qstrip,$(BR2_PACKAGE_DROPBEAR_OPENIPC_LOCALOPTIONS_FILE))
ifneq ($(DROPBEAR_OPENIPC_LOCALOPTIONS_FILE),)
define DROPBEAR_OPENIPC_APPEND_LOCALOPTIONS_FILE
	cat $(DROPBEAR_OPENIPC_LOCALOPTIONS_FILE) >> $(@D)/localoptions.h
endef
DROPBEAR_OPENIPC_POST_EXTRACT_HOOKS += DROPBEAR_OPENIPC_APPEND_LOCALOPTIONS_FILE
endif

define DROPBEAR_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 $(@D)/dropbearmulti $(TARGET_DIR)/usr/sbin/dropbear
	for f in $(DROPBEAR_OPENIPC_TARGET_BINS); do \
		ln -snf ../sbin/dropbear $(TARGET_DIR)/usr/bin/$$f ; \
	done
	ln -snf /var/run/dropbear $(TARGET_DIR)/etc/dropbear
endef

$(eval $(autotools-package))
