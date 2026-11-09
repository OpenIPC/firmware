/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Automatic exposure.
 *
 * Meter. Each of the 256 grid cells gives a white-balanced linear luma, the
 * BT.601 sum of its channel means weighted by the current white-balance
 * gains. The scene level is the mean of the cell lumas, each limited to four
 * times the target so that a lamp or a window cannot dominate it. Two
 * histogram constraints then adjust the exposure error, in stops:
 *  - Shadow lift: if the 10th percentile cell would sit more than three
 *    stops below the target, raise the exposure by up to half a stop.
 *  - Highlight protection: the cell maximum of the (budget + 1)th brightest
 *    cell must stay below the clip level, so at most the budget of cells
 *    can contain clipped pixels. It may lower the exposure by up to one
 *    stop below the mean-based value, and it overrides the shadow lift.
 *
 * Controller. The statistics of frame N show the exposure commanded a fixed
 * number of frames earlier. Each frame's measurement is normalised to the
 * newest command through a short history of commanded settings, so the
 * controller can decide every frame without waiting for its own writes to
 * appear. The error is a log2 ratio. Inside a stable zone nothing changes;
 * the zone is entered below 0.10 stop and left above 0.25 stop for three
 * frames, or at once above 0.75 stop. Outside it the controller closes half
 * of the error per decision, 80% beyond one stop, at most three stops.
 *
 * Allocation. The total exposure is split by a monotonic function:
 * integration time first, up to the frame length (quantised to mains
 * half-periods with anti-flicker on), then analogue gain to its knee, ISP
 * gain to its knee, the rest of the analogue range, the rest of the ISP
 * range.
 *
 * Slow shutter. With a longest frame above the nominal one, the analogue
 * gain first rises to the entry gain at the nominal frame; beyond it the
 * integration, and with it the frame, grows to the longest frame, and then
 * the gain chain continues. The state is left when the gain at the nominal
 * frame would fall to the lower exit gain: in the long-frame state the
 * allocation integrates longer at the exit gain, so on the way back the gain
 * falls first and the frame shortens afterwards, and the two thresholds keep
 * the frame rate from toggling.
 */
#include "ae-controller.h"

#define STOP			256	/* log2 Q8 */
#define STABLE_ENTER		26	/* 0.10 stop */
#define STABLE_LEAVE		64	/* 0.25 stop */
#define STABLE_BREAK		192	/* 0.75 stop */
#define STEP_LIMIT		(3 * STOP)
#define CLIP_LEVEL		3400	/* 12-bit, below the post-black-level full scale */
#define CLIP_LEVEL_Q4		(CLIP_LEVEL << 4)

static const uint32_t log_tab[17] = {
	0, 5732, 11136, 16248, 21098, 25711, 30109, 34312, 38336,
	42196, 45904, 49472, 52911, 56229, 59434, 62534, 65536,
};

static const uint32_t exp_tab[17] = {
	65536, 68438, 71468, 74632, 77936, 81386, 84990, 88752, 92682,
	96785, 101070, 105545, 110218, 115098, 120194, 125515, 131072,
};

int32_t fx_log2_q8(uint64_t val)
{
	uint32_t n, m, idx, frac, l;

	if (!val)
		return 0;
	n = 63 - (uint32_t)__builtin_clzll(val);
	m = n >= 16 ? (uint32_t)(val >> (n - 16)) : (uint32_t)(val << (16 - n));
	m -= 65536;
	idx = m >> 12;
	frac = m & 0xfff;
	l = log_tab[idx] + (uint32_t)(((uint64_t)(log_tab[idx + 1] - log_tab[idx]) * frac) >> 12);
	return (int32_t)(n << 8) + (int32_t)((l + 128) >> 8);
}

