#ifndef _XM_PRINT_H_
#define  _XM_PRINT_H_
#include "xm530_tmp.h"
#if (defined SOC_SYSTEM) ||(defined SOC_ALIOS)
#include <stdio.h>
#endif


#define NONE          "\033[m"   
#define RED           "\033[0;32;31m"   
#define LIGHT_RED     "\033[1;31m"   
#define GREEN         "\033[0;32;32m"   
#define LIGHT_GREEN   "\033[1;32m"   
#define BLUE          "\033[0;32;34m"   
#define LIGHT_BLUE    "\033[1;34m"   
#define DARY_GRAY     "\033[1;30m"   
#define CYAN          "\033[0;36m"   
#define LIGHT_CYAN    "\033[1;36m"   
#define PURPLE        "\033[0;35m"   
#define LIGHT_PURPLE "\033[1;35m"   
#define BROWN         "\033[0;33m"   
#define YELLOW        "\033[1;33m"   
#define LIGHT_GRAY    "\033[0;37m"   
#define WHITE         "\033[1;37m"   


#ifdef SOC_SYSTEM  	// have system lib
#if 1
#define ERR(fmt, args ... )    	fprintf(stderr, RED "ERR: " fmt NONE, ## args)
#define DEBUG(fmt, args ... ) 	fprintf(stderr, GREEN "DEBUG: " fmt NONE,## args)
#define DBG(fmt, args ... ) 	printf(fmt,## args)
#define ENTER()		printf("\n")
#else
#define DBG(fmt)	 
#define ENTER()	
#define DEBUG(fmt)	 
#define ERR(fmt)	
#endif

#elif (defined SOC_ALIOS)
#define ERR(fmt, args ... )    	fprintf(stderr, RED "ERR: " fmt NONE, ## args)
#define DEBUG(fmt, args ... ) 	fprintf(stderr, GREEN "DEBUG: " fmt NONE,## args)
#define DBG(fmt, args ... ) 	fprintf(stderr,  fmt,## args)
#define ENTER()					fprintf(stderr, "\n\r")

#else
//#ifdef SOC_NONE		// have no system lib
 typedef enum
{
	_DWORD = 8,
	_WORD =4 ,
	_BYTE =2 ,
}HEX_STR;
extern unsigned char putstr(unsigned char u8UartNum, char const *str);
extern int puthex_str(unsigned char uart_NO,HEX_STR style,void *Word); 
#if 1
#define DBG(fmt)	 putstr(0, fmt)
#define ENTER()	putstr(0, "\n\r")
#define DEBUG(fmt)	 putstr(0,"DBG:" fmt "\r")
#define ERR(fmt)	 putstr(0,"ERR:" fmt "\r")
#else
#define DBG(fmt)	 
#define ENTER()	
#define DEBUG(fmt)	 
#define ERR(fmt)	

#endif
#endif

#endif // _XM_PRINT_H_



