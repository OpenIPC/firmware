/*
 * rtl8xxxu_compat.h - compatibility shims for backporting mainline
 * rtl8xxxu (v6.2) to Linux 3.4.43.
 *
 * Linux 3.4.43 has mac80211/cfg80211 but predates several APIs that
 * mainline rtl8xxxu relies on. This header provides minimal shims so
 * the driver compiles and runs on the old stack.
 */
#ifndef _RTL8XXXU_COMPAT_H
#define _RTL8XXXU_COMPAT_H

#include <linux/version.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ieee80211.h>
#include <net/mac80211.h>
#include <linux/nl80211.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0)

/*
 * ieee80211_hw_set(hw, FLAG) was added in 3.14 as a macro that expands
 * to __set_bit(IEEE80211_HW_FLAG, &hw->flags). Re-implement it for
 * 3.4.43 (which uses the IEEE80211_HW_* enum + hw->flags bitmap).
 */
#define ieee80211_hw_set(hw, flag) \
	__set_bit(IEEE80211_HW_##flag, (unsigned long *)&(hw)->flags)

#endif /* < 3.14 */

/*
 * ether_addr_copy() / eth_broadcast_addr() were added in 3.13.
 * 3.4.43 lacks them; provide inline equivalents.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0)
static inline void ether_addr_copy(u8 *dst, const u8 *src)
{
	memcpy(dst, src, ETH_ALEN);
}
static inline void eth_broadcast_addr(u8 *addr)
{
	memset(addr, 0xff, ETH_ALEN);
}
#endif

/*
 * ieee80211_ave_rssi() was added later to fetch average station RSSI.
 * 3.4.43 lacks it; stub it out. rtl8xxxu uses it only for rate tuning,
 * returning 0 is safe for connectivity.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
static inline int ieee80211_ave_rssi(struct ieee80211_vif *vif)
{
	return 0;
}
#endif

/*
 * 3.4.43 uses enum ieee80211_band (IEEE80211_BAND_*). Mainline renamed
 * these to NL80211_BAND_*. Map the new names to the old constants.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
#define NL80211_BAND_2GHZ	IEEE80211_BAND_2GHZ
#define NL80211_BAND_5GHZ	IEEE80211_BAND_5GHZ
#define NL80211_BAND_60GHZ	IEEE80211_BAND_60GHZ
#define NL80211_BAND_NUM	IEEE80211_NUM_BANDS
/* IEEE80211_NUM_TIDS (16) added after 3.4 */
#define IEEE80211_NUM_TIDS	16
#endif

/*
 * 3.4.43 has no enum nl80211_chan_width (it uses nl80211_channel_type).
 * Define the width constants mainline rtl8xxxu switches on so the
 * case labels compile. The actual width is derived from
 * hw->conf.channel_type via rtl8xxxu_conf_width().
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0)
#define NL80211_CHAN_WIDTH_20_NOHT	0
#define NL80211_CHAN_WIDTH_20		1
#define NL80211_CHAN_WIDTH_40		2
#define NL80211_CHAN_WIDTH_80		3
#define NL80211_CHAN_WIDTH_80P80	4
#define NL80211_CHAN_WIDTH_160		5
#define NL80211_CHAN_WIDTH_5		6
#define NL80211_CHAN_WIDTH_10		7
#endif

/* fallthrough statement macro added in 5.x; provide a no-op for 3.4.43 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
#define fallthrough do {} while (0)
#endif

/*
 * offsetofend(TYPE, MEMBER) was added in 4.13. Provide it for 3.4.43.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,13,0)
#define offsetofend(TYPE, MEMBER) \
	(offsetof(TYPE, MEMBER) + sizeof(((TYPE *)0)->MEMBER))
#endif

/*
 * RX status API: 3.4.43 uses rx_status->flag bits (RX_FLAG_HT,
 * RX_FLAG_40MHZ, RX_FLAG_SHORT_GI, RX_FLAG_MACTIME_MPDU) instead of
 * the mainline rx_status->encoding / ->bw / ->enc_flags fields plus
 * RX_ENC_* / RATE_INFO_BW_* enums. Map the new names to the old flags.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
#define RX_FLAG_MACTIME_START	RX_FLAG_MACTIME_MPDU
/* RATE_INFO_BW_* used only as local integer sentinels in the driver */
#define RATE_INFO_BW_20	0
#define RATE_INFO_BW_40	1
#endif

/*
 * General kernel helpers added after 3.4 that mainline rtl8xxxu uses.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,18,0)
#define GENMASK(h, l) (((1UL << ((h) - (l) + 1)) - 1) << (l))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,3,0)
/* strscpy() added in 4.3; 3.4.43 has strlcpy() with the same arg order */
#define strscpy(dst, src, size) strlcpy(dst, src, size)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0)
static inline bool ether_addr_equal(const u8 *a, const u8 *b)
{
	return compare_ether_addr(a, b) == 0;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
/* list_first_entry_or_null() added in 4.0 */
#define list_first_entry_or_null(ptr, type, member)			\
	({ struct list_head *__h = (ptr);					\
	   list_empty(__h) ? NULL : list_first_entry(__h, type, member); })
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
/* IEEE80211_SEQ_TO_SN() not present in 3.4.43; IEEE80211_SCTL_SEQ is */
#define IEEE80211_SEQ_TO_SN(seq) (((seq) & IEEE80211_SCTL_SEQ) >> 4)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,16,0)
/* u8p_replace_bits() and friends added in 4.16 (bitfield helpers) */
static inline void u8p_replace_bits(u8 *addr, u8 value, u8 field)
{
	u8 shift = __builtin_ffs(field) - 1;
	*addr = (u8)((*addr & ~field) | ((value << shift) & field));
}
static inline void u16p_replace_bits(u16 *addr, u16 value, u16 field)
{
	u16 shift = __builtin_ffs(field) - 1;
	*addr = (u16)((*addr & ~field) | ((value << shift) & field));
}
static inline void u32p_replace_bits(u32 *addr, u32 value, u32 field)
{
	u32 shift = __builtin_ffs(field) - 1;
	*addr = (*addr & ~field) | ((value << shift) & field);
}
static inline u8 u8_get_bits(u8 val, u8 field)
{
	u8 shift = __builtin_ffs(field) - 1;
	return (u8)((val & field) >> shift);
}
static inline u16 u16_get_bits(u16 val, u16 field)
{
	u16 shift = __builtin_ffs(field) - 1;
	return (u16)((val & field) >> shift);
}
static inline u32 u32_get_bits(u32 val, u32 field)
{
	u32 shift = __builtin_ffs(field) - 1;
	return (val & field) >> shift;
}
/* FIELD_GET / FIELD_PREP from bitfield.h (added in 4.x) */
#define FIELD_GET(mask, val) (((val) & (mask)) >> (__builtin_ffs(mask) - 1))
#define FIELD_PREP(mask, val) (((val) << (__builtin_ffs(mask) - 1)) & (mask))
#endif

/*
 * IEEE80211_KEY_FLAG_SW_MGMT_TX was added later; 3.4.43 has the older
 * IEEE80211_KEY_FLAG_SW_MGMT. Map the new name to the old flag.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
#define IEEE80211_KEY_FLAG_SW_MGMT_TX	IEEE80211_KEY_FLAG_SW_MGMT
#endif

/*
 * chandef compat: 3.4.43 ieee80211_conf exposes `channel` (pointer to
 * struct ieee80211_channel) and `channel_type` (enum
 * nl80211_channel_type) instead of a `chandef` (cfg80211_chan_def).
 * Provide helpers that synthesise the chandef accessors the mainline
 * driver expects (chan, width, center_freq1) from the old fields.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0)
static inline int rtl8xxxu_conf_width(struct ieee80211_hw *hw)
{
	switch (hw->conf.channel_type) {
	case NL80211_CHAN_NO_HT:
		return NL80211_CHAN_WIDTH_20_NOHT;
	case NL80211_CHAN_HT20:
		return NL80211_CHAN_WIDTH_20;
	case NL80211_CHAN_HT40MINUS:
	case NL80211_CHAN_HT40PLUS:
		return NL80211_CHAN_WIDTH_40;
	default:
		return NL80211_CHAN_WIDTH_20;
	}
}

static inline int rtl8xxxu_conf_center_freq1(struct ieee80211_hw *hw)
{
	struct ieee80211_channel *c = hw->conf.channel;

	if (!c)
		return 0;
	switch (hw->conf.channel_type) {
	case NL80211_CHAN_HT40PLUS:
		return c->center_freq + 10;
	case NL80211_CHAN_HT40MINUS:
		return c->center_freq - 10;
	default:
		return c->center_freq;
	}
}
#endif

#endif /* _RTL8XXXU_COMPAT_H */
