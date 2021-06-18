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
#ifndef __MI_VENC_DATATYPE_
#define __MI_VENC_DATATYPE_

#include "mi_common_datatype.h"
#include "mi_sys_datatype.h"

#define MI_VENC_MAX_CHN_NUM_PER_MODULE (9)
#define MI_VENC_MAX_CHN_NUM_PER_DC (3)
#define VENC_CUST_MAP_NUM    (2)
#define VENC_MAX_SAD_RANGE_NUM (16)

//max supported channel number. But the number would be limited by each module.
#define VENC_MAX_CHN_NUM    (16) //from SPEC


#define MI_DEF_VENC_ERR(err) MI_DEF_ERR(E_MI_MODULE_ID_VENC, E_MI_ERR_LEVEL_ERROR, err)

#define MI_VENC_OK  MI_DEF_VENC_ERR(MI_SUCCESS)

/* invalid handle */
//#define MI_ERR_VENC_INVALID_HANDLE    MI_DEF_VENC_ERR(E_MI_ERR_INVALID_DEVID)

/* invalid device ID */
#define MI_ERR_VENC_INVALID_DEVID     MI_DEF_VENC_ERR(E_MI_ERR_INVALID_DEVID)
/* invalid channel ID */
#define MI_ERR_VENC_INVALID_CHNID     MI_DEF_VENC_ERR(E_MI_ERR_INVALID_CHNID)
/* at lease one parameter is illegal, e.g, an illegal enumeration value  */
#define MI_ERR_VENC_ILLEGAL_PARAM     MI_DEF_VENC_ERR(E_MI_ERR_ILLEGAL_PARAM)
/* channel exists */
#define MI_ERR_VENC_EXIST             MI_DEF_VENC_ERR(E_MI_ERR_EXIST)
/*UN exist*/
#define MI_ERR_VENC_UNEXIST           MI_DEF_VENC_ERR(E_MI_ERR_UNEXIST)
/* using a NULL point */
#define MI_ERR_VENC_NULL_PTR          MI_DEF_VENC_ERR(E_MI_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configuring attribute */
#define MI_ERR_VENC_NOT_CONFIG        MI_DEF_VENC_ERR(E_MI_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define MI_ERR_VENC_NOT_SUPPORT       MI_DEF_VENC_ERR(E_MI_ERR_NOT_SUPPORT)
/* operation is not permitted, e.g, try to change static attribute */
#define MI_ERR_VENC_NOT_PERM          MI_DEF_VENC_ERR(E_MI_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define MI_ERR_VENC_NOMEM             MI_DEF_VENC_ERR(E_MI_ERR_NOMEM)
/* failure caused by malloc buffer */
#define MI_ERR_VENC_NOBUF             MI_DEF_VENC_ERR(E_MI_ERR_NOBUF)
/* no data in buffer */
#define MI_ERR_VENC_BUF_EMPTY         MI_DEF_VENC_ERR(E_MI_ERR_BUF_EMPTY)
/* no buffer for new data */
#define MI_ERR_VENC_BUF_FULL          MI_DEF_VENC_ERR(E_MI_ERR_BUF_FULL)
/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define MI_ERR_VENC_NOTREADY          MI_DEF_VENC_ERR(E_MI_ERR_SYS_NOTREADY)

/* bad address, e.g. used for copy_from_user & copy_to_user */
#define MI_ERR_VENC_BADADDR           MI_DEF_VENC_ERR(E_MI_ERR_BADADDR)
/* resource is busy, e.g. destroy a VENC channel without unregistering it */
#define MI_ERR_VENC_BUSY              MI_DEF_VENC_ERR(E_MI_ERR_BUSY)

/* channel not start*/
#define MI_ERR_VENC_CHN_NOT_STARTED   MI_DEF_VENC_ERR(E_MI_ERR_CHN_NOT_STARTED)
/* channel not stop*/
#define MI_ERR_VENC_CHN_NOT_STOPPED   MI_DEF_VENC_ERR(E_MI_ERR_CHN_NOT_STOPED)

#define MI_ERR_VENC_PRIVATE_START     MI_DEF_VENC_ERR(MI_VENC_INITIAL_ERROR_CODE)
/* to be removed later */
#define MI_ERR_VENC_UNDEFINED         MI_DEF_VENC_ERR(E_MI_ERR_FAILED)

/* No error but return it's VENC no error.*/
//#define MI_ERR_VENC_NONE              MI_DEF_VENC_ERR(0)

/* =======================================================
 * Missing definitions
 * =======================================================*/
#define RC_TEXTURE_THR_SIZE     1       // Fixme

/* =======================================================
 * Missing Types
 * =======================================================*/
//FIXME add dummy fields at least
/*
#define MI_VENC_ParamModH264e_t  MI_VENC_ParamModH265e_t
#define MI_VENC_ParamModVenc_t   MI_VENC_ParamModH265e_t
*/

