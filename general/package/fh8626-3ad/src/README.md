# Independent ISP statistics and 3A daemon

`fh8626-3ad` consumes the ISP subdevice's frame events and the zone statistics
control (`V4L2_CID_FH8626_GRID_STATS`) defined in
the kernel's `linux/fh8626-isp.h`. The capture application owns the V4L2
capture queue and streaming. This daemon waits when no frames arrive.

## Statistics

Every frame (`V4L2_EVENT_FRAME_SYNC`) the daemon reads one frame of zone
statistics: a 16x16 grid of cells, each with the sample sums of the R, Gr, Gb
and B sites, the sample count and the largest sample, plus a 24x16 motion
bitmap from the ISP's motion detector. The sums are taken after black-level
subtraction and the ISP digital gain, before white balance, in linear 12-bit.
The daemon reduces them to per-cell channel means (G is the mean of Gr and Gb).

Measured on the GC1054 board (2026-10-20, 25 and 12.5 fps, writes issued
right after a statistics frame): an integration time or analogue gain first
shows in the statistics 3 frames later, an ISP digital gain 2 frames later.
A sensor write issued more than about 20 ms after the frame event at 25 fps
(40 ms at 12.5 fps) takes one frame more; the daemon writes about 3 ms after
it. An earlier measurement with older drivers gave 4 and 3 frames. A frame in
which a global exposure change first shows sets every motion block.

## Automatic exposure

- **Meter.** Each cell's white-balanced luma is the BT.601 sum of its channel
  means weighted by the current white-balance gains. The scene level is the
  mean of the cells, each limited to four times the target, so a lamp or a
  window cannot dominate it. Two histogram constraints adjust the exposure
  error:
  - *Shadow lift*: if the 10th-percentile cell would be more than three stops
    below the target, the exposure rises by up to half a stop.
  - *Highlight protection*: the brightest sample of the 6th brightest cell must
    stay below 3400, so at most 5 cells (2%) clip. It may lower the exposure up
    to one stop below the mean-based value and overrides the shadow lift.
- **Target.** `-t` gives the perceived 12-bit level (default 1856, mid-grey
  after the tone curves); the linear target is `t² / 4096`. Once integration is
  at its maximum, the target falls with log2 of the total gain and reaches 1600
  (scaled with `-t`) at the analogue gain knee: less brightness, less amplified
  noise.
- **Control.** The error is a log2 ratio (Q8 stops). Statistics of frame N show
  the setting commanded `exposure_delay` frames earlier; a short history of
  commanded settings normalises each measurement to the newest command, so the
  loop decides every frame without waiting for its own writes. Each decision
  closes half the error (80% beyond one stop), at most three stops.
- **Hysteresis.** Below 0.10 stop for two frames AE is stable and writes
  nothing. It leaves the stable state above 0.25 stop for three frames, or at
  once above 0.75 stop.
- **Write timing.** Integration time is written when decided; each gain
  `exposure_delay - its delay` frames later (analogue at once, ISP gain one
  frame later), so all three first show in the same frame, without a
  half-applied step. `-L e,g[,d]` overrides the delays (default 3,3,2) for
  another sensor or driver.
- **Allocation.** A monotonic split of the total exposure: integration time
  first, from 16 lines to the nominal frame length minus the sensor's margin
  (1/100 s in the `motion` profile), rounded down to whole mains half-periods
  with 50 or 60 Hz selected; then analogue gain to 8x, ISP digital gain to 2x,
  the rest of the analogue range, the rest of the ISP range. Slow shutter
  inserts a longer frame into this order (below).
- **Other writers.** The daemon subscribes to `V4L2_EVENT_CTRL` value and range
  changes of the sensor's VBLANK, HBLANK, PIXEL_RATE, EXPOSURE and
  ANALOGUE_GAIN, and of the ISP's DIGITAL_GAIN and POWER_LINE_FREQUENCY. The
  core does not echo the daemon's own writes, so a value event is another
  client's write: the daemon restarts the history from the values read and
  waits until they show. Without control events it polls, as before (controls
  every 4 frames, timing every 16).
