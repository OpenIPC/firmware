/*
 * Based on m25p80.c, by Mike Lavender (mike@steroidmicros.com), with
 * influence from lart.c (Abraham Van Der Merwe) and mtd_dataflash.c
 *
 * Copyright (C) 2005, Intec Automation Inc.
 * Copyright (C) 2014, Freescale Semiconductor, Inc.
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/math64.h>
#include <linux/sizes.h>

#include <linux/mtd/mtd.h>
#include <linux/of_platform.h>
#include <linux/spi/flash.h>
#include <linux/mtd/spi-nor.h>

/* Define max times to check status register before we give up. */

/*
 * For everything but full-chip erase; probably could be much smaller, but kept
 * around for safety for now
 */
#define DEFAULT_READY_WAIT_JIFFIES		(40UL * HZ)

/*
 * For full-chip erase, calibrated to a 2MB flash (M25P16); should be scaled up
 * for larger flash
 */
#define CHIP_ERASE_2MB_READY_WAIT_JIFFIES	(40UL * HZ)

#define SPI_NOR_MAX_ID_LEN	6
#define SPI_NOR_MAX_ADDR_WIDTH	4

struct flash_info {
	char		*name;

	/*
	 * This array stores the ID bytes.
	 * The first three bytes are the JEDIC ID.
	 * JEDEC ID zero means "no ID" (mostly older chips).
	 */
	u8		id[SPI_NOR_MAX_ID_LEN];
	u8		id_len;

	/* The size listed here is what works with SPINOR_OP_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16		n_sectors;

	u16		page_size;
	u16		addr_width;

	u16		flags;
#define SECT_4K			BIT(0)	/* SPINOR_OP_BE_4K works uniformly */
#define SPI_NOR_NO_ERASE	BIT(1)	/* No erase command needed */
#define SST_WRITE		BIT(2)	/* use SST byte programming */
#define SPI_NOR_NO_FR		BIT(3)	/* Can't do fastread */
#define SECT_4K_PMC		BIT(4)	/* SPINOR_OP_BE_4K_PMC works uniformly */
#define SPI_NOR_DUAL_READ	BIT(5)	/* Flash supports Dual Read */
#define SPI_NOR_QUAD_READ	BIT(6)	/* Flash supports Quad Read */
#define USE_FSR			BIT(7)	/* use flag status register */
#define SPI_NOR_HAS_LOCK	BIT(8)	/* Flash supports lock/unlock via SR */
#define SPI_NOR_HAS_TB		BIT(9)	/*
					 * Flash SR has Top/Bottom (TB) protect
					 * bit. Must be used with
					 * SPI_NOR_HAS_LOCK.
					 */
#define SPI_NOR_4B_OPCODES	BIT(10)	/*
					 * Use dedicated 4byte address op codes
					 * to support memory size above 128Mib.
					 */

	const struct spi_nor_basic_flash_parameter *params;
	u32 clkrate;
};

#define JEDEC_MFR(info)	((info)->id[0])

static const struct flash_info *spi_nor_match_id(const char *name);

/*
 * Read the status register, returning its value in the location
 * Return the status register value.
 * Returns negative if error occurred.
 */
static int read_sr(struct spi_nor *nor)
{
	int ret;
	u8 val;

	ret = nor->read_reg(nor, SPINOR_OP_RDSR, &val, 1);
	if (ret < 0) {
		pr_err("error %d reading SR\n", (int) ret);
		return ret;
	}

	return val;
}

/*
 * Read the flag status register, returning its value in the location
 * Return the status register value.
 * Returns negative if error occurred.
 */
static int read_fsr(struct spi_nor *nor)
{
	int ret;
	u8 val;

	ret = nor->read_reg(nor, SPINOR_OP_RDFSR, &val, 1);
	if (ret < 0) {
		pr_err("error %d reading FSR\n", ret);
		return ret;
	}

	return val;
}

/*
 * Read configuration register, returning its value in the
 * location. Return the configuration register value.
 * Returns negative if error occured.
 */
static int read_cr(struct spi_nor *nor)
{
	int ret;
	u8 val;

	ret = nor->read_reg(nor, SPINOR_OP_RDCR, &val, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error %d reading CR\n", ret);
		return ret;
	}

	return val;
}

/*
 * Write status register 1 byte
 * Returns negative if error occurred.
 */
static inline int write_sr(struct spi_nor *nor, u8 val)
{
	nor->cmd_buf[0] = val;
	return nor->write_reg(nor, SPINOR_OP_WRSR, nor->cmd_buf, 1);
}


/*
 * Write status register-2 1 byte
 * Returns negative if error occurred.
 */
static inline int write_sr2(struct spi_nor *nor, u8 val)
{
	nor->cmd_buf[0] = val;
	return nor->write_reg(nor, SPINOR_OP_WRSR2, nor->cmd_buf, 1);
}

/*
 * Set write enable latch with Write Enable command.
 * Returns negative if error occurred.
 */
static inline int write_enable(struct spi_nor *nor)
{
	return nor->write_reg(nor, SPINOR_OP_WREN, NULL, 0);
}

/*
 * Send write disble instruction to the chip.
 */
static inline int write_disable(struct spi_nor *nor)
{
	return nor->write_reg(nor, SPINOR_OP_WRDI, NULL, 0);
}

static inline struct spi_nor *mtd_to_spi_nor(struct mtd_info *mtd)
{
	return mtd->priv;
}

struct spi_nor_address_entry {
	u8	src_opcode;
	u8	dst_opcode;
};

static u8 spi_nor_convert_opcode(u8 opcode,
				 const struct spi_nor_address_entry *entries,
				 size_t num_entries)
{
	unsigned int min, max;

	min = 0;
	max = num_entries - 1;
	while (min <= max) {
		int mid = (min + max) >> 1;
		const struct spi_nor_address_entry *entry = &entries[mid];

		if (opcode == entry->src_opcode)
			return entry->dst_opcode;

		if (opcode < entry->src_opcode)
			max = mid - 1;
		else
			min = mid + 1;
	}

	/* No conversion found */
	return opcode;
}

static u8 spi_nor_3to4_opcode(u8 opcode)
{
	/* MUST be sorted by 3byte opcode */
#define ENTRY_3TO4(_opcode)	{ _opcode, _opcode##_4B }
	static const struct spi_nor_address_entry spi_nor_3to4_table[] = {
		ENTRY_3TO4(SPINOR_OP_PP),		/* 0x02 */
		ENTRY_3TO4(SPINOR_OP_READ),		/* 0x03 */
		ENTRY_3TO4(SPINOR_OP_READ_FAST),	/* 0x0b */
		ENTRY_3TO4(SPINOR_OP_BE_4K),		/* 0x20 */
		ENTRY_3TO4(SPINOR_OP_PP_1_1_4),		/* 0x32 */
		ENTRY_3TO4(SPINOR_OP_PP_1_4_4),		/* 0x38 */
		ENTRY_3TO4(SPINOR_OP_READ_1_1_2),	/* 0x3b */
		ENTRY_3TO4(SPINOR_OP_BE_32K),		/* 0x52 */
		ENTRY_3TO4(SPINOR_OP_READ_1_1_4),	/* 0x6b */
		ENTRY_3TO4(SPINOR_OP_READ_1_2_2),	/* 0xbb */
		ENTRY_3TO4(SPINOR_OP_SE),		/* 0xd8 */
		ENTRY_3TO4(SPINOR_OP_READ_1_4_4),	/* 0xeb */
	};
#undef ENTRY_3TO4

	return spi_nor_convert_opcode(opcode, spi_nor_3to4_table,
				      ARRAY_SIZE(spi_nor_3to4_table));
}

static void spi_nor_set_4byte_opcodes(struct spi_nor *nor,
				      const struct flash_info *info)
{
	/* Do some manufacturer fixups first */
	switch (JEDEC_MFR(info)) {
	case SNOR_MFR_SPANSION:
		/* No small sector erase for 4-byte command set */
		nor->erase_opcode = SPINOR_OP_SE;
		nor->mtd.erasesize = info->sector_size;
		break;

	default:
		break;
	}

	nor->read_opcode	= spi_nor_3to4_opcode(nor->read_opcode);
	nor->program_opcode	= spi_nor_3to4_opcode(nor->program_opcode);
	nor->erase_opcode	= spi_nor_3to4_opcode(nor->erase_opcode);
}

/* Enable/disable 4-byte addressing mode. */
static inline int set_4byte(struct spi_nor *nor, const struct flash_info *info,
			    u32 enable)
{
	int status;
	bool need_wren = false;
	u8 cmd;

	switch (JEDEC_MFR(info)) {
	case SNOR_MFR_MICRON:
		/* Some Micron need WREN command; all will accept it */
		need_wren = true;
	case SNOR_MFR_MACRONIX:
		if (need_wren)
			write_enable(nor);

		cmd = enable ? SPINOR_OP_EN4B : SPINOR_OP_EX4B;
		status = nor->write_reg(nor, cmd, NULL, 0);

		if (need_wren)
			write_disable(nor);

		return status;
	case SNOR_MFR_WINBOND:
		if (enable)
			return nor->write_reg(nor, SPINOR_OP_EN4B, NULL, 0);
		else {
			/* w25q256fvfg must send reset to disable 4 byte mode */
			nor->write_reg(nor, SPINOR_ENABLE_RESET, NULL, 0);
			nor->write_reg(nor, SPINOR_OP_RESET, NULL, 0);
			udelay(30);
		}
		return 0;
	default:
		/* Spansion style */
		nor->cmd_buf[0] = enable << 7;
		return nor->write_reg(nor, SPINOR_OP_BRWR, nor->cmd_buf, 1);
	}
}
static inline int spi_nor_sr_ready(struct spi_nor *nor)
{
	int sr = read_sr(nor);

	if (sr < 0)
		return sr;
	else
		return !((unsigned int)sr & SR_WIP);
}

static inline unsigned int spi_nor_fsr_ready(struct spi_nor *nor)
{
	int fsr = read_fsr(nor);
	unsigned int ufsr = (unsigned int)fsr;

	if (fsr < 0)
		return fsr;
	else
		return ufsr & FSR_READY;
}

static int spi_nor_ready(struct spi_nor *nor)
{
	int sr, fsr;

	sr = spi_nor_sr_ready(nor);
	if (sr < 0)
		return sr;
	fsr = nor->flags & SNOR_F_USE_FSR ? spi_nor_fsr_ready(nor) : 1;
	if (fsr < 0)
		return fsr;
	return sr && fsr;
}

/*
 * Service routine to read status register until ready, or timeout occurs.
 * Returns non-zero if error.
 */
static int spi_nor_wait_till_ready_with_timeout(struct spi_nor *nor,
						unsigned long timeout_jiffies)
{
	unsigned long deadline;
	int timeout = 0, ret;

	deadline = jiffies + timeout_jiffies;

	while (!timeout) {
		if (time_after_eq(jiffies, deadline))
			timeout = 1;

		ret = spi_nor_ready(nor);
		if (ret < 0)
			return ret;
		if (ret)
			return 0;

		cond_resched();
	}

	dev_err(nor->dev, "flash operation timed out\n");

	return -ETIMEDOUT;
}

static int spi_nor_wait_till_ready(struct spi_nor *nor)
{
	return spi_nor_wait_till_ready_with_timeout(nor,
						    DEFAULT_READY_WAIT_JIFFIES);
}
static int issi_spi_nor_wait_till_ready(struct spi_nor *nor)
{
	unsigned long deadline;
	int timeout = 0;
	int ret;

	deadline = jiffies + DEFAULT_READY_WAIT_JIFFIES;

	while (!timeout) {
		if (time_after_eq(jiffies, deadline))
			timeout = 1;

		ret = spi_nor_sr_ready(nor);
		if (ret < 0)
			return ret;
		if (ret)
			return 0;

		cond_resched();
	}

	dev_err(nor->dev, "flash operation timed out\n");

	return -ETIMEDOUT;
}
/*
 * Erase the whole flash memory
 *
 * Returns 0 if successful, non-zero otherwise.
 */
static int erase_chip(struct spi_nor *nor)
{
	dev_dbg(nor->dev, " %lldKiB\n", (long long)(nor->mtd.size >> 10));

	return nor->write_reg(nor, SPINOR_OP_CHIP_ERASE, NULL, 0);
}

static int spi_nor_lock_and_prep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	int ret = 0;

	mutex_lock(&nor->lock);

	if (nor->prepare) {
		ret = nor->prepare(nor, ops);
		if (ret) {
			dev_err(nor->dev, "failed in the preparation.\n");
			mutex_unlock(&nor->lock);
			return ret;
		}
	}
	return ret;
}

static void spi_nor_unlock_and_unprep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	if (nor->unprepare)
		nor->unprepare(nor, ops);
	mutex_unlock(&nor->lock);
}

/*
 * Initiate the erasure of a single sector
 */
