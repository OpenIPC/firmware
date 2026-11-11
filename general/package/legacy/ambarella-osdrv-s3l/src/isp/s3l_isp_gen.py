#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""S3L ISP configuration generator: a sensor preset in, an S3L ISP configuration file out (the file Divinus loads
into the DSP's IMG buffer before preview starts: the LISO, HDR and CE image blocks and the tables they point at).

  s3l_isp_gen.py OUT.bin [--preset FILE.yaml]... [--set key=value]... [--show]

Presets are `key: value` lines, applied in order, then --set on top. Keys (OPTIONS below):
  width, height, bayer   sensor output size and Bayer phase (0..3); default 2688x1520, bayer 1 (OV4689)
  black_level            sensor black level in 12-bit raw units, 0..2047
  wb_gain                starting white balance 'R,B' or 'R,G,B', each 0.25..8 (the streamer's AWB takes over)
  dbp                    dynamic bad-pixel correction: off | MODE,THRESH (mode 1..4, threshold 0..255)
  sharpness              final sharpening: off | mild | medium | strong
  temporal_nr            motion-compensated temporal noise reduction: off | low | med | high
  saturation             colour saturation, 0..3.9 (1.0 = as the sensor gives it); the default is 1.0
  rgb2yuv                colour matrix and range of the YUV output: bt601 (studio range, the default) | bt709-full
  tone_curve             output tone curve as IN:OUT,IN:OUT,... knots on the 8-bit code of each colour channel
                         (IN rising, OUT not falling), joined by straight lines; off = identity
Everything not set by a preset is neutral: identity colour with gamma 0.45, BT.601, no local exposure, contrast
enhancement or HDR (a linear sensor).

Where the format comes from: the block layout, field offsets, table sizes and encodings, and the limits the DSP
enforces were worked out by reverse engineering the S3L ISP library and DSP microcode, checked against the camera.
The values written are this file's own: neutral or identity settings, values measured on an OV4689 camera (black
level, white balance), and noise-reduction and sharpening curves designed and measured here. Nothing is copied from
a camera's firmware or tuning files. The layout has been run on the IP4M-1026E with the DSP microcode OpenIPC ships
(SDK 2.5.0, 2018/10/28 284712) and with that camera's own (2017/02/22 267837).