- The supported sensor is `gc1054`, the only one the ISP tuning tables are
  measured for. Its control contract is exposure in integration lines and
  analogue gain in Q6 (64 = 1x); limits and steps are queried from the driver.
  Automatic discovery requires exactly one supported sensor; otherwise use `-s`.
- **Timing.** The line time is `(width + HBLANK) / PIXEL_RATE`; frame lines are
  `height + VBLANK`. Without PIXEL_RATE/HBLANK, the frame interval divided by
  the frame lines is used. The integration margin is the frame length minus
  the exposure control's maximum (5 lines on GC1054). Timing is read at
  start-up and again on every timing control event, so a VBLANK set by another
  client (directly or through `VIDIOC_SUBDEV_S_FRAME_INTERVAL`) changes the
  line and frame timing, the exposure limits and the anti-flicker quantisation
  without a restart. The mains frequency is the ISP's standard
  `V4L2_CID_POWER_LINE_FREQUENCY` control: `-f` writes it at start-up. The
  1/100 s cap and 50/60 Hz quantisation need sensor timing; requesting them
  without it fails clearly.

## Slow shutter

When the sensor's VBLANK is writable and its exposure limit follows the frame
length, as on GC1054, AE may
lengthen the frame in low light.

- **Frames.** The *nominal* frame is the VBLANK found at start-up, or the last
  one another client set. The longest frame gives `-F` frames/s (default 12.5;
  `-F off` disables slow shutter), at most the VBLANK maximum.
- **Order, darkening.** Integration up to the nominal frame; analogue gain up
  to the entry gain (`-G`, default 4x); then integration, and with it the
  frame, up to the longest frame; then the usual gain chain (analogue to 8x,
  ISP gain, ...). Anti-flicker quantisation applies to the longer integration
  too; the frame is the integration plus the margin, so with 50/60 Hz it grows
  in whole half-periods.
- **Order, brightening, and hysteresis.** Slow shutter ends when the gain at
  the nominal frame would fall to the exit gain (default two thirds of the
  entry gain, 2.67x; `-G enter,exit`). While it is on, the integration beyond
  the nominal frame is allocated at the exit gain, so on the way back the gain
  falls first and the frame shortens afterwards; between exit and entry gain
  (0.58 stop) the state does not change.
- **Low-light target.** A longer frame counts as the gain it replaces, so the
  target stays a function of the total exposure.
- **Write order.** A longer frame is written one frame before the integration
  that needs it (VBLANK first shows in the frame period 2 frames after the
  write, integration 3). A shorter frame is written only after every
  integration still in effect or on its way fits it, that is `exposure_delay`
  frames after the last longer one was written. The sensor driver also clamps
  the exposure into a shorter frame. The integration never exceeds the frame.
- **Exit.** On a clean stop the daemon writes the nominal VBLANK back.

## Automatic white balance

Chroma is measured in log2 space before white balance: x = log2(B/G),
y = log2(R/G). The sensor's four reference white points (B/G, R/G in Q12:
(1252,3768), (1635,3026), (2217,2456), (2923,2081), warm to cool), extended by
30% beyond each end, approximate its illuminant locus as a polyline.

- **Usable cells**: green mean at least 32 (12-bit) and brightest sample below
  3600, so no channel is clipped.
- **Grey zone** (`whitepoint` mode): cells within 48 (Q8, about 14% in each
  ratio) of the locus are candidate grey surfaces, weighted by
  `(48² − d²) · min(brightness, 1024)`. Strongly coloured cells carry no weight.
- **Grey edge**: summed channel differences between neighbouring cells with at
  least 12% green contrast, both near the locus, estimate the illuminant from
  edges. With 16 or more such pairs and a result near the locus, it counts for
  half of the estimate.
- **Locus constraint**: the estimate is pulled to within 12 (about 3.5%) of the
  locus. With fewer than 8 grey cells, the mean of all usable cells projected
  onto the locus is used at a quarter of the confidence; with fewer than 8
  usable cells the gains hold.
- **Start-up**: gains taken over from the ISP that are not near the locus (the
  unity reset gains) are replaced at once by the third reference white point.
  Before this, a scene without grey cells kept unity gains and a green cast.