static int spi_nor_erase_sector(struct spi_nor *nor, u32 addr)
{
	u8 buf[SPI_NOR_MAX_ADDR_WIDTH];
	int i;

	if (nor->erase)
		return nor->erase(nor, addr);

	/*
	 * Default implementation, if driver doesn't have a specialized HW
	 * control
	 */
	for (i = nor->addr_width - 1; i >= 0; i--) {
		buf[i] = addr & 0xff;
		addr >>= 8;
	}

	return nor->write_reg(nor, nor->erase_opcode, buf, nor->addr_width);
}

/*
 * Erase an address range on the nor chip.  The address range may extend
 * one or more erase sectors.  Return an error is there is a problem erasing.
 */
static int spi_nor_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct spi_nor *nor = mtd_to_spi_nor(mtd);
	u32 addr, len;
	uint32_t rem;
	int ret;

	dev_dbg(nor->dev, "at 0x%llx, len %lld\n", (long long)instr->addr,
			(long long)instr->len);

	div_u64_rem(instr->len, mtd->erasesize, &rem);
	if (rem)
		return -EINVAL;

	addr = instr->addr;
	len = instr->len;

	ret = spi_nor_lock_and_prep(nor, SPI_NOR_OPS_ERASE);
	if (ret)
		return ret;

#ifdef CONFIG_GOKE_SPI_BLOCK_PROTECT
	if ((nor->level) && (addr < nor->end_addr)) {
		dev_err(nor->dev, "Error: The erase area was locked\n");
		return -EINVAL;
	}
#endif
	/* whole-chip erase? */
	if (len == mtd->size) {
		unsigned long timeout;

		write_enable(nor);

		if (erase_chip(nor)) {
			ret = -EIO;
			goto erase_err;
		}

		/*
		 * Scale the timeout linearly with the size of the flash, with
		 * a minimum calibrated to an old 2MB flash. We could try to
		 * pull these from CFI/SFDP, but these values should be good
		 * enough for now.
		 */
		timeout = max(CHIP_ERASE_2MB_READY_WAIT_JIFFIES,
			      CHIP_ERASE_2MB_READY_WAIT_JIFFIES *
			      (unsigned long)(mtd->size / SZ_2M));
		ret = spi_nor_wait_till_ready_with_timeout(nor, timeout);
		if (ret)
			goto erase_err;

	/* REVISIT in some cases we could speed up erasing large regions
	 * by using SPINOR_OP_SE instead of SPINOR_OP_BE_4K.  We may have set up
	 * to use "small sector erase", but that's not always optimal.
	 */

	/* "sector"-at-a-time erase */
	} else {
		while (len) {
			write_enable(nor);

			ret = spi_nor_erase_sector(nor, addr);
			if (ret)
				goto erase_err;

			addr += mtd->erasesize;
			len -= mtd->erasesize;

			ret = spi_nor_wait_till_ready(nor);
			if (ret)
				goto erase_err;
		}
	}

	write_disable(nor);

erase_err:
	spi_nor_unlock_and_unprep(nor, SPI_NOR_OPS_ERASE);

	instr->state = ret ? MTD_ERASE_FAILED : MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return ret;
}

static void stm_get_locked_range(struct spi_nor *nor, u8 sr, loff_t *ofs,
				 uint64_t *len)
{
	struct mtd_info *mtd = &nor->mtd;
	u8 mask = SR_BP2 | SR_BP1 | SR_BP0;
	int shift = ffs(mask) - 1;
	int pow;

	if (!(sr & mask)) {
		/* No protection */
		*ofs = 0;
		*len = 0;
	} else {
		pow = ((sr & mask) ^ mask) >> shift;
		*len = mtd->size >> pow;
		if (nor->flags & SNOR_F_HAS_SR_TB && sr & SR_TB)
			*ofs = 0;
		else
			*ofs = mtd->size - *len;
	}
}

/*
 * Return 1 if the entire region is locked (if @locked is true) or unlocked (if
 * @locked is false); 0 otherwise
 */
static int stm_check_lock_status_sr(struct spi_nor *nor, loff_t ofs, uint64_t len,
				    u8 sr, bool locked)
{
	loff_t lock_offs;
	uint64_t lock_len;

	if (!len)
		return 1;

	stm_get_locked_range(nor, sr, &lock_offs, &lock_len);

	if (locked)
		/* Requested range is a sub-range of locked range */
		return (ofs + len <= lock_offs + lock_len) && (ofs >= lock_offs);
	else
		/* Requested range does not overlap with locked range */
		return (ofs >= lock_offs + lock_len) || (ofs + len <= lock_offs);
}

static int stm_is_locked_sr(struct spi_nor *nor, loff_t ofs, uint64_t len,
			    u8 sr)
{
	return stm_check_lock_status_sr(nor, ofs, len, sr, true);
}

static int stm_is_unlocked_sr(struct spi_nor *nor, loff_t ofs, uint64_t len,
			      u8 sr)
{
	return stm_check_lock_status_sr(nor, ofs, len, sr, false);
}

/*
 * Lock a region of the flash. Compatible with ST Micro and similar flash.
 * Supports the block protection bits BP{0,1,2} in the status register
 * (SR). Does not support these features found in newer SR bitfields:
 *   - SEC: sector/block protect - only handle SEC=0 (block protect)
 *   - CMP: complement protect - only support CMP=0 (range is not complemented)
 *
 * Support for the following is provided conditionally for some flash:
 *   - TB: top/bottom protect
 *
 * Sample table portion for 8MB flash (Winbond w25q64fw):
 *
 *   SEC  |  TB   |  BP2  |  BP1  |  BP0  |  Prot Length  | Protected Portion
 *  --------------------------------------------------------------------------
 *    X   |   X   |   0   |   0   |   0   |  NONE         | NONE
 *    0   |   0   |   0   |   0   |   1   |  128 KB       | Upper 1/64
 *    0   |   0   |   0   |   1   |   0   |  256 KB       | Upper 1/32
 *    0   |   0   |   0   |   1   |   1   |  512 KB       | Upper 1/16
 *    0   |   0   |   1   |   0   |   0   |  1 MB         | Upper 1/8
 *    0   |   0   |   1   |   0   |   1   |  2 MB         | Upper 1/4
 *    0   |   0   |   1   |   1   |   0   |  4 MB         | Upper 1/2
 *    X   |   X   |   1   |   1   |   1   |  8 MB         | ALL
 *  ------|-------|-------|-------|-------|---------------|-------------------
 *    0   |   1   |   0   |   0   |   1   |  128 KB       | Lower 1/64
 *    0   |   1   |   0   |   1   |   0   |  256 KB       | Lower 1/32
 *    0   |   1   |   0   |   1   |   1   |  512 KB       | Lower 1/16
 *    0   |   1   |   1   |   0   |   0   |  1 MB         | Lower 1/8
 *    0   |   1   |   1   |   0   |   1   |  2 MB         | Lower 1/4
 *    0   |   1   |   1   |   1   |   0   |  4 MB         | Lower 1/2
 *
 * Returns negative on errors, 0 on success.
 */
static int stm_lock(struct spi_nor *nor, loff_t ofs, uint64_t len)
{
	struct mtd_info *mtd = &nor->mtd;
	int status_old, status_new;
	u8 mask = SR_BP2 | SR_BP1 | SR_BP0;
	u8 shift = ffs(mask) - 1, pow, val;
	loff_t lock_len;
	bool can_be_top = true, can_be_bottom = nor->flags & SNOR_F_HAS_SR_TB;
	bool use_top;
	int ret;

	status_old = read_sr(nor);
	if (status_old < 0)
		return status_old;

	/* If nothing in our range is unlocked, we don't need to do anything */
	if (stm_is_locked_sr(nor, ofs, len, status_old))
		return 0;

	/* If anything below us is unlocked, we can't use 'bottom' protection */
	if (!stm_is_locked_sr(nor, 0, ofs, status_old))
		can_be_bottom = false;

	/* If anything above us is unlocked, we can't use 'top' protection */
	if (!stm_is_locked_sr(nor, ofs + len, mtd->size - (ofs + len),
				status_old))
		can_be_top = false;

	if (!can_be_bottom && !can_be_top)
		return -EINVAL;

	/* Prefer top, if both are valid */
	use_top = can_be_top;

	/* lock_len: length of region that should end up locked */
	if (use_top)
		lock_len = mtd->size - ofs;
	else
		lock_len = ofs + len;

	/*
	 * Need smallest pow such that:
	 *
	 *   1 / (2^pow) <= (len / size)
	 *
	 * so (assuming power-of-2 size) we do:
	 *
	 *   pow = ceil(log2(size / len)) = log2(size) - floor(log2(len))
	 */
	pow = ilog2(mtd->size) - ilog2(lock_len);
	val = mask - (pow << shift);
	if (val & ~mask)
		return -EINVAL;
	/* Don't "lock" with no region! */
	if (!(val & mask))
		return -EINVAL;

	status_new = (status_old & ~mask & ~SR_TB) | val;

	/* Disallow further writes if WP pin is asserted */
	status_new |= SR_SRWD;

	if (!use_top)
		status_new |= SR_TB;

	/* Don't bother if they're the same */
	if (status_new == status_old)
		return 0;

	/* Only modify protection if it will not unlock other areas */
	if ((status_new & mask) < (status_old & mask))
		return -EINVAL;

	write_enable(nor);
	ret = write_sr(nor, status_new);
	if (ret)
		return ret;
	return spi_nor_wait_till_ready(nor);
}

/*
 * Unlock a region of the flash. See stm_lock() for more info
 *
 * Returns negative on errors, 0 on success.
 */
static int stm_unlock(struct spi_nor *nor, loff_t ofs, uint64_t len)
{
	struct mtd_info *mtd = &nor->mtd;
	int status_old, status_new;
	u8 mask = SR_BP2 | SR_BP1 | SR_BP0;
	u8 shift = ffs(mask) - 1, pow, val;
	loff_t lock_len;
	bool can_be_top = true, can_be_bottom = nor->flags & SNOR_F_HAS_SR_TB;
	bool use_top;
	int ret;

	status_old = read_sr(nor);
	if (status_old < 0)
		return status_old;

	/* If nothing in our range is locked, we don't need to do anything */
	if (stm_is_unlocked_sr(nor, ofs, len, status_old))
		return 0;

	/* If anything below us is locked, we can't use 'top' protection */
	if (!stm_is_unlocked_sr(nor, 0, ofs, status_old))
		can_be_top = false;

	/* If anything above us is locked, we can't use 'bottom' protection */
	if (!stm_is_unlocked_sr(nor, ofs + len, mtd->size - (ofs + len),
				status_old))
		can_be_bottom = false;

	if (!can_be_bottom && !can_be_top)
		return -EINVAL;

	/* Prefer top, if both are valid */
	use_top = can_be_top;

	/* lock_len: length of region that should remain locked */
	if (use_top)
		lock_len = mtd->size - (ofs + len);
	else
		lock_len = ofs;

	/*
	 * Need largest pow such that:
	 *
	 *   1 / (2^pow) >= (len / size)
	 *
	 * so (assuming power-of-2 size) we do:
	 *
	 *   pow = floor(log2(size / len)) = log2(size) - ceil(log2(len))
	 */
	pow = ilog2(mtd->size) - order_base_2(lock_len);
	if (lock_len == 0) {
		val = 0; /* fully unlocked */
	} else {
		val = mask - (pow << shift);
		/* Some power-of-two sizes are not supported */
		if (val & ~mask)
			return -EINVAL;
	}

	status_new = (status_old & ~mask & ~SR_TB) | val;

	/* Don't protect status register if we're fully unlocked */
	if (lock_len == 0)
		status_new &= ~SR_SRWD;

	if (!use_top)
		status_new |= SR_TB;

	/* Don't bother if they're the same */
	if (status_new == status_old)
		return 0;

	/* Only modify protection if it will not lock other areas */
	if ((status_new & mask) > (status_old & mask))
		return -EINVAL;

	write_enable(nor);
	ret = write_sr(nor, status_new);
	if (ret)
		return ret;
	return spi_nor_wait_till_ready(nor);
}

/*
 * Check if a region of the flash is (completely) locked. See stm_lock() for
 * more info.
 *
 * Returns 1 if entire region is locked, 0 if any portion is unlocked, and
 * negative on errors.
 */
static int stm_is_locked(struct spi_nor *nor, loff_t ofs, uint64_t len)
{
	int status;

	status = read_sr(nor);
	if (status < 0)
		return status;

	return stm_is_locked_sr(nor, ofs, len, status);
}

static int spi_nor_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct spi_nor *nor = mtd_to_spi_nor(mtd);
	int ret;

	ret = spi_nor_lock_and_prep(nor, SPI_NOR_OPS_LOCK);
	if (ret)
		return ret;

	ret = nor->flash_lock(nor, ofs, len);

	spi_nor_unlock_and_unprep(nor, SPI_NOR_OPS_UNLOCK);
	return ret;
}

