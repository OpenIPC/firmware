#!/bin/sh

destdir=/mnt

my_umount() {
	if grep -qs "^/dev/$1 " /proc/mounts; then
		umount "${destdir}/$1"
	fi

	[ -d "${destdir}/$1" ] && rmdir "${destdir}/$1"
}

my_mount() {
	mkdir -p "${destdir}/$1" || exit 1

	if ! mount -t auto "/dev/$1" "${destdir}/$1"; then
		# A card that is present but holds nothing mountable used to be the
		# one failure with no trace at all: no kernel line, no syslog line,
		# and an operator looking at a camera that streams perfectly and
		# records nothing. It reads as a dead card, and twice it was not one.
		# #2384.
		logger -s -p daemon.err -t automount "cannot mount /dev/$1"
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

# Does this whole disk have a partition table the kernel could read?
#
# Only ever asked of a disk, and the answer is trustworthy at the moment it is
# asked: the kernel suppresses a disk's own add event until after it has
# scanned the partitions, so by the time mdev runs this the partition nodes
# already exist in sysfs -- they are merely announced immediately afterwards,
# each bringing mdev back here under its own name.
has_partitions() {
	for part in "/sys/block/$1/$1"p[0-9]* "/sys/block/$1/$1"[0-9]*; do
		[ -d "${part}" ] && return 0
	done

	return 1
}

case "${ACTION}" in
	add|"")
		# A disk that has partitions is not ours to mount -- its partitions
		# are, and each arrives here in its own right. A disk with none falls
		# through, which is what carries the two cases that used to end in
		# silence: a card formatted straight onto the device with no partition
		# table at all, and a card whose partition table has been damaged. The
		# first now mounts; the second now says so. #2384.
		if [ -d "/sys/block/${MDEV}" ] && has_partitions "${MDEV}"; then
			exit 0
		fi

		my_umount ${MDEV}
		my_mount ${MDEV}
		;;

	remove)
		my_umount ${MDEV}
		;;
esac
