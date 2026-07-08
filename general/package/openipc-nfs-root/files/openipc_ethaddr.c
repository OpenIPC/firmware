// SPDX-License-Identifier: GPL-2.0-or-later
#include <linux/init.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/etherdevice.h>

// Default u-boot env ethaddr on openipc platforms
static const u8 default_mac[ETH_ALEN] = {0x00,0x00,0x23,0x34,0x45,0x66};
static u8 openipc_ethaddr[ETH_ALEN] __aligned(2);

static int openipc_hex_to_bin(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}

static bool openipc_mac_pton(const char *str, u8 *mac)
{
	int i;

	/* XX:XX:XX:XX:XX:XX */
	if (strlen(str) != 3 * ETH_ALEN - 1)
		return false;

	for (i = 0; i < ETH_ALEN; i++) {
		int hi = openipc_hex_to_bin(str[i * 3]);
		int lo = openipc_hex_to_bin(str[i * 3 + 1]);

		if (hi < 0 || lo < 0)
			return false;

		if (i != ETH_ALEN - 1 && str[i * 3 + 2] != ':')
			return false;

		mac[i] = (hi << 4) | lo;
	}

	return true;
}

static int __init openipc_early_ethaddr(char *str)
{
	u8 mac[ETH_ALEN] __aligned(2);

	if (!str)
		return 0;

	if (!openipc_mac_pton(str, mac)) {
		pr_warn("openipc_ethaddr: invalid ethaddr bootarg '%s'\n", str);
		return 0;
	}

	if (ether_addr_equal(mac, default_mac))
		pr_warn("openipc_ethaddr: ignoring default ethaddr '%s'\n",
			str);
	else
		ether_addr_copy(openipc_ethaddr, mac);

	return 0;
}
early_param("ethaddr", openipc_early_ethaddr);

unsigned char *arch_get_platform_mac_address(void)
{
	return is_valid_ether_addr(openipc_ethaddr) ? openipc_ethaddr : NULL;
}
