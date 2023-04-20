/*
 * Audio utils header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_AUDIO_H__
#define __IMP_AUDIO_H__

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP 音频输入输出头文件
 */

/**
 * @defgroup IMP_Audio
 * @ingroup imp
 * @brief 音频模块，包含录放音、音频编解码、音量及增益设置、回声消除、自动增益等功能
 *
 * @section audio_summary 1 概述
 * 音频功能包含音频输入,音频输出,回音消除,音频编码和音频解码5个模块. \n
 * 其中音频输入和音频输出存在设备和通道的概念.其中一个MIC我们认为是一个Device,而一个MIC可以有多路Channel输入. \n
 * 同样的一个SPK我们认为是一个放音Device,而一个SPK也可以有多路Channel输出. \n
 * 当前版本的音频API一个Device只支持一个Channel. \n
 * 回音消除位于音频输入接口中,具体说明在功能描述中体现. \n
 * 音频编码当前音频API中支持PT_G711A、PT_G711U和PT_G726格式音频编码,如需要增加新的编码方式,需要注册编码器. \n
 * 音频解码当前音频API中支持PT_G711A、PT_G711U和PT_G726格式音频解码,如需要增加新的解码方式,需要注册解码器. \n
 * @section audio_function_description 2 功能描述
 * 以下是对每个模块的具体说明
 * @subsection audio_in 2.1 音频输入
 * 音频输入Device ID 对应关系, 0: 对应数字MIC  1: 对应模拟MIC \n
 * 音频输入Channel当前API只支持1个通道. \n
 * 音频输入的音量设置,音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.其中60是音量设置的一个临界点，\n
 * 在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB。
 * @subsection audio_out 2.2 音频输出
 * 音频输出Device ID 对应关系, 0: 对应默认SPK 1: 对应其他SPK \n
 * 音频输出Channel当前API只支持1个通道. \n
 * 音频输出的音量设置,音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.其中60是音量设置的一个临界点，\n
 * 在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB. \n
 * @subsection audio_aec 2.3 回音消除
 * 回音消除属于音频输入接口中的一项功能,所以在使能回音消除时必须先使能音频输入设备和通道. \n
 * 回音消除当前支持音频采样率为8K和16K，一帧数据采样数为10ms音频数据的整数倍（如：8K采样率， 送入的数据为：8000 × 2 / 100 = 160byte的整数倍）。\n
 * 回音消除针对不同的设备，不同的封装，回音消除会有不同的效果. \n
 * 回音消除目前不支持自适应，所以针对不同设备有单独的回音消除参数, \n
 * 回音消除的参数文件位于/etc/webrtc_profile.ini 配置文件中. \n
 * 配置文件格式为如下(以下列出主要需要调试的三个参数): \n
 * [Set_Far_Frame] \n
 * Frame_V=0.3 \n
 * [Set_Near_Frame] \n
 * Frame_V=0.1 \n
 * delay_ms=150 \n
 *
 * 其中第一个标签[Set_Far_Frame]中的内容代表远端参数,即SPK端放音数据参数. \n
 * Fram_V 代表音频幅值比例,调节该参数可以调节放音数据的幅度(此幅度只用于回音消除). \n
 * 其中第一个标签[Set_Near_Frame]中的内容代表近端参数,即MIC端录音数据参数. \n
 * Fram_V 代表音频幅值比例,调节该参数可以调节录音数据的幅度(此幅度只用于回音消除). \n
 * delay_ms 由于软件和硬件具有延时性,且SPK与MIC的安放有一定距离,SPK放音数据会重新被MIC采样,所以SPK数据在MIC数据中体现会存在一定延时. \n
 * 该时间代表放音数据在录音数据中的时间差. \n

 * @subsection audio_enc 2.4 音频编码
 * 音频编码目前音频API支持PT_G711A、PT_G711U和PT_G726格式音频编码,如果需要增加新的编码方式,需要调用IMP_AENC_RegisterEncoder接口进行注册编码器.
 * @subsection audio_dec 2.5 音频解码
 * 音频解码目前音频API支持PT_G711A、PT_G711U和PT_G726格式音频解码,如果需要增加新的解码方式,需要调用IMP_ADEC_RegisterDecoder接口进行注册解码器.
 * @{
 */

/**
 * 最大音频帧缓存数
 */
#define MAX_AUDIO_FRAME_NUM 50

/**
 * 音频流阻塞类型
 */
typedef enum {
	BLOCK = 0,				/**< 阻塞 */
	NOBLOCK = 1,			/**< 非阻塞 */
} IMPBlock;

/**
 * 音频采样率定义.
 */
typedef enum {
	AUDIO_SAMPLE_RATE_8000	= 8000,		/**< 8KHz采样率 */
	AUDIO_SAMPLE_RATE_16000 = 16000,	/**< 16KHz采样率 */
	AUDIO_SAMPLE_RATE_24000 = 24000,	/**< 24KHz采样率 */
	AUDIO_SAMPLE_RATE_32000 = 32000,	/**< 32KHz采样率 */
	AUDIO_SAMPLE_RATE_44100 = 44100,	/**< 44.1KHz采样率 */
	AUDIO_SAMPLE_RATE_48000 = 48000,	/**< 48KHz采样率 */
	AUDIO_SAMPLE_RATE_96000 = 96000,	/**< 96KHz采样率 */
} IMPAudioSampleRate;

/**
 * 音频采样精度定义.
 */
typedef enum {
	AUDIO_BIT_WIDTH_16 = 16,		/**< 16bit采样精度 */
} IMPAudioBitWidth;

/**
 * 音频声道模式定义.
 */
typedef enum {
	AUDIO_SOUND_MODE_MONO	= 1,	/**< 单声道 */
	AUDIO_SOUND_MODE_STEREO = 2,	/**< 双声道 */
} IMPAudioSoundMode;

/**
 * 定义音频净荷类型枚举.
 */
