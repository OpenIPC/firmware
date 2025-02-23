#!/bin/bash

DST=$1
shift

rm -rf $DST
mkdir -p $DST

while [[ $# -ge 1 ]]; do
    echo "Put layer '$1' onto '$DST'..." >&2
#   if cp -r $1/* $DST/; then
    if rsync -a --ignore-times --chmod=u=rwX,go=rX --exclude .empty $1/ $DST/; then
        echo "DONE" >&2
    else
        echo "FAILED" >&2
        exit -1
    fi
    shift
done


