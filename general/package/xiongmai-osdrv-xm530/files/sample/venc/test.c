/******************************************************************************
  A simple program of xm510  video encode implementation.

******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int SAMPLE_VENC_720P_CLASSIC(int seconds);
extern int SAMPLE_VENC_720P_SetTitle(int seconds);
extern int SAMPLE_VENC_CaptureJPEG(int num);
extern int SAMPLE_VENC_400W_CLASSIC(int seconds);


/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VENC_Usage(char *sPrgNm)
{
    printf("Usage : %s <index>\n", sPrgNm);
    printf("index:\n");
	printf("\t 0) 720p classic H264 encode.\n");
	printf("\t 1) 720p H264 encode with Title.\n");
    printf("\t 2) 1*720p JPEG snap.\n");
	printf("\t 3) 400W classic H264 encode.\n");
    return;
}

/******************************************************************************
* function    : main()
* Description : video venc sample
******************************************************************************/
int main(int argc, char *argv[])
{
    if ( (argc < 2) || (1 != strlen(argv[1])))
    {
        SAMPLE_VENC_Usage(argv[0]);
        return -1;
    }
    switch (*argv[1])
    {
        case '0':
           	SAMPLE_VENC_720P_CLASSIC(5);
            break;
		case '1':
			SAMPLE_VENC_720P_SetTitle(5);
			break;
		case '2':
			SAMPLE_VENC_CaptureJPEG(5);
			break;
		case '3':
			SAMPLE_VENC_400W_CLASSIC(5);
            break;
        default:
            printf("the index is invaild!\n");
            SAMPLE_VENC_Usage(argv[0]);
            return -1;
    }
	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
