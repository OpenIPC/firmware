#!/bin/bash

set -e

function log() {
    echo "--- $@"
}

function apply_patch() {
    DST=$1
    SRC=$2

    if [[ -d "$SRC" ]]; then
        if [[ ${SRC:${#SRC}-1} != '/' ]]; then
            log Apply \"$SRC\" as overlay directory
            cp -r $SRC/* $DST/
        else
            log Apply \"$SRC\" as patches directory
            for P in $SRC/*.patch; do
                patch -d $DST -p1 <$P
            done
        fi
    else
        if [[ -f "$SRC" ]]; then
            log Apply \"$SRC\" as single patch
            patch -d $DST -p1 <$SRC
        fi
    fi
}

function apply_patches() {
    log Gonna apply "$@"
    while [[ $# -ge 1 ]]; do
        apply_patch $DST $1
        shift
    done
}

DST=$1
shift

if [ -f $DST/.already-patched ]; then
    log Patched already
    exit
fi

apply_patches $@
touch $DST/.already-patched
