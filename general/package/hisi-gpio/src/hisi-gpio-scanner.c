#define _GNU_SOURCE
#include <fcntl.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define DEFAULT_MD_LEN 128
#define PAGE_SIZE 0x1000
#define PAGE_SIZE_MASK (~(0xfff))

typedef struct tag_MMAP_Node {
  unsigned long Start_P;
  unsigned long Start_V;
  unsigned long length;
  unsigned long refcount;
  struct tag_MMAP_Node *next;
} TMMAP_Node_t;

TMMAP_Node_t *pTMMAPNode = NULL;
int fd = -1;
const char dev[] = "/dev/mem";
jmp_buf *sigbus_jmp; // global

//************************************************************
void print_bin(unsigned long data) {
  int i;
  unsigned long ulbit;
  for (i = 7; i >= 0; i--) {
    ulbit = data >> i;
    if (ulbit & 1)
      printf("1");
    else
      printf("0");
  }
}
//************************************************************
void signal_handler(int sig) {
  if (sig == SIGBUS) {
    // printf("signal_handler SIGBUS!\n");
    if (sigbus_jmp)
      siglongjmp(*sigbus_jmp, 1);
    // no one to catch the error, so abort
    abort();
  }
}
//************************************************************
int set_handler() {
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_sigaction = (void *)signal_handler;
  act.sa_flags = SA_SIGINFO;
  if (sigaction(SIGBUS, &act, 0)) {
    perror("sigaction");
    return -1;
  }
  return 0;
}
//************************************************************
void *memmap(unsigned long phy_addr, unsigned long size) {
  unsigned long phy_addr_in_page;
  unsigned long page_diff;
  unsigned long size_in_page;
  unsigned long value = 0;
  TMMAP_Node_t *pTmp;
  TMMAP_Node_t *pNew;
  void *addr = NULL;
  if (size == 0) {
    printf("memmap():size can't be zero!\n");
    return NULL;
  }
  /* проверить, было ли преобразовано пространство физической памяти */
  pTmp = pTMMAPNode;
  while (pTmp != NULL) {
    if ((phy_addr >= pTmp->Start_P) &&
        ((phy_addr + size) <= (pTmp->Start_P + pTmp->length))) {
      pTmp->refcount++; /* referrence count increase by 1  */
      return (void *)(pTmp->Start_V + phy_addr - pTmp->Start_P);
    }
    pTmp = pTmp->next;
  }
  /* not mmaped yet */
  if (fd < 0) {
    /* dev not opened yet, so open it */
    fd = open(dev, O_RDWR | O_SYNC);
    if (fd < 0) {
      printf("memmap():open %s error!\n", dev);
      return NULL;
    }
  }
  /* addr align in page_size(4K) */
  phy_addr_in_page = phy_addr & PAGE_SIZE_MASK;
  page_diff = phy_addr - phy_addr_in_page;
  /* size in page_size */
  size_in_page = ((size + page_diff - 1) & PAGE_SIZE_MASK) + PAGE_SIZE;
  addr = mmap((void *)0, size_in_page, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
              phy_addr_in_page);
  if (addr == MAP_FAILED) {
    printf("memmap():mmap @ 0x%lx error!\n", phy_addr_in_page);
    return NULL;
  }
  /* add this mmap to MMAP Node */
  pNew = (TMMAP_Node_t *)malloc(sizeof(TMMAP_Node_t));
  if (NULL == pNew) {
    printf("memmap():malloc new node failed!\n");
    return NULL;
  }
  pNew->Start_P = phy_addr_in_page;
  pNew->Start_V = (unsigned long)addr;
  pNew->length = size_in_page;
  pNew->refcount = 1;
  pNew->next = NULL;
  if (pTMMAPNode == NULL) {
    pTMMAPNode = pNew;
  } else {
    pTmp = pTMMAPNode;
    while (pTmp->next != NULL) {
      pTmp = pTmp->next;
    }
    pTmp->next = pNew;
  }
  return (void *)(addr + page_diff);
}
//************************************************************
unsigned long GetValueRegister(unsigned long adress) {
  void *pMem = NULL;
  unsigned long value = -1;
  jmp_buf sigbus_jmpbuf;
  sigbus_jmp = &sigbus_jmpbuf;
  if (sigsetjmp(sigbus_jmpbuf, 1) == 0) {
    pMem = memmap(adress, DEFAULT_MD_LEN);
    if (pMem == NULL) {
      printf("memmap failed!\n");
      return -1;
    }
    value = *(unsigned int *)pMem; //читаем региср
  }
  return value;
}
//************************************************************
int SetValueRegister(unsigned long adress, unsigned long value) {
  void *pMem = NULL;
  pMem = memmap(adress, DEFAULT_MD_LEN);
  if (pMem == NULL) {
    printf("memmap failed!\n");
    return -1;
  }
  *(unsigned int *)pMem = value; //пишем в регистр
  return 0;
}
//************************************************************
void get_chip_gpio_adress(unsigned long *Chip_Id, unsigned long *GPIO_Groups,
                          unsigned long *GPIO_Base,
                          unsigned long *GPIO_Offset) {
  *GPIO_Offset = 0x10000;

  switch (*Chip_Id) {
  //-------------------------------------------
  // Default: 0x3516A100 одна группа
  // Hi3516Av100	A7 @ 600 MHz	0x2014_0000
  case 0x3516A100:
    *GPIO_Groups = 17; //пропустить G15
    *GPIO_Base = 0x20140000;
    break;
  // Default: 0x3516A100
  // Hi3516Dv100	A7 @600 MHz	0x2014_0000
  case 0x3516D100:
    *GPIO_Groups = 15;
    *GPIO_Base = 0x20140000;
    break;
  //-------------------------------------------
  // Hi3518Ev100	ARM926 @ 440 MHz	0x2014_0000
  case 0x35180100:
    *GPIO_Groups = 12;
    *GPIO_Base = 0x20140000;
    break;
  //-------------------------------------------
  // Hi3516Cv100	ARM926 @ 440 MHz	0x2014_0000
  case 0x3516C100:
    *GPIO_Groups = 12;
    *GPIO_Base = 0x20140000;
    break;
  //-------------------------------------------
  // Default: 0x3516C300
  // Hi3516Cv300	ARM926 @ 800 MHz	0x1214_0000
  case 0x3516C300:
    *GPIO_Groups = 9;
    *GPIO_Base = 0x12140000;
    *GPIO_Offset = 0x1000;
    break;
  case 0x3516A300:
    *GPIO_Groups = 12;
    *GPIO_Base = 0x120D0000;
    break;
  //-------------------------------------------
  // Default: 0x3516E200
  // Hi3516Ev200	A7 @ 900MHz	0x120B_0000
  case 0x3516E200:
    *GPIO_Groups = 9;
    *GPIO_Base = 0x120B0000;
    *GPIO_Offset = 0x1000;
    break;
  //-------------------------------------------
  // Default: 0x3516E300
  // Hi3516Ev300	A7 @ 900MHz	0x120B_0000
  case 0x3516E300:
    *GPIO_Groups = 10;
    *GPIO_Base = 0x120B0000;
    *GPIO_Offset = 0x1000;
    break;
  //-------------------------------------------
  // Default: 0x3518E200 одна группа
  // Hi3516Cv200	ARM926 @ 540 MHz	0x2014_0000
  case 0x3516C200:
    *GPIO_Groups = 9;
    *GPIO_Base = 0x20140000;
    break;
  // Default: 0x3518E200
  // Hi3518Ev200	ARM926 @ 540 MHz	0x2014_0000
  case 0x3518E200:
    *GPIO_Groups = 9;
    *GPIO_Base = 0x20140000;
    break;
  // Default: 0x3518E200
  // Hi3518Ev201	ARM926 @ 540 MHz	0x2014_0000
  case 0x3518E201:
    *GPIO_Groups = 9;
    *GPIO_Base = 0x20140000;
    break;
  //-------------------------------------------
  case 0x35350100:
    *GPIO_Groups = 15;
    *GPIO_Base = 0x20150000;
    break;
  //-------------------------------------------
  case 0x3536C100:
    *GPIO_Groups = 14; //пропустить G4
    *GPIO_Base = 0x12150000;
    break;
  //-------------------------------------------
  case 0x3536D100:
    *GPIO_Groups = 6;
    *GPIO_Base = 0x12150000;
    break;
  //-------------------------------------------
  case 0x72050200:
    *GPIO_Groups = 9;
    *GPIO_Base = 0x120B0000;
    *GPIO_Offset = 0x1000;
    break;
  case 0x72050300:
    *GPIO_Groups = 10;
    *GPIO_Base = 0x120B0000;
    *GPIO_Offset = 0x1000;
    break;
  case 0x72020300:
    *GPIO_Groups = 9;
    *GPIO_Base = 0x120B0000;
    *GPIO_Offset = 0x1000;
    break;
  case 0x76050100:
    *GPIO_Groups = 10;
    *GPIO_Base = 0x120B0000;
    *GPIO_Offset = 0x1000;
    break;
  //-------------------------------------------
  default:
    *GPIO_Groups = 0;
    *GPIO_Base = 0;
    *GPIO_Offset = 0;
    break;
    //---------------------------------
  }
}

