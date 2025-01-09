// Based on https://github.com/danjperron/BitBangingDS18B20.git

#include <fcntl.h>
#include <sched.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

int mem_fd;
void *gpio_map;

unsigned short DS_PIN = 7;
unsigned int GPIO_BASE = 0x120b6000;
unsigned short ArgResolution = 0;
unsigned short ArgScan = 0;
unsigned short ArgWaitTime = 750;

// I/O access
volatile unsigned *gpio;


#define BLOCK_SIZE (4 * 1024)

#define OUT_GPIO(g) *(gpio + 0x400 / sizeof(*gpio)) |= (1 << g)
#define INP_GPIO(g) *(gpio + 0x400 / sizeof(*gpio)) &= ~(1 << g)

#define GPIO_SET(n) (*(gpio + 0x3FC / sizeof(*gpio)) |= (1 << (n)))
#define GPIO_CLR(n) (*(gpio + 0x3FC / sizeof(*gpio)) &= ~(1 << (n)))
#define GPIO_READ(g) (((*(gpio + 0x3FC / sizeof(*gpio))) & (1 << (g))) >> (g))

#define DS18B20_SKIP_ROM 0xCC
#define DS18B20_CONVERT_T 0x44
#define DS18B20_MATCH_ROM 0x55
#define DS18B20_SEARCH_ROM 0XF0
#define DS18B20_READ_SCRATCHPAD 0xBE
#define DS18B20_WRITE_SCRATCHPAD 0x4E
#define DS18B20_COPY_SCRATCHPAD 0x48

unsigned char ScratchPad[9];
double temperature;
int resolution;

void setup_io();
#define DELAY1US DelayMicrosecondsNoSleep(1);

void DelayMicrosecondsNoSleep(int delay_us) {
  long int start_time;
  long int time_difference;
  struct timespec gettime_now;

  clock_gettime(CLOCK_REALTIME, &gettime_now);
  start_time = gettime_now.tv_nsec; // Get nS value
  while (1) {
    clock_gettime(CLOCK_REALTIME, &gettime_now);
    time_difference = gettime_now.tv_nsec - start_time;
    if (time_difference < 0)
      time_difference += 1000000000;         //(Rolls over every 1 second)
    if (time_difference > (delay_us * 1000)) // Delay for # nS
      break;
  }
}

int DoReset(void) {
  INP_GPIO(DS_PIN);

  DelayMicrosecondsNoSleep(10);

  INP_GPIO(DS_PIN);
  OUT_GPIO(DS_PIN);

  // pin low for 480 us
  GPIO_CLR(DS_PIN);
  usleep(480);
  INP_GPIO(DS_PIN);
  DelayMicrosecondsNoSleep(60);
  if (GPIO_READ(DS_PIN) == 0) {
    DelayMicrosecondsNoSleep(420);
    return 1;
  }
  return 0;
}

void WriteByte(unsigned char value) {
  unsigned char Mask = 1;
  int loop;

  for (loop = 0; loop < 8; loop++) {
    INP_GPIO(DS_PIN);
    OUT_GPIO(DS_PIN);
    GPIO_CLR(DS_PIN);

    if ((value & Mask) != 0) {
      DELAY1US
      INP_GPIO(DS_PIN);
      DelayMicrosecondsNoSleep(60);

    } else {
      DelayMicrosecondsNoSleep(60);
      INP_GPIO(DS_PIN);
      DelayMicrosecondsNoSleep(1);
    }
    Mask *= 2;
    DelayMicrosecondsNoSleep(60);
  }

  usleep(100);
}

void WriteBit(unsigned char value) {
  INP_GPIO(DS_PIN);
  OUT_GPIO(DS_PIN);
  GPIO_CLR(DS_PIN);
  if (value) {
    DELAY1US
    INP_GPIO(DS_PIN);
    DelayMicrosecondsNoSleep(60);
  } else {
    DelayMicrosecondsNoSleep(60);
    INP_GPIO(DS_PIN);
    DelayMicrosecondsNoSleep(1);
  }
  DelayMicrosecondsNoSleep(60);
}

unsigned char ReadBit(void) {
  unsigned char rvalue = 0;
  INP_GPIO(DS_PIN);
  OUT_GPIO(DS_PIN);
  GPIO_CLR(DS_PIN);
  DELAY1US
  INP_GPIO(DS_PIN);
  DelayMicrosecondsNoSleep(2);
  if (GPIO_READ(DS_PIN) != 0)
    rvalue = 1;
  DelayMicrosecondsNoSleep(60);
  return rvalue;
}

