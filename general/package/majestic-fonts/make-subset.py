#!/usr/bin/env python3
# Remakes files/UbuntuMono-Regular.ttf from upstream Ubuntu Mono 0.80
# (google/fonts, ufl/ubuntumono), byte for byte, with fontTools 4.61:
#
#   curl -LO https://github.com/google/fonts/raw/main/ufl/ubuntumono/UbuntuMono-Regular.ttf
#   python3 make-subset.py UbuntuMono-Regular.ttf files/UbuntuMono-Regular.ttf
#
# The subset is ASCII, Cyrillic, the degree sign and the numero sign, with
# hinting kept; anything majestic draws outside it renders as an empty box.
# A subset is a Modified Version under the Ubuntu Font Licence, which must
# not carry the original's name (clause 2), so it is named OpenIPC Mono.
import sys

from fontTools import subset
from fontTools.ttLib import TTFont

VERSION = "0.13"  # MAJESTIC_FONTS_VERSION
UNICODES = "U+0000,U+0008,U+0009,U+000D,U+001D,U+0020-007E,U+00A8,U+00B0," \
           "U+0138,U+0182,U+0410-0451,U+2116"

src, dst = sys.argv[1], sys.argv[2]
opts = subset.Options()
opts.name_IDs = ["*"]
opts.notdef_outline = True
opts.layout_closure = False
opts.drop_tables += ["VDMX"]
font = TTFont(src)
sub = subset.Subsetter(opts)
sub.populate(unicodes=subset.parse_unicodes(UNICODES))
sub.subset(font)

names = {
    0: "-",
    1: "OpenIPC Mono",
    2: "Regular",
    3: "OpenIPC Mono Regular Version " + VERSION,
    4: "OpenIPC Mono",
    5: "Version " + VERSION,
    6: "OpenIPC-Regular",
}
table = font["name"]
table.names = []
for nid, text in names.items():
    table.setName(text, nid, 1, 0, 0)
    table.setName(text, nid, 3, 1, 0x409)
font.save(dst)
