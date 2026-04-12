/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_vreg.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/01/09
  Description   : 
  History       :
  1.Date        : 2013/01/09
    Author      : n00168968
    Modification: Created file

******************************************************************************/

#ifndef __HI_VREG_H__
#define __HI_VREG_H__

#include "hi_type.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define REG_ACCESS_WIDTH_1  0
#define REG_ACCESS_WIDTH    0                /* 1: 16bit   2: 8bit */

/* Vreg is a blok of memory alloced to simulate the regs.
 * We try to differentiate vregs by baseaddr.  Each vreg
 * block's size should at least 4k bytes.
 */
 /*---------------------------------------------------------------------------------------------------------*
  *| 0x205A0000 |  0x10000  |  0x20000 |  0x21000 | ... |  0x30000  | ... |  0x40000 | ... |  0x80000 | ...|*
  *|------------|-----------|----------|----------|-----|-----------|-----|----------|-----|----------|----|*
  *|   ISP_REG  |  ISP_VREG | AE1_VREG | AE2_VREG | ... | AWB1_VREG | ... | AF1_VREG | ... | VIU_VREG | ...|*
  *---------------------------------------------------------------------------------------------------------*/
#define VREG_SIZE_ALIGN 0x1000

#define EXT_REG_BASE    0x10000

#define ISP_VREG_BASE   0x10000
#define ISP_VREG_SIZE   (VREG_SIZE_ALIGN << 4)

#define AE_LIB_VREG_BASE(id)    (0x20000 + VREG_SIZE_ALIGN * (id))
#define AWB_LIB_VREG_BASE(id)   (0x30000 + VREG_SIZE_ALIGN * (id))
#define AF_LIB_VREG_BASE(id)    (0x40000 + VREG_SIZE_ALIGN * (id))
#define ALG_LIB_VREG_SIZE       (VREG_SIZE_ALIGN)
#define MAX_ALG_LIB_VREG_NUM    (1 << 4)

#define HISI_AE_LIB_EXTREG_ID_0 0
#define HISI_AE_LIB_EXTREG_ID_1 1

#define HISI_AWB_LIB_EXTREG_ID_0 0
#define HISI_AWB_LIB_EXTREG_ID_1 1

#define VIU_VREG_BASE   0x80000
#define VPP_VREG_BASE   0x90000
#define VEDU_VREG_BASE  0xa0000
#define VOU_VREG_BASE   0xb0000

HI_S32 VReg_Init(HI_U32 u32BaseAddr, HI_U32 u32Size);
HI_S32 VReg_Exit(HI_U32 u32BaseAddr, HI_U32 u32Size);
HI_U32 VReg_GetVirtAddr(HI_U32 u32BaseAddr);
HI_VOID VReg_Munmap(HI_VOID);
HI_U32 IO_READ32(HI_U32 u32Addr);
HI_S32 IO_WRITE32(HI_U32 u32Addr, HI_U32 u32Value);
HI_U16 IO_READ16(HI_U32 u32Addr);
HI_S32 IO_WRITE16(HI_U32 u32Addr, HI_U32 u32Value);
HI_U8  IO_READ8(HI_U32 u32Addr);
HI_S32 IO_WRITE8(HI_U32 u32Addr, HI_U32 u32Value);

/* Dynamic bus access functions, 4 byte align access */
//TODO: allocate dev addr (such as ISP_REG_BASE_ADDR) according to devId.
#define __IO_CALC_ADDRESS_DYNAMIC(BASE)    (HI_U32)(((BASE >= EXT_REG_BASE) ? 0 : ISP_REG_BASE) + (BASE))

#define IORD_32DIRECT(BASE)             IO_READ32(__IO_CALC_ADDRESS_DYNAMIC(BASE))
#define IORD_16DIRECT(BASE)             IO_READ16(__IO_CALC_ADDRESS_DYNAMIC(BASE))
#define IORD_8DIRECT(BASE)              IO_READ8(__IO_CALC_ADDRESS_DYNAMIC(BASE))

#define IOWR_32DIRECT(BASE, DATA)       IO_WRITE32(__IO_CALC_ADDRESS_DYNAMIC(BASE), (DATA))
#define IOWR_16DIRECT(BASE, DATA)       IO_WRITE16(__IO_CALC_ADDRESS_DYNAMIC(BASE), (DATA))
#define IOWR_8DIRECT(BASE, DATA)        IO_WRITE8(__IO_CALC_ADDRESS_DYNAMIC(BASE), (DATA))


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
