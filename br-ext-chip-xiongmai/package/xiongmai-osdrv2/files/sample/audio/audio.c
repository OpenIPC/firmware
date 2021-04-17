/******************************************************************************
  A simple program of xm510 audio input/output/encoder/decoder implementation.

******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/******************************************************************************
* create   aenc chn 
******************************************************************************/
XM_S32 SAMPLE_COMM_AUDIO_CreatAencCh(AENC_CHN AeChn,PAYLOAD_TYPE_E enType)
{
	XM_S32 s32Ret;
	AENC_CHN_ATTR_S stAencAttr;
	AENC_ATTR_G711_S stG711;
	stAencAttr.enType = enType;
	stAencAttr.u32BufSize = 30;
	stAencAttr.pValue = &stG711;
	stG711.resv = 0;
	/* create aenc chn*/
	s32Ret = XM_MPI_AENC_CreateChn(AeChn, &stAencAttr);
	if (s32Ret)
	{
		SAMPLE_PRT("create aenc err:0x%x\n",s32Ret);
		return XM_FAILURE;
	}
	return XM_SUCCESS;
}

/******************************************************************************
* function : get stream from file, and send it  to Adec
******************************************************************************/
XM_S32  SAMPLE_COMM_AUDIO_StartAenc(AENC_CHN AeChn)
{
	XM_S32 s32Ret= XM_SUCCESS;
	AIO_ATTR_S stAioAttr;
	AI_VQE_CONFIG_S aiVqeConfig;
	AUDIO_DEV AiDevId = 0;
	AI_CHN AiChn = 0;
	AUDIO_DEV AoDevId = 0;
	AO_CHN AoChn = 0;

	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32EXFlag = 1;
	stAioAttr.u32FrmNum = 30;
	stAioAttr.u32PtNumPerFrm = 160;
	stAioAttr.u32ChnCnt = 2;
	stAioAttr.u32ClkSel = 1;
	s32Ret = XM_MPI_AI_SetPubAttr(AeChn, &stAioAttr);
	if (s32Ret)
	{
		SAMPLE_PRT("XM_MPI_AI_SetPubAttr(%d) failed with %d\n",AeChn, s32Ret);
		return XM_FAILURE;
	}

	memset(&aiVqeConfig, 0, sizeof(AI_VQE_CONFIG_S));
	aiVqeConfig.bAecOpen = 0;
	aiVqeConfig.bAgcOpen = 0;
	aiVqeConfig.bAnrOpen = 1;
	s32Ret = XM_MPI_AI_SetVqeAttr(AiDevId, AiChn, AoDevId, AoChn, &aiVqeConfig);
	if (s32Ret)
	{
		SAMPLE_PRT("XM_MPI_AI_SetVqeAttr(%d) failed with %#x\n", AiDevId, s32Ret);
		return XM_FAILURE;
	} 

	s32Ret = XM_MPI_AI_Enable(AeChn);
	if (s32Ret)
	{
		SAMPLE_PRT("XM_MPI_AI_Enable(%d) failed with %d\n", AeChn, s32Ret);
		return XM_FAILURE;
	} 
	return XM_SUCCESS;
}

XM_S32 AudioSaveAenc(FILE* fpAencFile, AUDIO_STREAM_S *pstStream)
{
	fwrite(pstStream->pStream, pstStream->u32Len, 1, fpAencFile);
	fflush(fpAencFile);
	return XM_SUCCESS;
}

XM_VOID* SAMPLE_COMM_AENC_GetVencStreamProc(XM_VOID *p)
{
	XM_S32 s32Ret;
	AENC_CHN AiChn = 0;
	AUDIO_STREAM_S stStream;
	FILE *pFile;
	XM_CHAR AencFileName[64];;
	
	SAMPLE_AENC_GETSTREAM_PARA_S *pstPara;
	pstPara = (SAMPLE_AENC_GETSTREAM_PARA_S*)p;

	sprintf(AencFileName, "Aencstream_chn%d",AiChn);
	pFile = fopen(AencFileName, "wb");
	if (!pFile)
	{
		SAMPLE_PRT("open file[%s] failed!\n", AencFileName);
		return NULL;
	}

	while(pstPara->bThreadStart)
	{	
		s32Ret = XM_MPI_AENC_GetStream(AiChn, &stStream,XM_TRUE);
		if (XM_SUCCESS != s32Ret )
		{
			usleep(10);
			break;
		}
		AudioSaveAenc(pFile,&stStream);
		
		s32Ret = XM_MPI_AENC_ReleaseStream(AiChn, &stStream);
		if(XM_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("XM_MPI_AENC_ReleaseStream failed!\n");
			break;
		}
		usleep(10);
	}
	fclose(pFile);
	return NULL;
}


