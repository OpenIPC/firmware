#!/bin/sh
# Regression test for the lite CA bundle cut in general/scripts/rootfs_script.sh
# (#2508).
#
# A bundle that drops the wrong root is invisible until a camera in the field
# runs sysupgrade and cannot reach GitHub, and by then the image is on flash.
# This runs the real rootfs_script.sh, filter-ca-bundle.py and keep-list
# against a synthetic target holding the committed bundle, so every Mozilla
# refresh and every keep-list edit is checked without paying for a build.
#
# What is checked:
#   - lite keeps a non-empty strict subset, every kept certificate byte for
#     byte one of the full bundle's (majestic refuses to start on a single
#     unparsable certificate, so the cut must only ever remove);
#   - the roots sysupgrade and Let's Encrypt chain to survive it;
#   - ultimate is left untouched, and a target without a bundle is a no-op;
#   - a keep entry matching nothing fails the build and leaves the bundle whole.
#
# Needs python3, no build, runs in under a second.

set -eu

SCRIPT_UNDER_TEST="${SCRIPT_UNDER_TEST:-general/scripts/rootfs_script.sh}"
BUNDLE="${BUNDLE:-general/overlay/etc/ssl/certs/ca-certificates.crt}"

fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

for f in "$SCRIPT_UNDER_TEST" "$BUNDLE" general/scripts/filter-ca-bundle.py general/scripts/ca-bundle-lite.keep; do
	[ -f "$f" ] || { echo "FAIL cannot find $f"; exit 1; }
done

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

# Only the filter and its keep-list: without the comment stripper and the
# overlay lists, the rest of rootfs_script.sh has nothing to do.
ext="$work/general"
mkdir -p "$ext/scripts"
cp general/scripts/filter-ca-bundle.py general/scripts/ca-bundle-lite.keep "$ext/scripts/"
: > "$work/br2config"

run() {	# run VARIANT TARGET -> exit status, output in $work/out.txt
	set +e
	env TARGET_DIR="$2" \
		BR2_EXTERNAL_GENERAL_PATH="$ext" \
		BR2_CONFIG="$work/br2config" \
		OPENIPC_SOC_MODEL=testsoc \
		OPENIPC_VARIANT="$1" \
		bash "$SCRIPT_UNDER_TEST" > "$work/out.txt" 2>&1
	status=$?
	set -e
	return $status
}

fresh() {	# fresh NAME -> a target holding the committed bundle
	t="$work/$1"
	mkdir -p "$t/etc/ssl/certs" "$t/usr/lib"
	cp "$BUNDLE" "$t/etc/ssl/certs/ca-certificates.crt"
	echo "$t"
}

# Compared decoded, so line wrapping cannot make two copies of a root differ.
ders() {
	python3 - "$1" <<-'EOF'
	import base64, re, sys
	for pem in re.findall(rb"-----BEGIN CERTIFICATE-----\n(.*?)-----END", open(sys.argv[1], "rb").read(), re.S):
	    print(base64.b64decode(b"".join(pem.split())).hex())
	EOF
}
has_root() {	# has_root BUNDLE "printable subject text"
	python3 - "$1" "$2" <<-'EOF'
	import base64, re, sys
	needle = sys.argv[2].encode()
	ders = [base64.b64decode(b"".join(p.split())) for p in
	        re.findall(rb"-----BEGIN CERTIFICATE-----\n(.*?)-----END", open(sys.argv[1], "rb").read(), re.S)]
	sys.exit(0 if any(needle in d for d in ders) else 1)
	EOF
}

# ----- lite -----
t=$(fresh lite)
if run lite "$t"; then ok "lite build succeeds"; else bad "lite build failed: $(cat "$work/out.txt")"; fi
cut="$t/etc/ssl/certs/ca-certificates.crt"
full_n=$(grep -c 'BEGIN CERTIFICATE' "$BUNDLE")
cut_n=$(grep -c 'BEGIN CERTIFICATE' "$cut" || true)
[ "$cut_n" -gt 0 ] && [ "$cut_n" -lt "$full_n" ] && ok "lite keeps $cut_n of $full_n roots" \
	|| bad "lite kept $cut_n of $full_n roots"
grep -q "ca-bundle: kept $cut_n of $full_n roots" "$work/out.txt" && ok "the cut is reported in the build log" \
	|| bad "no 'ca-bundle: kept' line in the build log"

ders "$BUNDLE" | sort > "$work/full.hex"
ders "$cut" | sort > "$work/cut.hex"
[ -z "$(comm -13 "$work/full.hex" "$work/cut.hex")" ] && ok "every kept certificate is one of the full bundle's" \
	|| bad "the lite bundle holds a certificate the full bundle does not"

# What sysupgrade, openipc.org and every Let's Encrypt site chain to.
for root in "ISRG Root X1" "ISRG Root X2" "USERTrust ECC Certification Authority" \
		"USERTrust RSA Certification Authority" "Go Daddy Root Certificate Authority - G2" \
		"Starfield Services Root Certificate Authority - G2" \
		"Hellenic Academic and Research Institutions RootCA 2015"; do
	has_root "$cut" "$root" && ok "lite keeps $root" || bad "lite dropped $root"
done

# ----- ultimate -----
t=$(fresh ultimate)
if run ultimate "$t"; then ok "ultimate build succeeds"; else bad "ultimate build failed"; fi
cmp -s "$BUNDLE" "$t/etc/ssl/certs/ca-certificates.crt" && ok "ultimate ships the full bundle" \
	|| bad "ultimate's bundle was changed"

# ----- no bundle -----
t="$work/empty"
mkdir -p "$t/usr/lib"
if run lite "$t"; then ok "lite target without a bundle builds"; else bad "a target without a bundle failed the build"; fi
[ ! -e "$t/etc/ssl/certs/ca-certificates.crt" ] && ok "and none appears" || bad "a bundle appeared from nowhere"

# ----- stale keep entry -----
t=$(fresh stale)
echo "No Such Operator Inc" >> "$ext/scripts/ca-bundle-lite.keep"
if run lite "$t"; then bad "a keep entry matching nothing did not fail the build"; else ok "a keep entry matching nothing fails the build"; fi
grep -q "'No Such Operator Inc' matches no root" "$work/out.txt" && ok "and names the entry" \
	|| bad "the stale entry is not named: $(cat "$work/out.txt")"
cmp -s "$BUNDLE" "$t/etc/ssl/certs/ca-certificates.crt" && ok "and leaves the bundle whole" \
	|| bad "the bundle was rewritten despite the failure"

echo
if [ "$fail" -eq 0 ]; then
	echo "All CA bundle checks passed."
else
	echo "$fail check(s) failed."
	exit 1
fi
