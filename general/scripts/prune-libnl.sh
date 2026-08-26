#!/bin/sh
# Drop the libnl sub-libraries nothing on the image links against.
#
# Buildroot's libnl package installs six shared libraries and offers no
# configure switch for a subset. On gk7205v300_lite that is 575KB of rootfs, of
# which only two are reachable: wpa_supplicant's nl80211 driver speaks *generic*
# netlink, so it links -lnl-3 -lnl-genl-3 and nothing else. The other four are
# dead weight on every one of the ~97 boards that enable wpa_supplicant:
#
#   libnl-route-3  342KB  rtnetlink object model -- links, addresses, routes,
#                         neighbours, and the whole traffic-control catalogue
#                         (every qdisc, class and classifier type, every link
#                         type). Each registers itself from a constructor, so
#                         --gc-sections cannot drop any of it.
#   libnl-nf-3      66KB  netfilter; needs libnl-route-3, needed by nothing
#   libnl-xfrm-3    52KB  IPsec SA/SP
#   libnl-idiag-3   27KB  inet_diag socket dumps
#
# Measured on gk7205v300_lite: 478KB out of the target dir, 176KB off the xz
# squashfs (5100KB -> 4924KB), which takes that board from 20KB of headroom
# against its 5120KB cap to 196KB. The initramfs boards get the full 480KB.
#
# The set to drop is DERIVED, not hardcoded. OpenIPC/builder carries variants
# this tree does not build (iw, hostapd, the FPV configs), and a hand-written
# list would quietly rot the day one of them grows a real libnl-route consumer
# -- the same one-directional staleness the excludes lists in rootfs_script.sh
# had to start reporting on. So: scan the image for each SONAME, keep what is
# referenced, drop what is not.
#
# Matching is on raw bytes rather than DT_NEEDED via readelf, for two reasons.
# It needs no binutils able to read the target's architecture, and it also
# catches a dlopen()ed name or a library named from a script, neither of which
# appears in a dynamic section. It over-matches by construction -- a mere
# mention keeps the library -- which is the direction to fail in: shipping 27KB
# we could have dropped costs headroom, dropping a library something loads at
# runtime costs a camera that no longer joins its network.
set -eu

TARGET_DIR="${1:?target dir required}"
LIBDIR="${TARGET_DIR}/usr/lib"

[ -d "${LIBDIR}" ] || exit 0

# Every list built below is one path or one SONAME per line, and a rootfs may
# well sit under a directory with a space in its name.
IFS='
'

# Real library files only; the SONAME and linker-name symlinks come off with
# their target further down.
libs=$(find "${LIBDIR}" -maxdepth 1 -type f -name 'libnl*-3.so.*' 2>/dev/null | sort)
[ -n "${libs}" ] || exit 0

# libnl-route-3.so.200.26.0 -> libnl-route-3.so.200, the string a consumer
# carries in its DT_NEEDED.
soname_of() {
	printf '%s\n' "${1##*/}" | sed 's/\(\.so\.[0-9][0-9]*\)\..*$/\1/'
}

# libnl-route-3.so.200.26.0 -> libnl-route-3, enough to sweep up the two
# symlinks that point at it.
stem_of() {
	printf '%s\n' "${1##*/}" | sed 's/\.so\..*$//'
}

# All six SONAMEs as a grep argument vector, reused by every pass.
set --
for lib in ${libs}; do
	set -- "$@" -e "$(soname_of "${lib}")"
done

scan_list=$(mktemp)
live_files=$(mktemp)
trap 'rm -f "${scan_list}" "${live_files}"' EXIT

# Which of the six SONAMEs are named by the files listed in $1. grep exits 1 for
# a batch with no match and xargs turns that into 123, so neither is an error
# here. The </dev/null keeps grep off the script's own stdin.
referenced_by() {
	list="$1"
	shift
	xargs -0 -r -a "${list}" grep -oahF "$@" </dev/null 2>/dev/null || true
}

find "${TARGET_DIR}" -type f ! -path "${LIBDIR}/libnl*" -print0 > "${scan_list}"

# A scan that reads nothing looks exactly like an image that references nothing
# -- and "nothing references libnl" is the answer that deletes all six
# libraries. Prove the pass can see inside a binary before trusting a negative:
# every dynamically linked executable here carries libc.so in its dynamic
# section, musl and uClibc alike.
#
# "Did anything match" has to come from grep's OUTPUT, not from xargs' exit
# status. A file list longer than ARG_MAX is split across several grep
# invocations; `grep -q` exits 1 for every batch that happens to hold no ELF,
# and xargs reports 123 if any single batch did. Reading that as "the scan is
# broken" would skip the prune on exactly the large images this exists to help,
# and skip it silently. Caught in review on #2317.
libc_seen=$(xargs -0 -r -a "${scan_list}" grep -laF -e 'libc.so' </dev/null 2>/dev/null || true)
if [ -z "${libc_seen}" ]; then
	echo "prune-libnl: no libc.so reference found anywhere; scan is not working, not pruning"
	exit 0
fi

live=$(referenced_by "${scan_list}" "$@" | sort -u)

# libnl-nf-3 links libnl-route-3, so a live library keeps its own dependencies.
# Iterate to a fixed point rather than encoding that one edge by hand.
while :; do
	: > "${live_files}"
	for soname in ${live}; do
		find "${LIBDIR}" -maxdepth 1 -type f -name "${soname}.*" -print0 >> "${live_files}"
	done
	merged=$(
		{
			printf '%s\n' "${live}"
			referenced_by "${live_files}" "$@"
		} | sed '/^$/d' | sort -u
	)
	[ "${merged}" = "${live}" ] && break
	live="${merged}"
done

freed=0
dropped=
kept=

for lib in ${libs}; do
	soname=$(soname_of "${lib}")
	if printf '%s\n' "${live}" | grep -qxF "${soname}"; then
		kept="${kept} ${soname}"
		continue
	fi
	freed=$((freed + $(wc -c < "${lib}")))
	rm -f "${LIBDIR}/$(stem_of "${lib}")".so*
	dropped="${dropped} ${soname}"
done

# /etc/libnl/{classid,pktloc} are the traffic-control classid and packet-location
# tables, parsed only by lib/route -- and by the CLI helpers, which OpenIPC does
# not build (BR2_PACKAGE_LIBNL_TOOLS is off everywhere).
case " ${dropped} " in
	*" libnl-route-3.so."*)
		if [ -d "${TARGET_DIR}/etc/libnl" ]; then
			freed=$((freed + $(cat "${TARGET_DIR}"/etc/libnl/* 2>/dev/null | wc -c)))
			rm -rf "${TARGET_DIR}/etc/libnl"
		fi
		;;
esac

if [ -n "${dropped}" ]; then
	echo "prune-libnl: dropped${dropped} ($((freed / 1024))KB), kept${kept:-" nothing"}"
else
	echo "prune-libnl: all of${kept} referenced, nothing to drop"
fi
