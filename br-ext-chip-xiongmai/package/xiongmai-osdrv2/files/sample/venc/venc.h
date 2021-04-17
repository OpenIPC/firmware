/******************************************************************************
  xm510 sample programs head file.


******************************************************************************/

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_region.h"
#include "mpi_sys.h"
//#include "mpi_vda.h"
#include "mpi_venc.h"
#include "mpi_vi.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

/********************************************************
*    venc
********************************************************/
typedef enum sample_rc_e
{
	SAMPLE_RC_CBR = 0,
	SAMPLE_RC_VBR,
	SAMPLE_RC_FIXQP
}SAMPLE_RC_E;

/// 捕获分辨率类型
enum capture_size_t {
	CAPTURE_SIZE_D1,		///< 720*576(PAL)	720*480(NTSC)
	CAPTURE_SIZE_HD1,		///< 352*576(PAL)	352*480(NTSC)
	CAPTURE_SIZE_BCIF,		///< 720*288(PAL)	720*240(NTSC)
	CAPTURE_SIZE_CIF,		///< 352*288(PAL)	352*240(NTSC)
	CAPTURE_SIZE_QCIF,		///< 176*144(PAL)	176*120(NTSC)
	CAPTURE_SIZE_VGA,		///< 640*480(PAL)	640*480(NTSC)
	CAPTURE_SIZE_QVGA,		///< 320*240(PAL)	320*240(NTSC)
	CAPTURE_SIZE_SVCD,		///< 480*480(PAL)	480*480(NTSC)
	CAPTURE_SIZE_QQVGA,		///< 160*128(PAL)	160*128(NTSC)
	CAPTURE_SIZE_NR = 9,	///< 等临时处理等底层一起修改
	CAPTURE_SIZE_ND1 = 9,   ///< 240*192
	CAPTURE_SIZE_650TVL,    ///< 928*576
	CAPTURE_SIZE_720P,      ///< 1280*720
	CAPTURE_SIZE_1_3M,      ///< 1280*960
	CAPTURE_SIZE_UXGA ,     ///< 1600*1200
	CAPTURE_SIZE_1080P,     ///< 1920*1080
	CAPTURE_SIZE_WUXGA,     ///< 1920*1200
	CAPTURE_SIZE_2_5M,      ///< 1872*1408
	CAPTURE_SIZE_3M,        ///< 2048*1536
	CAPTURE_SIZE_5M,        ///< 3744*1408
	CAPTURE_SIZE_EXT_NR =19,    ///< 枚举的图形大小种类的数目。
	CAPTURE_SIZE_1080N = 19,     ///< 960*1080
	CAPTURE_SIZE_4M,        ///< 2592*1520
	CAPTURE_SIZE_6M,		///< 3072×2048
	CAPTURE_SIZE_8M,		///< 3264×2448
	CAPTURE_SIZE_12M,		///< 4000*3000
	CAPTURE_SIZE_4K,		///< 4096 * 2160通用/3840*2160海思
	CAPTURE_SIZE_EXT_V2_NR=25,    ///< 枚举的图形大小种类的数目。
	CAPTURE_SIZE_720N = 25,  //  640*720
	CAPTURE_SIZE_RES1,       // 0*0 冗余
	CAPTRUE_SIZE_RES2,       // 0*0
	CAPTURE_SIZE_RES3,       // 0*0
	CAPTURE_SIZE_RES4,       // 0*0
	CAPTURE_SIZE_RES5,       // 0*0
	CAPTURE_SIZE_RES6,       // 0*0
	CAPTURE_SIZE_EXT_V3_NR,
};


typedef struct sample_venc_getstream_s
{
     XM_BOOL bThreadStart;
     XM_S32  s32Cnt;
}SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct tagCAPTURE_TITLE_PARAM
{
	XM_U16	x;
	XM_U16	y;		 
	XM_U16	width;
	XM_U16	height;
	XM_U32	fg_color;					/*!< text color, rgba 8:8:8:8 */
	XM_U32	bg_color;					/*!< background color, rgba 8:8:8:8 */
	XM_U8 	*pixel;
}CAPTURE_TITLE_PARAM;

typedef struct tagCAPTURE_TITLE_PARAM_V2
{
	XM_S32 index;
	XM_BOOL enable;
	XM_U16	x;
	XM_U16	y;		 
	XM_U16	width;
	XM_U16	height;
	XM_U32	fg_color;					/*!< text color, rgba 8:8:8:8 */
	XM_U32	bg_color;					/*!< background color, rgba 8:8:8:8 */
	XM_U8 	*pixel;
}CAPTURE_TITLE_PARAM_V2;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
