//
//  "$Id: MotionDetect.h 4 2009-06-11 13:01:43Z liwj $"
//
//  Copyright (c)2008-2008, ZheJiang JuFeng Technology Stock CO.LTD.
//  All Rights Reserved.
//
//	Description:	
//	Revisions:		Year-Month-Day  SVN-Author  Modification
//

#ifndef __PAL_MOTIONDETECT_H__
#define __PAL_MOTIONDETECT_H__

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup MotionDetectAPI API Motion Detect
/// 按区域和灵敏度的动态检测接口。
///	\n 调用流程图:
/// \code
///    ============================
///                  |
///         *MotionDetectGetCaps
///                  |
///          MotionDetectCreate
///     +------------|
///     |   MotionDetectGetState
///     | MotionDetectSetParameter
///     |   MotionDetectGetResult
///     |   MotionDetectShowHint
///     +------------|
///         MotionDetectDestory
///                  |
///    ============================
/// \endcode
/// @{

/// 动态检测参数
typedef struct MOTION_DETECT_PARAM
{
	/// 动态检测的敏感度档次
	int		iLevel;

	/// 动态检测的区域，数据的每一位对应一个矩形区域块，置1表示需要在该区域块动
	/// 态检测，置0表示不需要在该区域块动态检测。每一行用一个unsigned int表示，最左边的
	/// 块对应最低位，从上到下的行对应的下标从0到17。支持的行列数之外对应的数据
	/// 位无效。
	unsigned int	win[18];

	/// 灵敏度的具体值
	unsigned char	sensitiveness;

	/// 帧间隔
	unsigned char	fieldinterval;

	/// 使能开关，是否使能，为1表示使能，为0时表示禁用，其他参数被忽略。
	int		enable;
}MOTION_DETECT_PARAM;

/// 动态检测结果
typedef struct MOTION_DETECT_RESULT
{
	/// 有动态检测发生的区域，数据的每一位对应一个矩形区域块，置1表示在该区域块
	/// 动态检测成功，置0表示该区域块动态检测失败。每一行用一个unsigned int表示，最左边
	/// 的块在最高位，从上到下的行对应的下标从0到17。支持的行列数之外对应的数据
	/// 位无效。
	unsigned int	win[18];
}MOTION_DETECT_RESULT;

/// 视频动态检测特性结构
typedef struct MOTION_DETECT_CAPS
{
	unsigned int Enabled;			///< 置1表示支持动态检测，置0表示不支持动态检测。
	unsigned int GridLines;		///< 动态检测的区域需要划分成的列数。
	unsigned int GridRows;			///< 动态检测的区域需要划分成的行数。
	unsigned char  Result;			///< 是否能得到每块区域的检测结果。
	unsigned char  Hint;				///< 是否能在检测成功时给予提示。
}MOTION_DETECT_CAPS;


/// 创建动态检测设备
/// 
/// \param 无
/// \retval <0 创建失败
/// \retval 0 创建成功
int MotionDetectCreate(void);


/// 销毁动态检测设备
/// 
/// \param 无
/// \retval <0 销毁失败
/// \retval 0 销毁成功
int MotionDetectDestory(void);


/// 执行动态检测。立即返回。
/// 
/// \param [out] pData 指向一个unsigned int值的指针，unsigned int值是各个报警输入通道电平状态
///        的掩码。低通道在低位，高通道在高位。高电平置1，低电平置0，不存在的通
///        道置0。
/// \retval 0  检测成功
/// \retval <0  检测失败
int MotionDetectGetState(unsigned int* pData);


/// 动态检测捕获的详细结果。
/// 
/// \param [in] channel 通道号。
/// \param [in] pResult 指向动态检测参数结构MOTION_DETECT_RESULT的指针。
/// \retval 0  设置成功
/// \retval <0  设置失败
int MotionDetectGetResult(int channel, MOTION_DETECT_RESULT *pResult);


/// 设置是否在动态检测成功的区域给予提示。
/// 
/// \param [in] channel 通道号。
/// \param [in] enable 为1表示提示，为0时表示不提示。
/// \retval 0  设置成功
/// \retval <0  设置失败
int MotionDetectShowHint(int channel, int enable);


/// 执行动态检测。立即返回。
/// 
/// \param [in] channel 通道号。
/// \param [in] pParam 指向动态检测参数结构MOTION_DETECT_PARAM的指针。
/// \retval 0  设置成功
/// \retval <0  设置失败
int MotionDetectSetParameter(int channel, MOTION_DETECT_PARAM *pParam);


/// 得到动态检测支持的特性。
/// 
/// \param [out] pCaps 指向动态检测特性结构MOTION_DETECT_CAPS的指针。
/// \retval 0  获取成功。
/// \retval <0  获取失败。
int MotionDetectGetCaps(MOTION_DETECT_CAPS * pCaps);

/// @} end of group

#ifdef __cplusplus
}
#endif

#endif 