uint64_t fx_exp2_q16(int32_t q8)
{
	int32_t i;
	uint32_t f, idx, frac, m;

	q8 = clamp_i32(q8, -16 * STOP, 15 * STOP);
	i = q8 >> 8;		/* floor */
	f = (uint32_t)(q8 - i * STOP);
	idx = f >> 4;
	frac = f & 15;
	m = exp_tab[idx] + (((exp_tab[idx + 1] - exp_tab[idx]) * frac) >> 4);
	return i >= 0 ? (uint64_t)m << i : (uint64_t)m >> -i;
}

uint32_t fx_isqrt(uint64_t val)
{
	uint64_t result = 0, bit = 1ULL << 62;

	while (bit > val)
		bit >>= 2;
	while (bit) {
		if (val >= result + bit) {
			val -= result + bit;
			result = (result >> 1) + bit;
		} else {
			result >>= 1;
		}
		bit >>= 2;
	}
	return (uint32_t)result;
}

void ae_controller_set_digital_gain_range(struct ae_controller *ae,
					 uint32_t min, uint32_t max, uint32_t step)
{
	ae->digital_gain.min = min ? min : 64;
	ae->digital_gain.max = max >= ae->digital_gain.min ? max : ae->digital_gain.min;
	ae->digital_gain.step = step ? step : 1;
	ae->current_digital_gain = clamp_u32(ae->current_digital_gain,
					     ae->digital_gain.min, ae->digital_gain.max);
	ae->target_digital_gain = ae->current_digital_gain;
}

void ae_controller_set_current_digital_gain(struct ae_controller *ae, uint32_t gain)
{
	if (!ae->digital_gain.max)
		ae->digital_gain.max = 64;
	if (!ae->digital_gain.min)
		ae->digital_gain.min = 64;
	ae->current_digital_gain = clamp_u32(gain ? gain : 64,
					     ae->digital_gain.min, ae->digital_gain.max);
	ae->target_digital_gain = ae->current_digital_gain;
}

void ae_controller_set_defaults(struct ae_config *cfg, uint32_t target_luma)
{
	memset(cfg, 0, sizeof(*cfg));
	cfg->target_luma = clamp_u32(target_luma, 1, 4096);
	cfg->low_light_target = cfg->target_luma * DEFAULT_AE_LOW_LIGHT_TARGET /
				DEFAULT_AE_TARGET_LUMA;
	cfg->gain_knee = 512;		/* 8x analogue */
	cfg->digital_knee = 128;	/* 2x ISP */
	cfg->flicker_mode = AE_ANTIFLICKER_OFF;
	cfg->profile = AE_PROFILE_NORMAL;
	/*
	 * GC1054 at 25 and 12.5 frames/s, writes issued a few milliseconds
	 * after the statistics frame: integration and analogue gain 3 frames,
	 * ISP gain 2. A sensor write more than about 20 ms after the frame
	 * event at 25 frames/s takes one frame more.
	 */
	cfg->exposure_delay = 3;
	cfg->gain_delay = 3;
	cfg->digital_gain_delay = 2;
	cfg->clip_budget = 5;		/* 2% of the cells */
	cfg->exposure_margin = 5;
	cfg->slow_gain_enter = 256;	/* 4x analogue */
	cfg->slow_gain_leave = 171;	/* 2.67x: 0.58 stop below */
}

void ae_controller_init(struct ae_controller *ae, const struct ae_config *cfg)
{
	memset(ae, 0, sizeof(*ae));
	if (cfg)
		ae->config = *cfg;
	else
		ae_controller_set_defaults(&ae->config, DEFAULT_AE_TARGET_LUMA);
	if (ae->config.gain_delay > ae->config.exposure_delay)
		ae->config.gain_delay = ae->config.exposure_delay;
	if (!ae->config.digital_gain_delay ||
	    ae->config.digital_gain_delay > ae->config.exposure_delay)
		ae->config.digital_gain_delay = ae->config.gain_delay;
	ae->digital_gain.min = 64;
	ae->digital_gain.max = 64;
	ae->digital_gain.step = 1;
	ae->current_digital_gain = 64;
	ae->target_digital_gain = 64;
}

