// SPDX-License-Identifier: MIT
/*
 * Not a module: Kbuild compiles this to assembly only, with the kernel's own
 * headers and flags, so the package can read the offsets the prebuilt iav.ko
 * depends on. See ambarella-osdrv-s3l.mk.
 */
#include <linux/fs.h>
#include <linux/kbuild.h>

void abi_probe(void)
{
	DEFINE(INODE_I_CDEV, offsetof(struct inode, i_cdev));
	DEFINE(FILE_PRIVATE_DATA, offsetof(struct file, private_data));
}