/* =======================================================
 * Data Types
 * =======================================================*/

//==== Enumerates ====
typedef enum
{
    E_MI_VENC_MODTYPE_VENC = 1, //!< E_MI_VENC_MODTYPE_VENC
    E_MI_VENC_MODTYPE_H264E,   //!< E_MI_VENC_MODTYPE_H264E
    E_MI_VENC_MODTYPE_H265E,   //!< E_MI_VENC_MODTYPE_H265E
    E_MI_VENC_MODTYPE_JPEGE,   //!< E_MI_VENC_MODTYPE_JPEGE
    E_MI_VENC_MODTYPE_MAX     //!< E_MI_VENC_MODTYPE_MAX
} MI_VENC_ModType_e;

typedef enum
{
    E_MI_VENC_BASE_IDR = 0,
    E_MI_VENC_BASE_P_REFTOIDR,
    E_MI_VENC_BASE_P_REFBYBASE,
    E_MI_VENC_BASE_P_REFBYENHANCE,
    E_MI_VENC_ENHANCE_P_REFBYENHANCE,
    E_MI_VENC_ENHANCE_P_NOTFORREF,
    E_MI_VENC_REF_TYPE_MAX
} MI_VENC_H264eRefType_e;
typedef MI_VENC_H264eRefType_e MI_VENC_H265eRefType_e;

typedef enum
{
    E_MI_VENC_H264E_NALU_PSLICE = 1,
    E_MI_VENC_H264E_NALU_ISLICE = 5,
    E_MI_VENC_H264E_NALU_SEI = 6,
    E_MI_VENC_H264E_NALU_SPS = 7,
    E_MI_VENC_H264E_NALU_PPS = 8,
    E_MI_VENC_H264E_NALU_IPSLICE = 9,
    E_MI_VENC_H264E_NALU_PREFIX = 14,
    E_MI_VENC_H264E_NALU_MAX
} MI_VENC_H264eNaluType_e;

typedef enum
{
    E_MI_VENC_H264E_REFSLICE_FOR_1X = 1,
    E_MI_VENC_H264E_REFSLICE_FOR_2X = 2,
    E_MI_VENC_H264E_REFSLICE_FOR_4X,
    E_MI_VENC_H264E_REFSLICE_FOR_MAX = 5
} MI_VENC_H264eRefSliceType_e;

typedef enum
{
    E_MI_VENC_JPEGE_PACK_ECS = 5,
    E_MI_VENC_JPEGE_PACK_APP = 6,
    E_MI_VENC_JPEGE_PACK_VDO = 7,
    E_MI_VENC_JPEGE_PACK_PIC = 8,
    E_MI_VENC_JPEGE_PACK_MAX
} MI_VENC_JpegePackType_e;

typedef enum
{
    E_MI_VENC_H265E_NALU_PSLICE = 1,
    E_MI_VENC_H265E_NALU_ISLICE = 19,
    E_MI_VENC_H265E_NALU_VPS = 32,
    E_MI_VENC_H265E_NALU_SPS = 33,
    E_MI_VENC_H265E_NALU_PPS = 34,
    E_MI_VENC_H265E_NALU_SEI = 39,
    E_MI_VENC_H265E_NALU_MAX
} MI_VENC_H265eNaulType_e;

typedef enum
{
    E_MI_VENC_RC_MODE_H264CBR = 1,
    E_MI_VENC_RC_MODE_H264VBR,
    E_MI_VENC_RC_MODE_H264ABR,
    E_MI_VENC_RC_MODE_H264FIXQP,
    E_MI_VENC_RC_MODE_H264AVBR,
    E_MI_VENC_RC_MODE_MJPEGCBR,
    E_MI_VENC_RC_MODE_MJPEGFIXQP,
    E_MI_VENC_RC_MODE_H265CBR,
    E_MI_VENC_RC_MODE_H265VBR,
    E_MI_VENC_RC_MODE_H265FIXQP,
    E_MI_VENC_RC_MODE_H265AVBR,
    E_MI_VENC_RC_MODE_MAX,
} MI_VENC_RcMode_e;

typedef enum
{
    E_MI_VENC_SUPERFRM_NONE,
    E_MI_VENC_SUPERFRM_DISCARD,
    E_MI_VENC_SUPERFRM_REENCODE,
    E_MI_VENC_SUPERFRM_MAX
} MI_VENC_SuperFrmMode_e;

typedef enum
{
    E_MI_VENC_H264E_IDR_PIC_ID_MODE_USR,
} MI_VENC_H264eIdrPicIdMode_e;

typedef enum
{
    E_MI_VENC_FRMLOST_NORMAL,
    E_MI_VENC_FRMLOST_PSKIP,
    E_MI_VENC_FRMLOST_MAX,
} MI_VENC_FrameLostMode_e;

