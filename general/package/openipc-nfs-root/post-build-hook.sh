#!/bin/sh
set -eu

TARGET_DIR="${1:?target dir required}"

OPENIPC_NFS_ROOT_TMPFS_SIZE=$(
	awk -F= '/^BR2_PACKAGE_OPENIPC_NFS_ROOT_TMPFS_SIZE=/{gsub(/"/, "", $2); print $2}' \
		"${BR2_CONFIG}" | tail -n1
)
[ -n "${OPENIPC_NFS_ROOT_TMPFS_SIZE}" ] || OPENIPC_NFS_ROOT_TMPFS_SIZE=16M

if [ -f "${TARGET_DIR}/init" ]; then
	sed -i "s/@TMPFS_SIZE@/${OPENIPC_NFS_ROOT_TMPFS_SIZE}/" "${TARGET_DIR}/init"
fi
