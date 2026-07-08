/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hifb.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 
  Description   :
  History       :
  1.Date        : 2009/03/12
    Author      : 
    Modification: Created file
******************************************************************************/

 
#ifndef __HIFB_H__
#define __HIFB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include <linux/fb.h>
/*************************** Structure Definition ****************************/

#define IOC_TYPE_HIFB       'F'
/** To obtain the colorkey of an overlay layer */
#define FBIOGET_COLORKEY_HIFB       _IOR(IOC_TYPE_HIFB, 90, HIFB_COLORKEY_S)
/** To set the colorkey of an overlay layer */
#define FBIOPUT_COLORKEY_HIFB       _IOW(IOC_TYPE_HIFB, 91, HIFB_COLORKEY_S)
/** To get the alpha of an overlay layer */
#define FBIOGET_ALPHA_HIFB          _IOR(IOC_TYPE_HIFB, 92, HIFB_ALPHA_S)
/** To set the alpha of an overlay layer */
#define FBIOPUT_ALPHA_HIFB          _IOW(IOC_TYPE_HIFB, 93, HIFB_ALPHA_S)
/** To get the origin of an overlay layer on the screen */
#define FBIOGET_SCREEN_ORIGIN_HIFB  _IOR(IOC_TYPE_HIFB, 94, HIFB_POINT_S)
/** To set the origin of an overlay layer on the screen */
#define FBIOPUT_SCREEN_ORIGIN_HIFB  _IOW(IOC_TYPE_HIFB, 95, HIFB_POINT_S)
/** To obtain the anti-flicker setting of an overlay layer */
#define FBIOGET_DEFLICKER_HIFB       _IOR(IOC_TYPE_HIFB, 98, HIFB_DEFLICKER_S)
/** To set the anti-flicker setting of an overlay layer */
#define FBIOPUT_DEFLICKER_HIFB       _IOW(IOC_TYPE_HIFB, 99, HIFB_DEFLICKER_S)
/** To wait for the vertical blanking region of an overlay layer */
#define FBIOGET_VBLANK_HIFB         _IO(IOC_TYPE_HIFB, 100)
/** To set the display state of an overlay layer */
#define FBIOPUT_SHOW_HIFB           _IOW(IOC_TYPE_HIFB, 101, HI_BOOL)
/** To obtain the display state of an overlay layer */
#define FBIOGET_SHOW_HIFB           _IOR(IOC_TYPE_HIFB, 102, HI_BOOL)
/** to obtain the capability of an overlay layer */
#define FBIOGET_CAPABILITY_HIFB    _IOR(IOC_TYPE_HIFB, 103, HIFB_CAPABILITY_S)
/** set the screen output size */
#define FBIOPUT_SCREENSIZE          _IOW(IOC_TYPE_HIFB, 130, HIFB_SIZE_S*)
/** get the screen output size */
#define FBIOGET_SCREENSIZE          _IOR(IOC_TYPE_HIFB, 131, HIFB_SIZE_S*)

/** To display multiple surfaces in turn and set the alpha and colorkey attributes */
#define FBIOFLIP_SURFACE            _IOW(IOC_TYPE_HIFB, 132, HIFB_SURFACEEX_S)

/**To set the compression function status of an overlay layer*/
#define FBIOPUT_COMPRESSION_HIFB	_IOW(IOC_TYPE_HIFB, 133, HI_BOOL)
/**To obtain the compression function status of an overlay layer*/
#define FBIOGET_COMPRESSION_HIFB	_IOR(IOC_TYPE_HIFB, 134, HI_BOOL)


typedef struct
{
    HI_U32  u32Width;         
    HI_U32  u32Height;        
}HIFB_SIZE_S;

static inline HI_U8  hifb_rgb(const struct fb_bitfield* pBit, HI_S32 color)
{
    return ((HI_U8)((((HI_U32)color)>>pBit->offset) << (8-pBit->length)) +
             ((HI_U8)(((HI_U32)(color)>>pBit->offset) << (8-pBit->length)) >> pBit->length));
}

static inline HI_S32 hifb_color2key(const struct fb_var_screeninfo* pVar, HI_S32 color)
{
   if (pVar->bits_per_pixel <= 8)
   {
       return color; 
   }
   else
   {
      HI_U8 r, g, b;
      r = hifb_rgb(&pVar->red, color);
      g = hifb_rgb(&pVar->green, color);      
      b = hifb_rgb(&pVar->blue, color);
      return (r<<16) + (g<<8) + b;
   }
}

