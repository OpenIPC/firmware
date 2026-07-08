#!/bin/sh
set -eu

TARGET_DIR="${1:?target dir required}"
HOST_DIR="${HOST_DIR:-$(dirname "${TARGET_DIR}")/host}"

read_kconfig_string() {
	awk -F= -v key="$1" '
		$1 == key {
			gsub(/"/, "", $2)
			value = $2
		}
		END {
			if (value != "") {
				print value
			}
		}
	' "${BR2_CONFIG}" | tail -n1
}

detect_host_timezone() {
	local tz

	if [ -f /etc/timezone ]; then
		tz="$(sed -n '1p' /etc/timezone)"
		[ -n "${tz}" ] && {
			printf '%s\n' "${tz}"
			return 0
		}
	fi

	tz="$(readlink /etc/localtime 2>/dev/null || true)"
	case "${tz}" in
		*/zoneinfo/*)
			printf '%s\n' "${tz#*/zoneinfo/}"
			return 0
			;;
	esac

	return 1
}

extract_posix_tz() {
	local zone="$1"
	local zonefile=
	local tz

	for candidate in \
		"${HOST_DIR}/share/zoneinfo/posix/${zone}" \
		"/usr/share/zoneinfo/posix/${zone}" \
		"/usr/share/zoneinfo/${zone}"
	do
		if [ -f "${candidate}" ]; then
			zonefile="${candidate}"
			break
		fi
	done

	[ -n "${zonefile}" ] || {
		echo "openipc-nfs-root: missing zoneinfo file for '${zone}'" >&2
		return 1
	}

	tz="$(strings "${zonefile}" | tail -n1)"
	[ -n "${tz}" ] || {
		echo "openipc-nfs-root: failed to extract POSIX TZ from '${zonefile}'" >&2
		return 1
	}

	printf '%s\n' "${tz}"
}

write_target_timezone() {
	local zone="$1"
	local tz="$2"

	printf '%s\n' "${tz}" > "${TARGET_DIR}/etc/TZ"
	printf '%s\n' "${zone}" > "${TARGET_DIR}/etc/timezone"
}

OPENIPC_NFS_ROOT_TMPFS_SIZE="$(read_kconfig_string BR2_PACKAGE_OPENIPC_NFS_ROOT_TMPFS_SIZE)"
[ -n "${OPENIPC_NFS_ROOT_TMPFS_SIZE}" ] || OPENIPC_NFS_ROOT_TMPFS_SIZE=16M

OPENIPC_NFS_ROOT_PASSWD="$(read_kconfig_string BR2_PACKAGE_OPENIPC_NFS_ROOT_PASSWD)"
OPENIPC_NFS_ROOT_PASSWD=$(printf '%s' "$OPENIPC_NFS_ROOT_PASSWD" | sed 's/[\/&\\]/\\&/g')
[ -n "${OPENIPC_NFS_ROOT_PASSWD}" ] || OPENIPC_NFS_ROOT_PASSWD="12345"

OPENIPC_NFS_ROOT_NTP_SERVER="$(read_kconfig_string BR2_PACKAGE_OPENIPC_NFS_ROOT_NTP_SERVER)"
[ -n "${OPENIPC_NFS_ROOT_NTP_SERVER}" ] || OPENIPC_NFS_ROOT_NTP_SERVER=""

OPENIPC_NFS_ROOT_DNS_SERVER="$(read_kconfig_string BR2_PACKAGE_OPENIPC_NFS_ROOT_DNS_SERVER)"
[ -n "${OPENIPC_NFS_ROOT_DNS_SERVER}" ] || OPENIPC_NFS_ROOT_DNS_SERVER=""

OPENIPC_NFS_ROOT_HOSTNAME="$(read_kconfig_string BR2_PACKAGE_OPENIPC_NFS_ROOT_HOSTNAME)"
[ -n "${OPENIPC_NFS_ROOT_HOSTNAME}" ] || OPENIPC_NFS_ROOT_HOSTNAME=""

OPENIPC_NFS_ROOT_TIMEZONE="$(read_kconfig_string BR2_PACKAGE_OPENIPC_NFS_ROOT_TIMEZONE)"
if [ -z "${OPENIPC_NFS_ROOT_TIMEZONE}" ]; then
	OPENIPC_NFS_ROOT_TIMEZONE="$(detect_host_timezone)"
fi

[ -n "${OPENIPC_NFS_ROOT_TIMEZONE}" ] || {
	echo "openipc-nfs-root: unable to determine timezone; set BR2_PACKAGE_OPENIPC_NFS_ROOT_TIMEZONE" >&2
	exit 1
}

OPENIPC_NFS_ROOT_TZ_POSIX="$(extract_posix_tz "${OPENIPC_NFS_ROOT_TIMEZONE}")"

# Note: delimiter '|' chosen over '/' due to password hash collisions for OPENIPC_NFS_ROOT_PASSWD
if [ -f "${TARGET_DIR}/init" ]; then
	sed -i "s|@TMPFS_SIZE@|${OPENIPC_NFS_ROOT_TMPFS_SIZE}|" "${TARGET_DIR}/init"
fi

if [ -f "${TARGET_DIR}/etc/init.d/S45setupenv" ]; then
	sed -i "s|@ROOTFS_PASSWD@|${OPENIPC_NFS_ROOT_PASSWD}|" "${TARGET_DIR}/etc/init.d/S45setupenv"
fi

if [ -f "${TARGET_DIR}/etc/init.d/S45setupenv" ]; then
	sed -i "s|@NTP_SERVER@|${OPENIPC_NFS_ROOT_NTP_SERVER}|" "${TARGET_DIR}/etc/init.d/S45setupenv"
fi

if [ -f "${TARGET_DIR}/etc/init.d/S45setupenv" ]; then
	sed -i "s|@DNS_SERVER@|${OPENIPC_NFS_ROOT_DNS_SERVER}|" "${TARGET_DIR}/etc/init.d/S45setupenv"
fi

if [ -f "${TARGET_DIR}/etc/init.d/S45setupenv" ]; then
	sed -i "s|@HOSTNAME@|${OPENIPC_NFS_ROOT_HOSTNAME}|" "${TARGET_DIR}/etc/init.d/S45setupenv"
fi

if [ -d "${TARGET_DIR}/etc" ]; then
	write_target_timezone "${OPENIPC_NFS_ROOT_TIMEZONE}" "${OPENIPC_NFS_ROOT_TZ_POSIX}"
fi
