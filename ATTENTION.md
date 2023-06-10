Attention
=========

This is where the hot news will be posted that you should pay attention to, it's not a changelog


## 01.06.2023

We updated the Buildroot version, changed the toolchain build system, moved and streamlined configuration files to configure network devices - a lot of changes.
The global changes in the structure of the project caused some devices that use external WiFi modules to stop or stop working after the update.

We kindly ask you to be understanding and until documentation and releases are corrected, please do NOT update at least these types of devices, and maybe others with WiFi:

* Smartwares CIP-37210
* Switcam HS303 v1
* Switcam HS303 v2
* Switcam HS303 v3
* Ezviz (any)
* Xiaomi (mjsxj02hl)

If you broke your device, please rollback your firmware to 01.05.2023 or return to the firmware of the [sub-project](https://github.com/topics/device-openipc).

You can get official OpenIPC firmware for the last half year on our Telegram [channel](https://t.me/openipc_dev), they are published automatically.


## 31.05.2023

For owners of Ingenic processor-based cameras, we recommend temporarily disabling the OSD because arbitrary device reboots can occur.

You can do this with this command by specifying your IP address

```
ssh root@192.168.1.10 cli -s .osd.enabled false
```