/******************************************************************************
* function : file -> ADec -> Ao
******************************************************************************/
XM_S32  SAMPLE_AUDIO_AdecAo(void)
{ 
	XM_S32 s32Ret;
	AIO_ATTR_S stAioAttr;
	AUDIO_DEV   AoDev = 0;
	ADEC_CHN    AdChn = 0;
	ADEC_CHN_ATTR_S Adec_Chn;
	AENC_ATTR_G711_S stG711;
	AUDIO_STREAM_S stStream;
	XM_U8 *pAudioStream = NULL;
	FILE *fp = NULL;

	s32Ret = XM_MPI_SYS_Init(); //调用XM_MPI_SYS_Init后不要再调用XM_MPI_AUDIO_Init
	if(s32Ret != XM_SUCCESS)
	{
		SAMPLE_PRT("XM_MPI_SYS_Init err:%d\n",s32Ret);
		return s32Ret;
	}
	
	/********************************************
	step1: set ao public attr
	********************************************/
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16; 
	stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO; 
	stAioAttr.u32EXFlag = 1;
	stAioAttr.u32FrmNum = 30;
	stAioAttr.u32PtNumPerFrm = 160;
	stAioAttr.u32ChnCnt = 2;
	stAioAttr.u32ClkSel = 1;
	s32Ret = XM_MPI_AO_SetPubAttr(AoDev,&stAioAttr);
	if(XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("set ao %d attr err:0x%x\n", AoDev,s32Ret);
		return  XM_FAILURE;
	}
	
	/********************************************
	step2: create adec chn and enable it
	********************************************/
	Adec_Chn.enType = PT_G711A;
	Adec_Chn.u32BufSize = 8;
	Adec_Chn.enMode = ADEC_MODE_PACK ;
	Adec_Chn.pValue = &stG711;
	stG711.resv = 0;
	s32Ret = XM_MPI_ADEC_CreateChn( AdChn ,&Adec_Chn);
	if(XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("creat adec %d channel err:0x%x\n", AdChn,s32Ret);
		return  XM_FAILURE;
	}

	s32Ret = XM_MPI_AO_Enable( AoDev);
	if(XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("creat adec %d channel err:0x%x\n", AoDev,s32Ret);
		return  XM_FAILURE;
	}

	/********************************************
	step3: Adec Send Stream
	********************************************/
	fp = fopen(QQ_FILE,"r");
	if(fp==NULL)
	{
		perror("fopen");
		return -2;
	}
	
	pAudioStream = (XM_U8*)malloc(sizeof(XM_U8)*320); 
	while(1)
	{
		s32Ret = fread(pAudioStream, 1, 320, fp);
		if(s32Ret == 0)
		{
			SAMPLE_PRT("file end.\n");
			usleep(200*1000);
			break;
		}

		stStream.u32Seq = 1;
		stStream.u64TimeStamp = 1;
		stStream.u32Len = s32Ret;
		stStream.pStream = pAudioStream;
		s32Ret = XM_MPI_ADEC_SendStream(AdChn, &stStream,XM_TRUE);
		if (XM_SUCCESS != s32Ret)
		{
			printf("XM_MPI_ADEC_SendStream(%d) failed with %#x!\n", AdChn, s32Ret);
			break;
		}
	}

	free(pAudioStream);
	fclose(fp);

	s32Ret = XM_MPI_AO_Disable(AoDev);
	if(s32Ret != XM_SUCCESS)
	{
		SAMPLE_PRT("XM_MPI_AO_DisEnable dev 0 error\n");
	}

	s32Ret = XM_MPI_ADEC_DestroyChn( AdChn );
	if(XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("creat adec %d channel err:0x%x\n", AdChn,s32Ret);
	}

	return XM_SUCCESS;
}

/******************************************************************************
* function : Ai -> Aenc -> file
******************************************************************************/
XM_S32 SAMPLE_AUDIO_AiAenc(void)
{
	XM_S32 AeChn = 0;
	AUDIO_DEV AiDev = 0;
	XM_S32  s32Ret;
	pthread_t gs_AencPid;
	SAMPLE_AENC_GETSTREAM_PARA_S gs_stPara;

	s32Ret = XM_MPI_SYS_Init(); //调用XM_MPI_SYS_Init后不要再调用XM_MPI_AUDIO_Init
	if(s32Ret != XM_SUCCESS)
	{
		SAMPLE_PRT("XM_MPI_SYS_Init err:%d\n",s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_AUDIO_CreatAencCh(AeChn,PT_G711A);
	if(s32Ret != XM_SUCCESS)
	{
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_CreatAencCh failed!\n");
		goto END_AUDIO_DESTORY;
	}
	s32Ret = SAMPLE_COMM_AUDIO_StartAenc(AeChn);
	if(s32Ret != XM_SUCCESS)
	{
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAenc failed!\n");
		goto END_AUDIO_STOPAEENC;
	}
	
	gs_stPara.bThreadStart = XM_TRUE;
	s32Ret = pthread_create(&gs_AencPid, 0, SAMPLE_COMM_AENC_GetVencStreamProc, (XM_VOID*)&gs_stPara);
	if (XM_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_AENC_StartGetStream failed!\n");
		goto END_AUDIO_STOPAEENC;
	}
	
	sleep(10);

	gs_stPara.bThreadStart = XM_FALSE;
	pthread_join(gs_AencPid, 0);

END_AUDIO_STOPAEENC:
	s32Ret = XM_MPI_AI_Disable(AiDev);
	if (s32Ret)
	{
		SAMPLE_PRT("XM_MPI_AI_Disable [%d] failed\n",AiDev);
		return XM_FAILURE;
	} 
END_AUDIO_DESTORY:
	s32Ret = XM_MPI_AENC_DestroyChn(AeChn);
	if(XM_SUCCESS!=s32Ret)
	{
		SAMPLE_PRT(" XM_MPI_AENC_DestroyChn :%d err:0x%x\n", AeChn, s32Ret);
	}
	return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif 
#endif /* End of #ifdef __cplusplus */

