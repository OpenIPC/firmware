# OpenIPC firmware build manifest

This branch is maintained automatically by
[.github/workflows/manifest.yml](https://github.com/OpenIPC/firmware/blob/master/.github/workflows/manifest.yml).

Generated files (do not edit by hand):

- `manifest.json` — full build index (JSON; for hosts, agents, CI tools).
- `manifest.flat` — whitespace-delimited index parseable by busybox shell
  (for on-device `sysupgrade --channel`/`--build`/`--list-builds`).

URLs:

- <https://openipc.github.io/firmware/manifest.json>
- <https://openipc.github.io/firmware/manifest.flat>

See the redesign plan: PR #2111 (preflight gate + dated releases) and follow-ups
introduce the aggregator that writes these files.