typedef enum
{
    E_MI_VENC_RC_PRIORITY_BITRATE_FIRST = 1,
    E_MI_VENC_RC_PRIORITY_FRAMEBITS_FIRST,
    E_MI_VENC_RC_PRIORITY_MAX,
} MI_VENC_RcPriority_e;

typedef enum
{
    E_MI_VENC_INPUT_MODE_NORMAL_FRMBASE = 0, /*Handshake with input by about 3 buffers in frame mode*/
    E_MI_VENC_INPUT_MODE_RING_ONE_FRM, /*Handshake with input by one buffer in ring mode*/
    E_MI_VENC_INPUT_MODE_RING_HALF_FRM, /*Handshake with input by half buffer in ring mode*/
    E_MI_VENC_INPUT_MODE_MAX
} MI_VENC_InputSrcBufferMode_e;

typedef enum
{
    E_MI_VENC_MD_DET=1,
    E_MI_VENC_ROI_DET,
    E_MI_VENC_SMART_DET_MAX,
} MI_VENC_SmartDetType_e;

//==== Structures ====

typedef struct MI_VENC_Rect_s
{
    MI_U32 u32Left;
    MI_U32 u32Top;
    MI_U32 u32Width;
    MI_U32 u32Height;
} MI_VENC_Rect_t;

typedef union MI_VENC_DataType_s
{
    MI_VENC_H264eNaluType_e eH264EType;
    MI_VENC_JpegePackType_e eJPEGEType;
    MI_VENC_H265eNaulType_e eH265EType;
} MI_VENC_DataType_t;

typedef struct MI_VENC_PackInfo_s
{
    MI_VENC_DataType_t stPackType;
    MI_U32 u32PackOffset;
    MI_U32 u32PackLength;
    MI_U32 u32SliceId;
} MI_VENC_PackInfo_t;

typedef struct MI_VENC_Pack_s
{
    MI_PHY phyAddr;
    MI_U8 *pu8Addr;
    MI_U32 u32Len;
    MI_U64 u64PTS;
    MI_BOOL bFrameEnd;
    MI_VENC_DataType_t stDataType;
    MI_U32 u32Offset;
    MI_U32 u32DataNum;
    MI_VENC_PackInfo_t asackInfo[8];
} MI_VENC_Pack_t;

typedef struct MI_VENC_StreamInfoH264_s
{
    MI_U32 u32PicBytesNum;
    MI_U32 u32PSkipMbNum;
    MI_U32 u32IpcmMbNum;
    MI_U32 u32Inter16x8MbNum;
    MI_U32 u32Inter16x16MbNum;
    MI_U32 u32Inter8x16MbNum;
    MI_U32 u32Inter8x8MbNum;
    MI_U32 u32Intra16MbNum;
    MI_U32 u32Intra8MbNum;
    MI_U32 u32Intra4MbNum;
    MI_VENC_H264eRefSliceType_e eRefSliceType;
    MI_VENC_H264eRefType_e eRefType;
    MI_U32 u32UpdateAttrCnt;
    MI_U32 u32StartQp;
} MI_VENC_StreamInfoH264_t;

typedef struct MI_VENC_StreamInfoJpeg_s
{
    MI_U32 u32PicBytesNum;
    MI_U32 u32UpdateAttrCnt;
    MI_U32 u32Qfactor;
} MI_VENC_StreamInfoJpeg_t;

typedef struct MI_VENC_StreamInfoH265_s
{
    MI_U32 u32PicBytesNum;
    MI_U32 u32Inter64x64CuNum;
    MI_U32 u32Inter32x32CuNum;
    MI_U32 u32Inter16x16CuNum;
    MI_U32 u32Inter8x8CuNum;
    MI_U32 u32Intra32x32CuNum;
    MI_U32 u32Intra16x16CuNum;
    MI_U32 u32Intra8x8CuNum;
    MI_U32 u32Intra4x4CuNum;
    MI_VENC_H265eRefType_e eRefType;
    MI_U32 u32UpdateAttrCnt;
    MI_U32 u32StartQp;
} MI_VENC_StreamInfoH265_t;

typedef struct MI_VENC_Stream_s
{
    MI_VENC_Pack_t *pstPack;
    MI_U32 u32PackCount;
    MI_U32 u32Seq;
    MI_SYS_BUF_HANDLE hMiSys;
    union
    {
        MI_VENC_StreamInfoH264_t stH264Info;
        MI_VENC_StreamInfoJpeg_t stJpegInfo;
        MI_VENC_StreamInfoH265_t stH265Info;
    };
} MI_VENC_Stream_t;

typedef struct MI_VENC_StreamBufInfo_s
{
    MI_PHY phyAddr;
    void *pUserAddr;
    MI_U32 u32BufSize;
} MI_VENC_StreamBufInfo_t;

