#!/bin/sh
# Regression check: assert that every kernel-module package selected in
# the build's .config produced its expected .ko under output/target/lib/modules/.
#
# Catches per-package merge regressions like #2032 where extra/wireguard.ko
# silently disappeared from firmware on hi3516cv200/hi3518ev200 builds.
#
# CI-only — not part of the local make flow, so contributors experimenting
# locally don't get blocked. Add new (config var, .ko) pairs to the list
# below whenever a new kernel-module package lands.
set -eu

CONFIG="${1:-output/.config}"
TARGET_DIR="${2:-output/target}"

if [ ! -f "$CONFIG" ]; then
	echo "check_target_modules: config file not found: $CONFIG" >&2
	exit 1
fi

if [ ! -d "$TARGET_DIR/lib/modules" ]; then
	echo "check_target_modules: no /lib/modules in $TARGET_DIR — nothing to verify"
	exit 0
fi

fail=0

# config-var → expected .ko filename
# Add new pairs here when a kernel-module package is introduced.
check_module() {
	var="$1"
	ko="$2"
	grep -q "^${var}=y" "$CONFIG" || return 0
	if find "$TARGET_DIR/lib/modules" -name "$ko" | grep -q .; then
		echo "OK: $ko present (${var}=y)"
	else
		echo "MISSING: ${var}=y but $ko not found under $TARGET_DIR/lib/modules/" >&2
		fail=1
	fi
}

check_module BR2_PACKAGE_WIREGUARD_LINUX_COMPAT wireguard.ko

if [ "$fail" -ne 0 ]; then
	echo "check_target_modules: regression detected" >&2
	exit 1
fi
echo "check_target_modules: all expected kernel modules present"
