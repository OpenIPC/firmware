/******************************************************************************

  Copyright (C), 2016-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : acodec.h
  Version         : Initial Draft
  Author          : Hisilicon multimedia software group
  Created        : 2016/6/15
  Last Modified :
  Description    :
  Function List  :
******************************************************************************/

#ifndef _ACODEC_H_
#define _ACODEC_H_

#define IOC_TYPE_ACODEC 'A'

typedef enum hiACODEC_FS_E {
    ACODEC_FS_8000  =   0x1,
    ACODEC_FS_11025 =   0x2,
    ACODEC_FS_12000 =   0x3,
    ACODEC_FS_16000 =   0x4,
    ACODEC_FS_22050 =   0x5,
    ACODEC_FS_24000 =   0x6,
    ACODEC_FS_32000 =   0x7,
    ACODEC_FS_44100 =   0x8,
    ACODEC_FS_48000 =   0x9,
    ACODEC_FS_64000 =   0xa,
    ACODEC_FS_96000 =   0xb,

    ACODEC_FS_BUTT = 0xc,
} ACODEC_FS_E;

typedef enum hiACODEC_MIXER_E {
    ACODEC_MIXER_IN0   = 0x0,   /* 16EV200/16EV300/18EV300 Unsupport IN0. */
    ACODEC_MIXER_IN1   = 0x1,
    ACODEC_MIXER_IN_D  = 0x2,

    ACODEC_MIXER_BUTT,
} ACODEC_MIXER_E;

typedef struct {
    /* volume control, 0x00~0x7e, 0x7F:mute */
    unsigned int vol_ctrl;
    /* adc/dac mute control, 1:mute, 0:unmute */
    unsigned int vol_ctrl_mute;
} ACODEC_VOL_CTRL;

typedef enum hiACODEC_IOCTL_E {
    IOC_NR_SOFT_RESET_CTRL = 0x0,

    IOC_NR_SET_INPUT_VOL,
    IOC_NR_SET_OUTPUT_VOL,
    IOC_NR_GET_INPUT_VOL,
    IOC_NR_GET_OUTPUT_VOL,

    IOC_NR_SET_I2S1_FS,
    IOC_NR_SET_MIXER_MIC,
    IOC_NR_SEL_DAC_CLK,
    IOC_NR_SEL_ADC_CLK,
    IOC_NR_SEL_ANA_MCLK,
    IOC_NR_SET_GAIN_MICL,
    IOC_NR_SET_GAIN_MICR,
    IOC_NR_SET_DACL_VOL,
    IOC_NR_SET_DACR_VOL,
    IOC_NR_SET_ADCL_VOL,
    IOC_NR_SET_ADCR_VOL,
    IOC_NR_SET_MICL_MUTE,
    IOC_NR_SET_MICR_MUTE,
    IOC_NR_SET_DACL_MUTE,
    IOC_NR_SET_DACR_MUTE,
    IOC_NR_BOOSTL_ENABLE,
    IOC_NR_BOOSTR_ENABLE,

    IOC_NR_GET_GAIN_MICL,
    IOC_NR_GET_GAIN_MICR,
    IOC_NR_GET_DACL_VOL,
    IOC_NR_GET_DACR_VOL,
    IOC_NR_GET_ADCL_VOL,
    IOC_NR_GET_ADCR_VOL,

    IOC_NR_SET_PD_DACL,
    IOC_NR_SET_PD_DACR,
    IOC_NR_SET_PD_ADCL,
    IOC_NR_SET_PD_ADCR,
    IOC_NR_SET_PD_LINEINL,
    IOC_NR_SET_PD_LINEINR,

    IOC_NR_SET_DAC_DE_EMPHASIS,
    IOC_NR_SET_ADC_HP_FILTER,

    IOC_NR_SET_I2S1_DATAWIDTH,
} ACODEC_IOCTL_E;

/* reset the audio code to the default config */
#define ACODEC_SOFT_RESET_CTRL \
    _IO(IOC_TYPE_ACODEC, IOC_NR_SOFT_RESET_CTRL)
/* ACODEC_FS_E */
#define ACODEC_SET_I2S1_FS \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_I2S1_FS, unsigned int)