typedef struct MI_VENC_AttrH264_s
{
    MI_U32 u32MaxPicWidth;
    MI_U32 u32MaxPicHeight;
    MI_U32 u32BufSize;
    MI_U32 u32Profile;
    MI_BOOL bByFrame;
    MI_U32 u32PicWidth;
    MI_U32 u32PicHeight;
    MI_U32 u32BFrameNum;
    MI_U32 u32RefNum;
} MI_VENC_AttrH264_t;

/*
typedef struct MI_VENC_AttrMjpeg_s
{
    MI_U32 u32MaxPicWidth;
    MI_U32 u32MaxPicHeight;
    MI_U32 u32BufSize;
    MI_BOOL bByFrame;
    MI_U32 u32PicWidth;
    MI_U32 u32PicHeight;
} MI_VENC_AttrMjpeg_t;
*/

typedef struct MI_VENC_AttrJpeg_s
{
    MI_U32 u32MaxPicWidth;
    MI_U32 u32MaxPicHeight;
    MI_U32 u32BufSize;
    MI_BOOL bByFrame;
    MI_U32 u32PicWidth;
    MI_U32 u32PicHeight;
    MI_BOOL bSupportDCF;
    MI_U32 u32RestartMakerPerRowCnt;
} MI_VENC_AttrJpeg_t;

typedef struct MI_VENC_AttrH265_s
{
    MI_U32 u32MaxPicWidth;
    MI_U32 u32MaxPicHeight;
    MI_U32 u32BufSize;
    MI_U32 u32Profile;
    MI_BOOL bByFrame;
    MI_U32 u32PicWidth;
    MI_U32 u32PicHeight;
    MI_U32 u32BFrameNum;
    MI_U32 u32RefNum;
} MI_VENC_AttrH265_t;

typedef struct MI_VENC_Attr_s
{
    MI_VENC_ModType_e eType;
    union
    {
        MI_VENC_AttrH264_t stAttrH264e;
        //MI_VENC_AttrMjpeg_t stAttrMjpeg;
        MI_VENC_AttrJpeg_t stAttrJpeg;
        MI_VENC_AttrH265_t stAttrH265e;
    };
} MI_VENC_Attr_t;

typedef struct MI_VENC_ChnStat_s
{
    MI_U32 u32LeftPics;
    MI_U32 u32LeftStreamBytes;
    MI_U32 u32LeftStreamFrames;
    MI_U32 u32LeftStreamMillisec;
    MI_U32 u32CurPacks;
    MI_U32 u32LeftRecvPics;
    MI_U32 u32LeftEncPics;
    MI_U32 u32FrmRateNum;
    MI_U32 u32FrmRateDen;
    MI_U32 u32BitRate;
} MI_VENC_ChnStat_t;

typedef struct MI_VENC_ParamH264SliceSplit_s
{
    MI_BOOL bSplitEnable;
    MI_U32 u32SliceRowCount;
} MI_VENC_ParamH264SliceSplit_t;

typedef struct MI_VENC_ParamH264InterPred_s
{
    /* search window */
    MI_U32 u32HWSize;
    MI_U32 u32VWSize;
    MI_BOOL bInter16x16PredEn;
    MI_BOOL bInter16x8PredEn;
    MI_BOOL bInter8x16PredEn;
    MI_BOOL bInter8x8PredEn;
    MI_BOOL bInter8x4PredEn;
    MI_BOOL bInter4x8PredEn;
    MI_BOOL bInter4x4PredEn;
    MI_BOOL bExtedgeEn;
} MI_VENC_ParamH264InterPred_t;

typedef struct MI_VENC_ParamH264IntraPred_s
{
    MI_BOOL bIntra16x16PredEn;
    MI_BOOL bIntraNxNPredEn;
    MI_BOOL bConstrainedIntraPredFlag;     //special naming for CODEC ISO SPEC.
    MI_BOOL bIpcmEn;
    MI_U32 u32Intra16x16Penalty;
    MI_U32 u32Intra4x4Penalty;
    MI_BOOL bIntraPlanarPenalty;
} MI_VENC_ParamH264IntraPred_t;

typedef struct MI_VENC_ParamH264Trans_s
{
    MI_U32 u32IntraTransMode;
    MI_U32 u32InterTransMode;
    MI_S32 s32ChromaQpIndexOffset;
} MI_VENC_ParamH264Trans_t;

typedef struct MI_VENC_ParamH264Entropy_s
{
    MI_U32 u32EntropyEncModeI;
    MI_U32 u32EntropyEncModeP;
} MI_VENC_ParamH264Entropy_t;

