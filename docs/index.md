![OpenIPC Logo](https://cdn.themactep.com/images/logo_openipc.png)

## OpenIPC v2.1
_(based on Buildroot)_

OpenIPC is a rapidly developing open source alternative firmware for popular IP cameras.

Historically, OpenIPC firmware supports SoC manufactured by HiSilicon, but as the development continues,
the list of supported processors expands. Today, it also includes chips from Goke, SigmaStar, Xiongmai,
and is expected to grow further.

More information about the project is available [in our wiki](https://github.com/OpenIPC/openipc-2.1/wiki)
and [on the website](https://openipc.org/).

### Current development status <a href="#commercial">*</a>

[![CI status](https://img.shields.io/github/downloads/OpenIPC/openipc-2.1/total.svg)](https://github.com/OpenIPC/openipc-2.1/releases)
![GitHub repo size](https://img.shields.io/github/repo-size/OpenIPC/openipc-2.1)
![GitHub issues](https://img.shields.io/github/issues/OpenIPC/openipc-2.1)
![GitHub pull requests](https://img.shields.io/github/issues-pr/OpenIPC/openipc-2.1)
[![License](https://img.shields.io/github/license/OpenIPC/openipc-2.1)](https://opensource.org/licenses/MIT)
![Backers](https://opencollective.com/openipc/tiers/backer/badge.svg)
![Backers](https://opencollective.com/openipc/tiers/badge.svg)

_NB! Downloads are for research only, without any guarantee._

| SoC                                                    | U-Boot | Kernel | Rootfs | USB | WiFi | MMC | IPv6 |    D/L    | Status |
|--------------------------------------------------------|:------:|:------:|:------:|:---:|:----:|:---:|:----:|:---------:|--------|
| `Fullhan FH8632`<br>![FH8632][FH8632]                  |        |        |        |     |      |     |      |           | R&D    |
| `Fullhan FH8852`<br>![FH8652][FH8852]                  |        |        |        |     |      |     |      |           | R&D    |
| `Fullhan FH8856`<br>![FH8856][FH8856]                  |        |        |        |     |      |     |      |           | R&D    |
| `Goke GK7102S`<br>![GK7102S][GK7102S]                  |        |        |        |     |      |     |      |           | R&D    |
| `Goke GK7202v300`<br>![GK7202v300][GK7202v300]         |        |        |        |     |      |     |      | [⬇][dl01] | WIP    |
| `Goke GK7205v200`<br>![GK7205v200][GK7205v200]         |   +    |   +    |   +    |  +  |  +   |     |      | [⬇][dl02] | DONE   |
| `Goke GK7205v300`<br>![GK7205v300][GK7205v300]         |        |        |        |     |      |     |      | [⬇][dl03] | WIP    |
| `Goke GK7605v100`<br>![GK7605v100][GK7605v100]         |   +    |   +    |   +    |     |      |     |      | [⬇][dl04] | WIP    |
| `HiSilicon Hi3516Av100`<br>![Hi3516Av100][Hi3516Av100] |        |   +    |   +    |     |      |     |      | [⬇][dl05] | WIP    |
| `HiSilicon Hi3516Av300`<br>![Hi3516Av300][Hi3516Av300] |        |   +    |   +    |     |      |     |      | [⬇][dl06] | WIP    |
| `HiSilicon Hi3516Cv200`<br>![Hi3516Cv200][Hi3516Cv200] |        |   +    |   +    |  +  |  +   |  +  |      | [⬇][dl07] | DONE   |
| `HiSilicon Hi3516Cv300`<br>![Hi3516Cv300][Hi3516Cv300] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][dl08] | DONE   |
| `HiSilicon Hi3516Cv500`<br>![Hi3516Cv500][Hi3516Cv500] |        |   +    |   +    |     |      |     |      | [⬇][dl09] | WIP    |
| `HiSilicon Hi3516Dv100`<br>![Hi3516Dv100][Hi3516Dv100] |        |   +    |   +    |     |      |     |      | [⬇][dl10] | WIP    |
| `HiSilicon Hi3516Dv200`<br>![Hi3516Dv200][Hi3516Dv200] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][dl11] | DONE   |
| `HiSilicon Hi3516Dv300`<br>![Hi3516Dv300][Hi3516Dv300] |        |   +    |   +    |     |      |     |      | [⬇][dl12] | WIP    |
| `HiSilicon Hi3516Ev100`<br>![Hi3516Ev100][Hi3516Ev100] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][dl13] | DONE   |
| `HiSilicon Hi3516Ev200`<br>![Hi3516Ev200][Hi3516Ev200] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][dl14] | DONE   |
| `HiSilicon Hi3516Ev300`<br>![Hi3516Ev300][Hi3516Ev300] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][dl15] | DONE   |
| `HiSilicon Hi3518Ev200`<br>![Hi3518Ev200][Hi3518Ev200] |        |   +    |   +    |  +  |  +   |  +  |      | [⬇][dl16] | DONE   |
| `HiSilicon Hi3518Ev300`<br>![Hi3518Ev300][Hi3518Ev300] |   +    |   +    |   +    |  +  |  +   |  +  |  +   | [⬇][dl17] | DONE   |
| `MStar MSC313E`<br>![MSC313E][MSC313E]                 |        |        |        |     |      |     |      |           | R&D    |
| `MStar MSC316D`<br>![MSC316D][MSC316D]                 |        |        |        |     |      |     |      |           | R&D    |
| `Novatek NT98562`<br>![NT98562][NT98562]               |        |        |        |     |      |     |      | [⬇][dl18] | R&D    |
| `Novatek NT98566`<br>![NT98566][NT98566]               |        |        |        |     |      |     |      | [⬇][dl19] | R&D    |
| `SigmaStar SSC325`<br>![SSC325][SSC325]                |        |   +    |   +    |     |      |     |      | [⬇][dl20] | R&D    |
| `SigmaStar SSC335`<br>![SSC335][SSC335]                |        |   +    |   +    |  +  |      |  +  |      | [⬇][dl21] | MVP    |
| `SigmaStar SSC337`<br>![SSC337][SSC337]                |        |   +    |   +    |  +  |      |  +  |      | [⬇][dl22] | MVP    |
| `SigmaStar SSC337DE`<br>![SSC337DE][SSC337DE]          |        |   +    |   +    |     |      |     |      | [⬇][dl23] | WIP    |
| `Ingenic T10`<br>![T10][T10]                           |        |        |        |     |      |     |      |           | R&D    |
| `Ingenic T20`<br>![T20][T20]                           |        |        |        |     |      |     |      |           | R&D    |
| `Ingenic T21`<br>![T21][T21]                           |        |        |        |     |      |     |      |           | R&D    |
| `Ingenic T31`<br>![T31][T31]                           |        |        |        |     |      |     |      |           | R&D    |
| `Xiongmai XM510`<br>![XM510][XM510]                    |        |   +    |   +    |     |      |     |      | [⬇][dl24] | MVP    |
| `Xiongmai XM530`<br>![XM530][XM530]                    |        |   +    |   +    |     |      |     |      | [⬇][dl25] | MVP    |
| `Xiongmai XM550`<br>![XM550][XM550]                    |        |   +    |   +    |     |      |     |      | [⬇][dl26] | MVP    |


### Support

OpenIPC offers two levels of support.

- Free support through the community (via [chat](https://openipc.org/#telegram-chat-groups) and mailing lists).
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


[FH8632]: https://img.shields.io/static/v1?label=FH8632&message=help%20needed&color=5500ff&logo=github
[FH8852]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/fh8852_images.yml/badge.svg
[FH8856]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/fh8856_images.yml/badge.svg
[GK7102S]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7102s_images.yml/badge.svg
[GK7202v300]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7202v300_images.yml/badge.svg
[GK7205v200]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7205v200_images.yml/badge.svg
[GK7205v300]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7205v300_images.yml/badge.svg
[GK7605v100]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/gk7605v100_images.yml/badge.svg
[Hi3516Av100]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516av100_images.yml/badge.svg
[Hi3516Av300]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516av300_images.yml/badge.svg
[Hi3516Cv200]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516cv200_images.yml/badge.svg
[Hi3516Cv300]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516cv300_images.yml/badge.svg
[Hi3516Cv500]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516cv500_images.yml/badge.svg
[Hi3516Dv100]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516dv100_images.yml/badge.svg
[Hi3516Dv200]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516dv200_images.yml/badge.svg
[Hi3516Dv300]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516dv300_images.yml/badge.svg
[Hi3516Ev100]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516ev100_images.yml/badge.svg
[Hi3516Ev200]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516ev200_images.yml/badge.svg
[Hi3516Ev300]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3516ev300_images.yml/badge.svg
[Hi3518Ev200]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3518ev200_images.yml/badge.svg
[Hi3518Ev300]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/hi3518ev300_images.yml/badge.svg
[MSC313E]: https://img.shields.io/static/v1?label=MSC313E&message=help%20needed&color=5500ff&logo=github
[MSC316D]: https://img.shields.io/static/v1?label=MSC316D&message=help%20needed&color=5500ff&logo=github
[NT98562]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/nt98562_images.yml/badge.svg
[NT98566]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/nt98566_images.yml/badge.svg
[SSC325]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc325_images.yml/badge.svg
[SSC335]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc335_images.yml/badge.svg
[SSC337]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc337_images.yml/badge.svg
[SSC337DE]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/ssc337de_images.yml/badge.svg
[T10]: https://img.shields.io/static/v1?label=T10&message=help%20needed&color=5500ff&logo=github
[T20]: https://img.shields.io/static/v1?label=T20&message=help%20needed&color=5500ff&logo=github
[T21]: https://img.shields.io/static/v1?label=T21&message=help%20needed&color=5500ff&logo=github
[T31]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/t31_images.yml/badge.svg
[XM510]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/xm510_images.yml/badge.svg
[XM530]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/xm530_images.yml/badge.svg
[XM550]: https://github.com/OpenIPC/openipc-2.1/actions/workflows/xm550_images.yml/badge.svg

[dl01]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7202v300-br.tgz
[dl02]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7205v200-br.tgz
[dl03]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7205v300-br.tgz
[dl04]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.gk7605v100-br.tgz
[dl05]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516av100-br.tgz
[dl06]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516av300-br.tgz
[dl07]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516cv200-br.tgz
[dl08]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516cv300-br.tgz
[dl09]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516cv500-br.tgz
[dl10]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516dv100-br.tgz
[dl11]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516dv200-br.tgz
[dl12]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516dv300-br.tgz
[dl13]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516ev100-br.tgz
[dl14]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516ev200-br.tgz
[dl15]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3516ev300-br.tgz
[dl16]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3518ev200-br.tgz
[dl17]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.hi3518ev300-br.tgz
[dl18]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.nt98562-br.tgz
[dl19]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.nt98566-br.tgz
[dl20]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc325-br.tgz
[dl21]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc335-br.tgz
[dl22]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc337-br.tgz
[dl23]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.ssc337de-br.tgz
[dl24]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.xm510-br.tgz
[dl25]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.xm530-br.tgz
[dl26]: https://github.com/OpenIPC/openipc-2.1/releases/download/latest/openipc.xm550-br.tgz
