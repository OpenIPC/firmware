# openipc-v2.0

OpenIPC v2.0 based on Buildroot

[![Test building...](https://github.com/FlyRouter/openipc-v2.0/actions/workflows/main.yml/badge.svg)](https://github.com/FlyRouter/openipc-v2.0/actions/workflows/main.yml)

-----

## Quick Start for Hi3516Ev300

```
make prepare
make -j$(($(nproc)+1)) BOARD=unknown_unknown_hi3516ev300_unknown all
```

Execute the commands above and your files will be in the directory **output/images**

-----

## ToDo support

* br-ext-chip-anyka
* br-ext-chip-fullhan
* br-ext-chip-goke
* br-ext-chip-grainmedia
* br-ext-chip-ingenic

* br-ext-chip-infineon

-----

## Other strings

```
#
# make help
# make install-deps
# make prepare
# make list-configs
#
# make busybox-{dirclean,rebuild}
# make fwprintenv-ng-{dirclean,rebuild}
# make hisilicon-osdrv4-{dirclean,rebuild}
# make ipctool-{dirclean,rebuild}
# make jpeg-turbo-{dirclean,rebuild}
# make libevent-mini-{dirclean,rebuild}
# make linux-{dirclean,rebuild}
# make majestic-{dirclean,rebuild}
# make uboot-tools-fwprintenv-{dirclean,rebuild}
#
# make BOARD=unknown_unknown_hi3516ev300_unknown board-info
# make BOARD=unknown_unknown_hi3516ev300_unknown all && 2dolphin
#
# make BOARD=unknown_unknown_xm530_unknown board-info
# make BOARD=unknown_unknown_xm530_unknown all
#
# cd output ; make sdk
#
# tag="v21.04.10.3"
#
# git pull
# git push
# git tag ${tag} -m "Jump to ${tag}"
# git push origin --tags
# git push --follow-tags
#
```