#define MAX_ERROR_MSG 0x1000
static int compile_regex(regex_t *r, const char *regex_text) {
  int status = regcomp(r, regex_text, REG_EXTENDED | REG_NEWLINE | REG_ICASE);
  if (status != 0) {
    char error_message[MAX_ERROR_MSG];
    regerror(status, r, error_message, MAX_ERROR_MSG);
    printf("Regex error compiling '%s': %s\n", regex_text, error_message);
    return -1;
  }
  return 1;
}

bool get_regex_line_from_file(const char *filename, const char *re, char *buf,
                              size_t buflen) {
  long res = false;

  FILE *f = fopen(filename, "r");
  if (!f)
    return false;

  regex_t regex;
  regmatch_t matches[2];
  if (!compile_regex(&regex, re))
    goto exit;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&line, &len, f)) != -1) {
    if (regexec(&regex, line, sizeof(matches) / sizeof(matches[0]),
                (regmatch_t *)&matches, 0) == 0) {
      regoff_t start = matches[1].rm_so;
      regoff_t end = matches[1].rm_eo;

      line[end] = 0;
      strncpy(buf, line + start, buflen);
      res = true;
      break;
    }
  }
  if (line)
    free(line);

exit:
  regfree(&regex);
  fclose(f);
  return res;
}

