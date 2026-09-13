#!/bin/sh

destdir=/mnt
sysblock=/sys/block

my_umount() {
	if grep -qs "^/dev/$1 " /proc/mounts; then
		umount "${destdir}/$1"
	fi

	[ -d "${destdir}/$1" ] && rmdir "${destdir}/$1"
}

# What a removable medium is allowed to be mounted as.
#
# Named, rather than left to `mount -t auto`. Auto walks /proc/filesystems in
# order, and on these kernels it reaches yaffs and yaffs2 -- raw-NAND
# filesystems, with no business on a block device -- before it reaches vfat.
# yaffs2 does not decline: it accepts the device, the card mounts as garbage,
# and the kernel oopses. Twice on a hi3516av300 here, once in yaffs_update_oh()
# on the first write into such a mount, and once at mount time on a partition
# whose filesystem could not be identified:
#
#   yaffs: yaffs: Attempting MTD mount of 179.1,"mmcblk0p1"
#   Internal error: Oops: 805 [#1] SMP ARM
#
# It is not size-dependent in any way that could be relied on -- the same image
# mounted vfat on one loop device and yaffs2 on another -- and the card most
# likely to reach it is the one this whole change is about, since a damaged
# partition table is exactly what makes vfat decline and auto keep walking.
#
# The list is every block filesystem any board config in this tree enables
# (CONFIG_VFAT_FS on 80 of them, then ext4/3/2, f2fs, exfat, ntfs, iso9660,
# udf, msdos), so no camera loses a card it could mount before. What it leaves
# out is the raw-flash and image filesystems that were never candidates for
# removable media: yaffs, yaffs2, jffs2, ubifs, squashfs.
disk_fstypes="vfat exfat ext4 ext3 ext2 f2fs msdos ntfs iso9660 udf"

my_mount() {
	mkdir -p "${destdir}/$1" || exit 1

	# mount's own stderr goes nowhere. mdev is spawned through
	# /proc/sys/kernel/hotplug, so the helper runs with its descriptors on
	# /dev/null, and the reason a mount failed dies there. Catching it is what
	# makes the line below a diagnosis: a wiped partition table and a
	# filesystem this image has no driver for read identically otherwise, and
	# they need opposite things done about them. #2384.
	mounted=
	for fstype in ${2:-auto}; do
		if err=$(mount -t "${fstype}" "/dev/$1" "${destdir}/$1" 2>&1); then
			mounted=yes
			break
		fi
	done

	if [ -z "${mounted}" ]; then
		# A card that is present but holds nothing mountable used to be the
		# one failure with no trace at all: no kernel line, no syslog line,
		# and an operator looking at a camera that streams perfectly and
		# records nothing. It reads as a dead card, and twice it was not one.
		logger -s -p daemon.err -t automount \
			"cannot mount /dev/$1 as ${2:-auto}: ${err:-reason not reported}"
		# failed to mount, clean up mountpoint
		rmdir "${destdir}/$1"
		exit 1
	fi

	# copy files from autoconfig folder
	[ -d "${destdir}/$1/autoconfig" ] && cp -afv ${destdir}/$1/autoconfig/* / | logger -s -p daemon.info -t autoconfig

	# execution of the specified commands one time
	[ -f "${destdir}/$1/autoconfig.sh" ] && (sh ${destdir}/$1/autoconfig.sh; rm -f ${destdir}/$1/autoconfig.sh) | logger -s -p daemon.info -t autoconfig

	# execution of the specified commands
	[ -f "${destdir}/$1/autostart.sh" ] && sh ${destdir}/$1/autostart.sh | logger -s -p daemon.info -t autostart
}

# Does this whole disk have partitions?
#
# Asked of the kernel rather than inferred from the device name: every
# partition carries a `partition` attribute whatever the naming scheme, so this
# needs no second copy of mdev.conf's mmcblkNpN / sdXN rules to be kept in step
# with it.
#
# Trustworthy at the moment it is asked, because the kernel suppresses a disk's
# own add event until after it has scanned for partitions -- so the partition
# directories are already in sysfs when mdev gets here. They are merely
# announced immediately afterwards, each bringing mdev back under its own name.
has_partitions() {
	for part in "${sysblock}/$1"/*/partition; do
		[ -f "${part}" ] && return 0
	done

	return 1
}

# Is there actually a medium in this device?
#
# The kernel skips the partition scan entirely when the capacity is zero, so
# "formatted with no partition table" and "card reader with no card in it" are
# the same emptiness in sysfs. Without this the empty ones would be mounted at,
# fail, and be reported as a fault on every boot -- a multi-LUN reader, or the
# mass-storage LUN a ZeroCD modem presents until usb_modeswitch flips it
# (etc/wireless/modem). Teaching an operator to ignore the line this script
# adds would cost more than the silence it was added to break.
has_medium() {
	# stderr is redirected before the input is, not after: redirections are
	# applied left to right, and the open is what fails.
	read -r sectors 2>/dev/null < "${sysblock}/$1/size" || return 1

	case "${sectors}" in
		'' | *[!0-9]*) return 1 ;;
	esac

	[ "${sectors}" != 0 ]
}

case "${ACTION}" in
	add|"")
		# An empty name collapses every path below onto its parent: mkdir -p
		# /mnt/, mount /dev/, and finally rmdir /mnt -- which on an overlay
		# root does not simply fail, it records a whiteout over the /mnt the
		# squashfs provides. The tree has been here before: "sysupgrade: ship
		# /ram, and stop rmdir turning it into a whiteout".
		[ -n "${MDEV}" ] || exit 0

		# Whatever this name had mounted goes first, before the guard below
		# can decide the device is no longer ours to hold. A disk mounted
		# whole and since partitioned would otherwise stay mounted over the
		# very sectors its new partition is mounted from, and a write through
		# the stale superblock corrupts the new one.
		my_umount ${MDEV}

		if [ -d "${sysblock}/${MDEV}" ]; then
			# A disk that has partitions is not ours to mount -- its
			# partitions are, and each arrives here in its own right. A disk
			# with none falls through, which is what carries the two cases
			# that used to end in silence: a card formatted straight onto the
			# device with no partition table at all, and a card whose
			# partition table has been damaged. The first now mounts; the
			# second now says so. #2384.
			has_partitions "${MDEV}" && exit 0

			# An empty slot is not a fault and does not get a line.
			has_medium "${MDEV}" || exit 0
		fi

		# Never a device something else is already using. my_umount has just
		# released anything this script had mounted under /mnt, so a mount
		# still standing here belongs to someone else -- a root filesystem on
		# a whole-disk card, or one an autostart.sh made. Mounting it again
		# would lay a second live superblock over the same sectors, and would
		# then run autoconfig and autostart.sh out of it.
		grep -qs "^/dev/${MDEV} " /proc/mounts && exit 0

		# A whole disk -- the path this change opens -- is mounted only as
		# something removable media plausibly is. A partition keeps the auto
		# it has always had: it is exposed to the same trap, but narrowing a
		# path every camera already depends on is a fleet-wide behaviour
		# change, and wants its own evidence rather than a ride on this one.
		if [ -d "${sysblock}/${MDEV}" ]; then
			my_mount ${MDEV} "${disk_fstypes}"
		else
			my_mount ${MDEV}
		fi
		;;

	remove)
		[ -n "${MDEV}" ] || exit 0

		my_umount ${MDEV}
		;;
esac
