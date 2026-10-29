################################################################################
#
# majestic-fonts
#
################################################################################

# files/UbuntuMono-Regular.ttf is a subset of Ubuntu Mono 0.80: ASCII,
# Cyrillic, the degree sign and the numero sign, hinting kept. It is remade
# byte for byte from the upstream font with fontTools 4.61:
#   curl -LO https://github.com/google/fonts/raw/main/ufl/ubuntumono/UbuntuMono-Regular.ttf
#   pyftsubset UbuntuMono-Regular.ttf --output-file=files/UbuntuMono-Regular.ttf \
#     --name-IDs='*' --notdef-outline --no-layout-closure --drop-tables+=VDMX \
#     --unicodes=U+0000,U+0008,U+0009,U+000D,U+001D,U+0020-007E,U+00A8,U+00B0,\
#     U+0138,U+0182,U+0410-0451,U+2116
# Anything majestic draws that is not in it renders as an empty box.
MAJESTIC_FONTS_VERSION = 0.13
MAJESTIC_FONTS_SITE_METHOD = local
MAJESTIC_FONTS_SITE = $(MAJESTIC_FONTS_PKGDIR)/files

MAJESTIC_FONTS_LICENSE = UFL-1.0
MAJESTIC_FONTS_LICENSE_FILES = LICENSE

define MAJESTIC_FONTS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts/truetype
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts/truetype $(@D)/UbuntuMono-Regular.ttf
	$(INSTALL) -m 644 $(@D)/LICENSE $(TARGET_DIR)/usr/share/fonts/truetype/UbuntuMono-Regular.LICENSE
endef

$(eval $(generic-package))
