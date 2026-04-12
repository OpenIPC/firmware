#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/mman.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

//#define DEBUG_ENABLE 0 //debug 1

#define DEFAULT_MD_LEN 128
#define PAGE_SIZE 0x1000
#define PAGE_SIZE_MASK (~(0xfff))

int debug;

typedef struct tag_MMAP_Node
  {
    unsigned long Start_P;
    unsigned long Start_V;
    unsigned long length;
    unsigned long refcount;
    struct tag_MMAP_Node * next;
  }TMMAP_Node_t;

  TMMAP_Node_t * pTMMAPNode = NULL;
  int fd = -1;
  const char dev[]="/dev/mem";
  jmp_buf *sigbus_jmp; // global

/**************************************************************** 
 * signal_handler
 ****************************************************************/ 
void signal_handler (int sig)
{
  if (sig == SIGBUS)
  {
    //printf("signal_handler SIGBUS!\n");
    if(sigbus_jmp) siglongjmp(*sigbus_jmp, 1);
    // no one to catch the error, so abort
    abort();
  }
}
/**************************************************************** 
 * set_handle 
 ****************************************************************/ 
int set_handler()
{
  struct sigaction act;
  memset (&act, 0, sizeof(act));
  act.sa_sigaction = (void *)signal_handler;
  act.sa_flags = SA_SIGINFO;
  if (sigaction(SIGBUS, &act, 0))
  {
    perror ("sigaction");
    return -1;
  }
  return 0;
}
/**************************************************************** 
 * memmap 
 ****************************************************************/ 
void * memmap(unsigned long phy_addr, unsigned long size)
{
  unsigned long phy_addr_in_page;
  unsigned long page_diff;
  unsigned long size_in_page;
  unsigned long value = 0;
  TMMAP_Node_t * pTmp;
  TMMAP_Node_t * pNew;
  void *addr=NULL;
  if(size == 0)
  {
    printf("memmap():size can't be zero!\n");
    return NULL;
  }
  /* check if the physical memory space have been mmaped */
  pTmp = pTMMAPNode;
  while(pTmp != NULL)
  {
    if( (phy_addr >= pTmp->Start_P) && ( (phy_addr + size) <= (pTmp->Start_P + pTmp->length) ) )
    {
      pTmp->refcount++;   /* referrence count increase by 1  */
      return (void *)(pTmp->Start_V + phy_addr - pTmp->Start_P);
    }
    pTmp = pTmp->next;
  }
  /* not mmaped yet */
  if(fd < 0)
  {
    /* dev not opened yet, so open it */
    fd = open (dev, O_RDONLY);
    if(fd < 0)
    {
      printf("memmap():open %s error!\n", dev);
      return NULL;
    }
  }
  /* addr align in page_size(4K) */
  phy_addr_in_page = phy_addr & PAGE_SIZE_MASK;
  page_diff = phy_addr - phy_addr_in_page;
  /* size in page_size */
  size_in_page =((size + page_diff - 1) & PAGE_SIZE_MASK) + PAGE_SIZE;
  addr = mmap((void *)0, size_in_page, PROT_READ, MAP_SHARED, fd, phy_addr_in_page);
  if(addr == MAP_FAILED)
  {
    printf("memmap():mmap @ 0x%x error!\n", phy_addr_in_page);
    return NULL;
  }
  /* add this mmap to MMAP Node */
  pNew = (TMMAP_Node_t *)malloc(sizeof(TMMAP_Node_t));
  if(NULL == pNew)
  {
    printf("memmap():malloc new node failed!\n");
   	return NULL;
  }
  pNew->Start_P = phy_addr_in_page;
  pNew->Start_V = (unsigned long)addr;
  pNew->length = size_in_page;
  pNew->refcount = 1;
  pNew->next = NULL;
  if(pTMMAPNode == NULL)
  {
    pTMMAPNode = pNew;
  }
  else
  {
    pTmp = pTMMAPNode;
    while(pTmp->next != NULL)
    {
      pTmp = pTmp->next;
    }
    pTmp->next = pNew;
  }
  return (void *)(addr+page_diff);
}
/**************************************************************** 
 * GetValueRegister 
 ****************************************************************/ 
