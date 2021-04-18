#ifndef __SMART_RESULT_H__
#define __SMART_RESULT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

typedef enum{
	SMART_MOVE_DETECT = 0,
	SMART_PRED,
}SMART_TYPE_E;
typedef struct stSmart_Result
{
	unsigned int cmd;//查询命令，每bit代表一种算法(算法顺序与SMART_TYPE_E中顺序保持一致)
	unsigned int uiFlag; //查询结果，每bit代表一种算法结果(算法顺序与SMART_TYPE_E中顺序保持一致)
	unsigned int reserved[16];
}Smart_Result_S;

XM_S32 Smart_GetResult(int nChannel,Smart_Result_S *pstResult) __attribute__((weak));

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif 


