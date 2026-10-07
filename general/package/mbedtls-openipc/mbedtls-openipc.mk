################################################################################
#
# mbedtls-openipc
#
################################################################################

MBEDTLS_OPENIPC_VERSION = 3.6.4
# The release tarball, not a tag archive: from 3.6 the generated sources and
# the framework submodule are only in the release.
MBEDTLS_OPENIPC_SOURCE = mbedtls-$(MBEDTLS_OPENIPC_VERSION).tar.bz2
MBEDTLS_OPENIPC_SITE = https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-$(MBEDTLS_OPENIPC_VERSION)
# BUILD_SHARED_LIBS, which Buildroot sets, would turn the bundled Everest and
# p256-m objects into two shared libraries of their own that libmbedcrypto
# then needs at run time, empty here. They belong inside libmbedcrypto.
MBEDTLS_OPENIPC_CONF_OPTS = \
	-DENABLE_PROGRAMS=$(if $(BR2_PACKAGE_MBEDTLS_OPENIPC_PROGRAMS),ON,OFF) \
	-DENABLE_TESTING=OFF -DBUILD_SHARED_LIBS=OFF
MBEDTLS_OPENIPC_INSTALL_STAGING = YES
MBEDTLS_OPENIPC_LICENSE = Apache-2.0
MBEDTLS_OPENIPC_LICENSE_FILES = apache-2.0.txt

MBEDTLS_OPENIPC_CONFIG_H = $(@D)/include/mbedtls/mbedtls_config.h

# TLS 1.3 stays off, as it was in 2.x: with it on, 3.6 refuses a handshake
# until the application has called psa_crypto_init(), which none of the
# libraries and programs on the image do for 1.2.
define MBEDTLS_ENABLE_SRTP
	$(SED) "s://#define MBEDTLS_SSL_DTLS_SRTP:#define MBEDTLS_SSL_DTLS_SRTP:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
	$(SED) "s:^#define MBEDTLS_SSL_PROTO_TLS1_3$$://#define MBEDTLS_SSL_PROTO_TLS1_3:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
	$(SED) "s:#define MBEDTLS_ECP_DP_SECP224K1_ENABLED://#define MBEDTLS_ECP_DP_SECP224K1_ENABLED:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
	$(SED) "s:#define MBEDTLS_ECP_DP_SECP256K1_ENABLED://#define MBEDTLS_ECP_DP_SECP256K1_ENABLED:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
endef
MBEDTLS_OPENIPC_POST_PATCH_HOOKS += MBEDTLS_ENABLE_SRTP

# Upstream's default config compiles in every algorithm's self-test vectors
# and a table of feature-name strings. Nothing on a camera calls
# mbedtls_*_self_test or mbedtls_version_check_feature, majestic included (it
# is linked against this library, so its imports were checked on every
# build), and ENABLE_PROGRAMS is off. Neither changes a struct layout.
define MBEDTLS_OPENIPC_DROP_TEST_CODE
	$(SED) "s:^#define MBEDTLS_SELF_TEST$$://#define MBEDTLS_SELF_TEST:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
	$(SED) "s:^#define MBEDTLS_VERSION_FEATURES$$://#define MBEDTLS_VERSION_FEATURES:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
endef
MBEDTLS_OPENIPC_POST_PATCH_HOOKS += MBEDTLS_OPENIPC_DROP_TEST_CODE

# Modules 3.6 compiles in by default that nothing on the image calls: the PSA
# crypto core (TLS 1.2 and every caller here use the classic API; curl falls
# back to it without PSA), LMS, SHA-3, PKCS#7, PKCS#12, EC-JPAKE, ARIA,
# Camellia, CCM, RIPEMD-160, HKDF (TLS 1.3 only), CRL parsing, the Brainpool,
# 192/224-bit and Curve448 groups, the TLS key exchanges no server needs
# (static ECDH, DHE, PSK), the server-side session cache, tickets and DTLS
# cookies, the timing module, maximum fragment length, RSASSA-PSS
# certificates, RSA-alt keys, DTLS connection IDs and context serialisation.
# AESCE_C, the Armv8 Crypto Extension AES, goes too: no SoC here runs code
# that has it, and on the 32-bit NEON toolchains it does not even compile
# without a +crypto target. This is what keeps 3.6 smaller than the 2.25 it
# replaces; every symbol the image's own binaries import is still there.
MBEDTLS_OPENIPC_UNUSED = \
	PSA_CRYPTO_C PSA_CRYPTO_STORAGE_C PSA_ITS_FILE_C LMS_C SHA3_C PKCS7_C \
	PKCS12_C ECJPAKE_C ARIA_C CAMELLIA_C CCM_C RIPEMD160_C HKDF_C \
	X509_CRL_PARSE_C \
	ECP_DP_BP256R1_ENABLED ECP_DP_BP384R1_ENABLED ECP_DP_BP512R1_ENABLED \
	ECP_DP_SECP192R1_ENABLED ECP_DP_SECP192K1_ENABLED \
	ECP_DP_SECP224R1_ENABLED ECP_DP_CURVE448_ENABLED \
	KEY_EXCHANGE_DHE_RSA_ENABLED KEY_EXCHANGE_PSK_ENABLED \
	KEY_EXCHANGE_DHE_PSK_ENABLED KEY_EXCHANGE_ECDHE_PSK_ENABLED \
	KEY_EXCHANGE_RSA_PSK_ENABLED KEY_EXCHANGE_ECDH_ECDSA_ENABLED \
	KEY_EXCHANGE_ECDH_RSA_ENABLED \
	SSL_CACHE_C SSL_TICKET_C SSL_COOKIE_C TIMING_C \
	SSL_MAX_FRAGMENT_LENGTH X509_RSASSA_PSS_SUPPORT PK_RSA_ALT_SUPPORT \
	SSL_DTLS_CONNECTION_ID SSL_DTLS_CONNECTION_ID_COMPAT \
	SSL_CONTEXT_SERIALIZATION AESCE_C