typedef enum {
	PT_PCM		= 0,
	PT_G711A	= 1,
	PT_G711U	= 2,
	PT_G726 	= 3,
	PT_AEC		= 4,
	PT_ADPCM	= 5,
	PT_MAX		= 6,
} IMPAudioPalyloadType;

/**
 * 定义解码方式.
 */
typedef enum {
	ADEC_MODE_PACK   = 0,	/**< Pack 方式解码 */
	ADEC_MODE_STREAM = 1,	/**< Stream 方式解码 */
} IMPAudioDecMode;

/**
 * 音频输入输出设备属性.
 */
typedef struct {
	IMPAudioSampleRate samplerate;		/**< 音频采样率 */
	IMPAudioBitWidth bitwidth;			/**< 音频采样精度 */
	IMPAudioSoundMode soundmode;		/**< 音频声道模式 */
	int frmNum;							/**< 缓存帧的数目, 取值范围:[2, MAX_AUDIO_FRAME_NUM] */
	int numPerFrm;						/**< 每帧的采样点个数 */
	int chnCnt;							/**< 支持的通道数目 */
} IMPAudioIOAttr;

/**
 * 音频帧结构体.
 */
typedef struct {
	IMPAudioBitWidth bitwidth;			/**< 音频采样精度 */
	IMPAudioSoundMode soundmode;		/**< 音频声道模式 */
	uint32_t *virAddr;					/**< 音频帧数据虚拟地址 */
	uint32_t phyAddr;					/**< 音频帧数据物理地址 */
	int64_t timeStamp;					/**< 音频帧数据时间戳 */
	int seq;							/**< 音频帧序号 */
	int len;							/**< 音频帧长度 */
} IMPAudioFrame;

/**
 * 音频通道参数结构体.
 */
typedef struct {
	int usrFrmDepth;					/**< 音频帧缓存深度 */
	int Rev;							/**< 保留 */
} IMPAudioIChnParam;

/**
 * 音频输出通道的数据缓存状态结构体.
 */
typedef struct {
	int chnTotalNum;				/**< 输出通道总的缓存块数 */
	int chnFreeNum;					/**< 空闲缓存块数 */
	int chnBusyNum;					/**< 被占用的缓存块数 */
} IMPAudioOChnState;

/**
 * 定义音频码流结构体.
 */
typedef struct {
	uint8_t *stream;				/**< 数据流指针 */
	uint32_t phyAddr;				/**< 数据流物理地址 */
	int len;						/**< 音频码流长度 */
	int64_t timeStamp;				/**< 时间戳 */
	int seq;						/**< 音频码流序号 */
} IMPAudioStream;

/**
 * 定义音频编码通道属性结构体.
 */
typedef struct {
	IMPAudioPalyloadType type;				/**< 音频净荷数据类型 */
	int bufSize;							/**<  buf 大小，以帧为单位，[2～MAX_AUDIO_FRAME_NUM] */
	uint32_t *value;						/**< 协议属性指针 */
} IMPAudioEncChnAttr;

/**
 * 定义编码器属性结构体.
 */
typedef struct {
	IMPAudioPalyloadType type;		/**< 编码协议类型 */
	int maxFrmLen;					/**< 最大码流长度 */
	char name[16];					/**< 编码器名称 */
	int (*openEncoder)(void *encoderAttr, void
			*encoder);
	int (*encoderFrm)(void *encoder, IMPAudioFrame
			*data, unsigned char *outbuf,int *outLen);
	int (*closeEncoder)(void *encoder);
} IMPAudioEncEncoder;

/**
 * 定义解码通道属性结构体.
 */
typedef struct {
	IMPAudioPalyloadType type;			/**< 音频解码协议类型 */
	int bufSize;						/**< 音频解码缓存大小 */
	IMPAudioDecMode mode;				/**< 解码方式 */
	void *value;						/**< 具体协议属性指针 */
} IMPAudioDecChnAttr;

/**
 * 定义解码器属性结构体.
 */
typedef struct {
	IMPAudioPalyloadType type;		/**< 音频解码协议类型 */
	char name[16];					/**< 音频解码器名字 */
	int (*openDecoder)(void *decoderAttr, void
			*decoder);
	int (*decodeFrm)(void *decoder, unsigned char
			*inbuf,int inLen, unsigned short *outbuf,int
			*outLen,int *chns);
	int (*getFrmInfo)(void *decoder, void *info);
	int (*closeDecoder)(void *decoder);
} IMPAudioDecDecoder;

/**
 * 定义AGC增益结构体.
 */
typedef struct {
	int TargetLevelDbfs;	/**< 增益级别,取值为[0, 31], 这指目标音量级别,单位为db,为负值.值越小,音量越大. */
	int CompressionGaindB;	/**< 设置最大的增益值,[0, 90],0代表无增益,值越大,增益越高. */
} IMPAudioAgcConfig;

/**
 * 定义噪声抑制级别.
 */
enum Level_ns {
	NS_LOW,			/**< 低等级级别噪声抑制 */
	NS_MODERATE,	/**< 中等级级别噪声抑制 */
	NS_HIGH,		/**< 高等级级别噪声抑制 */
	NS_VERYHIGH		/**< 最高等级级别噪声抑制 */
};

/**
 * @fn int IMP_AI_SetPubAttr(int audioDevId, IMPAudioIOAttr *attr)
 *
 * 设置音频输入设备属性.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] attr 音频设备属性指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * int devID = 1;
 * IMPAudioIOAttr attr;
 * attr.samplerate = AUDIO_SAMPLE_RATE_8000;
 * attr.bitwidth = AUDIO_BIT_WIDTH_16;
 * attr.soundmode = AUDIO_SOUND_MODE_MONO;
 * attr.frmNum = 20;
 * attr.numPerFrm = 400;
 * attr.chnCnt = 1;
 * ret = IMP_AI_SetPubAttr(devID, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Set Audio in %d attr err: %d\n", devID, ret);
 *		return ret;
 * }
 * @endcode
 *
 * @attention 需要在IMP_AI_Enable前调用.
 */
