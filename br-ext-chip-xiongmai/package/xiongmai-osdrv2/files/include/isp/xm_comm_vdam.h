#ifndef _XM_COMM_VDAM_H_
#define _XM_COMM_VDAM_H_

#include "xm_type.h"
#include "xm_errno.h"
#include "xm_defines.h"
#include "xm_comm_isp.h"
#include "xm_comm_3a.h"



#define VDAMOWIN 18

typedef struct tagVDA_MOTION_SHELTER_INIT
{
	XM_U32 ShAlarmU; 	//过曝报警阈值 
	XM_U32 ShAlarmD; 	//遮挡报警阈值1(遮挡预警值)
	XM_U32 ShAlarmAf;	//遮挡报警阈值2(对焦值预警)	
	XM_U8 ShRatio;		//遮挡比例
	XM_U8 ShDframe;		//遮挡间隔帧
	XM_U8 ShLevel;
	XM_U8 ShEnble;
}VDA_MOTION_SHELTER_INIT;

typedef struct tagVDA_MOTION_SHELTER_RECDATA
{
	XM_U32 ShChvall;
}VDA_MOTION_SHELTER_RECDATA;

typedef struct tagVDA_MOTION_SHELTER_RESULT
{
	XM_U8 ShReflag;
}VDA_MOTION_SHELTER_RESULT;


typedef struct tagVDA_MOTION_MOVE_INIT
{
	XM_U8 u8RatioThresh;	// [0,100]
	XM_U8 MoRatio;
	XM_U8 MoDframe;
	XM_U8 MoEnble;
	XM_U32 MoSentiven;
	XM_U32 MoGridLine;
	XM_U32 MoGridRows;
	XM_U32 MoWinset[18];
}VDA_MOTION_MOVE_INIT;

typedef struct  tagVDA_MOTION_MOVE_RECDATA
{
	XM_U32 MoChvall[AE_ZONE_ROW][AE_ZONE_COLUMN]; //存取值
}VDA_MOTION_MOVE_RECDATA;

typedef struct	tagVDA_MOTION_MOVE_RESULT
{
	XM_U32 MoVdaBmark[AE_ZONE_ROW];  //移动标志位，每行占四个byte
	XM_U16 MoReflag;
	XM_U16 MoVdaCount;	
}VDA_MOTION_MOVE_RESULT;


typedef struct	tagVDA_MOTION_MOVE_S
{
	VDA_MOTION_MOVE_INIT 	ModInit;
	VDA_MOTION_MOVE_RECDATA MoRatival;
	VDA_MOTION_MOVE_RESULT	Modmark;
}VDA_MOTION_MOVE_S;

typedef struct  tagVDA_MOTION_SHELTER_S
{
	VDA_MOTION_SHELTER_RECDATA ShRaival;
	VDA_MOTION_SHELTER_INIT	   ShdInit;
	VDA_MOTION_SHELTER_RESULT  Shdmark;
}VDA_MOTION_SHELTER_S;

typedef struct	tagVDA_MOTION_CHANNEL
{
	XM_U8   VdaCreate;
	VDA_MOTION_MOVE_S 		MoveReslt;
	VDA_MOTION_SHELTER_S	ShelReslt;
}VDA_MOTION_CHANNEL;


typedef enum
{
	XM_VDA_LEV1 =1,
	XM_VDA_LEV2 =2 ,
	XM_VDA_LEV3 =3 ,
	XM_VDA_LEV4 =4 ,
	XM_VDA_LEV5 =5 ,
	XM_VDA_LEV6 =6 ,	
}VDA_MOTION_Level;

typedef enum
{
	VDA_SHL_RAT1=2,
	VDA_SHL_RAT2=4,
	VDA_SHL_RAT3=6,
	VDA_SHL_RAT4=8,
	VDA_SHL_RAT5=10,
	VDA_SHL_RAT6=16
}VDA_MOTION_SHELTER_RATIO;

typedef enum
{
	VDA_MOV_RAT1=2,
	VDA_MOV_RAT2=4,
	VDA_MOV_RAT3=6,
	VDA_MOV_RAT4=8,
	VDA_MOV_RAT5=10,
	VDA_MOV_RAT6=12
}VDA_MOTION_MOVE_RATIO;


#endif 

