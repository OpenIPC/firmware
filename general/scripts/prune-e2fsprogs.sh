#!/bin/sh
# Drop the e2fsprogs tools nothing on the image runs.
#
# Buildroot's e2fsprogs package installs every tool it builds and offers a
# switch for only a few of them. An image with e2fsprogs wants it to make,
# check and label an ext4 disk: mke2fs (mkfs.ext2/3/4), e2fsck (fsck.ext2/3/4)
# and tune2fs (e2label). The rest are diagnostic and developer utilities, and
# no script in the tree names any of them:
#
#   dumpe2fs, e2mmpstatus   print superblock and group descriptors
#   badblocks               surface scan
#   e2freefrag, filefrag    free-space and per-file fragmentation reports
#   e2undo                  replays an undo file nothing here writes
#   e4crypt                 ext4 encryption policies (the kernels have none)
#   logsave                 boot-time fsck logging for sysvinit
#   mklost+found            mke2fs already makes it
#   chattr, lsattr          file attributes
#   compile_et, mk_cmds     build-time code generators, with their
#                           /usr/share/et and /usr/share/ss templates
#   libss                   debugfs's command line, and debugfs is off
#                           unless BR2_PACKAGE_E2FSPROGS_DEBUGFS
#
# Measured on ssr621q_lite: 153KB stripped out of the target dir.
set -eu

TARGET_DIR="${1:?target dir required}"

freed=0
dropped=
drop() {
	[ -e "${TARGET_DIR}/$1" ] || [ -L "${TARGET_DIR}/$1" ] || return 0
	[ -L "${TARGET_DIR}/$1" ] || freed=$((freed + $(du -sb "${TARGET_DIR}/$1" | cut -f1)))
	rm -rf "${TARGET_DIR:?}/$1"
	dropped="${dropped} ${1##*/}"
}

for tool in sbin/dumpe2fs sbin/e2mmpstatus sbin/badblocks sbin/e2freefrag \
	sbin/filefrag sbin/e2undo sbin/e4crypt sbin/logsave sbin/mklost+found \
	bin/chattr bin/lsattr bin/compile_et bin/mk_cmds \
	usr/share/et usr/share/ss; do
	drop "${tool}"
done

if [ ! -e "${TARGET_DIR}/sbin/debugfs" ] && [ ! -e "${TARGET_DIR}/usr/sbin/debugfs" ]; then
	for lib in "${TARGET_DIR}"/usr/lib/libss.so*; do
		[ -e "${lib}" ] || [ -L "${lib}" ] || continue
		drop "usr/lib/${lib##*/}"
	done
fi

[ -n "${dropped}" ] && echo "prune-e2fsprogs: dropped${dropped} ($((freed / 1024))KB)"
exit 0