int IMP_AI_SetPubAttr(int audioDevId, IMPAudioIOAttr *attr);

/**
 * @fn int IMP_AI_GetPubAttr(int audioDevId, IMPAudioIOAttr *attr)
 *
 * 获取音频输入设备属性.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[out] attr 音频设备属性指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AI_GetPubAttr(int audioDevId, IMPAudioIOAttr *attr);

/**
 * @fn int IMP_AI_Enable(int audioDevId)
 *
 * 启用音频输入设备.
 *
 * @param[in] audioDevId 音频设备号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 在调用此函数前必须调用 IMP_AI_SetPubAttr().
 */
int IMP_AI_Enable(int audioDevId);

/**
 * @fn int IMP_AI_Disable(int audioDevId)
 *
 * 禁用音频输入设备.
 *
 * @param[in] audioDevId 音频设备号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 与IMP_AI_Enable配套使用,在系统休眠前必须执行IMP_AI_Disable.
 */
int IMP_AI_Disable(int audioDevId);

/**
 * @fn int IMP_AI_EnableChn(int audioDevId, int aiChn)
 *
 * 启用音频输入通道.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 必须先使能device.
 */
int IMP_AI_EnableChn(int audioDevId, int aiChn);

/**
 * @fn int IMP_AI_DisableChn(int audioDevId, int aiChn)
 *
 * 禁用音频输入通道.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 与IMP_AI_EnableChn配套使用.
 */
int IMP_AI_DisableChn(int audioDevId, int aiChn);

/**
 * @fn int IMP_AI_PollingFrame(int audioDevId, int aiChn, unsigned int timeout_ms)
 *
 * Polling音频流缓存.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[in] timeout_ms Polling超时时间.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 在使用IMP_AI_GetFrame之前使用该接口，当该接口调用成功之后表示音频
 *			  数据已经准备完毕，可以使用IMP_AI_GetFrame获取音频数据.
 */
int IMP_AI_PollingFrame(int audioDevId, int aiChn, unsigned int timeout_ms);

/**
 * @fn int IMP_AI_GetFrame(int audioDevId, int aiChn, IMPAudioFrame *frm, IMPBlock block)
 *
 * 获取音频帧.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] frm 音频帧结构体指针.
 * @param[in] block 阻塞/非阻塞标识.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * IMPAudioFrame frm;
 * // 获取音频帧
 * ret = IMP_AI_GetFrame(devID, chnID, &frm, BLOCK);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
 *		return ret;
 * }
 *
 * fwrite(frm.virAddr, 1, frm.len, record_file); // 使用音频帧数据
 *
 * // 释放音频帧
 * ret = IMP_AI_ReleaseFrame(devID, chnID, &frm);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio release frame data error\n");
 *		return ret;
 * }
 * @endcode
 *
 * @attention 无.
 */
int IMP_AI_GetFrame(int audioDevId, int aiChn, IMPAudioFrame *frm, IMPBlock block);

/**
 * @fn int IMP_AI_ReleaseFrame(int audioDevId, int aiChn, IMPAudioFrame *frm)
 *
 * 释放音频帧.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[in] frm 音频帧结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 与IMP_AI_GetFrame配套使用.
 */
int IMP_AI_ReleaseFrame(int audioDevId, int aiChn, IMPAudioFrame *frm);

/**
 * @fn int IMP_AI_SetChnParam(int audioDevId, int aiChn, IMPAudioIChnParam *chnParam)
 *
 * 设置音频输入通道参数.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[in] chnParam 音频通道参数.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * int chnID = 0;
 * IMPAudioIChnParam chnParam;
 * chnParam.usrFrmDepth = 20;	// the range of valid value is [2, MAX_AUDIO_FRAME_NUM].
 * ret = IMP_AI_SetChnParam(devID, chnID, &chnParam);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "set ai %d channel %d attr err: %d\n", devID, chnID, ret);
 *		return ret;
 * }
 * @endcode
 *
 * @attention 在IMP_AI_EnableChn前调用.
 */
int IMP_AI_SetChnParam(int audioDevId, int aiChn, IMPAudioIChnParam *chnParam);

/**
 * @fn int IMP_AI_GetChnParam(int audioDevId, int aiChn, IMPAudioIChnParam *chnParam)
 *
 * 获取音频输入通道参数.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] chnParam 音频通道参数.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AI_GetChnParam(int audioDevId, int aiChn, IMPAudioIChnParam *chnParam);

/**
 * @fn int IMP_AI_EnableAec(int aiDevId, int aiChn, int aoDevId, int aoChn)
 *
 * 启用指定音频输入和音频输出的回声抵消功能.
 *
 * @param[in] aiDevId 需要进行回声抵消的音频输入设备号.
 * @param[in] aiChn 需要进行回声抵消的音频输入通道号.
 * @param[in] aoDevId 需要进行回声抵消的音频输出设备号.
 * @param[in] aoChn 需要进行回声抵消的音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 回音消除针对不同的设备，不同的封装，回音消除会有不同的效果.
 * @remarks 回音消除目前不支持自适应，所以针对不同设备有单独的回音消除参数,
 * @remarks 仅仅只是使能该功能效果不一定会好。
 * @remarks 回音消除的参数文件位于/etc/webrtc_profile.ini 配置文件中.
 * @remarks 配置文件格式为如下(以下列出主要需要调试的三个参数):
 * @remarks [Set_Far_Frame]
 * @remarks Frame_V=0.3
 * @remarks [Set_Near_Frame]
 * @remarks Frame_V=0.1
 * @remarks delay_ms=150
 *
 * @remarks 其中第一个标签[Set_Far_Frame]中的内容代表远端参数,即SPK端放音数据参数.
 * @remarks Fram_V 代表音频幅值比例,调节该参数可以调节放音数据的幅度(此幅度只用于回音消除).
 * @remarks 其中第一个标签[Set_Near_Frame]中的内容代表近端参数,即MIC端录音数据参数.
 * @remarks Fram_V 代表音频幅值比例,调节该参数可以调节录音数据的幅度(此幅度只用于回音消除).
 * @remarks delay_ms 由于软件和硬件具有延时性,且SPK与MIC的安放有一定距离,SPK放音数据会重新被MIC采样,所以SPK数据在MIC数据中体现会存在一定延时.
 * @remarks 该时间代表放音数据在录音数据中的时间差.
 *
 * @attention 实际上接口只会检查aiDevId和aiChn.但是最好在两个通道同时使能后调用. \n
 * 在关闭音频输入通道的同时,回音消除功能同时关闭.如果再次使用需要再次打开.
 */
