let configData = { majestic: {}, wfb: {} };

async function loadYAML(url, setter) {
	const response = await fetch(url);
	const text = await response.text();
	setter(jsyaml.load(text));
}

async function uploadYAML(data, location) {
	const yamlData = jsyaml.dump(data);
	await fetch('/upload', {
		method: 'POST',
		headers: { 'File-Location': location },
		body: yamlData
	});
}

async function runCommand(command) {
	await fetch('/command', {
		method: 'POST',
		headers: { 'Run-Command': command }
	});
}

function syncForm(data, formPrefix, mode) {
	Object.keys(data).forEach((section) => {
		Object.keys(data[section]).forEach((key) => {
			const field = document.getElementsByName(formPrefix + '.' + section + '.' + key)[0];
			if (!field) {
				return;
			}

			const isCheckbox = field.type === "checkbox";
			const value = data[section][key];

			if (mode === "setup") {
				if (isCheckbox) {
					field.checked = value === true;
				} else {
					field.value = value;
				}
			} else if (mode === "update") {
				if (isCheckbox) {
					data[section][key] = field.checked;
				} else {
					data[section][key] = field.value;
				}
			}
		});
	});
}

function executeDelay(button, command) {
	button.disabled = true;
	runCommand(command);
	setTimeout(() => {
		button.disabled = false;
		location.reload();
	}, 1000);
}

function addClickListener(buttonId, command, configKey) {
	document.getElementById(buttonId).addEventListener('click', function () {
		if (configKey && configData[configKey]) {
			syncForm(configData[configKey], configKey, "update");
			var yamlPath = '/etc/' + configKey + '.yaml';
			uploadYAML(configData[configKey], yamlPath);
		}
		executeDelay(this, command);
	});
}

addClickListener('apply-video-button', 'killall -1 majestic', 'majestic');
addClickListener('apply-wfb-button', 'wifibroadcast start', 'wfb');
addClickListener('reset-video-button', 'wifibroadcast resetv');
addClickListener('reset-wfb-button', 'wifibroadcast resetw');

function populateSelect(id, start, end, step) {
	const select = document.getElementById(id);
	for (let i = start; i <= end; i += step) {
		const option = document.createElement('option');
		option.value = i;
		option.textContent = i;
		select.appendChild(option);
	}
}

populateSelect('bitrate', 1024, 20480, 1024);
populateSelect('txpower', 1, 58, 1);
populateSelect('wfb_index', 0, 8, 1);
populateSelect('tun_index', 0, 8, 1);

loadYAML('/etc/majestic.yaml', (data) => {
	configData.majestic = data;
	syncForm(configData.majestic, 'majestic', "setup");
});

loadYAML('/etc/wfb.yaml', (data) => {
	configData.wfb = data;
	syncForm(configData.wfb, 'wfb', "setup");
});