static int spi_nor_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct spi_nor *nor = mtd_to_spi_nor(mtd);
	int ret;

	ret = spi_nor_lock_and_prep(nor, SPI_NOR_OPS_UNLOCK);
	if (ret)
		return ret;

	ret = nor->flash_unlock(nor, ofs, len);

	spi_nor_unlock_and_unprep(nor, SPI_NOR_OPS_LOCK);
	return ret;
}

static int spi_nor_is_locked(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct spi_nor *nor = mtd_to_spi_nor(mtd);
	int ret;

	ret = spi_nor_lock_and_prep(nor, SPI_NOR_OPS_UNLOCK);
	if (ret)
		return ret;

	ret = nor->flash_is_locked(nor, ofs, len);

	spi_nor_unlock_and_unprep(nor, SPI_NOR_OPS_LOCK);
	return ret;
}

#define SNOR_RD_MODES			\
	(SNOR_MODE_SLOW |			\
	 SNOR_MODE_1_1_1 |			\
	 SNOR_MODE_1_1_2 |			\
	 SNOR_MODE_1_2_2 |			\
	 SNOR_MODE_1_1_4 |			\
	 SNOR_MODE_1_4_4)

#define SNOR_WR_MODES			\
	(SNOR_MODE_1_1_1 |			\
	 SNOR_MODE_1_1_4)

static int spansion_quad_enable(struct spi_nor *nor);
static int macronix_quad_enable(struct spi_nor *nor);
static int gd_quad_enable(struct spi_nor *nor);
static int xtx_quad_enable(struct spi_nor *nor);
static int issi_quad_enable(struct spi_nor *nor);
static int puya_quad_enable(struct spi_nor *nor);
static int puya_quad_enable(struct spi_nor *nor);

#define SNOR_EON_RD_MODES			\
	(SNOR_MODE_SLOW |			\
	 SNOR_MODE_1_1_1 |			\
	 SNOR_MODE_1_1_2 |			\
	 SNOR_MODE_1_2_2)

#define SNOR_EON_WR_MODES			\
	(SNOR_MODE_1_1_1)

static const struct spi_nor_basic_flash_parameter eon_params = {
	.rd_modes		= SNOR_EON_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(8, 0, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_EON_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

};

static const struct spi_nor_basic_flash_parameter esmt_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(8, 0, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = macronix_quad_enable,

};

#define SNOR_PARAGON_WR_MODES			\
	(SNOR_MODE_1_1_1)

static const struct spi_nor_basic_flash_parameter paragon_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(8, 0, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_PARAGON_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = spansion_quad_enable,

};

static const struct spi_nor_basic_flash_parameter gd_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(8, 0, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = gd_quad_enable,

};

static const struct spi_nor_basic_flash_parameter winbond_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(8, 0, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = spansion_quad_enable,

};

static const struct spi_nor_basic_flash_parameter spansion_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = spansion_quad_enable,

};

static const struct spi_nor_basic_flash_parameter issi_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = issi_quad_enable,

};

#define SNOR_MXIC_WR_MODES			\
	(SNOR_MODE_1_1_1 |			\
	 SNOR_MODE_1_4_4)

static const struct spi_nor_basic_flash_parameter mxic_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(8, 16, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_MXIC_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_4_4]	= SPINOR_OP_PP_1_4_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = macronix_quad_enable,

};

static const struct spi_nor_basic_flash_parameter xmc_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(0, 24, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

};

static const struct spi_nor_basic_flash_parameter micron_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(8, 8, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(1, 7, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(0, 40, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

};

static const struct spi_nor_basic_flash_parameter micron_4k_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(1, 7, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(1, 7, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(1, 9, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),
	.erase_types[1]		= SNOR_OP_ERASE_4K(SPINOR_OP_BE_4K),
};

static const struct spi_nor_basic_flash_parameter xtx_params = {
	.rd_modes		= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]	= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]	= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]	= SNOR_OP_READ(0, 24, SPINOR_OP_READ_1_4_4),

	.wr_modes		= SNOR_MODE_1_1_1,
	.page_programs[SNOR_MIDX_1_1_1]	= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]	= SPINOR_OP_PP_1_1_4,

	.erase_types[0]		= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = xtx_quad_enable,

};

static const struct spi_nor_basic_flash_parameter puya_params = {
	.rd_modes= SNOR_RD_MODES,
	.reads[SNOR_MIDX_SLOW]= SNOR_OP_READ(0, 0, SPINOR_OP_READ),
	.reads[SNOR_MIDX_1_1_1]= SNOR_OP_READ(0, 8, SPINOR_OP_READ_FAST),
	.reads[SNOR_MIDX_1_1_2]= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_2),
	.reads[SNOR_MIDX_1_2_2]= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_2_2),
	.reads[SNOR_MIDX_1_1_4]= SNOR_OP_READ(0, 8, SPINOR_OP_READ_1_1_4),
	.reads[SNOR_MIDX_1_4_4]= SNOR_OP_READ(0, 24, SPINOR_OP_READ_1_4_4),

	.wr_modes= SNOR_WR_MODES,
	.page_programs[SNOR_MIDX_1_1_1]= SPINOR_OP_PP,
	.page_programs[SNOR_MIDX_1_1_4]= SPINOR_OP_PP_1_1_4,

	.erase_types[0]= SNOR_OP_ERASE_64K(SPINOR_OP_SE),

	.enable_quad_io         = puya_quad_enable,
};

#define PARAMS(_name) .params = &_name##_params

/* Used when the "_ext_id" is two bytes at most */
#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
		.id = {							\
			((_jedec_id) >> 16) & 0xff,			\
			((_jedec_id) >> 8) & 0xff,			\
			(_jedec_id) & 0xff,				\
			((_ext_id) >> 8) & 0xff,			\
			(_ext_id) & 0xff,				\
			},						\
		.id_len = (!(_jedec_id) ? 0 : (3 + ((_ext_id) ? 2 : 0))),	\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags)

#define INFO6(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
		.id = {							\
			((_jedec_id) >> 16) & 0xff,			\
			((_jedec_id) >> 8) & 0xff,			\
			(_jedec_id) & 0xff,				\
			((_ext_id) >> 16) & 0xff,			\
			((_ext_id) >> 8) & 0xff,			\
			(_ext_id) & 0xff,				\
			},						\
		.id_len = 6,						\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),

#define CAT25_INFO(_sector_size, _n_sectors, _page_size, _addr_width, _flags)	\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = (_page_size),				\
		.addr_width = (_addr_width),				\
		.flags = (_flags),

/* Different from spi-max-frequency in DTS, the clk here stands for the clock
 * rate on SPI interface, it is half of the FMC CRG configuration */
#define CLK_MHZ_2X(clk)  .clkrate = (clk * 2000000),

/* NOTE: double check command sets and memory organization when you add
 * more nor chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 *
 * All newly added entries should describe *hardware* and should use SECT_4K
 * (or SECT_4K_PMC) if hardware supports erasing 4 KiB sectors. For usage
 * scenarios excluding small sectors there is config option that can be
 * disabled: CONFIG_MTD_SPI_NOR_USE_4K_SECTORS.
 * For historical (and compatibility) reasons (before we got above config) some
 * old entries may be missing 4K flag.
 */
#define SPI_NOR_IDS_VER     "1.2"

/******* SPI Nor ID Table ************************************************************************
 * Version   Manufacturer    	Chip Name    		Chipsize	Block	Vol  	Operation
 *		Macronix/MXIC	MX25V1635F              2M	        64K     3V3
 * 1.0		Macronix/MXIC	MX25L1606E		2M		64K	3V3
 *		Macronix/MXIC	MX25L6436F		8M		64K	3V3
 *		Macronix/MXIC	MX25R6435F		8M		64K	1V8/3V3 Add 14chips
 *		Macronix/MXIC	MX25U6435F		8M		64K	1V8
 *		Macronix/MXIC	MX25U12835F		16M		64K     1V8
 *		Macronix/MXIC	MX25F128XXX		16M		64K     3V3
 *		Macronix/MXIC	MX25U25635F/45G		32M		64K	1V8 	25645G-DTR
 *		Macronix/MXIC	MX25L(256/257)		32M     	64K     3V3	25645G-DTR
 *		Macronix/MXIC	MX25U51245G		64M		64K	1V8 	51245G-DTR
 *		Macronix/MXIC   MX25L51245G		64M		64K	3V3
 *		Macronix/MXIC	MX66U1G45GM		128M		64K	1V8
 *		Spansion	S25FL129P1		16M     	64K	3V3
 *		Spansion	S25FL256S		32M     	64K	3V3
 *		Micron		N25Q064A		8M      	64K     3V3
 *		Micron		N25QL064A		8M      	64K     3V3
 *		Micron		N25Q128A11/MT25QU128AB  16M     	64K     1V8
 *		Micron		N25QL128A		16M     	64K     3V3
 *		Micron		MT25QU256A		32M     	64K     1V8
 *		Micron		MT25QL256A		32M     	64K     3V3
 *		Winbond		W25Q16(B/C)V/S25FL016K	2M		64K     3V3
 *		Winbond		W25Q32(B/F)V		4M		64K     3V3
 *		Winbond		W25Q32FW		4M		64K     1V8
 *		Winbond		W25Q64FW		8M		64K     1V8
 *		Winbond		W25Q64FV(SPI)/W25Q64JV_IQ 8M		64K     3V3
 *		Winbond		W25Q128FW		16M     	64K     1V8
 *		Winbond		W25Q128(B/F)V		16M     	64K     3V3
 *		Winbond		W25Q128JV_IM		16M     	64K     3V3  	DTR
 *		Winbond     	W25Q256JWEIQ        	32M		64K     1V8
 *		Winbond         W25Q256JWFIM        	32M		64K     1V8
 *		ESMT/CFEON	EN25Q32B		4M      	64K     3V3
 *		ESMT/CFEON	EN25Q64			8M      	64K     3V3
 *		ESMT/CFEON	EN25Q128		16M     	64K     3V3
 *		ESMT/CFEON	F25L64QA		8M      	64K     3V3
 *		GD		GD25Q16C	        2M		64K     3V3
 *		GD		GD25Q64			8M      	64K     3V3
 *		GD		GD25LQ128		16M     	64K     1V8
 *		GD		GD25Q128		16M     	64K     3V3
 *		GD		GD25Q256		32M     	64K     3V3
 *		GD		GD25LQ64C		8M      	64K     1V8
 *		GD		GD25Q32			4M      	64K     3V3
 *		Paragon		PN25F16S		2M		64K     3V3
 *		Paragon		PN25F32S		4M      	64K     3V3
 * 1.1		ESMT/CFEON	EN25QH64A		8M      	64K     3V3
 * 1.2		XMC		XM25QH64AHIG		8M      	64K     3V3
 * 		XMC		XM25QH128A		16M		64K	3V3
 * 		XMC		XM25QH128B		16M		64K	3V3
 *		Puya		P25Q128H-SUH-IT		16M		64K	3V3
 *		FM		FM25Q64-SOB-T-G		8M		64K	3V3
 *		FM		FM25Q128-SOB-T-G	16M		64K	3V3
 *		HUAHONG		H25S64			8M		64K	3V3
 *		HUAHONG		H25S128			16M		64K	3V3
 ********************************************************************************************/
