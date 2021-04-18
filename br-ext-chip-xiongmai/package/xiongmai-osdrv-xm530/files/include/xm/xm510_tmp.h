#ifndef _XM510_TMP_H_
#define _XM510_TMP_H_
#include "xm_type.h"
#include "xm_defines.h"
#ifdef SOC_SYSTEM
#include <stdio.h>
#endif


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


void SysDelay_ms(XM_S32 ms);
XM_S32 SysGetProductInfo(XM_PRODUCT_INFO *pstProductInfo);
#ifdef SOC_SYSTEM
int GetConfigIntValue(char *pInFileName,char *pInSectionName,const char (*pInKeyName)[NCNUM],char (*pOutKeyValue)[NCNUM]);  
int GetConfigStringValue(char *pInFileName,char *pInSectionName,const char (*pInKeyName)[NCNUM],char (*pOutKeyValue)[NCNUM]); 	  
int CompareString(char *pInStr1,char *pInStr2);
int GetKeyValue(FILE *fpConfig,const char (*pInKeyName)[NCNUM],char (*pOutKeyValue)[NCNUM]);
#endif
void CameraConfigInit(UPSCALER_CFG *gstvstdvp);
XM_U8 IrledConfigInit(void);
int string_get_value(const char *filename, const char *str, const char separate, char *value, char *raw);



XM_S32 VI_WinSet(XM_U8 u8Mode, XM_U8 u8Mirror, XM_U8 u8Flip,
						XM_U16 u16ValH, XM_U16 u16ValV);

#endif

