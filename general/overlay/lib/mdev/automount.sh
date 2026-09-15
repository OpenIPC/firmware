#!/bin/sh

destdir=/mnt
sysblock=/sys/block

# Is this device what the camera is running from?
#
# SD-rooted cameras exist in this tree -- /init and sysupgrade both decide
# whether the rootfs is on flash by testing the kernel command line for
# root=...mmcblk -- and on one of those this helper would be unmounting the
# filesystem it is running out of. The command line is checked as well as the
# mount table because the kernel reports the root device as /dev/root, under
# which name no rule here would recognise it.
backs_root() {
	grep -qs "^/dev/$1 / " /proc/mounts && return 0
	grep -qs "^/dev/$1 /rom " /proc/mounts && return 0

	for word in $(cat /proc/cmdline 2>/dev/null); do
		case "${word}" in
			root=/dev/$1 | root=$1) return 0 ;;
		esac
	done

	return 1
}

# $2 is "gone" when the medium has left the slot.
#
# That distinction decides what to do about a umount the kernel refuses. On an
# add the device is still there and a plain refusal is right: something is using
# it, and taking it away underneath that user is worse than not mounting.
#
# On a REMOVE there is nothing left to protect -- the card is out of the slot,
# every write to it is already failing -- and refusing to let go is actively
# destructive. A plain umount returns EBUSY for as long as anything holds a file
# open, which for a recording camera is the whole time, so the mount survives
# the card that backed it. The next card to arrive then finds its own name still
# in /proc/mounts, takes the "already mounted, leave it alone" exit below, and
# is never mounted at all -- silently, because mdev is spawned through
# /proc/sys/kernel/hotplug with its descriptors on /dev/null. One pulled card
# and the slot is dead until somebody reboots.
#
# So a removal detaches. The mount leaves the namespace at once and the name is
# free for the next card; the filesystem itself is cleaned up when the last
# holder lets go, and that holder is meanwhile getting the EIO it should be
# getting from a card that is not there.
my_umount() {
	if backs_root "$1"; then
		logger -s -p daemon.err -t automount \
			"refusing to unmount /dev/$1: the camera is running from it"
		return
	fi

	if grep -qs "^/dev/$1 " /proc/mounts; then
		if ! umount "${destdir}/$1" 2>/dev/null; then
			if [ "$2" != gone ]; then
				logger -s -p daemon.warn -t automount \
					"/dev/$1 is in use and was left mounted"
				return
			fi

			logger -s -p daemon.warn -t automount \
				"/dev/$1 was removed while in use; detaching ${destdir}/$1"
			umount -l "${destdir}/$1" 2>/dev/null
		fi
	fi

	# A mountpoint that will not go away is not tidiness. Anything written to
	# the path while nothing was mounted there lands on the rootfs overlay, and
	# it is then in the way of the next card -- which mounts over it, leaving
	# the stray files taking up flash where nobody will look for them.
	if [ -d "${destdir}/$1" ] && ! rmdir "${destdir}/$1" 2>/dev/null; then
		logger -s -p daemon.warn -t automount \
			"${destdir}/$1 could not be removed; something has written into it"
	fi
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
# The list is every block filesystem any board config in this tree enables:
# VFAT everywhere except the `neo` variants, which carry no block filesystem at
# all and so could never mount a card under any name, then ext4/3/2, f2fs,
# exfat, ntfs, iso9660, udf and msdos. No camera loses a card it could mount
# before. What it leaves out is the raw-flash and image filesystems that were
# never candidates for removable media: yaffs, yaffs2, jffs2, ubifs, squashfs.
#
# xfs joins them because the sentence above describes how the list was meant to
# be derived rather than how it came out: hi3519dv500 has carried
# CONFIG_XFS_FS=y since it was added, three months before the list landed in
# #2413, so a whole-disk xfs card has been refused on that board ever since.
#
# Derive this, do not reach for it. Both halves of the derivation are load
# bearing -- every CONFIG_*_FS symbol, over every board config that can see a
# removable block device at all, which is MMC or USB storage because mdev.conf
# routes sd[a-z] here as well as mmcblk:
#
#   for f in $(grep -rlE '^CONFIG_(MMC|USB_STORAGE)=[ym]' br-ext-chip-*/board/)
#   do grep -oE '^CONFIG_[A-Z0-9_]+_FS=[ym]' "$f"; done | sort -u
#
# Shortlisting the symbols by hand is how xfs went missing. Dropping the scope
# is how a filesystem looks missing when it is not: gfs2 is enabled in this
# tree, on a board with neither an SD slot nor USB storage, where nothing can
# ever reach this script.
disk_fstypes="vfat exfat ext4 ext3 ext2 f2fs xfs msdos ntfs iso9660 udf"

my_mount() {
	mkdir -p "${destdir}/$1" || exit 1

	# mount's own stderr goes nowhere. mdev is spawned through
	# /proc/sys/kernel/hotplug, so the helper runs with its descriptors on
	# /dev/null, and the reason a mount failed dies there. Catching it is what
	# makes the line below a diagnosis: a wiped partition table and a
	# filesystem this image has no driver for read identically otherwise, and
	# they need opposite things done about them. #2384.
	mounted=
	firsterr=
	for fstype in ${2:-auto}; do
		if err=$(mount -t "${fstype}" "/dev/$1" "${destdir}/$1" 2>&1); then
			mounted=yes
			break
		fi
		# The first refusal, not the last. The list is tried in order of what
		# a camera's card actually is, so vfat's answer is the diagnosis --
		# "invalid argument" means the filesystem is damaged, where the tail
		# of the list only ever reports the drivers this kernel was not built
		# with.
		[ -n "${firsterr}" ] || firsterr="${err}"
	done

	if [ -z "${mounted}" ]; then
		# A card that is present but holds nothing mountable used to be the
		# one failure with no trace at all: no kernel line, no syslog line,
		# and an operator looking at a camera that streams perfectly and
		# records nothing. It reads as a dead card, and twice it was not one.
		logger -s -p daemon.err -t automount \
			"cannot mount /dev/$1 as ${2:-auto}: ${firsterr:-reason not reported}"
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
		#
		# Said out loud, because this exit is also where a card goes to be
		# quietly ignored: before removals learned to detach, a mount left
		# behind by a card pulled while it was being written to would send
		# every later card down this branch, and the only symptom was a slot
		# that had stopped working.
		if grep -qs "^/dev/${MDEV} " /proc/mounts; then
			logger -s -p daemon.warn -t automount \
				"/dev/${MDEV} is already mounted; leaving it to its owner"
			exit 0
		fi

		# Both paths are mounted only as something removable media plausibly
		# is. The partition used to keep the `auto` it had always had, on the
		# grounds that narrowing a path every camera depends on wanted its own
		# evidence rather than a ride on the whole-disk change.
		#
		# Here is the evidence. The trap auto walks into is a raw-NAND driver
		# accepting a block device and oopsing the kernel, and mmcblk0p1 is the
		# device every camera with a card in it actually mounts -- so the one
		# path still exposed was the one that matters. Deriving the list from
		# the filesystems board configs enable, rather than from the ones that
		# seem likely, is what makes the narrowing safe; doing that derivation
		# again is what turned up the missing xfs above.
		my_mount ${MDEV} "${disk_fstypes}"
		;;

	remove)
		[ -n "${MDEV}" ] || exit 0

		my_umount ${MDEV} gone
		;;
esac
