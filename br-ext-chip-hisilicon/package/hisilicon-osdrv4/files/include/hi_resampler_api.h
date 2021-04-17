#ifndef _HI_RESAMPLER_API_H_
#define _HI_RESAMPLER_API_H_

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXFRAMESIZE 2048

#ifndef HI_ERR_RESAMPLE_PREFIX
 #define HI_ERR_RESAMPLE_PREFIX 0x80000000
#endif

/* input handle is invalid */
#define HI_ERR_RESAMPLE_HANDLE (HI_ERR_RESAMPLE_PREFIX | 0x0001)

/* pcm circ buffer state is invalid */
#define HI_ERR_RESAMPLE_PCMBUF (HI_ERR_RESAMPLE_PREFIX | 0x0002)

/* input sample number is more than MAXFRAMESIZE or input buffer size
   , or input sample number is not invalid (eg. even)*/
#define HI_ERR_RESAMPLE_SAMPLE_NUMBER (HI_ERR_RESAMPLE_PREFIX | 0x0003)

/* output pcm buffer space is not enough */
#define HI_ERR_RESAMPLE_OUTPCM_SPACE (HI_ERR_RESAMPLE_PREFIX | 0x0004)

/* the channels of input pcm is invalid */
#define HI_ERR_PCM_CHANNEL (HI_ERR_RESAMPLE_PREFIX | 0x0005)

/* the bit width of input pcm is invalid */
#define HI_ERR_PCM_FORMAT (HI_ERR_RESAMPLE_PREFIX | 0x0006)

/* invalid bypass flag */
#define HI_ERR_INVALID_BYPASSFLAG (HI_ERR_RESAMPLE_PREFIX | 0x0007)

/* error unknown */
#define HI_ERR_UNKNOWN (HI_ERR_RESAMPLE_PREFIX | 0x0008)

/* input Empty pointer*/
#define HI_ERR_INPUT_EMPTY_POINTER (HI_ERR_RESAMPLE_PREFIX | 0x0009)

/**************************************************************************************
 * Function:    HI_Resampler_Create
 *
 * Description: allocate memory for platform-specific data
 *              clear all the user-accessible fields
 *
 * Inputs:      inrate:  8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
 *              outrate: 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
 *              chans:   1 or 2
 * Outputs:     none
 *
 * Return:      handle to Resampler instance, 0 if malloc fails
 **************************************************************************************/
HI_VOID* HI_Resampler_Create(HI_S32 s32Inrate, HI_S32 s32Outrate, HI_S32 s32Chans);


/**************************************************************************************
 * Function:    HI_Resampler_Process
 *
 * Description: Resample pcm data to specific samplerate, only for interlaced format
 *
 * Inputs:      inst: valid Resampler instance pointer (HResampler)
 *              inbuf:   pointer to inputbuf
 *              insamps: input number of sample pointers
 * Outputs:     outbuf:  pointer to outputbuf
 *
 * Return:      output sample number per-channel
 * Notes:       sure insamps < MAXFRAMESIZE


 **************************************************************************************/
HI_S32 HI_Resampler_Process(HI_VOID* inst, HI_S16* s16Inbuf, HI_S32 s32Insamps, HI_S16* s16Outbuf);


/**************************************************************************************
 * Function:    HI_Resampler_Destroy
 *
 * Description: free platform-specific data allocated by ResamplerCreate
 *
 * Inputs:      valid Resampler instance pointer (HResampler)
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
HI_VOID HI_Resampler_Destroy(HI_VOID* inst);


/*******************************************************************************
 * Function:	HI_Resampler_GetMaxOutputNum
 *
 * Description: Caculate max output number at specific input number
 *
 * Inputs:		inst:	  valid Resampler instance pointer (HI_HANDLE)
 *				insamps:  input data number per-channel, insamps must be even
 * Outputs: 	none
 * Return:		>=0:	  Success, return the max output number per-channel
 *				other:	  Fail, return error code
 * Notes:
 * 1  if stereo(chans==2), sure insamps%2 == 0
 ******************************************************************************/
HI_S32 HI_Resampler_GetMaxOutputNum(HI_VOID* inst, HI_S32 s32Insamps);


#ifdef __cplusplus
}
#endif



#endif
