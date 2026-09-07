// The USB page: pick what the single port is for, and make it so.
//
// Two owners, two calls, in this order and not the other one:
//
//   1. /cgi-bin/j/usb.cgi?role=...  the DWC3 role. Releases majestic's hold on
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

	// Absolute, like every other j/ CGI call in this UI. Relative, it resolved
	// against the directory of the page asking — which IS /cgi-bin/ — so the
	// `cgi-bin/` in the path doubled and every request went to
	// /cgi-bin/cgi-bin/j/usb.cgi and 404'd. The page could never read the port,
	// and could never change it either (the same URL below).
	const USB_CGI = '/cgi-bin/j/usb.cgi';

	// null, not { ok: false }, when the request does not complete.
	//
	// The same rule config() states below, and for the same reason: the CGI
	// sends `ok:false, supported:false` for a camera that genuinely has no
	// switchable port, and folding a 404, a 500 or an unreachable camera into
	// that same value made the page answer a question about the HARDWARE with
	// what was really "nobody replied". It printed "this camera may not have a
	// switchable USB port" at cameras whose port was working perfectly — which
	// is precisely how the doubled URL above stayed hidden, since the one
	// symptom it produced read as a statement about the board.
	function usbStatus() {
		return rawFetch(USB_CGI, { credentials: 'same-origin' })
			.then(r => r.ok ? r.json() : null)
			.catch(() => null);
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
	// You cannot change what you cannot read. One rule, decided before any of
	// the early returns below, because there are two ways to be in the dark --
	// the port endpoint failing and the settings failing -- and an earlier
	// version only covered the second. With a role already selected from a
	// refresh that worked, the first left a stale selection armed.
	function setActionable(ok) {
		if (apply) apply.disabled = !ok;
	}

	function render(st, cfg) {
		const portKnown = !!(st && st.ok);
		const known = cfg !== null && cfg !== undefined;
		setActionable(portKnown && known);

		// Three different answers, and they were one. `null` is nobody having
		// replied, which says nothing about the port; `supported:false` is the
		// camera saying it has no switchable one; anything else with `ok:false`
		// is the camera failing to answer a question it does understand.
		if (st === null) {
			statusEl.innerHTML = mjNotice('warn',
				'<b>Cannot reach the camera</b> &mdash; the port was not read, ' +
				'so nothing here describes it. This usually means the daemon ' +
				'is restarting; try again in a moment.');
			return;
		}
		if (!portKnown) {
			statusEl.innerHTML = mjNotice('warn', st.supported === false
				? '<b>No switchable USB port</b> &mdash; this camera\u2019s USB ' +
					'controller cannot change role, so there is nothing to set here.'
				: '<b>Cannot read the port</b> &mdash; the camera answered, but ' +
					'could not say what the port is doing.');
			return;
		}

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
			return;
		}

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

		rawFetch(USB_CGI + '?role=' + encodeURIComponent(wire),
			{ method: 'POST', credentials: 'same-origin' })
			.then(r => r.ok ? r.json() : { ok: false })
			.then(st => {
				// `error` is the failure, not `ok`. The two answer different
				// questions and the CGI means both: `ok` says it could read the
				// port, which stays true after a switch that did not take —
				// the role and the persisted value are still readable, and
				// render() below prints them. `error` is usb-mode having
				// returned nonzero.
				//
				// Asking only `ok` let a refused switch through to setFlags(),
				// which writes majestic's flags for the role that was ASKED
				// for: the controller stays in the old mode, the video config
				// moves to the new one, and the camera is left with the two
				// disagreeing. It could not happen while the URL above was
				// wrong — the POST 404'd and this threw on the missing page
				// instead — so fixing that URL is what makes this reachable,
				// and it belongs in the same change.
				if (!st.ok || st.error)
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
				// The role and the flags agreeing is not the webcam existing.
				// A device-mode port can sit with the gadget never composed and
				// no node behind it, which is the failure this page was built
				// to make visible -- so it cannot be what "Done." papers over.
				// Host asks for no such thing: a port with nothing plugged into
				// it has no node and is working correctly.
				// st is null when the check-back could not reach the camera,
				// which is a likely moment for it: the pipeline is being
				// rebuilt. Unknown is not proof, so it reads as not-yet rather
				// than throwing on a null and reporting the TypeError as the
				// reason the switch failed.
				const pipelineOk = role !== 'device' ||
					!!(st && st.gadget === true && st.video);
				text(msg, roleOk && flagsOk && pipelineOk ? 'Done.'
					: 'Applied, but the camera does not report it yet.');
			})
			.catch(err => {
				text(msg, '');
				statusEl.innerHTML = mjNotice('danger',
					'<b>That did not work</b> &mdash; ' + esc(err.message)) +
					statusEl.innerHTML;
				return refresh();
			})
			// Deliberately not re-enabling here. Every path above ends in
			// refresh(), and render() decides from what the camera actually
			// answered; an unconditional re-enable at the end of the chain
			// reversed that decision the moment it mattered.
			;
	});

	refresh();
})();
