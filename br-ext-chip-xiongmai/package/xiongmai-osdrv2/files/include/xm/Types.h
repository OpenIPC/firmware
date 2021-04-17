#ifndef _TYPES_H_
#define _TYPES_H_

#include "BlindDetect.h"
#include "MotionDetect.h"
#include "xm_type.h"




typedef struct _stvda_date_ 
{
	// Motion
	MOTION_DETECT_CAPS stMotionPar;
	XM_U8 u8MotionStatus;
	// Blind
	BLIND_DETECT_PARAM stBlindPar;
	XM_U8 u8BlindStatus;
}stVDA_DATE;


extern stVDA_DATE gstVdaDate;



extern int get_dvr_hwinfo(int info_cmd,void *val); 
#define HWID_VGA_CHIP 	(0x59)
#endif