typedef struct
{
    HI_BOOL bKeyEnable;         /* colorkey enable flag */
    HI_U32 u32Key;              /* colorkey value, maybe contains alpha */
}HIFB_COLORKEY_S;

typedef struct
{
    HI_S32 x, y;    
    HI_S32 w, h;    
} HIFB_RECT;

typedef struct
{
    HI_S32 s32XPos;         /**<  horizontal position */
    HI_S32 s32YPos;         /**<  vertical position */
}HIFB_POINT_S;

typedef struct hiHIFB_DEFLICKER_S
{
    HI_U32  u32HDfLevel;    /**<  horizontal deflicker level */     
    HI_U32  u32VDfLevel;    /**<  vertical deflicker level */
    HI_U8   *pu8HDfCoef;    /**<  horizontal deflicker coefficient */
    HI_U8   *pu8VDfCoef;    /**<  vertical deflicker coefficient */
}HIFB_DEFLICKER_S;

/** Alpha info */
typedef struct
{
    HI_BOOL bAlphaEnable;   /**<  alpha enable flag */
    HI_BOOL bAlphaChannel;  /**<  alpha channel enable flag */
    HI_U8 u8Alpha0;         /**<  alpha0 value, used in ARGB1555 */
    HI_U8 u8Alpha1;         /**<  alpha1 value, used in ARGB1555 */
    HI_U8 u8GlobalAlpha;    /**<  global alpha value */
    HI_U8 u8Reserved;
}HIFB_ALPHA_S;

typedef enum
{        
    HIFB_FMT_RGB565 = 0,        
    HIFB_FMT_RGB888,		      /**<  RGB888 24bpp */
	
    HIFB_FMT_KRGB444,       /**<  RGB444 16bpp */
    HIFB_FMT_KRGB555,       /**<  RGB555 16bpp */
    HIFB_FMT_KRGB888,       /**<  RGB888 32bpp */
    
    HIFB_FMT_ARGB4444,      /**< ARGB4444 */     
    HIFB_FMT_ARGB1555,      /**< ARGB1555 */  
    HIFB_FMT_ARGB8888,      /**< ARGB8888 */   
    HIFB_FMT_ARGB8565,      /**< ARGB8565 */
	
    HIFB_FMT_RGBA4444,      /**< ARGB4444 */
    HIFB_FMT_RGBA5551,      /**< RGBA5551 */
    HIFB_FMT_RGBA5658,      /**< RGBA5658 */
    HIFB_FMT_RGBA8888,      /**< RGBA8888 */
    
    HIFB_FMT_BGR565,        /**< BGR565 */   
    HIFB_FMT_BGR888,        /**< BGR888 */   
    HIFB_FMT_ABGR4444,      /**< ABGR4444 */   
    HIFB_FMT_ABGR1555,      /**< ABGR1555 */   
    HIFB_FMT_ABGR8888,      /**< ABGR8888 */   
    HIFB_FMT_ABGR8565,      /**< ABGR8565 */      
    HIFB_FMT_KBGR444,       /**< BGR444 16bpp */
    HIFB_FMT_KBGR555,       /**< BGR555 16bpp */
    HIFB_FMT_KBGR888,       /**< BGR888 32bpp */
    
    HIFB_FMT_1BPP,          /**<  clut1 */
    HIFB_FMT_2BPP,          /**<  clut2 */    
    HIFB_FMT_4BPP,          /**<  clut4 */ 
    HIFB_FMT_8BPP,          /**< clut8 */
    HIFB_FMT_ACLUT44,       /**< AClUT44*/
    HIFB_FMT_ACLUT88,         /**< ACLUT88 */
    HIFB_FMT_PUYVY,         /**< UYVY */
    HIFB_FMT_PYUYV,         /**< YUYV */
    HIFB_FMT_PYVYU,         /**< YVYU */
    HIFB_FMT_YUV888,        /**< YUV888 */
    HIFB_FMT_AYUV8888,      /**< AYUV8888 */
    HIFB_FMT_YUVA8888,      /**< YUVA8888 */
    HIFB_FMT_BUTT
}HIFB_COLOR_FMT_E;