uint32_t ae_controller_total_gain(const struct ae_controller *ae)
{
	return (uint32_t)(((uint64_t)ae->current_analog_gain *
			   (ae->current_digital_gain ? ae->current_digital_gain : 64)) >> 6);
}

static uint64_t setting_total(const struct ae_setting *s)
{
	return (uint64_t)s->exposure * s->again * s->dgain;
}

void ae_controller_sync(struct ae_controller *ae, uint32_t sequence)
{
	ae->history[0] = (struct ae_setting){
		.sequence = sequence - ae->config.exposure_delay,
		.exposure = ae->current_exposure_lines,
		.again = ae->current_analog_gain,
		.dgain = ae->current_digital_gain,
	};
	ae->history_len = 1;
	ae->history_valid = ae->current_exposure_lines && ae->current_analog_gain;
	ae->slow = ae_controller_slow_shutter(ae) &&
		   ae->current_exposure_lines + ae->config.exposure_margin >
		   ae->config.nominal_frame_lines;
	ae->stable = false;
	ae->stable_count = ae->unstable_count = 0;
}

void ae_controller_commit(struct ae_controller *ae, uint32_t sequence)
{
	if (ae->history_len == AE_HISTORY) {
		memmove(&ae->history[0], &ae->history[1],
			(AE_HISTORY - 1) * sizeof(ae->history[0]));
		ae->history_len--;
	}
	ae->history[ae->history_len++] = (struct ae_setting){
		.sequence = sequence,
		.exposure = ae->target_exposure_lines,
		.again = ae->target_analog_gain,
		.dgain = ae->target_digital_gain,
	};
	ae->current_exposure_lines = ae->target_exposure_lines;
	ae->current_analog_gain = ae->target_analog_gain;
	ae->current_digital_gain = ae->target_digital_gain;
}

/* The setting statistics frame @sequence was exposed with. */
static const struct ae_setting *effective_setting(const struct ae_controller *ae,
						  uint32_t sequence)
{
	unsigned int i;

	for (i = ae->history_len; i-- > 1;)
		if ((int32_t)(sequence - ae->history[i].sequence) >=
		    (int32_t)ae->config.exposure_delay)
			return &ae->history[i];
	return &ae->history[0];
}

uint32_t ae_controller_effective_gain(const struct ae_controller *ae, uint32_t sequence,
				      uint32_t *again)
{
	const struct ae_setting *s;

	if (!ae->history_valid || !ae->history_len) {
		*again = ae->current_analog_gain;
		return ae_controller_total_gain(ae);
	}
	s = effective_setting(ae, sequence);
	*again = s->again;
	return (uint32_t)(((uint64_t)s->again * (s->dgain ? s->dgain : 64)) >> 6);
}

bool ae_controller_in_transition(const struct ae_controller *ae, uint32_t sequence)
{
	unsigned int i;
	int32_t first = ae->config.gain_delay < ae->config.digital_gain_delay ?
			ae->config.gain_delay : ae->config.digital_gain_delay;

	for (i = 1; i < ae->history_len; i++) {
		int32_t d = (int32_t)(sequence - ae->history[i].sequence);

		if (d >= first &&
		    d <= (int32_t)ae->config.exposure_delay)
			return true;
	}
	return false;
}

static void shutter_limits(const struct ae_controller *ae, uint32_t *min_out,
			   uint32_t *nominal_out, uint32_t *max_out);
static uint32_t get_flicker_period_lines(const struct ae_config *cfg);

bool ae_controller_slow_shutter(const struct ae_controller *ae)
{
	const struct ae_config *c = &ae->config;

	return c->max_frame_lines > c->nominal_frame_lines && c->nominal_frame_lines &&
	       c->profile != AE_PROFILE_MOTION;
}

static uint32_t frame_for(const struct ae_controller *ae, uint32_t exposure)
{
	uint32_t lines = exposure + ae->config.exposure_margin;

	if (lines < ae->config.nominal_frame_lines)
		lines = ae->config.nominal_frame_lines;
	return lines > ae->config.max_frame_lines ? ae->config.max_frame_lines : lines;
}