- **Temporal**: an exponential filter, a quarter per frame scaled by confidence
  (number of grey cells, full at 64), half for changes larger than a quarter
  stop. The applied white point holds until the filtered estimate is more than
  8 (about 2%) away for three frames, or 32 at once, then tracks it until it
  stops moving.
- **Gains**: the inverse neutral ratios, normalised so that the smallest gain
  is unity; a clipped white then reaches full scale in all three channels.
  Gains are Q9, 1..8191, including green (`V4L2_CID_FH8626_WB_GAIN_G`).
- `grayworld` takes the mean of all usable cells, without zone or locus.

The colour correction matrix follows the white point (see below).

## Motion and temporal noise reduction

The motion level (0..256) drives the temporal/spatial noise budget:

- Two measurements, the larger wins: the share of the ISP motion blocks that
  are set, and the share of cells whose green mean changed by more than
  `mean/16 + 2 + gain/16` levels after removing the global level change.
- Frames in which an exposure or white-balance change first shows are skipped,
  and a frame with nearly all blocks set but few changed cells is a global
  change, not motion.
- At high gain noise alone sets motion blocks, in bursts. The block share that
  noise gives is learned as a running mean and mean deviation (1/64 per frame,
  restarted when the gain changes by more than 2x); only the excess over
  mean + 3 deviations that two frames in a row show counts. Motion lasting
  longer than a few seconds is slowly learned as noise.
- Three times the share, capped, is the motion level; it rises at once and
  falls by an eighth per frame.

