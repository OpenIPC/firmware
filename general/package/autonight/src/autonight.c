#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define BUF_SIZE 256

const char *device = "/dev/jz_adc_aux_0";
const char *nightModeCmd = "curl localhost/night/";
int delayBetweenReads = 3;
int thresholdOn = 900;
int thresholdOff = 1000;
bool nightModeEnabled = false;

void
jzAuxRead(const char *device, int *value)
{
	int fd, size, count;

	fd = open(device, O_RDONLY);
	if(fd < 0){
		perror(device);
		exit(EXIT_FAILURE);
	}
	size = sizeof(*value);
	count = read(fd, (void *)value, size);
	close(fd);
	if(count != size){
		perror("read()");
		exit(EXIT_FAILURE);
	}
}

void
updateNightMode()
{
	int ret;
	char buf[BUF_SIZE];

	printf("Night Mode %s\n", nightModeEnabled ? "Enabled" : "Disabled");
	snprintf(buf, BUF_SIZE, "%s%s\n", nightModeCmd, nightModeEnabled ? "on" : "off");
	ret = system(buf);
	if(ret != 0){
		perror("system()");
		exit(EXIT_FAILURE);
	}
}

void
usage(const char *argv0)
{
	fprintf(stderr, "\nusage: %s [options]\n\n"
	                "options:\n"
	                "	-D sets jz_adc_aux device (default: %s)\n"
	                "	-c sets the command to call to set night mode (default: %s)\n"
	                "	-d delay (in seconds) between ADC reads (default: %d)\n"
	                "	-O turn on night mode when ADC value drops below this threshold (default: %d)\n"
	                "	-F turn off night mode when ADC value goes above this threshold (default: %d)\n"
	                "	-h print this usage statement and exit\n\n",  argv0, device, nightModeCmd, delayBetweenReads, thresholdOn, thresholdOff);
}

int
main(int argc, char *argv[])
{
	int opt, value;

	while((opt = getopt(argc, argv, "D:c:d:O:F:h:")) != -1){
		switch(opt){
			case 'D':
				device = optarg;
				break;
			case 'c':
				nightModeCmd = optarg;
				break;
			case 'd':
				delayBetweenReads = atoi(optarg);
				break;
			case 'O':
				thresholdOn = atoi(optarg);
				break;
			case 'F':
				thresholdOff = atoi(optarg);
				break;
			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if(daemon(0, 0) < 0){
		perror("daemon()");
		exit(EXIT_FAILURE);
	}

	sleep(5);

	while(true){
		jzAuxRead(device, &value);
		printf("Current value: %d\n", value);
		if(!nightModeEnabled && value >= thresholdOn){
			nightModeEnabled = true;
			updateNightMode();
		} else if(nightModeEnabled && value < thresholdOff){
			nightModeEnabled = false;
			updateNightMode();
		}
		sleep(delayBetweenReads);
	}

	return 0;
}
