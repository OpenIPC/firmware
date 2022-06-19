![OpenIPC Logo](https://cdn.themactep.com/images/logo_openipc.png)

## OpenIPC firmware
_(based on Buildroot)_

OpenIPC is a rapidly developing open source alternative firmware for popular IP cameras.

Historically, OpenIPC firmware supports SoC manufactured by HiSilicon, but as the development continues,
the list of supported processors expands. Today, it also includes chips from Ambarella, Anyka, Fullhan,
Goke, GrainMedia, Ingenic, MStar, Novatek, SigmaStar, XiongMai and is expected to grow further.

More information about the project is available [in our wiki](https://openipc.github.io/wiki/)
and [on the website](https://openipc.org/).

### Current development status <a href="#support">*</a>

[![CI status](https://img.shields.io/github/downloads/OpenIPC/firmware/total.svg)](https://github.com/OpenIPC/firmware/releases)
![GitHub repo size](https://img.shields.io/github/repo-size/OpenIPC/firmware)
![GitHub issues](https://img.shields.io/github/issues/OpenIPC/firmware)
![GitHub pull requests](https://img.shields.io/github/issues-pr/OpenIPC/firmware)
[![License](https://img.shields.io/github/license/OpenIPC/firmware)](https://opensource.org/licenses/MIT)
![Backers](https://opencollective.com/openipc/tiers/backer/badge.svg)
![Backers](https://opencollective.com/openipc/tiers/badge.svg)

[![Join the chat at https://gitter.im/OpenIPC/english](https://badges.gitter.im/OpenIPC/english.svg)](https://gitter.im/OpenIPC/english?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

**IMPORTANT!** Backup of original firmware is required because our U-Boot version will overwrite original crypto partition (it is not used by OpenIPC, but it will make impossible to restore to factory firmware if you will want).

We provide our own advanced U-boot release for the platform (that could replace the original board U-boot, depending on user's willingness). In the case when manufacturer has a password protected U-boot, consider to replacing it.

_**NB!** Downloads are for research only, without any guarantee._

| SoC                                                       | Status        | U-Boot               | Linux              | Video | Audio | GPIO | Sensors |
|-----------------------------------------------------------|:-------------:|:--------------------:|:------------------:|:-----:|:-----:|:----:|:-------:|
| Ambarella&nbsp;S2L<br>![AmbaS2L][AmbaS2L]                 | <br>![sRnD]   |                      |                    |       |       |      |         |
| Ambarella&nbsp;S3L<br>![AmbaS3L][AmbaS3L]                 | <br>![sWIP]   |                      | [⬇][fwAmbaS3L]     |       |       |      |         |
| Anyka&nbsp;AK3916Ev300<br>![AK3916Ev300][AK3916Ev300]     | <br>![sEQUIP] |                      | [⬇][fwAK3916Ev300] |       |       |      |         |
| Anyka&nbsp;AK3916Ev301<br>![AK3916Ev301][AK3916Ev301]     | <br>![sRnD]   |                      |                    |       |       |      |         |
| Anyka&nbsp;AK3918Ev200<br>![AK3918Ev200][AK3918Ev200]     | <br>![sHELP]  |                      | [⬇][fwAK3918Ev200] |       |       |      |         |
| Anyka&nbsp;AK3918Ev300<br>![AK3918Ev300][AK3918Ev300]     | <br>![sEQUIP] |                      | [⬇][fwAK3918Ev300] |       |       |      |         |
| Anyka&nbsp;AK3918Ev330<br>![AK3918Ev330][AK3918Ev330]     | <br>![sRnD]   |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8626v100<br>![FH8626v100][FH8626v100]      | <br>![sHELP]  |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8632v100<br>![FH8632v100][FH8632v100]      | <br>![sEQUIP] |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8652v100<br>![FH8652v100][FH8652v100]      | <br>![sHELP]  |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8852v100<br>![FH8652v100][FH8852v100]      | <br>![sWIP]   |                      | [⬇][fwFH8852v100]  |       |       |      |         |
| Fullhan&nbsp;FH8852v200<br>![FH8652v200][FH8852v200]      | <br>![sWIP]   |                      | [⬇][fwFH8852v200]  |       |       |      |         |
| Fullhan&nbsp;FH8852v210<br>![FH8652v210][FH8852v210]      | <br>![sEQUIP] |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8856v100<br>![FH8856v100][FH8856v100]      | <br>![sEQUIP] |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8856v200<br>![FH8856v200][FH8856v200]      | <br>![sWIP]   |                      | [⬇][fwFH8856v200]  |       |       |      |         |
| Fullhan&nbsp;FH8856v210<br>![FH8856v210][FH8856v210]      | <br>![sEQUIP] |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8858v200<br>![FH8858v200][FH8858v200]      | <br>![sEQUIP] |                      |                    |       |       |      |         |
| Fullhan&nbsp;FH8858v210<br>![FH8858v210][FH8858v210]      | <br>![sEQUIP] |                      |                    |       |       |      |         |
| Goke&nbsp;GK7102S<br>![GK7102S][GK7102S]                  | <br>![sRnD]   |                      |                    |       |       |      |         |
| Goke&nbsp;GK7202v300<br>![GK7202v300][GK7202v300]         | <br>![sDONE]  | [⬇][bootGK7202v300]  | [⬇][fwGK7202v300]  |  +    |  +    |  +   |         |
| Goke&nbsp;GK7205v200<br>![GK7205v200][GK7205v200]         | <br>![sDONE]  | [⬇][bootGK7205v200]  | [⬇][fwGK7205v200]  |  +    |  +    |  +   |         |
| Goke&nbsp;GK7205v210<br>![GK7205v210][GK7205v210]         | <br>![sDONE]  |                      | [⬇][fwGK7205v210]  |  +    |  +    |  +   |         |
| Goke&nbsp;GK7205v300<br>![GK7205v300][GK7205v300]         | <br>![sDONE]  | [⬇][bootGK7205v300]  | [⬇][fwGK7205v300]  |  +    |  +    |  +   |         |
| Goke&nbsp;GK7605v100<br>![GK7605v100][GK7605v100]         | <br>![sDONE]  | [⬇][bootGK7605v100]  | [⬇][fwGK7605v100]  |  +    |  +    |  +   |         |
| GrainMedia&nbsp;GM8135<br>![GM8135][GM8135]               | <br>![sEQUIP] |                      |                    |       |       |      |         |
| GrainMedia&nbsp;GM8136<br>![GM8136][GM8136]               | <br>![sMVP]   |                      | [⬇][fwGM8136]      |  +    |       |  +   |         |
| HiSilicon&nbsp;Hi3516Av100<br>![Hi3516Av100][Hi3516Av100] | <br>![sDONE]  | [⬇][bootHi3516Av100] | [⬇][fwHi3516Av100] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Av200<br>![Hi3516Av200][Hi3516Av200] | <br>![sDONE]  | [⬇][bootHi3516Av200] | [⬇][fwHi3516Av200] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Av300<br>![Hi3516Av300][Hi3516Av300] | <br>![sDONE]  | [⬇][bootHi3516Av300] | [⬇][fwHi3516Av300] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Cv100<br>![Hi3516Cv100][Hi3516Cv100] | <br>![sDONE]  | [⬇][bootHi3516Cv100] | [⬇][fwHi3516Cv100] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Cv200<br>![Hi3516Cv200][Hi3516Cv200] | <br>![sDONE]  | [⬇][bootHi3516Cv200] | [⬇][fwHi3516Cv200] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Cv300<br>![Hi3516Cv300][Hi3516Cv300] | <br>![sDONE]  | [⬇][bootHi3516Cv300] | [⬇][fwHi3516Cv300] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Cv500<br>![Hi3516Cv500][Hi3516Cv500] | <br>![sDONE]  | [⬇][bootHi3516Cv500] | [⬇][fwHi3516Cv500] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Dv100<br>![Hi3516Dv100][Hi3516Dv100] | <br>![sDONE]  | [⬇][bootHi3516Dv100] | [⬇][fwHi3516Dv100] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Dv200<br>![Hi3516Dv200][Hi3516Dv200] | <br>![sDONE]  | [⬇][bootHi3516Dv200] | [⬇][fwHi3516Dv200] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Dv300<br>![Hi3516Dv300][Hi3516Dv300] | <br>![sDONE]  | [⬇][bootHi3516Dv300] | [⬇][fwHi3516Dv300] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Ev100<br>![Hi3516Ev100][Hi3516Ev100] | <br>![sDONE]  | [⬇][bootHi3516Ev100] | [⬇][fwHi3516Ev100] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Ev200<br>![Hi3516Ev200][Hi3516Ev200] | <br>![sDONE]  | [⬇][bootHi3516Ev200] | [⬇][fwHi3516Ev200] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3516Ev300<br>![Hi3516Ev300][Hi3516Ev300] | <br>![sDONE]  | [⬇][bootHi3516Ev300] | [⬇][fwHi3516Ev300] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3518Cv100<br>![Hi3518Cv100][Hi3518Cv100] | <br>![sDONE]  | [⬇][bootHi3518Cv100] | [⬇][fwHi3518Cv100] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3518Ev100<br>![Hi3518Ev100][Hi3518Ev100] | <br>![sDONE]  | [⬇][bootHi3518Ev100] | [⬇][fwHi3518Ev100] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3518Ev200<br>![Hi3518Ev200][Hi3518Ev200] | <br>![sDONE]  | [⬇][bootHi3518Ev200] | [⬇][fwHi3518Ev200] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3518Ev300<br>![Hi3518Ev300][Hi3518Ev300] | <br>![sDONE]  | [⬇][bootHi3518Ev300] | [⬇][fwHi3518Ev300] |  +    |  +    |  +   |         |
| HiSilicon&nbsp;Hi3519v101<br>![Hi3519v101][Hi3519v101]    | <br>![sDONE]  | [⬇][bootHi3519v101]  | [⬇][fwHi3519v101]  |  +    |  +    |  +   |         |
| Ingenic&nbsp;T10<br>![T10][T10]                           | <br>![sWIP]   |                      | [⬇][fwT10]         |       |       |      |         |
| Ingenic&nbsp;T20<br>![T20][T20]                           | <br>![sMVP]   |                      | [⬇][fwT20]         |  +    |       |      |         |
| Ingenic&nbsp;T21<br>![T21][T21]                           | <br>![sMVP]   |                      | [⬇][fwT21]         |  +    |       |      |         |
| Ingenic&nbsp;T31<br>![T31][T31]                           | <br>![sMVP]   |                      | [⬇][fwT31]         |  +    |       |      |         |
| MStar&nbsp;MSC313E<br>![MSC313E][MSC313E]                 | <br>![sRnD]   |                      |                    |       |       |      |         |
| MStar&nbsp;MSC316D<br>![MSC316D][MSC316D]                 | <br>![sRnD]   |                      |                    |       |       |      |         |
| Novatek&nbsp;NT98562<br>![NT98562][NT98562]               | <br>![sWIP]   |                      | [⬇][fwNT98562]     |       |       |      |         |
| Novatek&nbsp;NT98566<br>![NT98566][NT98566]               | <br>![sWIP]   |                      | [⬇][fwNT98566]     |       |       |      |         |
| SigmaStar&nbsp;SSC325<br>![SSC325][SSC325]                | <br>![sRnD]   |                      | [⬇][fwSSC325]      |       |       |      |         |
| SigmaStar&nbsp;SSC335<br>![SSC335][SSC335]                | <br>![sMVP]   |                      | [⬇][fwSSC335]      |  +    |       |  +   |         |
| SigmaStar&nbsp;SSC337<br>![SSC337][SSC337]                | <br>![sMVP]   |                      | [⬇][fwSSC337]      |  +    |       |  +   |         |
| SigmaStar&nbsp;SSC337DE<br>![SSC337DE][SSC337DE]          | <br>![sRnD]   |                      | [⬇][fwSSC337DE]    |       |       |      |         |
| Xiongmai&nbsp;XM510<br>![XM510][XM510]                    | <br>![sMVP]   |                      | [⬇][fwXM510]       |  +    |       |  +   |         |
| Xiongmai&nbsp;XM530<br>![XM530][XM530]                    | <br>![sMVP]   |                      | [⬇][fwXM530]       |  +    |       |  +   |         |
| Xiongmai&nbsp;XM550<br>![XM550][XM550]                    | <br>![sMVP]   |                      | [⬇][fwXM550]       |  +    |       |  +   |         |

Typical development lifecycle:

When we acquire SDK for new SoC we add SoC name to the table with status `NoEquipment` (if we don't have specific hardware modules) or `HelpNeeded` (to seek developer who will be responsible for the SoC) and it's considered as starting state of development. Then gradually status changes -> ![sRnD] -> ![sWIP] -> ![sMVP] -> ![sDONE]

Therefore: there is no reason to have SoC in the table without having SDK.

![sDONE] Everything seems works like as in original firmware (but we're still waiting for feedback from you)

![sMVP] "Minimal Viable Product", basic system is done (video works at least on main channel), everything is clear (including hardware and software design), but due to lack of human resources we're looking for help from experienced embedded developers or sponsors to continue development and make it `DONE` in the future

![sWIP] "Work In Progress" stage, we're tinkering with hardware and code, and probably early birds can do tests on their boards and help us to move further

![sRnD] "Research&Development" stage, when we have platform SDK and probably hardware boards, but there is a lot of work ahead

![sHELP] There is hardware in our lab, but we're looking for experienced embedded developers or sponsors to start `RnD` stage

![sEQUIP] We have SDK for the platform, but we don't have specific hardware to continue development, you can donate it to our `RnD` guys (it's important for ancient boards which are in End-Of-Life state and there is no way to buy it with on the market)

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

If you have any specific questions concerning our project, feel free to [contact us](mailto:dev@openipc.org).

### Participating and Contribution

If you like what we do, and willing to intensify the development, please consider participating.

You can improve existing code and send us patches. You can add new features missing from our code.

You can help us to write a better documentation, proofread and correct our websites.

You can just donate some money to cover the cost of development and long-term maintaining of what we believe
is going to be the most stable, flexible, and open IP Network Camera Framework for users like yourself.

You can make a financial contribution to the project at [Open Collective](https://opencollective.com/openipc/contribute/backer-14335/checkout).

Thank you.

<p align="center">
<a href="https://opencollective.com/openipc/contribute/backer-14335/checkout" target="_blank"><img src="https://opencollective.com/webpack/donate/button@2x.png?color=blue" width="375" alt="Open Collective donate button"></a>
</p>


[sDONE]: https://img.shields.io/static/v1?label=%20&message=DONE&color=008000
[sMVP]: https://img.shields.io/static/v1?label=%20&message=MVP&color=FFFF00
[sRnD]: https://img.shields.io/static/v1?label=%20&message=RnD&color=008080
[sWIP]: https://img.shields.io/static/v1?label=%20&message=WIP&color=FF00FF
[sHELP]: https://img.shields.io/static/v1?label=%20&message=HelpNeeded&color=800000
[sEQUIP]: https://img.shields.io/static/v1?label=%20&message=NoEquipment&color=FF0000



[AmbaS2L]: https://img.shields.io/static/v1?label=AmbaS2L&message=%20&logo=github
[AmbaS3L]: https://github.com/OpenIPC/firmware/actions/workflows/ambarella-s3l_images.yml/badge.svg
[AK3916Ev301]: https://img.shields.io/static/v1?label=AK3916Ev301&message=%20&logo=github
[AK3916Ev300]: https://github.com/OpenIPC/firmware/actions/workflows/ak3916ev300_images.yml/badge.svg
[AK3918Ev200]: https://github.com/OpenIPC/firmware/actions/workflows/ak3918ev200_images.yml/badge.svg
[AK3918Ev300]: https://github.com/OpenIPC/firmware/actions/workflows/ak3918ev300_images.yml/badge.svg
[AK3918Ev330]: https://img.shields.io/static/v1?label=AK3918EV330&message=%20&logo=github
[FH8626v100]: https://img.shields.io/static/v1?label=FH8626V100&message=%20&logo=github
[FH8632v100]: https://img.shields.io/static/v1?label=FH8632V100&message=%20&logo=github
[FH8652v100]: https://img.shields.io/static/v1?label=FH8652V100&message=%20&logo=github
[FH8852v100]: https://github.com/OpenIPC/firmware/actions/workflows/fh8852v100_images.yml/badge.svg
[FH8852v200]: https://github.com/OpenIPC/firmware/actions/workflows/fh8852v200_images.yml/badge.svg
[FH8852v210]: https://img.shields.io/static/v1?label=FH8852V210&message=%20&logo=github
[FH8856v100]: https://img.shields.io/static/v1?label=FH8856V100&message=%20&logo=github
[FH8856v200]: https://github.com/OpenIPC/firmware/actions/workflows/fh8856v200_images.yml/badge.svg
[FH8856v210]: https://img.shields.io/static/v1?label=FH8856V210&message=%20&logo=github
[FH8858v200]: https://img.shields.io/static/v1?label=FH8856V200&message=%20&logo=github
[FH8858v210]: https://img.shields.io/static/v1?label=FH8856V210&message=%20&logo=github
[GK7102S]: https://img.shields.io/static/v1?label=GK7102S&message=%20&logo=github
[GK7202v300]: https://github.com/OpenIPC/firmware/actions/workflows/gk7202v300_images.yml/badge.svg
[GK7205v200]: https://github.com/OpenIPC/firmware/actions/workflows/gk7205v200_images.yml/badge.svg
[GK7205v210]: https://github.com/OpenIPC/firmware/actions/workflows/gk7205v210_images.yml/badge.svg
[GK7205v300]: https://github.com/OpenIPC/firmware/actions/workflows/gk7205v300_images.yml/badge.svg
[GK7605v100]: https://github.com/OpenIPC/firmware/actions/workflows/gk7605v100_images.yml/badge.svg
[GM8135]: https://img.shields.io/static/v1?label=GM8135&message=%20&logo=github
[GM8136]: https://github.com/OpenIPC/firmware/actions/workflows/gm8136_images.yml/badge.svg
[Hi3516Av100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516av100_images.yml/badge.svg
[Hi3516Av200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516av200_images.yml/badge.svg
[Hi3516Av300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516av300_images.yml/badge.svg
[Hi3516Cv100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516cv100_images.yml/badge.svg
[Hi3516Cv200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516cv200_images.yml/badge.svg
[Hi3516Cv300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516cv300_images.yml/badge.svg
[Hi3516Cv500]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516cv500_images.yml/badge.svg
[Hi3516Dv100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516dv100_images.yml/badge.svg
[Hi3516Dv200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516dv200_images.yml/badge.svg
[Hi3516Dv300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516dv300_images.yml/badge.svg
[Hi3516Ev100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516ev100_images.yml/badge.svg
[Hi3516Ev200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516ev200_images.yml/badge.svg
[Hi3516Ev300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3516ev300_images.yml/badge.svg
[Hi3518Cv100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3518cv100_images.yml/badge.svg
[Hi3518Ev100]: https://github.com/OpenIPC/firmware/actions/workflows/hi3518ev100_images.yml/badge.svg
[Hi3518Ev200]: https://github.com/OpenIPC/firmware/actions/workflows/hi3518ev200_images.yml/badge.svg
[Hi3518Ev300]: https://github.com/OpenIPC/firmware/actions/workflows/hi3518ev300_images.yml/badge.svg
[Hi3519v101]: https://github.com/OpenIPC/firmware/actions/workflows/hi3519v101_images.yml/badge.svg
[MSC313E]: https://img.shields.io/static/v1?label=MSC313E&message=%20&logo=github
[MSC316D]: https://img.shields.io/static/v1?label=MSC316D&message=%20&logo=github
[NT98562]: https://github.com/OpenIPC/firmware/actions/workflows/nt98562_images.yml/badge.svg
[NT98566]: https://github.com/OpenIPC/firmware/actions/workflows/nt98566_images.yml/badge.svg
[SSC325]: https://github.com/OpenIPC/firmware/actions/workflows/ssc325_images.yml/badge.svg
[SSC335]: https://github.com/OpenIPC/firmware/actions/workflows/ssc335_images.yml/badge.svg
[SSC337]: https://github.com/OpenIPC/firmware/actions/workflows/ssc337_images.yml/badge.svg
[SSC337DE]: https://github.com/OpenIPC/firmware/actions/workflows/ssc337de_images.yml/badge.svg
[T10]: https://github.com/OpenIPC/firmware/actions/workflows/t10_images.yml/badge.svg
[T20]: https://github.com/OpenIPC/firmware/actions/workflows/t20_images.yml/badge.svg
[T21]: https://github.com/OpenIPC/firmware/actions/workflows/t21_images.yml/badge.svg
[T31]: https://github.com/OpenIPC/firmware/actions/workflows/t31_images.yml/badge.svg
[XM510]: https://github.com/OpenIPC/firmware/actions/workflows/xm510_images.yml/badge.svg
[XM530]: https://github.com/OpenIPC/firmware/actions/workflows/xm530_images.yml/badge.svg
[XM550]: https://github.com/OpenIPC/firmware/actions/workflows/xm550_images.yml/badge.svg


[bootGK7202v300]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-gk7202v300-universal.bin
[bootGK7205v200]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-gk7205v200-universal.bin
[bootGK7205v210]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-gk7205v210-universal.bin
[bootGK7205v300]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-gk7205v300-universal.bin
[bootGK7605v100]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-gk7605v100-universal.bin
[bootHi3516Av100]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516av100-universal.bin
[bootHi3516Av200]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516av200-universal.bin
[bootHi3516Av300]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516av300-universal.bin
[bootHi3516Cv100]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516cv100-universal.bin
[bootHi3516Cv200]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516cv200-universal.bin
[bootHi3516Cv300]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516cv300-universal.bin
[bootHi3516Cv500]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516cv500-universal.bin
[bootHi3516Dv100]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516dv100-universal.bin
[bootHi3516Dv200]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516dv200-universal.bin
[bootHi3516Dv300]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516dv300-universal.bin
[bootHi3516Ev100]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516ev100-universal.bin
[bootHi3516Ev200]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516ev200-universal.bin
[bootHi3516Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3516ev300-universal.bin
[bootHi3518Cv100]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3518cv100-universal.bin
[bootHi3518Ev100]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3518ev100-universal.bin
[bootHi3518Ev200]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3518ev200-universal.bin
[bootHi3518Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3518ev300-universal.bin
[bootHi3519v101]: https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3519v101-universal.bin


[fwAmbaS2L]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ambarella-s2l-br.tgz
[fwAmbaS3L]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ambarella-s3l-br.tgz
[fwAK3916Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ak3916ev300-br.tgz
[fwAK3916Ev301]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ak3916ev301-br.tgz
[fwAK3918Ev200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ak3918ev200-br.tgz
[fwAK3918Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ak3918ev300-br.tgz
[fwAK3918Ev330]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ak3918ev330-br.tgz
[fwFH8626v100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8626v100-br.tgz
[fwFH8632v100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8632v100-br.tgz
[fwFH8652v100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8652v100-br.tgz
[fwFH8852v100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8852v100-br.tgz
[fwFH8852v200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8852v200-br.tgz
[fwFH8856v100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8856v100-br.tgz
[fwFH8856v200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8856v200-br.tgz
[fwFH8856v210]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8856v210-br.tgz
[fwFH8858v200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8858v200-br.tgz
[fwFH8858v210]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8858v210-br.tgz
[fwFH8856v200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.fh8856v200-br.tgz
[fwGK7102S]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7102s-br.tgz
[fwGK7202v300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7202v300-br.tgz
[fwGK7205v200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7205v200-br.tgz
[fwGK7205v210]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7205v210-br.tgz
[fwGK7205v300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7205v300-br.tgz
[fwGK7605v100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gk7605v100-br.tgz
[fwGM8135]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gm8135-br.tgz
[fwGM8136]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.gm8136-br.tgz
[fwHi3516Av100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516av100-br.tgz
[fwHi3516Av200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516av200-br.tgz
[fwHi3516Av300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516av300-br.tgz
[fwHi3516Cv100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516cv100-br.tgz
[fwHi3516Cv200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516cv200-br.tgz
[fwHi3516Cv300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516cv300-br.tgz
[fwHi3516Cv500]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516cv500-br.tgz
[fwHi3516Dv100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516dv100-br.tgz
[fwHi3516Dv200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516dv200-br.tgz
[fwHi3516Dv300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516dv300-br.tgz
[fwHi3516Ev100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516ev100-br.tgz
[fwHi3516Ev200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516ev200-br.tgz
[fwHi3516Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516ev300-br.tgz
[fwHi3518Cv100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3518cv100-br.tgz
[fwHi3518Ev100]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3518ev100-br.tgz
[fwHi3518Ev200]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3518ev200-br.tgz
[fwHi3518Ev300]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3518ev300-br.tgz
[fwHi3519v101]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3519v101-br.tgz
[fwMSC313E]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.msc313e-br.tgz
[fwMSC316D]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.msc316d-br.tgz
[fwNT98562]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.nt98562-br.tgz
[fwNT98566]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.nt98566-br.tgz
[fwSSC325]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc325-br.tgz
[fwSSC335]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc335-br.tgz
[fwSSC337]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc337-br.tgz
[fwSSC337DE]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc337de-br.tgz
[fwT10]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.t10-br.tgz
[fwT20]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.t20-br.tgz
[fwT21]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.t21-br.tgz
[fwT31]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.t31-br.tgz
[fwXM510]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.xm510-br.tgz
[fwXM530]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.xm530-br.tgz
[fwXM550]: https://github.com/OpenIPC/firmware/releases/download/latest/openipc.xm550-br.tgz
