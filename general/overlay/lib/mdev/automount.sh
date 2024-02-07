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

	if ! mount -t auto -o sync "/dev/$1" "${destdir}/$1"; then
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

case "${ACTION}" in
	add|"")
		my_umount ${MDEV}
		my_mount ${MDEV}
		;;

	remove)
		my_umount ${MDEV}
		;;
esac