With motion, the temporal blend (`V4L2_CID_FH8626_NR3D_BLEND`, the current
frame's share) rises from its table value towards 96, so moving objects do not
trail; the Bayer NR strength grows by up to 50% and the luma NR strength by up
to 25% to carry the noise the temporal filter no longer removes. In a static
scene above 8x the luma NR is 15% lighter, as the temporal filter carries the
noise.

## Scheduled ISP tuning

`isp-tuning.c` (GC1054). Every frame the daemon derives these parameters and
writes only the controls that changed. It uses the gain of the frames the new
values will apply to: ISP parameters written now first show `gain_delay`
frames later, like a sensor gain written now, so the black level and noise
filters change in the same frame as the gain. Gain tables hold one entry per
octave from 1x and are interpolated linearly within an octave.

- **Black level** (`V4L2_CID_FH8626_BLACK_LEVEL`), indexed by analogue gain
  (1x to 32x) and integration time (20, 40 and 80 ms, linear in between); ISP
  gain does not move the pedestal. At 1x to 4x it is about 129 everywhere; at
  32x R/B fall to 123/118 at 20 ms, 116/106 at 40 ms and 92/76 at 80 ms.
  Measured in a dark room with the black level set to 0. The table depends on
  the sensor's own black-level setting in its mode table (pedestal near 128);
  a 30 fps mode table that sets it near 13, which the ISP then over-subtracted,
  crushing shadows and turning dim areas green.
- **Low-light profile**: entered after 25 frames at a total gain of 16x or
  more, left after 25 frames at 6x or less. It selects the night temporal NR
  tables and lowers saturation by 15%.
- **Temporal NR**: strength `((6 + 6g/1024) · s + 8) >> 4`, scale `s` day
  {2,3,4,6,8,10,16,20} / low light {2,3,4,5,12,16,16,16}; noise
  `((k·g² >> 20) + k + 8) >> 4` with k = 8 day, 6 low light; shift 6; motion
  weight 32; blend day {16,16,16,16,16,20,26,32} / low light
  {28,28,28,28,28,28,32,32} plus the motion term; coefficient set 4.
- **Bayer NR**: strength `(s · (16 + 16g/4096) + 8) >> 4` times the motion
  factor, noise `(s · ((64g² >> 22) + 64) + 8) >> 4`, `s` from
  {2,4,5,8,12,16,20,24}, shift 9.
- **Colour correction** (`V4L2_CID_FH8626_CCM`): four matrices calibrated at
  the four reference white points. The white point follows from the WB gains;
  the two matrices at the ends of the nearest locus segment are blended
  linearly.
- **Highlight chroma threshold** (`V4L2_CID_FH8626_HLR_THRESHOLD`):
  `Σ w[c] · min(gain) / gain[c]` with the BT.601 weights (306, 601, 116),
  clamped to 0..1023: full scale for a neutral balance, falling as the gains
  spread, so highlights where one raw channel has clipped lose their chroma
  instead of taking on the gain ratio as a tint.
- **Luma tone curve** (`V4L2_CID_FH8626_GAMMA_Y`): the GC1054 160-point curve,
  written once at start-up.
- Tables with one entry per octave from 1x to 256x:
  - Luma NR (`V4L2_CID_FH8626_YNR_*`): thresholds 16/16/20; band strength
    (Q6) {6,10,16,24,36,48,56,64,128} for band 0 and
    {0,4,8,16,32,48,56,64,64} for bands 1 and 2, times the motion factor;
    noise levels at mid-dark {5,10,12,16,24,32,36,40,47} and mid-grey
    {3,8,14,20,30,40,44,48,52}, the latter raised to the former.
  - Chroma NR (`V4L2_CID_FH8626_CNR_*`): strength {0,0,0,5,16,48,64,64,64},
    thresholds lower {0,0,0,0,0,4,6,10,16}, upper {0,0,0,0,0,8,10,16,24}.
  - Edge enhancement (`V4L2_CID_FH8626_APC_*`): detail/edge limits
    {112,112,96,96,80,80,80,80,12} / {170,170,160,160,150,150,150,150,52},
    gains {7,7,6,6,5,4,4,3,3} / {8,8,7,7,6,5,5,4,4}. `V4L2_CID_SHARPNESS`
    still scales the limits.
  - Saturation (`V4L2_CID_FH8626_SATURATION_SCALE`, Q6):
    {72,68,65,62,58,55,36,8,8}, multiplied with `V4L2_CID_SATURATION`.

A block whose controls the ISP does not expose is skipped.

## Current boundaries

- AWB, the CCM and the motion response to real moving objects have not been
  checked on a lit, coloured or moving scene: the test scene is dark and
  static.
- There is no day/night (IR) switch. The low-light profile is gain scheduling
  of the colour pipeline; IR operation with the IR-cut filter open would need
  its own monochrome profile and IR-cut ownership.
- The black-level table and the NR tables are calibrated for the GC1054.
- There is no Bayes/CCT estimator; `-m bayes` and `-f auto` report errors.
- `-A off` and `-W off` are final: the ISP's `V4L2_CID_EXPOSURE_AUTO` and
  `V4L2_CID_AUTO_WHITE_BALANCE` controls only pause a loop the command line
  enabled. A paused or disabled loop follows the controls read back for the
  tuning but never writes them.

## Service and CLI

The rootfs installs `/etc/init.d/S80fh8626-3ad`. It passes
`-F "$SLOW_SHUTTER_MIN_FPS"` from `/etc/default/fh8626-3ad` (12.5; `off`
disables slow shutter).

```text
fh8626-3ad [-i ISP-subdev] [-s sensor-subdev] [-t target]
           [-f 50|60|off] [-m whitepoint|grayworld] [-p normal|motion]
           [-A auto|off] [-W auto|off] [-L exp,gain[,isp]] [-F fps|off]
           [-G enter[,exit]] [-P pidfile] [-b] [-o] [-v]

-i <dev>         ISP subdevice, auto-detected when unique
-s <dev>         Sensor subdevice, auto-detected when unique
-t <level>       Perceived 12-bit AE target, 1..4096 (default 1856)
-f <50|60|off>   Flicker mode (default off; 50/60 require sensor timing)
-m <mode>        AWB mode (default whitepoint; wp/gw aliases accepted)
-p <normal|motion> Exposure profile (default normal; motion requires sensor timing)
-A <auto|off>    AE loop ownership (default auto)
-W <auto|off>    AWB loop ownership (default auto)
-L <e,g[,d]>     Frames from a write to the statistics showing it: integration,
                 analogue gain, ISP gain (default 3,3,2; d defaults to g)
-F <fps|off>     Slow shutter: slowest frame rate in low light (default 12.5)
-G <e[,x]>       Analogue gain entering and leaving slow shutter (default 4,2.67)
-P <pidfile>     Absolute path for writing/removing the service PID file after setup
-b               Background daemon
-o               Observe statistics without changing controls
-v               Debug logging, one line per loop and frame
```

