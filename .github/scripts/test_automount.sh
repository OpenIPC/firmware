#!/bin/bash
# mdev's automount helper decides, for every block device the kernel announces,
# whether to mount it and what to say when it cannot. It ships in
# general/overlay/, so it reaches every camera in the tree, and nothing else in
# CI executes it: the build proves it is installed, not that it is right.
#
# The logic under test is pure sysfs reading, so a fake /sys/block tree
# exercises it exactly. The script is copied with its two path variables
# rewritten into a sandbox rather than given a test hook, so what runs here is
# the shipped file, and a stray rmdir lands in a temporary directory instead of
# on the host's /mnt.
set -u

script="$(dirname "$0")/../../general/overlay/lib/mdev/automount.sh"
[ -f "${script}" ] || { echo "missing ${script}" >&2; exit 1; }

tmp="$(mktemp -d)"
trap 'rm -rf "${tmp}"' EXIT

sed -e "s#^destdir=/mnt\$#destdir=${tmp}/mnt#" \
    -e "s#^sysblock=/sys/block\$#sysblock=${tmp}/sys/block#" \
    "${script}" > "${tmp}/automount.sh"

# The rewrite is the whole basis of the sandbox; if it silently missed, every
# assertion below would be testing the host's real /mnt and /sys.
grep -q "^destdir=${tmp}/mnt\$" "${tmp}/automount.sh" || { echo "FAIL: destdir rewrite missed" >&2; exit 1; }
grep -q "^sysblock=${tmp}/sys/block\$" "${tmp}/automount.sh" || { echo "FAIL: sysblock rewrite missed" >&2; exit 1; }

mkdir -p "${tmp}/mnt"

# ACTION is set to something the dispatch does not match, so sourcing defines
# the functions without performing any action.
# shellcheck disable=SC1090
ACTION=selftest MDEV= . "${tmp}/automount.sh"

fails=0
ok()   { echo "ok   $1"; }
fail() { echo "FAIL $1"; fails=$((fails + 1)); }
check() { if [ "$2" = "$3" ]; then ok "$1"; else fail "$1 (got '$2', want '$3')"; fi; }

disk() {
	# A disk as sysfs presents one: attribute files and the housekeeping
	# directories that are not partitions and must never be counted as any.
	mkdir -p "${tmp}/sys/block/$1"/{queue,power,holders,slaves,device,bdi}
	printf '%s\n' "${2:-0}" > "${tmp}/sys/block/$1/size"
}

part() {
	mkdir -p "${tmp}/sys/block/$1/$2"
	# The attribute the kernel marks every partition with, whatever its name.
	printf '%s\n' "1" > "${tmp}/sys/block/$1/$2/partition"
	printf '%s\n' "2048" > "${tmp}/sys/block/$1/$2/size"
}

say() { if "$@"; then echo yes; else echo no; fi; }

# --- has_partitions: the disk/partition discrimination -----------------------

disk mmcblk0 61194240; part mmcblk0 mmcblk0p1
check "partitioned mmc disk has partitions"        "$(say has_partitions mmcblk0)" yes

disk mmcblk1 61194240
check "bare mmc disk has none"                     "$(say has_partitions mmcblk1)" no

disk sda 30000000; part sda sda1
check "partitioned scsi disk has partitions"       "$(say has_partitions sda)" yes

disk sdb 30000000
check "bare scsi disk has none"                    "$(say has_partitions sdb)" no

# The naming rules in mdev.conf stop at one digit; the kernel attribute does
# not, so a high-numbered partition is still seen here.
disk sdc 30000000; part sdc sdc10
check "double-digit partition is still a partition" "$(say has_partitions sdc)" yes

check "unknown device has no partitions"           "$(say has_partitions nosuchdev)" no

# --- has_medium: an empty reader is not a broken card ------------------------

check "disk with capacity has a medium"            "$(say has_medium mmcblk0)" yes

disk sdd 0
check "zero-capacity LUN has no medium"            "$(say has_medium sdd)" no

check "device with no size attribute has none"     "$(say has_medium nosuchdev)" no