uint32_t ae_controller_target_frame_lines(const struct ae_controller *ae)
{
	return ae_controller_slow_shutter(ae) ? frame_for(ae, ae->target_exposure_lines) : 0;
}

uint32_t ae_controller_frame_lines(const struct ae_controller *ae, uint32_t sequence)
{
	uint32_t lines, f;
	unsigned int i, first = 0;

	if (!ae_controller_slow_shutter(ae))
		return 0;
	lines = frame_for(ae, ae->current_exposure_lines);
	/* The setting in effect and every newer one still on its way. */
	for (i = ae->history_len; i-- > 0;)
		if ((int32_t)(sequence - ae->history[i].sequence) >=
		    (int32_t)ae->config.exposure_delay) {
			first = i;
			break;
		}
	for (i = first; i < ae->history_len; i++) {
		f = frame_for(ae, ae->history[i].exposure);
		if (f > lines)
			lines = f;
	}
	return lines;
}

static int cmp_u32(const void *a, const void *b)
{
	uint32_t x = *(const uint32_t *)a, y = *(const uint32_t *)b;

	return x < y ? -1 : x > y;
}

/*
 * Target linear luma, Q4. In low light, once integration is at its limit,
 * the perceived target falls with log2 of the gain and reaches the low-light
 * target at the analogue gain knee: less brightness, less amplified noise.
 */
static uint32_t target_level(const struct ae_controller *ae)
{
	uint32_t t = ae->config.target_luma, lo, hi, longest, step;

	/*
	 * Counted from the nominal frame's limit. A longer frame counts as the
	 * gain it replaces, so the target stays a function of the total
	 * exposure across slow shutter.
	 */
	shutter_limits(ae, &lo, &hi, &longest);
	step = get_flicker_period_lines(&ae->config);
	if (step < ae->exposure.step)
		step = ae->exposure.step;
	if (ae->config.low_light_target && ae->config.low_light_target < t &&
	    ae->current_exposure_lines + step > hi) {
		uint32_t knee = ae->config.gain_knee > 64 ? ae->config.gain_knee : 512;
		uint32_t total = ae_controller_total_gain(ae);

		if (ae->current_exposure_lines > hi)
			total = (uint32_t)((uint64_t)total * ae->current_exposure_lines / hi);
		int32_t span = fx_log2_q8(knee) - fx_log2_q8(64);
		int32_t pos = total > 64 ? fx_log2_q8(total) - fx_log2_q8(64) : 0;

		if (pos > span)
			pos = span;
		t -= (t - ae->config.low_light_target) * (uint32_t)pos / (uint32_t)span;
	}
	/* Perceived level to linear 12-bit, then Q4. */
	return (uint32_t)(((uint64_t)t * t * 16) >> 12);
}