Output (S3LISP03): "S3LISP03", u32 ops, u32 img_len, u32 img_off, u32 nrel; ops x {u8 kind, u8 nr, u16 0,
u32 offset, u32 flags}; nrel x u32 relocation; then img_len bytes of the IMG buffer from img_off. Offsets are from
the start of the IMG buffer. Table pointers inside the blocks are written as IMG offsets, and each relocation names a
pointer word the loader adds the IMG buffer's DSP address to, so the file does not depend on where the buffer is.
"""
import math, os, struct, sys

IMG_LEN = 0x600000
ALLOC_START = 0x1c0000         # well clear of the command rings at +0x500000
ALLOC_END = 0x500000

# --- format ---------------------------------------------------------------------------------------------------------
LISO_SIZE, HDR_SIZE, CE_SIZE = 1024, 68, 8
# LISO pointer fields of the tables written here; the rest stay 0 (table not used).
LISO_PTRS = {0x100: 'le_gain_curve', 0x120: 'chroma_gain_curve', 0x124: 'cc_reg', 0x128: 'cc_3d',
             0x12c: 'tone_curve', 0x160: 'dbp_dark', 0x164: 'dbp_hot', 0x22c: 'video_mctf', 0x230: 'final_sharpen'}
TABLE_SIZES = {'le_gain_curve': 512, 'chroma_gain_curve': 256, 'cc_reg': 2304, 'cc_3d': 16384, 'tone_curve': 1024,
               'dbp_dark': 768, 'dbp_hot': 768, 'video_mctf': 10080,
               'final_sharpen': 12800}   # the DSP reads 12752 bytes of it
# The image-block ioctls (kind 's' = set, ioctl nr), sent in this order before preview starts, which applies every
# sub-filter: contrast enhancement (0x65), HDR (0x64), then LISO (0x60). The flags word is unused for 's'.
OPS = [('s', 0x65, 'ce'), ('s', 0x64, 'hdr'), ('s', 0x60, 'liso')]


class Config:
    def __init__(self, width, height, bayer):
        self.liso = bytearray(LISO_SIZE)
        self.tables = {n: None for n in TABLE_SIZES}
        L = self.liso
        struct.pack_into('<I', L, 0x044, bayer & 3)              # maps the four black levels onto the CFA phase
        struct.pack_into('<4I', L, 0x0e4, *[0x10000] * 4)        # WB 1.0 until a preset or the streamer's AWB sets it
        struct.pack_into('<HHI', L, 0x10c, 0, 1, 1)              # chroma scale on, clamped to the legal range
        L[0x148] = 0x02                                          # use the chroma-scale curve (saturation)
        struct.pack_into('<4H', L, 0x14a, *[16383] * 4)          # WB saturation level: the 14-bit maximum, no clip
        struct.pack_into('<4H', L, 0x152, 0, 16383, 0, 16383)    # AWB/AE statistics windows: the full range
        # demosaic: enable/mode 7, clamp 0, activity 4, activity diff 1024, grad clip 512, grad noise 128 (chosen and
        # checked on the camera: no zipper or false colour on edges)
        struct.pack_into('<BBBBHHH', L, 0x18c, 0, 7, 0, 4, 1024, 512, 128)
        struct.pack_into('<6H', L, 0x220, width, height, width // 2, height // 2, width, height)
        # The local-exposure gain curve is read even with local exposure off: no table gives a black frame with a
        # grid of live pixels. Flat 1024 = 1.0.
        self.tables['le_gain_curve'] = bytearray(u16s([1024] * 256))
        self.tables['chroma_gain_curve'] = bytearray(u16s([1024] * 128))    # saturation 1.0
        self.tables['video_mctf'] = bytearray(TABLE_SIZES['video_mctf'])  # temporal NR off (all zero)
        color_chain(self)
        rgb2yuv_bt601(self)
        final_sharpen_off(self)

    def build(self):
        """Lay the blocks and tables out in an IMG image: (image, {name: offset}, [relocated pointer offsets])."""
        img = bytearray(IMG_LEN)
        pos, place, rel = ALLOC_START, {}, []

        def alloc(name, data, align=64):
            nonlocal pos
            pos = (pos + align - 1) & ~(align - 1)
            if pos + len(data) > ALLOC_END:
                raise SystemExit('ISP layout does not fit')
            place[name] = pos
            img[pos:pos + len(data)] = data
            pos += len(data)

        for name, data in self.tables.items():
            if data is not None:
                alloc(name, data)
        for i, (_, _, blk) in enumerate(OPS):
            if blk == 'ce':
                alloc(f'ce{i}', bytes(CE_SIZE))           # descriptor {0, 0}: contrast enhancement off
            elif blk == 'hdr':
                alloc(f'hdr{i}', bytes(HDR_SIZE))         # HDR blending off (linear sensor)
            else:
                alloc(f'liso{i}', self.liso)
                for fo, name in LISO_PTRS.items():
                    if name in place:
                        struct.pack_into('<I', img, place[f'liso{i}'] + fo, place[name])
                        rel.append(place[f'liso{i}'] + fo)
        return img, place, rel

    def write(self, path):
        img, place, rel = self.build()
        ops = b''.join(struct.pack('<BB2xII', ord(k), nr, place[f'{blk}{i}'], 0) for i, (k, nr, blk) in enumerate(OPS))
        start = ALLOC_START
        end = (max(place.values()) + 0x10000 + 0xfff) & ~0xfff
        end = min(end, ALLOC_END)
        while end > start and not any(img[end - 0x1000:end]):   # trim trailing zero pages
            end -= 0x1000
        end = max(end, max(place.values()) + 0x400)
        with open(path, 'wb') as f:
            f.write(b'S3LISP03' + struct.pack('<IIII', len(OPS), end - start, start, len(rel)) + ops
                    + struct.pack(f'<{len(rel)}I', *rel) + img[start:end])
        return end - start, place


# --- fixed parts ----------------------------------------------------------------------------------------------------
def u16s(values):
    return struct.pack('<%dH' % len(values), *values)

# Colour chain: input LUT (+0x124) -> 16x16x16 LUT (+0x128) -> output LUT (+0x12c). The input LUT has 352 knots,
# spaced (count, step) in 12-bit units:
CC_IN_RUNS = [(64, 1), (32, 2), (32, 4), (96, 8), (64, 16), (64, 32)]


def color_chain(cfg):
    """Identity colour, with the display gamma 0.45 in the input LUT."""
    knots, x = [], 0
    for n, step in CC_IN_RUNS:
        for _ in range(n):
            knots.append(x)
            x += step
    e = [min(4095, round(4095 * (k / 4096) ** 0.45)) for k in knots]
    words = [e[2 * i] | e[2 * i + 1] << 12 for i in range(176)] + [0] * 16
    cfg.tables['cc_reg'] = bytearray(struct.pack('<192I', *words) * 3)
    q = lambda k: round(k * 1023 / 15)
    cfg.tables['cc_3d'] = bytearray(struct.pack('<4096I', *(q(a) | q(b) << 10 | q(c) << 20
                                                            for a in range(16) for b in range(16) for c in range(16))))
    tone_table(cfg)


def tone_table(cfg, vals=None):
    """Install the tone table: 256 10-bit values, one per 8-bit input code. No values means the identity."""
    if vals is None:
        vals = [min(4 * i, 1023) for i in range(256)]
    cfg.tables['tone_curve'] = bytearray(struct.pack('<256I', *(x | x << 10 | x << 20 for x in vals)))


def rgb2yuv_bt601(cfg):
    """RGB->YUV (+0x130, Q10, 1024 = 1.0) and offsets: BT.601, studio range."""
    m = ((0.257, 0.504, 0.098), (-0.148, -0.291, 0.439), (0.439, -0.368, -0.071))
    struct.pack_into('<9h3h', cfg.liso, 0x130, *[round(c * 1024) for row in m for c in row], 16, 128, 128)


FS_UPDATE = 0x3fff0000          # LISO +0x234 bits 16-29: the final-sharpen sub-tables to apply


def final_sharpen_off(cfg):
    """Final sharpen off: zeros with byte 0 = 1 (mcts_en: without it the DSP never writes the output buffer, which
    shows as a line pattern)."""
    t = bytearray(TABLE_SIZES['final_sharpen'])
    t[0] = 0x01
    cfg.tables['final_sharpen'] = t
    struct.pack_into('<I', cfg.liso, 0x234, struct.unpack_from('<I', cfg.liso, 0x234)[0] | FS_UPDATE)


# --- final sharpen --------------------------------------------------------------------------------------------------
FS_SHPB, FS_SHPC, FS_T1, FS_T2, FS_R1, FS_R2, FS_CORE1_T, FS_CORE1_R = 0x10, 0x2c8, 0x580, 0xa18, 0xca8, 0x1140, 0x1bd0, 0x1dd0
FIR_W = [5] * 8 + [6] * 5 + [5, 5, 6, 7, 8, 7, 6, 5, 5, 6, 8, 9]          # signed width of taps 0..24
FIR_BYTE = list(range(16)) + [16, 18, 20, 22, 23, 24, 25, 27]             # byte of taps 0..23 in the 32-byte record
FIR_UNIT = 64   # measured: 1.0 at shift 4. DC 0 pushes every pixel down by MAX_CHANGE_DOWN, DC 128 up by
                # MAX_CHANGE_UP, DC 64 passes through. The amount is FIR(Y) - Y, so the kernel is a full filter.


def fs_lvl_const(t, off, val):
    """A level-control record (0x50 bytes) giving the same value at every level: low, base and high value = val.
    With the three equal, the thresholds, deltas and area that place the levels do not change the output, so they
    stay 0 (measured: thresholds, deltas and areas at 0 or elsewhere give the same picture)."""
    for slot in (0x30, 0x38, 0x40):
        t[off + slot] = val


def fs_kernel(g, sigma):
    """7x7 unsharp detail kernel K = g * (delta - Gaussian(sigma)), zero DC: the 25 unique taps (1.0 units)."""
    G = [[math.exp(-((x - 3) ** 2 + (y - 3) ** 2) / (2 * sigma * sigma)) for x in range(7)] for y in range(7)]
    n = sum(map(sum, G))
    K = [[-g * G[y][x] / n for x in range(7)] for y in range(7)]
    K[3][3] += g
    return [K[i // 7][i % 7] for i in range(24)] + [K[3][3]]


def fs_encode_fir(taps):
    """The detail taps at the largest shift S (4..0) whose taps fit their field widths; centre = -2 sum(others)
    keeps DC at 0. -> (S, q[25])"""
    for S in range(4, -1, -1):
        q = [round(FIR_UNIT * k / 2 ** (4 - S)) for k in taps[:24]]
        q.append(-2 * sum(q))
        if all(-(1 << (w - 1)) <= v < (1 << (w - 1)) for v, w in zip(q, FIR_W)):
            return S, q
    raise SystemExit('sharpness: kernel too strong to encode')


def fs_fir_record(t, off, q):
    for i in range(24):
        t[off + FIR_BYTE[i]] = q[i] & 0xff
    struct.pack_into('<I', t, off + 28, (q[24] & 0x1ff) << 8)


def fs_fir_config(t, base, shift, q, clip2):
    # Clip limits at the full range of their signed 13-bit fields (no clip), stored masked to the width.
    s13 = lambda v: v & 0x1fff
    struct.pack_into('<2I', t, base + 0x000, s13(-4096), s13(4095))
    if clip2:
        struct.pack_into('<2I', t, base + 0x008, s13(-4096), s13(4095))
    struct.pack_into('<2H', t, base + 0x010, s13(-4096), s13(4095))
    struct.pack_into('<Q', t, base + 0x018, sum(shift << (5 * j) for j in range(9)))   # FIR 0 shift, 9 directions
    struct.pack_into('<Q', t, base + 0x020, sum(4 << (5 * j) for j in range(9)))       # FIR 1: zero taps, shift 4
    if q:
        for j in range(9):                                          # the same kernel in all 9 directions
            fs_fir_record(t, base + 0x030 + j * 32, q)
    struct.pack_into('<2I', t, base + 0x390, s13(-4096), s13(4095))
    if clip2:
        struct.pack_into('<2I', t, base + 0x398, s13(-4096), s13(4095))
    struct.pack_into('<2H', t, base + 0x3a0, -256 & 0x1ff, 255)
    for n in range(3):                                              # scale level controls 0..2: 0
        fs_lvl_const(t, base + 0x3a8 + n * 0x50, 0)


def build_final_sharpen(sharp):
    """Final sharpen table. sharp = None: the filters run with nothing to add (unity). Otherwise (g, sigma, c, m):
    unsharp kernel g, sigma; soft-knee coring at c x noise; halo limit m."""
    t = bytearray(TABLE_SIZES['final_sharpen'])
    t[0] = 0x01                                                     # mcts_en
    en = sharp is not None
    g, sigma, c, m = sharp if en else (0, 1, 0, 0)
    q, S = None, 4
    if en:
        S, q = fs_encode_fir(fs_kernel(g, sigma))
    for base, shpb in ((FS_SHPB, True), (FS_SHPC, False)):
        t[base] = (1 | 3 << 3 | (1 << 5 if shpb else 0)) if en else 0
        t[base + 0x20] = 0                                          # coring 1 multiplier shift 0 (coring unity 8)
        t[base + 0x40] = 0x03                                       # chroma upsampling H and V: the 4:2:0 input
        t[base + 0x48] = 1                                          # USE_GENERATED_LOWPASS: the base image (measured:
                                                                    # 0 turns the stage into a smoothing filter)
        t[base + 0x50] = t[base + 0x51] = math.ceil(0.75 * m) if en else 0   # MAX_CHANGE_DOWN (+_FROM_CENTER)
        t[base + 0x58] = t[base + 0x59] = m if en else 0                     # MAX_CHANGE_UP (+_FROM_CENTER)
        t[base + 0x60], t[base + 0x68] = 0, 255                     # CLIP_LOW / CLIP_HIGH: the 8-bit range
        # iso/dir alpha and alpha_mul 0 (measured: no effect on detail or noise in this form), min 0, max 255 (the
        # 8-bit range), pre-MCTF blend 0
        for i, v in enumerate((0, 0, 0, 0, 255, 0)):
            fs_lvl_const(t, base + 0x70 + i * 0x50, v)
    fs_fir_config(t, FS_T1, S, q, True)
    fs_fir_config(t, FS_R1, S, q, False)
    u = 16                                                          # coring index scale: unity
    for base in (FS_T2, FS_R2):                                     # coring 1: index scale u, min 0, max 255, scale u
        for n, v in enumerate((u, 0, 255, u) if en else (0, 0, 0, 0)):
            fs_lvl_const(t, base + n * 0x50, v)
        t[base + 0x288] = 1                                         # measured: 0 loses about 20 % of the detail
    if en:                                                          # soft-knee coring table, unity 8
        noise = 2.0
        d2 = math.sqrt(sum(k * k for k in fs_kernel(1, sigma)[:24]) * 2 + fs_kernel(1, sigma)[24] ** 2)
        th = max(c * noise * d2 * g, 0.5)
        cor = [0 if i <= th else min(31, round(8 * (i - th) / th)) if i < 2 * th else 8 for i in range(256)]
        for base in (FS_CORE1_T, FS_CORE1_R):
            for r in range(64):
                struct.pack_into('<I', t, base + 8 * r, sum((cor[4 * r + k] & 31) << (5 * k) for k in range(4)))
    return t


SHARP_PRESETS = {'mild': (0.35, 1.0, 2.5, 6), 'medium': (0.7, 1.0, 2.0, 12), 'strong': (1.2, 0.8, 1.5, 24)}


def opt_sharpness(cfg, v):
    if v == 'off':
        cfg.tables['final_sharpen'] = build_final_sharpen(None)
    elif v in SHARP_PRESETS:
        cfg.tables['final_sharpen'] = build_final_sharpen(SHARP_PRESETS[v])
    else:
        raise SystemExit(f'sharpness {v!r}: off, ' + ', '.join(SHARP_PRESETS))
    struct.pack_into('<I', cfg.liso, 0x234, struct.unpack_from('<I', cfg.liso, 0x234)[0] | FS_UPDATE)


# --- temporal noise reduction (video MCTF, +0x22c) -------------------------------------------------------------------
MCTF_CURVE = {'B': 0x0b08, 'C': 0x0bc8, 'D': 0x0c88}         # 64 x u8 per channel, channel stride 64
# alpha floor a (current-frame weight for static areas), ramp T0..T1 in curve bins, max change Y / chroma in code
# values, frame counts, filter strength gates iso/dir (Y, chroma). Chosen by temporal-noise measurements at high gain.
TNR_PRESETS = {
    'low':  dict(a=192, t0=2, t1=6,  mc_y=4, mc_c=6,  counts=(1, 1), gate=(64, 128)),
    'med':  dict(a=128, t0=4, t1=10, mc_y=6, mc_c=8,  counts=(2, 2), gate=(128, 255)),
    'high': dict(a=64,  t0=6, t1=14, mc_y=8, mc_c=12, counts=(3, 3), gate=(192, 255)),
}


def mctf_curve(a, t0, t1):
    return bytes([a if i < t0 else 255 if i >= t1 else round(a + (255 - a) * (i - t0) / (t1 - t0)) for i in range(64)])


def mctf_lvl(m, base, vals):
    """MCTF level-control record (0x50 bytes, y/cb/cr per slot) giving the same value at every level: low, base and
    high value = vals. As for fs_lvl_const, the thresholds, deltas and area then do not matter and stay 0."""
    for slot in (0x30, 0x38, 0x40):
        m[base + slot: base + slot + 3] = bytes(vals)


def opt_temporal_nr(cfg, v):
    """off: an all-zero table (verified to pass video through). low/med/high: curves B and C hold the current-frame
    weight per motion bin (alpha floor for static areas, a ramp, 255 above); curve D passes through (255). The mode,
    median and temporal-adjust fields stay 0: measured on a static scene at high gain and on a moving pattern, setting
    them changed neither the noise nor the detail, and no setting showed ghosting."""
    m = bytearray(TABLE_SIZES['video_mctf'])
    if v != 'off':
        if v not in TNR_PRESETS:
            raise SystemExit(f'temporal_nr {v!r}: off, ' + ', '.join(TNR_PRESETS))
        p = TNR_PRESETS[v]
        gy, gc = p['gate']
        mctf_lvl(m, 0x108, (gy, gc, gc))                      # iso filter gate
        mctf_lvl(m, 0x158, (gy, gc, gc))                      # dir filter gate
        mctf_lvl(m, 0x218, (255, 255, 255))                   # overall max change 255 (no limit)
        m[0x210:0x213] = b'\xff\xff\xff'                       # tf_max_change_from_SB2: 255 (no limit)
        struct.pack_into('<H', m, 0xc0, 2 * 0x111)            # wide weight 2, narrow 6: the two sum to 8 (unity);
        struct.pack_into('<H', m, 0xc8, 6 * 0x111)            # measured about 4 % less noise than 0 and 8
        m[0xd0] = 1 | 1 << 2 | 1 << 4                          # temporal_neighbor 1 per channel (measured: as low
                                                              # noise as 2 or 3; 0 is noisier)
        m[0x1a8:0x1ab] = b'\x02\x02\x02'                       # dir_filter_sel 2 (measured: 0 and 1 lose 12-24 %
                                                              # of the detail at the same noise)
        m[0x2c0] = p['counts'][0] | p['counts'][1] << 4
        for ch, ca in enumerate((p['a'], round(p['a'] * 0.75), round(p['a'] * 0.75))):
            for f in ('B', 'C'):
                m[MCTF_CURVE[f] + 64 * ch: MCTF_CURVE[f] + 64 * ch + 64] = mctf_curve(max(ca, 32), p['t0'], p['t1'])
            m[MCTF_CURVE['D'] + 64 * ch: MCTF_CURVE['D'] + 64 * ch + 64] = b'\xff' * 64
        m[0x30:0x33] = bytes([p['mc_y'], p['mc_c'], p['mc_c']])   # max_change_B
        m[0x38:0x3b] = bytes([p['mc_y'], p['mc_c'], p['mc_c']])   # max_change_C
    cfg.tables['video_mctf'] = m


# --- the other options ----------------------------------------------------------------------------------------------
def opt_black_level(cfg, v):
    """Written as negative global offsets (LISO +0xbc..+0xc8), which the DSP adds to the raw values."""
    n = int(v)
    if not 0 <= n <= 2047:
        raise SystemExit(f'black_level {n}: 0..2047')
    struct.pack_into('<4i', cfg.liso, 0xbc, -n, -n, -n, -n)


def opt_wb_gain(cfg, v):
    """LISO +0xe4..+0xf0: R/Gr/Gb/B, u32 16.16 (1.0 = 0x10000)."""
    parts = [float(x) for x in v.split(',')]
    if len(parts) not in (2, 3):
        raise SystemExit(f'wb_gain {v!r}: R,B or R,G,B')
    r, g, b = (parts[0], 1.0, parts[1]) if len(parts) == 2 else parts
    for name, x in (('R', r), ('G', g), ('B', b)):
        if not 0.25 <= x <= 8.0:
            raise SystemExit(f'wb_gain {name}={x}: 0.25..8')
    struct.pack_into('<4I', cfg.liso, 0xe4, round(r * 65536), round(g * 65536), round(g * 65536), round(b * 65536))


def opt_dbp(cfg, v):
    """Mode in LISO +0x1f0 bits 0-2 (1..4 = hot/dark, first/second order), both 384-entry threshold tables flat."""
    if v == 'off':
        cfg.liso[0x1f0] &= ~0x07
        cfg.tables['dbp_dark'] = cfg.tables['dbp_hot'] = None
        return
    parts = v.split(',')
    if len(parts) != 2:
        raise SystemExit(f'dbp {v!r}: off or MODE,THRESH')
    mode, t = int(parts[0]), int(parts[1])
    if not (1 <= mode <= 4 and 0 <= t <= 255):
        raise SystemExit(f'dbp {v!r}: mode 1..4, threshold 0..255')
    cfg.liso[0x1f0] = (cfg.liso[0x1f0] & ~0x07) | mode
    cfg.tables['dbp_dark'] = bytearray(u16s([t] * 384))
    cfg.tables['dbp_hot'] = bytearray(u16s([t] * 384))


def opt_saturation(cfg, v):
    """The chroma gain curve (LISO +0x120, 128 x u16, 1024 = 1.0), flat."""
    x = float(v)
    if not 0 <= x < 4:
        raise SystemExit(f'saturation {x}: 0..3.9')
    cfg.tables['chroma_gain_curve'] = bytearray(u16s([round(1024 * x)] * 128))


def opt_rgb2yuv(cfg, v):
    """RGB->YUV matrix, Q10 (LISO +0x130). The default, bt601, is the studio-range BT.601 matrix (black at 16, white at
    235). The S3L encoder tags its streams full range with BT.709 colours (ffprobe: yuvj420p, pc, bt709), so players
    show that signal with black lifted to 16 and white dimmed to 235. bt709-full is Y = 0.2126 R + 0.7152 G + 0.0722 B
    over 0..255, which is what such a stream is displayed with."""
    if v == 'bt601':
        rgb2yuv_bt601(cfg)
    elif v == 'bt709-full':
        m = ((0.2126, 0.7152, 0.0722), (-0.1146, -0.3854, 0.5), (0.5, -0.4542, -0.0458))
        struct.pack_into('<9h3h', cfg.liso, 0x130, *[round(c * 1024) for row in m for c in row], 0, 128, 128)
    else:
        raise SystemExit(f'rgb2yuv {v!r}: bt601 or bt709-full')


def opt_tone_curve(cfg, v):
    """The tone table (LISO +0x12c): 256 entries, one 10-bit value per colour channel, indexed by the 8-bit input."""
    if v == 'off':
        tone_table(cfg)                    # undo a curve an earlier preset or option set
        return
    knots = []
    for part in v.split(','):
        a, sep, b = part.partition(':')
        if not sep:
            raise SystemExit(f'tone_curve {v!r}: IN:OUT,IN:OUT,...')
        knots.append((float(a), float(b)))
    if len(knots) < 2:
        raise SystemExit(f'tone_curve {v!r}: at least two knots')
    for (x0, y0), (x1, y1) in zip(knots, knots[1:]):
        if not x0 < x1 or y1 < y0:
            raise SystemExit(f'tone_curve {v!r}: IN must rise and OUT must not fall')
    if not all(0 <= x <= 255 and 0 <= y <= 255 for x, y in knots):
        raise SystemExit(f'tone_curve {v!r}: values 0..255')

    def curve(x):
        if x <= knots[0][0]:
            return knots[0][1]
        for (x0, y0), (x1, y1) in zip(knots, knots[1:]):
            if x <= x1:
                return y0 + (y1 - y0) * (x - x0) / (x1 - x0)
        return knots[-1][1]
    tone_table(cfg, [min(1023, max(0, round(1023 * curve(i) / 255))) for i in range(256)])


OPTIONS = {'black_level': opt_black_level, 'wb_gain': opt_wb_gain, 'dbp': opt_dbp,
           'sharpness': opt_sharpness, 'temporal_nr': opt_temporal_nr, 'saturation': opt_saturation,
           'rgb2yuv': opt_rgb2yuv, 'tone_curve': opt_tone_curve}
GEOMETRY = {'width': (2688, 16, 4096), 'height': (1520, 8, 4096), 'bayer': (1, 0, 3)}


def load_preset(path):
    out = []
    with open(path) as f:
        for n, line in enumerate(f, 1):
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            k, sep, v = line.partition(':')
            if not sep or not k.strip() or not v.strip():
                raise SystemExit(f'{path}:{n}: not a "key: value" line: {line}')
            out.append((k.strip(), v.strip()))
    return out


def main(argv):
    if len(argv) < 2 or argv[1].startswith('-'):
        sys.exit(__doc__)
    out, args, sets, show = argv[1], argv[2:], [], False
    while args:
        a = args.pop(0)
        if a == '--show':
            show = True
        elif a in ('--preset', '--set') and args:
            v = args.pop(0)
            if a == '--preset':
                sets += load_preset(v)
            else:
                k, sep, val = v.partition('=')
                if not sep or not k or not val:
                    sys.exit(f'--set {v!r}: key=value')
                sets.append((k, val))
        else:
            sys.exit(f'{a}: unknown argument, or missing its value\n\n{__doc__}')
    geom = {k: d for k, (d, _, _) in GEOMETRY.items()}
    for k, v in sets:
        if k not in OPTIONS and k not in GEOMETRY:
            sys.exit(f'unknown option {k}; known: {", ".join(list(GEOMETRY) + list(OPTIONS))}')
    try:
        for k, v in sets:
            if k in GEOMETRY:
                geom[k] = int(v, 0)
                _, lo, hi = GEOMETRY[k]
                if not lo <= geom[k] <= hi:
                    sys.exit(f'{k} {geom[k]}: {lo}..{hi}')
        cfg = Config(**geom)
        for k, v in sets:
            if k in OPTIONS:
                OPTIONS[k](cfg, v)
    except ValueError as e:
        sys.exit(f'{k}: bad value {v!r} ({e})')
    used, place = cfg.write(out)
    print(f'{out}: {len(OPS)} ops, {used:#x} image bytes, file {os.path.getsize(out)} B')
    if show:
        for n, o in sorted(place.items(), key=lambda x: x[1]):
            print(f'  IMG+{o:#08x}  {n}')


if __name__ == '__main__':
    main(sys.argv)