typedef struct
{
    HI_BOOL bKeyRgb;
    HI_BOOL bKeyAlpha;      /**<  whether support colorkey alpha */
    HI_BOOL bGlobalAlpha;   /**<  whether support global alpha */
    HI_BOOL bCmap;          /**<  whether support color map */
    HI_BOOL bHasCmapReg;    /**<  whether has color map register*/
    HI_BOOL bColFmt[HIFB_FMT_BUTT]; /**<  support which color format */
    HI_BOOL bVoScale;       /**< support vo scale*/
    HI_BOOL bLayerSupported;    /**< whether support a certain layer, for example:x5 HD support HIFB_SD_0 not support HIFB_SD_1*/
    HI_U32  u32MaxWidth;    /**<  the max pixels per line */
    HI_U32  u32MaxHeight;   /**<  the max lines */
    HI_U32  u32MinWidth;    /**<  the min pixels per line */
    HI_U32  u32MinHeight;   /**<  the min lines */ 
    HI_U32  u32VDefLevel;   /**<  vertical deflicker level, 0 means vertical deflicker is unsupported */
    HI_U32  u32HDefLevel;   /**<  horizontal deflicker level, 0 means horizontal deflicker is unsupported  */
    HI_BOOL  bDcmp;
    HI_BOOL  bPreMul;
}HIFB_CAPABILITY_S;

/*refresh mode*/
typedef enum 
{
    HIFB_LAYER_BUF_DOUBLE = 0x0,       /**<  2 display buf in fb */
    HIFB_LAYER_BUF_ONE    = 0x1,       /**<  1 display buf in fb */
    HIFB_LAYER_BUF_NONE   = 0x2,       /**<  no display buf in fb,the buf user refreshed will be directly set to VO*/    
    HIFB_LAYER_BUF_DOUBLE_IMMEDIATE=0x3, /**< 2 display buf in fb, each refresh will be displayed*/
    HIFB_LAYER_BUF_BUTT
} HIFB_LAYER_BUF_E;

/* surface info */
typedef struct
{
    HI_U32  u32PhyAddr;     /**<  start physical address */
    HI_U32  u32Width;       /**<  width pixels */
    HI_U32  u32Height;      /**<  height pixels */   
    HI_U32  u32Pitch;       /**<  line pixels */         
    HIFB_COLOR_FMT_E enFmt; /**<  color format */     
}HIFB_SURFACE_S;

typedef struct
{
    HI_U32 u32PhyAddr;
    HIFB_ALPHA_S stAlpha;
    HIFB_COLORKEY_S stColorkey;
}HIFB_SURFACEEX_S;

/* refresh surface info */
typedef struct
{
    HIFB_SURFACE_S stCanvas;    
    HIFB_RECT UpdateRect;       /* refresh region*/
}HIFB_BUFFER_S;

/* cursor info */
typedef struct
{
    HIFB_SURFACE_S stCursor;
    HIFB_POINT_S stHotPos;
} HIFB_CURSOR_S;

/* crusor handle */
/* Attention:surface in cursor will be released by user*/
#define FBIOPUT_CURSOR_INFO		      _IOW(IOC_TYPE_HIFB, 104, HIFB_CURSOR_S *)
#define FBIOGET_CURSOR_INFO		      _IOW(IOC_TYPE_HIFB, 105, HIFB_CURSOR_S *)

#define FBIOPUT_CURSOR_STATE		  _IOW(IOC_TYPE_HIFB, 106, HI_BOOL *)
#define FBIOGET_CURSOR_STATE		  _IOW(IOC_TYPE_HIFB, 107, HI_BOOL *)

#define FBIOPUT_CURSOR_POS		      _IOW(IOC_TYPE_HIFB, 108, HIFB_POINT_S *)	
#define FBIOGET_CURSOR_POS		      _IOR(IOC_TYPE_HIFB, 109, HIFB_POINT_S *)

#define FBIOPUT_CURSOR_COLORKEY       _IOR(IOC_TYPE_HIFB, 110, HIFB_COLORKEY_S *)
#define FBIOGET_CURSOR_COLORKEY       _IOW(IOC_TYPE_HIFB, 111, HIFB_COLORKEY_S *)
#define FBIOPUT_CURSOR_ALPHA          _IOR(IOC_TYPE_HIFB, 112, HIFB_ALPHA_S *)
#define FBIOGET_CURSOR_ALPHA          _IOW(IOC_TYPE_HIFB, 113, HIFB_ALPHA_S *)