/* Exposure error in stops (Q8), positive when the frame is too dark. */
static bool meter(struct ae_controller *ae, const struct fh8626_grid *grid,
		  const uint32_t wb[3], int32_t *error)
{
	static uint32_t luma[GRID_CELLS], peak[GRID_CELLS];
	uint64_t sum = 0;
	uint32_t target = target_level(ae), cap = target * 4, kth;
	int32_t e, lift, protect;
	unsigned int i, clipped = 0;

	for (i = 0; i < GRID_CELLS; i++) {
		uint64_t y = ((uint64_t)306 * wb[0] * grid->r[i] +
			      (uint64_t)601 * wb[1] * grid->g[i] +
			      (uint64_t)116 * wb[2] * grid->b[i]) >> 19;

		luma[i] = y > 0xffffffU ? 0xffffffU : (uint32_t)y;
		sum += luma[i] < cap ? luma[i] : cap;
		peak[i] = grid->max[i];
		clipped += grid->max[i] >= CLIP_LEVEL;
	}
	ae->mean = (uint32_t)(sum / GRID_CELLS);
	ae->target = target;
	ae->clipped = clipped;
	ae->perceived = fx_isqrt(((uint64_t)ae->mean << 12) >> 4);
	qsort(luma, GRID_CELLS, sizeof(luma[0]), cmp_u32);
	qsort(peak, GRID_CELLS, sizeof(peak[0]), cmp_u32);
	ae->shadow = luma[GRID_CELLS / 10];
	kth = peak[GRID_CELLS - 1 - ae->config.clip_budget];
	ae->highlight = kth;

	/* A black frame says nothing about the scene level; go up 3 stops. */
	e = ae->mean ? fx_log2_q8(target) - fx_log2_q8(ae->mean) : STEP_LIMIT;

	/* Shadow lift: 10th percentile within 3 stops of the target. */
	if (ae->shadow) {
		lift = fx_log2_q8(target) - 3 * STOP - fx_log2_q8(ae->shadow);
		if (lift > e)
			e = lift < e + STOP / 2 ? lift : e + STOP / 2;
	}
	/* Highlight protection, at most one stop under the mean-based value. */
	if (kth) {
		protect = fx_log2_q8(CLIP_LEVEL) - fx_log2_q8(kth);
		if (kth >= CLIP_LEVEL)
			protect -= STOP / 4;	/* clipped: true level unknown */
		if (protect < e) {
			int32_t floor_e = (ae->mean ? fx_log2_q8(target) -
					   fx_log2_q8(ae->mean) : e) - STOP;

			e = protect > floor_e ? protect : floor_e;
		}
	}
	*error = e;
	return true;
}

static uint32_t get_flicker_period_lines(const struct ae_config *cfg)
{
	uint32_t period_ns;

	if (cfg->flicker_mode == AE_ANTIFLICKER_50HZ)
		period_ns = 10000000;
	else if (cfg->flicker_mode == AE_ANTIFLICKER_60HZ)
		period_ns = 8333333;
	else
		return 0;
	if (!cfg->line_time_ns)
		return 0;
	return (period_ns + cfg->line_time_ns / 2) / cfg->line_time_ns;
}

/*
 * Valid values satisfy (value - origin) % step == 0, origin being the control
 * minimum. An off-grid effective minimum cannot be rounded: V4L2 rounds to
 * the nearest step, which can sit below the effective minimum forever. Step
 * up to the next in-range grid point; if none exists, keep the highest grid
 * point that does not exceed the maximum.
 */
static uint32_t align_origin_step(uint32_t value, uint32_t origin, uint32_t step,
				  uint32_t lo, uint32_t hi)
{
	uint32_t down, n;
	uint64_t up;

	if (step <= 1 || hi < lo || value < origin || hi < origin)
		return value;
	down = value - (value - origin) % step;
	if (down >= lo)
		return down;
	if (lo <= origin)
		return origin;
	n = (uint32_t)(((uint64_t)(lo - origin) + step - 1) / step);
	up = (uint64_t)origin + (uint64_t)n * step;
	if (up <= hi)
		return (uint32_t)up;
	return hi - (hi - origin) % step;
}

/*
 * Integration limits in lines for the current profile: the shortest, the
 * longest inside the nominal frame, and the longest at all, which exceeds
 * the nominal one only with slow shutter.
 */
