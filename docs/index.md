## OpenIPC v2.1 (Buildroot based..)

Alternative firmware for IP cameras based on the HiSilicon (and other) SoC's

More information about the project is available in our [Wiki](https://github.com/OpenIPC/openipc-2.1/wiki) and [Site](https://openipc.org)

[![CI status](https://img.shields.io/github/downloads/OpenIPC/openipc-2.1/total.svg)](https://github.com/OpenIPC/openipc-2.1/releases)
![GitHub repo size](https://img.shields.io/github/repo-size/OpenIPC/openipc-2.1)
![GitHub issues](https://img.shields.io/github/issues/OpenIPC/openipc-2.1)
![GitHub pull requests](https://img.shields.io/github/issues-pr/OpenIPC/openipc-2.1)
[![License](https://img.shields.io/github/license/OpenIPC/openipc-2.1)](https://opensource.org/licenses/MIT)

-----

### Supporting

If you like our work, please consider supporting us on [Opencollective](https://opencollective.com/openipc/contribute/backer-14335/checkout) or [PayPal](https://www.paypal.com/donate/?hosted_button_id=C6F7UJLA58MBS) or [YooMoney](https://openipc.org/donation/yoomoney.html). 

[![Backers](https://opencollective.com/openipc/tiers/backer/badge.svg?label=backer&color=brightgreen)](https://opencollective.com/openipc)
[![Backers](https://opencollective.com/openipc/tiers/badge.svg)](https://opencollective.com/openipc)

[![Backers](https://opencollective.com/openipc/tiers/backer.svg?avatarHeight=36)](https://opencollective.com/openipc#support)

### Thanks a lot !!!

<p align="center">
<a href="https://opencollective.com/openipc/contribute/backer-14335/checkout" target="_blank"><img src="https://opencollective.com/webpack/donate/button@2x.png?color=blue" width="300" alt="OpenCollective donate button" /></a>
<a href="https://www.paypal.com/donate/?hosted_button_id=C6F7UJLA58MBS"><img src="https://www.paypalobjects.com/en_US/IT/i/btn/btn_donateCC_LG.gif" alt="PayPal donate button" /> </a>
<a href="https://openipc.org/donation/yoomoney.html"><img src="https://yoomoney.ru/transfer/balance-informer/balance?id=596194605&key=291C29A811B500D7" width="100" alt="YooMoney donate button" /> </a>
</p>

-----

### Downloads (for research only, without any guarantee) <a href="#commercial">*</a>

| Manufacturer | SoC | Files | Status |
|--------------|-----|-------|--------|
|Fullhan  |![Image](https://img.shields.io/static/v1?label=FH8632&message=help%20needed&color=5500ff&logo=github)  | | R&D   |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/fh8852_images.yml/badge.svg)          | | R&D   |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/fh8856_images.yml/badge.svg)          | | R&D   |
|Goke     |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7102s_images.yml/badge.svg)         | | R&D   |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7202v300_images.yml/badge.svg)      |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7202v300-br.tgz) | WIP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7205v200_images.yml/badge.svg)      |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7205v200-br.tgz) | DONE |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7205v300_images.yml/badge.svg)      |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7205v300-br.tgz) | WIP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7605v100_images.yml/badge.svg)      |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7605v100-br.tgz) | WIP  |
|HiSilicon|![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516cv300_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516cv300-br.tgz)| DONE |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516ev100_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516ev100-br.tgz)| DONE |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516ev200_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516ev200-br.tgz)| DONE |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516ev300_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516ev300-br.tgz)| DONE |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3518ev300_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3518ev300-br.tgz)| DONE |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516dv200_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516dv200-br.tgz)| DONE |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516av300_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516av300-br.tgz)| WIP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516cv500_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516cv500-br.tgz)| WIP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516dv300_images.yml/badge.svg)     |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516dv300-br.tgz)| WIP  |
|MStar    |![Image](https://img.shields.io/static/v1?label=MSC313E&message=help%20needed&color=5500ff&logo=github) | | R&D   |
|         |![Image](https://img.shields.io/static/v1?label=MSC316D&message=help%20needed&color=5500ff&logo=github) | | R&D   |
|Novatek  |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/nt98562_images.yml/badge.svg)         |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.nt98562-br.tgz)    | R&D  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/nt98566_images.yml/badge.svg)         |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.nt98566-br.tgz)    | R&D  |
|SigmaStar|![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc325_images.yml/badge.svg)          |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc325-br.tgz)     | R&D  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc335_images.yml/badge.svg)          |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc335-br.tgz)     | MVP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc337_images.yml/badge.svg)          |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc337-br.tgz)     | MVP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc337de_images.yml/badge.svg)        |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc337de-br.tgz)   | WIP  |
|Ingenic  |![Image](https://img.shields.io/static/v1?label=T20&message=help%20needed&color=5500ff&logo=github)     | | R&D   |
|         |![Image](https://img.shields.io/static/v1?label=T21&message=help%20needed&color=5500ff&logo=github)     | | R&D   |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/t31_images.yml/badge.svg)             | | R&D   |
|Xiongmai |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/xm510_images.yml/badge.svg)           |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.xm510-br.tgz)      | MVP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/xm530_images.yml/badge.svg)           |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.xm530-br.tgz)      | MVP  |
|         |![Image](https://github.com/OpenIPC/openipc-2.1/actions/workflows/xm550_images.yml/badge.svg)           |[uImage + rootfs](https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.xm550-br.tgz)      | MVP  |

-----

### Current development status

| Chip        | U-Boot | Kernel | Rootfs | Video  | Audio  | GPIO   | USB    | WiFi   | MMC    | IPv6   |
|-------------|--------|--------|--------|--------|--------|--------|--------|--------|--------|--------|
| FH8632      |        |        |        |        |        |        |        |        |        |        |
| FH8852      |        |        |        |        |        |        |        |        |        |        |
| FH8856      |        |        |        |        |        |        |        |        |        |        |
| GK7102S     |        |        |        |        |        |        |        |        |        |        |
| GK7202v300  |        |        |        |        |        |        |        |        |        |        |
| GK7205v200  | ✅               | ✅               | ✅               | ✅               | ✅               |        | ✅               | ✅               |        |        |
| GK7205v300  |        |        |        |        |        |        |        |        |        |        |
| GK7605v100  | ✅               | ✅               | ✅               | ✅               | ✅               |        |        |        |        |        |
| Hi3516Cv300 | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               |
| Hi3516Ev100 | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               |
| Hi3516Ev200 | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               |
| Hi3516Ev300 | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               |
| Hi3518Ev300 | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               |
| Hi3516Dv200 | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               | ✅               |
| Hi3516Av300 |        | ✅               | ✅               | ✅               |        |        |        |        |        |        |
| Hi3516Cv500 |        | ✅               | ✅               | ✅               |        |        |        |        |        |        |
| Hi3516Dv300 |        | ✅               | ✅               | ✅               |        |        |        |        |        |        |
| MSC313E     |        |        |        |        |        |        |        |        |        |        |
| NT98562     |        |        |        |        |        |        |        |        |        |        |
| NT98566     |        |        |        |        |        |        |        |        |        |        |
| SSC325      |        | ✅               | ✅               |        |        |        |        |        |        |        |
| SSC335      |        | ✅               | ✅               | ✅               |        | ✅               | ✅               |        | ✅               |        |
| SSC337      |        | ✅               | ✅               | ✅               |        | ✅               | ✅               |        | ✅               |        |
| SSC337DE    |        | ✅               | ✅               |        |        |        |        |        |        |        |
| T20         |        |        |        |        |        |        |        |        |        |        |
| T21         |        |        |        |        |        |        |        |        |        |        |
| T31         |        |        |        |        |        |        |        |        |        |        |
| XM510       |        | ✅               | ✅               | ✅               |        |        |        |        |        |        |
| XM530       |        | ✅               | ✅               | ✅               |        |        |        |        |        |        |
| XM550       |        | ✅               | ✅               | ✅               |        |        |        |        |        |        |

### Commercial Support
<a id="commercial"></a>

If you're using OpenIPC in your product consider buying commercial support. It's
a win-win strategy that supports your business and helps core developers to work
full-time on the project.

Please contact [OpenIPC](mailto:flyrouter@gmail.com) to inquire further.
