#!/bin/sh
# Drop the exfatprogs tools nothing on the image runs.
#
# Buildroot's exfatprogs package installs all six of its tools and offers no
# switch for a subset. A camera has a use for two: mkfs.exfat and fsck.exfat,
# for formatting and repairing an SD card from a shell. The other four are
# developer and forensic utilities, and no script in the tree or the WebUI
# names any of them:
#
#   exfat2img   copies a volume's metadata into an image file for debugging
#   dump.exfat  prints on-disk structures
#   tune.exfat  changes the volume serial or label
#   exfatlabel  prints or changes the label
#
# They come to 89KB stripped and 24KB of xz squashfs. Measured on
# hi3516ev300_ultimate, whose rootfs had grown to 8196KB of its 8192KB cap
# with no commit here (majestic and the WebUI are unpinned, see #2420). With
# them gone it is 8168KB.
set -eu

TARGET_DIR="${1:?target dir required}"

freed=0
dropped=
for tool in exfat2img dump.exfat tune.exfat exfatlabel; do
	f="${TARGET_DIR}/usr/sbin/${tool}"
	[ -f "${f}" ] || continue
	freed=$((freed + $(wc -c < "${f}")))
	rm -f "${f}"
	dropped="${dropped} ${tool}"
done

[ -n "${dropped}" ] && echo "prune-exfatprogs: dropped${dropped} ($((freed / 1024))KB)"
exit 0