static void shutter_limits(const struct ae_controller *ae, uint32_t *min_out,
			   uint32_t *nominal_out, uint32_t *max_out)
{
	uint32_t max_shutter = ae->exposure.max, min_shutter = ae->exposure.min;
	uint32_t margin = ae->config.exposure_margin, frame, nominal;
	bool slow = ae_controller_slow_shutter(ae);

	/* The exposure control's range follows the current frame length. */
	if (slow && ae->config.max_frame_lines > margin)
		max_shutter = ae->config.max_frame_lines - margin;
	/* In the motion profile the integration is capped at 1/100 s. */
	if (ae->config.profile == AE_PROFILE_MOTION && ae->config.line_time_ns) {
		uint32_t motion_max = (10000000 + ae->config.line_time_ns / 2) /
				      ae->config.line_time_ns;

		if (motion_max < max_shutter)
			max_shutter = motion_max;
	}
	if (min_shutter < 16 && max_shutter >= 16)
		min_shutter = 16;
	if (max_shutter < min_shutter)
		max_shutter = min_shutter;
	/* Without slow shutter never stretch the frame. */
	nominal = max_shutter;
	frame = ae->config.nominal_frame_lines ? ae->config.nominal_frame_lines :
						 ae->config.frame_lines;
	if (frame > margin && nominal > frame - margin && frame - margin >= min_shutter)
		nominal = frame - margin;
	if (!slow)
		max_shutter = nominal;
	*min_out = min_shutter;
	*nominal_out = nominal;
	*max_out = max_shutter;
}

static uint32_t flicker_quantise(uint32_t lines, uint32_t step)
{
	return step && lines >= step ? lines / step * step : lines;
}

/* Split a total exposure (lines x Q6 x Q6) into the three actuators. */
static void allocate(struct ae_controller *ae, uint64_t total)
{
	uint32_t max_shutter, min_shutter, nominal_shutter;
	uint32_t gain_min, base_gain = 64, flicker_step, candidate, new_exp;
	uint32_t new_gain, new_dgain, dmin, dmax, knee, dknee, slow_gain = 0;
	uint64_t needed;

	shutter_limits(ae, &min_shutter, &nominal_shutter, &max_shutter);
	gain_min = ae->gain.min ? ae->gain.min : 64;
	if (gain_min < 64 && ae->gain.max >= 64)
		gain_min = 64;
	if (base_gain < gain_min)
		base_gain = gain_min;
	if (ae->gain.max && base_gain > ae->gain.max)
		base_gain = ae->gain.max;
	dmin = ae->digital_gain.min ? ae->digital_gain.min : 64;
	dmax = ae->digital_gain.max >= dmin ? ae->digital_gain.max : dmin;

	/* total is lines x Q6 x Q6; the sensor-side share is lines x Q6. */
	needed = total / dmin;
	if (needed < (uint64_t)min_shutter * base_gain)
		needed = (uint64_t)min_shutter * base_gain;

	/*
	 * Integrate whole mains half-periods when possible; the remainder goes
	 * to gain. In bright light shorter integration is allowed.
	 */
	flicker_step = get_flicker_period_lines(&ae->config);
	knee = clamp_u32(ae->config.gain_knee ? ae->config.gain_knee : ae->gain.max,
			 gain_min, ae->gain.max);

	/* Slow shutter state, decided on the gain at the nominal frame. */
	if (max_shutter > nominal_shutter) {
		uint64_t nominal = flicker_quantise(nominal_shutter, flicker_step);
		uint32_t enter = clamp_u32(ae->config.slow_gain_enter, base_gain, knee);
		uint32_t leave = clamp_u32(ae->config.slow_gain_leave, base_gain, enter);

		if (!ae->slow && needed > nominal * enter)
			ae->slow = true;
		else if (ae->slow && needed <= nominal * leave)
			ae->slow = false;
		slow_gain = leave;
	} else {
		ae->slow = false;
	}
	if (ae->slow) {
		/* Integrate longer at the exit gain, up to the longest frame. */
		candidate = (uint32_t)(needed / slow_gain > max_shutter ? max_shutter :
				       needed / slow_gain);
		if (candidate < nominal_shutter)
			candidate = nominal_shutter;
	} else {
		candidate = (uint32_t)(needed / base_gain > nominal_shutter ? nominal_shutter :
				       needed / base_gain);
	}
	new_exp = flicker_quantise(candidate, flicker_step);
	if (new_exp < min_shutter)
		new_exp = min_shutter;

	/* Gain still needed, Q6, including the ISP share. */
	needed = (total / new_exp + 32) >> 6;
	if (!needed)
		needed = 1;
	dknee = clamp_u32(ae->config.digital_knee ? ae->config.digital_knee : dmax,
			  dmin, dmax);
	if (dmax <= dmin) {
		new_gain = (uint32_t)clamp_u32((uint32_t)(needed > UINT32_MAX ? UINT32_MAX : needed),
					       gain_min, ae->gain.max);
		new_dgain = dmin;
	} else if (needed <= knee) {
		new_gain = clamp_u32((uint32_t)needed, gain_min, knee);
		new_dgain = dmin;
	} else if (needed * 64 <= (uint64_t)knee * dknee) {
		new_gain = knee;
		new_dgain = (uint32_t)((needed * 64 + knee / 2) / knee);
	} else if (needed * 64 <= (uint64_t)ae->gain.max * dknee) {
		new_dgain = dknee;
		new_gain = (uint32_t)((needed * 64 + dknee / 2) / dknee);
	} else {
		new_gain = ae->gain.max;
		needed = (needed * 64 + ae->gain.max / 2) / ae->gain.max;
		new_dgain = needed > dmax ? dmax : (uint32_t)needed;
	}

	new_exp = clamp_u32(new_exp, min_shutter, max_shutter);
	new_gain = clamp_u32(new_gain, gain_min, ae->gain.max);
	new_dgain = clamp_u32(new_dgain, dmin, dmax);
	new_exp = align_origin_step(new_exp, ae->exposure.min, ae->exposure.step,
				    min_shutter, max_shutter);
	new_gain = align_origin_step(new_gain, ae->gain.min, ae->gain.step,
				     gain_min, ae->gain.max);
	if (ae->digital_gain.step > 1)
		new_dgain -= (new_dgain - dmin) % ae->digital_gain.step;

	ae->target_exposure_lines = new_exp;
	ae->target_analog_gain = new_gain;
	ae->target_digital_gain = new_dgain;
}