int IMP_AI_EnableAec(int aiDevId, int aiChn, int aoDevId, int aoChn);

/**
 * @fn int IMP_AI_DisableAec(int aiDevId, int aiChn)
 *
 * 禁用回声抵消功能.
 *
 * @param[in] aiDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AI_DisableAec(int aiDevId, int aiChn);

/**
 * @fn int IMP_AI_EnableNs(IMPAudioIOAttr *attr, int mode)
 *
 * 启用指定音频输入的噪声抑制功能.
 *
 * @param[in] attr 需要进行噪声抑制的音频属性.
 * @param[in] mode 噪声抑制的级别0 ～ 3,参见 Level_ns.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 噪声抑制的mode参数表示噪声抑制的级别,范围为[0 ~ 3],级别越高,噪声抑制的越干净.
 * @remarks 然而,噪声抑制的越干净同时也就会丢失更多的声音细节,所以这里有一个矛盾点,需要在
 * @remarks 使用的时候进行权衡.
 *
 * @attention 回声消除包含了噪声抑制功能,如果启用回声抵消,就不需要做噪声抑制.
 */
int IMP_AI_EnableNs(IMPAudioIOAttr *attr, int mode);

/**
 * @fn int IMP_AI_DisableNs(void)
 *
 * 禁用噪声抑制功能.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AI_DisableNs(void);

/**
 * @fn int IMP_AI_EnableAgc(IMPAudioIOAttr *attr, IMPAudioAgcConfig agcConfig)
 *
 * 启用音频输入的自动增益功能.
 *
 * @param[in] attr 需要进行自动增益的音频属性.
 * @param[in] agcConfig 自动增益的参数配置,配置放大倍数.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 需要注意agcConfig的配置,AGC的放大倍数主要有该参数配置,具体增益见IMPAudioAgcConfig说明.
 * @remarks 需要注意的是,AGC可以将声音的增益放大,但是如果增益的参数不合适,就会导致破音等情况,请在具体使用时自行调整.
 *
 * @attention 回声消除包含了 AGC 功能,如果启用回声抵消,就不需要做自动增益.
 */
int IMP_AI_EnableAgc(IMPAudioIOAttr *attr, IMPAudioAgcConfig agcConfig);

/**
 * @fn int IMP_AI_DisableAgc(void)
 *
 * 禁用AI自动增益功能.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AI_DisableAgc(void);

/**
 * @fn int IMP_AO_EnableAgc(IMPAudioIOAttr *attr, IMPAudioAgcConfig agcConfig)
 *
 * 启用音频输出的自动增益功能.
 *
 * @param[in] attr 需要进行自动增益的音频属性.
 * @param[in] agcConfig 自动增益的参数配置,配置放大倍数.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 需要注意agcConfig的配置,AGC的放大倍数主要有该参数配置,具体增益见IMPAudioAgcConfig说明.
 * @remarks 需要注意的是,AGC可以将声音的增益放大,但是如果增益的参数不合适,就会导致破音等情况,请在具体使用时自行调整.
 *
 * @attention 回声消除包含了 AGC 功能,如果启用回声抵消,就不需要做自动增益.
 */
int IMP_AO_EnableAgc(IMPAudioIOAttr *attr, IMPAudioAgcConfig agcConfig);

/**
 * @fn int IMP_AO_DisableAgc(void)
 *
 * 禁用AO自动增益功能.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_DisableAgc(void);

/**
 * @fn int IMP_AI_EnableHpf(IMPAudioIOAttr *attr)
 *
 * 启用音频输入的高通滤波.
 *
 * @param[in] attr 需要进行高通滤波的音频属性.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 回声消除包含了 HPF 功能,如果启用回声抵消,就不需要做 HPF.
 */
int IMP_AI_EnableHpf(IMPAudioIOAttr *attr);

/**
 * @fn int IMP_AI_DisableHpf(void)
 *
 * 禁用AI高通滤波功能.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AI_DisableHpf(void);

/**
 * @fn int IMP_AO_EnableHpf(IMPAudioIOAttr *attr)
 *
 * 启用音频输出的高通滤波.
 *
 * @param[in] attr 需要进行高通滤波的音频属性.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 回声消除包含了 HPF 功能,如果启用回声抵消,就不需要做 HPF.
 */
int IMP_AO_EnableHpf(IMPAudioIOAttr *attr);

/**
 * @fn int IMP_AO_DisableHpf(void)
 *
 * 禁用AO高通滤波功能.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_DisableHpf(void);

/**
 * @fn int IMP_AO_SetPubAttr(int audioDevId, IMPAudioIOAttr *attr)
 *
 * 设置音频输入输出设备属性.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] attr 音频输出设备属性指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_SetPubAttr(int audioDevId, IMPAudioIOAttr *attr);

/**
 * @fn int IMP_AO_GetPubAttr(int audioDevId, IMPAudioIOAttr *attr)
 *
 * 获取音频输入输出设备属性.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[out] attr 音频输出设备属性指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_GetPubAttr(int audioDevId, IMPAudioIOAttr *attr);

/**
 * @fn int IMP_AO_Enable(int audioDevId)
 *
 * 启用音频输出设备.
 *
 * @param[in] audioDevId 音频设备号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 在使能之前必须先调用IMP_AO_SetPubAttr.
 */
