## OpenIPC v2.1 (Buildroot based..)

Alternative firmware for IP cameras based on the HiSilicon (and other) SoC's

More information about the project is available in our [Wiki](https://github.com/OpenIPC/openipc-2.1/wiki) and [Site](https://openipc.org)

[![CI status](https://img.shields.io/github/downloads/OpenIPC/openipc-2.1/total.svg)](https://github.com/OpenIPC/openipc-2.1/releases)
![GitHub repo size](https://img.shields.io/github/repo-size/OpenIPC/openipc-2.1)
![GitHub issues](https://img.shields.io/github/issues/OpenIPC/openipc-2.1)
![GitHub pull requests](https://img.shields.io/github/issues-pr/OpenIPC/openipc-2.1)
[![License](https://img.shields.io/github/license/OpenIPC/openipc-2.1)](https://opensource.org/licenses/MIT)

-----

### Debugger compile

```
BR2_PTHREAD_DEBUG=y
BR2_PACKAGE_HOST_GDB=y
BR2_PACKAGE_HOST_GDB_NOPYTHON=y
BR2_GDB_VERSION_9_2=y
BR2_GDB_VERSION="9.2"
BR2_PACKAGE_GDB=y
BR2_PACKAGE_GDB_SERVER=y
BR2_PACKAGE_GDB_DEBUGGER=y
BR2_PACKAGE_NCURSES=y
BR2_PACKAGE_NCURSES_ADDITIONAL_TERMINFO=""
BR2_PACKAGE_STRACE=y
```