static long get_uart0_address() {
  char buf[256];

  bool res = get_regex_line_from_file(
      "/proc/iomem", "^([0-9a-f]+)-[0-9a-f]+ : .*uart[@:][0-9]", buf,
      sizeof(buf));
  if (!res) {
    return -1;
  }
  return strtol(buf, NULL, 16);
}

//************************************************************
void get_chip_id(unsigned long *Chip_Id, char *cpu, char *hardware) {
  unsigned long SCBASE;
  unsigned long SCSYSID0 = 0xEE0;
  unsigned long SCSYSID1 = 0xEE4;
  unsigned long SCSYSID2 = 0xEE8;
  unsigned long SCSYSID3 = 0xEEC;
  unsigned long Chip_Ver;
  unsigned long Val;
  char Buffer[4096];
  int i;

  long uart_base = get_uart0_address();
  switch (uart_base) {
  // hi3516cv300
  case 0x12100000:
  // hi3516ev200
  case 0x120a0000:
  case 0x12040000:
    SCBASE = 0x12020000;
    break;
  // hi3536c
  case 0x12080000:
    SCBASE = 0x12050000;
    break;
  // hi3516cv100
  // hi3518ev200
  default:
    SCBASE = 0x20050000;
  }

  //---------------------------------------------
  if ((GetValueRegister(SCBASE + SCSYSID0) & 0xFF000000) >> 24 == 0x35 ||
      (GetValueRegister(SCBASE + SCSYSID0) & 0xFF000000) >> 24 == 0x72 ||
      (GetValueRegister(SCBASE + SCSYSID0) & 0xFF000000) >> 24 == 0x76)
  //если старший байт соответсвует сигнатуре значит все ID в одном регистре
  {
    *Chip_Id = GetValueRegister(SCBASE + SCSYSID0);
    Chip_Ver = 0;
  } else {
    Chip_Ver = (GetValueRegister(SCBASE + SCSYSID0) & 0xFF000000) >>
               24; //старший байт регистра может быть версия чипа
    SCSYSID0 = GetValueRegister(SCBASE + SCSYSID0) &
               0xFF; //читаем младший байт регистра
    SCSYSID1 = GetValueRegister(SCBASE + SCSYSID1) &
               0xFF; //читаем младший байт регистра
    SCSYSID2 = GetValueRegister(SCBASE + SCSYSID2) &
               0xFF; //читаем младший байт регистра
    SCSYSID3 = GetValueRegister(SCBASE + SCSYSID3) &
               0xFF; //читаем младший байт регистра
    *Chip_Id =
        (SCSYSID3 << 24) + (SCSYSID2 << 16) + (SCSYSID1 << 8) + (SCSYSID0);
  }
  //---------------------------------------------
  if ((*Chip_Id == 0x3518E200) & (Chip_Ver == 0x01))
    *Chip_Id = 0x3516C200;
  if ((*Chip_Id == 0x3518E200) & (Chip_Ver == 0x02))
    *Chip_Id = 0x3518E200;
  if ((*Chip_Id == 0x3518E200) & (Chip_Ver == 0x03))
    *Chip_Id = 0x3518E201;
  //---------------------------------------------
  if ((*Chip_Id == 0x3516A100) & (Chip_Ver == 0x01))
    *Chip_Id = 0x3516A100;
  if ((*Chip_Id == 0x3516A100) & (Chip_Ver == 0x02))
    *Chip_Id = 0x3516D100;
  //---------------------------------------------
  switch (*Chip_Id) {
  //-------------------------------------------
  // Default: 0x3516A100
  case 0x3516A100:
    sprintf(cpu, "Hi3516Av100");
    sprintf(hardware, "A7 @ 600 MHz");
    break;
  //-------------------------------------------
  // Default: 0x3516A100
  case 0x3516D100:
    sprintf(cpu, "Hi3516Dv100");
    sprintf(hardware, "A7 @ 600 MHz");
    break;
  //-------------------------------------------
  // Default: 0x35180100
  case 0x35180100:
    sprintf(cpu, "Hi3518Ev100");
    sprintf(hardware, "ARM926 @ 440 MHz");
    break;
  //-------------------------------------------
  case 0x3516C100:
    sprintf(cpu, "Hi3516Cv100");
    sprintf(hardware, "ARM926 @ 440 MHz");
    break;
  //-------------------------------------------
  // Default: 0x3516C300
  case 0x3516C300:
    sprintf(cpu, "Hi3516Cv300");
    sprintf(hardware, "ARM926 @ 800 MHz");
    break;
  case 0x3516A300:
    sprintf(cpu, "Hi3516Av300");
    sprintf(hardware, "A7 dual core @ 900MHz");
    break;
  //-------------------------------------------
  // Default: 0x3516E200
  case 0x3516E200:
    sprintf(cpu, "Hi3516Ev200");
    sprintf(hardware, "A7 @ 900MHz");
    break;
  //-------------------------------------------
  // Default: 0x3516E300
  case 0x3516E300:
    sprintf(cpu, "Hi3516Ev300");
    sprintf(hardware, "A7 @ 900MHz");
    break;
  //-------------------------------------------
  // Default: 0x3518E200 одна группа
  case 0x3516C200:
    sprintf(cpu, "Hi3516Cv200");
    sprintf(hardware, "ARM926 @ 540 MHz");
    break;
  // Default: 0x3518E200
  case 0x3518E200:
    sprintf(cpu, "Hi3518Ev200");
    sprintf(hardware, "ARM926 @ 540 MHz");
    break;
  // Default: 0x3518E200
  case 0x3518E201:
    sprintf(cpu, "Hi3518Ev201");
    sprintf(hardware, "ARM926 @ 540 MHz");
    break;
  //-------------------------------------------
  case 0x35350100:
    sprintf(cpu, "Hi3535v100");
    sprintf(hardware, "A9 dual-core @ 900 MHz");
    break;
  //-------------------------------------------
  case 0xBDA9D100:
    sprintf(cpu, "Hi3536Cv100");
    sprintf(hardware, "A7 dual-core @ 1.3 GHz");
    break;
  //-------------------------------------------
  case 0x3536D100:
    sprintf(cpu, "Hi3536Dv100");
    sprintf(hardware, "A7 @ 850 MHz");
    break;
  //-------------------------------------------
  case 0x72050200:
    sprintf(cpu, "Gk7205v200");
    sprintf(hardware, "A7 @ 900MHz");
    break;
  //-------------------------------------------
  case 0x72050300:
    sprintf(cpu, "Gk7205v300");
    sprintf(hardware, "A7 @ 900MHz");
    break;
  //-------------------------------------------
  case 0x72020300:
    sprintf(cpu, "Gk7202v300");
    sprintf(hardware, "A7 @ 900MHz");
    break;
  //-------------------------------------------
  case 0x76050100:
    sprintf(cpu, "Gk7605v100");
    sprintf(hardware, "A7 @ 900MHz");
    break;
  //-------------------------------------------
  default: //чип не поддерживается
    sprintf(cpu, "unknown");
    sprintf(hardware, "unknown");
    break;
    //---------------------------------
  }
}
//************************************************************
int main(int argc, char *argv[]) {
  unsigned long Chip_Id = 0;
  char CPU[50] = {""};
  char HARDWARE[50] = {""};
  unsigned long GPIO_Groups, GPIO_Base, GPIO_Offset;
  unsigned long adress = 0;
  unsigned long direct = 0;
  unsigned long value = 0;
  unsigned long OldValue[20];
  int bit, old_bit, new_bit;
  int i, group, mask;
  int HeaderByte, Skip, SkipPin;
  //---------------------------------------------------------------
  if (set_handler() == -1) {
    printf("Set handler Error!\n");
    return 0;
  }
  get_chip_id(&Chip_Id, &CPU[0], &HARDWARE[0]);
  printf("To skip pin use: %s 12 14 ...\n", argv[0]);
  printf("================ Hisilicon GPIO Scaner (2021) OpenIPC.org collective "
         "=================\n");
  printf("Chip_Id: 0x%08lX, CPU: %s, Hardware: %s\n", Chip_Id, CPU, HARDWARE);
  if (!strcmp(CPU, "unknown")) //чип не поддерживается
  {
    printf("This CPU not support!\n");
    return 0;
  }
  get_chip_gpio_adress(&Chip_Id, &GPIO_Groups, &GPIO_Base, &GPIO_Offset);
  //-----------------------------
  for (group = 0; group < GPIO_Groups; group++) {
    adress = GPIO_Base + (group * GPIO_Offset) + 0x3fc; //регистр данных портов
    value = GetValueRegister(adress);
    OldValue[group] = value; //запоминаем в массив значение
    printf("Gr:%2d, Addr:0x%08lX, Data:0x%02lX = 0b", group, adress, value);
    print_bin(value); //выводим бинарный вид
    adress =
        GPIO_Base + (group * GPIO_Offset) + 0x400; //регистр направления портов
    direct = GetValueRegister(adress);
    printf(", Addr:0x%08lX, Dir:0x%02lX = 0b", adress, direct);
    print_bin(direct);
    printf("\n");
  }
  if (argc > 1) {
    printf("-------------------------------------------------------------------"
           "-------------------\n");
    for (i = 1; i < argc; i++)
      printf("Skip Pin: GPIO%s\n", argv[i]);
  }
  printf("---------------------------------------------------------------------"
         "-----------------\n");
  printf("While change value...\n");
  //-----------------------------
  while (1) {
    for (group = 0; group < GPIO_Groups; group++) {
      adress =
          GPIO_Base + (group * GPIO_Offset) + 0x3fc; //регистр данных портов
      value = GetValueRegister(adress);
      if (OldValue[group] != value) //старый и новый байты не равны
      {
        HeaderByte = 0;
        for (bit = 7; bit >= 0; bit--) //цикл побитного сравнения
        {
          old_bit = (OldValue[group] >> bit) & 1;
          new_bit = (value >> bit) & 1;
          Skip = 0;
          //-------------------------------
          if (argc > 1) //Есть пин для пропуска
          {
            for (i = 1; i < argc; i++) {
              SkipPin = atoi(argv[i]);
              if (((group * 8) + bit) == SkipPin)
                Skip = 1;
            }
          }
          //-------------------------------
          if (Skip == 0) {
            if (old_bit != new_bit) {
              if (HeaderByte == 0) //еще не выводили заголовок измененения байта
              {
                printf("-------------------------------------------------------"
                       "-------------------------------\n");
                printf("Gr:%d, Addr:0x%08lX, Data:0x%02lX = 0b", group, adress,
                       OldValue[group]);
                print_bin(OldValue[group]);
                printf(" --> 0x%02lX = 0b", value);
                print_bin(value);
                printf("\n");
                HeaderByte = 1;
              }
              adress = GPIO_Base + (group * GPIO_Offset) +
                       0x400; //регистр направления портов
              direct = GetValueRegister(adress);
              direct = (direct >> bit) &
                       1; //получили бит направления порта 0-вход 1-выход
              adress = GPIO_Base + (group * GPIO_Offset) + (1 << (bit + 2));
              if (direct == 1) {
                mask = value & 1 << bit;
                printf("Mask: \"himm 0x%08lX 0x%02X\", GPIO%d_%d, GPIO%d, "
                       "Dir:Output, Level:%d\n",
                       adress, mask, group, bit, (group * 8) + bit, new_bit);
              } else {
                mask = value & 1 << bit;
                printf("Mask: \"himm 0x%08lX 0x%02X\", GPIO%d_%d, GPIO%d, "
                       "Dir:Input, Level:%d\n",
                       adress, mask, group, bit, (group * 8) + bit, new_bit);
              }
            }
          }
        }
        OldValue[group] = value; //запоминаем новое значение
      }
    }
    usleep(100000);
  }
  return 0;
}
