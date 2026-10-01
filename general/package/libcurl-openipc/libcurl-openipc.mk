################################################################################
#
# libcurl-openipc
#
################################################################################

LIBCURL_OPENIPC_VERSION = 8.15.0
LIBCURL_OPENIPC_SOURCE = curl-$(LIBCURL_OPENIPC_VERSION).tar.xz
LIBCURL_OPENIPC_SITE = https://curl.se/download
LIBCURL_OPENIPC_DL_SUBDIR = libcurl
LIBCURL_OPENIPC_DEPENDENCIES = host-pkgconf \
	$(if $(BR2_PACKAGE_ZLIB),zlib) \
	$(if $(BR2_PACKAGE_RTMPDUMP),rtmpdump)
LIBCURL_OPENIPC_LICENSE = curl
LIBCURL_OPENIPC_LICENSE_FILES = COPYING
LIBCURL_OPENIPC_INSTALL_STAGING = YES

# We disable NTLM support because it uses fork(), which doesn't work
# on non-MMU platforms. Moreover, this authentication method is
# probably almost never used. See
# http://curl.haxx.se/docs/manpage.html#--ntlm.
# Likewise, there is no compiler on the target, so libcurl-option (to
# generate C code) isn't very useful
LIBCURL_OPENIPC_CONF_OPTS = --disable-manual --disable-ntlm-wb \
	--enable-hidden-symbols --with-random=/dev/urandom --disable-curldebug \
	--disable-libcurl-option --without-libpsl

# What curl 8 added over the 7.76 this replaced, none of which anything on a
# camera asks for: IPFS, websockets, HSTS, the headers API, SHA-512/256
# digests, TLS session export and HTTPS resource records. And what neither
# the image's scripts nor its programs use: NTLM (the only user of DES),
# DNS-over-HTTPS, alt-svc, .netrc, the easy-options table, the legacy form
# API (curl -F uses MIME, which stays), AWS signing and GSSAPI auth.
LIBCURL_OPENIPC_CONF_OPTS += --disable-ipfs --disable-websockets \
	--disable-hsts --disable-headers-api --disable-sha512-256 \
	--disable-ssls-export --disable-httpsrr --disable-ech \
	--disable-ntlm --disable-doh --disable-alt-svc --disable-netrc \
	--disable-get-easy-options --disable-form-api --disable-aws \
	--disable-kerberos-auth --disable-negotiate-auth --disable-dnsshuffle

# Link-time optimisation takes a tenth off libcurl, which is what curl 8 grew
# by over 7.76 beyond those features.
LIBCURL_OPENIPC_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -flto" \
	LDFLAGS="$(TARGET_LDFLAGS) -flto"

ifeq ($(BR2_TOOLCHAIN_HAS_THREADS),y)
LIBCURL_OPENIPC_CONF_OPTS += --enable-threaded-resolver
else
LIBCURL_OPENIPC_CONF_OPTS += --disable-threaded-resolver
endif

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_VERBOSE),y)
LIBCURL_OPENIPC_CONF_OPTS += --enable-verbose
else
LIBCURL_OPENIPC_CONF_OPTS += --disable-verbose
endif

LIBCURL_OPENIPC_CONFIG_SCRIPTS = curl-config

# wcurl, a shell wrapper curl 8 installs, is nothing a camera runs.
define LIBCURL_OPENIPC_DROP_WCURL
	rm -f $(TARGET_DIR)/usr/bin/wcurl
endef
LIBCURL_OPENIPC_POST_INSTALL_TARGET_HOOKS += LIBCURL_OPENIPC_DROP_WCURL

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_OPENSSL),y)
LIBCURL_OPENIPC_DEPENDENCIES += openssl
# configure adds the cross openssl dir to LD_LIBRARY_PATH which screws up
# native stuff during the rest of configure when target == host.
# Fix it by setting LD_LIBRARY_PATH to something sensible so those libs
# are found first.
LIBCURL_OPENIPC_CONF_ENV += LD_LIBRARY_PATH=$(if $(LD_LIBRARY_PATH),$(LD_LIBRARY_PATH):)/lib:/usr/lib
LIBCURL_OPENIPC_CONF_OPTS += --with-ssl=$(STAGING_DIR)/usr \
	--with-ca-path=/etc/ssl/certs
else ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_TLS_NONE),y)
# curl 8 spells "no TLS at all" --without-ssl; with another library chosen
# below, --without-ssl would contradict it, so only OpenSSL is ruled out.
LIBCURL_OPENIPC_CONF_OPTS += --without-ssl
else
LIBCURL_OPENIPC_CONF_OPTS += --without-openssl
endif

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_GNUTLS),y)
LIBCURL_OPENIPC_CONF_OPTS += --with-gnutls=$(STAGING_DIR)/usr \
	--with-ca-fallback
LIBCURL_OPENIPC_DEPENDENCIES += gnutls
else
LIBCURL_OPENIPC_CONF_OPTS += --without-gnutls
endif

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_MBEDTLS),y)
LIBCURL_OPENIPC_CONF_OPTS += --with-mbedtls=$(STAGING_DIR)/usr \
	--with-ca-bundle=/etc/ssl/certs/ca-certificates.crt