# The second round (#2507), measured against what majestic, curl, libevent's
# bufferevent and uacme import: ChaCha20-Poly1305, every TLS record cipher but
# AES-GCM (every server and browser these talk to offers it, and nothing on
# the image encrypts a PEM key), CFB/OFB/XTS, static-RSA key exchange,
# deterministic ECDSA and the HMAC-DRBG only it used, PKCS#5 (PBKDF2),
# compressed and specified-domain EC keys, session tickets (curl compiles
# them out without the option), the RFC 5705 exporter, DTLS client port
# reuse, the alerts beyond the ones a handshake sends, the debug module
# (majestic stopped needing it), and the error-string table: with
# ERROR_STRERROR_DUMMY, on by default, mbedtls_strerror() stays defined for
# majestic, curl and uacme and prints the numeric code. About 45 KB of xz on
# top of the first round. Curve25519 stays for majestic's DTLS, GENPRIME and
# CSR parsing for uacme's RSA keys, renegotiation for libevent, and P-521
# because the CA bundle carries a P-521 root (e-Szigno TLS Root CA 2023):
# without it mbedtls_x509_crt_parse_file() skips that one certificate, and
# majestic stops at startup ("Make sure CA repository ... exist", "Error while
# SSL init") rather than run with a partial bundle. A curve, key type or
# signature algorithm can only go once no root in general/overlay/etc/ssl/certs
# uses it.
MBEDTLS_OPENIPC_UNUSED += \
	CHACHA20_C POLY1305_C CHACHAPOLY_C \
	CIPHER_MODE_CFB CIPHER_MODE_OFB CIPHER_MODE_XTS \
	KEY_EXCHANGE_RSA_ENABLED \
	ECDSA_DETERMINISTIC HMAC_DRBG_C \
	PK_PARSE_EC_COMPRESSED PK_PARSE_EC_EXTENDED \
	SSL_SESSION_TICKETS SSL_KEYING_MATERIAL_EXPORT \
	SSL_DTLS_CLIENT_PORT_REUSE SSL_ALL_ALERT_MESSAGES SSL_ENCRYPT_THEN_MAC \
	DEBUG_C ERROR_C
# DES, DH, CMAC, NIST key wrap, AES-CBC and PKCS#5 are wanted by
# wpa_supplicant's mbedTLS backend (MS-CHAPv2, WPS, PMF, EAPOL key data, and
# PBKDF2 for the WPA-PSK passphrase) and by nothing else here.
ifneq ($(BR2_PACKAGE_WPA_SUPPLICANT_OPENIPC),y)
MBEDTLS_OPENIPC_UNUSED += DES_C DHM_C CMAC_C NIST_KW_C PKCS5_C \
	CIPHER_MODE_CBC CIPHER_PADDING_PKCS7 CIPHER_PADDING_ONE_AND_ZEROS \
	CIPHER_PADDING_ZEROS_AND_LEN CIPHER_PADDING_ZEROS
endif
define MBEDTLS_OPENIPC_DROP_UNUSED
	$(foreach o,$(MBEDTLS_OPENIPC_UNUSED),
		$(SED) "s:^#define MBEDTLS_$(o)\b://#define MBEDTLS_$(o):" \
			$(MBEDTLS_OPENIPC_CONFIG_H)
	)
endef
MBEDTLS_OPENIPC_POST_PATCH_HOOKS += MBEDTLS_OPENIPC_DROP_UNUSED

