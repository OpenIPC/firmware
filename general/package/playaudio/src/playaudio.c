#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>

#include "imp_audio.h"

#define AO_RATE 8000
#define AO_TIME 20
#define AO_SIZE (AO_RATE * sizeof(short) * AO_TIME / 1000)

int chnVol = 80;
const char *aoPlayFile = "/usr/share/test.pcm";

void
audioOutputPlay()
{
	unsigned char *buf;
	FILE *playFile;
	IMPAudioIOAttr attr;
	int devID, chnID, aogain, size, i;

	buf = NULL;
	buf = (unsigned char *)malloc(AO_SIZE);
	if(buf == NULL){
		perror("malloc()");
		exit(EXIT_FAILURE);
	}

	playFile = fopen(aoPlayFile, "rb");
	if(playFile == NULL){
		perror("fopen()");
		exit(EXIT_FAILURE);
	}

	devID = 0;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 640;
	attr.chnCnt = 1;

	if(IMP_AO_SetPubAttr(devID, &attr)){
		perror("IMP_AO_SetPubAttr()");
		exit(EXIT_FAILURE);
	}

	if(IMP_AO_Enable(devID)){
		perror("IMP_AO_Enable()");
		exit(EXIT_FAILURE);
	}

	chnID = 0;
	if(IMP_AO_EnableChn(devID, chnID)){
		perror("IMP_AO_EnableChn()");
		exit(EXIT_FAILURE);
	}

	if(IMP_AO_SetVol(devID, chnID, chnVol)){
		perror("IMP_AO_SetVol()");
		exit(EXIT_FAILURE);
	}

	aogain = 28;
	if(IMP_AO_SetGain(devID, chnID, aogain)){
		perror("IMP_AO_SetGain()");
		exit(EXIT_FAILURE);
	}

	size = 0;
	while(1){
		size = fread(buf, 1, AO_SIZE, playFile);
		if(size < AO_SIZE)
			break;

		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		if(IMP_AO_SendFrame(devID, chnID, &frm, BLOCK)){
			perror("IMP_AO_SendFrame()");
			exit(EXIT_FAILURE);
		}
	}

	if(IMP_AO_FlushChnBuf(devID, chnID)){
		perror("IMP_AO_FlushChnBuf()");
		exit(EXIT_FAILURE);
	}

	if(IMP_AO_DisableChn(devID, chnID)){
		perror("IMP_AO_DisableChn()");
		exit(EXIT_FAILURE);
	}

	if(IMP_AO_Disable(devID)){
		perror("IMP_AO_Disable()");
		exit(EXIT_FAILURE);
	}

	fclose(playFile);
	free(buf);
}

void
usage(const char *argv0)
{
	fprintf(stderr, "\nusage: %s [options]\n\n"
	                "options:\n"
	                "	-f set path to audio file (default: %s)\n"
	                "	-v set volume (default: %d)\n"
	                "	-h print this usage statement and exit\n\n",  argv0, aoPlayFile, chnVol);
}

int
main(int argc, char *argv[])
{
	int opt;

	while((opt = getopt(argc, argv, "f:v:h:")) != -1){
		switch(opt){
			case 'f':
				aoPlayFile = optarg;
				break;
			case 'v':
				chnVol = atoi(optarg);
				break;
			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	audioOutputPlay();

	return 0;
}