unsigned long GetValueRegister(unsigned long adress)
{
  void *pMem = NULL;
  unsigned long value = -1;
  jmp_buf sigbus_jmpbuf;
  sigbus_jmp = &sigbus_jmpbuf;
  if(sigsetjmp(sigbus_jmpbuf, 1) == 0)
  {
    pMem = memmap(adress,DEFAULT_MD_LEN);
    if (pMem == NULL)
    {
      printf("memmap failed!\n");
      return -1;
    }
    value = *(unsigned int*)pMem;
  }
  return value;
}
/**************************************************************** 
 * parse_int
 ****************************************************************/ 
unsigned long parse_int (char *str) {
	long long result;
	char *endptr; 

	result = strtoll(str, &endptr, 0);
	if (str == '\0' || *endptr != '\0') {
		fprintf(stderr, "\"%s\" is not a valid number\n", str);
		exit(EXIT_FAILURE);
	}

	return (unsigned long)result;
}

/**************************************************************** 
 * gpio_get_value
 ****************************************************************/ 
int gpio_get_value(unsigned int gpio, unsigned long *value)  
{  
    int fd, len;  
    char buf[MAX_BUF];  
    char ch;  
  
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);  
   
    fd = open(buf, O_RDONLY | O_NONBLOCK);  
    if (fd < 0) {  
        perror("gpio/get-value");  
        return fd;  
    }  
   
    read(fd, &ch, 1);  
  
    if (ch != '0') {  
        *value = 1;  
    } else {  
        *value = 0;  
    }  
   
    close(fd);  
    return 0;  
}
/**************************************************************** 
 * gpio_get_dir
 ****************************************************************/ 
int gpio_get_dir(unsigned int gpio, unsigned long *value)  
{  
    int fd, len, i;  
    char buf[MAX_BUF];
	char ch[5];	
   
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);  
	
	
    fd = open(buf, O_RDONLY | O_NONBLOCK);  
    if (fd < 0) {  
        perror("gpio/direction");  
        return fd;  
    }  
	i = read(fd, ch, 5); 
	ch[i] = 0;
	
	
	if(!strncmp("in",ch,2)){*value = 0;}
	else if(!strncmp("out",ch,2)){*value = 1;}
	else	{*value = -1;}

    close(fd);  
    return 0;  
}
/**************************************************************** 
 * check_run 
 ****************************************************************/ 
void check_run(unsigned char *runsh, int chkbit, int chkstat, int chkdirect)
{
	if (chkbit==chkstat) 
		{	
			if(debug)
				{
					if (chkdirect==1) {printf("Direction: Output\n");} else {printf("Direction: Input\n");}
						printf("Check Level:%d Current Level:%d\n",chkstat,chkbit);
				printf("Run Script: %s\n", runsh);}
			system (runsh);
			
		}
	else if (chkstat==-1)
		{	
			if(debug)
				{
					if (chkdirect==1) {printf("Direction: Output\n");} else {printf("Direction: Input\n");}
					printf("Check Level:%d Current Level:%d\n",chkstat,chkbit);
					printf("Run Script: %s\n", runsh);}
			system (runsh);
					
		}
} 
/******************************************************************************
* show usage
******************************************************************************/
void show_sage(char *sPrgNm, int usgc)
{	
	const char gpiou[]="Usage : %s GPIO(gpioN) Status(on|off|both) 'Programm_run'.\n";
	const char memu[]="Usage : %s GPIO_Base GPIO_Offset GPIO_Group GPIO_Bit Status(on|off|both) 'Programm_run'.\n";
	
	switch (usgc)
    {
		case 1:
			printf(memu,sPrgNm);
			break;
		case 2:
			printf(gpiou,sPrgNm);
			break;
        default:
            printf(memu,sPrgNm);
            printf(gpiou,sPrgNm);
            exit(EXIT_FAILURE);
    }
}

