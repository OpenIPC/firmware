#!/bin/sh
# Check that an ASan toolchain can actually sanitize, before anyone trusts it.
#
# A toolchain that builds is not a toolchain that sanitizes, and the gap between
# the two is not theoretical. Of the six libsanitizer defects that stood between
# an Ingenic camera and a working ASan, five announced themselves -- a hang, a
# SEGV, a refusal -- and the sixth handed you a clean run with no leaks and exit
# 0. Anything that ships a toolchain has to say more than "the build succeeded".
#
# Run against a tarball on its own to check a downloaded toolchain, or with a
# build tree to also check that the libsanitizer patches were applied:
#
#   general/scripts/verify-asan-toolchain.sh toolchain.ingenic-t31-asan.tgz
#   general/scripts/verify-asan-toolchain.sh <tarball> output
#
# What this cannot tell you is whether the sanitizer *reports* on your SoC. That
# needs the target, and it is what majestic's `scripts/asan.sh smoke --host
# <camera>` exists to answer. Run that before believing a leak-free `check`.

set -u

TARBALL=${1:-}
TARGET_DIR=${2:-}
PATCH_ROOT=${3:-general/package/all-patches/gcc}

if [ -z "$TARBALL" ]; then
	echo "usage: $0 <sdk-tarball> [buildroot-output-dir] [patch-root]" >&2
	exit 2
fi

fail=0
ok()   { echo "ok   $*"; }
bad()  { echo "FAIL $*"; fail=$((fail + 1)); }
note() { echo "--   $*"; }

[ -f "$TARBALL" ] || { bad "$TARBALL does not exist"; exit 1; }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

# ---------------------------------------------------------------------------
# 1. The libsanitizer patches were applied.
#
# They live in per-gcc-version directories, and Buildroot applies nothing at all
# when the version it selected has no directory -- no warning, no failure. A
# Buildroot bump that moves gcc from 13.3.0 to 14.x therefore produces a
# toolchain that builds, links, runs, and never reports a leak. That is the same
# outcome as the sixth defect above, reached by a different route, and it is
# invisible unless something looks.
if [ -z "$TARGET_DIR" ]; then
	note "no build tree given: skipping the patch check."
	note "pass Buildroot's output dir as the second argument to include it."
elif [ ! -d "$TARGET_DIR" ]; then
	bad "$TARGET_DIR is not a directory"
else
	build=$(echo "$TARGET_DIR"/build/host-gcc-final-*)
	if [ ! -d "$build" ]; then
		bad "no $TARGET_DIR/build/host-gcc-final-* -- gcc was not built here"
	else
		ver=${build##*host-gcc-final-}
		applied=$build/.applied_patches_list
		if [ ! -d "$PATCH_ROOT/$ver" ]; then
			bad "gcc is $ver but $PATCH_ROOT/$ver does not exist"
			note "the libsanitizer patches were not applied to this build."
			note "port them to $ver before publishing anything built here."
		elif [ ! -f "$applied" ]; then
			bad "$applied is missing -- cannot tell which patches were applied"
		else
			n=0
			missing=0
			for p in "$PATCH_ROOT/$ver"/*libsanitizer*.patch; do
				[ -e "$p" ] || break
				base=${p##*/}
				if grep -qF "$base" "$applied"; then
					n=$((n + 1))
				else
					bad "$base ships for gcc $ver but was not applied"
					missing=$((missing + 1))
				fi
			done
			if [ "$missing" -ne 0 ]; then
				note "$missing of $((n + missing)) shipped patches did not apply."
				note "the checks below will still pass: a toolchain missing these"
				note "builds, links, and cannot find a leak. That is the point."
			elif [ "$n" -lt 2 ]; then
				bad "only $n libsanitizer patch(es) found for gcc $ver"
				note "expected the musl compat patch plus the target's own fixes."
			else
				ok "$n libsanitizer patches applied for gcc $ver"
			fi
		fi
	fi
fi

# ---------------------------------------------------------------------------
# 2. The packaged tarball carries a runtime.
#
# Checked in the tarball rather than in the build tree because those are two
# different things: the SDK bundle is assembled separately, and its sysroot copy
# is the one that gets deployed next to the binary on the camera.
mkdir -p "$WORK/sdk"
if ! tar -xf "$TARBALL" -C "$WORK/sdk" --strip-components=1; then
	bad "cannot unpack $TARBALL"
	exit 1
fi

libs=$(find "$WORK/sdk" -name 'libasan.so.*' 2>/dev/null)
if [ -z "$libs" ]; then
	bad "no libasan.so.* in $TARBALL"
	note "most likely --enable-libsanitizer did not reach the gcc configure"
	note "line; check BR2_EXTRA_GCC_CONFIG_OPTIONS in the generated defconfig."
else
	for l in $libs; do
		ok "runtime $(basename "$l") $(wc -c <"$l" | tr -d ' ') bytes"
	done
fi

# ---------------------------------------------------------------------------
# 3. The compiler in the tarball links -fsanitize=address.
#
# A libasan.so sitting in the sysroot does not mean the driver knows to use it.
# The two are configured separately, and a gcc built without sanitizer support
# rejects the flag outright while the library sits there looking present.
# bin/ carries the driver under more than one name -- the full triple and a
# short alias -- and they are the same compiler. Sorted only so the choice does
# not depend on directory order.
cc=$(find "$WORK/sdk/bin" -maxdepth 1 -name '*-gcc' 2>/dev/null | sort | tail -1)
if [ -z "$cc" ]; then
	bad "no cross gcc in $TARBALL"
else
	printf '#include <stdlib.h>\nint main(void){ (void)malloc(64); return 0; }\n' \
		>"$WORK/leak.c"
	if ! "$cc" -fsanitize=address -g -o "$WORK/leak" "$WORK/leak.c" 2>"$WORK/cc.err"; then
		bad "$(basename "$cc") cannot link -fsanitize=address"
		sed 's/^/     /' "$WORK/cc.err"
	elif ! readelf -d "$WORK/leak" 2>/dev/null | grep -q libasan; then
		bad "the test binary did not pick up libasan"
	else
		ok "$(basename "$cc") links -fsanitize=address"
	fi
fi

echo
if [ "$fail" -ne 0 ]; then
	echo "$fail check(s) failed -- do not publish this toolchain"
	exit 1
fi
echo "toolchain looks sane. It is still unproven until 'scripts/asan.sh smoke'"
echo "reports the planted leak on the board itself."