typedef struct MI_VENC_ParamH265InterPred_s
{
    /* search window */
    MI_U32 u32HWSize;
    MI_U32 u32VWSize;
    MI_BOOL bInter16x16PredEn;
    MI_BOOL bInter16x8PredEn;
    MI_BOOL bInter8x16PredEn;
    MI_BOOL bInter8x8PredEn;
    MI_BOOL bInter8x4PredEn;
    MI_BOOL bInter4x8PredEn;
    MI_BOOL bInter4x4PredEn;
    MI_U32  u32Inter32x32Penalty;
    MI_U32  u32Inter16x16Penalty;
    MI_U32  u32Inter8x8Penalty;
    MI_BOOL bExtedgeEn;
} MI_VENC_ParamH265InterPred_t;

typedef struct MI_VENC_ParamH265IntraPred_s
{
    MI_BOOL bIntra32x32PredEn;
    MI_BOOL bIntra16x16PredEn;
    MI_BOOL bIntra8x8PredEn;
    MI_BOOL bConstrainedIntraPredFlag;
    MI_U32 u32Intra32x32Penalty;
    MI_U32 u32Intra16x16Penalty;
    MI_U32 u32Intra8x8Penalty;
} MI_VENC_ParamH265IntraPred_t;

typedef struct MI_VENC_ParamH265Trans_s
{
    MI_U32 u32IntraTransMode;
    MI_U32 u32InterTransMode;
    MI_S32 s32ChromaQpIndexOffset;
} MI_VENC_ParamH265Trans_t;

typedef struct MI_VENC_ParamH264Dblk_s
{
    MI_U32 disable_deblocking_filter_idc;     //special naming for CODEC ISO SPEC.
    MI_S32 slice_alpha_c0_offset_div2;     //special naming for CODEC ISO SPEC.
    MI_S32 slice_beta_offset_div2;     //special naming for CODEC ISO SPEC.
} MI_VENC_ParamH264Dblk_t;

typedef struct MI_VENC_ParamH264VuiAspectRatio_s
{
    MI_U8 u8AspectRatioInfoPresentFlag;
    MI_U8 u8AspectRatioIdc;
    MI_U8 u8OverscanInfoPresentFlag;
    MI_U8 u8OverscanAppropriateFlag;
    MI_U16 u16SarWidth;
    MI_U16 u16SarHeight;
} MI_VENC_ParamH264VuiAspectRatio_t;

typedef struct MI_VENC_ParamH264VuiTimeInfo_s
{
    MI_U8 u8TimingInfoPresentFlag;
    MI_U8 u8FixedFrameRateFlag;
    MI_U32 u32NumUnitsInTick;
    MI_U32 u32TimeScale;
} MI_VENC_ParamH264VuiTimeInfo_t;

typedef struct MI_VENC_ParamH264VuiVideoSignal_s
{
    MI_U8 u8VideoSignalTypePresentFlag;
    MI_U8 u8VideoFormat;
    MI_U8 u8VideoFullRangeFlag;
    MI_U8 u8ColourDescriptionPresentFlag;
    MI_U8 u8ColourPrimaries;
    MI_U8 u8TransferCharacteristics;
    MI_U8 u8MatrixCoefficients;
} MI_VENC_ParamH264VuiVideoSignal_t;

typedef struct MI_VENC_ParamH264Vui_s
{
    MI_VENC_ParamH264VuiAspectRatio_t stVuiAspectRatio;
    MI_VENC_ParamH264VuiTimeInfo_t stVuiTimeInfo;
    MI_VENC_ParamH264VuiVideoSignal_t stVuiVideoSignal;
} MI_VENC_ParamH264Vui_t;

typedef struct MI_VENC_ParamH265VuiAspectRatio_s
{
    MI_U8 u8AspectRatioInfoPresentFlag;
    MI_U8 u8AspectRatioIdc;
    MI_U8 u8OverscanInfoPresentFlag;
    MI_U8 u8OverscanAppropriateFlag;
    MI_U16 u16SarWidth;
    MI_U16 u16SarHeight;
} MI_VENC_ParamH265VuiAspectRatio_t;

typedef struct MI_VENC_ParamH265VuiTimeInfo_s
{
    MI_U8 u8TimingInfoPresentFlag;
    //MI_U8 u8FixedFrameRateFlag;
    MI_U32 u32NumUnitsInTick;
    MI_U32 u32TimeScale;
} MI_VENC_ParamH265VuiTimeInfo_t;

typedef struct MI_VENC_ParamH265VuiVideoSignal_s
{
    MI_U8 u8VideoSignalTypePresentFlag;
    MI_U8 u8VideoFormat;
    MI_U8 u8VideoFullRangeFlag;
    MI_U8 u8ColourDescriptionPresentFlag;
    MI_U8 u8ColourPrimaries;
    MI_U8 u8TransferCharacteristics;
    MI_U8 u8MatrixCoefficients;
} MI_VENC_ParamH265VuiVideoSignal_t;