//************************************************************
//************************************************************
//************************************************************
/**************************************************************** 
 * main
 ****************************************************************/ 
int main(int argc, char *argv[])
{	 
    unsigned int gpio;  
	int sfs = 0;
	char *stat = NULL;
	char *cmd = NULL;
	unsigned long GPIO_Base, GPIO_Offset;
	unsigned long adress = 0;
	unsigned long direct = 0;
	unsigned long value = 0;
	unsigned long OldValue= 0;
	int GPIO_Group, GPIO_Bit, old_bit, new_bit, status;
	
	if(set_handler()==-1)
	  {
		printf("Set handler Error!\n");
		return 0;
	  }
	
    if ( (argc < 2))
    {
        show_sage(argv[0], 0);
        exit(EXIT_FAILURE);
    }
	
	debug = (argv[0][strlen(argv[0])-1])- '0';			//debug ON
	if ((debug) == 1){ printf("Debug ON!\n"); }
	
	
	if ((sscanf (argv[1], "gpio%d", &gpio) == 1) || (!strncmp("gpio",argv[1],4)))
	{         
		if (argc != 4) {
			show_sage(argv[0], 2); 
			exit(EXIT_FAILURE);}
			
		sfs = 1;
		stat = malloc(strlen(argv[2]));
		strcpy(stat, argv[2]);							//Status_on|off|both
		cmd = malloc(strlen(argv[3]));
		strcpy(cmd, argv[3]);							//Programm_run
		gpio_get_value(gpio, &OldValue);				//Save current value
	}
	else
	{	
		if (argc != 7) {
			show_sage(argv[0], 1);	
			exit(EXIT_FAILURE);}
			
		GPIO_Base=parse_int(argv[1]);  					//GPIO_Base
		GPIO_Offset=parse_int(argv[2]); 				//GPIO_Offset
		GPIO_Group=atoi(argv[3]);						//GPIO_Group
		GPIO_Bit = atoi(argv[4]);						//GPIO_Bit
		stat = malloc(strlen(argv[5]));
		strcpy(stat, argv[5]);							//Status_on|off|both
		cmd = malloc(strlen(argv[6]));
		strcpy(cmd, argv[6]);							//Programm_run
	}
														//Status
	if(!strcmp("on",stat)){status = 1;}					//on = 1 
	else if(!strcmp("off",stat)){status = 0;}			//off = 0 
	else if(!strcmp("both",stat)){status = -1;}			//both = -1 (The script is executed twice)
	else	{status = -1;}
	
	
	while(1)
	{
      if (sfs == 0)
		{
			adress=GPIO_Base+(GPIO_Group*GPIO_Offset)+0x3fc;
			value = GetValueRegister(adress);
		}
		else {
			gpio_get_value(gpio, &value);
		}
			
      if(OldValue!=value)
      {
		  if (sfs == 0){
          old_bit = (OldValue>>GPIO_Bit)&1;
	      new_bit = (value>>GPIO_Bit)&1;

            if(old_bit!=new_bit)
            {	
				  adress=GPIO_Base+(GPIO_Group*GPIO_Offset)+0x400;
				  direct = GetValueRegister(adress);
				  direct = (direct>>GPIO_Bit)&1;
				  adress=GPIO_Base+(GPIO_Group*GPIO_Offset)+(1<<GPIO_Bit+2);              
				  check_run(cmd,new_bit,status,direct);  
            }
			}
			else {
				gpio_get_dir(gpio, &direct);
				check_run(cmd,value,status,direct);
			  }
			
			OldValue=value;
		}
		usleep(250000);
	}
	return 0;
}
//************************************************************
//************************************************************
//************************************************************
