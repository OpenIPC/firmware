# AX620U

Status: scaffold. `ax620u.lite.config` does not exist yet -- it
depends on the `axera-ax620u` kernel branch, not started.

Target kernel version: 4.19.125, ARMv7 (Cortex-A7 x4). Read directly
off a real deployed AX620U camera firmware's kernel modules'
`vermagic=` string, not assumed. AX620U is a distinct SoC from
AX620E/AX630C/AX620Q (root devicetree `compatible = "axera,ax620x"`,
not `"axera,ax620e"`) with its own register map and no public
open-source driver base to start from -- this is new driver work, not
a backport.