int IMP_AO_Enable(int audioDevId);

/**
 * @fn int IMP_AO_Disable(int audioDevId)
 *
 * 禁用音频输出设备.
 *
 * @param[in] audioDevId 音频设备号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_Disable(int audioDevId);

/**
 * @fn int IMP_AO_EnableChn(int audioDevId, int aoChn)
 *
 * 启用音频输出通道.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_EnableChn(int audioDevId, int aoChn);

/**
 * @fn int IMP_AO_DisableChn(int audioDevId, int aoChn)
 *
 * 禁用音频输出通道.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_DisableChn(int audioDevId, int aoChn);

/**
 * @fn int IMP_AO_SendFrame(int audioDevId, int aoChn, IMPAudioFrame *data, IMPBlock block)
 *
 * 发送音频输出帧.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[in] data 音频帧结构体指针.
 * @param[in] block 阻塞/非阻塞标识.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * while(1) {
 *		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
 *		if(size < IMP_AUDIO_BUF_SIZE)
 *			break;
 *
 *		IMPAudioFrame frm;
 *		frm.virAddr = (uint32_t *)buf;
 *		frm.len = size;
 *		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "send Frame Data error\n");
 *			return ret;
 *		}
 * }
 * @endcode
 *
 * @attention 无.
 */
int IMP_AO_SendFrame(int audioDevId, int aoChn, IMPAudioFrame *data, IMPBlock block);

/**
 * @fn int IMP_AO_PauseChn(int audioDevId, int aoChn)
 *
 * 暂停音频输出通道.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_PauseChn(int audioDevId, int aoChn);

/**
 * @fn int IMP_AO_ResumeChn(int audioDevId, int aoChn)
 *
 * 恢复音频输出通道.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_ResumeChn(int audioDevId, int aoChn);

/**
 * @fn int IMP_AO_ClearChnBuf(int audioDevId, int aoChn)
 *
 * 清除音频输出通道中当前的音频数据缓存.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_ClearChnBuf(int audioDevId, int aoChn);

/**
 * @fn int IMP_AO_QueryChnStat(int audioDevId, int aoChn, IMPAudioOChnState *status)
 *
 * 查询音频输出通道中当前的音频数据缓存状态.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[out] status 缓存状态结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_AO_QueryChnStat(int audioDevId, int aoChn, IMPAudioOChnState *status);

/**
 * @fn int IMP_AENC_CreateChn(int aeChn, IMPAudioEncChnAttr *attr)
 *
 * 创建音频编码通道.
 *
 * @param[in] aeChn 通道号.
 * @param[in] attr 音频编码通道属性指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * int AeChn = 0;
 * IMPAudioEncChnAttr attr;
 * attr.type = PT_G711A;
 * attr.bufSize = 20;
 * ret = IMP_AENC_CreateChn(AeChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio encode create channel failed\n");
 *		return ret;
 * }
 * @endcode
 *
 * @attention 目前SDK支持PT_G711A、PT_G711U和PT_G726编码. \n
 * 所以使用SDK中的编码,只需要attr.type = PT_G711A即可. \n
 * 如何需要使用自定义的编码器,则需要注册编码器,示例代码在注册接口中说明.
 */
 int IMP_AENC_CreateChn(int aeChn, IMPAudioEncChnAttr *attr);

/**
 * @fn int IMP_AENC_DestroyChn(int aeChn)
 *
 * 销毁音频编码通道.
 *
 * @param[in] aeChn 通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 与IMP_AENC_CreateChn配套使用.
 */
 int IMP_AENC_DestroyChn(int aeChn);

/**
 * @fn int IMP_AENC_SendFrame(int aeChn, IMPAudioFrame *frm)
 *
 * 发送音频编码音频帧.
 *
 * @param[in] aeChn 通道号.
 * @param[in] frm 音频帧结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * while(1) {
 *		// 读取一帧数据
 *		ret = fread(buf_pcm, 1, IMP_AUDIO_BUF_SIZE, file_pcm);
 *		if(ret < IMP_AUDIO_BUF_SIZE)
 *			break;
 *
 *		// 编码
 *		IMPAudioFrame frm;
 *		frm.virAddr = (uint32_t *)buf_pcm;
 *		frm.len = ret;
 *		ret = IMP_AENC_SendFrame(AeChn, &frm);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
 *			return ret;
 *		}
 *
 *		// 获取编码码流
 *		IMPAudioStream stream;
 *		ret = IMP_AENC_GetStream(AeChn, &stream, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode get stream failed\n");
 *			return ret;
 *		}
 *
 *		// 使用编码码流
 *		fwrite(stream.stream, 1, stream.len, file_g711);
 *
 *		// 释放编码码流
 *		ret = IMP_AENC_ReleaseStream(AeChn, &stream);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode release stream failed\n");
 *			return ret;
 *		}
 * }
 * @endcode
 *
 * @attention 无.
 */
 int IMP_AENC_SendFrame(int aeChn, IMPAudioFrame *frm);

/**
 * @fn int IMP_AENC_PollingStream(int AeChn, unsigned int timeout_ms)
 *
 * Polling编码音频流缓存.
 *
 * @param[in] AeChn 音频编码输入通道号.
 * @param[in] timeout_ms Polling超时时间.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 在使用IMP_AENC_GetStream之前使用该接口，当该接口调用成功之后表示音频
 *			  编码数据已经准备完毕，可以使用IMP_AENC_GetStream获取编码完毕的数据.
 */
int IMP_AENC_PollingStream(int AeChn, unsigned int timeout_ms);