static const struct flash_info spi_nor_ids[] = {
	/* Atmel -- some are (confusingly) marketed as "DataFlash" */
	{ "at25fs010",  INFO(0x1f6601, 0, 32 * 1024,   4, SECT_4K) },
	{ "at25fs040",  INFO(0x1f6604, 0, 64 * 1024,   8, SECT_4K) },

	{ "at25df041a", INFO(0x1f4401, 0, 64 * 1024,   8, SECT_4K) },
	{ "at25df321a", INFO(0x1f4701, 0, 64 * 1024,  64, SECT_4K) },
	{ "at25df641",  INFO(0x1f4800, 0, 64 * 1024, 128, SECT_4K) },

	{ "at26f004",   INFO(0x1f0400, 0, 64 * 1024,  8, SECT_4K) },
	{ "at26df081a", INFO(0x1f4501, 0, 64 * 1024, 16, SECT_4K) },
	{ "at26df161a", INFO(0x1f4601, 0, 64 * 1024, 32, SECT_4K) },
	{ "at26df321",  INFO(0x1f4700, 0, 64 * 1024, 64, SECT_4K) },

	{ "at45db081d", INFO(0x1f2500, 0, 64 * 1024, 16, SECT_4K) },

	/* EON -- en25xxx */
	{ "en25f32",    INFO(0x1c3116, 0, 64 * 1024,   64, SECT_4K) },
	{ "en25p32",    INFO(0x1c2016, 0, 64 * 1024,   64, 0) },
	{ "en25q32b",   INFO(0x1c3016, 0, 64 * 1024,   64,
			SPI_NOR_DUAL_READ), PARAMS(eon), CLK_MHZ_2X(80) },
	{ "en25p64",    INFO(0x1c2017, 0, 64 * 1024,  128, 0) },
	{ "en25q64",    INFO(0x1c3017, 0, 64 * 1024,  128,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(eon), CLK_MHZ_2X(80) },
	{ "en25qh32b-104hip2b",    INFO(0x1c7016, 0, 64 * 1024,  64,
			SPI_NOR_DUAL_READ), PARAMS(eon), CLK_MHZ_2X(80) },
	{ "en25qh64a",    INFO(0x1c7017, 0, 64 * 1024,  128,
			SPI_NOR_DUAL_READ), PARAMS(eon), CLK_MHZ_2X(80) },
	{ "en25q128",   INFO(0x1c3018, 0, 64 * 1024,  256,
			SPI_NOR_DUAL_READ), PARAMS(eon), CLK_MHZ_2X(80) },
	{ "en25qh128",  INFO(0x1c7018, 0, 64 * 1024,  256, 0) },
	{ "en25qh128a", INFO(0x1c7018, 0, 64 * 1024,  256,
			SPI_NOR_DUAL_READ), PARAMS(eon), CLK_MHZ_2X(80) },
	{ "en25qh256",  INFO(0x1c7019, 0, 64 * 1024,  512, 0) },
	{ "en25s64",	INFO(0x1c3817, 0, 64 * 1024,  128, SECT_4K) },

	/* ESMT */
	{ "f25l32pa", INFO(0x8c2016, 0, 64 * 1024, 64, SECT_4K) },
	{ "f25l64qa", INFO(0x8c4117, 0, 64 * 1024, 128,
					SPI_NOR_DUAL_READ), PARAMS(esmt), CLK_MHZ_2X(84) },

	/* Everspin */
	{ "mr25h256", CAT25_INFO(32 * 1024, 1, 256, 2, SPI_NOR_NO_ERASE
			| SPI_NOR_NO_FR) },
	{ "mr25h10",  CAT25_INFO(128 * 1024, 1, 256, 3, SPI_NOR_NO_ERASE
			| SPI_NOR_NO_FR) },

	/* Fujitsu */
	{ "mb85rs1mt", INFO(0x047f27, 0, 128 * 1024, 1, SPI_NOR_NO_ERASE) },

	/* GigaDevice 3.3V */
	{ "gd25q16c", INFO(0xc84015, 0, 64 * 1024, 32,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(gd), CLK_MHZ_2X(80) },
	{ "gd25q32", INFO(0xc84016, 0, 64 * 1024,  64,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(gd), CLK_MHZ_2X(80) },
	{ "gd25q64", INFO(0xc84017, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(gd), CLK_MHZ_2X(80) },
	{ "gd25q128/gd25q127", INFO(0xc84018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(gd), CLK_MHZ_2X(80) },
	{ "gd25q256", INFO(0xc84019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES), PARAMS(gd), CLK_MHZ_2X(80) },
	/* GigaDevice 1.8V */
	{ "gd25lq16c", INFO(0xc86015, 0, 64 * 1024, 32,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(gd), CLK_MHZ_2X(80) },
	{ "gd25lq64", INFO(0xc86017, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(gd), CLK_MHZ_2X(80) },
	{ "gd25lq128", INFO(0xc86018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(gd), CLK_MHZ_2X(80) },

	/* Intel/Numonyx -- xxxs33b */
	{ "160s33b",  INFO(0x898911, 0, 64 * 1024,  32, 0) },
	{ "320s33b",  INFO(0x898912, 0, 64 * 1024,  64, 0) },
	{ "640s33b",  INFO(0x898913, 0, 64 * 1024, 128, 0) },

	/* ISSI */
	{ "is25cd512", INFO(0x7f9d20, 0, 32 * 1024,   2, SECT_4K) },

	{ "IS25WP512M-RMLA3", INFO(0x9d701a, 0, 64 * 1024, 1024,
		SPI_NOR_DUAL_READ), PARAMS(issi), CLK_MHZ_2X(80) },

	/* Macronix/MXIC 3.3V */
	{ "mx25l512e",   INFO(0xc22010, 0, 64 * 1024,   1, SECT_4K) },
	{ "mx25l2005a",  INFO(0xc22012, 0, 64 * 1024,   4, SECT_4K) },
	{ "mx25l4005a",  INFO(0xc22013, 0, 64 * 1024,   8, SECT_4K) },
	{ "mx25l8005",   INFO(0xc22014, 0, 64 * 1024,  16, 0) },
	{ "mx25l1606e",  INFO(0xc22015, 0, 64 * 1024,  32, SECT_4K
			| SPI_NOR_DUAL_READ), CLK_MHZ_2X(80) },
	{ "mx25l3205d",  INFO(0xc22016, 0, 64 * 1024,  64, 0) },
	{ "mx25l3255e",  INFO(0xc29e16, 0, 64 * 1024,  64, SECT_4K) },
	{ "mx25l6436f",  INFO(0xc22017, 0, 64 * 1024, 128,
			SPI_NOR_DUAL_READ), CLK_MHZ_2X(80) },
	{ "mx25l12835f", INFO(0xc22018, 0, 64 * 1024, 256,
			SPI_NOR_DUAL_READ), PARAMS(mxic), CLK_MHZ_2X(84) },
	{ "mx25l12855e", INFO(0xc22618, 0, 64 * 1024, 256, 0) },
	{ "mx25l25635f", INFO(0xc22019, 0, 64 * 1024, 512,
			SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES), PARAMS(mxic), CLK_MHZ_2X(84) },
	{ "mx25l25673g", INFO(0xc22019, 0, 64 * 1024, 512, SPI_NOR_QUAD_READ
			| SPI_NOR_4B_OPCODES) },
	{ "mx25l25655e", INFO(0xc22619, 0, 64 * 1024, 512, 0) },
	{ "mx66l51235l/mx25l51245g", INFO(0xc2201a, 0, 64 * 1024, 1024,
					SPI_NOR_DUAL_READ), PARAMS(mxic), CLK_MHZ_2X(80)},
	{ "mx66l1g55g",  INFO(0xc2261b, 0, 64 * 1024, 2048, SPI_NOR_QUAD_READ)},
	{ "mx25v1635f",  INFO(0xc22315, 0, 64 * 1024, 32 ,
			SPI_NOR_DUAL_READ), PARAMS(mxic), CLK_MHZ_2X(80) },
	/* Macronix/MXIC Wide Voltage Range 1.65~3.6V */
	{ "mx25r6435f",  INFO(0xc22817, 0, 64 * 1024, 128,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ), CLK_MHZ_2X(80) },
	/* Macronix/MXIC 1.8V */
	{ "mx25u1633f",  INFO(0xc22535, 0, 64 * 1024, 32,
			SPI_NOR_DUAL_READ), PARAMS(mxic), CLK_MHZ_2X(80) },
	{ "mx25u6435f",  INFO(0xc22537, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(mxic), CLK_MHZ_2X(84) },
	{ "mx25u12835f/mx25u12832f", INFO(0xc22538, 0, 64 * 1024, 256,
			SPI_NOR_DUAL_READ), PARAMS(mxic), CLK_MHZ_2X(84) },
	{ "mx25u25635f", INFO(0xc22539, 0, 64 * 1024, 512,
			SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES), PARAMS(mxic), CLK_MHZ_2X(84) },
	{ "mx25u51245g", INFO(0xc2253a, 0, 64 * 1024, 1024,
			SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES), PARAMS(mxic), CLK_MHZ_2X(166) },
	{ "mx66u1g45gm", INFO(0xc2253b, 0, 64 * 1024, 2048,
			SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES), PARAMS(mxic), CLK_MHZ_2X(80) },

	/* Micron 3.3V */
	{ "n25q032",     INFO(0x20ba16, 0, 64 * 1024,   64, SPI_NOR_DUAL_READ),
			PARAMS(micron), CLK_MHZ_2X(84) },
	{ "n25q064",     INFO(0x20ba17, 0, 64 * 1024,  128, SPI_NOR_DUAL_READ),
			PARAMS(micron_4k), CLK_MHZ_2X(108) },
	{ "n25q128a13",  INFO(0x20ba18, 0, 64 * 1024,  256, SPI_NOR_DUAL_READ),
			PARAMS(micron), CLK_MHZ_2X(108) },
 	{ "mt25ql256a",  INFO(0x20ba19, 0x1044, 64 * 1024,  512, SPI_NOR_DUAL_READ),
			PARAMS(micron), CLK_MHZ_2X(108) },
	{ "n25q512ax3",  INFO(0x20ba20, 0, 64 * 1024, 1024, USE_FSR),
			PARAMS(micron_4k) },
	{ "n25q00",      INFO(0x20ba21, 0, 64 * 1024, 2048, USE_FSR | SPI_NOR_DUAL_READ),
			PARAMS(micron_4k), CLK_MHZ_2X(80) },
	/* Micron 1.8V */
	{ "n25q032a",    INFO(0x20bb16, 0, 64 * 1024,   64, SPI_NOR_DUAL_READ),
			PARAMS(micron), CLK_MHZ_2X(108) },
	{ "n25q064a",    INFO(0x20bb17, 0, 64 * 1024,  128, SPI_NOR_DUAL_READ),
			PARAMS(micron), CLK_MHZ_2X(108) },
	{ "mt25qu128a/n25q128a11",  INFO(0x20bb18, 0, 64 * 1024,  256, SPI_NOR_DUAL_READ),
			PARAMS(micron), CLK_MHZ_2X(108) },
	{ "mt25qu256a",  INFO(0x20bb19, 0, 64 * 1024,  512,
			SPI_NOR_4B_OPCODES | SPI_NOR_DUAL_READ), PARAMS(micron), CLK_MHZ_2X(108) },
	{ "n25q512a",    INFO(0x20bb20, 0, 64 * 1024, 1024, USE_FSR | SPI_NOR_DUAL_READ),
			PARAMS(micron_4k), CLK_MHZ_2X(80) },

	/* XMC */
	{ "xm25qh64a",    INFO(0x207017, 0, 64 * 1024,   128, SPI_NOR_DUAL_READ),
			PARAMS(xmc), CLK_MHZ_2X(80) },
	{ "xm25qh64b",    INFO(0x206017, 0, 64 * 1024,   128, SPI_NOR_DUAL_READ),
			PARAMS(xmc), CLK_MHZ_2X(80) },
	{ "xm25qh128a",   INFO(0x207018, 0, 64 * 1024,   256, SPI_NOR_DUAL_READ),
			PARAMS(xmc), CLK_MHZ_2X(80) },
	{ "xm25qh128b",   INFO(0x206018, 0, 64 * 1024,   256, SPI_NOR_DUAL_READ),
			PARAMS(xmc), CLK_MHZ_2X(80) },

	/* PMC */
	{ "pm25lv512",	INFO(0,        0, 32 * 1024,    2, SECT_4K_PMC) },
	{ "pm25lv010",	INFO(0,        0, 32 * 1024,    4, SECT_4K_PMC) },
	{ "pm25lq032",	INFO(0x7f9d46, 0, 64 * 1024,   64, SECT_4K) },

	/* Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl032p",  INFO(0x010215, 0x4d00,  64 * 1024,  64,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "s25sl064p",  INFO(0x010216, 0x4d00,  64 * 1024, 128,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "s25fl256s0", INFO(0x010219, 0x4d00, 256 * 1024, 128, 0) },
	{ "s25fl256s1", INFO(0x010219, 0x4d01,  64 * 1024, 512,
			 SPI_NOR_4B_OPCODES | SPI_NOR_DUAL_READ), PARAMS(spansion), CLK_MHZ_2X(80) },
	{ "s25fl512s",	INFO(0x010220, 0x4d00, 256 * 1024, 256,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "s70fl01gs",	INFO(0x010221, 0x4d00, 256 * 1024, 256, 0) },
	{ "s25fl127s/129p1", INFO(0x012018, 0x4d01, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(spansion), CLK_MHZ_2X(108) },
	{ "s25sl12800", INFO(0x012018, 0x0300, 256 * 1024,  64, 0) },
	{ "s25sl12801", INFO(0x012018, 0x0301,  64 * 1024, 256, 0) },
	{ "s25fl128s",	INFO6(0x012018, 0x4d0180, 64 * 1024, 256, SECT_4K
			| SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "s25fl128lagmfi010z",  INFO(0x016018, 0, 64 * 1024, 256,
			SPI_NOR_QUAD_READ)},
	{ "s25fl129p0", INFO(0x012018, 0x4d00, 256 * 1024,  64,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "s25fl129p1", INFO(0x012018, 0x4d01,  64 * 1024, 256,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "s25sl004a",	INFO(0x010212,      0,  64 * 1024,   8, 0) },
	{ "s25sl008a",	INFO(0x010213,      0,  64 * 1024,  16, 0) },
	{ "s25sl016a",	INFO(0x010214,      0,  64 * 1024,  32, 0) },
	{ "s25sl032a",	INFO(0x010215,      0,  64 * 1024,  64, 0) },
	{ "s25sl064a",	INFO(0x010216,      0,  64 * 1024, 128, 0) },
	{ "s25fl004k",	INFO(0xef4013,      0,  64 * 1024,   8, SECT_4K
			| SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "s25fl008k",	INFO(0xef4014,      0,  64 * 1024,  16, SECT_4K
			| SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "w25Q16jv-iq/s25fl016k",	INFO(0xef4015,      0,  64 * 1024,  32,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(winbond), CLK_MHZ_2X(84) },
	/* { "s25fl064k",	INFO(0xef4017,      0,  64 * 1024, 128, SECT_4K
			| SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) }, */
	{ "s25fl132k",	INFO(0x014016,      0,  64 * 1024,  64, SECT_4K) },
	{ "s25fl164k",	INFO(0x014017,      0,  64 * 1024, 128, SECT_4K) },
	{ "s25fl204k",	INFO(0x014013,      0,  64 * 1024,   8, SECT_4K
			| SPI_NOR_DUAL_READ) },

	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", INFO(0xbf258d, 0, 64 * 1024,  8, SECT_4K | SST_WRITE) },
	{ "sst25vf080b", INFO(0xbf258e, 0, 64 * 1024, 16, SECT_4K | SST_WRITE) },
	{ "sst25vf016b", INFO(0xbf2541, 0, 64 * 1024, 32, SECT_4K | SST_WRITE) },
	{ "sst25vf032b", INFO(0xbf254a, 0, 64 * 1024, 64, SECT_4K | SST_WRITE) },
	{ "sst25vf064c", INFO(0xbf254b, 0, 64 * 1024, 128, SECT_4K) },
	{ "sst25wf512",  INFO(0xbf2501, 0, 64 * 1024,  1, SECT_4K | SST_WRITE) },
	{ "sst25wf010",  INFO(0xbf2502, 0, 64 * 1024,  2, SECT_4K | SST_WRITE) },
	{ "sst25wf020",  INFO(0xbf2503, 0, 64 * 1024,  4, SECT_4K | SST_WRITE) },
	{ "sst25wf020a", INFO(0x621612, 0, 64 * 1024,  4, SECT_4K) },
	{ "sst25wf040b", INFO(0x621613, 0, 64 * 1024,  8, SECT_4K) },
	{ "sst25wf040",  INFO(0xbf2504, 0, 64 * 1024,  8, SECT_4K | SST_WRITE) },
	{ "sst25wf080",  INFO(0xbf2505, 0, 64 * 1024, 16, SECT_4K | SST_WRITE) },

	/* ST Microelectronics -- newer production may have feature updates */
	{ "m25p05",  INFO(0x202010,  0,  32 * 1024,   2, 0) },
	{ "m25p10",  INFO(0x202011,  0,  32 * 1024,   4, 0) },
	{ "m25p20",  INFO(0x202012,  0,  64 * 1024,   4, 0) },
	{ "m25p40",  INFO(0x202013,  0,  64 * 1024,   8, 0) },
	{ "m25p80",  INFO(0x202014,  0,  64 * 1024,  16, 0) },
	{ "m25p16",  INFO(0x202015,  0,  64 * 1024,  32, 0) },
	{ "m25p32",  INFO(0x202016,  0,  64 * 1024,  64, 0) },
	{ "m25p64",  INFO(0x202017,  0,  64 * 1024, 128, 0) },
	{ "m25p128", INFO(0x202018,  0, 256 * 1024,  64, 0) },

	{ "m25p05-nonjedec",  INFO(0, 0,  32 * 1024,   2, 0) },
	{ "m25p10-nonjedec",  INFO(0, 0,  32 * 1024,   4, 0) },
	{ "m25p20-nonjedec",  INFO(0, 0,  64 * 1024,   4, 0) },
	{ "m25p40-nonjedec",  INFO(0, 0,  64 * 1024,   8, 0) },
	{ "m25p80-nonjedec",  INFO(0, 0,  64 * 1024,  16, 0) },
	{ "m25p16-nonjedec",  INFO(0, 0,  64 * 1024,  32, 0) },
	{ "m25p32-nonjedec",  INFO(0, 0,  64 * 1024,  64, 0) },
	{ "m25p64-nonjedec",  INFO(0, 0,  64 * 1024, 128, 0) },
	{ "m25p128-nonjedec", INFO(0, 0, 256 * 1024,  64, 0) },

	{ "m45pe10", INFO(0x204011,  0, 64 * 1024,    2, 0) },
	{ "m45pe80", INFO(0x204014,  0, 64 * 1024,   16, 0) },
	{ "m45pe16", INFO(0x204015,  0, 64 * 1024,   32, 0) },

	{ "m25pe20", INFO(0x208012,  0, 64 * 1024,  4,       0) },
	{ "m25pe80", INFO(0x208014,  0, 64 * 1024, 16,       0) },
	{ "m25pe16", INFO(0x208015,  0, 64 * 1024, 32, SECT_4K) },

	{ "m25px16",    INFO(0x207115,  0, 64 * 1024, 32, SECT_4K) },
	{ "m25px32",    INFO(0x207116,  0, 64 * 1024, 64, SECT_4K) },
	{ "m25px32-s0", INFO(0x207316,  0, 64 * 1024, 64, SECT_4K) },
	{ "m25px32-s1", INFO(0x206316,  0, 64 * 1024, 64, SECT_4K) },
	{ "m25px64",    INFO(0x207117,  0, 64 * 1024, 128, 0) },
	{ "m25px80",    INFO(0x207114,  0, 64 * 1024, 16, 0) },

	/* Winbond 3.3V-- w25x "blocks" are 64K, "sectors" are 4KiB */
	{ "w25x05", INFO(0xef3010, 0, 64 * 1024,  1,  SECT_4K) },
	{ "w25x10", INFO(0xef3011, 0, 64 * 1024,  2,  SECT_4K) },
	{ "w25x20", INFO(0xef3012, 0, 64 * 1024,  4,  SECT_4K) },
	{ "w25x40", INFO(0xef3013, 0, 64 * 1024,  8,  SECT_4K) },
	{ "w25x80", INFO(0xef3014, 0, 64 * 1024,  16, SECT_4K) },
	{ "w25x16", INFO(0xef3015, 0, 64 * 1024,  32, SECT_4K
			| SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "w25x32", INFO(0xef3016, 0, 64 * 1024,  64, SECT_4K) },
	{ "w25q32", INFO(0xef4016, 0, 64 * 1024,  64,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(winbond), CLK_MHZ_2X(80) },
	{ "w25x64", INFO(0xef3017, 0, 64 * 1024, 128, SECT_4K) },
	{ "w25q64fv(spi)/w25q64jv_iq", INFO(0xef4017, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(winbond), CLK_MHZ_2X(80) },
	{ "w25q80", INFO(0xef5014, 0, 64 * 1024,  16, SECT_4K) },
	{ "w25q80bl", INFO(0xef4014, 0, 64 * 1024,  16, SECT_4K) },
	{ "w25q128(b/f)v", INFO(0xef4018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(winbond), CLK_MHZ_2X(80) },
  	{ "w25q128jv_im", INFO(0xef7018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(winbond), CLK_MHZ_2X(80) },
#ifdef CONFIG_AUTOMOTIVE_GRADE
	{ "w25q256(f/j)v", INFO(0xef4019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ), PARAMS(winbond), CLK_MHZ_2X(80) },
#else	
	{ "w25q256(f/j)v", INFO(0xef4019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES), PARAMS(winbond), CLK_MHZ_2X(80) },
#endif
	/* Winbond 1.8V */
	{ "w25q32fw", INFO(0xef6016, 0, 64 * 1024,  64,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
			SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB), PARAMS(winbond), CLK_MHZ_2X(80) },
	{ "w25q64dw", INFO(0xef6017, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
			SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB), PARAMS(winbond), CLK_MHZ_2X(80) },
	{ "w25q128fw", INFO(0xef6018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
			SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB), PARAMS(winbond), CLK_MHZ_2X(80) },
	{ "w25q256jw-im", INFO(0xef8019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
			SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB | SPI_NOR_4B_OPCODES),
			PARAMS(winbond), CLK_MHZ_2X(80) },
	{ "w25q256jw-iq", INFO(0xef6019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
			SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB | SPI_NOR_4B_OPCODES),
			PARAMS(winbond), CLK_MHZ_2X(80) },

	/* Catalyst / On Semiconductor -- non-JEDEC */
	{ "cat25c11", CAT25_INFO(16, 8, 16, 1, SPI_NOR_NO_ERASE
			| SPI_NOR_NO_FR) },
	{ "cat25c03", CAT25_INFO(32, 8, 16, 2, SPI_NOR_NO_ERASE
			| SPI_NOR_NO_FR) },
	{ "cat25c09", CAT25_INFO(28, 8, 32, 2, SPI_NOR_NO_ERASE
			| SPI_NOR_NO_FR) },
	{ "cat25c17", CAT25_INFO(256, 8, 32, 2, SPI_NOR_NO_ERASE
			| SPI_NOR_NO_FR) },
	{ "cat25128", CAT25_INFO(2048, 8, 64, 2, SPI_NOR_NO_ERASE
			| SPI_NOR_NO_FR) },
	/* Paragon 3.3V */
	{ "pn25f16s", INFO(0xe04015, 0, 64 * 1024,  32,
			SPI_NOR_DUAL_READ), PARAMS(paragon), CLK_MHZ_2X(80) },
	{ "pn25f32s", INFO(0xe04016, 0, 64 * 1024,  64,
			SPI_NOR_DUAL_READ), PARAMS(paragon), CLK_MHZ_2X(80) },

	/* XTX */
	{ "xt25f16bssigu", INFO(0x0b4015, 0, 64 * 1024,  32,
		SPI_NOR_DUAL_READ), PARAMS(xtx), CLK_MHZ_2X(80) },

	{ "xt25f32bssigu-s", INFO(0x0b4016, 0, 64 * 1024,  64,
		SPI_NOR_DUAL_READ), PARAMS(xtx), CLK_MHZ_2X(80) },

	{ "xt25f128b", INFO(0x0b4018, 0, 64 * 1024,  256,
		SPI_NOR_DUAL_READ), PARAMS(xtx), CLK_MHZ_2X(70) },

	{ "xt25f64b", INFO(0x0b4017, 0, 64 * 1024,  128,
		SPI_NOR_DUAL_READ), PARAMS(xtx), CLK_MHZ_2X(70) },

	/*puya 3.3V */
	{"p25q128h", INFO(0x856018, 0, 64 * 1024, 256,
		SPI_NOR_DUAL_READ), PARAMS(puya), CLK_MHZ_2X(80) },

	/* FM 3.3v */
	{ "FM25Q64-SOB-T-G",INFO(0xa14017, 0, 64 * 1024, 128,
		SPI_NOR_DUAL_READ), PARAMS(spansion), CLK_MHZ_2X(80) },
	{ "FM25Q128-SOB-T-G",INFO(0xa14018, 0, 64 * 1024, 256,
		SPI_NOR_DUAL_READ), PARAMS(spansion), CLK_MHZ_2X(80) },
		
	/* HUAHONG 3.3v */
	{ "H25S64",INFO(0x684017, 0, 64 * 1024, 128,
		SPI_NOR_DUAL_READ), PARAMS(spansion), CLK_MHZ_2X(80) },
		
	{ "H25S128",INFO(0x684018, 0, 64 * 1024, 256,
		SPI_NOR_DUAL_READ), PARAMS(spansion), CLK_MHZ_2X(80) },

	{ "ZB25VQ64A",INFO(0x5e4017, 0, 64 * 1024, 128,
		SPI_NOR_DUAL_READ), PARAMS(spansion), CLK_MHZ_2X(80) },

	/* SiliconKaiser 3.3v */
	{ "sk25p128", INFO(0x256018, 0, 64 * 1024,  256, SPI_NOR_DUAL_READ), PARAMS(xtx), CLK_MHZ_2X(70) },

	{ "sk25p64", INFO(0x256017, 0, 64 * 1024,  128, SPI_NOR_DUAL_READ), PARAMS(xtx), CLK_MHZ_2X(70) },

	{ },
};

static const struct flash_info *spi_nor_read_id(struct spi_nor *nor)
{
	int			tmp;
	u8			id[SPI_NOR_MAX_ID_LEN];
	const struct flash_info	*info;

	tmp = nor->read_reg(nor, SPINOR_OP_RDID, id, SPI_NOR_MAX_ID_LEN);
	if (tmp < 0) {
		dev_dbg(nor->dev, "error %d reading JEDEC ID\n", tmp);
		return ERR_PTR(tmp);
	}

	if ((id[0] == 0xff) || (id[0] == 0x00)) {
		dev_err(nor->dev, "unrecognized Manufacturer ID\n");
		return ERR_PTR(-ENODEV);
	}

	for (tmp = 0; tmp < ARRAY_SIZE(spi_nor_ids) - 1; tmp++) {
		info = &spi_nor_ids[tmp];
		if (info->id_len) {
			if (!memcmp(info->id, id, info->id_len))
				return &spi_nor_ids[tmp];
		}
	}
	dev_err(nor->dev, "unrecognized JEDEC id bytes: %02x, %02x, %02x\n",
		id[0], id[1], id[2]);
	return ERR_PTR(-ENODEV);
}

static int puya_quad_enable(struct spi_nor *nor)
{
	int ret;
	u8 val;

	ret = read_cr(nor);
	if ((unsigned int)ret & CR_QUAD_EN_SPAN)
		return 0;

	val = (((unsigned int)ret & 0xff) | CR_QUAD_EN_SPAN);
	write_enable(nor);

	ret = write_sr2(nor, val);
	if (ret < 0) {
		dev_err(nor->dev,
			"error while writing status register-2\n");
		return -EINVAL;
	}

	if (spi_nor_wait_till_ready(nor))
		return 1;

	/* read back and check it */
	ret = read_cr(nor);
	if ((unsigned int)ret & CR_QUAD_EN_SPAN)
		return 0;
	else
		return 1;
}

static int spi_nor_read(struct mtd_info *mtd, loff_t from, size_t len,
			size_t *retlen, u_char *buf)
{
	struct spi_nor *nor = mtd_to_spi_nor(mtd);
	int ret;

	dev_dbg(nor->dev, "from 0x%08x, len %zd\n", (u32)from, len);

	ret = spi_nor_lock_and_prep(nor, SPI_NOR_OPS_READ);
	if (ret)
		return ret;

	while (len) {
		ret = nor->read(nor, from, len, buf);
		if (ret == 0) {
			/* We shouldn't see 0-length reads */
			ret = -EIO;
			goto read_err;
		}
		if (ret < 0)
			goto read_err;

		WARN_ON(ret > len);
		*retlen += ret;
		buf += ret;
		from += ret;
		len -= ret;
	}
	ret = 0;

read_err:
	spi_nor_unlock_and_unprep(nor, SPI_NOR_OPS_READ);
	return ret;
}

static int sst_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	struct spi_nor *nor = mtd_to_spi_nor(mtd);
	size_t actual;
	int ret;

	dev_dbg(nor->dev, "to 0x%08x, len %zd\n", (u32)to, len);

	ret = spi_nor_lock_and_prep(nor, SPI_NOR_OPS_WRITE);
	if (ret)
		return ret;

	write_enable(nor);

	nor->sst_write_second = false;

	actual = to % 2;
	/* Start write from odd address. */
	if (actual) {
		nor->program_opcode = SPINOR_OP_BP;

		/* write one byte. */
		ret = nor->write(nor, to, 1, buf);
		if (ret < 0)
			goto sst_write_err;
		WARN(ret != 1, "While writing 1 byte written %i bytes\n",
		     (int)ret);
		ret = spi_nor_wait_till_ready(nor);
		if (ret)
			goto sst_write_err;
	}
	to += actual;

	/* Write out most of the data here. */
	for (; actual < len - 1; actual += 2) {
		nor->program_opcode = SPINOR_OP_AAI_WP;

		/* write two bytes. */
		ret = nor->write(nor, to, 2, buf + actual);
		if (ret < 0)
			goto sst_write_err;
		WARN(ret != 2, "While writing 2 bytes written %i bytes\n",
		     (int)ret);
		ret = spi_nor_wait_till_ready(nor);
		if (ret)
			goto sst_write_err;
		to += 2;
		nor->sst_write_second = true;
	}
	nor->sst_write_second = false;

	write_disable(nor);
	ret = spi_nor_wait_till_ready(nor);
	if (ret)
		goto sst_write_err;

	/* Write out trailing byte if it exists. */
	if (actual != len) {
		write_enable(nor);

		nor->program_opcode = SPINOR_OP_BP;
		ret = nor->write(nor, to, 1, buf + actual);
		if (ret < 0)
			goto sst_write_err;
		WARN(ret != 1, "While writing 1 byte written %i bytes\n",
		     (int)ret);
		ret = spi_nor_wait_till_ready(nor);
		if (ret)
			goto sst_write_err;
		write_disable(nor);
		actual += 1;
	}
sst_write_err:
	*retlen += actual;
	spi_nor_unlock_and_unprep(nor, SPI_NOR_OPS_WRITE);
	return ret;
}

/*
 * Write an address range to the nor chip.  Data must be written in
 * FLASH_PAGESIZE chunks.  The address range may be any size provided
 * it is within the physical boundaries.
 */
static int spi_nor_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct spi_nor *nor = mtd_to_spi_nor(mtd);
	size_t page_offset, page_remain, i;
	ssize_t ret;

	dev_dbg(nor->dev, "to 0x%08x, len %zd\n", (u32)to, len);

	ret = spi_nor_lock_and_prep(nor, SPI_NOR_OPS_WRITE);
	if (ret)
		return ret;
#ifdef CONFIG_GOKE_SPI_BLOCK_PROTECT
	if (nor->level && (to < nor->end_addr)) {
		dev_err(nor->dev, "Error: The DMA write area was locked\n");
		return -EINVAL;
	}
#endif

	for (i = 0; i < len; ) {
		ssize_t written;

		page_offset = (to + i) & (nor->page_size - 1);
#ifndef CONFIG_SPI_GOKE_SFC
		WARN_ONCE(page_offset,
			  "Writing at offset %zu into a NOR page. Writing partial pages may decrease reliability and increase wear of NOR flash.",
			  page_offset);
#endif
		/* the size of data remaining on the first page */
		page_remain = min_t(size_t,
				    nor->page_size - page_offset, len - i);

		write_enable(nor);
		ret = nor->write(nor, to + i, page_remain, buf + i);
		if (ret < 0)
			goto write_err;
		written = ret;

		ret = spi_nor_wait_till_ready(nor);
		if (ret)
			goto write_err;
		*retlen += written;
		i += written;
		if (written != page_remain) {
			dev_err(nor->dev,
				"While writing %zu bytes written %zd bytes\n",
				page_remain, written);
			ret = -EIO;
			goto write_err;
		}
	}

write_err:
	spi_nor_unlock_and_unprep(nor, SPI_NOR_OPS_WRITE);
	return ret;
}

static int macronix_quad_enable(struct spi_nor *nor)
{
	int ret, val;

	val = read_sr(nor);
	if (val < 0)
		return val;

	if ((unsigned int)val & SR_QUAD_EN_MX)
		return 0;

	/* Update the Quad Enable bit. */
	dev_dbg(nor->dev, "setting Macronix Quad Enable (non-volatile) bit\n");

	write_enable(nor);

	write_sr(nor, (u8)val | SR_QUAD_EN_MX);

	if (spi_nor_wait_till_ready(nor))
		return 1;

	ret = read_sr(nor);
	if (!(ret > 0 && ((unsigned int)ret & SR_QUAD_EN_MX))) {
		dev_err(nor->dev, "Macronix Quad bit not set\n");
		return -EINVAL;
	}

	return 0;
}

static int xtx_quad_enable(struct spi_nor *nor)
{
	u8 ret, val_h,val_l;
	/* read SR high 8bit*/
	val_h = read_cr(nor);
	if (val_h < 0)
		return val_h;

	if (val_h & SR_QUAD_EN_XTX)
		return 0;

	/* Update the Quad Enable bit. */
	dev_dbg(nor->dev, "setting xtx Quad Enable (non-volatile) bit\n");

	write_enable(nor);

	/* read SR low 8bit*/
	val_l = read_sr(nor);

	/* write SR */
	nor->cmd_buf[0] = val_l;
	nor->cmd_buf[1] = val_h;
	nor->write_reg(nor, SPINOR_OP_WRSR, nor->cmd_buf, 2);

	if (spi_nor_wait_till_ready(nor))
		return 1;

	ret = read_cr(nor);
	if (!(ret > 0 && (ret & SR_QUAD_EN_XTX))) {
		dev_err(nor->dev, "xtx Quad bit not set\n");
		return -EINVAL;
	}

	return 0;
}
/*
 * Write status Register and configuration register with 2 bytes
 * The first byte will be written to the status register, while the
 * second byte will be written to the configuration register.
 * Return negative if error occured.
 */
static int write_sr_cr(struct spi_nor *nor, u16 val)
{
	nor->cmd_buf[0] = val & 0xff;
	nor->cmd_buf[1] = (val >> 8);

	return nor->write_reg(nor, SPINOR_OP_WRSR, nor->cmd_buf, 2);
}

static int spansion_quad_enable(struct spi_nor *nor)
{
	unsigned int ret;
	u16 val;

	ret = read_cr(nor);
	if (ret & CR_QUAD_EN_SPAN)
		return 0;

	/* Update the Quad Enable bit. */
	dev_dbg(nor->dev, "setting Quad Enable (non-volatile) bit\n");

	val = ((ret & 0xff) | CR_QUAD_EN_SPAN) << 8;

	ret = read_sr(nor);
	val |= (ret & 0xff);

	write_enable(nor);

	ret = write_sr_cr(nor, val);
	if (ret < 0) {
		dev_err(nor->dev,
			"error while writing configuration register\n");
		return -EINVAL;
	}

	if (spi_nor_wait_till_ready(nor))
		return 1;

	/* read back and check it */
	ret = read_cr(nor);
	if (!(ret > 0 && (ret & CR_QUAD_EN_SPAN))) {
		dev_err(nor->dev, "Spansion Quad bit not set\n");
		return -EINVAL;
	}

	return 0;
}

static int issi_quad_enable(struct spi_nor *nor)
{
	unsigned int ret;
	u16 val;

	ret = read_sr(nor);
	if (ret & QUAD_EN_ISSI)
		return 0;

	/* Update the Quad Enable bit. */
	dev_dbg(nor->dev, "setting Quad Enable (non-volatile) bit\n");

	val = ((ret & 0xff) | QUAD_EN_ISSI);

	write_enable(nor);

	ret = write_sr(nor, val);
	if (ret < 0) {
		dev_err(nor->dev,
			"error while writing configuration register\n");
		return -EINVAL;
	}

	if (issi_spi_nor_wait_till_ready(nor))
		return 1;

	/* read back and check it */
	ret = read_sr(nor);
	if (!(ret > 0 && (ret & QUAD_EN_ISSI))) {
		dev_err(nor->dev, "ISSI Quad bit not set\n");
		return -EINVAL;
	}

	return 0;
}

static int micron_quad_enable(struct spi_nor *nor)
{
	int ret;
	u8 val;

	ret = nor->read_reg(nor, SPINOR_OP_RD_EVCR, &val, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error %d reading EVCR\n", ret);
		return ret;
	}

	write_enable(nor);

	/* set EVCR, enable quad I/O */
	nor->cmd_buf[0] = val & ~EVCR_QUAD_EN_MICRON;
	ret = nor->write_reg(nor, SPINOR_OP_WD_EVCR, nor->cmd_buf, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error while writing EVCR register\n");
		return ret;
	}

	ret = spi_nor_wait_till_ready(nor);
	if (ret)
		return ret;

	/* read EVCR and check it */
	ret = nor->read_reg(nor, SPINOR_OP_RD_EVCR, &val, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error %d reading EVCR\n", ret);
		return ret;
	}
	if (val & EVCR_QUAD_EN_MICRON) {
		dev_err(nor->dev, "Micron EVCR Quad bit not clear\n");
		return -EINVAL;
	}

	return 0;
}

static int gd_quad_enable(struct spi_nor *nor)
{
	int ret;
	u16 val;

	/* First, Quad Enable for 16-Pin GD flash, use WRSR[01h] cmd */
	ret = read_cr(nor);
	val = (((unsigned int)ret & 0xff) | CR_QUAD_EN_SPAN) << 8;

	ret = read_sr(nor);
	val |= ((unsigned int)ret & 0xff);

	write_enable(nor);

	ret = write_sr_cr(nor, val);
	if (ret < 0) {
		dev_err(nor->dev,
			"error while writing config and status register\n");
		return -EINVAL;
	}

	if (spi_nor_wait_till_ready(nor))
		return 1;

	/* read back and check it */
	ret = read_cr(nor);
	if ((unsigned int)ret & CR_QUAD_EN_SPAN)
		return 0;

	/* Second, Quad Enable for 8-Pin GD flash, use WRCR[31h] cmd */
	ret = read_sr(nor);
	if (!((unsigned int)ret & SR_WEL))
		write_enable(nor);

	ret = read_cr(nor);
	nor->cmd_buf[0] = ((unsigned int)ret & 0xff) | CR_QUAD_EN_SPAN;

	ret = nor->write_reg(nor, SPINOR_OP_WRCR, nor->cmd_buf, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error while writing config register\n");
		return ret;
	}

	if (spi_nor_wait_till_ready(nor))
		return 1;

	/* read back and check it */
	ret = read_cr(nor);
	if (!(ret > 0 && ((unsigned int)ret & CR_QUAD_EN_SPAN))) {
		dev_err(nor->dev, "GigaDevice Quad bit not set\n");
		return -EINVAL;
	}

	return 0;
}

static int set_quad_mode(struct spi_nor *nor, const struct flash_info *info)
{
	int status;

	switch (JEDEC_MFR(info)) {
	case SNOR_MFR_ESMT:
	case SNOR_MFR_MACRONIX:
		status = macronix_quad_enable(nor);
		if (status) {
			dev_err(nor->dev, "Macronix quad-read not enabled\n");
			return -EINVAL;
		}
		return status;
	case SNOR_MFR_MICRON:
		status = micron_quad_enable(nor);
		if (status) {
			dev_err(nor->dev, "Micron quad-read not enabled\n");
			return -EINVAL;
		}
		return status;
	case SNOR_MFR_GD:
		status = gd_quad_enable(nor);
		if (status) {
			dev_err(nor->dev, "GD quad-read not enabled\n");
			return -EINVAL;
		}
		return status;
	case SNOR_MFR_XTX:
		status = xtx_quad_enable(nor);
		if (status) {
			dev_err(nor->dev, "xtx quad-read not enabled\n");
			return -EINVAL;
		}
		return status;
	case SNOR_MFR_PUYA:
		status = puya_quad_enable(nor);
		if (status) {
			dev_err(nor->dev, "puya quad-read not enabled\n");
			return -EINVAL;
		}
		return status;
	case SNOR_MFR_ISSI:
		status = issi_quad_enable(nor);
		if (status) {
			dev_err(nor->dev, "puya quad-read not enabled\n");
			return -EINVAL;
		}
		return status;
	default:
		status = spansion_quad_enable(nor);
		if (status) {
			dev_err(nor->dev, "Spansion quad-read not enabled\n");
			return -EINVAL;
		}
		return status;
	}
}

static int spi_nor_check(struct spi_nor *nor)
{
	if (!nor->dev || !nor->read || !nor->write ||
		!nor->read_reg || !nor->write_reg) {
		pr_err("spi-nor: please fill all the necessary fields!\n");
		return -EINVAL;
	}

	return 0;
}

#ifdef CONFIG_GOKE_SPI_BLOCK_PROTECT
static void spi_lock_update_address(struct spi_nor *nor, const struct flash_info *info)
{
	unsigned int lock_level_max, sectorsize, chipsize;

	if (!nor->level) {
		nor->end_addr = 0;
		dev_warn(nor->dev, "all blocks is unlocked.\n");
		return;
	}

	sectorsize = info->sector_size;
	chipsize = sectorsize * info->n_sectors;
	lock_level_max = nor->lock_level_max;

	switch (JEDEC_MFR(info)) {
	case SNOR_MFR_MACRONIX:
		if (chipsize == _2M) {
			if ((nor->level != lock_level_max)
					&& (nor->level != 1))
				nor->end_addr = chipsize - (sectorsize <<
					(lock_level_max - nor->level - 1));
			else
				nor->end_addr = chipsize;
			return;
		}

		if (chipsize != _8M)
			break;
	case SNOR_MFR_ESMT:
		/* this case is for ESMT and MXIC 8M devices */
		if (nor->level != lock_level_max)
			nor->end_addr = chipsize - (sectorsize
					<< (lock_level_max - nor->level));
		else
			nor->end_addr = chipsize;
		return;
	case SNOR_MFR_EON:
		if (nor->level != lock_level_max)
			nor->end_addr = chipsize - (sectorsize
					<< (nor->level - 1));
		else
			nor->end_addr = chipsize;
		return;
	default:
		break;
	}

	/* general case */
	nor->end_addr = chipsize >> (lock_level_max - nor->level);
}

static unsigned char bsp_bp_to_level(struct spi_nor *nor,
		const struct flash_info *info, unsigned int bp_num)
{
	int ret;
	unsigned char val;
	unsigned char level;
	unsigned int chipsize;

	ret = spi_nor_wait_till_ready(nor);
	BUG_ON(ret);

	ret = nor->read_reg(nor, SPINOR_OP_RDSR, &val, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error %d reading SR\n", ret);
		return ret;
	}

	if (bp_num == BP_NUM_3)
		level = (val & SPI_NOR_SR_BP_MASK_3) >> SPI_NOR_SR_BP0_SHIFT;
	else
		level = (val & SPI_NOR_SR_BP_MASK_4) >> SPI_NOR_SR_BP0_SHIFT;

	dev_dbg(nor->dev, "the current level[%d]\n", level);

	if (bp_num == BP_NUM_4) {
		nor->lock_level_max = LOCK_LEVEL_MAX(bp_num) - 5;
		chipsize = info->sector_size * info->n_sectors;
		if ((JEDEC_MFR(info) == SNOR_MFR_MACRONIX)
				&& (chipsize == _16M))
			nor->lock_level_max--;
	} else
		nor->lock_level_max = LOCK_LEVEL_MAX(bp_num);
	dev_dbg(nor->dev, "Get the max bp level: [%d]\n",
	   nor->lock_level_max);

	return level;
}

static void bsp_get_spi_lock_info(struct spi_nor *nor, const struct flash_info *info)
{
	unsigned int chipsize;
	struct device *dev = nor->dev;

	chipsize = info->sector_size * info->n_sectors;

	/* read the BP bit in RDSR to check whether nor is lock or not */
	switch (JEDEC_MFR(info)) {
	case SNOR_MFR_GD:
	case SNOR_MFR_FM:
	case SNOR_MFR_ESMT:
	case SNOR_MFR_EON:
	case SNOR_MFR_SPANSION:
		/* BP bit convert to lock level */
		nor->level = bsp_bp_to_level(nor, info, BP_NUM_3);
		break;
	case SNOR_MFR_WINBOND:
		/* BP bit convert to lock level */
		if (chipsize <= _16M)
			nor->level = bsp_bp_to_level(nor, info, BP_NUM_3);
		else
			nor->level = bsp_bp_to_level(nor, info, BP_NUM_4);
		break;
	case SNOR_MFR_MACRONIX:
	
		/* BP bit convert to lock level */
		if (chipsize <= _8M)
			nor->level = bsp_bp_to_level(nor, info, BP_NUM_3);
		else
			nor->level = bsp_bp_to_level(nor, info, BP_NUM_4);
		break;
	case SNOR_MFR_XTX:
		/* BP bit convert to lock level */
		nor->level = bsp_bp_to_level(nor, info, BP_NUM_4);
		break;
	default:
		goto usage;
	}

	spi_lock_update_address(nor, info);
	if (nor->end_addr)
		dev_info(dev, "Address range [0 => %#x] is locked.\n",
		nor->end_addr);
	return;
usage:
	dev_err(dev, "The ID: %#x isn't in the BP table,"
			" Current device can't not protect\n",
			JEDEC_MFR(info));
}
#endif/* CONFIG_GOKE_SPI_BLOCK_PROTECT */

static int spi_nor_midx2proto(int midx, enum spi_nor_protocol *proto)
{
	switch (midx) {
	case SNOR_MIDX_SLOW:
	case SNOR_MIDX_1_1_1:
		*proto = SNOR_PROTO_1_1_1;
		break;

	case SNOR_MIDX_1_1_2:
		*proto = SNOR_PROTO_1_1_2;
		break;

	case SNOR_MIDX_1_2_2:
		*proto = SNOR_PROTO_1_2_2;
		break;
	case SNOR_MIDX_1_1_4:
		*proto = SNOR_PROTO_1_1_4;
		break;

	case SNOR_MIDX_1_4_4:
		*proto = SNOR_PROTO_1_4_4;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int spi_nor_sr3_to_reset(struct spi_nor *nor)
{
	int ret;
	unsigned char val;

	ret = nor->read_reg(nor, SPINOR_OP_RDSR3, &val, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error %d reading Status Reg 3.\n", ret);
		return ret;
	}

	if (SPI_NOR_GET_RST(val)) {
		dev_dbg(nor->dev, "Device has worked on RESET#.\n");
		return 0;
	}

	dev_dbg(nor->dev, "Start to enable RESET# function.\n");
	val = SPI_NOR_SET_RST(val);

	nor->write_reg(nor, SPINOR_OP_WRSR3, &val, 1);
	if (ret < 0) {
		dev_err(nor->dev, "error while writing Status Reg 3.\n");
		return ret;
	}

	dev_dbg(nor->dev, "Enable RESET# function success.\n");

	return 0;
}

static int spi_nor_reset_pin_enable(struct spi_nor *nor,
		const struct flash_info *info)
{
	switch (JEDEC_MFR(info)) {
	case SNOR_MFR_WINBOND:
	case SNOR_MFR_GD:
		return spi_nor_sr3_to_reset(nor);
	default:
		return 0;
	}
}

static int spi_nor_setup(struct spi_nor *nor, const struct flash_info *info,
			 const struct spi_nor_basic_flash_parameter *params,
			 const struct spi_nor_modes *modes)
{
	bool enable_quad_io;
	u32 rd_modes, wr_modes;
	const struct spi_nor_erase_type *erase_type;
	const struct spi_nor_read_op *read;
	int rd_midx, wr_midx, err = 0;
#ifdef CONFIG_MTD_SPI_NOR_USE_4K_SECTORS
	int i = 0;
#endif
	rd_modes = modes->rd_modes;
	wr_modes = modes->wr_modes;

	/* Setup read operation. */
	rd_midx = fls(params->rd_modes & rd_modes) - 1;
	if (spi_nor_midx2proto(rd_midx, &nor->read_proto)) {
		dev_err(nor->dev, "invalid (fast) read\n");
		return -EINVAL;
	}
	read = &params->reads[rd_midx];
	nor->read_opcode = read->opcode;
	nor->read_dummy = read->num_mode_clocks + read->num_wait_states;

	/* Set page program op code and protocol. */
	wr_midx = fls(params->wr_modes & wr_modes) - 1;
	if (spi_nor_midx2proto(wr_midx, &nor->write_proto)) {
		dev_err(nor->dev, "invalid page program\n");
		return -EINVAL;
	}
	nor->program_opcode = params->page_programs[wr_midx];

	/* Set sector erase op code and size. */
	erase_type = &params->erase_types[0];
#ifdef CONFIG_MTD_SPI_NOR_USE_4K_SECTORS
	for (i = 1; i < SNOR_MAX_ERASE_TYPES; ++i)
		if (params->erase_types[i].size == 0x0c)
			erase_type = &params->erase_types[i];
#endif
	nor->erase_opcode = erase_type->opcode;
	nor->mtd.erasesize = (1 << erase_type->size);

	enable_quad_io = (SNOR_PROTO_DATA_FROM_PROTO(nor->read_proto) == 4 ||
			  SNOR_PROTO_DATA_FROM_PROTO(nor->write_proto) == 4);

	/* Enable Quad I/O if needed. */
	if (enable_quad_io && params->enable_quad_io) {
		err = params->enable_quad_io(nor);
		if (err) {
			dev_err(nor->dev,
				"failed to enable the Quad I/O mode\n");
			return err;
		}
	}

	/*
	 * Fix erase protocol if needed, read and write protocols should
	 * already be valid.
	 */
	nor->erase_proto = SNOR_PROTO_1_1_1;

	dev_dbg(nor->dev,
		"(Fast) Read:  opcode=%02Xh, protocol=%03x, mode=%u, wait=%u\n",
		nor->read_opcode, nor->read_proto,
		read->num_mode_clocks, read->num_wait_states);
	dev_dbg(nor->dev,
		"Page Program: opcode=%02Xh, protocol=%03x\n",
		nor->program_opcode, nor->write_proto);
	dev_dbg(nor->dev,
		"Sector Erase: opcode=%02Xh, protocol=%03x, sector size=%zu\n",
		nor->erase_opcode, nor->erase_proto, (size_t)nor->mtd.erasesize);

	return 0;
}

static int spi_nor_config(struct spi_nor *nor, const struct flash_info *info,
			 const struct spi_nor_basic_flash_parameter *params,
			 struct spi_nor_modes *modes)
{
	int ret;
	unsigned char cval,val;

	if (JEDEC_MFR(info) == SNOR_MFR_MACRONIX){
		val = read_sr(nor);
		if (val < 0)
			return val;

		/* read Configuration Register for macronix's spi nor flash */
		ret = nor->read_reg(nor, SPINOR_OP_RDSR3, &cval, 1);
		if(ret < 0){
			dev_err(nor->dev, "error %d reading config Reg.\n", ret);
			return ret;
		}

		/* check the bit[6:7] whether is set in uboot when use DTR mode;if it was set and clear it. */
		/* pay attention to sequence of issuing WRSR instruction */
		if (cval & CR_DUMMY_CYCLE){
			write_enable(nor);
			nor->cmd_buf[0]=val;
			nor->cmd_buf[1]=(cval & (~CR_DUMMY_CYCLE));
			ret = nor->write_reg(nor, SPINOR_OP_WRSR, nor->cmd_buf, 2);
		}
	}

	if (params) {
		ret = spi_nor_setup(nor, info, params, modes);
		if (ret)
			return ret;
	} else if (modes->rd_modes & SNOR_MODE_1_1_4 &&
			info->flags & SPI_NOR_QUAD_READ) {
		/*
		 * This branch is spcially for some devices which can
		 * not be stated by params, but only SPI_NOR_QUAD_READ,
		 * it just supports the protocol 1_1_4.
		 */
			if (spi_nor_wait_till_ready(nor))
				return 1;

			ret = set_quad_mode(nor, info);
			if (ret) {
				dev_err(nor->dev, "quad mode not supported\n");
				return ret;
			}
			nor->read_proto = SNOR_PROTO_1_1_4;
			nor->read_opcode = SPINOR_OP_READ_1_1_4;
			nor->read_dummy = 8;
	} else if (modes->rd_modes & SNOR_MODE_1_1_2 &&
			info->flags & SPI_NOR_DUAL_READ) {
		/*
		 * This branch is spcially for some devices which can
		 * not be stated by params, but only SPI_NOR_DUAL_READ,
		 * it just supports the protocol 1_1_2.
		 */
			nor->read_proto = SNOR_PROTO_1_1_2;
			nor->read_opcode = SPINOR_OP_READ_1_1_2;
			nor->read_dummy = 8;
	} else {
		if (modes->rd_modes & SNOR_MODE_1_1_1) {
			nor->read_opcode = SPINOR_OP_READ_FAST;
			nor->read_dummy = 8;
		} else {
			nor->read_opcode = SPINOR_OP_READ;
			nor->read_dummy = 0;
		}
	}

	if (!(modes->rd_modes & (SNOR_MODE_1_1_4 | SNOR_MODE_1_4_4))) {
		ret = spi_nor_reset_pin_enable(nor, info);
		if (ret < 0) {
			dev_err(nor->dev, "Enable RESET# fail.\n");
			return ret;
		}
	}

	return 0;
}

int spi_nor_scan(struct spi_nor *nor, const char *name,
		 struct spi_nor_modes *modes)
{
	const struct spi_nor_basic_flash_parameter *params = NULL;
	const struct flash_info *info = NULL;
	struct device *dev = nor->dev;
	struct mtd_info *mtd = &nor->mtd;
	struct device_node *np = spi_nor_get_flash_node(nor);
	int ret;
	int i;

	ret = spi_nor_check(nor);
	if (ret)
		return ret;

	/* Reset SPI protocol for all commands */
	nor->erase_proto = SNOR_PROTO_1_1_1;
	nor->read_proto = SNOR_PROTO_1_1_1;
	nor->write_proto = SNOR_PROTO_1_1_1;

	if (name)
		info = spi_nor_match_id(name);
	/* Try to auto-detect if chip name wasn't specified or not found */
	if (!info) {
		dev_info(dev, "SPI Nor ID Table Version %s\n", SPI_NOR_IDS_VER);
		info = spi_nor_read_id(nor);
	}

	if (IS_ERR_OR_NULL(info))
		return -ENOENT;

	/*
	 * If caller has specified name of flash model that can normally be
	 * detected using JEDEC, let's verify it.
	 */
	if (name && info->id_len) {
		const struct flash_info *jinfo;

		jinfo = spi_nor_read_id(nor);
		if (IS_ERR(jinfo)) {
			return PTR_ERR(jinfo);
		} else if (jinfo != info) {
			/*
			 * JEDEC knows better, so overwrite platform ID. We
			 * can't trust partitions any longer, but we'll let
			 * mtd apply them anyway, since some partitions may be
			 * marked read-only, and we don't want to lose that
			 * information, even if it's not 100% accurate.
			 */
			dev_warn(dev, "found %s, expected %s\n",
				 jinfo->name, info->name);
			info = jinfo;
		}
	}
	if (info->params)
		params = info->params;

	mutex_init(&nor->lock);

#ifdef CONFIG_GOKE_SPI_BLOCK_PROTECT
	/* NOR block protection support */
	bsp_get_spi_lock_info(nor, info);
#else
	/*
	 * Atmel, SST, Intel/Numonyx, and others serial NOR tend to power up
	 * with the software protection bits set
	 */

	if (JEDEC_MFR(info) == SNOR_MFR_ATMEL ||
	    JEDEC_MFR(info) == SNOR_MFR_INTEL ||
	    JEDEC_MFR(info) == SNOR_MFR_SST ||
	    info->flags & SPI_NOR_HAS_LOCK) {
		write_enable(nor);
		write_sr(nor, 0);
		spi_nor_wait_till_ready(nor);
	}
#endif

	if (!mtd->name)
		mtd->name = dev_name(dev);
	mtd->priv = nor;
	mtd->type = MTD_NORFLASH;
	mtd->writesize = 1;
	mtd->flags = MTD_CAP_NORFLASH;
	mtd->size = info->sector_size * info->n_sectors;
	mtd->_erase = spi_nor_erase;
	mtd->_read = spi_nor_read;

	/* NOR protection support for STmicro/Micron chips and similar */
	if (JEDEC_MFR(info) == SNOR_MFR_MICRON ||
	    JEDEC_MFR(info) == SNOR_MFR_WINBOND ||
	    JEDEC_MFR(info) == SNOR_MFR_XTX ||
	    JEDEC_MFR(info) == SNOR_MFR_FM ||
		info->flags & SPI_NOR_HAS_LOCK) {
		nor->flash_lock = stm_lock;
		nor->flash_unlock = stm_unlock;
		nor->flash_is_locked = stm_is_locked;
	}

	if (nor->flash_lock && nor->flash_unlock && nor->flash_is_locked) {
		mtd->_lock = spi_nor_lock;
		mtd->_unlock = spi_nor_unlock;
		mtd->_is_locked = spi_nor_is_locked;
	}

	/* sst nor chips use AAI word program */
	if (info->flags & SST_WRITE)
		mtd->_write = sst_write;
	else
		mtd->_write = spi_nor_write;

	if (info->flags & USE_FSR)
		nor->flags |= SNOR_F_USE_FSR;
	if (info->flags & SPI_NOR_HAS_TB)
		nor->flags |= SNOR_F_HAS_SR_TB;

#ifdef CONFIG_MTD_SPI_NOR_USE_4K_SECTORS
	/* prefer "small sector" erase if possible */
	if (info->flags & SECT_4K) {
		nor->erase_opcode = SPINOR_OP_BE_4K;
		mtd->erasesize = 4096;
	} else if (info->flags & SECT_4K_PMC) {
		nor->erase_opcode = SPINOR_OP_BE_4K_PMC;
		mtd->erasesize = 4096;
	} else
#endif
	{
		nor->erase_opcode = SPINOR_OP_SE;
		mtd->erasesize = info->sector_size;
	}

	if (info->flags & SPI_NOR_NO_ERASE)
		mtd->flags |= MTD_NO_ERASE;

	mtd->dev.parent = dev;
	nor->page_size = info->page_size;
	mtd->writebufsize = nor->page_size;

	if (np) {
		/* If we were instantiated by DT, use it */
		if (of_property_read_bool(np, "m25p,fast-read"))
			modes->rd_modes |= SNOR_MODE_1_1_1;
		else
			modes->rd_modes &= ~SNOR_MODE_1_1_1;
	} else {
		/* If we weren't instantiated by DT, default to fast-read */
		modes->rd_modes |= SNOR_MODE_1_1_1;
	}

	/* Some devices cannot do fast-read, no matter what DT tells us */
	if (info->flags & SPI_NOR_NO_FR)
		modes->rd_modes &= ~SNOR_MODE_1_1_1;

	nor->program_opcode = SPINOR_OP_PP;

	/*
	 * Configure the SPI memory:
	 * - select op codes for (Fast) Read, Page Program and Sector Erase.
	 * - set the number of dummy cycles (mode cycles + wait states).
	 * - set the SPI protocols for register and memory accesses.
	 * - set the Quad Enable bit if needed (required by SPI x-y-4 protos).
	 */
	ret = spi_nor_config(nor, info, params, modes);
	if (ret)
		return ret;

	if (info->addr_width)
		nor->addr_width = info->addr_width;
	else if (mtd->size > 0x1000000) {
		/* enable 4-byte addressing if the device exceeds 16MiB */
		nor->addr_width = 4;
		if (JEDEC_MFR(info) == SNOR_MFR_SPANSION ||
		    info->flags & SPI_NOR_4B_OPCODES)
			spi_nor_set_4byte_opcodes(nor, info);
		else
			set_4byte(nor, info, 1);
	} else {
		nor->addr_width = 3;
	}

	/* choose the suitable clockrate */
	if ((info->flags & (SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)) /* device supports dual or quad */
		&& (modes->rd_modes & (~SNOR_MODE_SLOW)) /* controller supports fast mode */
		&& info->clkrate)
		nor->clkrate = info->clkrate;
	else
		nor->clkrate = 24000000;

	if (nor->addr_width > SPI_NOR_MAX_ADDR_WIDTH) {
		dev_err(dev, "address width is too large: %u\n",
			nor->addr_width);
		return -EINVAL;
	}

	dev_info(dev, "%s (Chipsize %lld Mbytes, Blocksize %uKiB)\n",
		info->name, (long long)mtd->size >> 20, mtd->erasesize / 1024);

	dev_dbg(dev,
		"mtd .name = %s, .size = 0x%llx (%lldMiB), "
		".erasesize = 0x%.8x (%uKiB) .numeraseregions = %d\n",
		mtd->name, (long long)mtd->size, (long long)(mtd->size >> 20),
		mtd->erasesize, mtd->erasesize / 1024, mtd->numeraseregions);

	if (mtd->numeraseregions)
		for (i = 0; i < mtd->numeraseregions; i++)
			dev_dbg(dev,
				"mtd.eraseregions[%d] = { .offset = 0x%llx, "
				".erasesize = 0x%.8x (%uKiB), "
				".numblocks = %d }\n",
				i, (long long)mtd->eraseregions[i].offset,
				mtd->eraseregions[i].erasesize,
				mtd->eraseregions[i].erasesize / 1024,
				mtd->eraseregions[i].numblocks);
	return 0;
}
EXPORT_SYMBOL_GPL(spi_nor_scan);

static const struct flash_info *spi_nor_match_id(const char *name)
{
	const struct flash_info *id = spi_nor_ids;

	while (id->name) {
		if (!strcmp(name, id->name))
			return id;
		id++;
	}
	return NULL;
}

/******************************************************************************/
void spi_nor_driver_shutdown(struct spi_nor *nor)
{
	/* disable 4-byte addressing if the device exceeds 16MiB */
	if (nor->addr_width == 4) {
		const struct flash_info *info = NULL;

		info = spi_nor_read_id(nor);
		set_4byte(nor, info, 0);
	}
	return;
}

#ifdef CONFIG_PM
/******************************************************************************/
int spi_nor_suspend(struct spi_nor *nor, pm_message_t state)
{
	return spi_nor_wait_till_ready(nor);
}

/******************************************************************************/
int spi_nor_resume(struct spi_nor *nor)
{
	int ret;
	const struct flash_info *info = NULL;
	const struct spi_nor_basic_flash_parameter *params = NULL;
	struct spi_nor_modes modes = {
		.rd_modes = SNOR_MODE_SLOW,
		.wr_modes = SNOR_MODE_1_1_1,
	};

	modes.rd_modes |= SNOR_MODE_1_1_1
			| SNOR_MODE_1_1_2
			| SNOR_MODE_1_2_2;
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
	modes.rd_modes |= SNOR_MODE_1_1_4 | SNOR_MODE_1_4_4;
	modes.wr_modes |= SNOR_MODE_1_1_4 | SNOR_MODE_1_4_4;
#endif

	if (!info)
		info = spi_nor_read_id(nor);

	/* Quad mode takes precedence over fast/normal */
	if (info->params)
		params = info->params;

	ret = spi_nor_config(nor, info, params, &modes);
	if (ret)
		return ret;

	/* enable 4-byte addressing if the device exceeds 16MiB */
	if (nor->addr_width == 4 && JEDEC_MFR(info) != SNOR_MFR_SPANSION)
		set_4byte(nor, info, 1);

	return 0;
}
#endif /* End of CONFIG_PM */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huang Shijie <shijie8@gmail.com>");
MODULE_AUTHOR("Mike Lavender");
MODULE_DESCRIPTION("framework for SPI NOR");
