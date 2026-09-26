################################################################################
#
# majestic-af
#
################################################################################

MAJESTIC_AF_SITE = $(call github,openipc,majestic-af,$(MAJESTIC_AF_VERSION))
MAJESTIC_AF_VERSION = a76927a79a200c8e2fb48b8638abbd296cc81e62

MAJESTIC_AF_LICENSE = MIT
MAJESTIC_AF_LICENSE_FILES = LICENSE

# Out-of-core autofocus / PTZ engine for majestic: one shared object majestic
# dlopen()s at runtime. The HAL seams it calls (sdk_get_focus_value, ...) are
# left UNDEFINED here and resolve against the majestic executable at dlopen, so
# this links nothing from majestic — only its own vendored headers. majestic must
# be built with plugin-symbol export (WITH_PLUGINS_SUPPORT) for those to resolve;
# the plugin otherwise fails to load and majestic keeps working without it.
#
# The pin is a full SHA and stays one. It went stale for ten days here — three
# merged autofocus fixes that no nightly ever built — and the cure for that is
# something that MOVES the pin, not the removal of the pin: a SHA is what lets a
# published image be tied back to the plugin source inside it, and nothing else
# in this package records that. Tracking HEAD would also give the download a
# constant filename, which the Makefile's `prepare` expiry does not cover (it
# names only majestic.*.master.tar.bz2 and majestic-webui-dist.tar.gz), so a
# local from-source build would serve a stale majestic-af archive indefinitely —
# the very staleness this bump exists to end.
#
# Built through upstream's own CMakeLists, which is the only place the source
# list is allowed to live. This package used to carry a hand-written compile
# line naming each .c file, and the list silently went stale the first time the
# version was bumped: d1ff109 added src/motion.c and src/proto.c, the line still
# named three of the five, and the .so shipped with its own motion_*/ptz_verb_*
# symbols undefined. Nothing failed — `-shared` accepts undefined symbols, and
# it has to here, because the HAL seams above are undefined on purpose. The
# camera was where it surfaced: dlopen failed, majestic answered /ptz with 503,
# and the WebUI told operators to install the package they already had.
MAJESTIC_AF_CONF_OPTS += -DCMAKE_BUILD_TYPE=MinSizeRel

$(eval $(cmake-package))
