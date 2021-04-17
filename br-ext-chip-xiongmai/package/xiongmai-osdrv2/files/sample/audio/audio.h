/******************************************************************************
  xm510 sample programs head file.


******************************************************************************/

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_sys.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define QQ_FILE "/usr/share/music/welcome.pcm"

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)


typedef struct sample_aenc_getstream_s
{
     XM_BOOL bThreadStart;
}SAMPLE_AENC_GETSTREAM_PARA_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