/** cursor will be separated from attached layer automatically if you attach cursor to another layer,that means
cursor can be attached to only one layer at any time*/
#define FBIOPUT_CURSOR_ATTCHCURSOR    _IOW(IOC_TYPE_HIFB, 114, HI_U32 *)
#define FBIOPUT_CURSOR_DETACHCURSOR   _IOW(IOC_TYPE_HIFB, 115, HI_U32 *)

/**antiflicker level*/
/**Auto means fb will choose a appropriate antiflicker level automatically according to the color info of map*/
typedef enum
{
    HIFB_LAYER_ANTIFLICKER_NONE = 0x0,	/**< no antiflicker*/
    HIFB_LAYER_ANTIFLICKER_LOW = 0x1,	/**< low level*/
    HIFB_LAYER_ANTIFLICKER_MIDDLE = 0x2,/**< middle level*/
    HIFB_LAYER_ANTIFLICKER_HIGH = 0x3, /**< high level*/
    HIFB_LAYER_ANTIFLICKER_AUTO = 0x4, /**< auto*/
    HIFB_LAYER_ANTIFLICKER_BUTT
}HIFB_LAYER_ANTIFLICKER_LEVEL_E;

/*layer info maskbit*/
typedef enum
{
    HIFB_LAYERMASK_BUFMODE = 0x1,           /**< BUFMODE bitmask */
    HIFB_LAYERMASK_ANTIFLICKER_MODE = 0x2,  /**< ANTIFLICKER_MODE bitmask */
    HIFB_LAYERMASK_POS = 0x4,               /**< the position bitmask */
    HIFB_LAYERMASK_CANVASSIZE = 0x8,      /**< canvassize bitmask */
    HIFB_LAYERMASK_DISPSIZE = 0x10,       /**< displaysize bitmask */
    HIFB_LAYERMASK_SCREENSIZE = 0x20,     /**< screensize bitmask */
    HIFB_LAYERMASK_BMUL = 0x40,           /**< pre-mult bitmask */
    HIFB_LAYERMASK_BUTT
}HIFB_LAYER_INFO_MASKBIT;

/**layer info*/
typedef struct
{
    HIFB_LAYER_BUF_E BufMode;
    HIFB_LAYER_ANTIFLICKER_LEVEL_E eAntiflickerLevel;	
    HI_S32 s32XPos;           /**<  the x pos of origion point in screen */
    HI_S32 s32YPos;	          /**<  the y pos of origion point in screen */
    HI_S32 u32CanvasWidth;    /**<  the width of canvas buffer */
    HI_S32 u32CanvasHeight;   /**<  the height of canvas buffer */
    HI_U32 u32DisplayWidth;	  /**<  the width of display buf in fb.for 0 buf ,there is no display buf in fb, so it's effectless*/
    HI_U32 u32DisplayHeight;  /**<  the height of display buf in fb. */
    HI_U32 u32ScreenWidth;    /**<  the width of screen */
    HI_U32 u32ScreenHeight;   /**<  the height of screen */
    HI_BOOL bPreMul;          /**<  The data drawed in buf is premul data or not*/
    HI_U32 u32Mask;			  /**<  param modify mask bit*/
}HIFB_LAYER_INFO_S;

/** To set the layer information */
#define FBIOPUT_LAYER_INFO                _IOW(IOC_TYPE_HIFB, 120, HIFB_LAYER_INFO_S*)
/** To get the layer information */
#define FBIOGET_LAYER_INFO                _IOR(IOC_TYPE_HIFB, 121, HIFB_LAYER_INFO_S*)
/** To get canvas buf */
#define FBIOGET_CANVAS_BUFFER             _IOR(IOC_TYPE_HIFB, 123, HIFB_BUFFER_S*) 
/** To refresh the displayed contents in extended mode */
#define FBIO_REFRESH                      _IOW(IOC_TYPE_HIFB, 124, HIFB_BUFFER_S*) 

/**sync refresh*/
#define FBIO_WAITFOR_FREFRESH_DONE        _IO(IOC_TYPE_HIFB, 125)



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HIFB_H__ */