# The smaller SHA-256/512 code: a few KB less, and hashing at camera rates is
# nowhere near the handshake's cost.
define MBEDTLS_OPENIPC_SMALLER_SHA
	$(SED) "s:^//#define MBEDTLS_SHA256_SMALLER$$:#define MBEDTLS_SHA256_SMALLER:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
	$(SED) "s:^//#define MBEDTLS_SHA512_SMALLER$$:#define MBEDTLS_SHA512_SMALLER:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
endef
MBEDTLS_OPENIPC_POST_PATCH_HOOKS += MBEDTLS_OPENIPC_SMALLER_SHA

# Smaller AES tables: a few cycles more per block at SRTP rates.
define MBEDTLS_OPENIPC_SMALLER_AES
	$(SED) "s:^//#define MBEDTLS_AES_FEWER_TABLES$$:#define MBEDTLS_AES_FEWER_TABLES:" \
		$(MBEDTLS_OPENIPC_CONFIG_H)
endef
MBEDTLS_OPENIPC_POST_PATCH_HOOKS += MBEDTLS_OPENIPC_SMALLER_AES
ifeq ($(BR2_STATIC_LIBS),y)
MBEDTLS_OPENIPC_CONF_OPTS += -DLINK_WITH_PTHREAD=ON
endif

ifeq ($(BR2_STATIC_LIBS),y)
MBEDTLS_OPENIPC_CONF_OPTS += \
	-DUSE_SHARED_MBEDTLS_LIBRARY=OFF -DUSE_STATIC_MBEDTLS_LIBRARY=ON
else ifeq ($(BR2_SHARED_STATIC_LIBS),y)
MBEDTLS_OPENIPC_CONF_OPTS += \
	-DUSE_SHARED_MBEDTLS_LIBRARY=ON -DUSE_STATIC_MBEDTLS_LIBRARY=ON
else ifeq ($(BR2_SHARED_LIBS),y)
MBEDTLS_OPENIPC_CONF_OPTS += \
	-DUSE_SHARED_MBEDTLS_LIBRARY=ON -DUSE_STATIC_MBEDTLS_LIBRARY=OFF
endif

define MBEDTLS_OPENIPC_DISABLE_ASM
	$(SED) '/^#define MBEDTLS_AESNI_C/d' \
		$(MBEDTLS_OPENIPC_CONFIG_H)
	$(SED) '/^#define MBEDTLS_HAVE_ASM/d' \
		$(MBEDTLS_OPENIPC_CONFIG_H)
	$(SED) '/^#define MBEDTLS_PADLOCK_C/d' \
		$(MBEDTLS_OPENIPC_CONFIG_H)
endef

# ARM in thumb mode breaks debugging with asm optimizations
# Microblaze asm optimizations are broken in general
# MIPS R6 asm is not yet supported
ifeq ($(BR2_ENABLE_DEBUG)$(BR2_ARM_INSTRUCTIONS_THUMB)$(BR2_ARM_INSTRUCTIONS_THUMB2),yy)
MBEDTLS_OPENIPC_POST_CONFIGURE_HOOKS += MBEDTLS_OPENIPC_DISABLE_ASM
else ifeq ($(BR2_microblaze)$(BR2_MIPS_CPU_MIPS32R6)$(BR2_MIPS_CPU_MIPS64R6),y)
MBEDTLS_OPENIPC_POST_CONFIGURE_HOOKS += MBEDTLS_OPENIPC_DISABLE_ASM
endif

# The OpenIPC toolchains carry an mbedTLS of their own in the sysroot, and
# with per-package directories a package's staging is merged from its
# dependencies, so the toolchain's libmbedtls.so can win over this package's
# and leave a library compiled against these headers linked against another
# version's soname. This package is the one copy: take the toolchain's out.
ifeq ($(BR2_PACKAGE_MBEDTLS_OPENIPC),y)
define MBEDTLS_OPENIPC_DROP_TOOLCHAIN_COPY
	rm -rf $(STAGING_DIR)/usr/include/mbedtls $(STAGING_DIR)/usr/include/psa \
		$(STAGING_DIR)/usr/lib/cmake/MbedTLS
	rm -f $(STAGING_DIR)/usr/lib/libmbedtls.* $(STAGING_DIR)/usr/lib/libmbedcrypto.* \
		$(STAGING_DIR)/usr/lib/libmbedx509.* $(STAGING_DIR)/usr/lib/pkgconfig/mbed*.pc
endef
TOOLCHAIN_EXTERNAL_CUSTOM_POST_INSTALL_STAGING_HOOKS += MBEDTLS_OPENIPC_DROP_TOOLCHAIN_COPY
endif

$(eval $(cmake-package))