unsigned char ReadByte(void) {

  unsigned char Mask = 1;
  int loop;
  unsigned char data = 0;

  for (loop = 0; loop < 8; loop++) {
    INP_GPIO(DS_PIN);
    OUT_GPIO(DS_PIN);
    GPIO_CLR(DS_PIN);
    DELAY1US
    INP_GPIO(DS_PIN);
    // Wait  2 us
    DelayMicrosecondsNoSleep(2);
    if (GPIO_READ(DS_PIN) != 0)
      data |= Mask;
    Mask *= 2;
    DelayMicrosecondsNoSleep(60);
  }

  return data;
}

int ReadScratchPad(void) {
  int loop;

  WriteByte(DS18B20_READ_SCRATCHPAD);
  for (loop = 0; loop < 9; loop++) {
    ScratchPad[loop] = ReadByte();
  }
  return 1;
}

unsigned char CalcCRC(unsigned char *data, unsigned char byteSize) {
  unsigned char shift_register = 0;
  unsigned char loop, loop2;
  char DataByte;

  for (loop = 0; loop < byteSize; loop++) {
    DataByte = *(data + loop);
    for (loop2 = 0; loop2 < 8; loop2++) {
      if ((shift_register ^ DataByte) & 1) {
        shift_register = shift_register >> 1;
        shift_register ^= 0x8C;
      } else
        shift_register = shift_register >> 1;
      DataByte = DataByte >> 1;
    }
  }
  return shift_register;
}

char IDGetBit(unsigned long long *llvalue, char bit) {
  unsigned long long Mask = 1ULL << bit;

  return ((*llvalue & Mask) ? 1 : 0);
}

unsigned long long IDSetBit(unsigned long long *llvalue, char bit,
                            unsigned char newValue) {
  unsigned long long Mask = 1ULL << bit;

  if ((bit >= 0) && (bit < 64)) {
    if (newValue == 0)
      *llvalue &= ~Mask;
    else
      *llvalue |= Mask;
  }
  return *llvalue;
}

void SelectSensor(unsigned long long ID) {
  int BitIndex;

  WriteByte(DS18B20_MATCH_ROM);

  for (BitIndex = 0; BitIndex < 64; BitIndex++)
    WriteBit(IDGetBit(&ID, BitIndex));
}

int SearchSensor(unsigned long long *ID, int *LastBitChange) {
  int BitIndex;
  char Bit, NoBit;

  if (*LastBitChange < 0)
    return 0;

  // Set bit at LastBitChange Position to 1
  // Every bit after LastbitChange will be 0

  if (*LastBitChange < 64) {

    IDSetBit(ID, *LastBitChange, 1);
    for (BitIndex = *LastBitChange + 1; BitIndex < 64; BitIndex++)
      IDSetBit(ID, BitIndex, 0);
  }

  *LastBitChange = -1;

  if (!DoReset())
    return -1;

  WriteByte(DS18B20_SEARCH_ROM);

  for (BitIndex = 0; BitIndex < 64; BitIndex++) {

    NoBit = ReadBit();
    Bit = ReadBit();

    if (Bit && NoBit)
      return -2;

    if (!Bit && !NoBit) {
      // ok 2 possibilities
      if (IDGetBit(ID, BitIndex)) {
        // Bit High already set
        WriteBit(1);
      } else {
        // ok let's try LOW value first
        *LastBitChange = BitIndex;
        WriteBit(0);
      }
    } else if (!Bit) {
      WriteBit(1);
      IDSetBit(ID, BitIndex, 1);
    } else {
      WriteBit(0);
      IDSetBit(ID, BitIndex, 0);
    }
  }

  return 1;
}

