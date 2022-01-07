![OpenIPC Logo](https://cdn.themactep.com/images/logo_openipc.png)

## OpenIPC firmware
_(based on Buildroot)_

OpenIPC is a rapidly developing open source alternative firmware for popular IP cameras.

Historically, OpenIPC firmware supports SoC manufactured by HiSilicon, but as the development continues,
the list of supported processors expands. Today, it also includes chips from Goke, SigmaStar, Xiongmai,
and is expected to grow further.

More information about the project is available [in our wiki](https://github.com/OpenIPC/firmware/wiki)
and [on the website](https://openipc.org/).

### Current development status <a href="#commercial">*</a>

[![CI status](https://img.shields.io/github/downloads/OpenIPC/firmware/total.svg)](https://github.com/OpenIPC/firmware/releases)
![GitHub repo size](https://img.shields.io/github/repo-size/OpenIPC/firmware)
![GitHub issues](https://img.shields.io/github/issues/OpenIPC/firmware)
![GitHub pull requests](https://img.shields.io/github/issues-pr/OpenIPC/firmware)
[![License](https://img.shields.io/github/license/OpenIPC/firmware)](https://opensource.org/licenses/MIT)
![Backers](https://opencollective.com/openipc/tiers/backer/badge.svg)
![Backers](https://opencollective.com/openipc/tiers/badge.svg)

_NB! Downloads are for research only, without any guarantee._

| SoC                                                       | U-Boot | Kernel | Rootfs | USB | WiFi | MMC | IPv6 |        D/L         | Status |
|-----------------------------------------------------------|:------:|:------:|:------:|:---:|:----:|:---:|:----:|:------------------:|--------|
| Ambarella&nbsp;S3L<br>![AmbaS3L][AmbaS3L]                 |        |        |        |     |      |     |      |                    | R&D    |
| Fullhan&nbsp;FH8632<br>![FH8632][FH8632]                  |        |        |        |     |      |     |      |                    | R&D    |
| Fullhan&nbsp;FH8852<br>![FH8652][FH8852]                  |        |        |        |     |      |     |      |                    | R&D    |
| Fullhan&nbsp;FH8856<br>![FH8856][FH8856]                  |        |        |        |     |      |     |      |                    | R&D    |
| Goke&nbsp;GK7102S<br>![GK7102S][GK7102S]                  |        |        |        |     |      |     |      |                    | R&D    |
| Goke&nbsp;GK7202v300<br>![GK7202v300][GK7202v300]         |        |        |        |     |      |     |      | [⬇][fwGK7202v300]  | WIP    |
| Goke&nbsp;GK7205v200<br>![GK7205v200][GK7205v200]         |   +    |   +    |   +    |  +  |  +   |     |      | [⬇][fwGK7205v200]  | DONE   |
| Goke&nbsp;GK7205v300<br>![GK7205v300][GK7205v300]         |        |        |        |     |      |     |      | [⬇][fwGK7205v300]  | DONE   |
| Goke&nbsp;GK7605v100<br>![GK7605v100][GK7605v100]         |   +    |   +    |   +    |     |      |     |      | [⬇][fwGK7605v100]  | WIP    |
| HiSilicon&nbsp;Hi3516Av100<br>![Hi3516Av100][Hi3516Av100] |        |   +    |   +    |     |      |     |      | [⬇][fwHi3516Av100] | WIP    |
| HiSilicon&nbsp;Hi3516Av200<br>![Hi3516Av200][Hi3516Av200] |        |   +    |   +    |     |      |     |      | [⬇][fwHi3516Av200] | WIP    |
| HiSilicon&nbsp;Hi3516Av300<br>![Hi3516Av300][Hi3516Av300] |        |   +    |   +    |     |      |     |      | [⬇][fwHi3516Av300] | WIP    |
| HiSilicon&nbsp;Hi3516Cv200<br>![Hi3516Cv200][Hi3516Cv200] |        |   +    |   +    |  +  |  +   |  +  |      | [⬇][fwHi3516Cv200] | DONE   |
| HiSilicon&nbsp;Hi3516Cv300<br>![Hi3516Cv300][Hi3516Cv300] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][fwHi3516Cv300] | DONE   |
| HiSilicon&nbsp;Hi3516Cv500<br>![Hi3516Cv500][Hi3516Cv500] |        |   +    |   +    |     |      |     |      | [⬇][fwHi3516Cv500] | WIP    |
| HiSilicon&nbsp;Hi3516Dv100<br>![Hi3516Dv100][Hi3516Dv100] |        |   +    |   +    |     |      |     |      | [⬇][fwHi3516Dv100] | WIP    |
| HiSilicon&nbsp;Hi3516Dv200<br>![Hi3516Dv200][Hi3516Dv200] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][fwHi3516Dv200] | DONE   |
| HiSilicon&nbsp;Hi3516Dv300<br>![Hi3516Dv300][Hi3516Dv300] |        |   +    |   +    |     |      |     |      | [⬇][fwHi3516Dv300] | WIP    |
| HiSilicon&nbsp;Hi3516Ev100<br>![Hi3516Ev100][Hi3516Ev100] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][fwHi3516Ev100] | DONE   |
| HiSilicon&nbsp;Hi3516Ev200<br>![Hi3516Ev200][Hi3516Ev200] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][fwHi3516Ev200] | DONE   |
| HiSilicon&nbsp;Hi3516Ev300<br>![Hi3516Ev300][Hi3516Ev300] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][fwHi3516Ev300] | DONE   |
| HiSilicon&nbsp;Hi3518Ev200<br>![Hi3518Ev200][Hi3518Ev200] |        |   +    |   +    |  +  |  +   |  +  |      | [⬇][fwHi3518Ev200] | DONE   |
| HiSilicon&nbsp;Hi3518Ev300<br>![Hi3518Ev300][Hi3518Ev300] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][fwHi3518Ev300] | DONE   |
| HiSilicon&nbsp;Hi3519v101<br>![Hi3519v101][Hi3519v101]    |        |   +    |   +    |     |      |     |      | [⬇][fwHi3519v101]  | WIP    |
| Ingenic&nbsp;T10<br>![T10][T10]                           |        |        |        |     |      |     |      |                    | R&D    |
| Ingenic&nbsp;T20<br>![T20][T20]                           |        |        |        |     |      |     |      |                    | R&D    |
| Ingenic&nbsp;T21<br>![T21][T21]                           |        |        |        |     |      |     |      |                    | R&D    |
| Ingenic&nbsp;T31<br>![T31][T31]                           |        |        |        |     |      |     |      |                    | R&D    |
| MStar&nbsp;MSC313E<br>![MSC313E][MSC313E]                 |        |        |        |     |      |     |      |                    | R&D    |
| MStar&nbsp;MSC316D<br>![MSC316D][MSC316D]                 |        |        |        |     |      |     |      |                    | R&D    |
| Novatek&nbsp;NT98562<br>![NT98562][NT98562]               |        |        |        |     |      |     |      |   [⬇][fwNT98562]   | R&D    |
| Novatek&nbsp;NT98566<br>![NT98566][NT98566]               |        |        |        |     |      |     |      |   [⬇][fwNT98566]   | R&D    |
| SigmaStar&nbsp;SSC325<br>![SSC325][SSC325]                |        |   +    |   +    |     |      |     |      |   [⬇][fwSSC325]    | R&D    |
| SigmaStar&nbsp;SSC335<br>![SSC335][SSC335]                |        |   +    |   +    |  +  |      |  +  |      |   [⬇][fwSSC335]    | MVP    |
| SigmaStar&nbsp;SSC337<br>![SSC337][SSC337]                |        |   +    |   +    |  +  |      |  +  |      |   [⬇][fwSSC337]    | MVP    |
| SigmaStar&nbsp;SSC337DE<br>![SSC337DE][SSC337DE]          |        |   +    |   +    |     |      |     |      |  [⬇][fwSSC337DE]   | TEST   |
| Xiongmai&nbsp;XM510<br>![XM510][XM510]                    |        |   +    |   +    |     |      |     |      |    [⬇][fwXM510]    | MVP    |
| Xiongmai&nbsp;XM530<br>![XM530][XM530]                    |        |   +    |   +    |     |      |     |      |    [⬇][fwXM530]    | MVP    |
| Xiongmai&nbsp;XM550<br>![XM550][XM550]                    |        |   +    |   +    |     |      |     |      |    [⬇][fwXM550]    | MVP    |

-----

### Support

OpenIPC offers two levels of support.

- Free support through the community (via [chat](https://openipc.org/#telegram-chat-groups)).
- Paid commercial support (from the team of developers).

Please consider subscribing for paid commercial support if you intend to use our product for business.
As a paid customer, you will get technical support and maintenance services directly from our skilled team.
Your bug reports and feature requests will get prioritized attention and expedited solutions. It's a win-win
strategy for both parties, that would contribute to the stability your business, and help core developers
to work on the project full-time.

If you have any specific questions concerning our project, feel free to [contact us](mailto:flyrouter@gmail.com).

### Participating and Contribution

If you like what we do, and willing to intensify the development, please consider participating.

You can improve existing code and send us patches. You can add new features missing from our code.

You can help us to write a better documentation, proofread and correct our websites.

You can just donate some money to cover the cost of development and long-term maintaining of what we believe
is going to be the most stable, flexible, and open IP Network Camera Framework for users like yourself.

You can make a financial contribution to the project
at [Open Collective](https://opencollective.com/openipc/contribute/backer-14335/checkout),
or via [PayPal](https://www.paypal.com/donate/?hosted_button_id=C6F7UJLA58MBS),
or via [YooMoney](https://openipc.org/donation/yoomoney.html).

Thank you.

<a href="https://opencollective.com/openipc/contribute/backer-14335/checkout" target="_blank"><img src="https://opencollective.com/webpack/donate/button@2x.png?color=blue" width="375" alt="Open Collective donate button"></a>
<a href="https://www.paypal.com/donate/?hosted_button_id=C6F7UJLA58MBS"><img src="https://www.paypalobjects.com/en_US/IT/i/btn/btn_donateCC_LG.gif" alt="PayPal donate button"></a>
<a href="https://openipc.org/donation/yoomoney.html"><img src="https://yoomoney.ru/transfer/balance-informer/balance?id=596194605&key=291C29A811B500D7" width="140" alt="YooMoney donate button"></a>


[AmbaS3L]: https://img.shields.io/static/v1?label=AmbaS3L&message=help%20needed&color=5500ff&logo=github
[FH8632]: https://img.shields.io/static/v1?label=FH8632&message=help%20needed&color=5500ff&logo=github
[FH8852]: https://img.shields.io/static/v1?label=FH8852&message=help%20needed&color=5500ff&logo=github
[FH8856]: https://img.shields.io/static/v1?label=FH8856&message=help%20needed&color=5500ff&logo=github
[GK7102S]: https://img.shields.io/static/v1?label=GK7102S&message=help%20needed&color=5500ff&logo=github
[GK7202v300]: https://github.com/OpenIPC/firmware/actions/workflows/gk7202v300_images.yml/badge.svg
[GK7205v200]: https://github.com/OpenIPC/firmware/actions/workflows/gk7205v200_images.yml/badge.svg
[GK7205v300]: https://github.com/OpenIPC/firmware/actions/workflows/gk7205v300_images.yml/badge.svg
[GK7605v100]: https://github.com/OpenIPC/firmware/actions/workflows/gk7605v100_images.yml/badge.svg
[Hi3516Av100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516av100_images.yml/badge.svg
[Hi3516Av200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516av200_images.yml/badge.svg
[Hi3516Av300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516av300_images.yml/badge.svg
[Hi3516Cv200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516cv200_images.yml/badge.svg
[Hi3516Cv300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516cv300_images.yml/badge.svg
[Hi3516Cv500]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516cv500_images.yml/badge.svg
[Hi3516Dv100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516dv100_images.yml/badge.svg
[Hi3516Dv200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516dv200_images.yml/badge.svg
[Hi3516Dv300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516dv300_images.yml/badge.svg
[Hi3516Ev100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516ev100_images.yml/badge.svg
[Hi3516Ev200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516ev200_images.yml/badge.svg
[Hi3516Ev300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516ev300_images.yml/badge.svg
[Hi3518Ev200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3518ev200_images.yml/badge.svg
[Hi3518Ev300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3518ev300_images.yml/badge.svg
[Hi3519v101]: https://github.com/OpenIPC/firmware/actions/workflows/hi3519v101_images.yml/badge.svg
[MSC313E]: https://img.shields.io/static/v1?label=MSC313E&message=help%20needed&color=5500ff&logo=github
[MSC316D]: https://img.shields.io/static/v1?label=MSC316D&message=help%20needed&color=5500ff&logo=github
[NT98562]: https://github.com/OpenIPC/firmware/actions/workflows/nt98562_images.yml/badge.svg
[NT98566]: https://github.com/OpenIPC/firmware/actions/workflows/nt98566_images.yml/badge.svg
[SSC325]: https://github.com/OpenIPC/firmware/actions/workflows/ssc325_images.yml/badge.svg
[SSC335]: https://github.com/OpenIPC/firmware/actions/workflows/ssc335_images.yml/badge.svg
[SSC337]: https://github.com/OpenIPC/firmware/actions/workflows/ssc337_images.yml/badge.svg
[SSC337DE]: https://github.com/OpenIPC/firmware/actions/workflows/ssc337de_images.yml/badge.svg
[T10]: https://img.shields.io/static/v1?label=T10&message=help%20needed&color=5500ff&logo=github
[T20]: https://img.shields.io/static/v1?label=T20&message=help%20needed&color=5500ff&logo=github
[T21]: https://img.shields.io/static/v1?label=T21&message=help%20needed&color=5500ff&logo=github
[T31]: https://img.shields.io/static/v1?label=T31&message=help%20needed&color=5500ff&logo=github
[XM510]: https://github.com/OpenIPC/firmware/actions/workflows/xm510_images.yml/badge.svg
[XM530]: https://github.com/OpenIPC/firmware/actions/workflows/xm530_images.yml/badge.svg
[XM550]: https://github.com/OpenIPC/firmware/actions/workflows/xm550_images.yml/badge.svg

[fwGK7202v300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7202v300-br.tgz
[fwGK7205v200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7205v200-br.tgz
[fwGK7205v300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7205v300-br.tgz
[fwGK7605v100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7605v100-br.tgz
[fwHi3516Av100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516av100-br.tgz
[fwHi3516Av200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516av200-br.tgz
[fwHi3516Av300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516av300-br.tgz
[fwHi3516Cv200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516cv200-br.tgz
[fwHi3516Cv300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516cv300-br.tgz
[fwHi3516Cv500]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516cv500-br.tgz
[fwHi3516Dv100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516dv100-br.tgz
[fwHi3516Dv200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516dv200-br.tgz
[fwHi3516Dv300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516dv300-br.tgz
[fwHi3516Ev100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516ev100-br.tgz
[fwHi3516Ev200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516ev200-br.tgz
[fwHi3516Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516ev300-br.tgz
[fwHi3518Ev200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3518ev200-br.tgz
[fwHi3518Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3518ev300-br.tgz
[fwHi3519v101]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3519v101-br.tgz
[fwNT98562]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.nt98562-br.tgz
[fwNT98566]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.nt98566-br.tgz
[fwSSC325]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc325-br.tgz
[fwSSC335]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc335-br.tgz
[fwSSC337]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc337-br.tgz
[fwSSC337DE]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc337de-br.tgz
[fwXM510]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.xm510-br.tgz
[fwXM530]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.xm530-br.tgz
[fwXM550]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.xm550-br.tgz
