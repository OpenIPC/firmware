// SPDX-License-Identifier: GPL-2.0-or-later
#include <linux/init.h>
#include <linux/string.h>
#include <linux/version.h>
#include <linux/etherdevice.h>

// mac_pton moved in v6.4
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
#include <linux/hex.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,8)
#include <linux/kernel.h>
#else
#error "openipc_ethaddr(): mac_pton not supported in kernel"
#endif

// Default u-boot env ethaddr on openipc platforms
static const u8 default_mac[ETH_ALEN] = {0x00,0x00,0x23,0x34,0x45,0x66};
static u8 openipc_ethaddr[ETH_ALEN] __aligned(2);

static int __init openipc_early_ethaddr(char *str)
{
	u8 mac[ETH_ALEN] __aligned(2);

	if (!str)
		return 0;

	if (!mac_pton(str, mac)) {
		pr_warn("openipc_ethaddr: invalid ethaddr bootarg '%s'\n", str);
		return 0;
	}

	if (ether_addr_equal(mac, default_mac))
		pr_warn("openipc_ethaddr: ignoring default ethaddr '%s'\n",
				default_mac);
	else
		ether_addr_copy(openipc_ethaddr, mac);

	return 0;
}
early_param("ethaddr", openipc_early_ethaddr);

unsigned char *arch_get_platform_mac_address(void)
{
	return is_valid_ether_addr(openipc_ethaddr) ? openipc_ethaddr : NULL;
}
