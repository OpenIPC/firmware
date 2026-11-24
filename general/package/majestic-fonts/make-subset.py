#!/usr/bin/env python3
# Remakes files/UbuntuMono-Regular.ttf ("OpenIPC Mono") from Roboto Mono 3.001
# (google/fonts, ofl/robotomono), byte for byte, with fontTools 4.61:
#
#   curl -Lo RobotoMono.ttf "https://github.com/google/fonts/raw/main/ofl/robotomono/RobotoMono%5Bwght%5D.ttf"
#   python3 make-subset.py RobotoMono.ttf files/UbuntuMono-Regular.ttf
#
# The file keeps its old name: osd.font in deployed configs, the wiki, msposd
# and divinus all name it. The subset is ASCII, Russian Cyrillic, the degree
# and numero signs; anything outside it renders as an empty box. Of what
# upstream lacks, the control codes are drawn blank and U+0182 as U+0411.
# Hinting is dropped. Renamed because Roboto Mono is a trademark of Google; the
# copyright, trademark and licence records stay as upstream has them.
import sys

from fontTools import subset
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont

VERSION = "0.14"  # MAJESTIC_FONTS_VERSION
UNICODES = "U+0000,U+0008,U+0009,U+000D,U+001D,U+0020-007E,U+00A8,U+00B0," \
           "U+0138,U+0182,U+0401,U+0410-0451,U+2116"
ALIASES = {0x0000: 0x20, 0x0008: 0x20, 0x0009: 0x20, 0x001D: 0x20,
           0x0182: 0x0411}
ASCENT, DESCENT = 0.92, 0.23

src, dst = sys.argv[1], sys.argv[2]
weight = int(sys.argv[3]) if len(sys.argv) > 3 else 400
font = TTFont(src, recalcTimestamp=False)
font = instantiateVariableFont(font, {"wght": weight}, updateFontNames=False)
cmap = font.getBestCmap()
for table in font["cmap"].tables:
    if table.isUnicode():
        for new, old in ALIASES.items():
            table.cmap.setdefault(new, cmap[old])
wanted = subset.parse_unicodes(UNICODES)
missing = sorted(set(wanted) - set(font.getBestCmap()))
assert not missing, missing

opts = subset.Options()
opts.hinting = False
opts.notdef_outline = True
opts.layout_features = []
opts.layout_closure = False
opts.name_IDs = [0, 5, 7, 13, 14]
opts.drop_tables += ["GDEF", "GPOS", "GSUB", "STAT", "gasp"]
sub = subset.Subsetter(opts)
sub.populate(unicodes=wanted)
sub.subset(font)

upem = font["head"].unitsPerEm
asc, desc = round(ASCENT * upem), round(DESCENT * upem)
glyphs = [g for g in
          map(font["glyf"].__getitem__, font.getGlyphOrder())
          if g.numberOfContours]
for g in glyphs:
    g.recalcBounds(font["glyf"])
assert max(g.yMax for g in glyphs) <= asc, "ascent clips"
assert -min(g.yMin for g in glyphs) <= desc, "descent clips"
hhea, os2 = font["hhea"], font["OS/2"]
hhea.ascent, hhea.descent, hhea.lineGap = asc, -desc, 0
os2.sTypoAscender, os2.sTypoDescender, os2.sTypoLineGap = asc, -desc, 0
os2.usWinAscent, os2.usWinDescent = asc, desc
os2.usWeightClass = weight

names = font["name"]
version = "Version " + VERSION
for nid, text in {1: "OpenIPC Mono", 2: "Regular",
                  3: "OpenIPC Mono Regular " + version, 4: "OpenIPC Mono", 5: version,
                  6: "OpenIPCMono-Regular"}.items():
    names.setName(text, nid, 3, 1, 0x409)
font.save(dst)