bool ae_controller_update(struct ae_controller *ae, const struct fh8626_grid *grid,
			  const uint32_t wb[3])
{
	struct ae_setting cmd = {
		.exposure = ae->current_exposure_lines,
		.again = ae->current_analog_gain,
		.dgain = ae->current_digital_gain,
	};
	const struct ae_setting *eff;
	int32_t e, mag, step;
	uint64_t total, next;

	ae->target_exposure_lines = ae->current_exposure_lines;
	ae->target_analog_gain = ae->current_analog_gain;
	ae->target_digital_gain = ae->current_digital_gain;
	if (!ae->history_valid || !meter(ae, grid, wb, &e))
		return false;

	/* Normalise to the newest command: the frame shows an older one. */
	eff = effective_setting(ae, grid->sequence);
	total = setting_total(&cmd);
	if (!total || !setting_total(eff))
		return false;
	e -= fx_log2_q8(total) - fx_log2_q8(setting_total(eff));
	ae->error = e;
	mag = abs_i32(e);

	if (ae->stable) {
		if (mag >= STABLE_BREAK || (mag >= STABLE_LEAVE && ++ae->unstable_count >= 3)) {
			ae->stable = false;
			ae->unstable_count = 0;
		} else {
			if (mag < STABLE_LEAVE)
				ae->unstable_count = 0;
			return false;
		}
	}
	if (mag < STABLE_ENTER) {
		if (++ae->stable_count >= 2) {
			ae->stable = true;
			ae->stable_count = 0;
		}
		return false;
	}
	ae->stable_count = 0;

	step = mag > STOP ? e * 205 / 256 : e / 2;
	step = clamp_i32(step, -STEP_LIMIT, STEP_LIMIT);
	next = (total * fx_exp2_q16(step) + 32768) >> 16;
	allocate(ae, next ? next : 1);
	ae->saturated = setting_total(&(struct ae_setting){
		.exposure = ae->target_exposure_lines,
		.again = ae->target_analog_gain,
		.dgain = ae->target_digital_gain }) == total;
	return !ae->saturated;
}