typedef struct MI_VENC_ParamH265Vui_s
{
    MI_VENC_ParamH265VuiAspectRatio_t stVuiAspectRatio;
    MI_VENC_ParamH265VuiTimeInfo_t stVuiTimeInfo;
    MI_VENC_ParamH265VuiVideoSignal_t stVuiVideoSignal;
} MI_VENC_ParamH265Vui_t;

typedef struct MI_VENC_ParamH265SliceSplit_s
{
    MI_BOOL bSplitEnable;
    MI_U32 u32SliceRowCount;
} MI_VENC_ParamH265SliceSplit_t;

typedef struct MI_VENC_ParamH265Dblk_s
{
    MI_U32 disable_deblocking_filter_idc;     //special naming for CODEC ISO SPEC.
    MI_S32 slice_tc_offset_div2;     //special naming for CODEC ISO SPEC.
    MI_S32 slice_beta_offset_div2;     //special naming for CODEC ISO SPEC.
} MI_VENC_ParamH265Dblk_t;

typedef struct MI_VENC_ParamJpeg_s
{
    MI_U32 u32Qfactor;
    MI_U8 au8YQt[64];
    MI_U8 au8CbCrQt[64];
    MI_U32 u32McuPerEcs;
} MI_VENC_ParamJpeg_t;

typedef struct MI_VENC_ParamMjpeg_s
{
    MI_U8 au8YQt[64];
    MI_U32 u32McuPerEcs;
} MI_VENC_ParamMjpeg_t;

typedef struct MI_VENC_RoiCfg_s
{
    MI_U32 u32Index;
    MI_BOOL bEnable;
    MI_BOOL bAbsQp;
    MI_S32 s32Qp;
    MI_VENC_Rect_t stRect;
} MI_VENC_RoiCfg_t;

typedef struct MI_VENC_RoiBgFrameRate_s
{
    MI_S32 s32SrcFrmRate;
    MI_S32 s32DstFrmRate;
} MI_VENC_RoiBgFrameRate_t;

typedef struct MI_VENC_ParamRef_s
{
    MI_U32  u32Base;
    MI_U32  u32Enhance;
    MI_BOOL bEnablePred;
} MI_VENC_ParamRef_t;

typedef struct MI_VENC_AttrH264Cbr_s
{
    MI_U32 u32Gop;
    MI_U32 u32StatTime;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32BitRate;
    MI_U32 u32FluctuateLevel;
} MI_VENC_AttrH264Cbr_t;

typedef struct MI_VENC_AttrH264Vbr_s
{
    MI_U32 u32Gop;
    MI_U32 u32StatTime;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32MaxBitRate;
    MI_U32 u32MaxQp;
    MI_U32 u32MinQp;
} MI_VENC_AttrH264Vbr_t;

typedef struct MI_VENC_AttrH264FixQp_s
{
    MI_U32 u32Gop;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32IQp;
    MI_U32 u32PQp;
} MI_VENC_AttrH264FixQp_t;

typedef struct MI_VENC_AttrH264Abr_s
{
    MI_U32 u32Gop; /*the interval of ISLICE. */
    MI_U32 u32StatTime; /* the rate statistic time, the unit is seconds(s) */
    MI_U32 u32SrcFrmRateNum;/* the input frame rate of the venc */
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32AvgBitRate; /* average bitrate */
    MI_U32 u32MaxBitRate; /* the max bitrate */
} MI_VENC_AttrH264Abr_t;

typedef struct MI_VENC_AttrH264Avbr_s
{
    MI_U32 u32Gop;
    MI_U32 u32StatTime;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32MaxBitRate;
    MI_U32 u32MaxQp;
    MI_U32 u32MinQp;
} MI_VENC_AttrH264Avbr_t;

typedef struct MI_VENC_AttrMjpegCbr_s
{
    MI_U32 u32BitRate;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
} MI_VENC_AttrMjpegCbr_t;

typedef struct MI_VENC_AttrMjpegFixQp_s
{
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32Qfactor;
} MI_VENC_AttrMjpegFixQp_t;

typedef struct MI_VENC_AttrH265Cbr_s
{
    MI_U32 u32Gop;
    MI_U32 u32StatTime;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32BitRate;
    MI_U32 u32FluctuateLevel;
} MI_VENC_AttrH265Cbr_t;

typedef struct MI_VENC_AttrH265Vbr_s
{
    MI_U32 u32Gop;
    MI_U32 u32StatTime;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32MaxBitRate;
    MI_U32 u32MaxQp;
    MI_U32 u32MinQp;
} MI_VENC_AttrH265Vbr_t;

typedef struct MI_VENC_AttrH265FixQp_s
{
    MI_U32 u32Gop;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32IQp;
    MI_U32 u32PQp;
} MI_VENC_AttrH265FixQp_t;

