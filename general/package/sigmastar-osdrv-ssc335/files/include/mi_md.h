/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef __MI_MD_H__
#define __MI_MD_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _MI_MD_RET_E
{
    MI_MD_RET_SUCCESS                       = 0x00000000,    /*MD API execution success*/
    MI_MD_RET_INIT_ERROR                    = 0x10000401,   /*MD init error*/
    MI_MD_RET_IC_CHECK_ERROR                = 0x10000402,   /*Incorrect platform check for MD*/
    MI_MD_RET_INVALID_HANDLE                = 0x10000403,   /*Invalid MD handle*/
    MI_MD_RET_INVALID_PARAMETER             = 0x10000404,   /*Invalid MD parameter*/
    MI_MD_RET_MALLOC_ERROR                  = 0x10000405,   /*Allocate MD workiung buffer error*/

} MI_MD_RET;

typedef void* MD_HANDLE;

typedef enum MDMB_MODE_E
{
    MDMB_MODE_MB_4x4      = 0x0,
    MDMB_MODE_MB_8x8      = 0x1,
    MDMB_MODE_MB_16x16    = 0x2,
    MDMB_MODE_BUTT
} MDMB_MODE_e;

typedef enum MDSAD_OUT_CTRL_E
{
    MDSAD_OUT_CTRL_16BIT_SAD   = 0x0,
    MDSAD_OUT_CTRL_8BIT_SAD    = 0x1,
    MDSAD_OUT_CTRL_BUTT
} MDSAD_OUT_CTRL_e;

typedef enum MDALG_MODE_E
{
    MDALG_MODE_FG         = 0x0,
    MDALG_MODE_SAD        = 0x1,
    MDALG_MODE_FRAMEDIFF  = 0x2,
    MDALG_MODE_BUTT
} MDALG_MODE_e;

typedef struct MDCCL_ctrl_s
{
    uint16_t u16InitAreaThr;
    uint16_t u16Step;
} MDCCL_ctrl_t;

typedef struct MDPreproc_ctrl_s
{
    uint16_t u16Md_rgn_size;
    uint16_t u16Align;
} MDPreproc_ctrl_t;

typedef struct MDblock_info_s
{
    uint16_t st_x;
    uint16_t st_y;
    uint16_t end_x;
    uint16_t end_y;
} MDblock_info_t;

typedef struct MDPoint_s
{
    uint16_t x;
    uint16_t y;
} MDPoint_t;

typedef struct MDROI_s
{
    uint8_t num;
    MDPoint_t pnt[8];
} MDROI_t;

typedef struct MDSAD_DATA_s
{
    void *paddr;
    uint32_t stride;
    MDSAD_OUT_CTRL_e enOutCtrl;

} MDSAD_DATA_t;

typedef struct MDOBJ_s
{
    uint32_t u32Area;
    uint16_t u16Left;
    uint16_t u16Right;
    uint16_t u16Top;
    uint16_t u16Bottom;
} MDOBJ_t;

typedef struct MDOBJ_DATA_s
{
    uint8_t u8RegionNum;
    MDOBJ_t *astRegion;
    uint8_t indexofmaxobj;
    uint32_t areaofmaxobj;
    uint32_t areaoftotalobj;

} MDOBJ_DATA_t;

typedef struct MI_MD_IMG_s
{
    void *pu32PhyAddr;
    uint8_t *pu8VirAddr;
} MI_MD_IMG_t;

typedef struct MI_MD_static_param_s
{
    uint16_t width;
    uint16_t height;
    uint8_t color;
    uint32_t stride;
    MDMB_MODE_e mb_size;
    MDSAD_OUT_CTRL_e sad_out_ctrl;
    MDROI_t roi_md;
    MDALG_MODE_e md_alg_mode;
} MI_MD_static_param_t;

typedef struct MI_MD_param_s
{
    uint8_t sensitivity;
    uint16_t learn_rate;
    uint32_t md_thr;
    uint32_t obj_num_max;
    uint8_t LSD_open;
} MI_MD_param_t;

uint32_t MI_MD_GetLibVersion();
MD_HANDLE MI_MD_Init(MI_MD_static_param_t *static_param, MI_MD_param_t *param);
void MI_MD_Uninit(MD_HANDLE handle);
int32_t MI_MD_Run(MD_HANDLE handle, const MI_MD_IMG_t* pImage);
MI_MD_RET MI_MD_Preproc(MD_HANDLE handle, const MI_MD_IMG_t* pImage0, const MI_MD_IMG_t* pImage1, MDPreproc_ctrl_t* PpCtrl, MDSAD_DATA_t *sad_data, MDblock_info_t *md_region);

MI_MD_RET MI_MD_SetParam(MD_HANDLE handle, MI_MD_param_t *param);
MI_MD_RET MI_MD_GetParam(MD_HANDLE handle, MI_MD_param_t *param);
MI_MD_RET MI_MD_GetMBResult(MD_HANDLE handle, uint8_t *result_img, uint32_t *result_len);

MI_MD_RET MI_MD_ComputeImageSAD(MD_HANDLE handle, const MI_MD_IMG_t* pImage, MDSAD_DATA_t *sad_data);
MI_MD_RET MI_MD_CCL(MD_HANDLE handle, MDCCL_ctrl_t* pCclCtrl, MDOBJ_DATA_t *ccobj);
MI_MD_RET MI_MD_GetMotionCnt(MD_HANDLE handle, uint32_t *obj_cnt);
MI_MD_RET MI_MD_LightSwitchDetect(MD_HANDLE handle, const MI_MD_IMG_t* pImage, MDOBJ_DATA_t *ccobj);

void MI_MD_SetTime(MD_HANDLE handle, uint32_t time_diff);

#ifdef __cplusplus
}
#endif

#endif /* __MI_MD_H__ */