/**
 * @fn int IMP_AENC_GetStream(int aeChn, IMPAudioStream *stream ,IMPBlock block)
 *
 * 获取编码后码流.
 *
 * @param[in] aeChn 通道号.
 * @param[in] stream 获取音频码流.
 * @param[in] block 阻塞/非阻塞标识.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码见IMP_AENC_SendFrame函数说明.
 *
 * @attention 无.
 */
 int IMP_AENC_GetStream(int aeChn, IMPAudioStream *stream ,IMPBlock block);

/**
 * @fn int IMP_AENC_ReleaseStream(int aeChn,IMPAudioStream *stream)
 *
 * 释放从音频编码通道获取的码流.
 *
 * @param[in] aeChn 通道号.
 * @param[in] stream 获取音频码流指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码见IMP_AENC_SendFrame函数说明.
 *
 * @attention 无.
 */
 int IMP_AENC_ReleaseStream(int aeChn,IMPAudioStream *stream);

/**
 * @fn int IMP_AENC_RegisterEncoder(int *handle, IMPAudioEncEncoder *encoder)
 *
 * 注册编码器.
 *
 * @param[in] ps32handle 注册句柄.
 * @param[in] encoder 编码器属性结构体.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * int handle_g711a = 0;
 * IMPAudioEncEncoder my_encoder;
 * my_encoder.maxFrmLen = 1024;
 * sprintf(my_encoder.name, "%s", "MY_G711A");
 * my_encoder.openEncoder = NULL; // 编码器回调函数
 * my_encoder.encoderFrm = MY_G711A_Encode_Frm; // 编码器回调函数
 * my_encoder.closeEncoder = NULL; // 编码器回调函数
 *
 * ret = IMP_AENC_RegisterEncoder(&handle_g711a, &my_encoder);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "IMP_AENC_RegisterEncoder failed\n");
 *		return ret;
 * }
 *
 * // 使用编码器
 * int AeChn = 0;
 * IMPAudioEncChnAttr attr;
 * attr.type = handle_g711a; // 编码器type等于注册成功返回的handle_g711a的值即可.
 * attr.bufSize = 20;
 * ret = IMP_AENC_CreateChn(AeChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "imp audio encode create channel failed\n");
 *		return ret;
 * }
 * @endcode
 *
 * @attention 注册之后使用方法和使用SDK自带编码器一样.
 */
 int IMP_AENC_RegisterEncoder(int *handle, IMPAudioEncEncoder *encoder);

/**
 * @fn int IMP_AENC_ReleaseEncoder(int *handle)
 *
 * 注销编码器.
 *
 * @param[in] ps32handle 注册句柄(注册编码器时获得的句柄).
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_AENC_ReleaseEncoder(int *handle);

/**
 * @fn int IMP_ADEC_CreateChn(int adChn, IMPAudioDecChnAttr *attr)
 *
 * 创建音频解码通道.
 *
 * @param[in] adChn 通道号.
 * @param[in] attr 通道属性指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * int adChn = 0;
 * IMPAudioDecChnAttr attr;
 * attr.type = PT_G711A;
 * attr.bufSize = 20;
 * attr.mode = ADEC_MODE_PACK;
 * ret = IMP_ADEC_CreateChn(adChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
 *		return ret;
 * }
 * @endcode
 *
 * @attention 无.
 */
 int IMP_ADEC_CreateChn(int adChn, IMPAudioDecChnAttr *attr);

/**
 * @fn int IMP_ADEC_DestroyChn(int adChn)
 *
 * 销毁音频解码通道.
 *
 * @param[in] adChn 通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_ADEC_DestroyChn(int adChn);

/**
 * @fn int IMP_ADEC_SendStream(int adChn, IMPAudioStream *stream, IMPBlock block)
 *
 * 发送音频码流到音频解码通道.
 *
 * @param[in] adChn 通道号.
 * @param[in] stream 音频码流.
 * @param[in] block 阻塞/非阻塞标识.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * while(1) {
 *		// 获取需要解码的数据
 *		ret = fread(buf_g711, 1, IMP_AUDIO_BUF_SIZE/2, file_g711);
 *		if(ret < IMP_AUDIO_BUF_SIZE/2)
 *			break;
 *
 *		// 发送解码数据
 *		IMPAudioStream stream_in;
 *		stream_in.stream = (uint8_t *)buf_g711;
 *		stream_in.len = ret;
 *		ret = IMP_ADEC_SendStream(adChn, &stream_in, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
 *			return ret;
 *		}
 *
 *		// 获取解码后的数据
 *		IMPAudioStream stream_out;
 *		ret = IMP_ADEC_GetStream(adChn, &stream_out, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio decoder get stream failed\n");
 *			return ret;
 *		}
 *
 *		// 使用解码后的数据
 *		fwrite(stream_out.stream, 1, stream_out.len, file_pcm);
 *
 *		// 释放解码后的数据
 *		ret = IMP_ADEC_ReleaseStream(adChn, &stream_out);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio decoder release stream failed\n");
 *			return ret;
 *		}
 * }
 * @endcode
 *
 * @attention 无.
 */
 int IMP_ADEC_SendStream(int adChn, IMPAudioStream *stream, IMPBlock block);

/**
 * @fn int IMP_ADEC_PollingStream(int AdChn, unsigned int timeout_ms)
 *
 * Polling解码音频流缓存.
 *
 * @param[in] AdChn 音频解码输入通道号.
 * @param[in] timeout_ms Polling超时时间.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 在使用IMP_ADEC_GetStream之前使用该接口，当该接口调用成功之后表示音频
 *			  解码数据已经准备完毕，可以使用IMP_ADEC_GetStream获取解码完毕的数据.
 */
int IMP_ADEC_PollingStream(int AdChn, unsigned int timeout_ms);

/**
 * @fn int IMP_ADEC_GetStream(int adChn, IMPAudioStream *stream ,IMPBlock block)
 *
 * 获取解码后码流.
 *
 * @param[in] adChn 通道号.
 * @param[in] stream 获取解码码流.
 * @param[in] block 阻塞/非阻塞标识.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码见IMP_ADEC_SendStream函数说明.
 *
 * @attention 无.
 */
