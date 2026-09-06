// The USB page: pick what the single port is for, and make it so.
//
// Two owners, two calls, in this order and not the other one:
//
//   1. cgi-bin/j/usb.cgi?role=...   the DWC3 role. Releases majestic's hold on
//                                   the video node, flips the port, and leaves
//                                   the pipeline down.
//   2. POST /api/v1/config          the two majestic flags. Saving them is also
//                                   what rebuilds the pipeline, so it lands in
//                                   the new role first time.
//
// Doing it the other way round rebuilds the pipeline against the old role,
// which then has to be rebuilt again; doing only the first leaves the camera
// with no video until j/usb.cgi's own safety reload fires.
//
// Both flags are written on every apply, never just the one being turned on.
// majestic refuses to start the gadget while the second camera is enabled --
// and it decides that from the flag, not from whether the second camera
// actually started -- so a stale `true` on the other side is not a leftover, it
// is a camera that does neither thing.
(() => {
	'use strict';

	const ROLE_KEYS = {
		// role -> what majestic should have enabled in it
		host: { usbcam: 'true', uvcgadget: 'false' },
		device: { usbcam: 'false', uvcgadget: 'true' },
		off: { usbcam: 'false', uvcgadget: 'false' },
	};

	const form = document.getElementById('usb-form');
	const apply = document.getElementById('usb-apply');
	const msg = document.getElementById('usb-msg');
	const statusEl = document.getElementById('usb-status');
	if (!form || !statusEl) return;

	const text = (el, s) => { el.textContent = s; };

	function esc(s) {
		const d = document.createElement('div');
		d.textContent = s == null ? '' : String(s);
		return d.innerHTML;
	}

	function checked() {
		const r = form.querySelector('input[name="usb_role"]:checked');
		return r ? r.value : null;
	}

	function select(role) {
		const r = form.querySelector('input[name="usb_role"][value="' + role + '"]');
		if (r) r.checked = true;
	}

	function usbStatus() {
		return rawFetch('cgi-bin/j/usb.cgi', { credentials: 'same-origin' })
			.then(r => r.ok ? r.json() : { ok: false })
			.catch(() => ({ ok: false }));
	}

	// null, not {}, when the read fails.
	//
	// An empty object is not neutral here: every flag read off it comes back
	// undefined, which compares as "off", which this page would then print as
	// a fact and preselect "Nothing" from. Press Apply on that and it writes
	// both flags false -- turning off a second camera that was working, because
	// the page could not reach the daemon for a moment. Unknown has to stay
	// unknown all the way to the screen.
	function config() {
		// Not mjConfig(): that memoises, and this page's whole job is to change
		// the values it would be caching.
		return apiFetch('/api/v1/config.json', { credentials: 'same-origin' })
			.then(r => r.ok ? r.json() : null)
			.catch(() => null);
	}

	// The node has to be written along with the flag, not left at whatever it
	// was. The kernel does not reuse a v4l2 minor freed earlier in the same
	// boot, so a port that was /dev/video0 as a webcam input comes back as
	// /dev/video1 when the gadget takes it — and majestic is told a path. Left
	// stale it opens nothing, says so once in the log, and presents a camera
	// that is switched on and invisible. j/usb.cgi reports the node it actually
	// landed on; this is where that gets used.
	function setFlags(role, node) {
		const want = ROLE_KEYS[role] || ROLE_KEYS.off;
		const body = {
			usbcam: { enabled: want.usbcam },
			uvcgadget: { enabled: want.uvcgadget },
		};
		if (node && role === 'device') body.uvcgadget.device = node;
		if (node && role === 'host') body.usbcam.device = node;
		return apiFetch('/api/v1/config', {
			method: 'POST',
			credentials: 'same-origin',
			headers: { 'Content-Type': 'application/json' },
			body: JSON.stringify(body),
		}).then(r => r.ok);
	}

	// What the port is doing, said the way a person would say it. The role on
	// its own is not the answer: a port in host mode with nothing plugged into
	// it and a port with a camera on it are the same role and different
	// situations, and only one of them is worth doing anything about.
	function render(st, cfg) {
		if (!st || !st.ok) {
			statusEl.innerHTML = mjNotice('warn',
				'<b>Cannot read the port</b> &mdash; this camera may not have a ' +
				'switchable USB port.');
			return;
		}

		const known = cfg !== null;
		const usbcam = known && String(mjGet(cfg, 'usbcam.enabled')) === 'true';
		const gadget = known && String(mjGet(cfg, 'uvcgadget.enabled')) === 'true';
		const onoff = v => (known ? (v ? 'on' : 'off') : 'cannot tell');
		const attached = (st.attached || '').trim();

		let rows = '';
		const row = (k, v) => { rows += '<div><b>' + esc(k) + ':</b> ' + v + '</div>'; };

		if (st.role === 'device') {
			row('Port', 'offered to a computer as a webcam');
			row('Offering it', known ? (st.gadget && gadget ? 'yes' : 'not right now')
				: 'cannot tell');
		} else {
			row('Port', 'ready for a webcam to be plugged in');
			row('Plugged in', attached ? esc(attached.split('\n')[0]) : 'nothing');
			row('Second camera', onoff(usbcam));
		}
		statusEl.innerHTML = rows;

		// Say the settings are unreadable rather than reporting them as off,
		// and take Apply away: writing a role from a page that cannot read the
		// current one is how a working camera gets switched off by accident.
		if (!known) {
			statusEl.innerHTML += mjNotice('warn',
				'<b>Cannot read this camera&rsquo;s settings</b> &mdash; the ' +
				'port is shown above, but what is using it is unknown until ' +
				'the camera answers again.');
			if (apply) apply.disabled = true;
			return;
		}
		if (apply) apply.disabled = false;

		// The two states worth saying something about, because in both of them
		// the camera looks configured and produces nothing.
		if (st.role === 'host' && usbcam && !attached)
			statusEl.innerHTML += mjNotice('warn',
				'<b>Nothing is plugged in</b> &mdash; the second camera is ' +
				'switched on and has no webcam to read.');
		if (st.role === 'device' && !gadget)
			statusEl.innerHTML += mjNotice('info',
				'<b>Not being offered</b> &mdash; the port is set up for a ' +
				'computer, but nothing is being sent down it.');
		if (st.error)
			statusEl.innerHTML += mjNotice('danger',
				'<b>The last change did not take</b> &mdash; ' + esc(st.error));
	}

	function refresh() {
		return Promise.all([usbStatus(), config()]).then(([st, cfg]) => {
			if (st && st.ok && cfg !== null && !checked()) {
				const usbcam = String(mjGet(cfg, 'usbcam.enabled')) === 'true';
				const gadget = String(mjGet(cfg, 'uvcgadget.enabled')) === 'true';
				// The role says what the port can do; the flags say whether
				// anything is using it. "Nothing" is the honest answer to a
				// port in host mode that nothing is reading.
				select(st.role === 'device' ? (gadget ? 'device' : 'off')
					: (usbcam ? 'host' : 'off'));
			}
			render(st, cfg);
			return { st: st, cfg: cfg };
		});
	}

	form.addEventListener('submit', ev => {
		ev.preventDefault();
		const role = checked();
		if (!role) return;

		// "off" is a host port with nothing reading it. There is no third role
		// in the controller, and parking it as a host is the harmless one: a
		// device-mode port with no gadget bound leaves a PC seeing a
		// half-present device.
		const wire = role === 'device' ? 'device' : 'host';

		apply.disabled = true;
		text(msg, 'Switching…');

		rawFetch('cgi-bin/j/usb.cgi?role=' + encodeURIComponent(wire),
			{ method: 'POST', credentials: 'same-origin' })
			.then(r => r.ok ? r.json() : { ok: false })
			.then(st => {
				if (!st.ok)
					throw new Error(st.error || 'the port did not change');
				return setFlags(role, st.video);
			})
			.then(saved => {
				if (!saved)
					throw new Error('the port changed, but the settings did not save');
				// Not "Done." yet. A 200 from the config write says the daemon
				// accepted it, not that the pipeline came back in the new role
				// -- and the rebuild is exactly the part that can lose a race
				// with USB enumeration. Ask the camera before claiming.
				text(msg, 'Applied, checking\u2026');
				return refresh();
			})
			.then(res => {
				const st = res && res.st, cfg = res && res.cfg;
				const want = ROLE_KEYS[role] || ROLE_KEYS.off;
				const roleOk = st && st.ok &&
					st.role === (role === 'device' ? 'device' : 'host');
				const flagsOk = cfg !== null && cfg !== undefined &&
					String(mjGet(cfg, 'usbcam.enabled')) === want.usbcam &&
					String(mjGet(cfg, 'uvcgadget.enabled')) === want.uvcgadget;
				text(msg, roleOk && flagsOk ? 'Done.'
					: 'Applied, but the camera does not report it yet.');
			})
			.catch(err => {
				text(msg, '');
				statusEl.innerHTML = mjNotice('danger',
					'<b>That did not work</b> &mdash; ' + esc(err.message)) +
					statusEl.innerHTML;
				return refresh();
			})
			.then(() => { apply.disabled = false; });
	});

	refresh();
})();
