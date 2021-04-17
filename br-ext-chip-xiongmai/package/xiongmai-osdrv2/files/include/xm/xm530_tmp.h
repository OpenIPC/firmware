#ifndef _XM530_TMP_H_
#define _XM530_TMP_H_
#include "xm_type.h"
#include "xm_defines.h"
#include "xm_i2c.h"


#define VI_BASE		(0x30000000)


#define VI_H_BEGIN	(VI_BASE+0x098)
#define VI_H_END		(VI_BASE+0x09C)
#define VI_V_BEGIN	(VI_BASE+0x0A0)
#define VI_V_END		(VI_BASE+0x0A4)

/**For read the configinit  */
#define  FILENAME_NOTEXIST      0x02 /*配置文件名不存在*/  
#define  SECTIONNAME_NOTEXIST    0x03 /*节名不存在*/  
#define  KEYNAME_NOTEXIST      0x04 /*键名不存在*/  
#define  STRING_LENNOTEQUAL     0x05 /*两个字符串长度不同*/  
#define  STRING_NOTEQUAL       0x06 /*两个字符串内容不相同*/  
#define  STRING_EQUAL        0x00 /*两个字符串内容相同*/  
	
#define  KEYNUM 2
#define  NCNUM 32



XM_S32 Write_IspRegSys(XM_U32 u32Addr, XM_U32 u32Value);
XM_S32 Read_IspRegSys(XM_U32 u32Addr);

void SysDelay_ms(XM_S32 ms);
XM_S32 SysGetProductInfo(XM_PRODUCT_INFO *pstProductInfo);
XM_S32 SysSetProductInfo(XM_PRODUCT_INFO *pstProductInfo);
int XM_I2C_Ioctl(int cmd, I2C_DATA_S *pstI2CData);
XM_S32 VI_WinSet(XM_U8 u8Mode, XM_U8 u8Mirror, XM_U8 u8Flip,
						XM_U16 u16ValH, XM_U16 u16ValV);
void PrintInt(unsigned char u8Num,int u32Data);
void PrintHex(unsigned char u8Num,int u32Data);
XM_U8 putstr(XM_U8 uart_NO, char const  *str) ;
void SysFlagSet(XM_U32 u32Flag);
#endif