int IMP_ADEC_GetStream(int adChn, IMPAudioStream *stream ,IMPBlock block);

/**
 * @fn int IMP_ADEC_ReleaseStream(int adChn,IMPAudioStream *stream)
 *
 * 释放从音频解码通道获取的码流.
 *
 * @param[in] adChn 通道号.
 * @param[in] stream 音频码流指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码见IMP_ADEC_SendStream函数说明.
 *
 * @attention 无.
 */
int IMP_ADEC_ReleaseStream(int adChn,IMPAudioStream *stream);

/**
 * @fn int IMP_ADEC_ClearChnBuf(int adChn)
 *
 * 清除音频解码通道中当前的音频数据缓存.
 *
 * @param[in] adChn 通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_ADEC_ClearChnBuf(int adChn);

/**
 * @fn int IMP_ADEC_RegisterDecoder(int *handle, IMPAudioDecDecoder *decoder)
 *
 * 注册解码器.
 *
 * @param[in] ps32handle 注册句柄.
 * @param[in] decoder 解码器属性结构体.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * int handle_g711a = 0;
 * IMPAudioDecDecoder my_decoder;
 * sprintf(my_decoder.name, "%s", "MY_G711A");
 * my_decoder.openDecoder = NULL; // 解码器回调函数
 * my_decoder.decodeFrm = MY_G711A_Decode_Frm; // 解码器回调函数
 * my_decoder.getFrmInfo = NULL; // 解码器回调函数
 * my_decoder.closeDecoder = NULL; // 解码器回调函数
 *
 * // 注册解码器
 * ret = IMP_ADEC_RegisterDecoder(&handle_g711a, &my_decoder);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "IMP_ADEC_RegisterDecoder failed\n");
 *		return ret;
 * }
 *
 * // 使用解码器
 * int adChn = 0;
 * IMPAudioDecChnAttr attr;
 * attr.type = handle_g711a; // 解码type等于解码器注册返回的handle_g711a.
 * attr.bufSize = 20;
 * attr.mode = ADEC_MODE_PACK;
 * // 创建解码通道
 * ret = IMP_ADEC_CreateChn(adChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
 *		return ret;
 * }
 * @endcode
 *
 * @attention 注册之后使用方法和使用SDK自带解码器一样.
 */
 int IMP_ADEC_RegisterDecoder(int *handle, IMPAudioDecDecoder *decoder);

/**
 * @fn int IMP_ADEC_ReleaseDecoder(int *handle)
 *
 * 注销解码器.
 *
 * @param[in] ps32handle 注册句柄(注册解码器时获得的句柄).
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_ADEC_ReleaseDecoder(int *handle);

/**
 * ACODEC配置.
 */
/**
 * @fn int IMP_AI_SetVol(int audioDevId, int aiChn, int aiVol)
 *
 * 设置音频输入音量.
 *
 * @param[in] aiDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[in] aiVol 音频输入音量大小.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.
 * @remarks 其中60是音量设置的一个临界点，在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB。
 *
 * 示例代码
 * @code
 * int volume = 60;
 * ret = IMP_AI_SetVol(devID, chnID, volume);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
 *		return ret;
 * }
 * @endcode
 * @attention 如果输入的aiVol超过了[-30 ~ 120]的范围,小于-30的将会取-30,大于120的取120.
 */
 int IMP_AI_SetVol(int audioDevId, int aiChn, int aiVol);

/**
 * @fn int IMP_AI_GetVol(int audioDevId, int aiChn, int *vol)
 *
 * 获取音频输入音量.
 *
 * @param[in] aiDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] vol 音频输入通道音量.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_AI_GetVol(int audioDevId, int aiChn, int *vol);

/**
 * @fn int IMP_AI_SetVolMute(int audioDevId, int aiChn, int mute)
 *
 * 设置音频输入静音.
 *
 * @param[in] aiDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] mute 音频输入静音标志, mute = 0:关闭静音, mute = 1:打开静音.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该接口可立刻静音.
 *
 * @attention 无.
 */
 int IMP_AI_SetVolMute(int audioDevId, int aiChn, int mute);

/**
 * @fn int IMP_AO_SetVol(int audioDevId, int aoChn, int aoVol)
 *
 * 设置音频输出通道音量.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[in] aoVol 音频输出音量.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.
 * @remarks 其中60是音量设置的一个临界点，在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB。
 *
 * @attention 如果输入的aoVol超过了[-30 ~ 120]的范围,小于-30的将会取-30,大于120的取120.
 */
 int IMP_AO_SetVol(int audioDevId, int aoChn, int aoVol);

/**
 * @fn int IMP_AO_GetVol(int audioDevId, int aoChn, int *vol)
 *
 * 获取音频输出通道音量.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[out] aoVol 音频输出音量.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_AO_GetVol(int audioDevId, int aoChn, int *vol);

/**
 * @fn int IMP_AO_SetVolMute(int audioDevId, int aoChn, int mute)
 *
 * 设置音频输出静音.
 *
 * @param[in] audioDevId 音频输出设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[out] mute 音频输出静音标志, mute = 0:关闭静音, mute = 1:打开静音.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该接口可立刻静音.
 *
 * @attention 无.
 */
 int IMP_AO_SetVolMute(int audioDevId, int aoChn, int mute);

/**
 * @fn int IMP_AI_SetGain(int audioDevId, int aiChn, int aiGain)
 *
 * 设置音频输入增益.
 *
 * @param[in] audioDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] aiGain 音频输入增益,范围[0 ~ 31],对应[-18dB ~ 28.5dB],步长1.5dB.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention aiGain的范围为[0 ~ 31],如果输入的值小于0,则aiGain的值将会\n
 * 被设置为0.如果值大于31,aiGain的值会被设置为10.
 *
 */
 int IMP_AI_SetGain(int audioDevId, int aiChn, int aiGain);