typedef struct MI_VENC_AttrH265Avbr_s
{
    MI_U32 u32Gop;
    MI_U32 u32StatTime;
    MI_U32 u32SrcFrmRateNum;
    MI_U32 u32SrcFrmRateDen;
    MI_U32 u32MaxBitRate;
    MI_U32 u32MaxQp;
    MI_U32 u32MinQp;
} MI_VENC_AttrH265Avbr_t;

typedef struct MI_VENC_RcAttr_s
{
    MI_VENC_RcMode_e eRcMode;
    union
    {
        MI_VENC_AttrH264Cbr_t stAttrH264Cbr;
        MI_VENC_AttrH264Vbr_t stAttrH264Vbr;
        MI_VENC_AttrH264FixQp_t stAttrH264FixQp;
        MI_VENC_AttrH264Abr_t stAttrH264Abr;
        MI_VENC_AttrH264Avbr_t stAttrH264Avbr;
        MI_VENC_AttrMjpegCbr_t stAttrMjpegCbr;
        MI_VENC_AttrMjpegFixQp_t stAttrMjpegFixQp;
        MI_VENC_AttrH265Cbr_t stAttrH265Cbr;
        MI_VENC_AttrH265Vbr_t stAttrH265Vbr;
        MI_VENC_AttrH265FixQp_t stAttrH265FixQp;
        MI_VENC_AttrH265Avbr_t stAttrH265Avbr;
    };
    void* pRcAttr;
} MI_VENC_RcAttr_t;

typedef struct MI_VENC_ChnAttr_s
{
    MI_VENC_Attr_t stVeAttr;
    MI_VENC_RcAttr_t stRcAttr;
} MI_VENC_ChnAttr_t;

typedef struct MI_VENC_ParamH264Vbr_s
{
    MI_S32 s32IPQPDelta;
    MI_S32 s32ChangePos;
    MI_U32 u32MaxIQp;
    MI_U32 u32MinIQp;
    MI_U32 u32MaxIPProp;
} MI_VENC_ParamH264Vbr_t;

typedef struct MI_VENC_ParamH264Cbr_s
{
    MI_U32 u32MaxQp;
    MI_U32 u32MinQp;
    MI_S32 s32IPQPDelta;
    MI_U32 u32MaxIQp;
    MI_U32 u32MinIQp;
    MI_U32 u32MaxIPProp;
} MI_VENC_ParamH264Cbr_t;

typedef struct MI_VENC_ParamH264Avbr_s
{
    MI_S32 s32IPQPDelta;
    MI_S32 s32ChangePos;
    MI_U32 u32MinIQp;
    MI_U32 u32MaxIPProp;
    MI_U32 u32MaxIQp;
    MI_U32 u32MaxISize;
    MI_U32 u32MaxPSize;
    MI_U32 u32MinStillPercent;
    MI_U32 u32MaxStillQp;
    MI_U32 u32MotionSensitivity;
} MI_VENC_ParamH264Avbr_t;

typedef struct MI_VENC_ParamMjpegCbr_s
{
    MI_U32 u32MaxQfactor;
    MI_U32 u32MinQfactor;
} MI_VENC_ParamMjpegCbr_t;

typedef struct MI_VENC_ParamH265Vbr_s
{
    MI_S32 s32IPQPDelta;
    MI_S32 s32ChangePos;
    MI_U32 u32MaxIQp;
    MI_U32 u32MinIQp;
    MI_U32 u32MaxIPProp;
} MI_VENC_ParamH265Vbr_t;

typedef struct MI_VENC_ParamH265Cbr_s
{
    MI_U32 u32MaxQp;
    MI_U32 u32MinQp;
    MI_S32 s32IPQPDelta;
    MI_U32 u32MaxIQp;
    MI_U32 u32MinIQp;
    MI_U32 u32MaxIPProp;
} MI_VENC_ParamH265Cbr_t;

typedef struct MI_VENC_ParamH265Avbr_s
{
    MI_S32 s32IPQPDelta;
    MI_S32 s32ChangePos;
    MI_U32 u32MinIQp;
    MI_U32 u32MaxIPProp;
    MI_U32 u32MaxIQp;
    MI_U32 u32MaxISize;
    MI_U32 u32MaxPSize;
    MI_U32 u32MinStillPercent;
    MI_U32 u32MaxStillQp;
    MI_U32 u32MotionSensitivity;
} MI_VENC_ParamH265Avbr_t;

typedef struct MI_VENC_RcParam_s
{
    MI_U32 au32ThrdI[RC_TEXTURE_THR_SIZE];
    MI_U32 au32ThrdP[RC_TEXTURE_THR_SIZE];
    MI_U32 u32RowQpDelta;
    union
    {
        MI_VENC_ParamH264Cbr_t stParamH264Cbr;
        MI_VENC_ParamH264Vbr_t stParamH264VBR;
        MI_VENC_ParamH264Avbr_t stParamH264Avbr;
        MI_VENC_ParamMjpegCbr_t stParamMjpegCbr;
        MI_VENC_ParamH265Cbr_t stParamH265Cbr;
        MI_VENC_ParamH265Vbr_t stParamH265Vbr;
        MI_VENC_ParamH265Avbr_t stParamH265Avbr;
    };
    void* pRcParam;
} MI_VENC_RcParam_t;

