/******************************************************************************
  A simple program of  xm510 audio input/output/encoder/decoder implementation.
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/******************************************************************************
* function : file -> ADec -> Ao
******************************************************************************/

extern int SAMPLE_AUDIO_AdecAo(void);

/******************************************************************************
* function : Ai -> Aenc -> file
******************************************************************************/
extern int SAMPLE_AUDIO_AiAenc(void);

void SAMPLE_AUDIO_Usage(void)
{
	printf("\n/************************************/\n");
	printf("press sample command as follows!\n");
	printf("1:  send audio frame to AENC channel form AI, save them\n");
	printf("2:  read audio stream from file,decode and send AO\n");
}
/******************************************************************************
* function : main
******************************************************************************/
int main(int argc, char *argv[])
{
    /* arg 1 is audio payload type */
	if ( (argc < 2) || (1 != strlen(argv[1])))
	{
		SAMPLE_AUDIO_Usage();
		return -1;
	}
	switch (*argv[1])
	{
		case '1':
		{
			SAMPLE_AUDIO_AiAenc();/* send audio frame to AENC channel form AI, save them*/
			break;
		}
		case '2':
		{
			SAMPLE_AUDIO_AdecAo();/* read audio stream from file,decode and send AO*/
			break;
		}
		default:
		{
			SAMPLE_AUDIO_Usage();
			return -1;
		}
	}
	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