/**
 * @fn int IMP_AI_GetGain(int audioDevId, int aiChn, int *aiGain)
 *
 * 获取AI增益值.
 *
 * @param[in] audioDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] aiGain 音频输入增益.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_AI_GetGain(int audioDevId, int aiChn, int *aiGain);

/**
 * @fn int IMP_AI_SetAlcGain(int audioDevId, int aiChn, int aiPgaGain)
 *
 * 设置音频输入增益.
 *
 * @param[in] audioDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] aiPgaGain 音频输入增益,范围[0 ~ 7],对应[-13.5dB, +28.5dB].步长6dB.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention aiPgaGain的范围为[0 ~ 7],如果输入的值小于0,则aiPgaGain的值将会\n
 * 被设置为0.如果值大于7,aiGain的值会被设置为7.
 *
 */
 int IMP_AI_SetAlcGain(int audioDevId, int aiChn, int aiPgaGain);

/**
 * @fn int IMP_AI_GetAlcGain(int audioDevId, int aiChn, int *aiPgaGain)
 *
 * 获取AIPga增益值.
 *
 * @param[in] audioDevId 音频输入设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] aiPgaGain 音频输入增益.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_AI_GetAlcGain(int audioDevId, int aiChn, int *aiPgaGain);

/**
 * @fn int IMP_AO_SetGain(int audioDevId, int aoChn, int aoGain)
 *
 * 设置音频输出增益.
 *
 * @param[in] audioDevId 音频输出设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[out] aoGain 音频输出增益,范围[0 ~ 0x1f],对应[-39dB ~ 6dB],步长1.5dB.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention aoGain的范围为[0 ~ 31],如果输入的值小于0,则aoGain的值将会\n
 * 被设置为0.如果值大于31,aoGain的值会被设置为31.
 *
 */
 int IMP_AO_SetGain(int audioDevId, int aoChn, int aoGain);

/**
 * @fn int IMP_AO_GetGain(int audioDevId, int aoChn, int *aoGain)
 *
 * 获取音频输出增益.
 *
 * @param[in] audioDevId 音频输出设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[out] aoGain 音频输出增益.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
 int IMP_AO_GetGain(int audioDevId, int aoChn, int *aoGain);

/**
 * @fn int IMP_AO_Soft_Mute(int audioDevId, int aoChn)
 *
 * 输出软静音控制.
 *
 * @param[in] audioDevId 音频输出设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该接口不会立马静音，会从正常放音状态缓慢将音量降低，直到真正静音.
 *
 * @attention 无.
 */
 int IMP_AO_Soft_Mute(int audioDevId, int aoChn);

/**
 * @fn int IMP_AO_Soft_UNMute(int audioDevId, int aoChn)
 *
 * 输出软撤销静音控制.
 *
 * @param[in] audioDevId 音频输出设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该接口不会立马恢复当前音量，会从静音状态缓慢将音量增加，直到音量达到设置好的音量.
 *
 * @attention 无.
 */
 int IMP_AO_Soft_UNMute(int audioDevId, int aoChn);

/**
 * @fn int IMP_AI_GetFrameAndRef(int audioDevId, int aiChn, IMPAudioFrame *frm, IMPAudioFrame *ref, IMPBlock block)
 *
 * 获取音频帧和输出参考帧.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[out] frm 音频帧结构体指针.
 * @param[out] ref 参考帧结构体指针.
 * @param[in] block 阻塞/非阻塞标识.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 示例代码
 * @code
 * IMPAudioFrame frm;
 * IMPAudioFrame ref;
 * // 获取音频帧和输出参考帧
 * ret = IMP_AI_GetFrameAndRef(devID, chnID, &frm, &ref, BLOCK);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
 *		return ret;
 * }
 *
 * fwrite(frm.virAddr, 1, frm.len, record_file); // 使用音频帧数据
 * fwrite(ref.virAddr, 1, ref.len, ref_file); // 使用音频参考帧
 *
 * // 释放音频帧
 * ret = IMP_AI_ReleaseFrame(devID, chnID, &frm);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio release frame data error\n");
 *		return ret;
 * }
 * @endcode
 *
 * @attention 无.
 */
 int IMP_AI_GetFrameAndRef(int audioDevId, int aiChn, IMPAudioFrame *frm, IMPAudioFrame *ref, IMPBlock block);

/**
 * @fn int IMP_AI_EnableAecRefFrame(int audioDevId, int aiChn, int audioAoDevId, int aoChn)
 *
 * 打开获取参考帧.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[in] audioAoDevId 音频输出设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用IMP_AI_GetFrameAndRef之前调用此接口.
 * @attention 无.
 */
 int IMP_AI_EnableAecRefFrame(int audioDevId, int aiChn, int audioAoDevId, int aoChn);

/**
 * @fn int IMP_AI_DisableAecRefFrame(int audioDevId, int aiChn, int audioAoDevId, int aoChn)
 *
 * 关闭获取参考帧.
 *
 * @param[in] audioDevId 音频设备号.
 * @param[in] aiChn 音频输入通道号.
 * @param[in] audioAoDevId 音频输出设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 * @attention 无.
 */
 int IMP_AI_DisableAecRefFrame(int audioDevId, int aiChn, int audioAoDevId, int aoChn);

/**
 * @fn int IMP_AO_CacheSwitch(int audioDevId, int aoChn, int cache_en)
 * 关闭音频播放缓存机制
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 * @param[in] cache_en 缓存机制开启开关
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 * @attention 无.
 */
 int IMP_AO_CacheSwitch(int audioDevId, int aoChn, int cache_en);

/**
 * @fn int IMP_AO_FlushChnBuf(int audioDevId, int aoChn);
 * 等待最后一段音频数据播完;
 * @param[in] audioDevId 音频设备号.
 * @param[in] aoChn 音频输出通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 * @attention 无.
 */
 int IMP_AO_FlushChnBuf(int audioDevId, int aoChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_AUDIO_H__ */
