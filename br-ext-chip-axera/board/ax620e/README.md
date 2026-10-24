# AX620E / AX630C

Status: scaffold. `ax620e.lite.config` (the kernel config this family's
defconfig points at) does not exist yet -- it depends on the
`axera-ax620e` kernel branch, still in progress.

Target kernel version: 5.15.73, aarch64. This is not the public vendor
SDK's kernel version (4.19.125) -- it is the version a real deployed
Xiongmai IVG-A6 (AX620E) camera firmware's own kernel modules were actually
built against, read directly off their `vermagic=` string. Do not
re-derive this from the public SDK; it is wrong for this chip.