mkdir -p "${tmp}/sys/block/sde"; printf 'junk\n' > "${tmp}/sys/block/sde/size"
check "non-numeric size is not a medium"           "$(say has_medium sde)" no

# --- the dispatch, end to end ------------------------------------------------

run() { env ACTION="$1" MDEV="$2" sh "${tmp}/automount.sh" >/dev/null 2>&1; }

# An empty MDEV must not reach mkdir -p "${destdir}/" and, on the failure path,
# rmdir "${destdir}" -- which over a squashfs /mnt leaves a persistent whiteout.
run add ""
check "empty MDEV leaves destdir alone"            "$([ -d "${tmp}/mnt" ] && echo yes || echo no)" yes

run remove ""
check "empty MDEV on remove leaves destdir alone"  "$([ -d "${tmp}/mnt" ] && echo yes || echo no)" yes

# A partitioned disk is not mounted as a whole; its partitions arrive under
# their own names.
run add mmcblk0
check "partitioned disk is not mounted whole"      "$([ -e "${tmp}/mnt/mmcblk0" ] && echo yes || echo no)" no

# An empty reader is passed over in silence: no mountpoint left behind.
run add sdd
check "empty LUN leaves no mountpoint"             "$([ -e "${tmp}/mnt/sdd" ] && echo yes || echo no)" no

# A disk with a medium and no partition table is attempted. The mount fails
# here -- there is no such device node on the test host -- so what is asserted
# is that it got as far as trying and then cleaned up after itself.
run add sdb
check "unpartitioned disk is attempted, then tidied" "$([ -e "${tmp}/mnt/sdb" ] && echo yes || echo no)" no

# --- which filesystem types each path offers -------------------------------
#
# The whole-disk path must never offer yaffs/yaffs2. `mount -t auto` walks
# /proc/filesystems and reaches yaffs2 before vfat on these kernels; yaffs2
# accepts a block device it has no business on, and the first write into the
# resulting mount oopses the kernel in yaffs_update_oh(). A stub mount records
# what was attempted.

stub="${tmp}/bin"
mkdir -p "${stub}"
cat > "${stub}/mount" <<STUB
#!/bin/sh
# record the type and refuse, so every candidate in the list is tried
[ "\$1" = "-t" ] && echo "\$2" >> "${tmp}/attempted"
echo "stub mount: refusing" >&2
exit 32
STUB
cat > "${stub}/logger" <<'STUB'
#!/bin/sh
exit 0
STUB
chmod +x "${stub}/mount" "${stub}/logger"

attempts() {
	: > "${tmp}/attempted"
	env PATH="${stub}:${PATH}" ACTION=add MDEV="$1" sh "${tmp}/automount.sh" >/dev/null 2>&1
	tr '\n' ' ' < "${tmp}/attempted" | sed 's/ $//'
}

disk zzdisk0 131072
expected="vfat exfat ext4 ext3 ext2 f2fs msdos ntfs iso9660 udf"

check "whole disk offers only removable-media types" "$(attempts zzdisk0)" "${expected}"

# The raw-flash and image filesystems must never be offered for a whole disk.
# yaffs2 is the one that oopses the kernel, and it sorts before vfat in
# /proc/filesystems, so `mount -t auto` reaches it first.
for banned in yaffs yaffs2 jffs2 ubifs squashfs auto; do
	check "whole disk never offers ${banned}" \
		"$(attempts zzdisk0 | tr ' ' '\n' | grep -cx "${banned}")" "0"
done

# A partition is not a directory under /sys/block, so it keeps the auto it has
# always had. This change opens a new path; it does not narrow the old one.
check "partition keeps auto" "$(attempts zzdisk0p1)" "auto"

# Every board config in the tree enables VFAT; it is tried first so the common
# card costs one syscall rather than ten.
check "vfat is tried first" "$(attempts zzdisk0 | cut -d' ' -f1)" "vfat"

echo
if [ "${fails}" -ne 0 ]; then
	echo "${fails} automount check(s) failed"
	exit 1
fi
echo "All automount checks passed."