/* select the micpga's input, micin linein, or differential input(ACODEC_MIXER_E) */
#define ACODEC_SET_MIXER_MIC \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MIXER_MIC, unsigned int)
/* analog part input volume control(left channel 0~0x1f) */
#define ACODEC_SET_GAIN_MICL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_GAIN_MICL, unsigned int)
/* analog part input volume control(right channel 0~0x1f) */
#define ACODEC_SET_GAIN_MICR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_GAIN_MICR, unsigned int)
/* Output volume control(left channel) ACODEC_VOL_CTRL */
#define ACODEC_SET_DACL_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_VOL, ACODEC_VOL_CTRL)
/* Output volume control(right channel) ACODEC_VOL_CTRL */
#define ACODEC_SET_DACR_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_VOL, ACODEC_VOL_CTRL)
/* Input volume control(left channel) ACODEC_VOL_CTRL */
#define ACODEC_SET_ADCL_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADCL_VOL, ACODEC_VOL_CTRL)
/* Input volume control(right channel) ACODEC_VOL_CTRL */
#define ACODEC_SET_ADCR_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADCR_VOL, ACODEC_VOL_CTRL)
/* Input mute control(left channel), 1:mute, 0:unmute */
#define ACODEC_SET_MICL_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MICL_MUTE, unsigned int)
/* Input mute control(right channel), 1:mute, 0:unmute */
#define ACODEC_SET_MICR_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MICR_MUTE, unsigned int)
/* Output mute control(left channel), 1:mute, 0:unmute */
#define ACODEC_SET_DACL_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_MUTE, unsigned int)
/* Output mute control(right channel), 1:mute, 0:unmute */
#define ACODEC_SET_DACR_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_MUTE, unsigned int)
/* Audio AD BOOST Control, 1:on, 0:off */
#define ACODEC_ENABLE_BOOSTL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_BOOSTL_ENABLE, unsigned int)
#define ACODEC_ENABLE_BOOSTR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_BOOSTR_ENABLE, unsigned int)

#define ACODEC_GET_GAIN_MICL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_GAIN_MICL, unsigned int)
#define ACODEC_GET_GAIN_MICR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_GAIN_MICR, unsigned int)
#define ACODEC_GET_DACL_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACL_VOL, ACODEC_VOL_CTRL)
#define ACODEC_GET_DACR_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACR_VOL, ACODEC_VOL_CTRL)
#define ACODEC_GET_ADCL_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_ADCL_VOL, ACODEC_VOL_CTRL)
#define ACODEC_GET_ADCR_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_ADCR_VOL, ACODEC_VOL_CTRL)

/* set adcl power, 0: power up, 1: power down */
#define  ACODEC_SET_PD_DACL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACL, unsigned int)
/* set adcr power, 0: power up, 1: power down */
#define  ACODEC_SET_PD_DACR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACR, unsigned int)
/* set adcl power, 0: power up, 1: power down */
#define  ACODEC_SET_PD_ADCL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_ADCL, unsigned int)
/* set adcr power, 0: power up, 1: power down */
#define  ACODEC_SET_PD_ADCR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_ADCR, unsigned int)
/* set adcl power, 0: power up, 1: power down */
#define  ACODEC_SET_PD_LINEINL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_LINEINL, unsigned int)
/* set adcr power, 0: power up, 1: power down */
#define  ACODEC_SET_PD_LINEINR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_LINEINR, unsigned int)

/* Don't need to set, the driver will set a default value */
/* clock of dac and adc is reverse or obverse */
#define ACODEC_SEL_DAC_CLK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_DAC_CLK, unsigned int)
#define ACODEC_SEL_ADC_CLK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_ADC_CLK, unsigned int)
/* clock of analog part and digital part is reverse or obverse */
#define ACODEC_SEL_ANA_MCLK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_ANA_MCLK, unsigned int)
#define ACODEC_SET_DAC_DE_EMPHASIS \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DAC_DE_EMPHASIS, unsigned int)
#define ACODEC_SET_ADC_HP_FILTER \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADC_HP_FILTER, unsigned int)

#define ACODEC_SET_INPUT_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_INPUT_VOL, unsigned int)
#define ACODEC_SET_OUTPUT_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_OUTPUT_VOL, unsigned int)
#define ACODEC_GET_INPUT_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_INPUT_VOL, unsigned int)
#define ACODEC_GET_OUTPUT_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_OUTPUT_VOL, unsigned int)

/* Reserved ioctl cmd */
#define ACODEC_SET_I2S1_DATAWIDTH \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_I2S1_DATAWIDTH, unsigned int)

#endif /* End of #ifndef _ACODEC_H_ */