LIBCURL_OPENIPC_DEPENDENCIES += mbedtls-openipc
else
LIBCURL_OPENIPC_CONF_OPTS += --without-mbedtls
endif

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_WOLFSSL),y)
LIBCURL_OPENIPC_CONF_OPTS += --with-wolfssl=$(STAGING_DIR)/usr
LIBCURL_OPENIPC_DEPENDENCIES += wolfssl
else
LIBCURL_OPENIPC_CONF_OPTS += --without-wolfssl
endif

ifeq ($(BR2_PACKAGE_C_ARES),y)
LIBCURL_OPENIPC_DEPENDENCIES += c-ares
LIBCURL_OPENIPC_CONF_OPTS += --enable-ares
else
LIBCURL_OPENIPC_CONF_OPTS += --disable-ares
endif

ifeq ($(BR2_PACKAGE_LIBIDN2),y)
LIBCURL_OPENIPC_DEPENDENCIES += libidn2
LIBCURL_OPENIPC_CONF_OPTS += --with-libidn2
else
LIBCURL_OPENIPC_CONF_OPTS += --without-libidn2
endif

# Configure curl to support libssh2
ifeq ($(BR2_PACKAGE_LIBSSH2),y)
LIBCURL_OPENIPC_DEPENDENCIES += libssh2
LIBCURL_OPENIPC_CONF_OPTS += --with-libssh2
else
LIBCURL_OPENIPC_CONF_OPTS += --without-libssh2
endif

ifeq ($(BR2_PACKAGE_BROTLI),y)
LIBCURL_OPENIPC_DEPENDENCIES += brotli
LIBCURL_OPENIPC_CONF_OPTS += --with-brotli
else
LIBCURL_OPENIPC_CONF_OPTS += --without-brotli
endif

ifeq ($(BR2_PACKAGE_NGHTTP2),y)
LIBCURL_OPENIPC_DEPENDENCIES += nghttp2
LIBCURL_OPENIPC_CONF_OPTS += --with-nghttp2
else
LIBCURL_OPENIPC_CONF_OPTS += --without-nghttp2
endif

ifeq ($(BR2_PACKAGE_LIBGSASL),y)
LIBCURL_OPENIPC_DEPENDENCIES += libgsasl
LIBCURL_OPENIPC_CONF_OPTS += --with-gsasl
else
LIBCURL_OPENIPC_CONF_OPTS += --without-gsasl
endif

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_COOKIES_SUPPORT),y)
LIBCURL_OPENIPC_CONF_OPTS += --enable-cookies
else
LIBCURL_OPENIPC_CONF_OPTS += --disable-cookies
endif

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_PROXY_SUPPORT),y)
LIBCURL_OPENIPC_CONF_OPTS += --enable-proxy
else
LIBCURL_OPENIPC_CONF_OPTS += --disable-proxy
endif

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_EXTRA_PROTOCOLS_FEATURES),y)
LIBCURL_OPENIPC_CONF_OPTS += \
	--enable-dict \
	--enable-gopher \
	--enable-imap \
	--enable-ldap \
	--enable-ldaps \
	--enable-pop3 \
	--enable-rtsp \
	--enable-smb \
	--enable-smtp \
	--enable-telnet \
	--enable-tftp
else
LIBCURL_OPENIPC_CONF_OPTS += \
	--disable-dict \
	--disable-gopher \
	--disable-imap \
	--disable-ldap \
	--disable-ldaps \
	--disable-pop3 \
	--disable-rtsp \
	--disable-smb \
	--disable-telnet \
	--disable-tftp
endif

#	--disable-smtp \


define LIBCURL_OPENIPC_FIX_DOT_PC
	printf 'Requires: openssl\n' >>$(@D)/libcurl.pc.in
endef
LIBCURL_OPENIPC_POST_PATCH_HOOKS += $(if $(BR2_PACKAGE_LIBCURL_OPENIPC_OPENSSL),LIBCURL_FIX_DOT_PC)

ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC_CURL),)
define LIBCURL_OPENIPC_TARGET_CLEANUP
	rm -rf $(TARGET_DIR)/usr/bin/curl
endef
LIBCURL_OPENIPC_POST_INSTALL_TARGET_HOOKS += LIBCURL_TARGET_CLEANUP
endif

HOST_LIBCURL_OPENIPC_DEPENDENCIES = host-openssl
HOST_LIBCURL_OPENIPC_CONF_OPTS = \
	--disable-manual \
	--disable-ntlm-wb \
	--disable-curldebug \
	--with-ssl \
	--without-gnutls \
	--without-mbedtls

HOST_LIBCURL_OPENIPC_POST_PATCH_HOOKS += LIBCURL_FIX_DOT_PC

# The toolchains carry a libcurl of their own, built against the mbedTLS they
# shipped; with per-package directories it can win the staging merge over
# this one, as the mbedTLS copy can. Take it out when this package builds one.
ifeq ($(BR2_PACKAGE_LIBCURL_OPENIPC),y)
define LIBCURL_OPENIPC_DROP_TOOLCHAIN_COPY
	rm -rf $(STAGING_DIR)/usr/include/curl
	rm -f $(STAGING_DIR)/usr/lib/libcurl.* $(STAGING_DIR)/usr/lib/pkgconfig/libcurl.pc \
		$(STAGING_DIR)/usr/bin/curl-config
endef
TOOLCHAIN_EXTERNAL_CUSTOM_POST_INSTALL_STAGING_HOOKS += LIBCURL_OPENIPC_DROP_TOOLCHAIN_COPY
endif

$(eval $(autotools-package))
$(eval $(host-autotools-package))