int ReadSensor(unsigned long long ID) {
  int RetryCount;
  unsigned char CRCByte;
  union {
    short SHORT;
    unsigned char CHAR[2];
  } IntTemp;

  temperature = -9999.9;

  for (RetryCount = 0; RetryCount < 10; RetryCount++) {

    if (!DoReset())
      continue;

    // start a conversion
    SelectSensor(ID);

    if (!ReadScratchPad())
      continue;

    // OK Check sum Check;
    CRCByte = CalcCRC(ScratchPad, 8);

    if (CRCByte != ScratchPad[8])
      continue;

    // Check Resolution
    resolution = 0;
    switch (ScratchPad[4]) {

    case 0x1f:
      resolution = 9;
      break;
    case 0x3f:
      resolution = 10;
      break;
    case 0x5f:
      resolution = 11;
      break;
    case 0x7f:
      resolution = 12;
      break;
    }

    if (resolution == 0)
      continue;
    // Read Temperature
    IntTemp.CHAR[0] = ScratchPad[0];
    IntTemp.CHAR[1] = ScratchPad[1];

    temperature = 0.0625 * (double)IntTemp.SHORT;

    ID &= 0x00FFFFFFFFFFFFFFULL;
    printf("%02llX-%012llX : ", ID & 0xFFULL, ID >> 8);

    // printf("%02d bits Temperature: %6.2f +/- %4.2f C\n", resolution,
    //        temperature, 0.0625 * (double)(1 << (12 - resolution)));
    printf("%6.2f C\n", temperature);

    return 1;
  }

  return 0;
}

int GlobalStartConversion(void) {
  int retry = 0;
  int maxloop;

  while (retry < 10) {
    if (!DoReset())
      usleep(10000);
    else {
      WriteByte(DS18B20_SKIP_ROM);
      WriteByte(DS18B20_CONVERT_T);
      maxloop = 0;

#define USE_CONSTANT_DELAY
#ifdef USE_CONSTANT_DELAY
      usleep(ArgWaitTime * 1000);
      return 1;
#else
      // wait until ready
      while (!ReadBit()) {
        maxloop++;
        if (maxloop > 100000)
          break;
      }

      if (maxloop <= 100000)
        return 1;
#endif
    }
    retry++;
  }
  return 0;
}

void WriteScratchPad(unsigned char TH, unsigned char TL, unsigned char config) {

  // First reset device
  DoReset();

  usleep(10);
  // Skip ROM command
  WriteByte(DS18B20_SKIP_ROM);

  // Write Scratch pad
  WriteByte(DS18B20_WRITE_SCRATCHPAD);

  // Write TH
  WriteByte(TH);

  // Write TL
  WriteByte(TL);

  // Write config
  WriteByte(config);
}

void CopyScratchPad(void) {

  // Reset device
  DoReset();
  usleep(1000);

  // Skip ROM Command
  WriteByte(DS18B20_SKIP_ROM);

  //  copy scratch pad
  WriteByte(DS18B20_COPY_SCRATCHPAD);
  usleep(100000);
}

void ChangeSensorsResolution(int resolution) {
  int config = 0;

  switch (resolution) {
  case 9:
    config = 0x1f;
    break;
  case 10:
    config = 0x3f;
    break;
  case 11:
    config = 0x5f;
    break;
  default:
    config = 0x7f;
    break;
  }
  WriteScratchPad(0xff, 0xff, config);
  usleep(1000);
  CopyScratchPad();
}

void ScanForSensor(void) {
  unsigned long long ID = 0ULL;
  int NextBit = 64;
  int _NextBit;
  int rcode;
  int retry = 0;
  unsigned long long _ID;
  unsigned char _ID_CRC;
  unsigned char _ID_Calc_CRC;
  // unsigned char _ID_Family;

  while (retry < 10) {
    _ID = ID;
    _NextBit = NextBit;
    rcode = SearchSensor(&_ID, &_NextBit);
    if (rcode == 1) {
      _ID_CRC = (unsigned char)(_ID >> 56);
      _ID_Calc_CRC = CalcCRC((unsigned char *)&_ID, 7);
      if (_ID_CRC == _ID_Calc_CRC) {
        if (ArgScan == 0) {
          if (ReadSensor(_ID)) {
            ID = _ID;
            NextBit = _NextBit;
            retry = 0;
          } else
            retry = 0;
        } else {
          ID = _ID;
          NextBit = _NextBit;
          printf("%016llX\n", ID);
        }
      } else
        retry++;
    } else if (rcode == 0)
      break;
    else
      retry++;
  }
}