typedef struct MI_VENC_CropCfg_s
{
    MI_BOOL bEnable; /* Crop region enable */
    MI_VENC_Rect_t stRect; /* Crop region, note: s32X must be multi of 16 */
} MI_VENC_CropCfg_t;

typedef struct MI_VENC_RecvPicParam_s
{
    MI_S32 s32RecvPicNum;
} MI_VENC_RecvPicParam_t;

typedef struct MI_VENC_H264IdrPicIdCfg_s
{
    MI_VENC_H264eIdrPicIdMode_e eH264eIdrPicIdMode;
    MI_U32 u32H264eIdrPicId;
} MI_VENC_H264IdrPicIdCfg_t;

typedef struct MI_VENC_ParamFrameLost_s
{
    MI_BOOL bFrmLostOpen;
    MI_U32 u32FrmLostBpsThr;
    MI_VENC_FrameLostMode_e eFrmLostMode;
    MI_U32 u32EncFrmGaps;
} MI_VENC_ParamFrameLost_t;

typedef struct MI_VENC_SuperFrameCfg_s
{
    MI_VENC_SuperFrmMode_e eSuperFrmMode;
    MI_U32 u32SuperIFrmBitsThr;
    MI_U32 u32SuperPFrmBitsThr;
    MI_U32 u32SuperBFrmBitsThr;
} MI_VENC_SuperFrameCfg_t;

typedef struct MI_VENC_ParamModH265e_s
{
    MI_U32 u32OneStreamBuffer;
    MI_U32 u32H265eMiniBufMode;
} MI_VENC_ParamModH265e_t;

typedef struct MI_VENC_ParamModJpege_s
{
    MI_U32 u32OneStreamBuffer;
    MI_U32 u32JpegeMiniBufMode;
} MI_VENC_ParamModJpege_t;

typedef struct MI_VENC_ModParam_s
{
    MI_VENC_ModType_e eVencModType;
    union
    {
        //MI_VENC_ParamModVenc_t stVencModParam; //not defined yet
        //MI_VENC_ParamModH264e_t stH264eModParam; //not defined yet
        MI_VENC_ParamModH265e_t stH265eModParam;
        MI_VENC_ParamModJpege_t stJpegeModParam;
    };
} MI_VENC_ModParam_t;

typedef struct MI_VENC_InputSourceConfig_s
{
    MI_VENC_InputSrcBufferMode_e eInputSrcBufferMode;
}MI_VENC_InputSourceConfig_t;

typedef struct MI_VENC_FrameHistoStaticInfo_s
{
    MI_U8   u8PicSkip;
    MI_U16  u16PicType;
    MI_U32  u32PicPoc;
    MI_U32  u32PicSliNum;
    MI_U32  u32PicNumIntra;
    MI_U32  u32PicNumMerge;
    MI_U32  u32PicNumSkip;
    MI_U32  u32PicAvgCtuQp;
    MI_U32  u32PicByte;
    MI_U32  u32GopPicIdx;
    MI_U32  u32PicNum;
    MI_U32  u32PicDistLow;
    MI_U32  u32PicDistHigh;
} MI_VENC_FrameHistoStaticInfo_t;

typedef struct MI_VENC_AdvCustRcAttr_s
{
    MI_BOOL bEnableQPMap;
    MI_BOOL bAbsQP;
    MI_BOOL bEnableModeMap;
    MI_BOOL bEnabelHistoStaticInfo;
} MI_VENC_AdvCustRcAttr_t;

typedef struct MI_VENC_MdInfo_s
{
    MI_U16 u16SadRangeRatio[VENC_MAX_SAD_RANGE_NUM];
} MI_VENC_MdInfo_t;

typedef struct MI_VENC_SmartDetInfo_s
{
    MI_VENC_SmartDetType_e eSmartDetType;
    union
    {
        MI_VENC_MdInfo_t  stMdInfo;
        MI_BOOL           bRoiExist;
    };
    MI_U8                  u8ProtectFrmNum;
} MI_VENC_SmartDetInfo_t;

typedef struct MI_VENC_IntraRefresh_s
{
   MI_BOOL bEnable;
   MI_U32 u32RefreshLineNum;
   MI_U32 u32ReqIQp;
}MI_VENC_IntraRefresh_t;

typedef struct MI_VENC_InitParam_s
{
    MI_U32 u32MaxWidth;
    MI_U32 u32MaxHeight;
}MI_VENC_InitParam_t;
#endif /* End of #ifndef __MI_VENC_DATATYPE_ */
