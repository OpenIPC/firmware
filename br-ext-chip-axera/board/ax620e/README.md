# AX620E / AX630C

Status: `ax620e.lite.config` is the real working config from the
`axera-ax620e` kernel branch (openipc-linux-ax620e, branch
`axera-ax620e`) -- builds clean, produces a real `Image` + devicetree.

Target kernel version: 5.15.73, aarch64. This is not the public vendor
SDK's kernel version (4.19.125) -- it is the version a real deployed
Xiongmai IVG-A6 (AX620E) camera firmware's own kernel modules were actually
built against, read directly off their `vermagic=` string. Do not
re-derive this from the public SDK; it is wrong for this chip.