void PrintUsage(char *app) {
  fprintf(stderr, "usage :\n\n\t");
  fprintf(stderr, "%s -base 0x -gpio n [-xbits] [-s] [-t delay]\n\n", app);
  fprintf(stderr, " -base 0xn   ->  n specify the GPIO group base address\n");
  fprintf(stderr,
          " -gpio n     ->  n specify the GPIO number in group [0-7]\n");
  fprintf(
      stderr,
      " -xbits      ->  x set the number of bits -9bits,-10bits,-11bits and "
      "-12bits\n");
  fprintf(
      stderr,
      " -t delay    ->  delay is the time in ms to wait after conversion\n");
  fprintf(stderr, " -s          ->  Scan for sensor\n");
}

int DecodeArg(int argc, char **argv) {

  int idx = 1;

  if (argc == 1) {
    PrintUsage(argv[0]);
    return 0;
  }

  while (idx < argc) {
    if (strstr(argv[idx], "help") != NULL) {
      PrintUsage(argv[0]);
      return 0;
    }
    if (strcmp(argv[idx], "-base") == 0)
      sscanf(argv[++idx], "%x", &GPIO_BASE);
    else if (strcmp(argv[idx], "-gpio") == 0)
      DS_PIN = atoi(argv[++idx]);
    else if (strcmp(argv[idx], "-9bits") == 0)
      ArgResolution = 9;
    else if (strcmp(argv[idx], "-10bits") == 0)
      ArgResolution = 10;
    else if (strcmp(argv[idx], "-11bits") == 0)
      ArgResolution = 11;
    else if (strcmp(argv[idx], "-12bits") == 0)
      ArgResolution = 12;
    else if (strcmp(argv[idx], "-s") == 0)
      ArgScan = 1;
    else if (strcmp(argv[idx], "-t") == 0)
      ArgWaitTime = atoi(argv[++idx]);
    else {
      fprintf(stderr, "Unknown argument %s! ", argv[idx]);
      exit(0);
    }
    idx++;
  }
  return 1;
}

void set_max_priority(void) {
  struct sched_param sched;
  memset(&sched, 0, sizeof(sched));
  // Use FIFO scheduler with highest priority for the lowest chance of the
  // kernel context switching.
  sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
  sched_setscheduler(0, SCHED_FIFO, &sched);
}

void set_default_priority(void) {
  struct sched_param sched;
  memset(&sched, 0, sizeof(sched));
  // Go back to default scheduler with default 0 priority.
  sched.sched_priority = 0;
  sched_setscheduler(0, SCHED_OTHER, &sched);
}

int main(int argc, char **argv) {
  int loop;
  int Flag = 0;

  if (DecodeArg(argc, argv) == 0)
    return 0;

  setup_io();

  // Check for pull up resistor
  // Signal  input should be high

  // Set PIN to INPUT MODE
  INP_GPIO(DS_PIN);

  Flag = 0;
  for (loop = 0; loop < 100; loop++) {
    usleep(1000);
    if (GPIO_READ(DS_PIN) != 0) {
      Flag = 1;
      break;
    }
  }

  if (Flag == 0) {
    fprintf(stderr, "*** Error Unable to detect HIGH level. No pull-up resistor ?\n");
    exit(-1);
  }

  if (ArgResolution > 0) {
    // need to change resolution
    ChangeSensorsResolution(ArgResolution);
    // do it twice just in case
    ChangeSensorsResolution(ArgResolution);
  }

  if (GlobalStartConversion() == 0) {
    fprintf(stderr, "*** Error Unable to detect any DS18B20 sensor\n");
    exit(-2);
  }

  set_max_priority();

  ScanForSensor();

  set_default_priority();

  return 0;

}

void setup_io() {
#ifdef USE_GPIOLIB

#else
  /* open /dev/mem */
  if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
    fprintf(stderr, "can't open /dev/mem \n");
    exit(-1);
  }
  /* mmap GPIO */
  gpio_map =
      mmap(NULL,                   // Any adddress in our space will do
           BLOCK_SIZE,             // Map length
           PROT_READ | PROT_WRITE, // Enable reading & writting to mapped memory
           MAP_SHARED,             // Shared with other processes
           mem_fd,                 // File to map
           GPIO_BASE               // Offset to GPIO peripheral
      );
#endif

#ifdef USE_GPIOLIB

#else
  close(mem_fd); // No need to keep mem_fd open after mmap

  if (gpio_map == MAP_FAILED) {
    fprintf(stderr, "mmap error gpio_map=%p\n", gpio_map); // errno also set!
    exit(-1);
  }

  // Always use volatile pointer!
  gpio = (volatile unsigned *)gpio_map;
#endif

}
